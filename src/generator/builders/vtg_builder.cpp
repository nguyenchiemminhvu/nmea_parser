// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/vtg_builder.cpp
//
// VTG sentence format:
//   $--VTG,x.x,T,x.x,M,x.x,N,x.x,K,a*hh

#include "vtg_builder.h"

namespace nmea
{
namespace generator
{

vtg_builder::vtg_builder(const std::string& talker,
                          const i_nmea_formatter& fmt,
                          const i_nmea_checksum& cs,
                          bool enabled)
    : sentence_builder_base(talker, fmt, cs, enabled)
{}

void vtg_builder::prepare(const vtg_input& input)
{
    input_       = input;
    input_valid_ = input.valid;
}

std::string vtg_builder::build() const
{
    if (!enabled_ || !input_valid_)
        return std::string();

    const vtg_input& d = input_;

    // [0-1] Course true, 'T'
    std::string fields = fmt_.format_double(d.course_true, 2);
    fields += ',';
    fields += 'T';
    fields += ',';

    // [2-3] Course magnetic, 'M' (empty when not valid)
    if (d.course_mag_valid)
        fields += fmt_.format_double(d.course_mag, 2);
    fields += ',';
    fields += 'M';
    fields += ',';

    // [4-5] Speed knots, 'N'
    fields += fmt_.format_double(d.speed_knots, 2);
    fields += ',';
    fields += 'N';
    fields += ',';

    // [6-7] Speed km/h, 'K'
    fields += fmt_.format_double(d.speed_kmh, 2);
    fields += ',';
    fields += 'K';
    fields += ',';

    // [8] Mode indicator
    fields += d.mode;

    return assemble(sentence_type(), fields);
}

} // namespace generator
} // namespace nmea
