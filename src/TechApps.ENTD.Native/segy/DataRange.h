//=============================================================================
// PSPA: Poststack Seismic Processing and Analysis (� Pagosa Geophysical Research, LLC, 2014)
// Class:  DataRange
// Purpose:  General purpose 3D seismic survey range info and methods
// Notes:  
// (1) The inline/crossline increments are the # of inlines/crosslines between
//     traces in this particular range.  This permits subsetting.  To avoid
//     problems, the increment cannot be 0. In the case of starting line =
//     ending line, the increment is set to 1. In this case, computations for
//     max line or number of lines must never count on this being set to 0.
// (2) This class stores all time information as seconds, not ms.
// (3) A NULL range covers no volume or area.  To be null, a range must have
//     either no lines, no traces, or no samples.
// (4) All times are in seconds.
// (5) A 3D (regular) file is organized either by lines and traces, and by
//     inlines and crosslines. The number of lines is the actual number of lines
//     in the file, the number of traces is the actual number of traces.
//     How these relate to the number of inlines and crosslines depends on
//     both the survey organization and also on the inline, crossline increments.
//     For an inline-organized survey (most common case), the number of traces
//     is the number of traces per inline in the file, and the number of lines
//     is the number of traces per crossline in the file.
// (6) _dlin and _dtrc are spacings in feet or meters between adjacent lines
//     or traces in the file, and are not the spacings between inlines and
//     crosslines except in the sole case where the inline/crossline increments
//     are identically 1. The inline and crossline spacings in feet or meters
//     is given by methods dInline() and dCrossline().
//============================================================================

#ifndef DATARANGE_H
#define DATARANGE_H

#include "DataDefs.h"
#include <string>
#include <inttypes.h>
//typedef int64_t int64_t;

using std::string;


namespace PagosaGeo {

struct IC_to_XY {
	// data members for deriving x, y loc from inln, crln loc.
	// xref, yref = x y coordinates at inlnRef, crlnRef
	//double xref = 0.0, yref = 0.0;
	//double inlnRef = 0.0, crlnRef = 0.0;
	//double dXdI = 1.0, dXdC = 1.0;
	//double dYdI = 1.0, dYdC = 1.0;

	double xref , yref ;
	double inlnRef , crlnRef ;
	double dXdI , dXdC ;
	double dYdI , dYdC ;
	void init()
	{
		xref = 0.0; yref = 0.0;
		inlnRef = 0.0; crlnRef = 0.0;
		dXdI = 1.0; dXdC = 1.0;
		dYdI = 1.0; dYdC = 1.0;
	}
};

//=============================================================================
//-----------------------------------------------------------------------------
class DataRange
{
public:
	DataRange();
	DataRange(double minilin, int64_t  numInln, double ilinc, double minxlin, int64_t  numCrln, 
		double xlinc, double mintime, int64_t  numSmps, double tinc);
	DataRange(const DataRange& rng);
	virtual ~DataRange() {}								// destructor

// Overloaded operators
	DataRange& operator=(const DataRange &rng);
	bool operator==(const DataRange& rng) const;
	bool operator!=(const DataRange& rng) const { return !operator==(rng); }
	DataRange operator+(const DataRange& rng) const;	// AND
	DataRange operator*(const DataRange& rng) const;	// OR
	DataRange& operator+=(const DataRange& rng);			// AND
	DataRange& operator*=(const DataRange& rng);			// OR
	
// Sets
	void setDescription(const string& desc) { _desc = desc; }
	void setDataName(const string& nam) { _dataName = nam; }
	void reset();
	void setIrregular(int64_t numTrcs);
	void setAsVolume();
	bool setInlineRange(double ilmin, double ilmax) { return setInlineRange(ilmin, ilmax, _inlnInc); }
	bool setInlineRange(double ilmin, double ilmax, double ilinc);
	bool setInlineRange(double ilmin, int64_t nilin, double ilinc);
	bool setCrosslineRange(double xlmin, double xlmax) { return setCrosslineRange(xlmin, xlmax, _crlnInc); }
	bool setCrosslineRange(double xlmin, double xlmax, double xlinc);
	bool setCrosslineRange(double xlmin, int64_t nxlin, double xlinc);
	bool setTimeRange(double tmin, int64_t nsmps, double dsmp);
   bool setTimeRange(double tmin, double tmax, double dsmp);
 	bool setTimeRange(double tmin, double tmax) { return setTimeRange(tmin, tmax, _dsmp); }
	bool setInlineIncrement(double ilinc);     
	bool setCrosslineIncrement(double xlinc);     
	bool setStartTime(double tmmin);		// acts as a time shift; num samples, dsmp remain the same
	bool setDomain(eDomain zdomain);		// TIME_DOMAIN, DEPTH_DOMAIN, etc.
	bool setInlineOrganized() { return setOrganization(BY_INLINE); }
	bool setCrosslineOrganized() { return setOrganization(BY_CROSSLINE); }
	bool setOrganization(eSurveyOrganization org);
	bool setXyUnits(eXyUnits units);
	bool setUnitDIntervals();
	bool setDLine(double dlin) { _dlin = dlin; return true; }
	bool setDTrace(double dtrc) { _dtrc = dtrc; return true; }
	bool setDSample(double dsmp);
	bool setDInline(double dlin);
	bool setDCrossline(double dlin);
	bool setNumInlines(int64_t nilin);
	bool setNumCrosslines(int64_t nxlin);
	bool setNumSmps(int64_t nsmp);
	bool setNumSmps(int64_t nsmp, double dsmp);

