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
// nmea_errors.h
//
// Error model for the nmea_parser library.

#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace nmea
{
namespace parser
{

// ─── parse_error_code ─────────────────────────────────────────────────────────

enum class parse_error_code : uint8_t
{
    /// No error.  Never dispatched via error callback.
    none = 0u,

    /// XOR checksum mismatch.  The sentence content was received but the
    /// trailing *HH checksum bytes did not match the computed value.
    checksum_mismatch = 1u,

    /// Sentence length exceeded NMEA_BUFFER_LEN.  The partial sentence is
    /// discarded and the parser re-synchronises on the next '$'.
    sentence_too_long = 2u,

    /// A '$' was seen before the current sentence was terminated with '\n'.
    /// The unfinished sentence is discarded and parsing restarts from the
    /// new '$'.
    unexpected_start_in_sentence = 3u,

    /// A sentence passed checksum but the field count or field content were
    /// invalid for the declared sentence type.
    malformed_fields = 4u,

    /// A complete, well-formed sentence arrived for a type that has no
    /// registered parser.  Dispatched to the error callback only when the
    /// caller has enabled unknown-sentence errors.
    unknown_sentence_type = 5u,
};

// ─── parse_error_info ─────────────────────────────────────────────────────────

struct parse_error_info
{
    /// The error code describing what went wrong.
    parse_error_code code;

    /// The talker ID at the time of the error (empty if not yet parsed).
    std::string talker;

    /// The sentence type at the time of the error (empty if not yet parsed).
    std::string sentence_type;

    /// Human-readable description for logging.
    std::string description;
};

// ─── Callback type ─────────────────────────────────────────────────────────────

using parse_error_callback = std::function<void(const parse_error_info&)>;

// ─── error_message ─────────────────────────────────────────────────────────────

inline std::string error_message(parse_error_code code)
{
    switch (code)
    {
        case parse_error_code::none:                        return "none";
        case parse_error_code::checksum_mismatch:           return "checksum_mismatch";
        case parse_error_code::sentence_too_long:           return "sentence_too_long";
        case parse_error_code::unexpected_start_in_sentence:return "unexpected_start_in_sentence";
        case parse_error_code::malformed_fields:            return "malformed_fields";
        case parse_error_code::unknown_sentence_type:       return "unknown_sentence_type";
        default:                                             return "unknown";
    }
}

} // namespace parser
} // namespace nmea
