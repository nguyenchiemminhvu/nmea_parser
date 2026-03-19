// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// internal/sentence_tokenizer.h
//
// Parses a raw accumulated sentence string (everything after '$' up to, but
// not including, '*HH\r\n') into a nmea_sentence struct.
//
// The raw buffer contains:
//   [talker+type],[field0],[field1],...,[fieldN]
//   e.g. "GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,"
//
// Thread-safety: stateless — safe to call from any thread.

#pragma once

#include "sentences/nmea_sentence.h"
#include "nmea_types.h"

#include <cstring>
#include <cctype>

namespace nmea
{
namespace parser
{
namespace internal
{

// ─── tokenize ─────────────────────────────────────────────────────────────────
//
// Build an nmea_sentence from the raw content between '$' and '*' (exclusive).
// @param buf   Null-terminated content string (no '$', no '*' or beyond).
// @param len   Length of buf (not including the null terminator).

inline nmea_sentence tokenize(const char* buf, size_t len)
{
    nmea_sentence result;

    if (len == 0u)
        return result;

    // ── Find talker + type (first token before first comma) ─────────────────
    const char* comma = reinterpret_cast<const char*>(
        std::memchr(buf, ',', len));

    std::string id;
    std::string remaining;

    if (comma == nullptr)
    {
        // sentence with no fields (unusual but possible for TXT etc.)
        id = std::string(buf, len);
    }
    else
    {
        id        = std::string(buf, static_cast<size_t>(comma - buf));
        remaining = std::string(comma + 1, len - static_cast<size_t>(comma - buf) - 1u);
    }

    // ── Split talker and sentence type ───────────────────────────────────────
    if (!id.empty() && id[0] == 'P')
    {
        // Proprietary: talker = "P", type = rest (e.g. "UBX" for PUBX)
        result.talker = "P";
        result.type   = id.substr(1u);
    }
    else if (id.size() >= 5u)
    {
        // Standard: first 2 chars = talker, remaining = type
        result.talker = id.substr(0u, 2u);
        result.type   = id.substr(2u);
    }
    else if (id.size() >= 3u)
    {
        // Short form (talker-less): treat whole ID as type
        result.talker.clear();
        result.type = id;
    }
    else
    {
        // Malformed
        result.type = id;
    }

    // ── Tokenise fields by comma ─────────────────────────────────────────────
    size_t start = 0u;
    const size_t rlen = remaining.size();

    while (start <= rlen)
    {
        size_t end = remaining.find(',', start);
        if (end == std::string::npos)
            end = rlen;

        result.fields.push_back(remaining.substr(start, end - start));
        start = end + 1u;
    }

    return result;
}

} // namespace internal
} // namespace parser
} // namespace nmea
