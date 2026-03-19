// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// parsers/gsv_parser.h — GSV sentence parser
//
// Fires the callback for each individual GSV part-sentence.
// The database handler (db_gsv_handler) accumulates parts into a complete set.

#pragma once

#include "parsers/i_sentence_parser.h"
#include "sentences/nmea_gsv.h"
#include <utility>

namespace nmea { namespace parser {

class gsv_parser : public i_sentence_parser
{
public:
    explicit gsv_parser(gsv_callback cb) : cb_(std::move(cb)) {}
    std::string sentence_type() const override { return "GSV"; }
    bool parse(const nmea_sentence& sentence) override;
private:
    gsv_callback cb_;
};

} } // nmea::parser
