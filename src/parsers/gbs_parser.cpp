// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/gbs_parser.h"
#include "internal/field_parser.h"

namespace nmea { namespace parser {

// $--GBS,hhmmss.ss,x.x,x.x,x.x,xx,x.x,x.x,x.x*hh
// [0]time [1]lat_err [2]lon_err [3]alt_err [4]sv_id [5]prob [6]bias [7]std_dev

bool gbs_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 4u) return false;

    nmea_gbs msg;
    msg.talker = s.talker;

    parse_utc_time(s.field(0u), msg.utc_time);
    parse_double(s.field(1u), msg.err_lat);
    parse_double(s.field(2u), msg.err_lon);
    parse_double(s.field(3u), msg.err_alt);

    if (s.has_field(4u)) { int sv = 0; if (parse_int(s.field(4u), sv)) msg.failed_sv_id = sv; }
    if (s.has_field(5u)) parse_double(s.field(5u), msg.probability);
    if (s.has_field(6u)) parse_double(s.field(6u), msg.bias);
    if (s.has_field(7u)) parse_double(s.field(7u), msg.std_dev);

    msg.valid = true;
    if (cb_) cb_(msg);
    return true;
}

} }
