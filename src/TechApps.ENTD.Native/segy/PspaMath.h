//===============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// File:  PspaMath 
// Purpose:  Set of simple math processes and defines
//===============================================================================


#ifndef PSPAMATH_H
#define PSPAMATH_H

#include <valarray>

using std::valarray;


namespace PagosaGeo {

#ifndef PI
#define PI 3.141592653589793
#endif
 
#ifndef VLARGE
#define VLARGE 1.0e20f
#endif
 
#ifndef VSMALL
#define VSMALL 1.0e-20f
#endif
  
#ifndef INV2PI  //   1/2pi
#define INV2PI 0.159154943091895
#endif

#ifndef DEGREES_PER_RADIAN
#define DEGREES_PER_RADIAN 57.29577951308232
#endif

#ifndef RADIANS_PER_DEGREE
#define RADIANS_PER_DEGREE 0.017453292519943
#endif


enum eAngleUnits {
	ANGLE_DEGREES = 1,
	ANGLE_RADIANS = 2,
	ANGLE_CYCLES = 3
};

enum eFrequencyUnits {
	FREQ_HERTZ = 1,
	FREQ_RAD_PER_SEC = 2
};


// General methods
double atanh(double x);
int sign(int n);
float sign(float x);                                                                     
double sign(double x);                                                                     
int ensureOdd(int n);
int nint(float x);
int nint(double x);

float interpolate(float x1, float x2, float x3, float tp);
bool exactPolyFit3pt(float x1, float x2, float x3, valarray<float>& polyCoef);
bool exactPolyFit4pt(float x1, float x2, float x3, float x4, valarray<float>& polyCoef);
bool exactPolyFit5pt(float x1, float x2, float x3, float x4, float x5, valarray<float>& polyCoef);
bool lsqPolyFitOrder2(float x1, float x2, float x3, float x4, float x5, valarray<float>& polyCoef);
float polynomialInterp(const valarray<float>& polyCoef, float xval);

// Methods for a const valarray of floats
float totalSum(const valarray<float>& trace);
float averageValue(const valarray<float>& trace);
float averageAbsValue(const valarray<float>& trace);
float energy(const valarray<float>& trace);
float energyHalftime(const valarray<float>& trace);
float averageEnergy(const valarray<float>& trace);
float rmsValue(const valarray<float>& trace);
float peakValue(const valarray<float>& pwr, const valarray<float>& atrb, int ipeak);
float maxPeakVal(const valarray<float>& vals);
float maxTroughVal(const valarray<float>& vals);
float avgPeakVal(const valarray<float>& vals);
float avgTroughVal(const valarray<float>& vals);
float standardDeviation(const valarray<float>& trace);
bool meanStdDev(const valarray<float>& trace, float& meanVal, float& stdDevVal); 
float correlationCoefficient(const valarray<float>& trace1, const valarray<float>& trace2); 
float dotProduct(const valarray<float>& trc1, const valarray<float>& trc2);
int mode(const valarray<int>& histogram);
int clip(int val, int minVal, int maxVal);

// Methods for a non-const valarray of floats
float normalize(valarray<float>& trace);


///#define SIGN(a) (((a) > 0.0) ? 1.0 : -1.0)

}  // end namespace PagosaGeo

#endif
