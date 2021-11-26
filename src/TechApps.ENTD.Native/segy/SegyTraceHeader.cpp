//==============================================================================
// Structure:  SegyTraceHeader
// Purpose:  define segy file header
// References:  Pullan (GEO 1990, 1260-1271); Bennett (GEO 1990, 1272-1284)
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include "SegyTraceHeader.h"
#include <cstdio>


namespace PagosaGeo {

//------------------------------------------------------------------------------
// Purpose: sets all parameters in trace header to default state (mostly zero)
//------------------------------------------------------------------------------
void SegyTraceHeader::reset()   // class method
{
  _trace_number_in_line = 0;
  _trace_number_in_reel = 0;
  _orig_field_rec_num = 0;
  _trace_num_in_orig_field_rec = 0;
  _energy_source_point_num = 0;
  _cdp_ensemble_num = 0;
  _trace_num_in_ensemble = 0;
  _trace_id_code = 1;  // 1= LIVE, 2 = DEAD
  _num_ver_stacked_traces = 0;
  _num_hor_stacked_traces = 0;
  _data_use = 0;
  _shot_receiver_dist = 0;
  _receiver_elevation = 0;
  _surface_elevation_at_source = 0;
  _source_depth_below_surface = 0;
  _datum_elevation_at_receiver = 0;
  _datum_elevation_at_source = 0;
  _water_depth_at_source = 0;
  _water_depth_at_group = 0;
  _depth_scale = 0;
  _coordinate_scalar = 0;
  _source_x = 0;
  _source_y = 0;
  _group_x = 0;
  _group_y = 0;
  _coordinate_units = 0;
  _weathering_velocity = 0;
  _subweathering_velocity = 0;
  _uphole_time_at_source = 0;
  _uphole_time_at_group = 0;
  _source_static_correction = 0;
  _group_static_correction = 0;
  _total_static = 0;
  _lag_time_A = 0;
  _lag_time_B = 0;
  _recording_delay = 0;
  _mute_time_start = 0;
  _mute_time_end = 0;
  _number_samples = 0;
  _sample_interval = 0;
  _instrument_gain_type = 0;
  _gain_constant = 0;
  _initial_gain = 0;
  _correlated_trace = 0;
  _sweep_freq_at_start = 0;
  _sweep_freq_at_end = 0;
  _sweep_length = 0;
  _sweep_type = 0;
  _sweep_start_taper_length = 0;
  _sweep_end_taper_length = 0;
  _taper_type = 0;
  _alias_filter_freq = 0;
  _alias_filter_slope = 0;
  _notch_filter_freq = 0;
  _notch_filter_slope = 0;
  _low_cut_freq = 0;
  _high_cut_freq = 0;
  _low_cut_slope = 0;
  _high_cut_slope = 0;
  _year = 0;
  _day = 0;
  _hour = 0;
  _minute = 0;
  _second = 0;
  _time_code = 0;
  _trace_weighting_factor = 0;
  _group_number_of_roll_switch = 0;
  _group_number_of_first_trace_in_orig = 0;
  _group_number_of_last_trace_in_orig = 0;
  _gap_size = 0;
  _taper_overtravel = 0;
  _cdp_x = 0;            // (181-184) ensemble (CDP) x position; trace scalar applies
  _cdp_y = 0;            // (185-188) ensemble (CDP) y position; trace scalar applies
  _inline_num = 0;       // (189-192)
  _crossline_num = 0;    // (193-196)
  _shotpoint_num = 0;    // (197-200) 2D only
  _shotpoint_scalar = 1; // (201-202)
  _trace_val_meas_unit = 0;
  _transduction_constant1 = 0;
  _transduction_constant2 = 0;
  _transduction_constant3 = 0;
  _transduction_units = 0;
  _device_identifier = 0;
  _time_scalar = 1;      // (215-216)
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose:  returns a string of basic information about the trace header
//------------------------------------------------------------------------------
string SegyTraceHeader::toString() const
{
	string info;
	char temp[80];

	snprintf(temp, 79,  "(1)   Sequence number in line:\t%d", _trace_number_in_line); info += temp;
	snprintf(temp, 79, "\n(5)   Sequence number in file:\t%d", _trace_number_in_reel); info += temp;
	snprintf(temp, 79, "\n(9)   Original field record number:\t%d", _trace_number_in_reel); info += temp;
	snprintf(temp, 79,	"\n(13)  Trace # in original record:\t%d", _trace_num_in_orig_field_rec); info += temp;
	snprintf(temp, 79,	"\n(21)  CDP ensemble number:\t%d", _cdp_ensemble_num); info += temp;
	snprintf(temp, 79,	"\n(25)  Trace number in ensemble:\t%d", _trace_num_in_ensemble); info += temp;
	snprintf(temp, 79,	"\n(29)  Trace ID code:\t\t%d", _trace_id_code); info += temp;
	snprintf(temp, 79,	"\n(41)  Receiver elevation:\t\t%d", _receiver_elevation); info += temp;
	snprintf(temp, 79,	"\n(45)  Surface elevation at source:\t%d", _surface_elevation_at_source); info += temp;
	snprintf(temp, 79,	"\n(71)  Coordinate scalar:\t\t%d", _coordinate_scalar); info += temp;
	snprintf(temp, 79,	"\n(73)  Source X coordinate:\t\t%d", _source_x); info += temp;
	snprintf(temp, 79,	"\n(77)  Source Y coordinate:\t\t%d", _source_y); info += temp;
	snprintf(temp, 79,	"\n(81)  Group X coordinate:\t\t%d", _group_x); info += temp;
	snprintf(temp, 79,	"\n(85)  Group Y coordinate:\t\t%d", _group_y); info += temp;
	snprintf(temp, 79,	"\n(109) Recording delay:\t\t%d (ms)", _recording_delay); info += temp;
	snprintf(temp, 79,	"\n(111) Mute time start:\t\t%d", _mute_time_start); info += temp;
	snprintf(temp, 79,	"\n(113) Mute time end:\t\t%d", _mute_time_end); info += temp;
	snprintf(temp, 79,	"\n(115) Number of samples in trace:\t%d", _number_samples); info += temp;
	snprintf(temp, 79,	"\n(117) Sample interval:\t\t%3.1f (ms)", (float)_sample_interval/1000.0); info += temp;
	snprintf(temp, 79,	"\n(181) CDP X coordinate:\t\t%d", _cdp_x); info += temp;
	snprintf(temp, 79,	"\n(185) CDP Y coordinate:\t\t%d", _cdp_y); info += temp;
	snprintf(temp, 79,	"\n(189) Inline number:\t\t%d", _inline_num); info += temp;
	snprintf(temp, 79,	"\n(193) Crossline number:\t\t%d", _crossline_num); info += temp;
	info += "\n";
	return info;
}
//------------------------------------------------------------------------------
//==============================================================================

}  // end of namespace PagosaGeo
