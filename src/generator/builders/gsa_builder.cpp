// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/gsa_builder.cpp
//
// GSA sentence format (NMEA 0183 v4.11):
//   $--GSA,a,x,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x,x*hh
//   Fields [0..1]: op_mode, nav_mode
//   Fields [2..13]: up to 12 satellite IDs (empty = unused)
//   Fields [14..16]: PDOP, HDOP, VDOP
//   Field  [17]:    system_id (NMEA 4.11; omit when 0)

#include "gsa_builder.h"

namespace nmea
{
namespace generator
{

gsa_builder::gsa_builder(const std::string& talker,
                          const i_nmea_formatter& fmt,
                          const i_nmea_checksum& cs,
                          bool enabled)
    : sentence_builder_base(talker, fmt, cs, enabled)
{}

void gsa_builder::prepare(const gsa_input& input)
{
    input_       = input;
    input_valid_ = input.valid;
}

std::string gsa_builder::build() const
{
    if (!enabled_ || !input_valid_)
        return std::string();

    const gsa_input& d = input_;

    // [0] Op mode: A or M
    std::string fields;
    fields += d.op_mode;
    fields += ',';

    // [1] Nav mode: 1/2/3
    fields += fmt_.format_int(static_cast<int>(d.nav_mode));
    fields += ',';

    // [2..13] Satellite IDs (0 = empty slot → empty field)
    for (int i = 0; i < 12; ++i)
    {
        if (d.sat_ids[i] != 0u)
            fields += fmt_.format_int(static_cast<int>(d.sat_ids[i]), 2);
        if (i < 11)
            fields += ',';
    }
    fields += ',';

    // [14] PDOP (empty when 0)
    if (d.pdop > 0.0)
        fields += fmt_.format_double(d.pdop, 2);
    fields += ',';

    // [15] HDOP
    if (d.hdop > 0.0)
        fields += fmt_.format_double(d.hdop, 2);
    fields += ',';

    // [16] VDOP
    if (d.vdop > 0.0)
        fields += fmt_.format_double(d.vdop, 2);

    // [17] System ID (NMEA 4.11) — omit field when 0
    if (d.system_id != 0u)
    {
        fields += ',';
        fields += fmt_.format_int(static_cast<int>(d.system_id));
    }

    return assemble(sentence_type(), fields);
}

} // namespace generator
} // namespace nmea
