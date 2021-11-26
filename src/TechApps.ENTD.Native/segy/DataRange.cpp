//=============================================================================
// PSPA: Poststack Seismic Processing and Analysis (� Pagosa Geophysical Research, LLC, 2014)
// Class:  DataRange
// Purpose:  Store inline/crossline/time ranges for a volume
// Notes:  
//		(1) A range is always assumed to have at least 1 point;
//		Thus it will have a minimum of 1 line, 1 trace, 1 sample.
//		(2) The number of inlines or number of crosslines refers to the actual number
//		in the data file, not the number of possible inlines or crosslines (which
//		in any case is infinite, since it is possible to have non-integral line
//		numbers, such inline 1120.5)
// Units:  time is in seconds and increases downward
//=============================================================================

#include <algorithm>
#include "DataRange.h"
#include <inttypes.h>
#include <stdio.h>
#include <math.h> 
//typedef int64_t int64_t;

using std::min;
using std::max;


namespace PagosaGeo {

//=============================================================================
//-----------------------------------------------------------------------------
// Full constructor
// Note:  This version sets the # of lines based on the increments.
//-----------------------------------------------------------------------------
DataRange::DataRange(double minilin, int64_t numInln, double ilinc, double minxlin, 
   int64_t numCrln, double xlinc, double mintime, int64_t numSmps, double tinc)
{
	_coef.init();
	reset();
	setInlineRange(minilin, numInln, ilinc);
	setCrosslineRange(minxlin, numCrln, xlinc);
	setTimeRange(mintime, numSmps, tinc);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Default constructor
//-----------------------------------------------------------------------------
DataRange::DataRange()
{
	_coef.init();
	reset();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
DataRange::DataRange(const DataRange& rng)
{
	*this = rng; 
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Note:  Does not equate descriptions
//-----------------------------------------------------------------------------
DataRange& DataRange::operator=(const DataRange& rng)
{
	_dataName = rng._dataName;
	_inlnMin = rng._inlnMin;
	_inlnMax = rng._inlnMax;
	_inlnInc = rng._inlnInc;
	_numInln = rng._numInln;
	_crlnMin = rng._crlnMin;
	_crlnMax = rng._crlnMax;
	_crlnInc = rng._crlnInc;
	_numCrln = rng._numCrln;
	_timeMin = rng._timeMin;
	_timeMax = rng._timeMax;
	_numSmps = rng._numSmps;
	_dsmp = rng._dsmp;
	_dtrc = rng._dtrc;
	_dlin = rng._dlin;
	_xyUnits = rng._xyUnits;
	_domain = rng._domain;
	_organization = rng._organization;
	_coef = rng._coef;
	_irregular = rng._irregular;
	_irregularNumTrcs = rng._irregularNumTrcs;
	return *this;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  Does not compare range descriptions or data names 
//-----------------------------------------------------------------------------
bool DataRange::operator==(const DataRange& rng) const
{
	if (_inlnMin == rng._inlnMin && _inlnMax == rng._inlnMax &&
		_inlnInc == rng._inlnInc && _numInln == rng._numInln &&
		_crlnMin == rng._crlnMin && _crlnMax == rng._crlnMax &&
		_crlnInc == rng._crlnInc && _numCrln == rng._numCrln &&
		_timeMin == rng._timeMin && _timeMax == rng._timeMax &&
		_numSmps == rng._numSmps && _dsmp == rng._dsmp && _dtrc == rng._dtrc &&
		_dlin == rng._dlin && _domain == rng._domain && _xyUnits == rng._xyUnits &&
		_irregular == rng._irregular && _irregularNumTrcs == rng._irregularNumTrcs &&
		_organization == rng._organization) return true;
	else return false;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose:  ANDs together two ranges to produce the smallest range that
//		incorporates both
// Note:  this is a key method for multi-volume operations
//-----------------------------------------------------------------------------
DataRange DataRange::operator+(const DataRange& rng) const
{
	DataRange andRng(*this);
	return andRng += rng;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose:  ORs together two ranges to produce the largest range
//		incorporated in both (the overlap between the two ranges)
//-----------------------------------------------------------------------------
DataRange DataRange::operator*(const DataRange& rng) const
{
	DataRange orRng(*this);
	return orRng *= rng;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose:  ANDs together two ranges to produce the smallest range that
//		contains both (union operation)
// Notes:  This assumes that min vals < max vals; eventually, I will need to
//		look at the sign of the increments, inlnInc, crlnInc, & timeInc
//		 to get this right if they are allowed to be negative.  Also, currently
//		this method takes no account of increments, which would be used in
//		determining the # of samples, lines, etc.
//-----------------------------------------------------------------------------
DataRange& DataRange::operator+=(const DataRange& rng)
{
	setInlineRange(min(_inlnMin, rng._inlnMin), max(_inlnMax, rng._inlnMax));
	setCrosslineRange(min(_crlnMin, rng._crlnMin), max(_crlnMax, rng._crlnMax));
	setTimeRange(min(_timeMin, rng._timeMin), max(_timeMax, rng._timeMax));
	return *this;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose:  ORs together two ranges to the overlap range
// Notes:  This requires that both this range and input range have increments
//		of same sign.  (Time increments are currently assumed always positive.)
//		this method takes no account of increments, which would be used in
//		determining the # of samples, lines, etc.
//	(2) Sets the range to null if non-overlapping ranges are multiplied together.
//-----------------------------------------------------------------------------
DataRange& DataRange::operator*=(const DataRange& rng)
{
	bool status = true;

	if (_inlnInc > 0 && rng._inlnInc > 0)
		status = setInlineRange(max(_inlnMin, rng._inlnMin), min(_inlnMax, rng._inlnMax));
	else if (_inlnInc < 0 && rng._inlnInc < 0)
		status = setInlineRange(min(_inlnMin, rng._inlnMin), max(_inlnMax, rng._inlnMax));
	else if (_inlnInc > 0)
		status = setInlineRange(max(_inlnMin, rng._inlnMax), min(_inlnMax, rng._inlnMin));
	else if (_inlnInc < 0)
		status = setInlineRange(min(_inlnMin, rng._inlnMax), max(_inlnMax, rng._inlnMin));
	if (status == false)  {
		_inlnMin = _inlnMax = _inlnInc = 0.0;
		_numInln = 0;
	}

  status = true;
	if (_crlnInc > 0 && rng._crlnInc > 0)
		status = setCrosslineRange(max(_crlnMin, rng._crlnMin), min(_crlnMax, rng._crlnMax));
	else if (_crlnInc < 0 && rng._crlnInc < 0)
		status = setCrosslineRange(min(_crlnMin, rng._crlnMin), max(_crlnMax, rng._crlnMax));
	else if (_crlnInc > 0)
		status = setCrosslineRange(max(_crlnMin, rng._crlnMax), min(_crlnMax, rng._crlnMin));
	else if (_crlnInc < 0)
		status = setCrosslineRange(min(_crlnMin, rng._crlnMax), max(_crlnMax, rng._crlnMin));
	if (status == false)  {
		_crlnMin = _crlnMax = _crlnInc = 0.0;
		_numCrln = 0;
	}

	status = setTimeRange(max(_timeMin, rng._timeMin), min(_timeMax, rng._timeMax));
	if (status == false)  {
		_timeMin = _timeMax = _dsmp = 0.0;
		_numSmps = 0;
	}

	return *this;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose:  set this Range to the default state
// Notes:  A range is assumed to always have at least 1 line, 1 trace, and 
//   1 sample
//-----------------------------------------------------------------------------
void DataRange::reset()
{
	_inlnMin = 1.0;
	_inlnMax = 1.0;
	_crlnMin = 1.0;
	_crlnMax = 1.0;
	_timeMin = 0.0;
	_timeMax = 0.0;
	_inlnInc = 1.0;
	_crlnInc = 1.0;
	_dsmp = 1.0;
	_dtrc = 1.0;
	_dlin = 1.0;
	_numInln = 1;
	_numCrln = 1;
	_numSmps = 1;
	_xyUnits = METRIC_UNITS;
	_domain = TIME_DOMAIN;
	_organization = BY_INLINE;
	_irregularNumTrcs = 0;
	_irregular = false;
}
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
void DataRange::setIrregular(int64_t numTrcs)
{
	_irregular = true;
	_irregularNumTrcs = abs(numTrcs);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void DataRange::setAsVolume()
{
	_irregular = false;
	_irregularNumTrcs = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DataRange::setDomain(eDomain zdomain)
{
	switch (zdomain) {
	case TIME_DOMAIN:
	case DEPTH_DOMAIN:
	case FREQ_DOMAIN:
	case ATRB_DOMAIN:
		_domain = zdomain;
		return true;
		
	default:
		return false;  // do nothing at present
	}
}
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::setXyUnits(eXyUnits units)
{
	switch (units) {
	case ENGLISH_UNITS: 
	case METRIC_UNITS: 
	case DIMENSIONLESS: 
		_xyUnits = units;
		break;
		
	default:
	   _xyUnits = METRIC_UNITS;
	}
	
	return (_xyUnits == units);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::setUnitDIntervals()
{
	setXyUnits(DIMENSIONLESS);
	_dlin = _dtrc = 1.0; 
	return setDSample(1.0);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::setDInline(double dlin)
{
	if (organizedByInline()) _dlin = dlin*_inlnInc;
	else _dtrc = dlin*_inlnInc;
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::setDCrossline(double dlin)
{
	if (organizedByInline()) _dtrc = dlin*_crlnInc;
	else _dlin = dlin*_crlnInc;
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: reset inline range using new inline increment.
// Notes: This method corrects the sign of the inline increment if it is
//		wrong, but it assumes the magnitude is correct
//-----------------------------------------------------------------------------
bool DataRange::setInlineRange(double ilmin, double ilmax, double ilinc)
{
	_inlnInc = ilinc;
	if ((ilmax < ilmin) && _inlnInc > 0) _inlnInc += -1;
	else if ((ilmax > ilmin) && _inlnInc < 0) _inlnInc += -1;
	_inlnMin = ilmin;
	_inlnMax = ilmax;
	_numInln = (int)((_inlnMax - _inlnMin)/_inlnInc + 1.001);
	// ensure _inlnMax falls on an integral line; adjust inlnMax if necessary
	_inlnMax = _inlnMin + _inlnInc*(_numInln - 1);
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Return:  true if unchanged or new settings were made
//          false if input was invalid
//-----------------------------------------------------------------------------
bool DataRange::setInlineRange(double ilmin, int64_t nilin, double ilinc)
{
	if (ilmin < 1.0 || nilin < 1 || ilinc == 0.0) return false;
	if (_inlnMin == ilmin && _numInln == nilin && _inlnInc == ilinc) return true;
	_numInln = nilin;
	_inlnMin = ilmin;
	_inlnInc = ilinc;
	_inlnMax = _inlnMin + (_numInln - 1)*_inlnInc;
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: reset crossline range using new crossline increment
// Notes: This method corrects the sign of the inline increment if it is
//		wrong, but it assumes the magnitude is correct
//-----------------------------------------------------------------------------
bool DataRange::setCrosslineRange(double xlmin, double xlmax, double xlinc)
{
	_crlnInc = xlinc;
	if ((xlmax < xlmin) && _crlnInc > 0) _crlnInc += -1;
	else if ((xlmax > xlmin) && _crlnInc < 0) _crlnInc += -1;
	_crlnMin = xlmin;
	_crlnMax = xlmax;
	_numCrln = (int)((_crlnMax - _crlnMin)/_crlnInc + 1.001);
	// ensure _crlnMax falls on an integral line; adjust crlnMax if necessary
	_crlnMax = _crlnMin + _crlnInc*(_numCrln - 1);
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Return: true if set correctly (or unchanged), false is problem
//-----------------------------------------------------------------------------
bool DataRange::setCrosslineRange(double xlmin, int64_t nxlin, double xlinc)
{
	if (xlmin < 1 || nxlin < 1 || xlinc == 0.0) return false;
	if (_crlnMin == xlmin && _numCrln == nxlin && _crlnInc == xlinc) return true;
	_numCrln = nxlin;
	_crlnMin = xlmin;
	_crlnInc = xlinc;
	_crlnMax = _crlnMin + (_numCrln - 1)*_crlnInc;
	return true;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Methods to increase or decrease the line increments
// Notes:  Increase the line increment to increase line spacing; use in
//   trace composite and other methods that reduce the number of traces/line.
//   Decrease the line increment to decrease the line spacing; use in methods
//   that interpolate new traces between existing traces. Adjusts line, trace
//   spacing by a factor = new incr / old incr.
//-----------------------------------------------------------------------------
bool DataRange::setInlineIncrement(double ilinc)
{
	return setInlineRange(startInline(), endInline(), ilinc);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::setCrosslineIncrement(double xlinc)
{
	return setCrosslineRange(startCrossline(), endCrossline(), xlinc);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  time increments must always be positive, unlike line increments
//-----------------------------------------------------------------------------
bool DataRange::setTimeRange(double tmin, int64_t nsmps, double dsmp)
{
	if (_timeMin == tmin && _numSmps == nsmps && _dsmp == dsmp) return true;
	if (dsmp <= 0.0) return false;
	if (nsmps < 1) {
		_numSmps = 1; 
		_dsmp = dsmp;
		_timeMin = tmin;
		return true;
	}
	_numSmps = nsmps;
	_timeMin = tmin;
	_dsmp = dsmp;
	_timeMax = _timeMin + _dsmp*(_numSmps - 1);
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Honors tmin, assumes _dsmp is set, sets _timeMax, _numSmps to best agree
//   with given tmax
//-----------------------------------------------------------------------------
bool DataRange::setTimeRange(double tmin, double tmax, double dsmp)
{
	if (tmax < tmin || dsmp < 0.0) return false;
	if (_timeMin == tmin && _timeMax == tmax && _dsmp == dsmp) return true;
	_timeMin = tmin;
	_timeMax = tmax;
   _dsmp = dsmp;
	_numSmps = (int)((_timeMax - _timeMin)/_dsmp + 1.5);
	// ensure timeMax is at an integral sample point
	_timeMax = _timeMin + _dsmp*(_numSmps - 1);
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose:  reset start and end times; keep same number of samples, dsmp
//-----------------------------------------------------------------------------
bool DataRange::setStartTime(double tmin)
{
	_timeMin = tmin;
	_timeMax = _timeMin + _dsmp*(_numSmps - 1);
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose:  reset dsmp, end time; keep same number of samples, start time
//-----------------------------------------------------------------------------
bool DataRange::setDSample(double dsmp)
{
	return setTimeRange(_timeMin, _numSmps, dsmp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::setOrganization(eSurveyOrganization org)
{
	if (_organization == org) return true;
	switch(org) {
	case BY_INLINE:
	case BY_CROSSLINE:
		_organization = org;
		return true;

	default:
		_organization = BY_INLINE;
		return false;
	}
}
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  resets inline min, max, and incr to reasonable default -- use this
//    method only if you don't care about the range!
//-----------------------------------------------------------------------------
bool DataRange::setNumInlines(int64_t nilin)
{
   if (nilin == 0) return false;
   if (_numInln == nilin) return true;
   
   _numInln = nilin;
   _inlnMin = 1.0;
   _inlnMax = (double)_numInln;
   _inlnInc = 1.0;
   return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  resets crossline min, max, and incr to reasonable default -- use this
//    method only if you don't care about the range!
//-----------------------------------------------------------------------------
bool DataRange::setNumCrosslines(int64_t nxlin)
{
   if (nxlin == 0) return false;
   if (_numCrln == nxlin) return true;
   
   _numCrln = nxlin;
   _crlnMin = 1.0;
   _crlnMax = (double)_numCrln;
   _crlnInc = 1.0;
   return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  Assumes constant _dsmp & time min; resets time max
//-----------------------------------------------------------------------------
bool DataRange::setNumSmps(int64_t nsmp)
{
   if (nsmp == 0) return false;
   if (_numSmps == nsmp) return true;
   
   _numSmps = nsmp;
   _timeMax = _timeMin + (double)(_numSmps - 1)*_dsmp;
    return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  Assumes constant _timeMin; resets _timeMax based on other parameters
//-----------------------------------------------------------------------------
bool DataRange::setNumSmps(int64_t nsmp, double dsmp)
{
   if (nsmp == 0) return false;
   if (_numSmps == nsmp) return true;
   
   _numSmps = nsmp;
	_dsmp = dsmp;
   _timeMax = _timeMin + (double)(_numSmps - 1)*_dsmp;
    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Get Methods
//-----------------------------------------------------------------------------
// Purpose:  Get the number of traces or lines in the file
//-----------------------------------------------------------------------------
int64_t DataRange::numTrcs() const
{
	if (organizedByInline()) return _numCrln;
	else return _numInln;
}
int64_t DataRange::numLins() const 
{
	if (organizedByInline()) return _numInln;
	else return _numCrln;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  Checks to ensure that the returned inline # is reasonable.  Returns
//    inline closest to given index.
//-----------------------------------------------------------------------------
double DataRange::getInline(int64_t k) const
{
  if (k <= 0) return _inlnMin;
  else if (k >= _numInln - 1) return _inlnMax;
  else return _inlnMin + (double)k*_inlnInc;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  Checks to ensure that the returned crossline # is reasonable.  Returns
//    crossline that is closest to the given index.  If index is out-of-bounds,
//    this could result in significant error.
//-----------------------------------------------------------------------------
double DataRange::getCrossline(int64_t j) const
{
  if (j <= 0) return _crlnMin;
  else if (j >= _numCrln - 1) return _crlnMax;
  else return _crlnMin + (double)j*_crlnInc;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  Checks to ensure that the returned time is reasonable.  Returns
//    time that is closest to the given index.  If index is out-of-bounds,
//    this could result in significant error.
//-----------------------------------------------------------------------------
double DataRange::getTime(int64_t i) const
{
  if (i <= 0) return _timeMin;
  else if (i >= _numSmps - 1) return _timeMax;
  else return _timeMin + (double)i*_dsmp;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  returns spacing in meters or feet between two consecutive inlines,
//   whether they are populated in the data or not.
//-----------------------------------------------------------------------------
double DataRange::dInline() const
{
	return organizedByInline() ? _dlin/_inlnInc : _dtrc/_inlnInc;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  returns spacing in meters or feet between two consecutive crosslines,
//   whether they are populated in the data or not.
//-----------------------------------------------------------------------------
double DataRange::dCrossline() const
{
  return organizedByInline() ? _dtrc/_crlnInc : _dlin/_crlnInc;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  First trace in file has sequence number = 1.
//-----------------------------------------------------------------------------
int64_t DataRange::traceSequenceNumber(double inln, double crln) const
{
	int64_t numLin, numTrc;
	double bias = 0.1;
	if (organizedByInline()) {
		numLin = (int64_t)((inln - _inlnMin)/_inlnInc + bias);
		numTrc = (int64_t)((crln - _crlnMin)/_crlnInc + bias);
		return numLin*_numCrln + numTrc + 1;
	}
	else {
		numLin = (int64_t)((crln - _crlnMin)/_crlnInc + bias);
		numTrc = (int64_t)((inln - _inlnMin)/_inlnInc + bias);
		return numLin*_numInln + numTrc + 1;
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Note:  Checks to ensure that input time lies within the range. First trace
//		in file has sequence number = 1.
//------------------------------------------------------------------------------
DataPosition DataRange::dataPosition(int64_t trcSeqNum) const
{
	DataPosition pos;
	if (totalNumTraces() == 0) return pos;		// throw exception here
	int64_t inlnIndex, crlnIndex;
	int64_t trcSeqIndex = trcSeqNum - 1;
	if (_organization == BY_INLINE) {
		inlnIndex = trcSeqIndex%_numCrln;
		crlnIndex = trcSeqIndex - inlnIndex*_numCrln;
	}
	else {
		crlnIndex = trcSeqIndex%_numInln;
		inlnIndex = trcSeqIndex - crlnIndex*_numInln;
	}
	pos.inln = getInline((int)inlnIndex);
	pos.crln = getCrossline((int)crlnIndex);
	pos.time = _timeMin;
	return pos;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  true if input inline/crossline position lies within this range, false otherwise
// TODO:  input must also be a valid point (take into account line increments)
//------------------------------------------------------------------------------
bool DataRange::isInRange(double inln, double crln) const
{
	if (_inlnInc > 0.0) {		// normal situation
		if (inln < _inlnMin || inln > _inlnMax) return false;
	}
	else {							// negative inline increment
		if (inln > _inlnMin || inln < _inlnMax) return false;
	}

	if (_crlnInc > 0.0) {		// normal situation
		if (crln < _crlnMin || crln > _crlnMax) return false;
	}
	else {							// negative crossline increment
		if (crln > _crlnMin || crln < _crlnMax) return false;
	}

	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  true if input position lies within this range, false otherwise
// TODO:  input must also be a valid point (take into account line increments)
//------------------------------------------------------------------------------
bool DataRange::isInRange(const DataPosition& pos) const
{
	if (!isInRange(pos.inln, pos.crln)) return false;

// time increments must always be positive, unlike line increments

	if (pos.time < _timeMin || pos.time > _timeMax) return false;
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  true if input inline/crossline position lies within this range, false otherwise
// TODO:  input must also be a valid point (take into account line increments)
//------------------------------------------------------------------------------
bool DataRange::isInRange(int inlnIndx, int crlnIndx) const
{
	if (inlnIndx < 0 || inlnIndx >= (int)_numInln) return false;
	else if (crlnIndx < 0 || crlnIndx >= (int)_numCrln) return false;
	else return true;
}
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  auto-determine the dimension of this range (not sure why)
// Return:  1 for single trace, 2 for single line, 3 for multiple lines
//------------------------------------------------------------------------------
int64_t DataRange::dimension() const
{
  if (_inlnMin == _inlnMax && _crlnMin == _crlnMax) return 1;
  else if (_inlnMin == _inlnMax || _crlnMin == _crlnMax) return 2;
  else return 3;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Purpose:  get the next location in the DataRange given an inline/crossline
//   location or data position.  If only inline/crossline position, then
//   sets to next trace location.
// Return:  true if next location is in range, false if out of range (finished incrementing)
// Notes:  Always sets time to minimum.  Use this for moving to new trace.
//   If the next location goes out of bounds, then it recycles back to the
//   first location.
// TODO: At present this method merely increments the inln, crln pair,
//    does nothing to ensure they fall on the implied grid of the range.
//------------------------------------------------------------------------------
bool DataRange::getNextLocation(double& inln, double& crln)
{
	if ((inln < _inlnMin && _organization == BY_INLINE) || 
		(crln < _crlnMin && _organization == BY_CROSSLINE)) 
	{
		DataPosition firstPos = getFirstLocation();
		inln = firstPos.inln;
		crln = firstPos.crln;
		return true;
	}
			
	if (_organization == BY_INLINE) {
		crln += _crlnInc;
		if (crln > _crlnMax) {
			crln = _crlnMin;
			inln += _inlnInc;
		}
	}
	else {		// crossline organized
		inln += _inlnInc;
		if (inln > _inlnMax) {
			inln = _inlnMin;
			crln += _crlnInc;
		}
	}

	return isInRange(inln, crln);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Purpose:  return the next (x,y) location defined in this DataRange.
//   Time is always set to minimum
// Return:  true if data position is in range, false otherwise
//------------------------------------------------------------------------------
bool DataRange::getNextLocation(DataPosition& nextLoc)
{
	nextLoc.time = _timeMin;	// move time to next position	
	return getNextLocation(nextLoc.inln, nextLoc.crln);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DataRange::getFirstLocation(double& inln, double& crln)
{
	inln = _inlnMin;
	crln = _crlnMin;
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
DataPosition DataRange::getFirstLocation()
{
	DataPosition firstLoc(_inlnMin, _crlnMin, _timeMin);
	return firstLoc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DataRange::getLastLocation(double& inln, double& crln)
{
	inln = _inlnMax;
	crln = _crlnMax;
	return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
DataPosition DataRange::getLastLocation()
{
	DataPosition lastLoc(_inlnMax, _crlnMax, _timeMax);
	return lastLoc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DataRange::increaseInlineIncrement(int factor)
{
	if (factor == 0) return false;
	factor = abs(factor);
	_inlnInc *= factor;
	setDInline(factor*dInline());
	return setInlineRange(startInline(), endInline());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DataRange::decreaseInlineIncrement(int factor)
{
	if (factor == 0) return false;
	factor = abs(factor);
	_inlnInc /= factor;
	setDInline(dInline()/factor);
	return setInlineRange(startInline(), endInline());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DataRange::increaseCrosslineIncrement(int factor)
{
	if (factor == 0) return false;
	factor = abs(factor);
	_crlnInc *= factor;
	setDCrossline(factor*dCrossline());
	return setCrosslineRange(startCrossline(), endCrossline());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DataRange::decreaseCrosslineIncrement(int factor)
{
	if (factor == 0) return false;
	factor = abs(factor);
	_crlnInc /= factor;
	setDCrossline(dCrossline()/factor);
	return setCrosslineRange(startCrossline(), endCrossline());
}
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::increaseTimeIncrement(int factor)
{
	if (factor == 0) return false;
	factor = abs(factor);
	_dsmp *= factor;
	return setTimeRange(startTime(), endTime());
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::decreaseTimeIncrement(int factor)
{
	if (factor == 0) return false;
	factor = abs(factor);
	_dsmp *= factor;
	return setTimeRange(startTime(), endTime());
}
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DataRange::subset(int inlnFctr, int crlnFctr)
{
	if (!increaseInlineIncrement(inlnFctr)) return false;
	return increaseCrosslineIncrement(crlnFctr);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DataRange::subset(int inlnFctr, int crlnFctr, int vertFctr)
{
	if (!increaseInlineIncrement(inlnFctr)) return false;
	if (!increaseCrosslineIncrement(crlnFctr)) return false;
	return increaseTimeIncrement(vertFctr);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Methods to get indices
//------------------------------------------------------------------------------
// Note:  Checks to ensure that the given inline # lies within the range.
//-----------------------------------------------------------------------------
int64_t DataRange::inlineIndex(double inln) const
{
	if (_inlnInc > 0.0) {
		if (inln < _inlnMin) return 0;
		if (inln > _inlnMax) return (int64_t)(_numInln - 1);
	}
	else {
		if (inln > _inlnMin) return 0;
		if (inln < _inlnMax) return (int64_t)(_numInln - 1);
	}
	return (int64_t)(0.5 + (double)(inln - _inlnMin)/_inlnInc);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Note:  Checks to ensure that given crossline # lies within the range.
//-----------------------------------------------------------------------------
int64_t DataRange::crosslineIndex(double crln) const
{
	if (_crlnInc > 0.0) {
      if (crln < _crlnMin) return 0;
      if (crln > _crlnMax) return (int64_t)(_numCrln - 1);
   }
	else {
      if (crln > _crlnMin) return 0;
      if (crln < _crlnMax) return (int64_t)(_numCrln - 1);
   }
	return (int64_t)(0.5 + (double)(crln - _crlnMin)/_crlnInc);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::inlnCrlnToLinTrcIndx(double inln, double crln, int& lin, int& trc) const
{
	if (outOfRange(inln, crln)) return false;

	int inlnIndx = (int)inlineIndex(inln);
	int crlnIndx = (int)crosslineIndex(crln);

	if (organizedByInline()) {
		lin = inlnIndx;
		trc = crlnIndx;
	}
	else {
		trc = inlnIndx;
		lin = crlnIndx;
	}
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool DataRange::linTrcIndxToInlnCrln(int lin, int trc, double& inln, double& crln) const
{
	if (organizedByInline()) {
		if (outOfRange(lin, trc)) return false;
		inln = getInline(lin);
		crln = getCrossline(trc);
	}
	else {
		if (outOfRange(trc, lin)) return false;
		inln = getInline(trc);
		crln = getCrossline(lin);
	}
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Return:  Index of sample nearest to given time. If input time is out of
//		the time range, then returns 0 or nz-1. TODO: Return -1 if out of range?
// Note:  Checks to ensure that input time lies within the range.
//-----------------------------------------------------------------------------
int64_t DataRange::timeIndex(double time) const
{
   if (_dsmp > 0.0) {
      if (time < _timeMin) return 0;
      if (time > _timeMax) return (int64_t)(_numSmps - 1);
      return (int64_t)(0.5 + (double)(time - _timeMin)/_dsmp);
   }
	else return -1;   // error
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Methods to handle X-Y positions and relate these to inline, crossline numbers
//		x0, y0 is X-Y position at inln/crln origin (0, 0)
//		a = dx/dI			b = dx/dC
//		c = dy/dI			d = dy/dC
//------------------------------------------------------------------------------
void DataRange::setInlnCrlnToXyCoeff(IC_to_XY coef) 
{
	_coef = coef;
	// Set dlin, dtrc
	setDInline(sqrt(pow(_inlnInc*coef.dXdI, 2) + pow(_inlnInc*coef.dYdI, 2)));
	setDCrossline(sqrt(pow(_crlnInc*coef.dXdC, 2) + pow(_crlnInc*coef.dYdC, 2)));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
double DataRange::xLocation(double inln, double crln) const
{
	return _coef.xref + _coef.dXdI*(inln - _coef.inlnRef) + _coef.dXdC*(crln - _coef.crlnRef);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
double DataRange::yLocation(double inln, double crln) const
{
	return _coef.yref + _coef.dYdI*(inln - _coef.inlnRef) + _coef.dYdC*(crln - _coef.crlnRef);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Does not include data name, as this name usually reported separately
//------------------------------------------------------------------------------
string DataRange::toString() const
{
	char temp[120];
	string info = description();
	snprintf(temp,119, "\n   Total number of traces:  %d", totalNumTraces());
	info += temp;
	if (organizedByInline()) info += "\n   Organized by inline";
	else info += "\n   Organized by crossline";
	snprintf(temp, 119, "\n   Inlines:    %d  to  %d,  increment %4.1f  (%d lines)",
		(int)_inlnMin, (int)_inlnMax, _inlnInc, (int)_numInln);
	info += temp;
	snprintf(temp, 119, "\n   Crosslines: %d  to  %d,  increment %4.1f  (%d lines)",
		(int)_crlnMin, (int)_crlnMax, _crlnInc, (int)_numCrln);
	info += temp;
	
	char* unitStr;
	if (isMetric()) unitStr = "meters";
	else unitStr = "feet";

	if (domain() == TIME_DOMAIN) {
		snprintf(temp, 119, "\n   Times:  %3.1f", 1000.0*_timeMin);
		info += temp;
		snprintf(temp, 119, "  to  %3.1f ms", 1000.0*_timeMax);
	}
	else if (domain() == DEPTH_DOMAIN) {
		snprintf(temp, 119, "\n   Depths:  %3.1f", 1000.0*_timeMin);
		info += temp;
		snprintf(temp, 119, "  to  %3.1f %s", 1000.0*_timeMax, unitStr);
	}
	else {	// assume an attribute domain
		snprintf(temp, 119, "\n   Attribute Values:  %3.1f", _timeMin);
		info += temp;
		snprintf(temp, 119, "  to  %3.1f", _timeMax);
	}
	info += temp;
	snprintf(temp, 119, "  (%d samples)", (int)_numSmps); info += temp;
	snprintf(temp, 119, "\n   Dtrc: %3.1f %s,  Dlin: %3.1f %s,  Dsmp: %3.1f ms",
		_dtrc, unitStr, _dlin, unitStr, 1000.0*_dsmp); info += temp;
	
	return info;
}
//------------------------------------------------------------------------------
//==============================================================================

}  // end namespace PagosaGeo
