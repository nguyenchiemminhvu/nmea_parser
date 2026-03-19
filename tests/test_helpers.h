// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_helpers.h — Shared test utilities

#pragma once

#include <cstdio>
#include <string>
#include <cstdint>
#include <vector>

// ─── pass / fail macros ───────────────────────────────────────────────────────

inline void test_pass(const std::string& name)
{
    std::printf("[PASS] %s\n", name.c_str());
}

inline void test_fail(const std::string& name, const std::string& reason)
{
    std::printf("[FAIL] %s: %s\n", name.c_str(), reason.c_str());
}

// ─── NMEA sentence helpers ────────────────────────────────────────────────────

/// Compute the XOR checksum of all chars between '$' and '*' (exclusive).
inline uint8_t nmea_checksum(const std::string& content)
{
    uint8_t cs = 0u;
    for (char c : content)
    {
        if (c == '$') continue;
        if (c == '*') break;
        cs ^= static_cast<uint8_t>(c);
    }
    return cs;
}

/// Build a valid NMEA sentence string with proper checksum and CRLF.
/// @param content  The sentence body WITHOUT '$', '*', checksum, or CRLF.
///                 Example: "GNGGA,092725.00,4717.11399,N,..."
inline std::string make_sentence(const std::string& content)
{
    const uint8_t cs = nmea_checksum(content);
    char hex[8];
    std::snprintf(hex, sizeof(hex), "*%02X\r\n", static_cast<unsigned>(cs));
    return "$" + content + hex;
}

/// Convert a string to a vector<uint8_t> for use with nmea_parser::feed().
inline std::vector<uint8_t> to_bytes(const std::string& s)
{
    return std::vector<uint8_t>(s.begin(), s.end());
}
