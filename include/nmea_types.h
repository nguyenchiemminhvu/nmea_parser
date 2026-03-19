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
// nmea_types.h
//
// Fundamental constants and type aliases shared across the nmea_parser library.
//
// All types live in the nmea::parser namespace to avoid collision with
// application code.
//
// Thread-safety: All types defined here are either plain data types or
// std::function aliases.  No internal synchronisation.

#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <functional>

namespace nmea
{
namespace parser
{

// ─── NMEA framing constants ────────────────────────────────────────────────────

/// NMEA 0183 standard maximum sentence length (including '$' and <CR><LF>).
static constexpr size_t NMEA_MAX_SENTENCE_LEN = 82u;

/// Internal sentence accumulation buffer size.  Wider than the standard to
/// accommodate u-blox PUBX proprietary sentences which may exceed 82 chars.
static constexpr size_t NMEA_BUFFER_LEN = 256u;

/// Sentence start delimiter.
static constexpr char NMEA_START_CHAR    = '$';

/// Checksum delimiter.
static constexpr char NMEA_CHECKSUM_CHAR = '*';

/// Field separator.
static constexpr char NMEA_FIELD_SEP     = ',';

/// Maximum number of fields in any supported sentence (after the type field).
static constexpr size_t NMEA_MAX_FIELDS = 32u;

/// Maximum number of satellites stored in the database layer.
static constexpr size_t NMEA_MAX_SATS_IN_VIEW = 64u;

/// Number of distinct per-satellite data attributes (sv_id, elevation, azimuth, snr).
static constexpr size_t NMEA_SAT_FIELDS = 4u;

// ─── Talker ID string constants ────────────────────────────────────────────────

static constexpr const char* TALKER_GP  = "GP";  ///< GPS
static constexpr const char* TALKER_GL  = "GL";  ///< GLONASS
static constexpr const char* TALKER_GA  = "GA";  ///< Galileo
static constexpr const char* TALKER_GB  = "GB";  ///< BeiDou
static constexpr const char* TALKER_GQ  = "GQ";  ///< QZSS
static constexpr const char* TALKER_GN  = "GN";  ///< Combined GNSS
static constexpr const char* TALKER_P   = "P";   ///< Proprietary (e.g. PUBX)

// ─── Sentence type string constants ───────────────────────────────────────────

static constexpr const char* SENTENCE_GGA = "GGA";  ///< Global Positioning System Fix Data
static constexpr const char* SENTENCE_RMC = "RMC";  ///< Recommended Minimum Specific GNSS Data
static constexpr const char* SENTENCE_GSA = "GSA";  ///< GNSS DOP and Active Satellites
static constexpr const char* SENTENCE_GSV = "GSV";  ///< GNSS Satellites in View
static constexpr const char* SENTENCE_GLL = "GLL";  ///< Geographic Position Latitude/Longitude
static constexpr const char* SENTENCE_VTG = "VTG";  ///< Course Over Ground and Ground Speed
static constexpr const char* SENTENCE_GBS = "GBS";  ///< GNSS Satellite Fault Detection
static constexpr const char* SENTENCE_GNS = "GNS";  ///< GNSS Fix Data
static constexpr const char* SENTENCE_GST = "GST";  ///< GNSS Pseudo Range Error Statistics
static constexpr const char* SENTENCE_ZDA = "ZDA";  ///< Time and Date
static constexpr const char* SENTENCE_DTM = "DTM";  ///< Datum Reference
static constexpr const char* SENTENCE_GRS = "GRS";  ///< GNSS Range Residuals
static constexpr const char* SENTENCE_TXT = "TXT";  ///< Text Transmission

} // namespace parser
} // namespace nmea
