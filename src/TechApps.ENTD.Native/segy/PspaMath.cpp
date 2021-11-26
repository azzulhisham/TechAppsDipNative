//===============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// File:  PspaMath
// Purpose:  Simple math operations
//===============================================================================

#include <algorithm>
#include "PspaMath.h"

using std::min;


namespace PagosaGeo {

	int sign(int n) { return (n<0) ? -1 : 1; }
	float sign(float x) { return (x<0.0) ? -1.0f : 1.0f; }
	double sign(double x) { return (x<0.0) ? -1.0 : 1.0; }
	int ensureOdd(int n) { return 2*(n/2) + 1; }
   int nint(float x) { if (x>=0) return (int)(x + 0.5f); else return (int)(x - 0.5f); }
   int nint(double x) { if (x>=0) return (int)(x + 0.5); else return (int)(x - 0.5); }
	
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// Purpose: arc tangent function (missing in Microsoft VS 2010)
// Note:  Assumes abs(x) >= 1
//-------------------------------------------------------------------------------
double atanh(double x)
{
///   if (x > 1.0) return pi/2.0;
///   else if (x < -1.0) return -pi/2.0;
   return 0.5*(log(1.0 + x) - log(1.0 - x));
}
//-------------------------------------------------------------------------------
	
//-------------------------------------------------------------------------------
// Purpose: Given 3 sample values, x1, x2, x3, spaced evenly with integral
//   sample spacing, centered on t=0, interpolate to find value at fractional 
//   sample -0.5 < tp < 0.5.
//-------------------------------------------------------------------------------
float interpolate(float x1, float x2, float x3, float tp)
{
	float c0 = x2;
	float c1 = 0.5f*(x3 - x1);
	float c2 = 0.5f*(x1 + x3) - x2;
	return c0 + c1*tp + c2*tp*tp;
}
//-------------------------------------------------------------------------------
	
//-------------------------------------------------------------------------------
// Purpose: Given 3 sample values, x1, x2, x3, spaced evenly with integral
//   sample spacing, find exact fitting polynomial of order 2
//-------------------------------------------------------------------------------
bool exactPolyFit3pt(float x1, float x2, float x3, valarray<float>& polyCoef)
{
	if (polyCoef.size() != 3) return false;
	polyCoef[0] = x2;
	polyCoef[1] = 0.5f*(x3 - x1);
	polyCoef[2] = 0.5f*(x1 + x3) - x2;
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose: Given 4 sample values, x1, x2, x3, x4 spaced evenly with integral
//   sample spacing, centered on t=0, find exact fitting polynomial of order 3
//-------------------------------------------------------------------------------
bool exactPolyFit4pt(float x1, float x2, float x3, float x4, valarray<float>& polyCoef)
{
	if (polyCoef.size() != 4) return false;
	float s23 = x2 + x3;
	float s14 = x1 + x4;
	float d32 = x3 - x2;
	float d41 = x4 - x1;
	float scalar16 = 1.0f/16.0f;
	float scalar48 = 1.0f/48.0f;
	polyCoef[0] = scalar16*(9.0f*s23 - s14);
	polyCoef[1] = scalar48*(27.0f*d32 - d41);
	polyCoef[2] = scalar16*(s14 - s23);
	polyCoef[3] = scalar48*(d41 - 3.0f*d32);
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose: Given 5 sample values, x1, x2, x3, x4, x5 spaced evenly with integral
//   sample spacing, centered on t=0 (time of middle sample), find exact fitting 
//   polynomial of order 4.
//-------------------------------------------------------------------------------
bool exactPolyFit5pt(float x1, float x2, float x3, float x4, float x5, valarray<float>& polyCoef)
{
	if (polyCoef.size() != 5) return false;
	polyCoef[0] = x3;
	polyCoef[1] = (x1 - x5 + 8.0f*(x4 -x2))/12.0f;
	polyCoef[2] = (-x1 - x5 + 16.0f*(x2 + x4) - 30.0f*x3)/24.0f;
	polyCoef[3] = (x5 - x1 + 2.0f*(x2 - x4))/12.0f;
	polyCoef[4] = (x1 + x5 - 4.0f*(x2 + x4) + 6.0f*x3)/24.0f;
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose: Given 5 sample values, x1, x2, x3, x4, x5 spaced evenly with integral
//   sample spacing, centered on t=0 (time of middle sample), find polynomial
//   of order 2 that fits best in least-squares sense
//-------------------------------------------------------------------------------
bool lsqPolyFitOrder2(float x1, float x2, float x3, float x4, float x5, valarray<float>& polyCoef)
{
	if (polyCoef.size() != 3) return false;
	float b0 = 4.0f;
	float b2 = 10.0f;
	float denom = 36.0f;
	float d0 = x1 + x2 + x3 + x4 + x5;
	float d1 = 2.0f*(x5 - x1) - x2 + x4;
	float d2 = 4.0f*(x1 + x5) + x2 + x4;

	polyCoef[2] = (b0*d2 - b2*d0)/denom;
	polyCoef[1] = d1/b2;
	polyCoef[0] = (d0 - b2*polyCoef[2])/b0;

	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Given the coefficients of a polynomial in variable x, and given a specific 
//   value of x, xval (which could represent distance or time), return the value 
//   of the polynomial at xval.
//-------------------------------------------------------------------------------
float polynomialInterp(const valarray<float>& polyCoef, float xval)
{
	int len = (int)polyCoef.size();
	float interpVal = polyCoef[0];
	float x = 1.0f;
	for (int i=1; i<len; i++) {
		x *= xval;
		interpVal += polyCoef[i]*x;
	}
	return interpVal;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float totalSum(const valarray<float>& vals)
{
	int numSmps = (int)vals.size();
	if (numSmps < 1) return 0.0f;

	float sum = 0.0f;
	for (int i=0; i<numSmps; i++) {
		sum += vals[i];
	}
	return sum;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float averageValue(const valarray<float>& vals)
{
	float sum = totalSum(vals);
	if (sum == 0.0) return 0.0;
	return sum/(float)vals.size();
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float averageAbsValue(const valarray<float>& vals)
{
	int numSmps = (int)vals.size();
	if (numSmps < 1) return 0.0f;

	float sum = 0.0f;
	for (int i=0; i<numSmps; i++) {
		sum += fabs(vals[i]);
	}
	return sum/numSmps;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float energy(const valarray<float>& vals)
{
	int numSmps = (int)vals.size();
	if (numSmps < 1) return 0.0f;

	float sum = 0.0f;
	for (int i=0; i<numSmps; i++) {
		sum += vals[i]*vals[i];
	}
	return sum;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float energyHalftime(const valarray<float>& vals)
{
	int numSmps = (int)vals.size();
	if (numSmps < 1) return 0.0f;

	float firstMoment = 0.0f;
	for (int i=1; i<vals.size(); i++) {
		firstMoment += float(i)*vals[i]*vals[i];
	}
	float centerOfGravity = firstMoment/energy(vals);
	return 200.0f*centerOfGravity/(vals.size() - 1) - 100.0f;   // new formula
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float averageEnergy(const valarray<float>& vals)
{
	float total = energy(vals);
	return total/vals.size();
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float rmsValue(const valarray<float>& vals)
{
	float avg = averageEnergy(vals);
	if (avg == 0.0f) return 0.0f;
	return sqrt(avg);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose:  given closest index on trace "pwr" to peak value, interpolate
//   to find value at true peak between samples on trace atrb
// Notes:  this method also works to find the trough value (finds where slope = 0).
//   Currently assumes 3 atrb values fit quadratic exactly
// TODO: Redesign this method completely.
//-------------------------------------------------------------------------------
float peakValue(const valarray<float>& pwr, const valarray<float>& atrb, int ipeak)
{
	int nz = (int)min(pwr.size(), atrb.size());
	if (nz == 0) return 0.0;
	if (ipeak < 1) return atrb[0];
	if (ipeak > nz-2) return atrb[nz-1];

	// Find time tp at envelope peak
	float a1 = pwr[ipeak-1];
	float a2 = pwr[ipeak];
	float a3 = pwr[ipeak+1];
	float tp = 0.5f*(a1 - a3)/(a1 + a3 - 2.0f*a2);  // -c1/2c2	

	// Find attribute value at time tp
	a1 = atrb[ipeak-1];
	a2 = atrb[ipeak];
	a3 = atrb[ipeak+1];
	float c0 = a2;
	float c1 = 0.5f*(a3 - a1);
	float c2 = 0.5f*(a1 + a3) - a2;
	return c0 + c1*tp + c2*tp*tp;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float maxPeakVal(const valarray<float>& vals)
{
	int nz = (int)vals.size();

	// find index of largest value
	int pkIndx = 0;
	for (int i=1; i<nz; i++) {
		if (vals[i] > vals[pkIndx]) pkIndx = i;
	}

	return peakValue(vals, vals, pkIndx);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float maxTroughVal(const valarray<float>& vals)
{
	int nz = (int)vals.size();

	// find index of largest value
	int trIndx = 0;
	for (int i=1; i<nz; i++) {
		if (vals[i] < vals[trIndx]) trIndx = i;
	}

	return peakValue(vals, vals, trIndx);  // valid for troughs as well
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float avgPeakVal(const valarray<float>& vals)
{
	int nz = (int)vals.size();
	int numPos = 0;
	float sum = 0.0f;
	for (int i=0; i<nz; i++) {
		if (vals[i] > 0.0) {
			numPos++;
			sum += vals[i];
		}
	}

	return sum/(numPos + VSMALL);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float avgTroughVal(const valarray<float>& vals)
{
	int nz = (int)vals.size();
	int numNeg = 0;
	float sum = 0.0f;
	for (int i=0; i<nz; i++) {
		if (vals[i] < 0.0) {
			numNeg++;
			sum += vals[i];
		}
	}

	return sum/(numNeg + VSMALL);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float standardDeviation(const valarray<float>& vals)
{
	int numSmps = (int)vals.size();
	if (numSmps < 2) return 0.0;
	  
	float meanVal = 0.0;
	float stdDevVal = 0.0;
	meanStdDev(vals, meanVal, stdDevVal);
	return stdDevVal;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
bool meanStdDev(const valarray<float>& vals, float& meanVal, float& stdDevVal)
{
	int numSmps = (int)vals.size();
	if (numSmps < 2) return false;
	  
	meanVal = averageValue(vals);
	float variance = 0.0;
	for (int i=0; i<numSmps; i++) {
		float diff = vals[i] - meanVal;
		variance += diff*diff;
	}
	stdDevVal = sqrt(variance/numSmps);
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float correlationCoefficient(const valarray<float>& trace1, const valarray<float>& trace2)
{
	int numSmps = (int)min(trace1.size(), trace2.size());
	float mean1 = 0.0, mean2 = 0.0;
	float stdDev1 = 0.0, stdDev2 = 0.0;
	if (!meanStdDev(trace1, mean1, stdDev1)) return 0.0;
	if (!meanStdDev(trace2, mean2, stdDev2)) return 0.0;
	
	float corrCoef = 0.0;
	for (int i=0; i<numSmps; i++) {
		corrCoef += (trace1[i] - mean1)*(trace2[i] - mean2);
	}
	corrCoef /= stdDev1*stdDev2*(float)numSmps;
	return corrCoef;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
float dotProduct(const valarray<float>& trc1, const valarray<float>& trc2)
{
	int numSmps = (int)min(trc1.size(), trc2.size());
	float dotProd = 0.0;
	for (int i=0; i<numSmps; i++) dotProd += trc1[i]*trc2[i];
	return dotProd;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Notes:  Assume index = 0 refers to "dead" members
//-------------------------------------------------------------------------------
int mode(const valarray<int>& histogram)
{
	int numClasses = (int)histogram.size() + 1;
	if (numClasses < 2) return 0;
	  
	int indxHistMax = 1;
	for (int i=2; i<=numClasses; i++) {
		if (histogram[i] > histogram[indxHistMax]) indxHistMax = i;
	}
	return indxHistMax;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
int clip(int val, int minVal, int maxVal)
{
	if (val < minVal) return minVal;
	else if (val > maxVal) return maxVal;
	else return val;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Methods for non-const valarray of floats
//-------------------------------------------------------------------------------
float normalize(valarray<float>& vals)
{
	float sum = totalSum(vals);
	if (sum == 0.0) return 0.0;

	int numSmps = (int)vals.size();
	for (int i=0; i<numSmps; i++) vals[i] /= sum;
	return sum;
}
//-------------------------------------------------------------------------------
//===============================================================================

}  // end namespace PagosaGeo
