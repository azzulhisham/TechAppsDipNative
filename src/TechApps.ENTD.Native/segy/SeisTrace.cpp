//===============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  SeisTrace
// Purpose:  Seismic trace data array, headers, and methods
//===============================================================================

#include <algorithm>
#include <math.h>
#include "SeisTrace.h"
#include "SegyUtil.h"

using std::min;
using std::max;
using std::abs;


namespace PagosaGeo {

//=============================================================================
// "Friends" of SeisTrace
//-----------------------------------------------------------------------------
// Notes:  multiply a trace with a float scalar to produce a third trace
//-----------------------------------------------------------------------------
SeisTrace operator*(float scalar, const SeisTrace& trc)
{
  SeisTrace temp(trc);
  return temp *= scalar;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Notes:  add a trace with a float value to produce a third trace
//-------------------------------------------------------------------------------
SeisTrace operator+(float value, const SeisTrace& trc)
{
 SeisTrace temp(trc);
  return temp += value;
}
//-------------------------------------------------------------------------------
//===============================================================================


//===============================================================================
// Default constructor
//-------------------------------------------------------------------------------
SeisTrace::SeisTrace(int nval, float ds, float ilin, float xlin,
  eDomain dom, bool state) : SeisData(dom, state), _startTime(0.0)
{
	setNumSmps(nval);
	setDSample(ds);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
SeisTrace::SeisTrace(const SeisTrace& st) : SeisData()
{
	*this = st;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Note:  This method does not compare values, just sizes and parameters.
//-------------------------------------------------------------------------------
bool SeisTrace::operator==(const SeisTrace& st) const
{
	if (!SeisData::operator==(st)) return false;
	if (numSmps() != st.numSmps()) return false;
	if (_dsmp != st._dsmp || _startTime != st._startTime) return false;
	if (_firstLiveSmp != st._firstLiveSmp ||
		_lastLiveSmp != st._lastLiveSmp) return false;
///	if (_hdrs != st._hdrs) return false;
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Make exact copy of input trace
//-------------------------------------------------------------------------------
SeisTrace& SeisTrace::operator=(const SeisTrace& st)
{
	if (&st == this) return *this;     // prevent assignment to self
	SeisData::operator=(st);
	setValues(st);	// sets state
	setHeaders(st._hdrs);
	_startTime = st._startTime;
   _dsmp = st._dsmp;
	_firstLiveSmp = st._firstLiveSmp;
	_lastLiveSmp = st._lastLiveSmp;
	return *this;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Overloaded Math Ops
//-------------------------------------------------------------------------------
// Purpose:  Adds two traces together to produce third trace
//-------------------------------------------------------------------------------
SeisTrace SeisTrace::operator+(const SeisTrace &trc) const
{
	SeisTrace temp(*this);
	return temp += trc;
}
//-------------------------------------------------------------------------------
 
//-------------------------------------------------------------------------------
// Purpose:  Add a trace to this trace
//-------------------------------------------------------------------------------
SeisTrace& SeisTrace::operator+=(const SeisTrace &trc)
{
   int nvals = min(numSmps(), (int)trc.numSmps());
	for (int i=0; i<nvals; i++) _vals[i] += trc._vals[i];
   return *this;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Add constant to trace.
//-------------------------------------------------------------------------------
SeisTrace& SeisTrace::operator+=(float scalar) 
{
	_vals += scalar;
	return *this;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Add constant to trace to produce new trace.
//-------------------------------------------------------------------------------
SeisTrace SeisTrace::operator+(float scalar) const
{
	SeisTrace temp(*this);
	return temp += scalar;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Subtracts a seismic trace from this trace to produce third trace
//-------------------------------------------------------------------------------
SeisTrace SeisTrace::operator-(const SeisTrace &trc) const
{
	SeisTrace temp(*this);
	return temp -= trc;
}
//-------------------------------------------------------------------------------
 
//-------------------------------------------------------------------------------
// Purpose:  Subtract a trace from this trace
//-------------------------------------------------------------------------------
SeisTrace& SeisTrace::operator-=(const SeisTrace &trc)
{
   int nvals = min(numSmps(), (int)trc.numSmps());
	for (int i=0; i<nvals; i++) _vals[i] -= trc._vals[i];
   return *this;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Subtract scalar from this trace.
//-------------------------------------------------------------------------------
SeisTrace& SeisTrace::operator-=(float scalar) 
{
	_vals -= scalar;
	return *this;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Subtract scalar from trace to produce new trace.
//-------------------------------------------------------------------------------
SeisTrace SeisTrace::operator-(float scalar) const
{
	SeisTrace temp(*this);
	return temp -= scalar;
}
//-------------------------------------------------------------------------------
 
//-------------------------------------------------------------------------------
// Purpose:  Multiply this trace by a given trace to modify this trace
//-------------------------------------------------------------------------------
SeisTrace& SeisTrace::operator*=(const SeisTrace &trc)
{
	if (trc.isDead()) kill();
	else for (int i=0; i<numSmps(); i++) _vals[i] *= trc._vals[i];
	autoSetState();
   return *this;
}
//-------------------------------------------------------------------------------
 
//-------------------------------------------------------------------------------
// Purpose:  Multiply this trace by a given trace to produce a third trace
//-------------------------------------------------------------------------------
SeisTrace SeisTrace::operator*(const SeisTrace &trc) const
{
	SeisTrace temp(*this);
	return temp *= trc;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:   Multiply a trace with a float scalar to produce a third trace
//-------------------------------------------------------------------------------
SeisTrace& SeisTrace::operator*=(float scalar) 
{
 	_vals *= scalar;
	return *this;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  multiply a trace with a float scalar to produce a third trace
//-------------------------------------------------------------------------------
SeisTrace SeisTrace::operator*(float scalar) const
{
	SeisTrace temp(*this);
	return temp *= scalar;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:   Divide this trace by a given trace, return this. Slow!
//   Ensures values to zero outside live zone.
//-------------------------------------------------------------------------------
SeisTrace& SeisTrace::operator/=(const SeisTrace &trc) 
{
	if (isDead()) return *this;
	else if (trc.isDead()) { kill();   return *this; }
	int istart = max(firstLiveSmp(), trc.firstLiveSmp());
	int iend = min(lastLiveSmp(), trc.lastLiveSmp());
	int i;
	for (i=0; i<istart; i++) _vals[i] = 0.0;
	for (i=istart; i<=iend; i++) {
		if (trc[i] == 0.0f) _vals[i] = 0.0f;		// prevent divide by zero
		else _vals[i] /= trc[i];
	}
	for (i=iend+1; i<numSmps(); i++) _vals[i] = 0.0;
	autoSetState();
	return *this;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Divide this trace by a given trace to produce a third trace
//-------------------------------------------------------------------------------
SeisTrace SeisTrace::operator/(const SeisTrace &trc) const
{
	SeisTrace temp(*this);
	return temp /= trc;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:   Divide a trace by a float scalar to produce a third trace
//-------------------------------------------------------------------------------
SeisTrace& SeisTrace::operator/=(float scalar) 
{
	if (scalar == 0.0f) return *this;    // should throw exception
	else return operator*=(1.0f/scalar); // multiplication is faster than division
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  multiply a trace with a float scalar to produce a third trace
//-------------------------------------------------------------------------------
SeisTrace SeisTrace::operator/(float scalar) const
{
	SeisTrace temp(*this);
	if (scalar == 0.0f) return temp;    // should throw exception
	else return temp *= 1.0f/scalar;    // multiplication is faster than division
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Sets
//-------------------------------------------------------------------------------
// Purpose: sets the size of the _vals array, sets all values to 0
// Return: true if no problems, false if a problem
// Notes: always clears but does not set state to dead unless size is changed to 0
//   Resets SEGY header # of samples also
//-------------------------------------------------------------------------------
bool SeisTrace::setNumSmps(int nsmp)
{
	if (numSmps() != nsmp) {
		_vals.resize(nsmp);
	}
	clear();  // set all values to zero, leave headers unchanged
	_hdrs._number_samples = nsmp;		// insurance
	if (nsmp == 0) setLive(false);
	return (numSmps() == nsmp);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Notes: Sets the sample period, which must be positive.
// Returns true if sample period changed as requested, false otherwise.
//-------------------------------------------------------------------------------
bool SeisTrace::setDSample(float sampleInterval) 
{
	_dsmp = (sampleInterval > 0.0f) ? sampleInterval : 1.0f;
	_hdrs._sample_interval = (short int)(_dsmp*1000000.0f);
	return (_dsmp == sampleInterval);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Set trace values equal to input trc. Automatically reset size to be
//    same as input if needed.
// Return:  is live trace, true or false
//-------------------------------------------------------------------------------
bool SeisTrace::setValues(const SeisTrace& trc, bool canResize)
{
	if (&trc == this) return isLive();     // prevent assignment to self
	return setValues(trc._vals, canResize);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Set trace values equal to input trc. Automatically reset size to be
//    same as input if needed.
// Return:  is live trace, true or false
//-------------------------------------------------------------------------------
bool SeisTrace::setValues(const valarray<float>& trc, bool canResize)
{
	if ((int)trc.size() == numSmps()) _vals = trc;
	else if (canResize) {
		setSize((int)trc.size());
		_vals = trc;
	}
	else {
		int nz = min(numSmps(), (int)trc.size());
		for (int i=0; i<nz; i++) _vals[i] = trc[i];
		for (int i=nz; i<numSmps(); i++) _vals[i] = 0.0f; // insurance
	}
	return autoSetState();
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose: Equate trace values to input array
//-------------------------------------------------------------------------------
template <class T>
bool SeisTrace::setTraceValues(const T trc, int nvals, bool canResize)
{
	if (nvals != numSmps() && canResize) setSize(nvals);
	int nz = min(numSmps(), nvals);
	for (int i = 0; i<nz; i++) _vals[i] = (float)(trc[i]);
	for (int i = nz; i<numSmps(); i++) _vals[i] = 0.0f; // insurance
	return autoSetState();
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  stores the trace values. Automatically resets size if needed by
//   default; otherwise user must select canResize = false and it will set the
//   values as well as it can.
// Return:  is live trace, true or false.
//-------------------------------------------------------------------------------
bool SeisTrace::setValues(const float* trc, int nvals, bool canResize)
{
	return setTraceValues<const float*>(trc, nvals, canResize);
}
//-------------------------------------------------------------------------------
bool SeisTrace::setValues(const int* trc, int nvals, bool canResize)
{
	return setTraceValues<const int*>(trc, nvals, canResize);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Set SEGY trace headers; used in SegyRead
// Notes:  honors this trace's number of samples, resets all else
// Return:  isLive()
//-------------------------------------------------------------------------------
bool SeisTrace::setHeaders(const SeisTrace& trc)
{
	return setHeaders(trc.getHeaders());
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Set SEGY trace headers; used in SegyRead
// Notes:  honors this trace's number of samples, copies all else
// Return:  isLive()
//-------------------------------------------------------------------------------
bool SeisTrace::setHeaders(const SegyTraceHeader& inHdrs)
{  
	_hdrs = inHdrs;
	_hdrs._number_samples = numSmps();  // insurance
//	setStartTime((float)_hdrs._recording_delay/1000.0f + 0.5f);
	return isLive();
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  sets the time of the first sample in the _vals array; units are ms
// Notes:  Eventually may want to do something about non-integral times
//-------------------------------------------------------------------------------
bool SeisTrace::setStartTime(float tzero)
{
	_startTime = tzero;
	_hdrs._recording_delay = (short int)(1000.0*_startTime + 0.5);  // WARNING! assumes int start time in ms
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  sets the first nonzero sample, clears all samples before this
// Return: true if valid input, false otherwise (kills in this case)
// Note:  This is essentially a top mute method.  Resets state to "live" if valid
//   index, and to "dead" if invalid index.
//-------------------------------------------------------------------------------
bool SeisTrace::setFirstLiveSmp(int first)
{
	if (first >= numSmps()) { kill();   return false; }
	setLive();
	_firstLiveSmp = first;
	for (int i=0; i<_firstLiveSmp; i++) _vals[i] = 0.0f;
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  sets the last nonzero sample, clears all samples after this
// Return: true if valid sample, false if too large (sets to end of trace)
// Note:  Does not reset state.
//-------------------------------------------------------------------------------
bool SeisTrace::setLastLiveSmp(int last)
{
	bool status = true;
	if (last >= numSmps()) { _lastLiveSmp = numSmps() - 1;   status = false; }
	else _lastLiveSmp = last;
	for (int i=numSmps()-1; i>_lastLiveSmp; i--) _vals[i] = 0.0f;
	return status;
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Utility Methods
//-------------------------------------------------------------------------------
// Purpose:  puts the trace values into a float array
// Notes:  Always tries to fill the float array, but never more than nsamps.
//   Ensures that the end of the trace gets filled with zeros if longer than
//   this SeisTrace.
//-------------------------------------------------------------------------------
void SeisTrace::getValues(float* trc, int nsamps) const
{
	int nz = min(nsamps, numSmps());
	for (int i=0; i<nz; i++) trc[i] = _vals[i];
	for (int i=nz; i<nsamps; i++) trc[i] = 0.0f;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Put trace values into a valarray<float>
// Notes:  Always tries to fill the float array, but never more than numSmps().
//   Ensure end of valarray gets filled with zeros if longer than this SeisTrace.
//-------------------------------------------------------------------------------
void SeisTrace::getValues(valarray<float> &trc) const
{
	int nz = min((int)trc.size(), numSmps());
	for (int i=0; i<nz; i++) trc[i] = _vals[i];
	for (int i=nz; i<(int)trc.size(); i++) trc[i] = 0.0f;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Miscellaneous Methods
//-------------------------------------------------------------------------------
void SeisTrace::resetMute()
{
	for (int i=0; i<_firstLiveSmp; i++) _vals[i] = 0.0f;
	for (int i=_lastLiveSmp+1; i<numSmps(); i++) _vals[i] = 0.0f;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose: regardless of trace state, determine if trace has all zeros
//-------------------------------------------------------------------------------
bool SeisTrace::allZero() const
{
	for (int i = 0; i < numSmps(); i++) if (_vals[i] != 0.0f) return false;
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose: compute & return the sum of all trace values
//-------------------------------------------------------------------------------
float SeisTrace::sum() const
{
	if (isDead()) return 0.0f;
	else return _vals.sum();
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  compute & return the average value over live portion of trace
//-------------------------------------------------------------------------------
float SeisTrace::averageValue() const
{
	if (isDead() || numSmps()==0) return 0.0f;
	int istart = firstLiveSmp();
	int iend = lastLiveSmp();
	if (iend < istart) return 0.0f;
	float sum = 0.0;
	for (int i=istart; i<=iend; i++) sum += _vals[i];
	return sum/(float)(iend - istart + 1);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  subtract average value from all live samples, return average value
//-------------------------------------------------------------------------------
float SeisTrace::removeDC()
{
	if (isDead()) return 0.0f;
	float avgVal = averageValue();
	if (avgVal == 0.0f) return 0.0f; 
	else {
		for (int i=_firstLiveSmp; i<=_lastLiveSmp; i++) _vals[i] -= avgVal;
		return avgVal;  // live trace
	}
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  reverse polarity of all trace values
//-------------------------------------------------------------------------------
bool SeisTrace::reversePolarity()
{
	if (isDead()) return false;
	_vals *= -1.0f;
///	for (int i=_firstLiveSmp; i<=_lastLiveSmp; i++) _vals[i] *= -1.0f;
	return true;  // live trace
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  take absolute value of all values of this trace
//-------------------------------------------------------------------------------
bool SeisTrace::absoluteValue()
{
	if (isDead()) return false;
	for (int i=_firstLiveSmp; i<=_lastLiveSmp; i++) _vals[i] = fabs(_vals[i]);
	return true;  // live trace
}
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
// Other vector operations
//-------------------------------------------------------------------------------
// Notes:  Multiply this trace with a given trace to get the dot product
//-------------------------------------------------------------------------------
float SeisTrace::dotProduct(const valarray<float> &trc) const
{
	if (isDead()) return 0.0f;
	int nv = min(numSmps(), (int)trc.size());
	float dotProd = 0.0f;
	for (int i=0; i<nv; i++) dotProd += _vals[i]*trc[i];
	return dotProd;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Compute zero-lag correlation coefficient between this trace and input.
//   Computed as the dot product normalized by the product of the trace lengths.
//   Ignore requirement of zero mean vectors.
//-------------------------------------------------------------------------------
float SeisTrace::corrCoef(const SeisTrace &trc) const
{
	if (isDead()) return 0.0f;
	float scalar = vecLength()*trc.vecLength();
	if (scalar > 1.0e-10f) return dotProduct(trc)/scalar;
	else return 0.0f;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Multiply this trace with itself to get the trace energy. Returns
// energy if live, 0.0 if dead.
//-------------------------------------------------------------------------------
float SeisTrace::energy() const
{ 
	if (isDead()) return 0.0f;
	float eng = 0.0f;
	for (int i=firstLiveSmp(); i<=lastLiveSmp(); i++) eng += _vals[i]*_vals[i];
	return eng;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Compute total energy divided by number of live samples. Equals
//   RMS amplitude squared.
// Return 0.0 if dead trace
//-------------------------------------------------------------------------------
float SeisTrace::averageEnergy() const
{ 
	if (isDead()) return 0.0f;
	return energy()/numLiveSmps();
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Divide this trace by its length. Returns length if live, 0.0 if dead.
//-------------------------------------------------------------------------------
float SeisTrace::normalize() 
{
	if (isDead()) return 0.0f;
	float len = sqrt(energy());
	if (len == 0.0) return 0.0f;
	*this /= len; 
	return len; 
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  set all elements in vector to zero, resets first, last nonzero samp
// Note:  does not change trace state (does not set to DEAD) and does not alter
//   headers.
//-------------------------------------------------------------------------------
bool SeisTrace::clear()
{
	_lastLiveSmp = 0;
	_firstLiveSmp = numSmps();
	if (numSmps() == 0) return false;
	_vals = 0.0f;
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  sets segy headers to default state
//-------------------------------------------------------------------------------
bool SeisTrace::clearHeaders()
{
	_hdrs.reset();
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  Set trace to live; also sets live sample range to entire trace.
//		In general, better to call method autoSetState(), which sets first and 
//		last live samples to correct range.
//-------------------------------------------------------------------------------
void SeisTrace::setLive(bool set)
{
	SeisData::setLive(set);
	if (isLive()) {
		_firstLiveSmp = 0;
		_lastLiveSmp = numSmps()-1;
	}
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  set the state as live or dead assuming an all zero trace is dead;
//   also set the starting and ending mute indices. Does not change the state of
//   the segy trace header (if it even exists)
// Return:  true if some nonzero elements, false otherwise
//-------------------------------------------------------------------------------
bool SeisTrace::autoSetState()
{
	int nz = numSmps();

	if (nz == 0) {
		setLive(false);
		_firstLiveSmp = 0;
		_lastLiveSmp = 0;
	}
	else {
		setLive(true);		// sets first, last live samples to full trace range
		int i = 0;
		while (i<nz && _vals[i]==0.0f) i++;
		_firstLiveSmp = i;
		if (_firstLiveSmp == nz) {
			setLive(false);
			_lastLiveSmp = 0;
		}
		else {
			i = nz - 1;
			while (i>_firstLiveSmp && _vals[i]==0.0f) i--;
			_lastLiveSmp = i;
		}
	}
	return isLive();
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Utility Methods 
//-------------------------------------------------------------------------------
// Purpose:  given an input trace that is usually smaller than this trace,
//   fill its values with this trace's values. The filling begins at this trace's
//   index value = ioff and continues as long as possible. This index can be
//   negative, which facilitates certain windowing operations. For negative
//   indices, 0 is written to trc.
// Return: true if live data copied, false if trc is dead.
//-------------------------------------------------------------------------------
bool SeisTrace::subset(SeisTrace& trc, int ioff) const
{
	// reset the input trace start time to reflect the offset
	trc.setDSample(dSample());  // insurance in case not already set
	trc.setStartTime(_startTime + ioff*_dsmp);
	subset(trc._vals, ioff);
	return trc.autoSetState();
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  given an input trace that is usually smaller than this trace,
//   fill its values with this trace's values. The filling begins at this trace's
//   index value = ioff and continues as long as possible. This index can be
//   negative, which facilitates certain windowing operations. For negative
//   indices, 0 is written to trc.
// Return: true if trc has nonzero data, false if trc is all zero
//-------------------------------------------------------------------------------
bool SeisTrace::subset(valarray<float>& trc, int ioff) const
{
	int nz = (int)trc.size();
	int istart = max(0, -ioff);                        // starting index in trc to copy to
	int iend = min(numSmps()-ioff, nz) - 1;				// ending index in trc to copy to
	if (isDead() || istart > iend) {
		for (int i=0; i<nz; i++) trc[i] = 0.0;
		return false;
	}
	for (int i=0; i<istart; i++) trc[i] = 0.0f;             // kill values that do not overlap
	for (int i=istart; i<=iend; i++) trc[i] = _vals[i+ioff]; // copy values that overlap
	for (int i=iend+1; i<nz; i++) trc[i] = 0.0f;   // kill values that do not overlap

	for (int i=istart; i<=iend; i++) if (trc[i] != 0.0) return true;
	return false;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  merge data from input trace to this trace in overlap region,
//   which is usually a subset of input trace. Merging can be additive (add input
//   to this data) or replacement (replace this data with input data).
// Return: true if live data copied, false if trc is dead.
//-------------------------------------------------------------------------------
bool SeisTrace::merge(const SeisTrace& trc, int ioff, bool addToData)
{
	if (trc.isDead()) return false;
	int istart = max(0, -ioff);
	int iend = min(numSmps()-ioff, trc.numSmps());
	if (addToData) for (int i=istart; i<iend; i++) _vals[i+ioff] += trc[i];
	else for (int i=istart; i<iend; i++) _vals[i+ioff] = trc[i];  // replace
	return true;
}
//-------------------------------------------------------------------------------
//===============================================================================

}  // end namespace PagosaGeo
