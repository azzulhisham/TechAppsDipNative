//=============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyWrite
// Purpose:  Write SEGY seismic data files
//==============================================================================

#include <algorithm>
#include "SegyWrite.h"
#include "PspaException.h"
#include <stdio.h>
using std::min;

#ifdef UNICODE 

#define _tcslen     wcslen
#define _tcscpy     wcscpy
#define _tcscpy_s   wcscpy_s
#define _tcsncpy    wcsncpy
#define _tcsncpy_s  wcsncpy_s
#define _tcscat     wcscat
#define _tcscat_s   wcscat_s
#define _tcsupr     wcsupr
#define _tcsupr_s   wcsupr_s
#define _tcslwr     wcslwr
#define _tcslwr_s   wcslwr_s

#define _stprintf_s swprintf_s
#define _stprintf   swprintf
#define _tprintf    wprintf

#define _vstprintf_s    vswprintf_s
#define _vstprintf      vswprintf

#define _tscanf     wscanf


#define TCHAR wchar_t

#else

#define _tcslen     strlen
#define _tcscpy     strcpy
#define _tcscpy_s   strcpy_s
#define _tcsncpy    strncpy
#define _tcsncpy_s  strncpy_s
#define _tcscat     strcat
#define _tcscat_s   strcat_s
#define _tcsupr     strupr
#define _tcsupr_s   strupr_s
#define _tcslwr     strlwr
#define _tcslwr_s   strlwr_s

#define _stprintf_s sprintf_s
#define _stprintf   sprintf
#define _tprintf    printf

#define _vstprintf_s    vsprintf_s
#define _vstprintf      vsprintf

#define _tscanf     scanf

#define TCHAR char
#endif

