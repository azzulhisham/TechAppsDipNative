//===============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// File:  DataDefs.h
// Purpose:  Definitions pertaining to seismic data container classes
//===============================================================================

#ifndef DATADEFS_H
#define DATADEFS_H

#include <string>
using std::string;


namespace PagosaGeo {

// coordinate system defines for vector attributes (not domain of vertical axis)
enum eCoordSystem {
  UNDEFINED_CS = -1,
  DEFAULT_CS =    0,
  X_CS =         10,
  T_CS =         11,
  F_CS =         12,
  K_CS =         13,
  XZ_CS =        20,   // components are wavelengths
  POLAR_CS =     21,
  XT_CS =        22,   // components are wavelength and period
  SLOPE_2D_CS =  23,
  KXKZ_CS =      24,   // components are both wavenumbers
  FK_CS =        25,   // components are frequency and wavenumber
  CARTESIAN_CS = 30,   // components are (wave) lengths  
  XYZ_CS =       30,   // duplicate define
  SPHERICAL_CS = 31,
  XYT_CS =       32,   // components are wavelengths and period
  SLOPE_CS =     33,   // depends on domain of input data (XYT or XYZ)
  KXKYKZ_CS =    34,   // components are all wavenumbers
  FKXKY_CS =     35    // components are frequency and 2 wavenumbers
};


// defines for domain (vertical axis)
enum eDomain {
  UNDEF_DOMAIN = -1,
  TIME_DOMAIN =   0,
  DEPTH_DOMAIN =  1,
  FREQ_DOMAIN = 2,
  ATRB_DOMAIN = 3
};


// Defines for x-y units
enum eXyUnits {
	DIMENSIONLESS = 0,      // unit distance between samples in all 3 directions
	METRIC_UNITS  = 1,      // same as segy if meters
	ENGLISH_UNITS = 2       // same as segy if feet
};


#ifndef RIGHT_HANDED
#define RIGHT_HANDED 1
#define LEFT_HANDED -1
#endif

 
enum eSurveyOrganization {
	BY_INLINE = 1,
	BY_CROSSLINE = 2
};


// structure to define a 3-D point in seismic space
struct DataPosition {
	double inln;			// line # (not feet or meters)
	double crln;			// line # (not feet or meters)
	double time;			// meters or seconds or samples
	DataPosition(double ilin = 0.0, double xlin = 0.0, double tstart = 0.0) {
		inln = ilin;  crln = xlin;  time = tstart;
	}
	DataPosition& operator=(const DataPosition& p)
	{ inln = p.inln; crln = p.crln; time = p.time; return *this; }
	bool operator==(const DataPosition& p) const
	{ return (inln==p.inln && crln==p.crln && time==p.time) ? true : false; }
	bool operator!=(const DataPosition& p) const
	{ return !operator==(p); }
	void reset() { inln = 0.0;  crln = 0.0;  time = 0.0; }
	string toString() const {
		string info = "Data Position:";
		char temp[80];
		snprintf(temp,79, "  inline %5.1f", inln); info+= temp;
		snprintf(temp, 79, ",  crossline %5.1f", crln); info+= temp;
		snprintf(temp, 79, ",  time %5.1f (s)", time); info+= temp;
		return info;
	}
};

}  // end namespace PagosaGeo

#endif
