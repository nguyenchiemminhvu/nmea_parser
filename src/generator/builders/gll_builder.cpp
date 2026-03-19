// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/gll_builder.cpp
//
// GLL sentence format:
//   $--GLL,llll.ll,a,yyyyy.yy,a,hhmmss.ss,A,a*hh

#include "gll_builder.h"

namespace nmea
{
namespace generator
{

gll_builder::gll_builder(const std::string& talker,
                          const i_nmea_formatter& fmt,
                          const i_nmea_checksum& cs,
                          bool enabled)
    : sentence_builder_base(talker, fmt, cs, enabled)
{}

void gll_builder::prepare(const gll_input& input)
{
    input_       = input;
    input_valid_ = input.valid;
}

std::string gll_builder::build() const
{
    if (!enabled_ || !input_valid_)
        return std::string();

    const gll_input& d = input_;

    // [0-1] Latitude, N/S
    std::string fields = fmt_.format_latitude(d.latitude_deg);
    fields += ',';
    fields += fmt_.ns_indicator(d.latitude_deg);
    fields += ',';

    // [2-3] Longitude, E/W
    fields += fmt_.format_longitude(d.longitude_deg);
    fields += ',';
    fields += fmt_.ew_indicator(d.longitude_deg);
    fields += ',';

    // [4] UTC time
    fields += fmt_.format_utc_time(d.hour, d.minute, d.second, d.millisecond);
    fields += ',';

    // [5] Status: A=active, V=void
    fields += (d.status_active ? 'A' : 'V');
    fields += ',';

    // [6] Mode indicator (NMEA 2.3+)
    fields += d.mode;

    return assemble(sentence_type(), fields);
}

} // namespace generator
} // namespace nmea
