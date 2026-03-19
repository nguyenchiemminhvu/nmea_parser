// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/dtm_parser.h"
#include "internal/field_parser.h"

namespace nmea { namespace parser {

// $--DTM,ccc,a,x.x,a,x.x,a,x.x,ccc*hh
// [0]datum_code [1]datum_sub [2]lat_off [3]N/S [4]lon_off [5]E/W [6]alt_off [7]ref_datum

bool dtm_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 7u) return false;

    nmea_dtm msg;
    msg.talker = s.talker;

    msg.datum_code = s.field(0u);

    if (s.has_field(1u)) msg.datum_sub = s.field(1u);

    double lat_off = 0.0, lon_off = 0.0;
    if (parse_double(s.field(2u), lat_off))
    {
        if (!s.field(3u).empty() && (s.field(3u)[0] == 'S' || s.field(3u)[0] == 's'))
            lat_off = -lat_off;
        msg.lat_offset = lat_off;
    }

    if (parse_double(s.field(4u), lon_off))
    {
        if (!s.field(5u).empty() && (s.field(5u)[0] == 'W' || s.field(5u)[0] == 'w'))
            lon_off = -lon_off;
        msg.lon_offset = lon_off;
    }

    parse_double(s.field(6u), msg.alt_offset);
    if (s.has_field(7u)) msg.ref_datum = s.field(7u);

    msg.valid = true;
    if (cb_) cb_(msg);
    return true;
}

} }
