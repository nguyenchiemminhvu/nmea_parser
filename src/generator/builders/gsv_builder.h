// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/gsv_builder.h
//
// GSV builder generates one or more sentences covering a full constellation.
// Because GSV is a multi-sentence set, the builder exposes build_all() which
// returns a vector<string> instead of a single string.
// build() returns the first sentence only for interface compatibility.

#pragma once

#include "sentence_builder_base.h"
#include "generator/nmea_sentence_inputs.h"

#include <vector>

namespace nmea
{
namespace generator
{

class gsv_builder : public sentence_builder_base
{
public:
    explicit gsv_builder(const std::string& talker,
                         const i_nmea_formatter& fmt,
                         const i_nmea_checksum& cs,
                         bool enabled = true);

    void prepare(const gsv_input& input);

    const char* sentence_type() const override { return "GSV"; }

    /// Returns the first sentence of the GSV set (or empty string).
    /// Use build_all() to obtain the complete set.
    std::string build() const override;

    /// Returns all sentences for this constellation's GSV set.
    std::vector<std::string> build_all() const;

private:
    std::string build_one(const gsv_input& d,
                          uint8_t msg_num,
                          uint8_t total_msgs,
                          uint8_t sat_offset) const;

    gsv_input input_;
    bool input_valid_ = false;
};

} // namespace generator
} // namespace nmea
