// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// include/generator/i_nmea_sentence_builder.h
//
// Interface for a single-sentence NMEA builder.
//
// Responsibilities:
//   • Accept a typed input DTO via a typed prepare() method on the concrete class.
//   • Produce a complete NMEA sentence string including talker prefix, fields,
//     checksum, and CRLF terminator.
//
// Design notes:
//   • The base interface is intentionally non-templated so the facade can store
//     heterogeneous builders in a uniform container.
//   • The concrete builder exposes a typed prepare(const XxxInput&) method that
//     the facade calls before invoking build().
//   • build() is const — it does not modify internal state.  All mutable state
//     is established by prepare().

#pragma once

#include <string>

namespace nmea
{
namespace generator
{

// ─── i_nmea_sentence_builder ─────────────────────────────────────────────────

class i_nmea_sentence_builder
{
public:
    virtual ~i_nmea_sentence_builder() = default;

    i_nmea_sentence_builder()                                      = default;
    i_nmea_sentence_builder(const i_nmea_sentence_builder&)        = delete;
    i_nmea_sentence_builder& operator=(const i_nmea_sentence_builder&) = delete;

    /// Three-character sentence type string, e.g. "GGA", "RMC".
    virtual const char* sentence_type() const = 0;

    /// Whether this builder is enabled in the current configuration.
    /// build() will return an empty string when disabled.
    virtual bool is_enabled() const = 0;

    /// Enable or disable this builder.
    virtual void set_enabled(bool enabled) = 0;

    /// Set the talker ID string (e.g. "GN", "GP").
    virtual void set_talker(const std::string& talker) = 0;

    /// Produce the complete NMEA sentence.
    /// Returns an empty string when is_enabled() == false or the input DTO
    /// was never populated via prepare().
    virtual std::string build() const = 0;
};

} // namespace generator
} // namespace nmea
