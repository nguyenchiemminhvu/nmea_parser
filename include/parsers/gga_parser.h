// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/gga_parser.h — GGA sentence parser

#pragma once

#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_gga.h"
#include <utility>

namespace nmea { namespace parser {

class gga_parser : public i_sentence_parser
{
public:
    explicit gga_parser(gga_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "GGA"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    gga_callback cb_;
};

} } // nmea::parser
