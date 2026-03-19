// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/i_sentence_parser.h
//
// Pure-virtual interface for all per-sentence-type parsers.
//
// Design (Open/Closed Principle):
//   To add support for a new NMEA sentence type:
//     1. Derive from i_sentence_parser.
//     2. Implement sentence_type() and parse().
//     3. Register an instance with nmea_sentence_registry.
//   The core parser and dispatcher need no modification.
//
// Multiple parsers per sentence type:
//   Unlike ubx_parser (one decoder per message type), the nmea_sentence_registry
//   allows MULTIPLE parsers to be registered for the same sentence type.
//   This lets user code and the database adapter both subscribe independently.

#pragma once

#include "sentences/nmea_sentence.h"
#include <string>

namespace nmea
{
namespace parser
{

// ─── i_sentence_parser ────────────────────────────────────────────────────────

class i_sentence_parser
{
public:
    virtual ~i_sentence_parser() = default;

    i_sentence_parser()                                      = default;
    i_sentence_parser(const i_sentence_parser&)              = delete;
    i_sentence_parser& operator=(const i_sentence_parser&)  = delete;

    // ── Identity ──────────────────────────────────────────────────────────────

    /// The three-letter (or longer) sentence type string this parser handles,
    /// e.g. "GGA", "RMC", "GSA".  Returns the SAME value unconditionally.
    virtual std::string sentence_type() const = 0;

    // ── Core parse operation ──────────────────────────────────────────────────

    /// Parse the tokenised fields of a sentence and invoke the registered
    /// callback (if set) with the decoded struct.
    ///
    /// @param sentence  A fully validated, tokenised nmea_sentence.  The
    ///                  caller has already verified the checksum (if present).
    ///
    /// @returns true  if the sentence was decoded successfully.
    /// @returns false if the field count or field content was invalid.
    ///                The caller may log a malformed_fields error.
    virtual bool parse(const nmea_sentence& sentence) = 0;
};

} // namespace parser
} // namespace nmea
