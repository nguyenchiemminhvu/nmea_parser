#pragma once

#include <cstdint>
#include <cstring>

namespace gnss
{

static constexpr uint8_t GNSS_MAX_SV_COUNT = 64u;

/// Per-satellite record populated from UBX-NAV-SAT (low-priority commit).
struct satellite_record
{
    uint8_t gnss_id;           ///< Constellation: 0=GPS 1=SBAS 2=Galileo 3=BeiDou 5=QZSS 6=GLONASS
    uint8_t sv_id;             ///< Satellite/PRN identifier
    uint8_t cno;               ///< Carrier-to-noise density [dBHz]
    int8_t  elevation_deg;     ///< Elevation angle [degrees]
    int16_t azimuth_deg;       ///< Azimuth angle [degrees, 0..359]

    bool    used;              ///< Used in navigation solution
    bool    has_almanac;       ///< Almanac data available
    bool    has_ephemeris;     ///< Ephemeris data available
    bool    healthy;           ///< SV health: true = signal OK
    bool    diff_correction;   ///< Differential correction applied
    bool    sbas_correction;   ///< SBAS correction used
    bool    rtcm_correction;   ///< RTCM correction used
};

/// Populated from UBX-NAV-SAT (low-priority epoch commit).
struct satellites_data
{
    uint8_t          num_svs;              ///< Number of valid entries in svs[]
    satellite_record svs[GNSS_MAX_SV_COUNT]; ///< Per-satellite records (0..num_svs-1)
    bool             valid;               ///< True when populated at least once
};

} // namespace gnss
