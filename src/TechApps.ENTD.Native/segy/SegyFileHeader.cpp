//=============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyFileHeader
// Purpose:  define segy file header
// References:  Pullan (GEO 1990, 1260-1271); Bennett (GEO 1990, 1272-1284)
//==============================================================================

#include "SegyFileHeader.h"
#include "SegyUtil.h"
#include <stdio.h>
namespace PagosaGeo {

//==============================================================================
//------------------------------------------------------------------------------
// Purpose:  set all parameters in file header to default state (mostly zero)
// public static method
//------------------------------------------------------------------------------
void SegyFileHeader::reset()
{
  _job_id = 1;
  _line_num = 0;		// 3d inline number
  _reel_num = 0;
  _traces_per_record = 1;
  _aux_traces_per_record = 0;
  _sample_int = 0; // in micro-seconds
  _orig_sample_int = 0;
  _samples_per_trace = 0;
  _orig_samples_per_trace = 0;
  _data_sample_format = 0;
  _cdp_fold = 1;
  _trace_sort_code = 4;		// stacked data
  _vertical_sum_code = 0;
  _sweep_freq_start = 0;
  _sweep_freq_end = 0;
  _sweep_length = 0;
  _sweep_type_code = 0;
  _sweep_trace_num = 0;
  _sweep_start_taper_length = 0;
  _sweep_end_taper_length = 0;
  _taper_type = 0;
  _correlated_traces = 0;
  _binary_gain_recovered = 0;
  _amplitude_recovery = 0;
  _distance_units = 0;
  _polarity = 0;
  _polarity_code = 0;
  _revision = 256;         // Rev1
  _fixed_length_flag = 1;  // fixed length = true
  _num_ext_text_hdr = 0;
  for (int i=0; i<240; i++) _unassigned1[i] = 0;
  for (int i=0; i<94; i++) _unassigned2[i] = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SegyFileHeader::setFromRange(const DataRange& rng)
{
	_job_id = 1;
	_line_num = 0;		// 3d inline number
	_reel_num = 0;
	_traces_per_record = 1;
	_aux_traces_per_record = 0;
	_orig_sample_int = _sample_int;								// in case sample int changes
	_sample_int = (short int)(1000000.*rng.dSample());		// convert seconds to micro-seconds
	_orig_samples_per_trace = _samples_per_trace;			// in case samples per trace changes
	_samples_per_trace = (short int)(rng.numSmps());
	_data_sample_format = 5;
	_cdp_fold = 1;
	_trace_sort_code = 4;		// stacked data
	_vertical_sum_code = 0;
	_sweep_freq_start = 0;
	_sweep_freq_end = 0;
	_sweep_length = 0;
	_sweep_type_code = 0;
	_sweep_trace_num = 0;
	_sweep_start_taper_length = 0;
	_sweep_end_taper_length = 0;
	_taper_type = 0;
	_correlated_traces = 0;
	_binary_gain_recovered = 0;
	_amplitude_recovery = 0;
	_distance_units = rng.isMetric() ? 1 : 2;
	_polarity = 0;
	_polarity_code = 0;
	_revision = 256;         // Rev1
	_fixed_length_flag = 1;  // fixed length = true
	_num_ext_text_hdr = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int SegyFileHeader::numBytesPerValue() const
{
	switch (_data_sample_format) {
	case 1:     // 4 byte IBM float
	case 2:     // 4 byte int (big endian)
	case 5:     // 4 byte IEEE float, big endian
		return 4;

	case 3:     // 2 byte int (big endian)
		return 2;

	case 8:      // 1 byte int (big endian)
		return 1;

	case 4:   // obsolete
	case 7:   // undefined
	default:  // invalid data format
		return 0;
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
eDataFormat SegyFileHeader::dataFormat() const
{
	switch (_data_sample_format) {
   case 1:
      return IBMFLT32;
   case 2:
      return INT32;
   case 3:
      return INT16;
   case 5:
      return IEEEFLT32;
   case 8:
      return INT8;
	default:
		return IEEEFLT32;  // ???
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
string SegyFileHeader::toString() const
{
	string info;
	char temp[80];

	snprintf(temp,79,   "(1)   Job ID:\t\t%d", _job_id); info += temp;
	snprintf(temp, 79, "\n(5)   Line number:\t\t%d", _line_num); info += temp;
	snprintf(temp, 79, "\n(9)   Reel number:\t\t%d", _reel_num); info += temp;
	snprintf(temp, 79, "\n(13)  Traces per record:\t%d", _traces_per_record); info += temp;
	snprintf(temp, 79, "\n(17)  Sample interval:\t\t%3.1f (ms)", (float)_sample_int/1000.0); info += temp;
	snprintf(temp, 79, "\n(19)  Orig. sample interval:\t%3.1f (ms)", (float)_orig_sample_int/1000.0); info += temp;
	snprintf(temp, 79, "\n(21)  Samples per trace:\t%d", _samples_per_trace); info += temp;
	snprintf(temp, 79, "\n(23)  Orig. smps per trace:\t%d", _orig_samples_per_trace); info += temp;
	if (dataFormat() == IBMFLT32) info += "\n(25)  Data format:\t\tIBM Float-32";
   else info += "\n(25)  Data format:\t\tIEEE Float-32";
   snprintf(temp, 79, "\n(27)  CDP fold:\t\t%d", _cdp_fold); info += temp;
	if (_distance_units == 1) info += "\n(55)  Distance units:\t\tmeters (1)";
   else info += "\n(55)  Distance units:\t\tfeet (2)";
   snprintf(temp,79, "\n(57)  Polarity:\t\t%d", _polarity); info += temp;
	info += "\n(301) Apparent revision:\t";
	if (_revision == 0) info += "SEGY Rev0";
	else info += "SEGY Rev1";
   if (_fixed_length_flag == 0) info += "\n(303) Fixed length flag:\tfalse (0)";
   else {
      snprintf(temp,79, "\n(303) Fixed length flag:\ttrue (%d)", _fixed_length_flag); 
      info += temp;
   }
   if (_revision != 0) {
	   snprintf(temp,79, "\n(305) Number of extended text headers:\t  %d", _num_ext_text_hdr);
      info += temp;
   }

	return info;
}
//------------------------------------------------------------------------------
//==============================================================================

}  // end of namespace PagosaGeo
