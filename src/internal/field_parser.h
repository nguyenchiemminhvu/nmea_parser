// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// internal/field_parser.h
//
// Utility functions for parsing individual NMEA sentence fields into
// native C++ types.
//
// NMEA field conventions:
//   - Empty string  → optional field not present
//   - Latitude      → "DDmm.mmmm" (degrees + decimal minutes, N/S indicator)
//   - Longitude     → "DDDmm.mmmm" (degrees + decimal minutes, E/W indicator)
//   - UTC time      → "hhmmss.ss" (stored as double for compatibility)
//   - UTC date      → "ddmmyy" (stored as uint32_t)
//
// Thread-safety: stateless — safe to call from any thread.

#pragma once

#include <cstdint>
#include <string>
#include <cstdlib>
#include <cmath>

namespace nmea
{
namespace parser
{
namespace internal
{

// ─── parse_double ─────────────────────────────────────────────────────────────

inline bool parse_double(const std::string& s, double& out)
{
    if (s.empty()) return false;
    char* end = nullptr;
    double v = std::strtod(s.c_str(), &end);
    if (end == s.c_str()) return false;  // no conversion
    out = v;
    return true;
}

// ─── parse_int ────────────────────────────────────────────────────────────────

inline bool parse_int(const std::string& s, int& out, int base = 10)
{
    if (s.empty()) return false;
    char* end = nullptr;
    long v = std::strtol(s.c_str(), &end, base);
    if (end == s.c_str()) return false;
    out = static_cast<int>(v);
    return true;
}

// ─── parse_uint ───────────────────────────────────────────────────────────────

inline bool parse_uint(const std::string& s, unsigned int& out, int base = 10)
{
    if (s.empty()) return false;
    char* end = nullptr;
    unsigned long v = std::strtoul(s.c_str(), &end, base);
    if (end == s.c_str()) return false;
    out = static_cast<unsigned int>(v);
    return true;
}

// ─── parse_latlon ─────────────────────────────────────────────────────────────
//
// Convert NMEA DDmm.mmmm (or DDDmm.mmmm) + direction character to decimal
// degrees.  Returns false if @p value is empty or unparseable.

inline bool parse_latlon(const std::string& value,
                         const std::string& dir,
                         double& out)
{
    if (value.empty()) return false;

    double raw = 0.0;
    if (!parse_double(value, raw)) return false;

    // Degrees are the integer part divided by 100 (e.g. 4717.11399 → 47°)
    double degrees  = std::floor(raw / 100.0);
    double minutes  = raw - degrees * 100.0;
    out = degrees + minutes / 60.0;

    if (!dir.empty())
    {
        char d = dir[0];
        if (d == 'S' || d == 's' || d == 'W' || d == 'w')
            out = -out;
    }
    return true;
}

// ─── parse_utc_time ───────────────────────────────────────────────────────────
//
// Store hhmmss.ss as a double (raw value, e.g. 92725.00).
// Callers that need structured time should compute:
//   hour   = floor(utc_time / 10000)
//   minute = floor(fmod(utc_time, 10000) / 100)
//   second = fmod(utc_time, 100)

inline bool parse_utc_time(const std::string& s, double& out)
{
    return parse_double(s, out);
}

// ─── parse_utc_date ───────────────────────────────────────────────────────────
//
// Store ddmmyy as a uint32_t raw value (e.g. 160223).

inline bool parse_utc_date(const std::string& s, uint32_t& out)
{
    if (s.empty()) return false;
    unsigned int v = 0u;
    if (!parse_uint(s, v)) return false;
    out = static_cast<uint32_t>(v);
    return true;
}

// ─── parse_char ───────────────────────────────────────────────────────────────

inline bool parse_char(const std::string& s, char& out)
{
    if (s.empty()) return false;
    out = s[0];
    return true;
}

} // namespace internal
} // namespace parser
} // namespace nmea
