// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/nmea_checksum_impl.h
//
// Concrete XOR checksum implementation — header-only.
// Reuses the existing internal::compute_checksum() from nmea_parser.

#pragma once

#include "generator/i_nmea_checksum.h"
#include "internal/checksum.h"

#include <cstdio>

namespace nmea
{
namespace generator
{

// ─── nmea_xor_checksum ───────────────────────────────────────────────────────

class nmea_xor_checksum : public i_nmea_checksum
{
public:
    uint8_t calculate(const std::string& body) const override
    {
        return nmea::parser::internal::compute_checksum(body.data(), body.size());
    }

    std::string calculate_hex(const std::string& body) const override
    {
        uint8_t cs = calculate(body);
        char buf[3];
        std::snprintf(buf, sizeof(buf), "%02X", static_cast<unsigned>(cs));
        return std::string(buf, 2u);
    }
};

} // namespace generator
} // namespace nmea
