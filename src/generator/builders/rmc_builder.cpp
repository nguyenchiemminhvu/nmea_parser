// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/rmc_builder.cpp
//
// RMC sentence format (NMEA 0183 v4.11):
//   $--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a,a*hh

#include "rmc_builder.h"

#include <cmath>

namespace nmea
{
namespace generator
{

rmc_builder::rmc_builder(const std::string& talker,
                          const i_nmea_formatter& fmt,
                          const i_nmea_checksum& cs,
                          bool enabled)
    : sentence_builder_base(talker, fmt, cs, enabled)
{}

void rmc_builder::prepare(const rmc_input& input)
{
    input_       = input;
    input_valid_ = input.valid;
}

std::string rmc_builder::build() const
{
    if (!enabled_ || !input_valid_)
        return std::string();

    const rmc_input& d = input_;

    // [0] hhmmss.ss
    std::string fields = fmt_.format_utc_time(d.hour, d.minute, d.second, d.millisecond);
    fields += ',';

    // [1] Status: A=active, V=void
    fields += (d.status_active ? 'A' : 'V');
    fields += ',';

    // [2-3] Latitude, N/S
    fields += fmt_.format_latitude(d.latitude_deg);
    fields += ',';
    fields += fmt_.ns_indicator(d.latitude_deg);
    fields += ',';

    // [4-5] Longitude, E/W
    fields += fmt_.format_longitude(d.longitude_deg);
    fields += ',';
    fields += fmt_.ew_indicator(d.longitude_deg);
    fields += ',';

    // [6] Speed over ground [knots]
    fields += fmt_.format_double(d.speed_knots, 2);
    fields += ',';

    // [7] Course over ground, true [degrees]
    fields += fmt_.format_double(d.course_true, 2);
    fields += ',';

    // [8] Date ddmmyy
    fields += fmt_.format_date(d.day, d.month, d.year);
    fields += ',';

    // [9-10] Magnetic variation, direction (omit when not available)
    if (d.mag_var_valid)
    {
        fields += fmt_.format_double(std::abs(d.mag_variation), 1);
        fields += ',';
        fields += (d.mag_variation >= 0.0) ? 'E' : 'W';
    }
    else
    {
        fields += ',';
    }
    fields += ',';

    // [11] Mode indicator (NMEA 2.3+)
    fields += d.mode;

    return assemble(sentence_type(), fields);
}

} // namespace generator
} // namespace nmea
