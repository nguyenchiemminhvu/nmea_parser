// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/gga_parser.h"
#include "internal/field_parser.h"
#include <cstdint>

namespace nmea
{
namespace parser
{

// $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
// Fields [0..13]

bool gga_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 14u)
        return false;

    nmea_gga msg;
    msg.talker = s.talker;

    // [0] UTC time
    if (!parse_utc_time(s.field(0u), msg.utc_time))
        return false;

    // [1] Latitude, [2] N/S
    if (!parse_latlon(s.field(1u), s.field(2u), msg.latitude))
        return false;

    // [3] Longitude, [4] E/W
    if (!parse_latlon(s.field(3u), s.field(4u), msg.longitude))
        return false;

    // [5] Fix quality
    int quality = 0;
    if (!parse_int(s.field(5u), quality))
        return false;
    msg.fix_quality = static_cast<gga_fix_quality>(quality);

    // [6] Number of satellites
    int num_sats = 0;
    if (!parse_int(s.field(6u), num_sats))
        return false;
    msg.num_satellites = static_cast<uint8_t>(num_sats);

    // [7] HDOP
    parse_double(s.field(7u), msg.hdop);

    // [8] Altitude MSL, [9] units (M)
    parse_double(s.field(8u), msg.altitude_msl);

    // [10] Geoid separation, [11] units (M)
    parse_double(s.field(10u), msg.geoid_sep);

    // [12] DGPS age (optional)
    if (s.has_field(12u))
        parse_double(s.field(12u), msg.dgps_age);
    else
        msg.dgps_age = -1.0;

    // [13] DGPS station ID (optional)
    if (s.has_field(13u))
    {
        unsigned int st_id = 0u;
        if (parse_uint(s.field(13u), st_id))
            msg.dgps_station_id = static_cast<uint16_t>(st_id);
    }

    msg.valid = true;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace nmea
