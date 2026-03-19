// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// include/generator/nmea_sentence_inputs.h
//
// Input DTOs (Data Transfer Objects) for each supported NMEA sentence type.
//
// These structs are the "clean" decoupled input layer between the caller's
// data source (e.g. UBX shared buffers) and the sentence builders.
//
// Conventions:
//   • All angular values are in decimal degrees unless noted otherwise.
//   • Speed values carry their unit in the field name (_knots / _kmh / _mmps).
//   • Sentinel "invalid" values: -1 for indices/counts, NAN-like defaults
//     represented by the comment // optional — leave 0 or negative if unknown.
//   • date_ddmmyy = day * 10000u + month * 100u + (year % 100u).

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace nmea
{
namespace generator
{

// ─── gga_input ────────────────────────────────────────────────────────────────
//
// Input for $--GGA sentence (Global Positioning System Fix Data).

struct gga_input
{
    uint8_t  hour          = 0u;
    uint8_t  minute        = 0u;
    uint8_t  second        = 0u;
    uint32_t millisecond   = 0u;

    double   latitude_deg  = 0.0;   ///< Signed: positive = North
    double   longitude_deg = 0.0;   ///< Signed: positive = East

    uint8_t  fix_quality      = 0u; ///< 0=invalid 1=GPS 2=DGPS 4=RTK-fix 5=RTK-float 6=DR
    uint8_t  num_satellites   = 0u;
    double   hdop             = 0.0; ///< 0.0 if unavailable (outputs empty field)
    double   altitude_msl_m   = 0.0; ///< Metres above MSL
    double   geoid_sep_m      = 0.0; ///< Geoid separation [m]; positive = above WGS-84 ellipsoid

    double   dgps_age_s       = -1.0; ///< Age of DGPS correction [s]; < 0 = not available
    uint16_t dgps_station_id  = 0u;   ///< DGPS ref station ID; used only when dgps_age_s >= 0

    bool     valid            = false;
};

// ─── rmc_input ────────────────────────────────────────────────────────────────
//
// Input for $--RMC sentence (Recommended Minimum Specific GNSS Data).

struct rmc_input
{
    uint8_t  hour          = 0u;
    uint8_t  minute        = 0u;
    uint8_t  second        = 0u;
    uint32_t millisecond   = 0u;

    bool     status_active = false; ///< true = A (active/valid), false = V (void)

    double   latitude_deg  = 0.0;
    double   longitude_deg = 0.0;

    double   speed_knots   = 0.0;   ///< Speed over ground [knots]
    double   course_true   = 0.0;   ///< Course over ground, true [degrees]

    uint8_t  day           = 0u;
    uint8_t  month         = 0u;
    uint16_t year          = 0u;

    double   mag_variation = 0.0;   ///< Magnetic variation [degrees]; positive = East
    bool     mag_var_valid = false; ///< false → leave mag_variation field empty

    char     mode          = 'N';   ///< A/D/E/N/S (NMEA 2.3+)

    bool     valid         = false;
};

// ─── gsa_input ────────────────────────────────────────────────────────────────
//
// Input for $--GSA sentence (GNSS DOP and Active Satellites).

struct gsa_input
{
    char    op_mode   = 'A';   ///< A = auto 2D/3D, M = manual
    uint8_t nav_mode  = 1u;    ///< 1=no fix, 2=2D, 3=3D

    /// Satellite IDs used in the navigation solution (0 = unused slot).
    /// Up to 12 entries per system (NMEA standard).
    uint8_t sat_ids[12];

    double  pdop      = 0.0;
    double  hdop      = 0.0;
    double  vdop      = 0.0;

    /// NMEA 4.11 system ID.
    /// 0 = omit field, 1=GPS, 2=GLONASS, 3=Galileo, 4=BeiDou, 5=QZSS
    uint8_t system_id = 0u;

    bool    valid     = false;

    gsa_input() { for (int i = 0; i < 12; ++i) sat_ids[i] = 0u; }
};

// ─── gsv_satellite_record ─────────────────────────────────────────────────────

struct gsv_satellite_record
{
    uint8_t  sv_id         = 0u;
    int8_t   elevation_deg = 0;   ///< -1 if not available
    int16_t  azimuth_deg   = 0;
    uint8_t  snr           = 0u;  ///< dBHz; 0 = not tracking
};

// ─── gsv_input ────────────────────────────────────────────────────────────────
//
// Input for one constellation's GSV sentence set.
// The builder splits this into multiple sentences of ≤4 satellites each.

struct gsv_input
{
    std::string talker;                          ///< e.g. "GP", "GL", "GA", "GB", "GQ"
    std::vector<gsv_satellite_record> satellites; ///< All SVs for this constellation
    bool valid = false;
};

// ─── vtg_input ────────────────────────────────────────────────────────────────
//
// Input for $--VTG sentence (Course Over Ground and Ground Speed).

struct vtg_input
{
    double course_true     = 0.0;  ///< Course over ground, true north [degrees]
    double course_mag      = 0.0;  ///< Course over ground, magnetic [degrees]
    bool   course_mag_valid = false;

    double speed_knots     = 0.0;
    double speed_kmh       = 0.0;

    char   mode            = 'N';  ///< A/D/E/N/S

    bool   valid           = false;
};

// ─── gll_input ────────────────────────────────────────────────────────────────
//
// Input for $--GLL sentence (Geographic Position – Latitude/Longitude).

struct gll_input
{
    double   latitude_deg  = 0.0;
    double   longitude_deg = 0.0;

    uint8_t  hour          = 0u;
    uint8_t  minute        = 0u;
    uint8_t  second        = 0u;
    uint32_t millisecond   = 0u;

    bool     status_active = false;
    char     mode          = 'N';  ///< A/D/E/N/S

    bool     valid         = false;
};

// ─── zda_input ────────────────────────────────────────────────────────────────
//
// Input for $--ZDA sentence (Time and Date).

struct zda_input
{
    uint8_t  hour          = 0u;
    uint8_t  minute        = 0u;
    uint8_t  second        = 0u;
    uint32_t millisecond   = 0u;

    uint8_t  day           = 0u;
    uint8_t  month         = 0u;
    uint16_t year          = 0u;

    int8_t   tz_hour       = 0;   ///< Local zone offset hours [-13..+13]; use 0 for UTC
    uint8_t  tz_min        = 0u;  ///< Local zone offset minutes [0 or 30 or 45]

    bool     valid         = false;
};

} // namespace generator
} // namespace nmea
