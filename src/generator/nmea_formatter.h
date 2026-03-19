// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/nmea_formatter.h
//
// Concrete implementation of i_nmea_formatter.
// Handles all field-level NMEA encoding.
//
// All methods are thread-safe (stateless) after construction.

#pragma once

#include "generator/i_nmea_formatter.h"

namespace nmea
{
namespace generator
{

// ─── nmea_formatter ──────────────────────────────────────────────────────────

class nmea_formatter : public i_nmea_formatter
{
public:
    /// Construct with the number of decimal digits for lat/lon and time fields.
    /// @p latlon_decimals  4 or 5 (default 5 for sub-metre precision).
    /// @p time_decimals    1 or 2 (default 2).
    explicit nmea_formatter(int latlon_decimals = 5, int time_decimals = 2);

    // Provide copy assignment to allow re-assignment in pImpl rebuild_builders().
    // The base class deletes its operator= to prevent slicing via base ref;
    // the derived class defines it explicitly to only copy its own members.
    nmea_formatter& operator=(const nmea_formatter& other) noexcept
    {
        if (this != &other)
        {
            latlon_decimals_ = other.latlon_decimals_;
            time_decimals_   = other.time_decimals_;
        }
        return *this;
    }

    // ── i_nmea_formatter ──────────────────────────────────────────────────────

    std::string format_utc_time(uint8_t hour, uint8_t minute,
                                uint8_t second, uint32_t ms) const override;

    std::string format_date(uint8_t day, uint8_t month,
                            uint16_t year) const override;

    std::string format_latitude(double latitude_deg)  const override;
    char        ns_indicator(double latitude_deg)     const override;
    std::string format_longitude(double longitude_deg) const override;
    char        ew_indicator(double longitude_deg)    const override;

    std::string format_double(double value, int precision) const override;
    std::string format_int(int value, int min_width = 0)   const override;

    std::string checksum_to_hex(uint8_t checksum) const override;

private:
    int latlon_decimals_;
    int time_decimals_;
};

} // namespace generator
} // namespace nmea
