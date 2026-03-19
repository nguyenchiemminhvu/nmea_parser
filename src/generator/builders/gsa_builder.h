// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/gsa_builder.h

#pragma once

#include "sentence_builder_base.h"
#include "generator/nmea_sentence_inputs.h"

namespace nmea
{
namespace generator
{

class gsa_builder : public sentence_builder_base
{
public:
    explicit gsa_builder(const std::string& talker,
                         const i_nmea_formatter& fmt,
                         const i_nmea_checksum& cs,
                         bool enabled = true);

    void prepare(const gsa_input& input);

    const char* sentence_type() const override { return "GSA"; }
    std::string build() const override;

private:
    gsa_input input_;
    bool input_valid_ = false;
};

} // namespace generator
} // namespace nmea
