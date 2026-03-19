// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/rmc_parser.h — RMC sentence parser

#pragma once

#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_rmc.h"
#include <utility>

namespace nmea { namespace parser {

class rmc_parser : public i_sentence_parser
{
public:
    explicit rmc_parser(rmc_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "RMC"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    rmc_callback cb_;
};

} } // nmea::parser