namespace PagosaGeo {

//==============================================================================
//------------------------------------------------------------------------------
SegyWrite::SegyWrite(const string& filePathName, const SegyData& sgy, eSegyRevision rev) 
	: SegyData(filePathName, sgy, rev)
{
   setIoRange(sgy.ioRange());  // ensure full range = io range; allocate _trace array
	if (fileHeader()._orig_samples_per_trace == 0) fileHeader()._orig_samples_per_trace = fileHeader()._samples_per_trace;
	if (fileHeader()._orig_sample_int == 0) fileHeader()._orig_sample_int = fileHeader()._sample_int;

	try {
		open();	// Open seismic data file for writing
	}
	catch (PspaException& ex) {
		ex.addSource("SegyWrite::SegyWrite(const string& filePathName, const SegyData& sgy, eSegyRevision rev)");
		throw ex;
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
SegyWrite::SegyWrite(const string& filePathName, const DataRange& rng) 
	: SegyData(filePathName, SEGY_REV1, false)
{
	setIoRange(rng);  // ensure full range = io range; allocate _trace array
	fileHeader().setFromRange(rng);
	setDefaultTextHeader();
	makeSeisTraceBuffer();

	try {
		open();	// Open seismic data file for writing
	}
	catch (PspaException& ex) {
		ex.addSource("SegyWrite::SegyWrite(const string& filePathName, const SegyData& sgy, eSegyRevision rev)");
		throw ex;
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
SegyWrite& SegyWrite::operator=(const SegyWrite& sgy)
{
	if (this == &sgy) return *this;  // prevent self-assignment
	SegyData::operator=(sgy);        
	return *this;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyWrite::operator==(const SegyWrite& sgy) const
{
	if (SegyData::operator!=(sgy)) return false;
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyWrite::setIoRange(const DataRange& rng)
{
   _ioRng = rng;
   _fileRng = _ioRng;
	_fileRng.setDescription("Output File");
	_ioRng.setDescription("Output IO");
	if (fileHeader()._orig_samples_per_trace == 0) 
		fileHeader()._orig_samples_per_trace = fileHeader()._samples_per_trace;
	fileHeader()._samples_per_trace = (int)_ioRng.numSmps();
	fileHeader()._sample_int = (int)(_ioRng.dSample()*1000000.0);
	makeSeisTraceBuffer();
	return true;	
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Write data from a buffer; numSmps data values each of sampSize bytes
// Notes:  Increments file ptr by sampSize*numSmps bytes
//------------------------------------------------------------------------------
void SegyWrite::fileWrite(void* buffer, size_t sampSize, size_t numSmps)
{
	size_t numWrite = fwrite(buffer, sampSize, numSmps, _filePtr);
   try {
      if (numWrite != numSmps) {
         string errSrc = "SegyWrite::fileWrite";
			string errMsg = "Problem writing to file ";
         errMsg += pathName();
         char temp[100];
         snprintf(temp,99, "\nSample size = %d bytes,  expected number samples = %d,  actual number samples = %d", 
            sampSize, numSmps, numWrite);
         errMsg += temp;
			PspaException ex(errMsg, errSrc);
			throw ex;
		}
	}
	catch (PspaException& ex) {
		throw ex; 
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Write both the SEGY text and the file headers (assume only 1 text header!)
// Return:  false if text and file headers could not be written,
//          true if both were successfully written out.
//------------------------------------------------------------------------------
void SegyWrite::writeFileHeader()
{
	SegyUtil::ASCII_to_EBCDIC((int)textHeaderLength(), _textHdr);  // convert text header to EBCDIC

	try {
      fileWrite(_textHdr, (size_t)textHeaderLength());
   } 
   catch (PspaException& ex) {
      ex.addSource("SegyWrite::writeFileHeader()");
	   ex.addMessage("Failed to write SEGY text header");
      throw ex;
	}
	
   incrementFileSize(textHeaderLength());      // keep track of output file size
   SegyUtil::EBCDIC_to_ASCII((int)textHeaderLength(), _textHdr);  // convert text header back to ASCII

	swapBytesFileHeader();  // convert to format for SEGY write
   try {
      fileWrite(&fileHeader(), (size_t)fileHeaderLength());
   } 
   catch (PspaException& ex) {
      ex.addSource("SegyWrite::writeFileHeader()");
	   ex.addMessage("Failed to write SEGY binary file header");
      throw ex;
	}
  
   incrementFileSize(fileHeaderLength());  // keep track of output file size
   swapBytesFileHeader();		             // convert back to standard format
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Return:  true for live trace, false for dead trace
// Notes:  Sets output header from the segy header of the input SeisTrace.
//------------------------------------------------------------------------------
void SegyWrite::writeTrace(const SeisTrace& trc)
{	
	// Output trace header
	_curTrcHdr = trc.getHeaders();
	if (trc.isLive()) _curTrcHdr._trace_id_code = 1;			// insurance
	else _curTrcHdr._trace_id_code = 2; 
	swapBytesTraceHeader(_curTrcHdr);
	try {
      fileWrite(&_curTrcHdr, (size_t)traceHeaderLength());	// Write header
   } 
   catch (PspaException& ex) {
      ex.addSource("SegyWrite::writeTrace(const SeisTrace& trc)");
	   ex.addMessage("Failed to write SEGY trace header");
      throw ex;
	}

	// Output trace values
	int nz = trc.numSmps();
	trc.getValues((float*)_trace, nz);
	// Convert trace values to required output format (IBM or IEEE floats only)
	eDataFormat outputFormat = dataFormat();
	switch (outputFormat) {
		case IBMFLT32:
			SegyUtil::intelFloatToIBMfloat((float*)_trace, 0, nz-1);
			break;
			
		case IEEEFLT32:
			SegyUtil::swapBytes((void*)_trace, 0, nz-1);
			break;
			
		default:
			SegyUtil::intelFloatToIBMfloat((float*)_trace, 0, nz-1);
	}

	try {
      fileWrite((void*)_trace, numBytesPerValue(), nz);	// Write trace values
   } 
   catch (PspaException& ex) {
      ex.addSource("SegyWrite::writeTrace(const SeisTrace& trc)");
	   ex.addMessage("Failed to write seismic trace values");
      char temp[80];
		snprintf(temp,79,"Error at inline %d, crossline %d, sequence number %d", trc.inln(), trc.crln(), trc.seqNum());
		ex.addMessage(temp);
      throw ex;
	}

	incrementFileSize(totalNumBytesPerTrace());  // keep track of output file size
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Notes:  linNum refers to the line number in the text header (range 1 -- 40)
//------------------------------------------------------------------------------
bool SegyWrite::writeLineTextHeader(const string& lin, int linNum)
{
	if (linNum < 1 || linNum > 40) return false;
	char temp[5];
	snprintf(temp,4, "C%2d ", linNum);
	string fullLin = temp + lin;
	int startIndex = (linNum - 1)*80;
	int numChar = min((int)fullLin.length(), 80);
	for (int i=0; i<numChar; i++) _textHdr[startIndex + i] = fullLin[i];
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Return the default SEGY text header (no information)
//------------------------------------------------------------------------------
void SegyWrite::setDefaultTextHeader()
{
unsigned char defTextHdr[] = {
"C 1 PROCESSED WITH:  Poststack Seismic Data Analyzer, Version 7/15              "
"C 2                                                                             "
"C 3                                                                             "
"C 4                                                                             "
"C 5                                                                             "
"C 6                                                                             "
"C 7                                                                             "
"C 8                                                                             "
"C 9                                                                             "
"C10                                                                             "
"C11                                                                             "
"C12                                                                             "
"C13                                                                             "
"C14                                                                             "
"C15                                                                             "
"C16                                                                             "
"C17                                                                             "
"C18                                                                             "
"C19                                                                             "
"C20                                                                             "
"C21 PROCESSING:                                                                 "
"C22                                                                             "
"C23                                                                             "
"C24                                                                             "
"C25                                                                             "
"C26                                                                             "
"C27                                                                             "
"C28                                                                             "
"C29                                                                             "
"C30                                                                             "
"C31                                                                             "
"C32                                                                             "
"C33                                                                             "
"C34                                                                             "
"C35                                                                             "
"C36 Pagosa Geophysical Research, LLC                                            "
"C37 Pagosa Springs, Colorado 81147                                              "
"C38                                                                             "
"C39 SEG Y REV1                                                                  "
"C40 END TEXTUAL HEADER                                                          " };

for (int i=0; i<3200; i++) _textHdr[i] = defTextHdr[i];
}
//------------------------------------------------------------------------------
//==============================================================================

}  // end of namespace PagosaGeo
