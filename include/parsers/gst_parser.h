// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/gst_parser.h — GST sentence parser

#pragma once
#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_gst.h"
#include <utility>

namespace nmea { namespace parser {
class gst_parser : public i_sentence_parser {
public:
    explicit gst_parser(gst_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "GST"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    gst_callback cb_;
};
} }
