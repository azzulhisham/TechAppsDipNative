//==============================================================================
// PSPA: Poststack Seismic Processing and Analysis (� Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyRead
// Purpose:  segy read for seismic/georadar data files
// Notes:  Currently requires a regular (boxy) volume or a single line
//==============================================================================

#ifndef SEGYREAD_H
#define SEGYREAD_H

#include "SegyData.h"
#include "SeisTrace.h"
#include <inttypes.h>
//typedef int64_t int64_t;

namespace PagosaGeo {

//==============================================================================
//------------------------------------------------------------------------------
class SegyRead : public SegyData
{
public:
	SegyRead(const string& name, eSegyRevision rev = SEGY_UNDEFINED);
	SegyRead(const string& name, const SegyHeaderLoc& hdrLoc);
	SegyRead(const SegyRead& segIo);   // standard copy constructor; does not copy file ptr
	virtual ~SegyRead() {}

// Standard overloaded operators
	SegyRead& operator=(const SegyRead&);
	bool operator==(const SegyRead& io) const;
	bool operator!=(const SegyRead& io) const { return !operator==(io); }

	bool openAndSetParameters();
	virtual bool reset() { _numTrcsRead = 0; _isRestoringMissingTrace = false; return SegyData::reset(); }  // set file ptr to start of first RW trace
   virtual bool setIoRange(const DataRange& rng);
	bool setRestoreMissingTraces(bool set = true) { _restoreMissingTraces = set;   return true; }
	bool restoreMissingTraces() const { return _restoreMissingTraces; }
	int64_t numTrcsInFile() const { return _numTrcsInFile; }
	int64_t numTrcsRead() const { return _numTrcsRead; }
   virtual string toString() const;
   string getTraceHeadersString(int64_t startTrc, int64_t endTrc); // cannot be const

	bool setParametersFromTraceHeaders();
	bool readTrace(SeisTrace& trc);
	bool readTrace(SeisTrace& trc, int64_t trcSeqNum);
	bool readTraceHeader(SegyTraceHeader& trcHdr);
	bool readTraceHeader(SegyTraceHeader& trcHdr, int64_t trcNum);

	// Utility Methods
	bool eof() { return numBytesRead() >= fileSize(); }		// feof(_filePtr) does not work
	bool notEof() { return numBytesRead() < fileSize(); }		// !feof(_filePtr) does not work
	bool getTraceInFile(int64_t trcSeqNum, SeisTrace& trc); // cannot be const
	bool getTracesInFile(int64_t trcSeqNum, int numTrcs, SeisTrace trc[]); // cannot be const
	bool getTraceInFile(int inln, int crln, SeisTrace& trc);
	bool readSegyHeaderMapFile();
	bool decodeHeaderChange(const string& tag, int val, SegyHeaderLoc& hdrLoc);
	int64_t numBytesRead() const;

protected:
	void read(void* buffer, size_t sampSize, size_t numSmps = 1);
	bool traceMatchesLocation(const SegyTraceHeader& trcHdr, const DataPosition& loc) const;
	bool setNumTrcsInFile();
	bool setIsRestoringMissingTrace(bool set = true);
	bool isRestoringMissingTrace() const { return _isRestoringMissingTrace; }
	
private:
	SegyRead() : SegyData() {}				// disallow default construction
	bool setParametersFromFileHeader();
	bool convertBufferStoreTrace(SeisTrace& trc);
	int64_t incrementNumTrcsRead() { return ++_numTrcsRead; }

	int64_t _numTrcsInFile;					// number of traces in file (file can be irregular)
	int64_t _numTrcsRead;
	bool _restoreMissingTraces;
	bool _isRestoringMissingTrace;
};
//==============================================================================

}  // end of namespace PagosaGeo

#endif
