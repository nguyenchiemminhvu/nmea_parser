// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// include/generator/nmea_generator_config.h
//
// Configuration object for nmea_generator_facade.
// Controls talker ID, per-sentence enable/disable flags, and
// output formatting options.

#pragma once

#include <string>

namespace nmea
{
namespace generator
{

// ─── nmea_generator_config ───────────────────────────────────────────────────

struct nmea_generator_config
{
    // ── Talker ID ─────────────────────────────────────────────────────────────

    /// Two-character talker ID prefix for all generated sentences.
    /// Common values: "GN" (combined GNSS), "GP" (GPS only), "GL" (GLONASS only).
    /// GSV sentences override this per-constellation (see below).
    std::string talker_id = "GN";

    // ── Per-sentence enable flags ─────────────────────────────────────────────

    bool enable_gga = true;
    bool enable_rmc = true;
    bool enable_gsa = true;
    bool enable_gsv = true;
    bool enable_vtg = true;
    bool enable_gll = true;
    bool enable_zda = true;

    // ── GSV per-constellation enable ──────────────────────────────────────────
    // GSV sentences carry a per-constellation talker (GPGSV, GLGSV, etc.).
    // These flags let callers suppress individual constellations.

    bool enable_gsv_gps      = true;
    bool enable_gsv_glonass  = true;
    bool enable_gsv_galileo  = true;
    bool enable_gsv_beidou   = true;
    bool enable_gsv_qzss     = true;

    // ── Output formatting ─────────────────────────────────────────────────────

    /// Number of decimal digits for latitude/longitude minutes (4 or 5).
    /// Use 5 for sub-meter precision.  NMEA standard minimum is 4.
    int latlon_decimal_digits = 5;

    /// Number of decimal digits for UTC time seconds (1 or 2).
    int time_decimal_digits = 2;
};

} // namespace generator
} // namespace nmea
