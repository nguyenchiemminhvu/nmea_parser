// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/vtg_parser.h — VTG sentence parser

#pragma once
#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_vtg.h"
#include <utility>

namespace nmea { namespace parser {
class vtg_parser : public i_sentence_parser {
public:
    explicit vtg_parser(vtg_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "VTG"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    vtg_callback cb_;
};
} }
