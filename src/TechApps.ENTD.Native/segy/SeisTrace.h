//===============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  SeisTrace
// Purpose:  Data container for traces with values, headers, and methods
// Notes:
//     Convention for "live" vs. "dead" traces:
//     Live traces contain valid headers and data values (which could be all zero)
//     Dead traces contain valid headers but data values must be zero
//     "Dead" traces are not used by processes. The "autoSetState" method sets 
//     the _isLive flag automatically.
// 
// A "live" trace has at least 1 nonzero value; a "dead" trace has all zero
// values. This is the theory, but in practice the values can change without the
// state changing. Only autoSetState() and setLive(), setDead() change the
// state. Note setDead() clears the trace, whereas setLive() only sets the state
// to live -- even if all values are zero.
//===============================================================================

#ifndef SEISTRACE_H
#define SEISTRACE_H

#include <valarray>
#include "SeisData.h"
#include "SegyTraceHeader.h"     // for segy trace header class

using std::valarray;


namespace PagosaGeo {

//===============================================================================
// There are several methods for copying. The basic method is operator=, which
// makes this trace exactly the same as the copied trace (except for pointers).
// There is the setValues() method, which copies only values from the input to
// this trace, and which will resize if needed. There is setHeaders(), which
// copies the trace parameters (i.e., "headers"). Finally, there is subset
// which will not reset the size of values, but will reset all headers except
// _start_time.
//-------------------------------------------------------------------------------
class SeisTrace : public SeisData
{
friend SeisTrace operator*(float scalar, const SeisTrace& trc);
friend SeisTrace operator+(float scalar, const SeisTrace& trc);

public:
	SeisTrace(int nsmp = 0, float dsamp = 1.0f, float ilin = 1, float xlin = 1,
		eDomain dom = TIME_DOMAIN, bool live = true);
	SeisTrace(const SeisTrace& st);
	virtual ~SeisTrace() {}
	// standard overloaded operators
	SeisTrace& operator=(const SeisTrace& st);
	float& operator[](int i) { return _vals[i]; }
	float operator[](int i) const { return _vals[i]; }
	bool operator==(const SeisTrace& td) const;
	bool operator!=(const SeisTrace& td) const { return !operator==(td); }

	// overloaded math operators
	SeisTrace& operator+=(const SeisTrace &td);
	SeisTrace  operator+(const SeisTrace &td) const;
	SeisTrace& operator+=(float scalar);
	SeisTrace  operator+(float scalar) const;
	SeisTrace& operator-=(const SeisTrace &td);
	SeisTrace  operator-(const SeisTrace &td) const;
	SeisTrace& operator-=(float scalar);
	SeisTrace  operator-(float scalar) const;
	SeisTrace& operator*=(const SeisTrace &td);
	SeisTrace  operator*(const SeisTrace &td) const;
	SeisTrace& operator*=(float scalar);
	SeisTrace  operator*(float scalar) const;
	SeisTrace& operator/=(const SeisTrace &st);  // slow but safe -- assumes possible zero values
	SeisTrace  operator/(const SeisTrace &st) const;
	SeisTrace& operator/=(float scalar);
	SeisTrace  operator/(float scalar) const;

	// Sets
	bool setNumSmps(int nvals);
	bool setSize(int nvals) { return setNumSmps(nvals); }
	bool setValues(const SeisTrace& trc, bool canResize = true);  // resizes this trace to input length
	bool setValues(const valarray<float>& trc, bool canResize = true);
	template <class T>
	bool setTraceValues(const T trc, int nvals, bool canResize = true);
	bool setValues(const float* trc, int nvals, bool canResize = true);
	bool setValues(const int* trc, int nvals, bool canResize = true);
	bool setHeaders(const SeisTrace& trc); // does not resize
	bool setHeaders(const SegyTraceHeader& hdrs); // does not resize
	bool setDSample(float dsmp);                  // sample period in sec or m
	bool setStartTime(float tzero);
	bool setFirstLiveSmp(int firstLive);  // top mute
	bool setLastLiveSmp(int lastLive);	// bottom mute
	virtual void setLive(bool live = true);
	bool autoSetState();
	bool clear();         // set trace values to 0 but leave state and headers unchanged
	bool clearHeaders();  // reset headers to default state, leave state unchanged
	bool clearAll() { clearHeaders(); return clear(); }
	void kill() { clear();  _isLive = false; }

	// Gets
	int numSmps() const { return (int)_vals.size(); }
	void getValues(float* trc, int nvals) const;  // copies _vals into trc
	void getValues(valarray<float> &trc) const;  // copies _vals into trc
	const SegyTraceHeader& getHeaders() const { return _hdrs; }
	double inln() const { return (double)_hdrs._inline_num; }
	double crln() const { return (double)_hdrs._crossline_num; }
	int seqNum() const { return _hdrs._trace_number_in_line; }
	float startTime() const { return _startTime; }  // sec
	float endTime() const { return _startTime + _dsmp*(numSmps() - 1); }  // sec
	float timeLength() const { return endTime() - startTime(); }
	float dSample() const { return _dsmp; }  // sample period in sec or m
	int firstLiveSmp() const { return _firstLiveSmp; }
	int lastLiveSmp() const { return _lastLiveSmp; }
	int numLiveSmps() const { return (isLive()) ? _lastLiveSmp - _firstLiveSmp + 1 : 0; }

	// Utility methods
   void resetMute(); // resets all values in top, bottom mute zones to zero
	bool allZero() const;
	float sum() const;
	float averageValue() const;
	float rmsValue() const { return sqrt(averageEnergy()); }
	float removeDC();
	bool reversePolarity();
	bool absoluteValue();
	float dotProduct(const valarray<float> &st) const;
	float dotProduct(const SeisTrace &st) const { return dotProduct(st._vals); }
	float corrCoef(const SeisTrace &st) const;
	float energy() const;
	float averageEnergy() const;
	float vecLength() const { return sqrt(energy()); }
	float normalize();
	bool subset(SeisTrace& trc, int ioff) const;         // copies trace data & headers, auto sets state
	bool subset(valarray<float>& trc, int ioff) const;
	bool merge(const SeisTrace& trc, int ioff, bool addToData = true);
	bool sameSize(const SeisTrace& trc) const { 
		if (trc.numSmps() == numSmps()) return true; else return false; }

private:
	valarray<float> _vals;                 // trace values
	SegyTraceHeader _hdrs;                 // Segy trace header, 240 bytes
	float _dsmp;                           // sample period (units are sec or m)
	float _startTime;                      // time of first sample in ms
	int _firstLiveSmp;                     // top mute zone above this sample
	int _lastLiveSmp;                      // bottom mute zone below this sample
};
//===============================================================================

}  // end namespace PagosaGeo

#endif
