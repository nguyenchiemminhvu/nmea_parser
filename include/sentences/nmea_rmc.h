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
// sentences/nmea_rmc.h
//
// Decoded representation of the NMEA RMC sentence.
//
// Format (NMEA 0183 v4.11):
//   $--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a,a*hh
//
// Fields:
//   [0]  hhmmss.ss  UTC time
//   [1]  A/V        Status: A=Active (valid fix), V=Void (warning)
//   [2]  llll.ll    Latitude (DDmm.mmmm)
//   [3]  N/S        North/South indicator
//   [4]  yyyyy.yy   Longitude (DDDmm.mmmm)
//   [5]  E/W        East/West indicator
//   [6]  x.x        Speed over ground [knots]
//   [7]  x.x        Course over ground, true [degrees]
//   [8]  ddmmyy     UTC date
//   [9]  x.x        Magnetic variation [degrees]
//   [10] E/W        Magnetic variation direction
//   [11] A/D/E/N    Mode indicator (NMEA 2.3+)

#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace nmea
{
namespace parser
{

// ─── rmc_mode ─────────────────────────────────────────────────────────────────

enum class rmc_mode : char
{
    autonomous   = 'A',  ///< Autonomous GNSS fix
    differential = 'D',  ///< Differential GNSS fix
    estimated    = 'E',  ///< Estimated / dead reckoning
    manual       = 'M',  ///< Manual input
    not_valid    = 'N',  ///< Data not valid
    simulated    = 'S',  ///< Simulator
    unknown      = '\0',
};

// ─── nmea_rmc ─────────────────────────────────────────────────────────────────

struct nmea_rmc
{
    std::string talker;

    double   utc_time       = 0.0;   ///< hhmmss.ss as double
    bool     status_active  = false; ///< true = A (active/valid)
    double   latitude       = 0.0;   ///< Decimal degrees; negative = South
    double   longitude      = 0.0;   ///< Decimal degrees; negative = West
    double   speed_knots    = 0.0;   ///< Speed over ground [knots]
    double   course_true    = 0.0;   ///< Course over ground, true [°]
    uint32_t date           = 0u;    ///< ddmmyy encoded as integer
    double   mag_variation  = 0.0;   ///< Magnetic variation [°]; positive = East
    rmc_mode mode           = rmc_mode::unknown;

    bool valid = false;
};

using rmc_callback = std::function<void(const nmea_rmc&)>;

} // namespace parser
} // namespace nmea
