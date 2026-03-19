// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/gbs_parser.h — GBS sentence parser

#pragma once
#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_gbs.h"
#include <utility>

namespace nmea { namespace parser {
class gbs_parser : public i_sentence_parser {
public:
    explicit gbs_parser(gbs_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "GBS"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    gbs_callback cb_;
};
} }
