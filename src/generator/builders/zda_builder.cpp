// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/zda_builder.cpp
//
// ZDA sentence format:
//   $--ZDA,hhmmss.ss,dd,mm,yyyy,hh,mm*hh

#include "zda_builder.h"

#include <cstdio>

namespace nmea
{
namespace generator
{

zda_builder::zda_builder(const std::string& talker,
                          const i_nmea_formatter& fmt,
                          const i_nmea_checksum& cs,
                          bool enabled)
    : sentence_builder_base(talker, fmt, cs, enabled)
{}

void zda_builder::prepare(const zda_input& input)
{
    input_       = input;
    input_valid_ = input.valid;
}

std::string zda_builder::build() const
{
    if (!enabled_ || !input_valid_)
        return std::string();

    const zda_input& d = input_;

    // [0] hhmmss.ss
    std::string fields = fmt_.format_utc_time(d.hour, d.minute, d.second, d.millisecond);
    fields += ',';

    // [1] day
    fields += fmt_.format_int(static_cast<int>(d.day), 2);
    fields += ',';

    // [2] month
    fields += fmt_.format_int(static_cast<int>(d.month), 2);
    fields += ',';

    // [3] year (4 digits)
    fields += fmt_.format_int(static_cast<int>(d.year), 4);
    fields += ',';

    // [4] local zone hours — NMEA: signed 2-digit. For UTC output this is 00.
    fields += fmt_.format_int(static_cast<int>(d.tz_hour), 2);
    fields += ',';

    // [5] local zone minutes
    fields += fmt_.format_int(static_cast<int>(d.tz_min), 2);

    return assemble(sentence_type(), fields);
}

} // namespace generator
} // namespace nmea
