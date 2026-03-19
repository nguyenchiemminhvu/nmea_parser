// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// include/generator/i_nmea_checksum.h
//
// Abstraction for NMEA sentence checksum calculation.
// The NMEA 0183 standard mandates an XOR checksum over the sentence body
// (bytes strictly between '$' and '*', both exclusive).

#pragma once

#include <cstdint>
#include <string>

namespace nmea
{
namespace generator
{

// ─── i_nmea_checksum ─────────────────────────────────────────────────────────

class i_nmea_checksum
{
public:
    virtual ~i_nmea_checksum() = default;

    i_nmea_checksum()                                = default;
    i_nmea_checksum(const i_nmea_checksum&)          = delete;
    i_nmea_checksum& operator=(const i_nmea_checksum&) = delete;

    /// Compute the checksum of @p sentence_body.
    /// @p sentence_body must be the raw content between '$' and '*' (exclusive).
    /// Returns the raw 8-bit checksum value.
    virtual uint8_t calculate(const std::string& sentence_body) const = 0;

    /// Compute and return the checksum as a two-digit uppercase hex string
    /// suitable for appending after '*' in a finished sentence.
    virtual std::string calculate_hex(const std::string& sentence_body) const = 0;
};

} // namespace generator
} // namespace nmea
