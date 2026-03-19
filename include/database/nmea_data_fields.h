// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/nmea_data_fields.h
//
// Enumeration of every scalar field that the NMEA database handlers may write.
// The flat-store approach mirrors ubx_parser's ubx_data_fields.h.
//
// Design:
//   • DATA_PARSE acts as the "ceiling" sentinel — only ids < DATA_PARSE may
//     be written to the store.
//   • DATA_NUM is the total storage slot count.
//   • Per-satellite GSV data occupies NMEA_SAT_BASE + sat_idx * 4 + field_offset.

#pragma once

#include "nmea_types.h"
#include <cstdint>

namespace nmea
{
namespace database
{

// ─── msg_type ────────────────────────────────────────────────────────────────

enum msg_type : uint8_t
{
    MSG_NMEA_GGA = 0u,
    MSG_NMEA_RMC,
    MSG_NMEA_GSA,
    MSG_NMEA_GSV,
    MSG_NMEA_VTG,
    MSG_NMEA_GLL,
    MSG_NMEA_GBS,
    MSG_NMEA_GNS,
    MSG_NMEA_GST,
    MSG_NMEA_ZDA,
    MSG_NMEA_DTM,
    MSG_NMEA_COUNT,
};

// ─── data_field ──────────────────────────────────────────────────────────────

enum data_field : uint16_t
{
    // ── GGA ──────────────────────────────────────────────────────────────────
    NMEA_GGA_UTC_TIME = 0,
    NMEA_GGA_LATITUDE,
    NMEA_GGA_LONGITUDE,
    NMEA_GGA_FIX_QUALITY,
    NMEA_GGA_NUM_SATELLITES,
    NMEA_GGA_HDOP,
    NMEA_GGA_ALTITUDE_MSL,
    NMEA_GGA_GEOID_SEP,
    NMEA_GGA_DGPS_AGE,

    // ── RMC ──────────────────────────────────────────────────────────────────
    NMEA_RMC_UTC_TIME,
    NMEA_RMC_STATUS_ACTIVE,  ///< 1.0 = active, 0.0 = void
    NMEA_RMC_LATITUDE,
    NMEA_RMC_LONGITUDE,
    NMEA_RMC_SPEED_KNOTS,
    NMEA_RMC_COURSE_TRUE,
    NMEA_RMC_DATE,
    NMEA_RMC_MAG_VARIATION,

    // ── GSA ──────────────────────────────────────────────────────────────────
    NMEA_GSA_NAV_MODE,   ///< 1=no fix, 2=2D, 3=3D
    NMEA_GSA_PDOP,
    NMEA_GSA_HDOP,
    NMEA_GSA_VDOP,
    NMEA_GSA_SYSTEM_ID,

    // ── VTG ──────────────────────────────────────────────────────────────────
    NMEA_VTG_COURSE_TRUE,
    NMEA_VTG_COURSE_MAG,
    NMEA_VTG_SPEED_KNOTS,
    NMEA_VTG_SPEED_KMPH,

    // ── GLL ──────────────────────────────────────────────────────────────────
    NMEA_GLL_LATITUDE,
    NMEA_GLL_LONGITUDE,
    NMEA_GLL_UTC_TIME,
    NMEA_GLL_STATUS_ACTIVE,

    // ── GBS ──────────────────────────────────────────────────────────────────
    NMEA_GBS_ERR_LAT,
    NMEA_GBS_ERR_LON,
    NMEA_GBS_ERR_ALT,

    // ── GNS ──────────────────────────────────────────────────────────────────
    NMEA_GNS_UTC_TIME,
    NMEA_GNS_LATITUDE,
    NMEA_GNS_LONGITUDE,
    NMEA_GNS_NUM_SATS,
    NMEA_GNS_HDOP,
    NMEA_GNS_ALTITUDE,

    // ── GST ──────────────────────────────────────────────────────────────────
    NMEA_GST_LAT_ERR,
    NMEA_GST_LON_ERR,
    NMEA_GST_ALT_ERR,
    NMEA_GST_RMS_DEV,

    // ── ZDA ──────────────────────────────────────────────────────────────────
    NMEA_ZDA_UTC_TIME,
    NMEA_ZDA_DAY,
    NMEA_ZDA_MONTH,
    NMEA_ZDA_YEAR,

    // ── GSV per-satellite data ────────────────────────────────────────────────
    // Parallel-array layout (mirrors ubx_parser's UBX-NAV-SAT layout):
    //   NMEA_GSV_SAT_SVID      + sat_idx  → sv_id       for satellite sat_idx
    //   NMEA_GSV_SAT_ELEVATION + sat_idx  → elevation
    //   NMEA_GSV_SAT_AZIMUTH   + sat_idx  → azimuth
    //   NMEA_GSV_SAT_SNR       + sat_idx  → snr
    // Each _X value is the exclusive-end sentinel for range checks.
    NMEA_GSV_NUM_SATS_IN_VIEW,
    NMEA_GSV_SAT_SVID,
    NMEA_GSV_SAT_SVID_X      = parser::NMEA_MAX_SATS_IN_VIEW + NMEA_GSV_SAT_SVID,
    NMEA_GSV_SAT_ELEVATION,
    NMEA_GSV_SAT_ELEVATION_X = parser::NMEA_MAX_SATS_IN_VIEW + NMEA_GSV_SAT_ELEVATION,
    NMEA_GSV_SAT_AZIMUTH,
    NMEA_GSV_SAT_AZIMUTH_X   = parser::NMEA_MAX_SATS_IN_VIEW + NMEA_GSV_SAT_AZIMUTH,
    NMEA_GSV_SAT_SNR,
    NMEA_GSV_SAT_SNR_X       = parser::NMEA_MAX_SATS_IN_VIEW + NMEA_GSV_SAT_SNR,

    // ── Sentinels ─────────────────────────────────────────────────────────────────
    DATA_PARSE = NMEA_GSV_SAT_SNR_X,
    DATA_NUM   = DATA_PARSE,
};

// ── Helper: safe per-satellite field accessor ─────────────────────────────────
// Returns DATA_PARSE (sentinel) if sat_idx is out of [0, NMEA_MAX_SATS_IN_VIEW).
// Usage:  store.get(nmea_gsv_sat_field(NMEA_GSV_SAT_SVID, idx), val)

inline data_field nmea_gsv_sat_field(data_field base, uint8_t sat_idx)
{
    uint16_t id = static_cast<uint16_t>(base) + static_cast<uint16_t>(sat_idx);
    if (id < static_cast<uint16_t>(DATA_PARSE))
        return static_cast<data_field>(id);
    return DATA_PARSE;
}

} // namespace database
} // namespace nmea