	bool increaseInlineIncrement(int factor);
	bool decreaseInlineIncrement(int factor);
	bool increaseCrosslineIncrement(int factor);
	bool decreaseCrosslineIncrement(int factor);
	bool increaseTimeIncrement(int factor);
	bool decreaseTimeIncrement(int factor);
	bool subset(int inlnFctr, int crlnFctr);
	bool subset(int inlnFctr, int crlnFctr, int vertFctr);

// Gets
	string description() const { return _desc + " Data Range"; }
	string dataName() const { return _dataName; }
	bool isSingleLine() const { return _irregular; }
	int64_t numInlines() const { return _numInln; }
	int64_t numCrosslines() const { return _numCrln; }
	int64_t numSmps() const { return _numSmps; }
	int64_t numTrcs() const;
	int64_t numLins() const;
	int64_t totalNumTraces() const { if (_irregular) return _irregularNumTrcs; else return (int64_t)(_numCrln*_numInln); }
	int64_t numPts() const { return _numInln*_numCrln*_numSmps; }
	double getInline(int64_t  k) const;	    // input is index number; depends on survey organization
	double getCrossline(int64_t  j) const;  // input is index number; depends on survey organization
	double getTime(int64_t  i) const;		 // input is index number
// The following three methods refer to the x-y spacings (ft or m) of the actual data
	double dSample() const { return _dsmp; }
	double dTrace() const { return _dtrc; }
	double dLine() const { return _dlin; }
// The following two methods refer to the x-y spacings (ft or m) of the inlines and crosslines;
// because the inline and crossline increments can be greater than 1, these do not
// necessarily equal the spacings between the traces and lines of the data
	double dInline() const;
	double dCrossline() const;
// Index get methods
	bool inlnCrlnToLinTrcIndx(double inln, double crln, int& lin, int& trc) const;
	bool linTrcIndxToInlnCrln(int lin, int trc, double& inln, double& crln) const;
	int64_t  timeIndex(double time) const;			// return closest sample index of given time; start time index = 0
	double nearestSampleTime(double tm) const { return _dsmp*timeIndex(tm); }	// return time at sample location closest to given time		
	int64_t traceSequenceNumber(double inln, double crln) const;
	int64_t traceSequenceNumber(const DataPosition& pos) const { return traceSequenceNumber(pos.inln, pos.crln); }
	DataPosition dataPosition(int64_t trcSeqNum) const;
	double inlineIncr() const { return _inlnInc; }
	double crosslineIncr() const { return _crlnInc; }
	double startInline() const { return _inlnMin; }
	double endInline() const { return _inlnMax; }
	double startCrossline() const { return _crlnMin; }
	double endCrossline() const { return _crlnMax; }
	double startTime() const { return _timeMin; }
	double endTime() const { return _timeMax; }
	double timeIncr() const { return _dsmp; }		// same as dSample()
	eDomain domain() const { return _domain; }
	bool isTimeDomain() const { return (_domain == TIME_DOMAIN); }
	bool isDepthDomain() const { return (_domain == DEPTH_DOMAIN); }
	bool isFrequencyDomain() const { return (_domain == FREQ_DOMAIN); }
	bool isAttributeDomain() const { return (_domain == ATRB_DOMAIN); }
	bool organizedByInline() const { return (_organization == BY_INLINE); }
	bool organizedByCrossline() const { return !organizedByInline(); }
	eXyUnits xyUnits() const { return _xyUnits; }
	bool isMetric() const { return (_xyUnits == METRIC_UNITS) ? true : false; }
	bool isEnglish() const { return (_xyUnits == ENGLISH_UNITS) ? true : false; }
	bool isDimensionless() const { return (_xyUnits == DIMENSIONLESS) ? true : false; }
	string toString() const;	// return range info in std string
	bool isInRange(int64_t  trcSeqNum) const { return (trcSeqNum > 0 && trcSeqNum <= totalNumTraces()) ? true : false; }
	bool isInRange(double inln, double crln) const;
	bool isInRange(int inlnIndx, int crlnIndx) const;
	bool outOfRange(double inln, double crln) const { return !isInRange(inln, crln); }
	bool outOfRange(int inlnIndx, int crlnIndx) const { return !isInRange(inlnIndx, crlnIndx); }
	bool isInRange(const DataPosition& pos) const;
	// Miscellaneous methods
	bool isDefault() const { return (numPts()==1) ? true : false; }
	int64_t dimension() const;
	bool getNextLocation(double& inln, double& crln);
	bool getNextLocation(DataPosition& pos);	// inline, crossline of next trace past given position
	bool getFirstLocation(double& inln, double& crln);	   // inline, crossline of first trace
	DataPosition getFirstLocation();	   // inline, crossline of first trace
	bool getLastLocation(double& inln, double& crln);	   // inline, crossline of last trace
	DataPosition getLastLocation();	   // inline, crossline of last trace

