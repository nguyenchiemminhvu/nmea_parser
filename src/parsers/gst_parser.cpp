// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/gst_parser.h"
#include "internal/field_parser.h"

namespace nmea { namespace parser {

// $--GST,hhmmss.ss,x.x,x.x,x.x,x.x,x.x,x.x,x.x*hh
// [0]time [1]rms [2]smaj [3]smin [4]orient [5]lat_err [6]lon_err [7]alt_err

bool gst_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 8u) return false;

    nmea_gst msg;
    msg.talker = s.talker;

    parse_utc_time(s.field(0u), msg.utc_time);
    parse_double(s.field(1u), msg.rms_dev);
    parse_double(s.field(2u), msg.semi_major);
    parse_double(s.field(3u), msg.semi_minor);
    parse_double(s.field(4u), msg.orient);
    parse_double(s.field(5u), msg.lat_err);
    parse_double(s.field(6u), msg.lon_err);
    parse_double(s.field(7u), msg.alt_err);

    msg.valid = true;
    if (cb_) cb_(msg);
    return true;
}

} }
