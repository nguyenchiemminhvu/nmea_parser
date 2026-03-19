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
// sentences/nmea_gsa.h
//
// Decoded representation of the NMEA GSA sentence.
//
// Format (NMEA 0183 v4.11):
//   $--GSA,a,x,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x,x*hh
//
// Fields:
//   [0]       a       Op mode: A=auto, M=manual
//   [1]       x       Nav mode: 1=no fix, 2=2D fix, 3=3D fix
//   [2..13]   xx      Satellite IDs used (up to 12; empty = unused slot)
//   [14]      x.x     PDOP
//   [15]      x.x     HDOP
//   [16]      x.x     VDOP
//   [17]      x       System ID (NMEA 4.11; 1=GPS,2=GLONASS,3=Galileo,4=BeiDou)

#pragma once

#include "nmea_types.h"
#include <cstdint>
#include <string>
#include <array>
#include <functional>

namespace nmea
{
namespace parser
{

// ─── gsa_op_mode / gsa_nav_mode ─────────────────────────────────────────────

enum class gsa_op_mode : char { auto_mode = 'A', manual = 'M', unknown = '\0' };

enum class gsa_nav_mode : uint8_t
{
    no_fix = 1u,
    fix_2d = 2u,
    fix_3d = 3u,
    unknown = 0u,
};

// ─── nmea_gsa ─────────────────────────────────────────────────────────────────

struct nmea_gsa
{
    std::string talker;

    gsa_op_mode  op_mode  = gsa_op_mode::unknown;
    gsa_nav_mode nav_mode = gsa_nav_mode::unknown;

    /// Satellite IDs used in the navigation solution (0 = empty slot).
    std::array<uint8_t, 12> sat_ids{};

    double pdop = 0.0;
    double hdop = 0.0;
    double vdop = 0.0;

    /// System ID (NMEA 4.11+).  0 = not present / unknown.
    /// 1=GPS, 2=GLONASS, 3=Galileo, 4=BeiDou, 5=QZSS
    uint8_t system_id = 0u;

    bool valid = false;
};

using gsa_callback = std::function<void(const nmea_gsa&)>;

} // namespace parser
} // namespace nmea
