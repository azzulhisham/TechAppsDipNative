//=============================================================================
// PSPA: Poststack Seismic Processing and Analysis (� Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyRead
// Purpose:  Read SEGY seismic data files
//==============================================================================

#include <algorithm>
#include <fstream>
#include "SegyRead.h"
#include "PspaException.h"
#include "PspaMath.h"
#include <inttypes.h>
#include <stdio.h>
//typedef int64_t int64_t;


using std::min;


namespace PagosaGeo {

//==============================================================================
// Standard Constructor
//------------------------------------------------------------------------------
SegyRead::SegyRead(const string& filePathName, eSegyRevision rev) 
	: SegyData(filePathName, rev, true), _restoreMissingTraces(false), 
	_isRestoringMissingTrace(false), _numTrcsInFile(0)
{
	openAndSetParameters();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Constructor for custom SEGY files
//------------------------------------------------------------------------------
SegyRead::SegyRead(const string& filePathName, const SegyHeaderLoc& hdrLoc) 
	: SegyData(filePathName, SEGY_CUSTOM), _restoreMissingTraces(false),
	_isRestoringMissingTrace(false), _numTrcsInFile(0)
{
	setCustomRev(hdrLoc);
	openAndSetParameters();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
SegyRead::SegyRead(const SegyRead& sgy) : SegyData(), _numTrcsInFile(0)
{
	*this = sgy;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Parent class does not equate file pointers, rwMode, or file names.
//------------------------------------------------------------------------------
SegyRead& SegyRead::operator=(const SegyRead& sgy)
{
	if (this == &sgy) return *this;  // prevent self-assignment
	SegyData::operator=(sgy);			// opens new file
	_restoreMissingTraces = sgy._restoreMissingTraces;
	_isRestoringMissingTrace = sgy._isRestoringMissingTrace;
	_numTrcsInFile = sgy._numTrcsInFile;
	_numTrcsRead = sgy._numTrcsRead;
	return *this;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyRead::operator==(const SegyRead& sgy) const
{
	if (SegyData::operator!=(sgy)) return false;
	if (_restoreMissingTraces != sgy._restoreMissingTraces) return false;
	if (_isRestoringMissingTrace != sgy._isRestoringMissingTrace) return false;
	if (_numTrcsInFile != sgy._numTrcsInFile) return false;
	if (_numTrcsRead != sgy._numTrcsRead) return false;
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Set the IO range. If ioRng = fullRng, then _readFullRange = true. Return true
//   if valid IO range, false otherwise
//------------------------------------------------------------------------------
bool SegyRead::openAndSetParameters()
{ 
	try {
		open();		// Open seismic data file for reading
	}
	catch (PspaException& ex) {
		ex.addSource("SegyRead::SegyRead()");
		throw ex;
	}

	_fileRng.setDescription("Input File");
	_ioRng.setDescription("Input IO");
	_numTrcsRead = 0;
	readSegyHeaderMapFile();
	setParametersFromFileHeader();
	return setParametersFromTraceHeaders();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Protected method to find the number of traces in the existing file;
//		requires that some info already be known about this file
//------------------------------------------------------------------------------
bool SegyRead::setNumTrcsInFile()
{
	if (totalNumBytesPerTrace()==0) {
		_numTrcsInFile = 0;
		return false;
	}
	else _numTrcsInFile = (fileSize() - textHeaderLength() - fileHeaderLength())/totalNumBytesPerTrace();
	return true; 
}
//------------------------------------------------------------------------------
// Set flag to indicate that traces have been found missing and will be restored.
//	This option is valid only for irregular surveys for which the user has decided
//	to restore the missing traces.
//------------------------------------------------------------------------------
bool SegyRead::setIsRestoringMissingTrace(bool set)
{
	if (!restoreMissingTraces()) return false;		// never true for regular survey
	_isRestoringMissingTrace = set;
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Set the IO range. If ioRng = fullRng, then _readFullRange = true. Return true
//   if valid IO range, false otherwise
//------------------------------------------------------------------------------
bool SegyRead::setIoRange(const DataRange& rng)
{ 
	_ioRng = rng;
	_ioRng *= _fileRng;	// ensure io read range is subset of full file range
	bool readFullRange = (_ioRng == _fileRng) ? true : false;
	setIoFullRange(readFullRange);
	return !_ioRng.isDefault();		// invalid if default
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Reads the 3200 byte Text Header and 400 byte File Header.  Fails
//    if file is not already open. Sets relevant parameters.  Sets file ptr to
//    start of first trace. Only called once.
// Return:  true if successful, false if failure
// Notes: The segy trace scalar is assumed constant for all traces
//------------------------------------------------------------------------------
bool SegyRead::setParametersFromFileHeader()
{
	if (fileNotOpen()) return false;

	findFileSize();  // rewinds
	try {
		read(_textHdr, textHeaderLength());		// Read textual file header (3200 bytes)
	}
	catch (PspaException& ex) {
      ex.addMessage("Cannot read SEGY text header");
		throw ex;
		return false;
   }
	SegyUtil::EBCDIC_to_ASCII((int)textHeaderLength(), _textHdr);  // TODO: check to see if already ASCII format
	
	// Read segy file header:  400 bytes
	try {
	   read(&fileHeader(), fileHeaderLength());
	}
	catch (PspaException& ex) {
      ex.addMessage("Cannot read SEGY file header");
		throw ex;
		return false;
	}
	
   swapBytesFileHeader();
	SegyFileHeader *filHdrStrc = &fileHeader();
	if (revision() == SEGY_UNDEFINED) {							// if not already defined
		if (filHdrStrc->_revision == 256) setSegyRev1();	// 256 --> SEGY_REV1
		else setSegyRev0();											// 0   --> SEGY_REV0
	}

   setDataFormat((eDataFormat)filHdrStrc->_data_sample_format);
   // Petrel never sets the fixed length flag -- assume it should be set (rarely needed)
	filHdrStrc->_fixed_length_flag = 1;

// TODO: Check for depth data
	int64_t numSmps = (int64_t)filHdrStrc->_samples_per_trace;  // convenience variable
	double dsmp = filHdrStrc->_sample_int/1000000.0;				// convert micro-seconds to seconds
	_fileRng.setTimeRange(0.0, numSmps, dsmp);

	// Sanity check -- fold must be at least 1 for real data
	if (filHdrStrc->_cdp_fold < 1) filHdrStrc->_cdp_fold = 1;
	_fileRng.setXyUnits((eXyUnits)(filHdrStrc->_distance_units));    // 1 = meters, 2 = feet
	_ioRng.setXyUnits(_fileRng.xyUnits());
	
	if (makeSeisTraceBuffer() <= 0) return false;
	return setNumTrcsInFile();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Notes: This method requires that the SEGY revision be set correctly. May be
//		called multiple times, if revision is incorrect at first and must be reset.
//------------------------------------------------------------------------------
bool SegyRead::setParametersFromTraceHeaders()
{
	if (fileNotOpen()) return false;

	// Read 3 trace headers in order to establish survey Dtrc, Dlin, & orientation
	// Read header of first trace, store in _curTrcHdr for possible use later
	readTraceHeader(_curTrcHdr, 1);
	// Get start time, convert milliseconds to seconds
	// Warning: this will not work properly if the file has variable start time
	float startTime = ((float)(_curTrcHdr._recording_delay))/1000.0f;  // set start time for first trace in seconds
	_fileRng.setStartTime(startTime);
	// get inline & crossline numbers, and X and Y coordinates of first trace in file
	double startInline = (double)(inlineNumber(_curTrcHdr));
	double startCrossline = (double)(crosslineNumber(_curTrcHdr));
	double startX = (double)(xLocation(_curTrcHdr));
	double startY = (double)(yLocation(_curTrcHdr));
	double coordScalar = (double)(_curTrcHdr._coordinate_scalar);
	if (_numTrcsInFile == 1) {	// set range and leave
		setIsRegularVolume(false);
		if (startInline == 0) _fileRng.setInlineRange(1.0, 1.0, 1);		// File might be a sweep and not have inline, crossline settings
		else _fileRng.setInlineRange(startInline, startInline, 1);
		if (startCrossline == 0) _fileRng.setCrosslineRange(1.0, 1.0, 1);
		else _fileRng.setCrosslineRange(startCrossline, startCrossline, 1);
		_fileRng.setDLine(1.0);
		_fileRng.setDTrace(1.0);
		setIoRange(_fileRng);	// default read-write range is full file range
		reset();  // reposition to start of first trace in file read-write range
//		FileUtil::warningMessage("SEGY Input Data", "File appears to have only 1 trace");
		return true;
	}
	

	// read header of second trace in file
	SegyTraceHeader trc2Hdr;
	readTraceHeader(trc2Hdr, 2);
	double trc2Inline = (double)(inlineNumber(trc2Hdr));
	double trc2Crossline = (double)(crosslineNumber(trc2Hdr));
	if (_numTrcsInFile == 2) {	// set range and leave
		setIsRegularVolume(false);
		if (startInline == 0 || trc2Inline == 0) _fileRng.setInlineRange(1.0, 2.0, 1); // File might be a sweep and not have inline, crossline settings
		else _fileRng.setInlineRange(startInline, trc2Inline, 1);
		if (startCrossline == 0 || trc2Crossline == 0) _fileRng.setCrosslineRange(1.0, 1.0, 1);
		else _fileRng.setCrosslineRange(startCrossline, trc2Crossline, 1);
		double trc2X = (double)(xLocation(trc2Hdr));
		double trc2Y = (double)(yLocation(trc2Hdr));
		_fileRng.setDLine(1.0);
		double dtrc = sqrt(pow(trc2X - startX, 2) + pow(trc2Y - startY, 2));
		if (coordScalar > 0) dtrc *= coordScalar;
		else dtrc /= coordScalar;
		_fileRng.setDTrace(dtrc);
		setIoRange(_fileRng);	// default read-write range is full file range
		reset();  // reposition to start of first trace in file (read-write range)
//		FileUtil::warningMessage("SEGY Input Data", "File appears to have only 2 traces");
		return true;
	}

	// read header of last trace in file (there are at least 3 traces in this file)
	setFilePtrFromEnd(totalNumBytesPerTrace());  // set file ptr to start of last trace
	SegyTraceHeader endTrcHdr;
	readTraceHeader(endTrcHdr);
	double endInline = (double)(inlineNumber(endTrcHdr));
	double endCrossline = (double)(crosslineNumber(endTrcHdr));		
	double endX = (double)(xLocation(endTrcHdr));
	double endY = (double)(yLocation(endTrcHdr));
	double incrInline = 1.0, incrCrossline = 1.0;
	int64_t numTrcs, numLins;
	if (trc2Inline == startInline) {
		_fileRng.setInlineOrganized();
		incrCrossline = trc2Crossline - startCrossline;
		numTrcs = (int64_t)((endCrossline - startCrossline)/incrCrossline) + 1;
		numLins = _numTrcsInFile/numTrcs;
		if (numLins > 1) incrInline = (endInline - startInline)/(numLins - 1);
	}
	else {
		_fileRng.setCrosslineOrganized();
		incrInline = trc2Inline - startInline;
		numTrcs = (int64_t)((endInline - startInline)/incrInline) + 1;
		numLins = _numTrcsInFile/numTrcs;
		if (numLins > 1) incrCrossline = (endCrossline - startCrossline)/(numLins - 1);
	}
	  
	// TODO: Generalize to handle non-integral inline, crossline increments
	_fileRng.setInlineRange(startInline, endInline, incrInline);
	_fileRng.setCrosslineRange(startCrossline, endCrossline, incrCrossline);

	// get x and y locations of first & last traces in file, and last trace on first line
	// mathematically I do not need lin1EndTrcHdr; I can compute all the required variables
	// with the 3 trace headers already read in. I use this new header to improve accuracy
	// with the dtrc, dlin estimation
	SegyTraceHeader lin1EndTrcHdr;
	readTraceHeader(lin1EndTrcHdr, numTrcs);
	double lin1EndX = (double)(xLocation(lin1EndTrcHdr));
	double lin1EndY = (double)(yLocation(lin1EndTrcHdr));
	double dtrc, dlin;

	if (_fileRng.totalNumTraces() != _numTrcsInFile) {
		setIsRegularVolume(false);
		// Send error message
//		string msg =  "ERROR:  Irregular data volume\n";
		return false;
	} 
	else {  // Regular volume, valid for 3D flows
		// compute trace spacing within first line (inline if organized by inlines)
		if (numTrcs > 1) dtrc = sqrt(pow((double)(lin1EndX - startX), 2.0) +
			pow((double)(lin1EndY - startY), 2.0))/(numTrcs - 1);
		// compute line spacing within last "trace" (crossline if organized by inlines)
		if (numLins > 1) dlin = sqrt(pow((double)(endX - lin1EndX), 2.0) +
			pow((double)(endY - lin1EndY), 2.0))/(numLins - 1);

		if (coordScalar > 0) {
			dtrc *= coordScalar;
			dlin *= coordScalar;
		}
		else {
			dtrc /= -coordScalar;
			dlin /= -coordScalar;
		}

		_fileRng.setDLine(dlin);
		_fileRng.setDTrace(dtrc);
	}

	setIoRange(_fileRng);	// default read-write range is full file range
	reset();						// reposition to start of first trace in read-write range
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Read data into a buffer; numSmps data values each of sampSize bytes
// Notes:  Increments file ptr by sampSize*numSmps bytes
//------------------------------------------------------------------------------
void SegyRead::read(void* buffer, size_t sampSize, size_t numSmps)
{
   size_t numRead = fread(buffer, sampSize, numSmps, _filePtr);
	try {
      if (numRead != numSmps) {
         string errSrc = "void SegyRead::read()";
			string errMsg = "Problem reading from file ";
         errMsg += pathName();
         char temp[100];
         snprintf(temp,99, "\nSample size = %d bytes,  expected number samples = %d,  actual number samples = %d", 
            sampSize, numSmps, numRead);
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
// Purpose:  Reads the next trace in a segy file, stores in input SeisTrace. 
//		Increments current data position to the next trace. The "next" trace
//		need not be consecutive if the data range allows skips.
// Return:  true if read is successful and the current location in the file  
//		remains within IO range, false otherwise.
// Notes:  Before any trace is read, the _curTrcHdr stores the header of the first
//		trace, for possible use in restoring missing traces prior to the first
//		trace in the file.
//------------------------------------------------------------------------------
bool SegyRead::readTrace(SeisTrace& trc)
{	
	if (!currentLocationIsInRange()) return false;	// current location out of range

	trc.setNumSmps((int)_ioRng.numSmps());				// always clears
	trc.setDSample((float)(_ioRng.dSample()));
	trc.setStartTime((float)(_ioRng.startTime()));

	if (numTrcsRead() == numTrcsInFile()) setIsRestoringMissingTrace();
	else if (!isRestoringMissingTrace())  {
		try {
			readTraceHeader(_curTrcHdr);
 			incrementNumTrcsRead();
		}
		catch (PspaException ex) {
			ex.addSource("bool SegyRead::readTrace");
			char temp[80];
			snprintf(temp,79,"Error after inline %d, crossline %d, trc. seq. num %d", trc.inln(), trc.crln(), trc.seqNum());
			ex.addMessage(temp);
			throw ex;
		}
	}
	_curTrcHdr._trace_number_in_reel = _curTrcHdr._trace_number_in_line = (int)numTrcsRead();

	if (!isRegularVolume()) {				// Handle irregular volumes
		if(!restoreMissingTraces()) {		// Do not restore missing traces
			// Set current location to current trace header
			setCurrentLocation((double)(inlineNumber(_curTrcHdr)), (double)(crosslineNumber(_curTrcHdr)));
		}
		else {									// Restore missing traces 
			DataPosition loc = currentLocation();			// location of trace to output	
			if (traceMatchesLocation(_curTrcHdr, loc)) {	// Trace not missing
				setIsRestoringMissingTrace(false);
				incrementCurrentLocation();					// Set to location of next trace to read/create
			}
			else {								// Fill in missing traces with dead traces
				setIsRestoringMissingTrace(true);
				SegyTraceHeader trcHdr = _curTrcHdr;
				trcHdr._inline_num = trcHdr._orig_field_rec_num = (int)loc.inln;		// set inln, crln locations in two places
				trcHdr._crossline_num = trcHdr._cdp_ensemble_num = (int)loc.crln;
				double cdpx = _fileRng.xLocation(loc.inln, loc.crln);
				double cdpy = _fileRng.yLocation(loc.inln, loc.crln);
				double coordScalar = (double)(_curTrcHdr._coordinate_scalar);
				if (coordScalar < 0.0) coordScalar *= -1.0;
				else coordScalar = 1.0/coordScalar;
				trcHdr._cdp_x = (int)(coordScalar*cdpx);
				trcHdr._cdp_y = (int)(coordScalar*cdpy);
				trcHdr._source_x = trcHdr._group_x = trcHdr._cdp_x;
				trcHdr._source_y = trcHdr._group_y = trcHdr._cdp_y;
				trc.setHeaders(trcHdr);
				trc.setDead();
				incrementCurrentLocation();		// set to location of next trace to read/create
				return true;
			}
		}
	}

	// Insurance:  Modify trace headers slightly and store in input trc
	_curTrcHdr._recording_delay = (short int)(1000.0*_ioRng.startTime() + 0.5); 
	if (_curTrcHdr._cdp_x == 0) {
		_curTrcHdr._cdp_x = _curTrcHdr._source_x;
		_curTrcHdr._cdp_y = _curTrcHdr._source_y;
	}
	else if (_curTrcHdr._source_x == 0) {
		_curTrcHdr._source_x = _curTrcHdr._cdp_x;
		_curTrcHdr._source_y = _curTrcHdr._cdp_y;
	}
	if (_curTrcHdr._group_x == 0) {
		_curTrcHdr._group_x = _curTrcHdr._source_x;
		_curTrcHdr._group_y = _curTrcHdr._source_y;
	}
	trc.setHeaders(_curTrcHdr);

	// Read entire trace but store only that part requested
	// TODO: on network will be slow -- redesign to read only those samples needed
	try {
      read((void*)_trace, numBytesPerValue(), _fileRng.numSmps());
   }
   catch (PspaException& ex) {
      ex.addSource("bool SegyRead::readTrace");
		ex.addMessage("Cannot read SEGY seismic trace values");
		char temp[80];
		snprintf(temp,79,"Error at inline %d, crossline %d, trc. seq. num %d", trc.inln(), trc.crln(), trc.seqNum());
		ex.addMessage(temp);
      throw ex;
	}
	convertBufferStoreTrace(trc);			// convert and store _trace values in input trc
	// Set start time of trace; convert milliseconds to seconds
//	float trcStartTime = (float)(_currentTrcHdr._recording_delay)/1000.0;
	trc.autoSetState();
	if (isRegularVolume()) setFilePtrToNextLocation();	// Increment current location, allow skips
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Compare location (defined only by inline, crossline) of trace
//		header with given location; does not compare times or X-Y positions
// Return:  true if same location, false otherwise
//------------------------------------------------------------------------------
bool SegyRead::traceMatchesLocation(const SegyTraceHeader& trcHdr, const DataPosition& loc) const
{
	if (nint(loc.inln) != inlineNumber(trcHdr) || nint(loc.crln) != crosslineNumber(trcHdr)) return false; 
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Read seismic trace at given trace sequence number in the file
//------------------------------------------------------------------------------
bool SegyRead::readTrace(SeisTrace& trc, int64_t trcSeqNum)
{
	if (!setFilePtrToTrace(trcSeqNum)) return false;
	return readTrace(trc);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Read seismic trace header at given trace sequence number in the file
//------------------------------------------------------------------------------
bool SegyRead::readTraceHeader(SegyTraceHeader& trcHdr, int64_t trcNum)
{
	if (!setFilePtrToTrace(trcNum)) return false;
	return readTraceHeader(trcHdr);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Read seismic trace header of next trace in file
//------------------------------------------------------------------------------
bool SegyRead::readTraceHeader(SegyTraceHeader& trcHdr)
{
	try {
		read(&(trcHdr), traceHeaderLength());
	} catch (PspaException& ex) {
		ex.addSource("bool SegyRead::readTraceHeader");
		ex.addMessage("Cannot read trace header in file");
		throw ex;
		return false;
	}
	swapBytesTraceHeader(trcHdr);
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Returns the number of bytes that have been read in this file.
//------------------------------------------------------------------------------
int64_t SegyRead::numBytesRead() const
{
	return textHeaderLength() + fileHeaderLength() + numTrcsRead()*totalNumBytesPerTrace();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Convert buffer _trace from input segy format to standard floats,
//   store converted values in input SeisTrace. Input trace assumed to have 
//   correct size (currently it is set in readTrace(trc).
//------------------------------------------------------------------------------
bool SegyRead::convertBufferStoreTrace(SeisTrace& trc)
{
	int nsampRead = (int)_ioRng.numSmps();	// # of samples to read
	int z1 = (int)_fileRng.timeIndex(_ioRng.startTime());
	int z2 = z1 + nsampRead - 1;
	eDataFormat inputFormat = dataFormat();
		
	switch (inputFormat) {
	case IBMFLT32:
	case IEEEFLT32:
		{
		if (inputFormat == IBMFLT32) SegyUtil::IBMfloatToIntelFloat((float*)_trace, z1, z2);
		else if (inputFormat == IEEEFLT32) SegyUtil::swapBytes((void*)_trace, z1, z2);
		float* vals = &(((float*)_trace)[z1]);
      for (int i=0; i<nsampRead; i++) trc[i] = vals[i];
		}
		break;
				
	default:
		return false;
	}
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Read a text file that gives the SEGY trace header locations of key
//   values (crosswire file); can be used by both read and write
// Return:  true if valid crosswire file exists, false otherwise
// Notes:  The parameter file is a text (txt) file that has the exact same name 
//   as the input SEGY data file, except it ends with "_hdrFix.txt".
//   The first line is a purely descriptive text, and can be anything. It is
//   followed by the header fixes, which may appear in any order. The following
//   header fixes are honored:
//
//			INLN  loc
//			CRLN  loc
//			CDPX  loc
//			CDPY  loc
//			DOMN  fix
//			UNIT  fix
//
//   The "loc" is an integer byte location in the header file; "fix" is an 
//   integer value to override what is in the header.
//------------------------------------------------------------------------------
bool SegyRead::readSegyHeaderMapFile()
{
	string filnam = pathName();
	size_t pos = filnam.find_last_of(".");
	string trcHdrMapFileName = filnam.substr(0, pos);
	trcHdrMapFileName += "_hdrFix.txt";
	const char* filePathName = trcHdrMapFileName.c_str();

	std::ifstream segyParamsFile;
	segyParamsFile.open(filePathName);
	if (!segyParamsFile.is_open()) return false;
	string line;
	getline(segyParamsFile, line);  // Read first line, which is purely descriptive of file
	SegyHeaderLoc hdrLoc;
	while (segyParamsFile.good()) {
		string tag;
		int value;
		segyParamsFile >> tag >> value;
		
		if (tag.compare("DOMN")==0) {
			setDomain((eDomain)value);
		}
		else if (!decodeHeaderChange(tag, value, hdrLoc)) break;
	}
	setCustomRev(hdrLoc);
	segyParamsFile.close();
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyRead::decodeHeaderChange(const string& tag, int val, SegyHeaderLoc& hdrLoc)
{
	if (tag.compare("INLN")==0) hdrLoc.inln = val;
	else if (tag.compare("CRLN")==0) hdrLoc.crln = val; 
	else if (tag.compare("CDPX")==0) hdrLoc.cdpX = val;
	else if (tag.compare("CDPY")==0) hdrLoc.cdpY = val;
	else return false;
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  return a string of class information
//------------------------------------------------------------------------------
string SegyRead::toString() const
{
	string info = SegyData::toString();
   char temp[80];
	snprintf(temp,79, "\n   Number of traces to read:  %lu", _ioRng.totalNumTraces());
	info += temp;
	int64_t fullNumPts = _fileRng.numPts();
	int64_t rwNumPts = _ioRng.numPts();
	float percentRead = 100.0f*((float)rwNumPts/(float)fullNumPts);
	snprintf(temp,79, "\n   Percentage of full data to read:  %4.1f %%", percentRead);
	info += temp;
	if (isRegularVolume()) {
		info += "\n   Regular 3D volume";
//		sprintf_s(temp, "\n   Number of traces in file range:  %lu", _fileRng.totalNumTraces());
//		info += temp;
	}
	else {
		info += "\n   Irregular 3D volume:";
		int64_t numTrc = numTrcsInFile();
		snprintf(temp,79, "\n      Number of traces in irregular file:  %lu", numTrc);
		info += temp;
		if (restoreMissingTraces()) {
			info += "\n      Restore missing traces";
			snprintf(temp,79, "\n      Number of traces in restored file range:  %lu", _fileRng.totalNumTraces());
			info += temp;	
			snprintf(temp,79, "\n      Number of traces missing in file:  %lu", (int)(_fileRng.totalNumTraces() - numTrc));
			info += temp;
		}
		else info += "\n      Do not restore missing traces";
	}

  return info;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose: return string of trace header info for viewing by user
// Note: resets position of file point to start of first trace
// Input traces refer to sequential trace numbers in file
//------------------------------------------------------------------------------
string SegyRead::getTraceHeadersString(int64_t startTrc, int64_t endTrc)
{
	if (endTrc < startTrc || endTrc > _fileRng.totalNumTraces()) return "invalid trace range";
	string hdrs;
	SegyTraceHeader trcHdr;
	for (int64_t jTrc=startTrc; jTrc<=endTrc; jTrc++) {
		setFilePtrToTrace(jTrc);
		try {
         read(&trcHdr, traceHeaderLength());
      }
      catch (PspaException& ex) {
         throw ex;
         return "Unable to read trace headers";
      }
		swapBytesTraceHeader(trcHdr);
		hdrs += trcHdr.toString();
		if (jTrc != endTrc) hdrs += "\n--------------------------------------------------------\n\n";
	}
	setFilePtrToTrace(1);	// to start of trace 1
	return hdrs;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Dumps specified trace to a file with same name as input file except with 
//   trace # appended; puts in same directory as input file
//   Resets position of file point to start of first trace
//------------------------------------------------------------------------------
bool SegyRead::getTracesInFile(int64_t trc1SeqNum, int numTrcs, SeisTrace trc[])
{
	try {
		if (!_fileRng.isInRange(trc1SeqNum) || !_fileRng.isInRange(trc1SeqNum+numTrcs-1)) {
			string errSrc = "bool SegyRead::getTraceInFile";
			string errMsg = "Trace sequence number out of range!\n";
			char temp[100];
			snprintf(temp,99,"\nSequence number = %d\nNumber of traces in file = %d", trc1SeqNum, _fileRng.totalNumTraces());
			errMsg += temp;
			PspaException ex(errMsg, errSrc);
			throw ex;
		} 
	}
	catch (PspaException& ex) {
		throw ex;
		return false;
	}
	for (int n=0; n<numTrcs; n++) {
		int64_t trcSeqNum = trc1SeqNum + n;
		readTrace(trc[n], trcSeqNum);
	}
	reset();   // reset file pointer to start of first trace, reset counters
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Dumps specified trace to a file with same name as input file except with 
//   trace # appended; puts in same directory as input file
//   Resets position of file point to start of first trace
//------------------------------------------------------------------------------
bool SegyRead::getTraceInFile(int64_t trcSeqNum, SeisTrace& trc)
{
	SeisTrace* trcArr = new SeisTrace[1];
	trcArr[0] = trc;
	getTracesInFile(trcSeqNum, 1, trcArr);
	trc = trcArr[0];
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool SegyRead::getTraceInFile(int inln, int crln, SeisTrace& trc)
{
	if (!isRegularVolume()) return false;
	return getTraceInFile(_fileRng.traceSequenceNumber((double)inln, (double)crln), trc);
}
//------------------------------------------------------------------------------
//==============================================================================

}  // end of namespace PagosaGeo
