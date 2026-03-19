// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/gsv_builder.cpp
//
// GSV sentence format (NMEA 0183 v4.11):
//   $--GSV,x,x,xx,sv,sv,sv,sv*hh
//
// Fields:
//   [0]  total number of GSV messages in this set
//   [1]  this message number (1-based)
//   [2]  total SVs in view
//   [3+] up to 4 blocks of: sv_id, elevation[°], azimuth[°], SNR[dBHz]
//        (SNR omitted if 0)

#include "gsv_builder.h"

#include <cmath>

namespace nmea
{
namespace generator
{

static constexpr uint8_t SATS_PER_GSV_MSG = 4u;

gsv_builder::gsv_builder(const std::string& talker,
                          const i_nmea_formatter& fmt,
                          const i_nmea_checksum& cs,
                          bool enabled)
    : sentence_builder_base(talker, fmt, cs, enabled)
{}

void gsv_builder::prepare(const gsv_input& input)
{
    input_       = input;
    input_valid_ = input.valid;
}

std::string gsv_builder::build() const
{
    auto all = build_all();
    if (all.empty())
        return std::string();
    return all[0];
}

std::vector<std::string> gsv_builder::build_all() const
{
    std::vector<std::string> result;

    if (!enabled_ || !input_valid_)
        return result;

    const gsv_input& d = input_;
    const uint8_t total_sats = static_cast<uint8_t>(d.satellites.size());
    const uint8_t total_msgs = (total_sats == 0u)
        ? 1u
        : static_cast<uint8_t>((total_sats + SATS_PER_GSV_MSG - 1u) / SATS_PER_GSV_MSG);

    for (uint8_t msg = 1u; msg <= total_msgs; ++msg)
    {
        uint8_t offset = static_cast<uint8_t>((msg - 1u) * SATS_PER_GSV_MSG);
        result.push_back(build_one(d, msg, total_msgs, offset));
    }

    return result;
}

std::string gsv_builder::build_one(const gsv_input& d,
                                    uint8_t msg_num,
                                    uint8_t total_msgs,
                                    uint8_t sat_offset) const
{
    const uint8_t total_sats = static_cast<uint8_t>(d.satellites.size());

    // [0] total messages
    std::string fields = fmt_.format_int(static_cast<int>(total_msgs));
    fields += ',';

    // [1] this message number
    fields += fmt_.format_int(static_cast<int>(msg_num));
    fields += ',';

    // [2] total SVs in view
    fields += fmt_.format_int(static_cast<int>(total_sats), 2);

    // [3+] satellite blocks (up to 4)
    const uint8_t count_in_msg = static_cast<uint8_t>(
        (sat_offset + SATS_PER_GSV_MSG <= total_sats)
            ? SATS_PER_GSV_MSG
            : (total_sats - sat_offset));

    for (uint8_t i = 0u; i < count_in_msg; ++i)
    {
        const gsv_satellite_record& sv = d.satellites[sat_offset + i];
        fields += ',';
        fields += fmt_.format_int(static_cast<int>(sv.sv_id));
        fields += ',';
        // Elevation: empty if -1
        if (sv.elevation_deg >= 0)
            fields += fmt_.format_int(static_cast<int>(sv.elevation_deg));
        fields += ',';
        fields += fmt_.format_int(static_cast<int>(sv.azimuth_deg));
        fields += ',';
        // SNR: empty if 0 (not tracking)
        if (sv.snr > 0u)
            fields += fmt_.format_int(static_cast<int>(sv.snr));
    }

    // Use per-constellation talker from the DTO (overrides class talker_).
    const std::string& use_talker = d.talker.empty() ? talker_ : d.talker;

    // Compute checksum over "<talker>GSV,<fields>"
    std::string content;
    content.reserve(80u);
    content += use_talker;
    content += "GSV";
    content += ',';
    content += fields;

    uint8_t raw_cs = cs_.calculate(content);

    std::string sentence;
    sentence += '$';
    sentence += content;
    sentence += '*';
    sentence += fmt_.checksum_to_hex(raw_cs);
    sentence += "\r\n";
    return sentence;
}

} // namespace generator
} // namespace nmea
