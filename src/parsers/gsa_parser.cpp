// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/gsa_parser.h"
#include "internal/field_parser.h"

namespace nmea
{
namespace parser
{

// $--GSA,a,x,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x[,x]*hh
// Fields [0..15] or [0..16] (NMEA 4.11 adds system_id as field [16])

bool gsa_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 17u)
        return false;

    nmea_gsa msg;
    msg.talker = s.talker;

    // [0] Op mode
    char op = '\0';
    if (parse_char(s.field(0u), op))
    {
        if (op == 'A' || op == 'a')      msg.op_mode = gsa_op_mode::auto_mode;
        else if (op == 'M' || op == 'm') msg.op_mode = gsa_op_mode::manual;
    }

    // [1] Nav mode
    int nav = 0;
    if (parse_int(s.field(1u), nav))
        msg.nav_mode = static_cast<gsa_nav_mode>(nav);

    // [2..13] Satellite IDs (12 slots; empty = unused)
    for (int i = 0; i < 12; ++i)
    {
        int sv = 0;
        if (parse_int(s.field(static_cast<size_t>(2 + i)), sv))
            msg.sat_ids[static_cast<size_t>(i)] = static_cast<uint8_t>(sv);
        else
            msg.sat_ids[static_cast<size_t>(i)] = 0u;
    }

    // [14] PDOP
    parse_double(s.field(14u), msg.pdop);

    // [15] HDOP
    parse_double(s.field(15u), msg.hdop);

    // [16] VDOP
    parse_double(s.field(16u), msg.vdop);

    // [17] System ID (NMEA 4.11+, optional)
    if (s.field_count() > 17u && s.has_field(17u))
    {
        int sys = 0;
        if (parse_int(s.field(17u), sys))
            msg.system_id = static_cast<uint8_t>(sys);
    }

    msg.valid = true;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace nmea
