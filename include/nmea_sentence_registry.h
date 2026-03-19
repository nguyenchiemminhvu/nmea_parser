// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// nmea_sentence_registry.h
//
// Registry mapping sentence type strings to one or more i_sentence_parser
// instances.
//
// Design improvement over ubx_parser:
//   - Multiple parsers can be registered for the same sentence type.
//   - This allows user code and the database adapter to both subscribe to
//     (e.g.) GGA sentences without interfering with each other.
//
// Ownership: the registry takes ownership of all registered parsers via
// std::unique_ptr.
//
// Thread-safety: NOT thread-safe.  All registrations must occur before the
// first call to nmea_parser::feed().

#pragma once

#include "parsers/i_sentence_parser.h"

#include <map>
#include <vector>
#include <memory>
#include <string>

namespace nmea
{
namespace parser
{

// ─── nmea_sentence_registry ───────────────────────────────────────────────────

class nmea_sentence_registry
{
public:
    nmea_sentence_registry()  = default;
    ~nmea_sentence_registry() = default;

    // Non-copyable (owns unique_ptrs)
    nmea_sentence_registry(const nmea_sentence_registry&)            = delete;
    nmea_sentence_registry& operator=(const nmea_sentence_registry&) = delete;

    // Movable
    nmea_sentence_registry(nmea_sentence_registry&&)            = default;
    nmea_sentence_registry& operator=(nmea_sentence_registry&&) = default;

    // ── Registration ──────────────────────────────────────────────────────────

    /// Register a parser.  Multiple parsers may be registered for the same
    /// sentence type; all will be called in registration order.
    ///
    /// @param parser  Heap-allocated parser.  Ownership is transferred.
    void register_parser(std::unique_ptr<i_sentence_parser> parser);

    // ── Look-up ───────────────────────────────────────────────────────────────

    /// Return the list of parsers registered for @p sentence_type.
    /// Returns an empty vector reference if no parsers are registered.
    const std::vector<std::unique_ptr<i_sentence_parser>>&
    find(const std::string& sentence_type) const;

    /// True if at least one parser is registered for @p sentence_type.
    bool has_parser(const std::string& sentence_type) const;

private:
    std::map<std::string, std::vector<std::unique_ptr<i_sentence_parser>>> parsers_;

    /// Returned for unknown sentence types (empty).
    static const std::vector<std::unique_ptr<i_sentence_parser>> empty_vec_;
};

} // namespace parser
} // namespace nmea
