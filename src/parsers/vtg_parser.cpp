// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/vtg_parser.h"
#include "internal/field_parser.h"

namespace nmea { namespace parser {

// $--VTG,x.x,T,x.x,M,x.x,N,x.x,K,a*hh
// [0]T [1]"T" [2]M [3]"M" [4]N [5]"N" [6]K [7]"K" [8]mode

bool vtg_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 7u) return false;

    nmea_vtg msg;
    msg.talker = s.talker;

    parse_double(s.field(0u), msg.course_true);

    if (s.has_field(2u))
    {
        msg.course_mag_valid = parse_double(s.field(2u), msg.course_mag);
    }

    parse_double(s.field(4u), msg.speed_knots);
    parse_double(s.field(6u), msg.speed_kmh);

    if (s.field_count() > 8u)
        parse_char(s.field(8u), msg.mode);

    msg.valid = true;
    if (cb_) cb_(msg);
    return true;
}

} }
