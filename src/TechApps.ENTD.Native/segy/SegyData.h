//==============================================================================
// PSPA: Poststack Seismic Processing and Analysis (� Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyData
// Purpose:  Base class for SEGY seismic data IO
//==============================================================================

#ifndef SEGYDATA_H
#define SEGYDATA_H

#include "SegyFileHeader.h"   // segy file header structure
#include "SegyTraceHeader.h"  // segy trace header structure
#include "SegyUtil.h"
#include "DataRange.h"
#include "SeisTrace.h"
#include <inttypes.h>
//typedef int64_t int64_t;

namespace PagosaGeo {

//------------------------------------------------------------------------------
// Structure for SEGY header locations, with default values
//------------------------------------------------------------------------------
struct SegyHeaderLoc {
	// data members
	//int cdpX = 181; 
	//int cdpY = 185;
	//int inln = 189;
	//int crln = 193;

	int cdpX ;
	int cdpY ;
	int inln ;
	int crln ;

	// methods
	void setRev1() {
		cdpX = 181; 
		cdpY = 185;
		inln = 189;
		crln = 193;
	}
	void setRev0() {
		cdpX = 73;		// location of srcX 
		cdpY = 77;		// location of srcY
		inln = 9;		// location of orig field rec num
		crln = 21;		// location of cdp ensemble num
	}
	bool operator!=(const SegyHeaderLoc& loc) const { 
		if (cdpX != loc.cdpX || cdpY != loc.cdpY) return false;
		if (inln != loc.inln || crln != loc.crln) return false;
		return true;
	}
	bool operator==(const SegyHeaderLoc& loc) const { 
		return operator!=(loc);
	}
};
//------------------------------------------------------------------------------

//==============================================================================
// Notes:
//   Setting the current location moves the file pointer to the start of the
//   inline/crossline position given, and ignores the time position. The entire
//   trace is read but only those samples within the RW range are passed. This
//   simplifies trace IO a great deal, but is inappropriate for other tasks,
//   such as those that operate on time slices. Currently the code does not
//   address this problem. The only way the time position might be used is in
//   checks for range, such isInRange(pos).
//
//   Methods that set the current location also move the file ptr to that
//   position in the file.

//   The current location (_currentLoc) and the file pointer (_filePtr) both
//   record the same position in the data set, but in different ways. They must
//   be kept in sync.
//------------------------------------------------------------------------------
class SegyData
{
public:
	SegyData(const string& name, eSegyRevision rev = SEGY_UNDEFINED, bool isRead = true);
	// standard copy constructor; does not copy file ptr, valid only for write
	SegyData(const string& name, const SegyData& segIo, eSegyRevision rev = SEGY_REV1);   
	virtual ~SegyData();

// Standard overloaded operators
	SegyData& operator=(const SegyData&);
	bool operator==(const SegyData& io) const;
	bool operator!=(const SegyData& io) const { return !operator==(io); }

// Set Methods
	bool setNumBytesPerValue(int numBytes);
	bool setDomain(eDomain dom);
   virtual bool setIoRange(const DataRange& rng) = 0;
	bool setTextHeader(const unsigned char* txtHdr);
	void setSegyRev0();
	void setSegyRev1();
	void setCustomRev(const SegyHeaderLoc& hdrLoc) { _revision = SEGY_CUSTOM; _hdrLoc = hdrLoc; }
	bool setDataFormat(eDataFormat frmt);

// Get methods
	bool isSingleLine() const { return _fileRng.isSingleLine(); }
	string pathName() const { return _filePathName; }
	const DataRange& range() const { return _fileRng; }	      // full file range
	const DataRange& ioRange() const { return _ioRng; }	      // input-output range -- subset of full range
	int64_t numBytesPerValue() const { return (int64_t)(_fileHdr.numBytesPerValue()); }
	int64_t totalNumBytesPerTrace() const { return _totalNumBytesPerTrc; }
	eDataFormat dataFormat() const { return _fileHdr.dataFormat(); }
	SegyHeaderLoc headerLocations() const { return _hdrLoc; }		// set by:  setCustomRev(hdrLoc)
	int64_t fileSize() const { return _fileSize; }
	double dSample() const { return _ioRng.dSample(); }
	int64_t numLinsIO() const { return _ioRng.numLins(); }
	int64_t numTrcsIO() const { return _ioRng.numTrcs(); }
	int64_t numSmpsIO() const { return _ioRng.numSmps(); }
	int64_t totalNumTrcsIO() const { return _ioRng.totalNumTraces(); }
	int64_t totalNumTraces() const { return _fileRng.totalNumTraces(); } // # traces in entire volume (useful for irregular volumes, 1D processes)
	eDomain domain() const { return _ioRng.domain(); }
	bool organizedByInline() const { return _ioRng.organizedByInline(); }
	bool organizedByCrossline() const { return _ioRng.organizedByCrossline(); }
	bool isRegularVolume() const { return _regularVolume; }
	DataPosition currentLocation() const { return _curTrcLoc; }
	float percentDataRw() const; // the percentage of data in the full range to read-write 
	int64_t textHeaderLength() const { return TEXTHDRLEN; }
	int64_t fileHeaderLength() const { return FILEHDRLEN; }
	int64_t traceHeaderLength() const { return TRACEHDRLEN; }
	eSegyRevision revision() const { return _revision; }

