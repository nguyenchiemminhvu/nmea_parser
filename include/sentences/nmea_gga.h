// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// sentences/nmea_gga.h
//
// Decoded representation of the NMEA GGA sentence.
//
// Format (NMEA 0183 v4.11):
//   $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
//
// Fields:
//   [0]  hhmmss.ss  UTC time
//   [1]  llll.ll    Latitude (DDmm.mmmm)
//   [2]  a          N/S indicator
//   [3]  yyyyy.yy   Longitude (DDDmm.mmmm)
//   [4]  a          E/W indicator
//   [5]  x          GPS quality indicator (0-8)
//   [6]  xx         Number of satellites used
//   [7]  x.x        HDOP
//   [8]  x.x        Altitude (MSL), metres
//   [9]  M          Altitude units (always M)
//   [10] x.x        Geoid separation, metres
//   [11] M          Geoid separation units
//   [12] x.x        Age of differential correction (empty if not used)
//   [13] xxxx       Differential reference station ID

#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace nmea
{
namespace parser
{

// ─── gga_fix_quality ──────────────────────────────────────────────────────────

enum class gga_fix_quality : uint8_t
{
    invalid       = 0u,  ///< Fix not valid
    gps_sps       = 1u,  ///< GPS fix (SPS mode)
    dgps          = 2u,  ///< Differential GPS fix
    pps           = 3u,  ///< PPS fix
    rtk_fixed     = 4u,  ///< RTK fixed integer solution
    rtk_float     = 5u,  ///< RTK float solution
    estimated     = 6u,  ///< Estimated (dead reckoning) mode
    manual        = 7u,  ///< Manual input mode
    simulation    = 8u,  ///< Simulation mode
};

// ─── nmea_gga ─────────────────────────────────────────────────────────────────

struct nmea_gga
{
    std::string talker;          ///< Talker ID ("GP", "GN", …)

    double      utc_time     = 0.0;   ///< hhmmss.ss encoded as double
    double      latitude     = 0.0;   ///< Decimal degrees; negative = South
    double      longitude    = 0.0;   ///< Decimal degrees; negative = West
    gga_fix_quality fix_quality = gga_fix_quality::invalid;
    uint8_t     num_satellites = 0u;  ///< Used satellite count
    double      hdop         = 0.0;
    double      altitude_msl = 0.0;   ///< Altitude above mean sea level [m]
    double      geoid_sep    = 0.0;   ///< Geoid separation (undulation) [m]
    double      dgps_age     = -1.0;  ///< Age of DGPS correction [s]; <0 = n/a
    uint16_t    dgps_station_id = 0u; ///< DGPS reference station ID

    /// True if all mandatory fields could be decoded.
    bool valid = false;
};

using gga_callback = std::function<void(const nmea_gga&)>;

} // namespace parser
} // namespace nmea
