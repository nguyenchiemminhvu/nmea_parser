// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_gll.h — GLL sentence struct

#pragma once
#include <cstdint>
#include <string>
#include <functional>

namespace nmea { namespace parser {

struct nmea_gll
{
    std::string talker;
    double   latitude  = 0.0;
    double   longitude = 0.0;
    double   utc_time  = 0.0;
    bool     status_active = false;
    char     mode      = '\0';   ///< A/D/E/N/S (NMEA 2.3+)
    bool     valid     = false;
};
using gll_callback = std::function<void(const nmea_gll&)>;

} } // nmea::parser
