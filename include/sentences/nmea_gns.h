// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_gns.h — GNS (GNSS Fix Data) sentence struct
//
// Format: $--GNS,hhmmss.ss,llll.ll,a,yyyyy.yy,a,c--c,xx,x.x,x.x,x.x,x.x,x.x*hh
//   [0] time [1] lat [2] N/S [3] lon [4] E/W [5] mode_indicator
//   [6] num_sats [7] hdop [8] alt [9] sep [10] diff_age [11] diff_ref

#pragma once
#include <string>
#include <functional>

namespace nmea { namespace parser {

struct nmea_gns
{
    std::string talker;
    double   utc_time     = 0.0;
    double   latitude     = 0.0;  ///< Decimal degrees; negative = South
    double   longitude    = 0.0;  ///< Decimal degrees; negative = West
    std::string mode_indicator;   ///< One char per GNSS system (e.g. "RRAA")
    int      num_sats     = 0;
    double   hdop         = 0.0;
    double   altitude     = 0.0;  ///< Orthometric height (HAE) [m]
    double   geoid_sep    = 0.0;
    double   diff_age     = -1.0; ///< Age of differential data [s]; -1 = n/a
    int      diff_ref     = -1;   ///< Differential reference station ID; -1=none
    bool     valid        = false;
};
using gns_callback = std::function<void(const nmea_gns&)>;

} } // nmea::parser
