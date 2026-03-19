// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// internal/checksum.h
//
// NMEA XOR checksum computation.
//
// The NMEA 0183 checksum is computed as an XOR of all bytes strictly between
// the '$' delimiter and the '*' delimiter (both exclusive).
//
// Thread-safety: stateless — safe to call from any thread.

#pragma once

#include <cstdint>
#include <cstddef>
#include <cctype>

namespace nmea
{
namespace parser
{
namespace internal
{

// ── compute_checksum ──────────────────────────────────────────────────────────
//
// Compute the XOR checksum of @p len bytes starting at @p data.
// The caller is responsible for passing only the bytes between '$' and '*'.

inline uint8_t compute_checksum(const char* data, size_t len)
{
    uint8_t cs = 0u;
    for (size_t i = 0u; i < len; ++i)
        cs ^= static_cast<uint8_t>(data[i]);
    return cs;
}

// ── parse_hex_byte ─────────────────────────────────────────────────────────────
//
// Convert two ASCII hex characters (e.g. '4', 'E') to a uint8_t.
// Returns false if either character is not a valid hex digit.

inline bool parse_hex_byte(char hi, char lo, uint8_t& out)
{
    auto hex_val = [](char c, uint8_t& v) -> bool {
        if (c >= '0' && c <= '9') { v = static_cast<uint8_t>(c - '0');        return true; }
        if (c >= 'A' && c <= 'F') { v = static_cast<uint8_t>(c - 'A' + 10u); return true; }
        if (c >= 'a' && c <= 'f') { v = static_cast<uint8_t>(c - 'a' + 10u); return true; }
        return false;
    };
    uint8_t h = 0u, l = 0u;
    if (!hex_val(hi, h) || !hex_val(lo, l))
        return false;
    out = static_cast<uint8_t>((h << 4u) | l);
    return true;
}

} // namespace internal
} // namespace parser
} // namespace nmea
