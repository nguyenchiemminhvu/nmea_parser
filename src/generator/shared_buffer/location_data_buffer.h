#pragma once

#include <cstdint>

namespace gnss
{

/// Populated from UBX-NAV-PVT (high-priority epoch commit).
/// Units follow what the db handler stores (see db_nav_pvt_handler.cpp).
struct location_data
{
    // ── Position ──────────────────────────────────────────────────────────────
    double   latitude_deg;          ///< Latitude  [degrees] (1e-7 raw → deg, already converted)
    double   longitude_deg;         ///< Longitude [degrees]
    double   altitude_ellipsoid_mm; ///< Height above ellipsoid [mm] (raw from handler)
    double   altitude_msl_mm;       ///< Height above MSL [mm]

    // ── Accuracy estimates ─────────────────────────────────────────────────
    float    h_accuracy_mm;         ///< Horizontal accuracy [mm]
    float    v_accuracy_mm;         ///< Vertical accuracy [mm]
    float    speed_accuracy_mmps;   ///< Speed accuracy [mm/s]
    float    heading_accuracy_deg;  ///< Heading accuracy [degrees]

    // ── Velocity (mm/s, raw from handler) ─────────────────────────────────
    float    vel_n_mmps;            ///< NED north velocity [mm/s]
    float    vel_e_mmps;            ///< NED east  velocity [mm/s]
    float    vel_d_mmps;            ///< NED down  velocity [mm/s]
    float    ground_speed_mmps;     ///< 2-D ground speed [mm/s]

    // ── Heading [degrees, already scaled from 1e-5 raw] ───────────────────
    float    vehicle_heading_deg;   ///< Vehicle heading
    float    motion_heading_deg;    ///< Motion heading
    float    magnetic_declination_deg;

    // ── Fix / quality ──────────────────────────────────────────────────────
    uint8_t  fix_type;  ///< 0=no_fix 1=dead_reck 2=2D 3=3D 4=GNSS+DR 5=time_only
    uint8_t  num_sv;    ///< SVs used in solution
    uint8_t  flags;     ///< Fix-status flags byte
    uint8_t  flags2;    ///< Additional flags byte

    // ── UTC date / time ────────────────────────────────────────────────────
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
    uint8_t  time_valid;         ///< Validity flags for date/time fields

    // ── Sub-second precision ───────────────────────────────────────────────
    uint32_t millisecond;        ///< i_tow % 1000
    int32_t  microsecond;        ///< nano / 1000 (µs within the second)

    // ── Timestamps ────────────────────────────────────────────────────────
    uint32_t i_tow_ms;           ///< GPS time of week [ms]
    int64_t  utc_timestamp_ms;   ///< UTC Unix timestamp [ms]

    bool     valid;              ///< True when this record has been populated at least once
};

} // namespace gnss
