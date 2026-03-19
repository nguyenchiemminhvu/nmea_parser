// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/shared_buffer_mapper.h
//
// Decoupled mapping layer: translates gnss:: shared-buffer structs into
// the generator DTOs consumed by the sentence builders.
//
// Design:
//   • Pure free functions — no state, no dependencies on builders.
//   • Each function maps one source struct (or a combination) to one DTO.
//   • Conversion decisions (unit changes, fix_type → fix_quality, etc.)
//     are documented inline.
//   • When a required field cannot be derived from current shared buffers
//     (e.g. DOP values), the DTO field is left at its default (0.0) and
//     the caller may inject supplementary data.

#pragma once

#include "generator/nmea_sentence_inputs.h"

#include "shared_buffer/location_data_buffer.h"
#include "shared_buffer/satellites_data_buffer.h"
#include "shared_buffer/timing_data_buffer.h"
#include "shared_buffer/dop_data_buffer.h"

#include <cmath>
#include <vector>

namespace nmea
{
namespace generator
{
namespace shared_buffer_mapper
{

// ── Internal helpers ──────────────────────────────────────────────────────────

namespace detail
{

/// Map UBX fix_type + flags2 to NMEA GGA fix_quality.
/// UBX-NAV-PVT flags2 bits 6:5 = carrSoln (0=none, 1=float, 2=fixed).
inline uint8_t fix_type_to_quality(uint8_t fix_type, uint8_t flags2)
{
    const uint8_t carr_soln = static_cast<uint8_t>((flags2 >> 5u) & 0x03u);
    switch (fix_type)
    {
        case 0u: return 0u; // invalid
        case 1u: return 6u; // dead reckoning
        case 2u: // fall-through
        case 3u:
            if (carr_soln == 2u) return 4u; // RTK fixed
            if (carr_soln == 1u) return 5u; // RTK float
            return 1u; // autonomous GPS
        case 4u: return 6u; // GNSS + DR
        case 5u: return 0u; // time-only — no position fix
        default: return 0u;
    }
}

/// Derive RMC mode character from fix_type and gnssFixOK flag.
/// gnssFixOK = flags bit 0.
inline char fix_type_to_rmc_mode(uint8_t fix_type, uint8_t flags)
{
    const bool gnss_fix_ok = ((flags & 0x01u) != 0u);
    if (!gnss_fix_ok)
        return 'N'; // not valid
    switch (fix_type)
    {
        case 1u:  // fall-through
        case 4u:  return 'E'; // dead reckoning / DR + GNSS
        case 2u:  // fall-through
        case 3u:  return 'A'; // autonomous
        default:  return 'N';
    }
}

} // namespace detail

// ─── to_gga ───────────────────────────────────────────────────────────────────

inline gga_input to_gga(const gnss::location_data& loc = gnss::location_data{},
                         const gnss::dop_data& dop = gnss::dop_data{})
{
    gga_input out;
    if (!loc.valid)
        return out;

    out.hour        = loc.hour;
    out.minute      = loc.minute;
    out.second      = loc.second;
    out.millisecond = loc.millisecond;

    out.latitude_deg  = loc.latitude_deg;
    out.longitude_deg = loc.longitude_deg;

    out.fix_quality    = detail::fix_type_to_quality(loc.fix_type, loc.flags2);
    out.num_satellites = loc.num_sv;
    out.hdop           = dop.hdop; // 0.0 if DOP not provided

    // Altitude MSL: convert mm → m
    out.altitude_msl_m = loc.altitude_msl_mm / 1000.0;

    // Geoid separation approximation: height above ellipsoid − height above MSL.
    // Note: this is a physical approximation.  Exact NMEA geoid undulation
    // would require an EGM96 or similar model lookup.
    out.geoid_sep_m = (loc.altitude_ellipsoid_mm - loc.altitude_msl_mm) / 1000.0;

    // DGPS: not available from UBX-NAV-PVT alone → leave as not-available.
    out.dgps_age_s = -1.0;

    out.valid = true;
    return out;
}

// ─── to_rmc ───────────────────────────────────────────────────────────────────

inline rmc_input to_rmc(const gnss::location_data& loc = gnss::location_data{})
{
    rmc_input out;
    if (!loc.valid)
        return out;

    out.hour        = loc.hour;
    out.minute      = loc.minute;
    out.second      = loc.second;
    out.millisecond = loc.millisecond;

    // gnssFixOK: flags bit 0
    out.status_active = ((loc.flags & 0x01u) != 0u) && (loc.fix_type >= 2u);

    out.latitude_deg  = loc.latitude_deg;
    out.longitude_deg = loc.longitude_deg;

    // Speed: mm/s → knots (1 knot = 514.444 mm/s)
    out.speed_knots = static_cast<double>(loc.ground_speed_mmps) / 514.444;

    // Course over ground: use motion heading (heading of travel direction)
    out.course_true = static_cast<double>(loc.motion_heading_deg);

    out.day   = loc.day;
    out.month = loc.month;
    out.year  = loc.year;

    // Magnetic variation from magnetic declination field
    const double mag_dec = static_cast<double>(loc.magnetic_declination_deg);
    out.mag_variation = mag_dec;
    // Consider valid only when |declination| > 0 to avoid emitting 0,E
    out.mag_var_valid = (std::fabs(mag_dec) > 1e-4);

    out.mode  = detail::fix_type_to_rmc_mode(loc.fix_type, loc.flags);
    out.valid = true;
    return out;
}

// ─── to_gsa ───────────────────────────────────────────────────────────────────

inline gsa_input to_gsa(const gnss::location_data& loc = gnss::location_data{},
                         const gnss::satellites_data& sats = gnss::satellites_data{},
                         const gnss::dop_data& dop = gnss::dop_data{})
{
    gsa_input out;
    if (!loc.valid)
        return out;

    out.op_mode  = 'A'; // Auto — u-blox defaults to automatic 2D/3D switching
    out.nav_mode = (loc.fix_type == 2u) ? 2u : (loc.fix_type >= 3u ? 3u : 1u);

    // Populate sat_ids from satellites used in the nav solution (up to 12)
    int slot = 0;
    if (sats.valid)
    {
        for (uint8_t i = 0u; i < sats.num_svs && slot < 12; ++i)
        {
            if (sats.svs[i].used)
                out.sat_ids[slot++] = sats.svs[i].sv_id;
        }
    }

    out.pdop = dop.pdop;
    out.hdop = dop.hdop;
    out.vdop = dop.vdop;

    // system_id = 0 → omit NMEA 4.11 field; GNGSA covers all constellations
    out.system_id = 0u;

    out.valid = true;
    return out;
}

// ─── to_vtg ───────────────────────────────────────────────────────────────────

inline vtg_input to_vtg(const gnss::location_data& loc = gnss::location_data{})
{
    vtg_input out;
    if (!loc.valid)
        return out;

    out.course_true = static_cast<double>(loc.motion_heading_deg);

    // Magnetic course = true course + magnetic declination
    const double mag_dec = static_cast<double>(loc.magnetic_declination_deg);
    if (std::fabs(mag_dec) > 1e-4)
    {
        out.course_mag       = out.course_true + mag_dec;
        out.course_mag_valid = true;
    }

    // Speed: mm/s → knots and km/h
    const double speed_mps = static_cast<double>(loc.ground_speed_mmps) / 1000.0;
    out.speed_knots = speed_mps / 0.514444; // 1 knot = 0.514444 m/s
    out.speed_kmh   = speed_mps * 3.6;

    out.mode  = detail::fix_type_to_rmc_mode(loc.fix_type, loc.flags);
    out.valid = true;
    return out;
}

// ─── to_gll ───────────────────────────────────────────────────────────────────

inline gll_input to_gll(const gnss::location_data& loc = gnss::location_data{})
{
    gll_input out;
    if (!loc.valid)
        return out;

    out.latitude_deg  = loc.latitude_deg;
    out.longitude_deg = loc.longitude_deg;

    out.hour        = loc.hour;
    out.minute      = loc.minute;
    out.second      = loc.second;
    out.millisecond = loc.millisecond;

    out.status_active = ((loc.flags & 0x01u) != 0u) && (loc.fix_type >= 2u);
    out.mode          = detail::fix_type_to_rmc_mode(loc.fix_type, loc.flags);
    out.valid         = true;
    return out;
}

// ─── to_zda ───────────────────────────────────────────────────────────────────

inline zda_input to_zda(const gnss::location_data& loc = gnss::location_data{})
{
    zda_input out;
    if (!loc.valid)
        return out;

    out.hour        = loc.hour;
    out.minute      = loc.minute;
    out.second      = loc.second;
    out.millisecond = loc.millisecond;
    out.day         = loc.day;
    out.month       = loc.month;
    out.year        = loc.year;
    // UTC output → timezone offset = 0,0
    out.tz_hour = 0;
    out.tz_min  = 0u;
    out.valid   = true;
    return out;
}

// ─── to_zda (from timing_data) ────────────────────────────────────────────────

inline zda_input to_zda(const gnss::timing_data& tim = gnss::timing_data{})
{
    zda_input out;
    if (!tim.valid)
        return out;

    // Prefer UBX-NAV-TIMEUTC fields for high-accuracy time output
    out.hour        = tim.utc_hour;
    out.minute      = tim.utc_min;
    out.second      = tim.utc_sec;
    // Sub-second from utc_nano_ns: clamp to [0, 999] ms
    int32_t ms = static_cast<int32_t>(tim.utc_nano_ns / 1000000);
    out.millisecond = static_cast<uint32_t>((ms < 0) ? 0 : (ms > 999 ? 999 : ms));
    out.day         = tim.utc_day;
    out.month       = tim.utc_month;
    out.year        = tim.utc_year;
    out.tz_hour     = 0;
    out.tz_min      = 0u;
    out.valid       = true;
    return out;
}

// ─── to_gsv ───────────────────────────────────────────────────────────────────

/// Map UBX gnss_id to NMEA GSV talker string.
inline const char* gnss_id_to_talker(uint8_t gnss_id)
{
    switch (gnss_id)
    {
        case 0u: return "GP"; // GPS
        case 2u: return "GA"; // Galileo
        case 3u: return "GB"; // BeiDou
        case 5u: return "GQ"; // QZSS
        case 6u: return "GL"; // GLONASS
        default: return nullptr; // Skip (SBAS=1, NavIC=4, etc.)
    }
}

/// Build GSV input DTOs (one per constellation) from satellites_data.
inline std::vector<gsv_input> to_gsv_all(const gnss::satellites_data& sats = gnss::satellites_data{})
{
    // Collect satellites per constellation
    struct ConstellationEntry
    {
        const char* talker;
        uint8_t gnss_id;
        std::vector<gsv_satellite_record> records;
    };

    ConstellationEntry constellations[] = {
        { "GP", 0u, {} },
        { "GL", 6u, {} },
        { "GA", 2u, {} },
        { "GB", 3u, {} },
        { "GQ", 5u, {} },
    };
    const int num_constellations = 5;

    if (sats.valid)
    {
        for (uint8_t i = 0u; i < sats.num_svs; ++i)
        {
            const gnss::satellite_record& sv = sats.svs[i];
            for (int c = 0; c < num_constellations; ++c)
            {
                if (sv.gnss_id == constellations[c].gnss_id)
                {
                    gsv_satellite_record rec;
                    rec.sv_id         = sv.sv_id;
                    rec.elevation_deg = sv.elevation_deg;
                    rec.azimuth_deg   = sv.azimuth_deg;
                    rec.snr           = sv.cno;
                    constellations[c].records.push_back(rec);
                    break;
                }
            }
        }
    }

    std::vector<gsv_input> result;
    for (int c = 0; c < num_constellations; ++c)
    {
        if (constellations[c].records.empty())
            continue;
        gsv_input in;
        in.talker     = constellations[c].talker;
        in.satellites = constellations[c].records;
        in.valid      = true;
        result.push_back(in);
    }
    return result;
}

} // namespace shared_buffer_mapper
} // namespace generator
} // namespace nmea
