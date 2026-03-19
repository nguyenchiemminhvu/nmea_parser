// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_dtm.h — DTM (Datum Reference) sentence struct
//
// Format: $--DTM,ccc,a,x.x,a,x.x,a,x.x,ccc*hh
//   [0] datum_code [1] datum_sub [2] lat_offset [3] N/S [4] lon_offset
//   [5] E/W [6] alt_offset [7] ref_datum

#pragma once
#include <cstdint>
#include <string>
#include <functional>

namespace nmea { namespace parser {

struct nmea_dtm
{
    std::string talker;
    std::string datum_code;   ///< Local datum (e.g. "W84", "W72", "S85")
    std::string datum_sub;    ///< Local datum subdivision code (may be empty)
    double lat_offset  = 0.0; ///< Latitude offset [minutes]; positive = North
    double lon_offset  = 0.0; ///< Longitude offset [minutes]; positive = East
    double alt_offset  = 0.0; ///< Altitude offset [m]
    std::string ref_datum;    ///< Reference datum (e.g. "W84")
    bool   valid       = false;
};
using dtm_callback = std::function<void(const nmea_dtm&)>;

} } // nmea::parser
