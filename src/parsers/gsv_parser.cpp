// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "parsers/gsv_parser.h"
#include "internal/field_parser.h"

namespace nmea
{
namespace parser
{

// $--GSV,x,x,xx[,xx,xx,xxx,xx]*hh
// [0] num_msgs  [1] msg_num  [2] sats_in_view
// [3..6] sv1: sv_id, elevation, azimuth, snr (repeating block up to x4)
// [last] optional signal_id (NMEA 4.11)

bool gsv_parser::parse(const nmea_sentence& s)
{
    using namespace internal;

    if (s.field_count() < 3u)
        return false;

    nmea_gsv msg;
    msg.talker = s.talker;

    int num_msgs = 0, msg_num = 0, sats = 0;
    if (!parse_int(s.field(0u), num_msgs)) return false;
    if (!parse_int(s.field(1u), msg_num))  return false;
    if (!parse_int(s.field(2u), sats))     return false;

    msg.num_msgs     = static_cast<uint8_t>(num_msgs);
    msg.msg_num      = static_cast<uint8_t>(msg_num);
    msg.sats_in_view = static_cast<uint8_t>(sats);

    // Each satellite block is 4 fields starting at index 3
    // Figure out if the last field is a signal_id (NMEA 4.11).
    // Satellite data fields = (field_count - 3) rounded down to multiple of 4.
    const size_t remaining = (s.field_count() > 3u) ? (s.field_count() - 3u) : 0u;
    const size_t sat_fields = (remaining / 4u) * 4u;
    const size_t sig_field  = 3u + sat_fields;
    const size_t num_sats   = sat_fields / 4u;

    // Optional signal_id
    if (sig_field < s.field_count() && s.has_field(sig_field))
    {
        int sig = 0;
        if (parse_int(s.field(sig_field), sig))
            msg.signal_id = static_cast<uint8_t>(sig);
    }

    msg.sat_count = static_cast<uint8_t>(num_sats > 4u ? 4u : num_sats);

    for (uint8_t i = 0u; i < msg.sat_count; ++i)
    {
        const size_t base = 3u + static_cast<size_t>(i) * 4u;
        gsv_satellite& sv = msg.satellites[i];

        int sv_id = 0;
        if (parse_int(s.field(base + 0u), sv_id))
            sv.sv_id = static_cast<uint8_t>(sv_id);

        int elev = 0;
        if (s.has_field(base + 1u) && parse_int(s.field(base + 1u), elev))
            sv.elevation = static_cast<int8_t>(elev);
        else
            sv.elevation = -1;

        int az = 0;
        if (s.has_field(base + 2u) && parse_int(s.field(base + 2u), az))
            sv.azimuth = static_cast<uint16_t>(az);

        int snr = 0;
        if (s.has_field(base + 3u) && parse_int(s.field(base + 3u), snr))
            sv.snr = static_cast<int8_t>(snr);
        else
            sv.snr = -1;
    }

    msg.valid = true;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace nmea
