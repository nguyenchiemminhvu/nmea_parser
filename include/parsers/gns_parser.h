// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/gns_parser.h — GNS sentence parser

#pragma once
#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_gns.h"
#include <utility>

namespace nmea { namespace parser {
class gns_parser : public i_sentence_parser {
public:
    explicit gns_parser(gns_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "GNS"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    gns_callback cb_;
};
} }
