// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_dispatcher.h
//
// Routes tokenised nmea_sentence objects to their registered parsers and
// to the generic/raw callbacks.
//
// Thread-safety: NOT thread-safe.  Must be used from the same thread that
// calls nmea_parser::feed().

#pragma once

#include "nmea_types.h"
#include "nmea_errors.h"
#include "nmea_sentence_registry.h"
#include "sentences/nmea_sentence.h"

namespace nmea
{
namespace parser
{

// ─── nmea_dispatcher ──────────────────────────────────────────────────────────

class nmea_dispatcher
{
public:
    /// Construct with a reference to an external registry.
    /// The registry must outlive the dispatcher.
    explicit nmea_dispatcher(const nmea_sentence_registry& registry);
    ~nmea_dispatcher() = default;

    // Non-copyable
    nmea_dispatcher(const nmea_dispatcher&)            = delete;
    nmea_dispatcher& operator=(const nmea_dispatcher&) = delete;

    // ── Callback registration ─────────────────────────────────────────────────

    /// Register a callback fired for every successfully framed sentence
    /// (regardless of type and independent of registered parsers).
    void set_raw_sentence_callback(raw_sentence_callback cb);

    /// Register a callback fired for every parse error.
    void set_error_callback(parse_error_callback cb);

    /// When true, an unknown_sentence_type error is reported for sentences
    /// with no registered parser.  Default: false (unknown sentences silently
    /// dispatched via raw_sentence_callback only).
    void set_report_unknown_sentences(bool enable);

    // ── Dispatch ──────────────────────────────────────────────────────────────

    /// Dispatch a fully framed and checksum-verified sentence:
    ///   1. Fire raw_sentence_callback.
    ///   2. Look up parsers by sentence type.
    ///   3. Call each registered parser; report malformed_fields on failure.
    ///   4. If no parsers found and report_unknown_ is set, fire error callback.
    void dispatch(const nmea_sentence& sentence);

    /// Report a parse error (typically called by nmea_parser for framing errors).
    void report_error(const parse_error_info& info);

private:
    const nmea_sentence_registry& registry_;
    raw_sentence_callback         raw_cb_;
    parse_error_callback          error_cb_;
    bool                          report_unknown_ = false;
};

} // namespace parser
} // namespace nmea