	// Methods to get correct trace header info (depends on Segy revision)
	int inlineNumber(const SegyTraceHeader& trcHdr) const;
	int crosslineNumber(const SegyTraceHeader& trcHdr) const;
	float xLocation(const SegyTraceHeader& trcHdr) const;
	float yLocation(const SegyTraceHeader& trcHdr) const;

	// Methods that move the file pointer and current location
   bool setFilePtrToCurrentLocation(const DataPosition& pos) { return setFilePtrToCurrentLocation(pos.inln, pos.crln); }
	bool setFilePtrToCurrentLocation(double inln, double crln);
	virtual bool reset() { return setFilePtrToFirstIOTrace(); }  // put file ptr at start of first IO trace

// Utility methods
   bool fileIsOpen() const { return (_filePtr != NULL); }
	bool fileNotOpen() const { return (_filePtr == NULL); }
	bool isRead() const { return _isRead; }
	bool isWrite() const { return !_isRead; }
	        
// Information methods
	virtual string toString() const;
	string getTextHeaderString() const;                  // makes a string from _textHdr
	string getFileHeaderString() const { return _fileHdr.toString(); }
	const unsigned char* getTextHeader() { return _textHdr; }    // returns a copy of ptr
	static void swapBytesTraceHeader(SegyTraceHeader& trcHdr);	// swaps bytes for input trace header

protected:
	SegyData() { _hdrLoc.setRev1(); }                        // disallow default construction but allow inheritance
	void setIoFullRange(bool set) { _ioFullRange = set; }
	bool ioFullRange() const { return _ioFullRange; }
   bool open();
   int64_t findFileSize();
	bool setFilePtrToNextLocation();
	void swapBytesFileHeader();
	void setIsRead() { _isRead = true; }
	void setIsWrite() { _isRead = false; }
	void setIsRegularVolume(bool reg = true) { _regularVolume = reg; }
	bool setTotalNumBytesPerTrace(int64_t numBytes);
	bool setFilePtrFromEnd(int64_t numBytes);
	bool setFilePtrToDataPosition(const DataPosition& pos) { 
		return setFilePtrToTrace(_fileRng.traceSequenceNumber(pos));
	}
	bool setFilePtrToTrace(int64_t trcSeqNum);  // First trace has sequence number = 1
	bool setFilePtrToFirstIOTrace();  // set file ptr to start of first trace in IO range (possibly not first trace in file range)
	bool setCurrentLocation(double inln, double crln);
	bool incrementCurrentLocation() { return _ioRng.getNextLocation(_curTrcLoc); }	// return true if valid location, false otherwise
	bool currentLocationIsInRange() { return _ioRng.isInRange(_curTrcLoc); }
   int64_t incrementFileSize(int64_t lengthIncr) { _fileSize += lengthIncr;   return _fileSize; }
	int64_t makeSeisTraceBuffer();    // uses _fileRng.numSmps()

	FILE* _filePtr;               // prefer this be private
	char* _trace;                 // trace buffer
	unsigned char _textHdr[3200]; // always stored in class as ASCII
	SegyFileHeader& fileHeader() { return _fileHdr; }
	SegyTraceHeader _curTrcHdr;		// current (most recently read) trace header
	DataRange _ioRng;		  // defaults to full file range
	DataRange _fileRng;	  // full file range

private:
	bool close();
	virtual bool initialize();    // call from constructor
	
	SegyFileHeader _fileHdr;      // 400 bytes
	SegyHeaderLoc _hdrLoc;			// for remapping nonstandard trace header locations
	eSegyRevision _revision;	
	string _filePathName; 
	string _dataDescription;
	eDataFormat _dataFormat;      // format of seismic trace data (float, int8, etc.)
	DataPosition _curTrcLoc;
	int64_t _totalNumBytesPerTrc; // # bytes per trace including trace header
	int64_t _fileSize;            // total # bytes in file; needed to find # traces
	bool _isRead;						// true = input, false = output
	bool _regularVolume;
	bool _ioFullRange;				// read or write the full range (usually meaningful only for read)
};
//==============================================================================

}  // end of namespace PagosaGeo

#endif
