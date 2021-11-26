//==============================================================================
// PSPA: Poststack Seismic Processing and Analysis (� Pagosa Geophysical Research, LLC, 2014)
// Structure:  SegyTraceHeader
// Purpose:  Define standard Rev1 SEGY trace header 
//==============================================================================

#ifndef SEGYTRACEHEADER_H
#define SEGYTRACEHEADER_H

#include <string>
using std::string;
#include <inttypes.h>
//typedef int64_t int64_t;

namespace PagosaGeo {

static const int64_t TRACEHDRLEN = 240;


struct SegyTraceHeader {
public:
	int _trace_number_in_line;            // (1-4) use for trace # in survey	
	int _trace_number_in_reel;            // (5-8) use for trace # in ensemble; sometimes used by Petrel for inline #
	int _orig_field_rec_num;              // (9-12) use for inline #; now prefer 181-184
	int _trace_num_in_orig_field_rec;     // (13-16)
	int _energy_source_point_num;         // (17-20)
	int _cdp_ensemble_num;                // (21-24) use for crossline #; now prefer 185-188
	int _trace_num_in_ensemble;           // (25-28)
	short int _trace_id_code;             // (29-30) 1 = data, 2 = dead, 3 = dummy
	short int _num_ver_stacked_traces;    // (31-32)
	short int _num_hor_stacked_traces;    // (33-34)
	short int _data_use;                  // (35-36)
	int _shot_receiver_dist;              // (37-40) offset
	int _receiver_elevation;              // (41-44)
	int _surface_elevation_at_source;     // (45-48)
	int _source_depth_below_surface;      // (49-52)
	int _datum_elevation_at_receiver;     // (53-56)
	int _datum_elevation_at_source;       // (57-60)
	int _water_depth_at_source;           // (61-64)
	int _water_depth_at_group;            // (65-68)
	short int _depth_scale;               // (69-70)
	short int _coordinate_scalar;         // (71-72) scalar for next 4 headers (+- 1, 10, 100, 1000, 10000)
	int _source_x;                        // (73-76) coordinate_scalar applies
	int _source_y;                        // (77-80) coordinate_scalar applies
	int _group_x;                         // (81-84) coordinate_scalar applies
	int _group_y;                         // (85-88) coordinate_scalar applies
	short int _coordinate_units;          // (89-90)
	short int _weathering_velocity;       // (91-92)
	short int _subweathering_velocity;    // (93-94)
	short int _uphole_time_at_source;     // (95-96)
 	short int _uphole_time_at_group;      // (97-98)
	short int _source_static_correction;  // (99-100)
	short int _group_static_correction;   // (101-102)
	short int _total_static;              // (103-104)
	short int _lag_time_A;                // (105-106) milliseconds
	short int _lag_time_B;                // (107-108) milliseconds
	short int _recording_delay;           // (109-110) start time in milliseconds (+ or -)
	short int _mute_time_start;				// (111-112) milliseconds
	short int _mute_time_end;					// (113-114) use for first nonzero sample; in milliseconds
	short int _number_samples;					// (115-116)
	short int _sample_interval;				// (117-118) in microseconds
	short int _instrument_gain_type;			// (119-120)
	short int _gain_constant;					// (121-122)
	short int _initial_gain;					// (123-124)
	short int _correlated_trace;				// (125-126)
	short int _sweep_freq_at_start;			// (127-128)
	short int _sweep_freq_at_end;				// (129-130)
	short int _sweep_length;					// (131-132)
	short int _sweep_type;						// (133-134)
	short int _sweep_start_taper_length;	// (135-136)
	short int _sweep_end_taper_length;		// (137-138)
	short int _taper_type;						// (139-140)
	short int _alias_filter_freq;				// (141-142)
	short int _alias_filter_slope;			// (143-144)
	short int _notch_filter_freq;				// (145-146)
	short int _notch_filter_slope;			// (147-148)
	short int _low_cut_freq;					// (149-150)
	short int _high_cut_freq;					// (151-152)
	short int _low_cut_slope;					// (153-154)
	short int _high_cut_slope;					// (155-156)
	short int _year;								// (157-158)
	short int _day;								// (159-160)
	short int _hour;								// (161-162)
	short int _minute;							// (163-164)
	short int _second;							// (165-166)
	short int _time_code;						// (167-168)
	short int _trace_weighting_factor;		// (169-170) used for 8 bit int _data only?
	short int _group_number_of_roll_switch;			// (171-172)
	short int _group_number_of_first_trace_in_orig;	// (173-174)
	short int _group_number_of_last_trace_in_orig;	// (175-176)
	short int _gap_size;                  // (177-178)
	short int _taper_overtravel;          // (179-180)
	int _cdp_x;                           // (181-184) ensemble (CDP) x position; coordinate_scalar applies
	int _cdp_y;                           // (185-188) ensemble (CDP) y position; coordinate_scalar applies
	int _inline_num;                      // (189-192) SEG Rev1 standard
	int _crossline_num;                   // (193-196) SEG Rev1 standard
	int _shotpoint_num;                   // (197-200) 2D only
	short int _shotpoint_scalar;          // (201-202) apply to shotpoint _numbers
	short int _trace_val_meas_unit;       // (203-204)
	short int _transduction_constant1;    // (205-206) multiplicative constants
	short int _transduction_constant2;    // (207-208) multiplicative constants
	short int _transduction_constant3;    // (209-210) multiplicative constants
	short int _transduction_units;        // (211-212)
	short int _device_identifier;         // (213-214)
	short int _time_scalar;               // (215-216) takes values of 1, 10, 100, 1000, 10000
	short int _source_orientation;        // (217-218)
	short int _source_energy_dir1;        // (219-220)
	short int _source_energy_dir2;        // (221-222)
	short int _source_energy_dir3;        // (223-224)
	int _source_meas_mantissa;            // (225-228)
	short int _source_meas_complement;    // (229-230)
	short int _source_meas_unit;          // (231-232)
	int _unassigned1;                     // (233-236)
	int _unassigned2;                     // (237-240)

   // convenience methods
   int64_t size() const { return TRACEHDRLEN; }
   string toString() const;
   void reset();
};
//------------------------------------------------------------------------------
//==============================================================================

} // end namespace PagosaGeo

#endif


