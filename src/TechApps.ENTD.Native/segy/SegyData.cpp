//=============================================================================
// PSPA:  Poststack Seismic Processing and Analysis (� Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyData
// Purpose:  Read or write segy data files
// References:  Pullan (GEO 1990, 1260-1271); Bennett (GEO 1990, 1272-1284)
//==============================================================================
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>         // for reading trace header map as ifstream
#include <sstream>
#include <math.h>          // needed for pow()
#include "PspaException.h"
#include "PspaMath.h"
#include "SegyData.h"
#include "SegyUtil.h"
#include <inttypes.h>
#include <string>
//typedef int64_t int64_t;
typedef int errno_t;


namespace PagosaGeo {

//==============================================================================
//------------------------------------------------------------------------------
SegyData::SegyData(const string& filePathName, eSegyRevision rev, bool isRead) : _trace(NULL),
	_regularVolume(true), _ioFullRange(true), _isRead(isRead)
{
   _filePathName = filePathName;
	_revision = rev;
   initialize();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Copy Constructor, valid only for writing. Does not copy fileptr.
//------------------------------------------------------------------------------
SegyData::SegyData(const string& filePathName, const SegyData& sgy, eSegyRevision rev) :
	_trace(NULL), _regularVolume(true), _ioFullRange(true), _isRead(false)
{
	try {
		if (&sgy == NULL) {
			PspaException ex("No existing Segy file!", "SegyData::SegyData");
			throw ex;
		}
	} catch (PspaException& ex) {
		throw ex;
		return;
	}

	*this = sgy;			// equates revisions, sets IO range
   _filePathName = filePathName;
	_revision = rev;		// reset the output revision to whatever is chosen
   _filePtr = NULL;
	_fileSize = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
SegyData::~SegyData()
{
	delete [] _trace;
   close();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Return:  a SegyData&; this follows the rule #15 of Scott Meyers, but
//   contravenes rule #2.2.1 on page 32 of Murray.
// Notes:  Checks for assignment to self (see Meyers, p. 75).
//   Two SegyDatas are equal if:
//   (1)  all parameters are equal
//   (2)  sizes are equal; memory not the same (unique to each)
//   (3)  data values are equal
// Does not equate file pointers, names, or whether is read or write.
//------------------------------------------------------------------------------
SegyData& SegyData::operator=(const SegyData& sgy)
{
	if (this == &sgy) return *this;  // prevent self-assignment
	_totalNumBytesPerTrc = sgy._totalNumBytesPerTrc;
	_dataFormat = sgy._dataFormat;
	_regularVolume = sgy._regularVolume;
	_revision = sgy._revision;
	setTextHeader(sgy._textHdr);     // copy 3200 byte ASCII or EBCIDC text header
	_fileHdr = sgy._fileHdr;
	_fileRng = sgy._fileRng;			// must be set before ioRng
   _ioRng = sgy._ioRng;					// remakes IO buffers dependent on ioRng
	_hdrLoc = sgy._hdrLoc;
	_ioFullRange = sgy._ioFullRange;
	return *this;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Note: Does not compare file headers
//------------------------------------------------------------------------------
bool SegyData::operator==(const SegyData& sgy) const
{
	if (_totalNumBytesPerTrc != sgy._totalNumBytesPerTrc) return false;
	if (_fileSize != sgy._fileSize) return false;
	if (_dataFormat != sgy._dataFormat) return false;
	if (_fileRng != sgy._fileRng) return false;
	if (_ioRng != sgy._ioRng) return false;
	if (_regularVolume != sgy._regularVolume) return false;
	if (_revision != sgy._revision) return false;
	if (_hdrLoc != sgy._hdrLoc) return false;
	if (_ioFullRange != sgy._ioFullRange) return false;

	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyData::setDataFormat(eDataFormat frmt)
{
	switch (frmt) {
	case IBMFLT32:
	case INT32:
	case INT16:
	case FIXPTGN32:
	case IEEEFLT32:
	case INT8:
		_dataFormat = frmt;
		return true;
		
	default:
		_dataFormat = IEEEFLT32;
		return false; 
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyData::setDomain(eDomain dom)
{
	switch (dom) {
	case TIME_DOMAIN:
	case DEPTH_DOMAIN:
		_ioRng.setDomain(dom);
		_fileRng.setDomain(dom);
		return true;
		
	default:
		return false;  // do nothing at present
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SegyData::setSegyRev0()
{ 
	_revision = SEGY_REV0; 
	_hdrLoc.setRev0(); 
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SegyData::setSegyRev1() 
{
	_revision = SEGY_REV1;
	_hdrLoc.setRev1(); 
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyData::setTotalNumBytesPerTrace(int64_t numBytes)
{
	if (numBytes > 0) { 
		_totalNumBytesPerTrc = numBytes; 
		return true; 
	}
	else return false;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyData::open()
{
	if (fileIsOpen()) return false;  // file already opened

	string temp = pathName();
	const char* filePathName = temp.c_str();
	//errno_t err;
	if (isRead()) _filePtr = fopen( filePathName, "rb");//err = fopen(&_filePtr, filePathName, "rb");
	else _filePtr =  fopen(filePathName, "wb");//err = fopen(&_filePtr, filePathName, "wb");

	try {
		if (fileNotOpen()) {
			string errSrc = "bool SegyData::open()";
			string errMsg = "Failed to open seismic data file:  \"" + pathName() + "\"";
			errMsg += "\n   The file may be locked by another process, or it may not exist";
			PspaException ex(errMsg, errSrc);
			throw ex;
		}
	} catch (PspaException& ex) {
		throw ex;
		return false;
	}

	_fileRng.setDataName(pathName());
	_ioRng.setDataName(pathName());

	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  close the file if open, throw exception if an open file failed to
//   close properly.
// Return:  true if closed, false if NULL file ptr or problem closing
//------------------------------------------------------------------------------
bool SegyData::close()
{
	if (fileNotOpen()) return false;  
	
	try {
		if (fclose(_filePtr) != NULL) {  // automatically flushes the buffer
			string errMsg = "Failed to close seismic data file:  " + _filePathName;
			string src = "DataIO::DataIO(string&, eOpenMode)";
			throw PspaException(errMsg, src);
		}
	}
	catch (PspaException& ex) {
		throw ex;
		return false;
	}
	 
	_filePtr = NULL;                // insurance
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
float SegyData::percentDataRw() const
{
	int64_t fullNumPts = _fileRng.numPts();
	int64_t rwNumPts = _ioRng.numPts();
	return 100.0f*((float)rwNumPts/((float)fullNumPts + VSMALL));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Set variables that must be defined during object construction
//------------------------------------------------------------------------------
bool SegyData::initialize()
{
   // Initialize parameters to NULL or to reasonable defaults
	_filePtr = NULL;
	_fileSize = 0;
	_dataFormat = PagosaGeo::IEEEFLT32;
	_totalNumBytesPerTrc = 0;
	_regularVolume = true;
	_hdrLoc.cdpX = 181;
	_hdrLoc.cdpY = 185;
	_hdrLoc.inln = 189;
	_hdrLoc.crln = 193;
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose: set this text header to input EBCDIC or ASCII textual file header
// Return: true if everything OK, false otherwise
//------------------------------------------------------------------------------
bool SegyData::setTextHeader(const unsigned char* txtHdr)
{
	for (int n=0; n<TEXTHDRLEN; n++) _textHdr[n] = txtHdr[n]; 
	return true;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Methods to set file pointer and current location
//------------------------------------------------------------------------------
// Purpose:  Set current location to next IO location. Resets file ptr if
//		a regular volume and skipping traces in read/write
// Return:  true if next location is in range, false if beyond boundaries of
//   the read-write range, in which case the current position is not changed
//------------------------------------------------------------------------------
bool SegyData::setFilePtrToNextLocation()
{
	if (!isRegularVolume()) return false;
	bool isInRange = incrementCurrentLocation();
	if (ioFullRange()) return isInRange;
	else if (isInRange) setFilePtrToDataPosition(_curTrcLoc);
	return isInRange;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  translate the given trace sequence number into a byte location and
//   move the file ptr to that location. First trace has sequence number = 1
// Return:  true if successful, false otherwise
//------------------------------------------------------------------------------
bool SegyData::setFilePtrToTrace(int64_t trcSeqNum)
{
	int64_t numBytes = textHeaderLength() + fileHeaderLength() + (trcSeqNum - 1)*totalNumBytesPerTrace();
	if (numBytes >= fileSize()) return false;
	if (fseek(_filePtr, numBytes, SEEK_SET)) return false;	// fseek returns 0 if successful, non-zero otherwise
	else return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyData::setFilePtrFromEnd(int64_t numBytes)
{
	if (numBytes > _fileSize) return false;
	fseek(_filePtr, -numBytes, SEEK_END);
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Puts file ptr to beginning of first trace in file to read or write
// Note:  sets current location appropriately -- this assumes that _ioRng
//   has already been set
//------------------------------------------------------------------------------
bool SegyData::setFilePtrToFirstIOTrace()
{
	_curTrcLoc = _ioRng.getFirstLocation();
	return setFilePtrToTrace(_fileRng.traceSequenceNumber(_curTrcLoc));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Resets file ptr for 3-D data arranged in a regular volume to the
//		given inline/crossline position, which becomes the current location. 
//		For an irregular volume, simply resets the current location, whether or
//		not it corresponds to an actual trace in the file.
//------------------------------------------------------------------------------
bool SegyData::setFilePtrToCurrentLocation(double inln, double crln)
{
	if (!setCurrentLocation(inln, crln)) return false;
	if (!isRegularVolume()) return true;
	else return setFilePtrToTrace(_fileRng.traceSequenceNumber(inln, crln));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Resets current location, does not reset file ptr
//------------------------------------------------------------------------------
bool SegyData::setCurrentLocation(double inln, double crln)
{
	if (_fileRng.outOfRange(inln, crln)) return false;
   _curTrcLoc.inln = inln;
	_curTrcLoc.crln = crln;
	_curTrcLoc.time = _ioRng.startTime();
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  find file size, store in _fileSize data member, rewind file ptr
//------------------------------------------------------------------------------
int64_t SegyData::findFileSize()
{
	fseek(_filePtr, 0L, SEEK_END);
	_fileSize = ftell(_filePtr);
	rewind(_filePtr);
   return _fileSize;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Utility methods
//------------------------------------------------------------------------------
// Purpose:  Makes a 1D trace buffer needed for reading or writing
// Note:  Resets _totalNumBytesPerTrc
// Return:  # of bytes in the trace buffer; -1 if bad allocation
//------------------------------------------------------------------------------
int64_t SegyData::makeSeisTraceBuffer()
{
	delete [] _trace;
	int numDataBytes = (int)(numBytesPerValue()*_fileRng.numSmps());
	setTotalNumBytesPerTrace(numDataBytes + traceHeaderLength());
	if (numDataBytes == 0) return 0;
   try {
	   _trace = new char[numDataBytes];
   }
   catch (std::bad_alloc& ba) {
      return -1;
   }
	return numDataBytes;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  swaps the bytes for the entire 400 byte file header. Assumes only
//   the first 3 entries are 4 byte and all the rest are 2 byte. Needed in going
//   from unix to linux and vice versa.
//------------------------------------------------------------------------------
void SegyData::swapBytesFileHeader()
{
	SegyFileHeader* filHdrStrc = &fileHeader();
	int *ptr = (int*)filHdrStrc;
	for (int i=0; i<3; i++) SegyUtil::swap4Bytes(ptr[i]);
	short int *sptr = (short int*)filHdrStrc + 6;
	for (int ii=0; ii<194; ii++) SegyUtil::swap2Bytes(sptr[ii]);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose  swaps the bytes for all variables in trace header
//    This method is needed in going from unix to linux or the other way.
// Notes: Bytes 201->240 are not defined. Paradigm uses bytes 201-204 
//  to store inline.
//------------------------------------------------------------------------------
void SegyData::swapBytesTraceHeader(SegyTraceHeader& trcHdrStrc)
{
	int* lptr = (int*)&trcHdrStrc;  short int* sptr; 
	int i;  // header sequence number (not byte)
	for (i=1; i<=7; i++) SegyUtil::swap4Bytes(*lptr++); sptr = (short int*)lptr;   // (1-28)
	for (i=1; i<=4; i++) SegyUtil::swap2Bytes(*sptr++); lptr = (int*)sptr;         // (29-36)
	for (i=1; i<=8; i++) SegyUtil::swap4Bytes(*lptr++); sptr = (short int*)lptr;   // (37-68)
	for (i=1; i<=2; i++) SegyUtil::swap2Bytes(*sptr++); lptr = (int*)sptr;         // (69-72)
	for (i=1; i<=4; i++) SegyUtil::swap4Bytes(*lptr++); sptr = (short int*)lptr;   // (73-88)
	for (i=1; i<=46; i++) SegyUtil::swap2Bytes(*sptr++); lptr = (int*)sptr;        // (89-180)
	for (i=1; i<=5; i++) SegyUtil::swap4Bytes(*lptr++); sptr = (short int*)lptr;   // (181-200)
	for (i=1; i<=12; i++) SegyUtil::swap2Bytes(*sptr++); lptr = (int*)sptr;        // (201-224)
	SegyUtil::swap4Bytes(*lptr++); sptr = (short int*)lptr;                        // (225-228)
	for (i=1; i<=6; i++) SegyUtil::swap2Bytes(*sptr++);                            // (229-240)
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Warning:  Petrel sometimes uses byte 5 = _trace_number_in_reel
//------------------------------------------------------------------------------
int SegyData::inlineNumber(const SegyTraceHeader& trcHdr) const
{
	switch (revision()) {
	case SEGY_REV1:
//		return trcHdr._inline_num;

	case SEGY_REV0:
//		return trcHdr._orig_field_rec_num;

	case SEGY_CUSTOM:
		return ((int*)((char*)(&trcHdr) + _hdrLoc.inln - 1))[0]; 

	case SEGY_LANDMARK:
		return trcHdr._orig_field_rec_num;

	case SEGY_PARADIGM:
		return trcHdr._cdp_y;

	default:
		return trcHdr._inline_num;
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int SegyData::crosslineNumber(const SegyTraceHeader& trcHdr) const
{
	switch (revision()) {
	case SEGY_REV1:
//		return trcHdr._crossline_num;

	case SEGY_REV0:
//		return trcHdr._cdp_ensemble_num;

	case SEGY_CUSTOM:
		return ((int*)((char*)(&trcHdr) + _hdrLoc.crln - 1))[0];

	case SEGY_LANDMARK:
		return trcHdr._cdp_ensemble_num;

	case SEGY_PARADIGM:
		return trcHdr._cdp_x;

	default:
		return trcHdr._crossline_num;
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
float SegyData::xLocation(const SegyTraceHeader& trcHdr) const
{
	switch (revision()) {
	case SEGY_REV1:
//		return (float)(trcHdr._cdp_x);

	case SEGY_REV0:
//		return (float)(trcHdr._cdp_x);

	case SEGY_CUSTOM:
		return (float)(((int*)((char*)(&trcHdr) + _hdrLoc.cdpX - 1))[0]);

	case SEGY_LANDMARK:
		return (float)(trcHdr._source_x);

	case SEGY_PARADIGM:
		return (float)(trcHdr._source_x);

	default:
		return (float)(trcHdr._cdp_x);
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
float SegyData::yLocation(const SegyTraceHeader& trcHdr) const
{
	switch (revision()) {
	case SEGY_REV1:
//		return (float)(trcHdr._cdp_y);

	case SEGY_REV0:
//		return (float)(trcHdr._cdp_y);

	case SEGY_CUSTOM:
		return (float)(((int*)((char*)(&trcHdr) + _hdrLoc.cdpY - 1))[0]);

	case SEGY_LANDMARK:
		return (float)(trcHdr._source_y);

	case SEGY_PARADIGM:
		return (float)(trcHdr._source_y);

	default:
		return (float)(trcHdr._cdp_y);
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Information methods
//------------------------------------------------------------------------------
// Purpose:  returns a string of basic file information
//------------------------------------------------------------------------------
string SegyData::toString() const
{
   char temp[80];

	string info = "\nSEGY Seismic File Information\n   Opened for:  ";
	if (isRead()) info += "Read";
	else info += "Write";
	info += "\n   Name:  " + _filePathName;
	info += "\n   Size:  " + std::to_string(((long long)_fileSize));
	info += "\n   " + SegyUtil::dataFormatToString(_dataFormat);
	info += "\n   Data domain:  " + SegyUtil::dataDomainToString(_fileRng.domain());
	info += "\n   " + SegyUtil::distanceUnits(_fileRng.xyUnits());
	info += "\n   Apparent SEGY revision:  ";
	switch (revision()) {
   case SEGY_REV0:
      info += "Rev0";
      break;

   case SEGY_REV1:
      info += "Rev1";
		break;

	default:
      info += "Custom";
		info += "\n   Trace Header Locations:";
		snprintf(temp,79,"\n      cdpX = %d,   cdpY = %d", _hdrLoc.cdpX, _hdrLoc.cdpY);
		info += temp;
		snprintf(temp, 79, "\n      inln = %d,   crln = %d\n", _hdrLoc.inln, _hdrLoc.crln);
		info += temp;
	}

	info += "\n";
	info += _fileRng.toString();

	return info;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  returns text header as string, used for information in Qt widgets
//------------------------------------------------------------------------------
string SegyData::getTextHeaderString() const
{
	string hdr;
	for (int j=0; j<40; j++) {
		for (int i=0; i<80; i++) {
			hdr += _textHdr[i+j*80];
		}
		hdr += "\n";
	}
	return hdr;
}
//------------------------------------------------------------------------------
//==============================================================================

}  // end of namespace PagosaGeo
