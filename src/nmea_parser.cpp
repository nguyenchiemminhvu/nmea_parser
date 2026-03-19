// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "nmea_parser.h"
#include "internal/parser_state.h"
#include "internal/checksum.h"
#include "internal/sentence_tokenizer.h"

#include <utility>
#include <string>

namespace nmea
{
namespace parser
{

nmea_parser::nmea_parser(nmea_sentence_registry registry)
    : registry_(std::move(registry))
    , dispatcher_(registry_)
    , ctx_(new internal::parse_context())
{
    ctx_->reset();
}

nmea_parser::~nmea_parser()
{
    delete ctx_;
}

// ── Callback registration ─────────────────────────────────────────────────────

void nmea_parser::set_raw_sentence_callback(raw_sentence_callback cb)
{
    dispatcher_.set_raw_sentence_callback(std::move(cb));
}

void nmea_parser::set_error_callback(parse_error_callback cb)
{
    dispatcher_.set_error_callback(std::move(cb));
}

void nmea_parser::set_report_unknown_sentences(bool enable)
{
    dispatcher_.set_report_unknown_sentences(enable);
}

// ── Statistics ────────────────────────────────────────────────────────────────

uint64_t nmea_parser::sentences_decoded()   const { return sentences_decoded_;   }
uint64_t nmea_parser::sentences_discarded() const { return sentences_discarded_; }

void nmea_parser::reset()
{
    ctx_->reset();
    sentences_decoded_   = 0u;
    sentences_discarded_ = 0u;
}

// ── Data ingestion ────────────────────────────────────────────────────────────

void nmea_parser::feed(const std::vector<uint8_t>& data)
{
    for (uint8_t b : data)
        process_byte(b);
}

void nmea_parser::feed(const char* data, size_t length)
{
    for (size_t i = 0u; i < length; ++i)
        process_byte(static_cast<uint8_t>(data[i]));
}

// ── Private helpers ───────────────────────────────────────────────────────────

void nmea_parser::discard_with_error(parse_error_code code,
                                      const std::string& talker,
                                      const std::string& type,
                                      const std::string& desc)
{
    ++sentences_discarded_;
    parse_error_info info;
    info.code          = code;
    info.talker        = talker;
    info.sentence_type = type;
    info.description   = desc;
    dispatcher_.report_error(info);
    ctx_->reset();
}

// ── State machine ─────────────────────────────────────────────────────────────

void nmea_parser::process_byte(uint8_t byte)
{
    using internal::parse_state;
    const char c = static_cast<char>(byte);

    switch (ctx_->state)
    {
    // ── WAIT_DOLLAR ──────────────────────────────────────────────────────────
    case parse_state::wait_dollar:
        if (c == '$')
        {
            ctx_->reset();
            ctx_->state = parse_state::accumulate_sentence;
        }
        // All other bytes are noise (UBX binary, NMEA partial, etc.) → discard
        break;

    // ── ACCUMULATE_SENTENCE ──────────────────────────────────────────────────
    case parse_state::accumulate_sentence:
        if (c == '$')
        {
            // A new '$' before '\n' → previous sentence was truncated
            discard_with_error(
                parse_error_code::unexpected_start_in_sentence, "", "",
                "new '$' received before previous sentence terminated");
            ctx_->buf[0] = '\0';
            ctx_->buf_len = 0u;
            ctx_->state = parse_state::accumulate_sentence;
        }
        else if (c == '*')
        {
            // Reached checksum delimiter
            ctx_->buf[ctx_->buf_len] = '\0';
            ctx_->checksum_present   = true;
            ctx_->state              = parse_state::wait_checksum_hi;
        }
        else if (c == '\r')
        {
            // CR: some devices omit the checksum; treat as end of sentence
            // (no '*HH' present).  We'll handle in wait_lf too.
            ctx_->buf[ctx_->buf_len] = '\0';
            ctx_->checksum_present   = false;
            ctx_->state              = parse_state::wait_lf;
        }
        else if (c == '\n')
        {
            // LF without CR and without '*HH': treat as sentence end (no checksum)
            ctx_->buf[ctx_->buf_len] = '\0';
            ctx_->checksum_present   = false;
            on_sentence_complete();
        }
        else
        {
            if (ctx_->buf_len >= NMEA_BUFFER_LEN)
            {
                discard_with_error(parse_error_code::sentence_too_long, "", "",
                                   "sentence exceeded buffer size");
                break;
            }
            ctx_->buf[ctx_->buf_len++] = c;
            ctx_->running_checksum ^= static_cast<uint8_t>(c);
        }
        break;

    // ── WAIT_CHECKSUM_HI ─────────────────────────────────────────────────────
    case parse_state::wait_checksum_hi:
        ctx_->cs_hi = c;
        ctx_->state = parse_state::wait_checksum_lo;
        break;

    // ── WAIT_CHECKSUM_LO ─────────────────────────────────────────────────────
    case parse_state::wait_checksum_lo:
    {
        uint8_t parsed = 0u;
        if (!internal::parse_hex_byte(ctx_->cs_hi, c, parsed))
        {
            discard_with_error(parse_error_code::checksum_mismatch, "", "",
                               "invalid non-hex checksum characters");
            break;
        }
        ctx_->received_checksum = parsed;
        ctx_->state = parse_state::wait_lf;
        break;
    }

    // ── WAIT_LF ──────────────────────────────────────────────────────────────
    case parse_state::wait_lf:
        if (c == '\r')
        {
            // Absorb optional CR before LF; stay in wait_lf
        }
        else if (c == '\n')
        {
            on_sentence_complete();
        }
        else if (c == '$')
        {
            // Stream error: no LF received, but new sentence starting
            discard_with_error(parse_error_code::unexpected_start_in_sentence, "", "",
                               "new '$' before LF");
            ctx_->reset();
            ctx_->state = parse_state::accumulate_sentence;
        }
        else
        {
            // Unexpected byte — still wait for LF
        }
        break;
    }
}

// ── on_sentence_complete ──────────────────────────────────────────────────────

void nmea_parser::on_sentence_complete()
{
    // Tokenise the accumulated buffer into an nmea_sentence
    nmea_sentence sentence = internal::tokenize(ctx_->buf, ctx_->buf_len);

    sentence.checksum_present = ctx_->checksum_present;
    sentence.checksum         = ctx_->received_checksum;

    // Verify checksum if present
    if (ctx_->checksum_present)
    {
        sentence.checksum_valid = (ctx_->running_checksum == ctx_->received_checksum);

        if (!sentence.checksum_valid)
        {
            discard_with_error(
                parse_error_code::checksum_mismatch,
                sentence.talker, sentence.type,
                "checksum mismatch in " + sentence.talker + sentence.type);
            return;
        }
    }
    else
    {
        // No checksum field — validity is assumed (some devices omit it)
        sentence.checksum_valid = false;
    }

    ++sentences_decoded_;
    dispatcher_.dispatch(sentence);
    ctx_->reset();
}

} // namespace parser
} // namespace nmea
