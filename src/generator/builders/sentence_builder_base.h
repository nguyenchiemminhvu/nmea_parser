// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/sentence_builder_base.h
//
// CRTP-free base class providing common builder plumbing:
//   • talker ID storage
//   • enabled flag
//   • sentence assembly helpers (field accumulation → final NMEA string)
//
// Concrete builders inherit from this class AND from i_nmea_sentence_builder.

#pragma once

#include "generator/i_nmea_sentence_builder.h"
#include "generator/i_nmea_formatter.h"
#include "generator/i_nmea_checksum.h"

#include <string>
#include <sstream>

namespace nmea
{
namespace generator
{

// ─── sentence_builder_base ───────────────────────────────────────────────────

class sentence_builder_base : public i_nmea_sentence_builder
{
public:
    explicit sentence_builder_base(const std::string& talker,
                                   const i_nmea_formatter& fmt,
                                   const i_nmea_checksum&  cs,
                                   bool enabled = true)
        : fmt_(fmt)
        , cs_(cs)
        , talker_(talker)
        , enabled_(enabled)
    {}

    bool is_enabled() const override         { return enabled_; }
    void set_enabled(bool enabled) override  { enabled_ = enabled; }
    void set_talker(const std::string& t) override { talker_ = t; }

protected:
    // ── Sentence assembly ─────────────────────────────────────────────────────

    /// Build the sentence body "$<talker><type>,<fields>" and append checksum.
    /// @p body_fields  Comma-separated fields string (without leading '$...,').
    std::string assemble(const char* type, const std::string& body_fields) const
    {
        // e.g. "GNGGA,123519.00,..."
        std::string content;
        content.reserve(80u);
        content += talker_;
        content += type;
        content += ',';
        content += body_fields;

        // Compute checksum over content (between '$' and '*', exclusive).
        uint8_t raw_cs = cs_.calculate(content);

        std::string sentence;
        sentence.reserve(content.size() + 6u);
        sentence += '$';
        sentence += content;
        sentence += '*';
        sentence += fmt_.checksum_to_hex(raw_cs);
        sentence += "\r\n";
        return sentence;
    }

    // Declare in initialization order (matches member-initializer-list order above).
    const i_nmea_formatter& fmt_;
    const i_nmea_checksum&  cs_;
    std::string             talker_;
    bool                    enabled_;
};

} // namespace generator
} // namespace nmea
