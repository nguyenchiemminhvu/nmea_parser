// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "nmea_dispatcher.h"
#include <utility>

namespace nmea
{
namespace parser
{

nmea_dispatcher::nmea_dispatcher(const nmea_sentence_registry& registry)
    : registry_(registry)
{}

void nmea_dispatcher::set_raw_sentence_callback(raw_sentence_callback cb)
{
    raw_cb_ = std::move(cb);
}

void nmea_dispatcher::set_error_callback(parse_error_callback cb)
{
    error_cb_ = std::move(cb);
}

void nmea_dispatcher::set_report_unknown_sentences(bool enable)
{
    report_unknown_ = enable;
}

void nmea_dispatcher::dispatch(const nmea_sentence& sentence)
{
    // 1. Fire generic raw callback first (regardless of parser presence)
    if (raw_cb_)
        raw_cb_(sentence);

    // 2. Find registered parsers for this sentence type
    const auto& parsers = registry_.find(sentence.type);

    if (parsers.empty())
    {
        if (report_unknown_ && error_cb_)
        {
            parse_error_info info;
            info.code          = parse_error_code::unknown_sentence_type;
            info.talker        = sentence.talker;
            info.sentence_type = sentence.type;
            info.description   = "no parser registered for sentence type: " + sentence.type;
            error_cb_(info);
        }
        return;
    }

    // 3. Call every registered parser; report malformed_fields on failure
    for (const auto& p : parsers)
    {
        if (!p->parse(sentence) && error_cb_)
        {
            parse_error_info info;
            info.code          = parse_error_code::malformed_fields;
            info.talker        = sentence.talker;
            info.sentence_type = sentence.type;
            info.description   = "malformed fields in " + sentence.talker + sentence.type;
            error_cb_(info);
        }
    }
}

void nmea_dispatcher::report_error(const parse_error_info& info)
{
    if (error_cb_)
        error_cb_(info);
}

} // namespace parser
} // namespace nmea
