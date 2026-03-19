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
// sentences/nmea_sentence.h
//
// Raw tokenised NMEA sentence produced by the framing state machine and
// sentence tokeniser.  This is the currency passed to all i_sentence_parser
// instances.
//
// Fields are stored as std::string copies so parsers can be called
// asynchronously without lifetime concerns.

#pragma once

#include "nmea_types.h"

#include <string>
#include <vector>
#include <cstdint>

namespace nmea
{
namespace parser
{

// ─── nmea_sentence ────────────────────────────────────────────────────────────
//
// The normalised representation of a single NMEA sentence after framing and
// tokenisation.  A sentence parser receives this and produces a typed struct.

struct nmea_sentence
{
    /// Talker identifier ("GP", "GN", "GL", "GA", "GB", "P", …).
    std::string talker;

    /// Sentence type identifier ("GGA", "RMC", "GSA", …).
    std::string type;

    /// Zero-indexed field list.  fields[0] is the first comma-separated field
    /// after the combined talker+type token.
    /// Example: "$GNGGA,092725.00,..." → fields[0] = "092725.00"
    std::vector<std::string> fields;

    /// Received checksum value (decoded from *HH).
    uint8_t checksum = 0u;

    /// True if a '*HH' checksum was present in the sentence.
    bool checksum_present = false;

    /// True if checksum_present && computed checksum matches received checksum.
    bool checksum_valid = false;

    // ── Field accessors ─────────────────────────────────────────────────────

    /// Return field at @p idx, or empty string if out of range.
    const std::string& field(size_t idx) const
    {
        static const std::string empty{};
        return (idx < fields.size()) ? fields[idx] : empty;
    }

    /// True if field at @p idx exists and is non-empty.
    bool has_field(size_t idx) const
    {
        return (idx < fields.size()) && !fields[idx].empty();
    }

    /// Number of fields.
    size_t field_count() const { return fields.size(); }
};

// ─── Callback type ─────────────────────────────────────────────────────────────

/// Callback for any raw tokenised sentence (generic observer).
using raw_sentence_callback = std::function<void(const nmea_sentence&)>;

} // namespace parser
} // namespace nmea
