// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/rmc_parser.h"
#include "internal/field_parser.h"

namespace nmea
{
namespace parser
{

// $--RMC,hhmmss.ss,A/V,llll.ll,N/S,yyyyy.yy,E/W,x.x,x.x,ddmmyy,x.x,E/W,a*hh
// Fields [0..11]

bool rmc_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 9u)
        return false;

    nmea_rmc msg;
    msg.talker = s.talker;

    // [0] UTC time
    if (!parse_utc_time(s.field(0u), msg.utc_time))
        return false;

    // [1] Status
    char status = '\0';
    if (!parse_char(s.field(1u), status))
        return false;
    msg.status_active = (status == 'A' || status == 'a');

    // [2] Latitude, [3] N/S
    if (!parse_latlon(s.field(2u), s.field(3u), msg.latitude))
        return false;

    // [4] Longitude, [5] E/W
    if (!parse_latlon(s.field(4u), s.field(5u), msg.longitude))
        return false;

    // [6] Speed over ground [knots]
    parse_double(s.field(6u), msg.speed_knots);

    // [7] Course over ground [degrees]
    parse_double(s.field(7u), msg.course_true);

    // [8] UTC date (ddmmyy)
    parse_utc_date(s.field(8u), msg.date);

    // [9] Magnetic variation (optional)
    if (s.has_field(9u))
        parse_double(s.field(9u), msg.mag_variation);

    // [10] Mag var E/W → negate if West
    if (s.field_count() > 10u)
    {
        char magdir = '\0';
        if (parse_char(s.field(10u), magdir))
        {
            if (magdir == 'W' || magdir == 'w')
                msg.mag_variation = -msg.mag_variation;
        }
    }

    // [11] Mode indicator (NMEA 2.3+)
    if (s.field_count() > 11u)
    {
        char mode = '\0';
        if (parse_char(s.field(11u), mode))
            msg.mode = static_cast<rmc_mode>(mode);
    }

    msg.valid = true;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace nmea
