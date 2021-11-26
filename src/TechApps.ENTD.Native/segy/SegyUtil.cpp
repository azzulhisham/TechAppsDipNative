//==============================================================================
// PSPA: Poststack Seismic Processing and Analysis (� Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyUtil
// Purpose: Utility methods for reading and writing seismic data files
//==============================================================================

#include <string.h>
#include <stdlib.h>
#include "SegyUtil.h"
#include "DataDefs.h"  // for distance units defines


namespace PagosaGeo {

static unsigned char ASCII_translate_EBCDIC[256] =
{
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
            0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D, 0x4D,
            0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,
            0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
            0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,
            0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8,
            0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,
            0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
            0xE8, 0xE9, 0xAD, 0xE0, 0xBD, 0x5F, 0x6D,
            0x7D, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
            0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
            0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
            0xA8, 0xA9, 0xC0, 0x6A, 0xD0, 0xA1, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B,
            0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B
};

static unsigned char EBCDIC_translate_ASCII[256] =
{
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
            0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
            0x2E, 0x2E, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
            0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x2E, 0x3F,
            0x20, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x2E, 0x2E, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,
            0x26, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x2E, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
            0x2D, 0x2F, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x2E, 0x7C, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
            0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x2E, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
            0x2E, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
            0x69, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x2E, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
            0x72, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x2E, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
            0x7A, 0x2E, 0x2E, 0x2E, 0x5B, 0x2E, 0x2E,
            0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x2E, 0x2E, 0x2E, 0x2E, 0x5D, 0x2E, 0x2E,
            0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
            0x49, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51,
            0x52, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x5C, 0x2E, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
            0x5A, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
            0x39, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E 
};



//==============================================================================
//------------------------------------------------------------------------------
// Static Methods:  IBM <--> Intel conversion routines
// Notes:  istart is starting index to convert, iend is ending index to convert
//------------------------------------------------------------------------------
void SegyUtil::IBMfloatToIntelFloat(float* vals, int istart, int iend)
{
	for (int i=istart; i<=iend; i++) IBMfloatToIntelFloat(vals[i]);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SegyUtil::intelFloatToIBMfloat(float* vals, int istart, int iend)
{
	for (int i=istart; i<=iend; i++) intelFloatToIBMfloat(vals[i]);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Big-endian <--> little-endian byte swapping for unix to linux
//   conversions or vice versa.  Sufficient for integer formats and INTEL <--> IEEE
//   floats. For IBM to Intel float conversions, use IBMtoIntel or IntelToIBM 
//   for byte swapping plus float conversion.
//------------------------------------------------------------------------------
void SegyUtil::swapBytes(void* vals, int istart, int iend)
{
   int* data = (int*)vals;
	for (int i=istart; i<=iend; i++) swap4Bytes(data[i]);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Convert IBM compatible floating point numbers to Intel IEEE format.
//   Must byte-reverse the numbers as required by Intel chips.
// References:  Pullan (GEO 1990 1260-1271); Bennett (GEO 1990, 1272-1284)
//------------------------------------------------------------------------------
void SegyUtil::IBMfloatToIntelFloat(float &value)
{
  swap4Bytes(value); // reverse bytes for PC; data from workstation
  unsigned int *umantis = (unsigned int*)&value;
  int *mantis = (int*)&value;

// Start extracting information from number.
  unsigned char expp = *mantis >> 24;  // get exponent from upper byte
  *mantis = (*mantis) << 8;            // shift off upper byte
  int shift = 1;                       // set a counter to 1
// Shift until a 1 in msb (most significant bit)
  while((*mantis > 0) & (shift < 23)) {
    *mantis = *mantis << 1;
    shift++;
  }
  *mantis = *mantis << 1;  // need one more shift to get implied one bit
  unsigned char sign = expp & 0x80;  // set sign to msb of exponent
  expp = expp & 0x7f;      // kill sign bit
  if (expp) {              // don't do anymore if zero exponent
    expp -= 64;            // compute what shift was (old exponent)
    *umantis = *umantis >> 9;     // MOST IMPORTANT an UNSIGNED shift back down.
    unsigned int expll = 0x7f + (expp*4 - shift); // add in excess 127
// The mantissa is now correctly aligned.  Create the other two parts.
// Needed the extended sign word and the explonent word
    expll = expll << 23;      		// shift exponent up
// Combine them into a floating point IEEE format.
    if (sign) *umantis = expll | *mantis | 0x80000000;
    else *umantis = expll | *mantis; // set or don't set sign bit
  }
}  // Value converted to IEEE floating point format and byte-swapped
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  Convert Intel IEEE floating point numbers to IBM-ANSI floating
//   numbers.  Must byte reverse the numbers as required by Intel chips.
//   Modified from Bennett, 1990, GEO p. 1283.
// Notes:
// (1) This method assumes that the data came from a PC and needs to go to
//     a workstation.
// (2) I had considerable difficulty with Bennett's version -- always gave
//     errors for some values.  A CSM version also failed.  Finally modified
//     Brian Sumner's float_to_IBM routine, which works and is cleaner.
//------------------------------------------------------------------------------
void SegyUtil::intelFloatToIBMfloat(float &value)
{
  unsigned int *fconv = (unsigned int*)&value;
  unsigned int fsign = 0x80000000 & *fconv;
  unsigned int fexpn = 0x7f800000 & *fconv;
  unsigned int fmant = (0x007fffff & *fconv) | 0x00800000;

  if (!fexpn) *fconv = 0;
  else {
    int t = (int)(fexpn >> 23) - 126;
    while (t & 0x3) { fmant >>= 1;   t++; }
    fexpn = (unsigned int)((t >> 2) + 64) << 24;
    *fconv = fsign | fexpn | fmant;
  }

// interchange bytes to make value ready for workstation
  swap4Bytes(value);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Byte-swapping routines to go from little-endian to big-endian and back.
//------------------------------------------------------------------------------
// Purpose:  swaps bytes for a 2 byte word in order to go from linux to unix
//   or vice-versa
// Notes:  For 2 byte words, this reverses the order of the 2 bytes.
//   Thus, for an original order of 1-2, the output order is 2-1.
//------------------------------------------------------------------------------
void SegyUtil::swap2Bytes(short int &inVal)
{
  char* val = (char*)(&inVal);
  char temp = val[0];
  val[0] = val[1];
  val[1] = temp;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Input:  char* pointer to a 4-byte word (float or int64_t longint).
// Notes:  For 4 byte words, this reverses the order of the four bytes.
//   For an original order of 1-2-3-4, the output order is 4-3-2-1.
//------------------------------------------------------------------------------
void SegyUtil::swap4Bytes(float &inVal)
{
  swap4Bytes((char*)(&inVal));
}
//------------------------------------------------------------------------------
void SegyUtil::swap4Bytes(int &inVal)
{
  swap4Bytes((char*)(&inVal));
}
//------------------------------------------------------------------------------
// Purpose:  swaps bytes for a 4 byte word in order to go from linux 
//   (little endian) to unix (big endian) or vice-versa
//------------------------------------------------------------------------------
void SegyUtil::swap4Bytes(char* val)
{
  char temp = val[0];
  val[0] = val[3];
  val[3]= temp;      // swap bytes 1 and 4
  temp = val[1];
  val[1] = val[2];
  val[2] = temp;     // swap bytes 2 and 3
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SegyUtil::EBCDIC_to_ASCII(int buf_length, unsigned char *buf_addr)
{
  unsigned char temp;
  for (int i=0; i<buf_length; i++) {
    temp = buf_addr[i];
    buf_addr[i] = EBCDIC_translate_ASCII[temp];
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SegyUtil::ASCII_to_EBCDIC(int buf_length, unsigned char *buf_addr)
{
  unsigned char temp;
  for (int i=0; i<buf_length; i++) {
    temp = buf_addr[i];
    buf_addr[i] = ASCII_translate_EBCDIC[temp];
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  returns a string that describes the data format
//------------------------------------------------------------------------------
string SegyUtil::dataFormatToString(eDataFormat format)
{
	string formatStr = "Data sample format:  ";
	switch (format) {
	case IBMFLT32:
		formatStr += "IBM float 32 bit (1)";
		break;

	case INT32:
		formatStr += "Integer 32 bit (2)";
		break;

	case IEEEFLT32:
		formatStr += "IEEE float 32 bit (5)";
		break;

	default:
		formatStr += "unknown";
	}

	return formatStr;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
string SegyUtil::dataDomainToString(eDomain domain)
{
	switch (domain) {
	case TIME_DOMAIN:
		return "time (ms)";

	case DEPTH_DOMAIN:
		return "depth (m)";

	default:
		return "undefined";
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
string SegyUtil::distanceUnits(int units)
{
	switch (units) {
	case METRIC_UNITS:
		return "Distance units:  meters (1)";
		
	case ENGLISH_UNITS:
		return "Distance units:  feet (2)";
		
	default:
		return "Unknown distance units";
	}
}
//-----------------------------------------------------------------------------------
//=================================================================================== 

} // End namespace PagosaGeo
