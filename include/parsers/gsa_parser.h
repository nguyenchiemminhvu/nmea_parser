// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/gsa_parser.h — GSA sentence parser

#pragma once

#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_gsa.h"
#include <utility>

namespace nmea { namespace parser {

class gsa_parser : public i_sentence_parser
{
public:
    explicit gsa_parser(gsa_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "GSA"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    gsa_callback cb_;
};

} } // nmea::parser
