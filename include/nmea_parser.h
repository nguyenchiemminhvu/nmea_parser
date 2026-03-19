// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_parser.h
//
// Top-level NMEA stream parser — the primary public API of this library.
//
// Responsibilities:
//   1. Accept incremental byte (or char) chunks via feed().
//   2. Run a byte-level state machine to frame NMEA sentences.
//   3. Validate each sentence's XOR checksum.
//   4. Dispatch validated sentences to nmea_dispatcher.
//   5. Report framing and checksum errors via the error callback.
//
// Real-device behaviour:
//   The u-blox GNSS chip may interleave NMEA sentences with UBX binary data
//   in the same serial stream.  nmea_parser skips over non-NMEA bytes and
//   re-synchronises automatically on the next '$'.
//
// Thread-safety:
//   nmea_parser is NOT thread-safe.  All feed() calls must originate from the
//   same thread.
//
// Typical (parser-only) usage:
//
//   nmea::parser::nmea_sentence_registry reg;
//   reg.register_parser(std::unique_ptr<nmea::parser::gga_parser>(
//       new nmea::parser::gga_parser([](const nmea::parser::nmea_gga& g) {
//           // use g.latitude, g.longitude, g.fix_quality, ...
//       })));
//
//   nmea::parser::nmea_parser p(std::move(reg));
//   p.set_error_callback([](const nmea::parser::parse_error_info& e) { ... });
//
//   std::vector<uint8_t> chunk = /* read from UART */;
//   p.feed(chunk);

#pragma once

#include "nmea_types.h"
#include "nmea_errors.h"
#include "nmea_sentence_registry.h"
#include "nmea_dispatcher.h"

#include <cstdint>
#include <vector>

namespace nmea
{
namespace parser
{

// Forward-declare internal context.
namespace internal { struct parse_context; }

// ─── nmea_parser ──────────────────────────────────────────────────────────────

class nmea_parser
{
public:
    /// Construct with a populated sentence registry.
    explicit nmea_parser(nmea_sentence_registry registry);
    ~nmea_parser();

    // Non-copyable
    nmea_parser(const nmea_parser&)            = delete;
    nmea_parser& operator=(const nmea_parser&) = delete;

    // ── Callback registration ─────────────────────────────────────────────────

    /// Set the callback fired for every successfully framed sentence,
    /// independent of registered parsers.
    void set_raw_sentence_callback(raw_sentence_callback cb);

    /// Set the parse-error callback.
    void set_error_callback(parse_error_callback cb);

    /// When true, report unknown_sentence_type errors for sentences that have
    /// no registered parser.  Default: false.
    void set_report_unknown_sentences(bool enable);

    // ── Data ingestion ────────────────────────────────────────────────────────

    /// Feed a chunk of bytes from the serial/UART layer.
    ///
    /// Handles:
    ///   - multiple complete sentences per call,
    ///   - sentences split across multiple calls,
    ///   - non-NMEA noise (UBX binary, raw bytes) interleaved in the stream.
    ///
    /// Callbacks fire synchronously within this call.
    void feed(const std::vector<uint8_t>& data);

    /// Convenience overload for char buffers (NMEA is ASCII).
    void feed(const char* data, size_t length);

    // ── Statistics / diagnostics ─────────────────────────────────────────────

    /// Number of sentences successfully framed and dispatched.
    uint64_t sentences_decoded()   const;

    /// Number of sentences discarded due to framing or checksum errors.
    uint64_t sentences_discarded() const;

    /// Reset statistics and parser state.
    void reset();

private:
    void process_byte(uint8_t byte);
    void on_sentence_complete();
    void discard_with_error(parse_error_code code,
                            const std::string& talker,
                            const std::string& type,
                            const std::string& desc);

    nmea_sentence_registry      registry_;
    nmea_dispatcher             dispatcher_;
    internal::parse_context*    ctx_ = nullptr;
    uint64_t                    sentences_decoded_   = 0u;
    uint64_t                    sentences_discarded_ = 0u;
};

} // namespace parser
} // namespace nmea
