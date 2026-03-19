// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_vtg.h — VTG sentence struct
//
// Format: $--VTG,x.x,T,x.x,M,x.x,N,x.x,K,a*hh
//   [0] course_true, [1] T
//   [2] course_mag,  [3] M
//   [4] speed_knots, [5] N
//   [6] speed_kmh,   [7] K
//   [8] mode

#pragma once
#include <cstdint>
#include <string>
#include <functional>

namespace nmea { namespace parser {

struct nmea_vtg
{
    std::string talker;
    double course_true    = 0.0;   ///< Course over ground, true north [°]
    double course_mag     = 0.0;   ///< Course over ground, magnetic [°]
    bool   course_mag_valid = false;
    double speed_knots    = 0.0;   ///< Speed over ground [knots]
    double speed_kmh      = 0.0;   ///< Speed over ground [km/h]
    char   mode           = '\0';  ///< A/D/E/N/S
    bool   valid          = false;
};
using vtg_callback = std::function<void(const nmea_vtg&)>;

} } // nmea::parser
