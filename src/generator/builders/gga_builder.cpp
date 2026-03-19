// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/gga_builder.cpp
//
// GGA sentence format (NMEA 0183 v4.11):
//   $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh

#include "gga_builder.h"

#include <string>

namespace nmea
{
namespace generator
{

gga_builder::gga_builder(const std::string& talker,
                          const i_nmea_formatter& fmt,
                          const i_nmea_checksum& cs,
                          bool enabled)
    : sentence_builder_base(talker, fmt, cs, enabled)
{}

void gga_builder::prepare(const gga_input& input)
{
    input_       = input;
    input_valid_ = input.valid;
}

std::string gga_builder::build() const
{
    if (!enabled_ || !input_valid_)
        return std::string();

    const gga_input& d = input_;

    // [0] hhmmss.ss
    std::string fields = fmt_.format_utc_time(d.hour, d.minute, d.second, d.millisecond);
    fields += ',';

    // [1-2] Latitude, N/S
    fields += fmt_.format_latitude(d.latitude_deg);
    fields += ',';
    fields += fmt_.ns_indicator(d.latitude_deg);
    fields += ',';

    // [3-4] Longitude, E/W
    fields += fmt_.format_longitude(d.longitude_deg);
    fields += ',';
    fields += fmt_.ew_indicator(d.longitude_deg);
    fields += ',';

    // [5] Fix quality
    fields += fmt_.format_int(static_cast<int>(d.fix_quality));
    fields += ',';

    // [6] Number of satellites
    fields += fmt_.format_int(static_cast<int>(d.num_satellites), 2);
    fields += ',';

    // [7] HDOP (empty when 0.0 — indicates unavailable)
    if (d.hdop > 0.0)
        fields += fmt_.format_double(d.hdop, 1);
    fields += ',';

    // [8-9] Altitude MSL, unit 'M'
    fields += fmt_.format_double(d.altitude_msl_m, 1);
    fields += ',';
    fields += 'M';
    fields += ',';

    // [10-11] Geoid separation, unit 'M'
    fields += fmt_.format_double(d.geoid_sep_m, 1);
    fields += ',';
    fields += 'M';
    fields += ',';

    // [12] Age of DGPS correction (empty when not available)
    if (d.dgps_age_s >= 0.0)
        fields += fmt_.format_double(d.dgps_age_s, 1);
    fields += ',';

    // [13] DGPS reference station ID (empty when not available)
    if (d.dgps_age_s >= 0.0)
        fields += fmt_.format_int(static_cast<int>(d.dgps_station_id));

    return assemble(sentence_type(), fields);
}

} // namespace generator
} // namespace nmea
