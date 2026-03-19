// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/nmea_formatter.cpp

#include "nmea_formatter.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace nmea
{
namespace generator
{

nmea_formatter::nmea_formatter(int latlon_decimals, int time_decimals)
    : latlon_decimals_(latlon_decimals)
    , time_decimals_(time_decimals)
{
}

// ── format_utc_time ──────────────────────────────────────────────────────────
// Produces "hhmmss.ss" (or "hhmmss.s" for time_decimals_=1).

std::string nmea_formatter::format_utc_time(uint8_t hour, uint8_t minute,
                                             uint8_t second, uint32_t ms) const
{
    char buf[16];
    if (time_decimals_ >= 2)
    {
        // "hhmmss.ss" — truncate to centiseconds
        unsigned cs = (ms / 10u) % 100u;
        std::snprintf(buf, sizeof(buf), "%02u%02u%02u.%02u",
                      static_cast<unsigned>(hour),
                      static_cast<unsigned>(minute),
                      static_cast<unsigned>(second),
                      cs);
    }
    else
    {
        // "hhmmss.s"
        unsigned ds = (ms / 100u) % 10u;
        std::snprintf(buf, sizeof(buf), "%02u%02u%02u.%01u",
                      static_cast<unsigned>(hour),
                      static_cast<unsigned>(minute),
                      static_cast<unsigned>(second),
                      ds);
    }
    return std::string(buf);
}

// ── format_date ──────────────────────────────────────────────────────────────
// Produces "ddmmyy".

std::string nmea_formatter::format_date(uint8_t day, uint8_t month,
                                         uint16_t year) const
{
    char buf[9];
    std::snprintf(buf, sizeof(buf), "%02u%02u%02u",
                  static_cast<unsigned>(day % 100u),
                  static_cast<unsigned>(month % 100u),
                  static_cast<unsigned>(year % 100u));
    return std::string(buf);
}

// ── format_latitude ──────────────────────────────────────────────────────────
// Converts signed decimal degrees to NMEA DDmm.mmmmm format.
// Only the numeric field is returned (no N/S indicator).

std::string nmea_formatter::format_latitude(double latitude_deg) const
{
    double abs_deg = std::fabs(latitude_deg);
    double degrees = std::floor(abs_deg);
    double minutes = (abs_deg - degrees) * 60.0;

    char fmt[32];
    std::snprintf(fmt, sizeof(fmt), "%%02.0f%%0%d.%df",
                  latlon_decimals_ + 3, latlon_decimals_);

    char buf[32];
    std::snprintf(buf, sizeof(buf), fmt, degrees, minutes);
    return std::string(buf);
}

char nmea_formatter::ns_indicator(double latitude_deg) const
{
    return (latitude_deg >= 0.0) ? 'N' : 'S';
}

// ── format_longitude ─────────────────────────────────────────────────────────
// Converts signed decimal degrees to NMEA DDDmm.mmmmm format.

std::string nmea_formatter::format_longitude(double longitude_deg) const
{
    double abs_deg = std::fabs(longitude_deg);
    double degrees = std::floor(abs_deg);
    double minutes = (abs_deg - degrees) * 60.0;

    char fmt[32];
    std::snprintf(fmt, sizeof(fmt), "%%03.0f%%0%d.%df",
                  latlon_decimals_ + 3, latlon_decimals_);

    char buf[32];
    std::snprintf(buf, sizeof(buf), fmt, degrees, minutes);
    return std::string(buf);
}

char nmea_formatter::ew_indicator(double longitude_deg) const
{
    return (longitude_deg >= 0.0) ? 'E' : 'W';
}

// ── format_double ─────────────────────────────────────────────────────────────

std::string nmea_formatter::format_double(double value, int precision) const
{
    char fmt[16];
    std::snprintf(fmt, sizeof(fmt), "%%.%df", precision);
    char buf[64];
    std::snprintf(buf, sizeof(buf), fmt, value);
    return std::string(buf);
}

// ── format_int ────────────────────────────────────────────────────────────────

std::string nmea_formatter::format_int(int value, int min_width) const
{
    if (min_width <= 0)
    {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%d", value);
        return std::string(buf);
    }
    char fmt[16];
    std::snprintf(fmt, sizeof(fmt), "%%0%dd", min_width);
    char buf[32];
    std::snprintf(buf, sizeof(buf), fmt, value);
    return std::string(buf);
}

// ── checksum_to_hex ───────────────────────────────────────────────────────────

std::string nmea_formatter::checksum_to_hex(uint8_t checksum) const
{
    char buf[3];
    std::snprintf(buf, sizeof(buf), "%02X", static_cast<unsigned>(checksum));
    return std::string(buf, 2u);
}

} // namespace generator
} // namespace nmea
