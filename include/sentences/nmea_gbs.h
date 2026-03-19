// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_gbs.h — GBS (GNSS Satellite Fault Detection) sentence struct
//
// Format: $--GBS,hhmmss.ss,x.x,x.x,x.x,xx,x.x,x.x,x.x*hh
//   [0] time  [1] err_lat [2] err_lon [3] err_alt
//   [4] sv_id [5] probability [6] bias [7] std_dev

#pragma once
#include <string>
#include <functional>

namespace nmea { namespace parser {

struct nmea_gbs
{
    std::string talker;
    double   utc_time     = 0.0;
    double   err_lat      = 0.0;   ///< Expected 1-sigma error in latitude [m]
    double   err_lon      = 0.0;
    double   err_alt      = 0.0;
    int      failed_sv_id = -1;    ///< PRN of most likely failed satellite; -1=none
    double   probability  = 0.0;   ///< Probability of missed detection (0–1)
    double   bias         = 0.0;   ///< Estimate of bias on failed SVs range [m]
    double   std_dev      = 0.0;   ///< Std deviation of bias estimate [m]
    bool     valid        = false;
};
using gbs_callback = std::function<void(const nmea_gbs&)>;

} } // nmea::parser