	void setInlnCrlnToXyCoeff(IC_to_XY coef);
	// Methods to derive x, y locations from inln, crln locations
	double xLocation(double inln, double crln) const;
	double yLocation(double inln, double crln) const;

protected:
	int64_t inlineIndex(double inln) const;			// use of this methods depends or organization
	int64_t crosslineIndex(double crln) const;		// use of this methods depends or organization
	
private:
	string _desc;			// default description is "Data Range"; use for reporting
	string _dataName;		// name of dataset that range refers to; use for reporting
	int64_t _numInln, _numCrln, _numSmps;
	double _inlnMin, _inlnMax, _inlnInc;
	double _crlnMin, _crlnMax, _crlnInc;
	double _timeMin, _timeMax;          // in seconds
	double _dsmp, _dtrc, _dlin;	
	eXyUnits _xyUnits;                  // spatial (or depth) units; metric (1) or English (2)
	eDomain _domain;                    // TX_DOM or DEPTH_DOMAIN
	eSurveyOrganization _organization;  // BY_INLINE or BY_CROSSLINE
	int64_t _irregularNumTrcs;
	bool _irregular;							// range valid only for 1D processing

	// data members for deriving x, y loc from inln, crln loc.
	// _X0, _Y0 = x y coordinates at inln, crln origin (=0)
	IC_to_XY _coef;
};
//-----------------------------------------------------------------------------
//=============================================================================

}  // end namespace PagosaGeo

#endif
