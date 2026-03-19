// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_gst.h — GST (GNSS Pseudo Range Error Statistics) sentence struct
//
// Format: $--GST,hhmmss.ss,x.x,x.x,x.x,x.x,x.x,x.x,x.x*hh
//   [0] time [1] rms [2] semi_major [3] semi_minor [4] orient
//   [5] lat_err [6] lon_err [7] alt_err

#pragma once
#include <string>
#include <functional>

namespace nmea { namespace parser {

struct nmea_gst
{
    std::string talker;
    double utc_time    = 0.0;
    double rms_dev     = 0.0;   ///< RMS value of the std deviation of the ranges
    double semi_major  = 0.0;   ///< Std deviation of semi-major axis [m]
    double semi_minor  = 0.0;   ///< Std deviation of semi-minor axis [m]
    double orient      = 0.0;   ///< Orientation of semi-major axis [°] (true north)
    double lat_err     = 0.0;   ///< Std deviation of latitude error [m]
    double lon_err     = 0.0;   ///< Std deviation of longitude error [m]
    double alt_err     = 0.0;   ///< Std deviation of altitude error [m]
    bool   valid       = false;
};
using gst_callback = std::function<void(const nmea_gst&)>;

} } // nmea::parser
