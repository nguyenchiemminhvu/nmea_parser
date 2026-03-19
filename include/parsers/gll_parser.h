// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/gll_parser.h — GLL sentence parser

#pragma once
#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_gll.h"
#include <utility>

namespace nmea { namespace parser {
class gll_parser : public i_sentence_parser {
public:
    explicit gll_parser(gll_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "GLL"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    gll_callback cb_;
};
} }
