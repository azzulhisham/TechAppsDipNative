//==============================================================================
// PSPA: Poststack Seismic Processing and Analysis (� Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyFileHeader
// Purpose:  Manage details of standard SEGY header files, REV1 (binary and text)
//==============================================================================

#ifndef SEGYFILEHEADER_H
#define SEGYFILEHEADER_H

#include <string>
#include "DataRange.h"
using std::string;
#include <inttypes.h>
//typedef int64_t int64_t;

namespace PagosaGeo {

static const int64_t TEXTHDRLEN = 3200;
static const int64_t FILEHDRLEN = 400;

//------------------------------------------------------------------------------
// Enumeration for SEGY definitions
//------------------------------------------------------------------------------
enum eSegyRevision {
   SEGY_UNDEFINED = -1,
   SEGY_REV0 = 0,
   SEGY_REV1 = 1,
	SEGY_LANDMARK = 2,
	SEGY_PARADIGM = 3,
	SEGY_CUSTOM = 5  // custom definition
};
//------------------------------------------------------------------------------

//------------------------------------------------CU------------------------------
// Enumeration for seismic data formats to match SEGY definitions
// Types 6 and 7 are undefined.
//------------------------------------------------------------------------------
enum eDataFormat {
	IBMFLT32 = 1,    // big endian IBM float
	INT32 = 2,
	INT16 = 3,
	FIXPTGN32 = 4,   // 4 byte fixed point with gain (obsolete)
	IEEEFLT32 = 5,   // big endian IEEE float
	INT8 = 8         // all integers are big endian for segy (unix, IBM), little endian on Intel (hence in code)
};
//------------------------------------------------------------------------------


struct SegyFileHeader {
public:
   int _job_id;                         // (1-4)
   int _line_num;                       // (5-8)
   int _reel_num;                       // (9-12)
   short int _traces_per_record;        // (13-14) not important for stacked data
   short int _aux_traces_per_record;    // (15-16) not needed for stacked data
   short int _sample_int;               // (17-18) in micro-seconds (max. ~ 32 ms)
   short int _orig_sample_int;          // (19-20) in micro-seconds
   short int _samples_per_trace;        // (21-22)
   short int _orig_samples_per_trace;   // (23-24)
   short int _data_sample_format;       // (25-26)
                                        //    1 = floating point (4 bytes)
                                        //    2 = 4 byte integer
                                        //    3 = 2 byte integer
                                        //    4 = fixed point _w/gain code (4 bytes)
                                        //    5 = IEEE 4 byte floating point
                                        //    6, 7 not currently assigned
                                        //    8 = 1 byte integer
   short int _cdp_fold;                 // (27-28)
   short int _trace_sort_code;          // (29-30) trace sorting code: 
                                        //    1 = as recorded (no sorting)
                                        //    2 = CDP ensemble
                                        //    3 = single fold continuous profile
                                        //    4 = horizontally stacked
   short int _vertical_sum_code;        // (31-32) vertical sum code:
                                        //    1 = no sum
                                        //    2 = two sum ...
                                        //    N = N sum (N = 32,767)
   short int _sweep_freq_start;         // (33-34)
   short int _sweep_freq_end;           // (35-36)
   short int _sweep_length;             // (37-38)
   short int _sweep_type_code;          // (39-40) sweep type code:
                                        //    1 = linear
                                        //    2 = parabolic
                                        //    3 = exponential
                                        //    4 = other
   short int _sweep_trace_num;          // (41-42)
   short int _sweep_start_taper_length; // (43-44)
   short int _sweep_end_taper_length;   // (45-46)
   short int _taper_type;               // (47-48)
   short int _correlated_traces;        // (49-50)
   short int _binary_gain_recovered;    // (51-52)
   short int _amplitude_recovery;       // (53-54) amplitude recovery method code:
                                        //     1 = none
                                        //     2 = spherical divergence
                                        //     3 = AGC
                                        //     4 = other
   short int _distance_units;           // (55-56) measurement system code:
                                        //    1 = meters
                                        //    2 = feet
   short int _polarity;                 // (57-58) impulse signal polarity code:
                                        //    1 = increase in pressure or upward; (REVERSE)
                                        //    geophone case movement gives negative number on tape
                                        //    2 = increase in pressure or upward; (NORMAL)
                                        //    geophone case movement gives positive number on tape
   short int _polarity_code;            // (59-60) Vibratory polarity code
   char _unassigned1[240];              // (61-300)
   short int _revision;                 // (301-302)
   short int _fixed_length_flag;        // (303-304)
   short int _num_ext_text_hdr;         // (305-306) # of 3200 byte extended textual headers
   char _unassigned2[94];               // (309-400)

   // convenience methods
	void reset();
	void setFromRange(const DataRange& rng);
	int64_t size() const { return FILEHDRLEN; }
	int numBytesPerValue() const;
   eDataFormat dataFormat() const;
	string toString() const;
};
//==============================================================================

} // end namespace PagosaGeo

#endif
