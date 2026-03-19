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
// sentences/nmea_gsv.h
//
// Decoded representation of the NMEA GSV sentence (one part).
//
// GSV sentences are emitted in multi-sentence sets.  Each sentence contains
// up to 4 satellite records.  The caller must accumulate a full set
// (num_msgs == msg_num) before treating the satellite table as complete.
//
// Format (NMEA 0183 v4.11):
//   $--GSV,x,x,xx,[sv,sv,sv,sv]*hh
//
//   Repeating block (up to 4 per sentence):
//     sv_id, elevation, azimuth, snr
//
//   Optional trailing field (NMEA 4.11):
//     signal_id

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

// ─── gsv_satellite ────────────────────────────────────────────────────────────

struct gsv_satellite
{
    uint8_t sv_id      = 0u;   ///< Satellite / PRN number
    int8_t  elevation  = -1;   ///< Elevation [°] 0-90; -1 = not available
    uint16_t azimuth   = 0u;   ///< Azimuth [°] 0-359
    int8_t  snr        = -1;   ///< Signal-to-noise ratio [dBHz]; -1 = not tracking
};

// ─── nmea_gsv (one part) ──────────────────────────────────────────────────────

struct nmea_gsv
{
    std::string talker;

    uint8_t  num_msgs        = 0u;  ///< Total number of GSV messages in this set
    uint8_t  msg_num         = 0u;  ///< This message number (1-based)
    uint8_t  sats_in_view    = 0u;  ///< Total satellites in view (across all msgs)

    /// Up to 4 satellite records in this one sentence.
    std::array<gsv_satellite, 4> satellites{};
    uint8_t  sat_count       = 0u;  ///< Valid entries in satellites[]

    /// Signal ID (NMEA 4.11+).  0 = unknown / not present.
    uint8_t  signal_id       = 0u;

    bool valid = false;
};

using gsv_callback = std::function<void(const nmea_gsv&)>;

} // namespace parser
} // namespace nmea
