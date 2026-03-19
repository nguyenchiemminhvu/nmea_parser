// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/builders/rmc_builder.h

#pragma once

#include "sentence_builder_base.h"
#include "generator/nmea_sentence_inputs.h"

namespace nmea
{
namespace generator
{

class rmc_builder : public sentence_builder_base
{
public:
    explicit rmc_builder(const std::string& talker,
                         const i_nmea_formatter& fmt,
                         const i_nmea_checksum& cs,
                         bool enabled = true);

    void prepare(const rmc_input& input);

    const char* sentence_type() const override { return "RMC"; }
    std::string build() const override;

private:
    rmc_input input_;
    bool input_valid_ = false;
};

} // namespace generator
} // namespace nmea
