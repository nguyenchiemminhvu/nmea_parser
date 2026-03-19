// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/gns_parser.h"
#include "internal/field_parser.h"

namespace nmea { namespace parser {

// $--GNS,hhmmss.ss,llll.ll,a,yyyyy.yy,a,c--c,xx,x.x,x.x,x.x,x.x,x.x*hh
// [0]time [1]lat [2]N/S [3]lon [4]E/W [5]mode [6]nsats [7]hdop
// [8]alt [9]sep [10]diff_age [11]diff_ref

bool gns_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 8u) return false;

    nmea_gns msg;
    msg.talker = s.talker;

    parse_utc_time(s.field(0u), msg.utc_time);
    parse_latlon(s.field(1u), s.field(2u), msg.latitude);
    parse_latlon(s.field(3u), s.field(4u), msg.longitude);

    if (s.has_field(5u)) msg.mode_indicator = s.field(5u);

    int nsats = 0;
    if (parse_int(s.field(6u), nsats)) msg.num_sats = nsats;
    parse_double(s.field(7u), msg.hdop);

    if (s.has_field(8u))  parse_double(s.field(8u), msg.altitude);
    if (s.has_field(9u))  parse_double(s.field(9u), msg.geoid_sep);
    if (s.has_field(10u)) parse_double(s.field(10u), msg.diff_age);
    if (s.has_field(11u)) { int ref = 0; if (parse_int(s.field(11u), ref)) msg.diff_ref = ref; }

    msg.valid = true;
    if (cb_) cb_(msg);
    return true;
}

} }
