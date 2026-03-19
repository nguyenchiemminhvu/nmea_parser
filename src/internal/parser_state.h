// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// internal/parser_state.h
//
// State machine state enumeration and parse context for the nmea_parser.
//
// NMEA framing:
//   '$' [talker+type] ',' [fields] '*' [CK_HI] [CK_LO] '\r' '\n'
//
// States (in order):
//   wait_dollar          - scanning for '$'
//   accumulate_sentence  - accumulating chars + computing running checksum
//   wait_checksum_hi     - received '*', waiting for first hex digit
//   wait_checksum_lo     - waiting for second hex digit
//   wait_lf              - waiting for '\n' (may optionally absorb '\r')

#pragma once

#include "nmea_types.h"
#include <cstdint>
#include <cstring>

namespace nmea
{
namespace parser
{
namespace internal
{

// ─── parse_state ──────────────────────────────────────────────────────────────

enum class parse_state : uint8_t
{
    wait_dollar,          ///< Searching for '$'
    accumulate_sentence,  ///< Collecting chars: talker+type+fields (after '$')
    wait_checksum_hi,     ///< Received '*', waiting for first hex nibble
    wait_checksum_lo,     ///< Waiting for second hex nibble
    wait_lf,              ///< Waiting for '\n' (or '\r\n')
};

// ─── parse_context ────────────────────────────────────────────────────────────

struct parse_context
{
    parse_state state = parse_state::wait_dollar;

    /// Raw content buffer: everything after '$' and before '*HH\r\n'.
    char    buf[NMEA_BUFFER_LEN + 1u];
    size_t  buf_len = 0u;

    /// Running XOR checksum accumulated during accumulate_sentence.
    uint8_t running_checksum = 0u;

    /// Received high nibble of the '*HH' checksum.
    char    cs_hi = '\0';

    /// Received checksum decoded from *HH (set after wait_checksum_lo).
    uint8_t received_checksum = 0u;

    /// True if '*HH' was seen (false for sentences without checksum).
    bool    checksum_present = false;

    void reset()
    {
        state              = parse_state::wait_dollar;
        buf_len            = 0u;
        buf[0]             = '\0';
        running_checksum   = 0u;
        cs_hi              = '\0';
        received_checksum  = 0u;
        checksum_present   = false;
    }
};

} // namespace internal
} // namespace parser
} // namespace nmea
