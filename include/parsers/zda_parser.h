// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/zda_parser.h — ZDA sentence parser

#pragma once
#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_zda.h"
#include <utility>

namespace nmea { namespace parser {
class zda_parser : public i_sentence_parser {
public:
    explicit zda_parser(zda_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "ZDA"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    zda_callback cb_;
};
} }
