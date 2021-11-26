//==============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyUtil
// Purpose:  Collection of static utility methods for handling SEGY data, 
//    including methods for byte swapping & IBM/IEEE float conversions.
// Methods modified from open-source code.
//==============================================================================

#ifndef SEGYUTIL_H
#define SEGYUTIL_H

#include <string>
#include "DataDefs.h"
#include "SegyFileHeader.h"

using std::string;


namespace PagosaGeo {

//==============================================================================
//------------------------------------------------------------------------------
class SegyUtil
{
// disallow construction, destruction, operator=
private:
  SegyUtil() {} // default constructor
  SegyUtil(const SegyUtil& util) {} // copy constructor does nothing
  virtual ~SegyUtil() {}

public:
// floating point conversion routines needed for segy
	static void IBMfloatToIntelFloat(float* vals, int istart, int iend);
	static void intelFloatToIBMfloat(float* vals, int istart, int iend);
	static void IBMfloatToIntelFloat(float &value);
	static void intelFloatToIBMfloat(float &value);

// byte swapping routines
	static void swapBytes(void* vals, int i1, int i2);
	static void swap2Bytes(short int &val);
	static void swap4Bytes(float &val);
	static void swap4Bytes(int &val);
	static void swap4Bytes(char* val);

	static void ASCII_to_EBCDIC (int, unsigned char*);
	static void EBCDIC_to_ASCII (int, unsigned char*);
  
// ENUM to String methods
	static string dataFormatToString(eDataFormat format);
	static string dataDomainToString(eDomain domain);
	static string distanceUnits(int units);
};
//==============================================================================

} // End namespace PagosaGeo

#endif
