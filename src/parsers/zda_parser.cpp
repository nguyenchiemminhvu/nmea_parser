// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/zda_parser.h"
#include "internal/field_parser.h"

namespace nmea { namespace parser {

// $--ZDA,hhmmss.ss,dd,mm,yyyy,hh,mm*hh
// [0]time [1]day [2]month [3]year [4]tz_hour [5]tz_min

bool zda_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 4u) return false;

    nmea_zda msg;
    msg.talker = s.talker;

    parse_utc_time(s.field(0u), msg.utc_time);

    int day = 0, month = 0, year = 0;
    if (parse_int(s.field(1u), day))   msg.day   = static_cast<uint8_t>(day);
    if (parse_int(s.field(2u), month)) msg.month = static_cast<uint8_t>(month);
    if (parse_int(s.field(3u), year))  msg.year  = static_cast<uint16_t>(year);

    if (s.has_field(4u)) { int h = 0; if (parse_int(s.field(4u), h)) msg.tz_hour = static_cast<int8_t>(h); }
    if (s.has_field(5u)) { int m = 0; if (parse_int(s.field(5u), m)) msg.tz_min  = static_cast<uint8_t>(m); }

    msg.valid = true;
    if (cb_) cb_(msg);
    return true;
}

} }
