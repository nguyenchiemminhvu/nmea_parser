// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// include/generator/i_nmea_formatter.h
//
// Abstraction for NMEA field encoding.
// Responsible for converting raw numeric/boolean values into the
// wire-format strings required by NMEA 0183.

#pragma once

#include <cstdint>
#include <string>

namespace nmea
{
namespace generator
{

// ─── i_nmea_formatter ────────────────────────────────────────────────────────

class i_nmea_formatter
{
public:
    virtual ~i_nmea_formatter() = default;

    i_nmea_formatter()                                 = default;
    i_nmea_formatter(const i_nmea_formatter&)          = delete;
    i_nmea_formatter& operator=(const i_nmea_formatter&) = delete;

    // ── Time / date ───────────────────────────────────────────────────────────

    /// Encode UTC time as "hhmmss.ss" from individual components.
    /// @p ms  milliseconds within the current second [0..999].
    virtual std::string format_utc_time(uint8_t hour, uint8_t minute,
                                        uint8_t second, uint32_t ms) const = 0;

    /// Encode date as "ddmmyy" from individual components.
    virtual std::string format_date(uint8_t day, uint8_t month,
                                    uint16_t year) const = 0;

    // ── Position ──────────────────────────────────────────────────────────────

    /// Encode latitude magnitude in NMEA DDmm.mmmmm format.
    /// Does NOT include the N/S indicator.
    /// @p latitude_deg  Signed decimal degrees (positive = North).
    virtual std::string format_latitude(double latitude_deg) const = 0;

    /// Return 'N' for North or 'S' for South based on the sign of the value.
    virtual char ns_indicator(double latitude_deg) const = 0;

    /// Encode longitude magnitude in NMEA DDDmm.mmmmm format.
    /// Does NOT include the E/W indicator.
    /// @p longitude_deg  Signed decimal degrees (positive = East).
    virtual std::string format_longitude(double longitude_deg) const = 0;

    /// Return 'E' for East or 'W' for West based on the sign of the value.
    virtual char ew_indicator(double longitude_deg) const = 0;

    // ── Numeric fields ────────────────────────────────────────────────────────

    /// Format a floating-point value with @p precision decimal places.
    virtual std::string format_double(double value, int precision) const = 0;

    /// Format an integer with optional zero-padding to @p min_width digits.
    virtual std::string format_int(int value, int min_width = 0) const = 0;

    // ── Checksum ──────────────────────────────────────────────────────────────

    /// Convert a raw checksum byte to a two-character uppercase hex string.
    virtual std::string checksum_to_hex(uint8_t checksum) const = 0;
};

} // namespace generator
} // namespace nmea
