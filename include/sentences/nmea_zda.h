// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_zda.h — ZDA (Time and Date) sentence struct
//
// Format: $--ZDA,hhmmss.ss,dd,mm,yyyy,hh,mm*hh
//   [0] time [1] day [2] month [3] year [4] tz_hour [5] tz_min

#pragma once
#include <cstdint>
#include <string>
#include <functional>

namespace nmea { namespace parser {

struct nmea_zda
{
    std::string talker;
    double   utc_time = 0.0;   ///< hhmmss.ss
    uint8_t  day      = 0u;
    uint8_t  month    = 0u;
    uint16_t year     = 0u;
    int8_t   tz_hour  = 0;     ///< Local zone hours offset from UTC (-13..+13)
    uint8_t  tz_min   = 0u;    ///< Local zone minutes (0 or 30 or 45)
    bool     valid    = false;
};
using zda_callback = std::function<void(const nmea_zda&)>;

} } // nmea::parser
