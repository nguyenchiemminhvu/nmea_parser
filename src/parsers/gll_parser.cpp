// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/gll_parser.h"
#include "internal/field_parser.h"

namespace nmea { namespace parser {

// $--GLL,llll.ll,a,yyyyy.yy,a,hhmmss.ss,A,a*hh
// [0] lat [1] N/S [2] lon [3] E/W [4] time [5] status [6] mode

bool gll_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 5u) return false;

    nmea_gll msg;
    msg.talker = s.talker;

    if (!parse_latlon(s.field(0u), s.field(1u), msg.latitude))  return false;
    if (!parse_latlon(s.field(2u), s.field(3u), msg.longitude)) return false;
    parse_utc_time(s.field(4u), msg.utc_time);

    if (s.has_field(5u)) msg.status_active = (s.field(5u)[0] == 'A');
    if (s.has_field(6u)) parse_char(s.field(6u), msg.mode);

    msg.valid = true;
    if (cb_) cb_(msg);
    return true;
}

} }
