// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "nmea_sentence_registry.h"
#include <utility>

namespace nmea
{
namespace parser
{

const std::vector<std::unique_ptr<i_sentence_parser>>
    nmea_sentence_registry::empty_vec_{};

void nmea_sentence_registry::register_parser(std::unique_ptr<i_sentence_parser> p)
{
    const std::string type = p->sentence_type();
    parsers_[type].push_back(std::move(p));
}

const std::vector<std::unique_ptr<i_sentence_parser>>&
nmea_sentence_registry::find(const std::string& sentence_type) const
{
    auto it = parsers_.find(sentence_type);
    if (it == parsers_.end())
        return empty_vec_;
    return it->second;
}

bool nmea_sentence_registry::has_parser(const std::string& sentence_type) const
{
    auto it = parsers_.find(sentence_type);
    return (it != parsers_.end()) && !it->second.empty();
}

} // namespace parser
} // namespace nmea
