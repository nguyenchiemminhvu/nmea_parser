// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/dtm_parser.h — DTM sentence parser

#pragma once
#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_dtm.h"
#include <utility>

namespace nmea { namespace parser {
class dtm_parser : public i_sentence_parser {
public:
    explicit dtm_parser(dtm_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "DTM"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    dtm_callback cb_;
};
} }
