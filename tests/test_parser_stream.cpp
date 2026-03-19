// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parser_stream.cpp
//
// Tests for the byte-level framing state machine:
//   - single sentence framing
//   - multiple sentences in one chunk
//   - sentence split across multiple feed() calls (1 byte at a time)
//   - checksum validation pass/fail
//   - malformed sentence recovery
//   - noise bytes ignored
//   - sentence too long
//   - CR/LF variants

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"

using namespace nmea::parser;

// ─── test: single complete sentence dispatched ────────────────────────────────

static void test_single_sentence()
{
    const char* name = "stream_single_sentence";

    int dispatch_count = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_raw_sentence_callback([&](const nmea_sentence& s) {
        if (s.type == "GGA") ++dispatch_count;
    });

    const std::string sent = make_sentence(
        "GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,");
    p.feed(to_bytes(sent));

    if (dispatch_count != 1) { test_fail(name, "expected 1 dispatch"); return; }
    if (p.sentences_decoded() != 1u) { test_fail(name, "sentences_decoded != 1"); return; }
    test_pass(name);
}

// ─── test: multiple sentences in one feed() call ─────────────────────────────

static void test_multiple_sentences_one_feed()
{
    const char* name = "stream_multiple_one_feed";

    int count = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_raw_sentence_callback([&](const nmea_sentence&) { ++count; });

    const std::string s1 = make_sentence("GPGGA,123456.00,4000.00,N,07500.00,W,1,06,1.5,10.0,M,0.0,M,,");
    const std::string s2 = make_sentence("GPRMC,123456.00,A,4000.00,N,07500.00,W,0.0,0.0,010124,,,A");
    const std::string s3 = make_sentence("GPVTG,0.0,T,,M,0.0,N,0.0,K,A");

    p.feed(to_bytes(s1 + s2 + s3));

    if (count != 3) { test_fail(name, "expected 3 dispatches, got " + std::to_string(count)); return; }
    test_pass(name);
}

// ─── test: sentence split across multiple feeds (1 byte at a time) ───────────

static void test_byte_by_byte()
{
    const char* name = "stream_byte_by_byte";

    int count = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_raw_sentence_callback([&](const nmea_sentence&) { ++count; });

    const std::string sent = make_sentence("GNRMC,092725.00,A,4717.11399,N,00833.91590,E,0.004,77.52,160223,,,A");
    for (char c : sent)
        p.feed(std::vector<uint8_t>{static_cast<uint8_t>(c)});

    if (count != 1) { test_fail(name, "expected 1 dispatch feeding byte-by-byte"); return; }
    test_pass(name);
}

// ─── test: checksum mismatch discards sentence ────────────────────────────────

static void test_checksum_mismatch()
{
    const char* name = "stream_checksum_mismatch";

    int dispatched = 0;
    int errors = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_raw_sentence_callback([&](const nmea_sentence&) { ++dispatched; });
    p.set_error_callback([&](const parse_error_info& e) {
        if (e.code == parse_error_code::checksum_mismatch) ++errors;
    });

    // Deliberately wrong checksum (correct would be from make_sentence)
    const std::string bad = "$GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,*FF\r\n";
    p.feed(to_bytes(bad));

    if (dispatched != 0) { test_fail(name, "sentence with bad checksum should not be dispatched"); return; }
    if (errors != 1)     { test_fail(name, "expected 1 checksum_mismatch error"); return; }
    test_pass(name);
}

// ─── test: good + bad + good sequence ────────────────────────────────────────

static void test_recovery_after_bad_sentence()
{
    const char* name = "stream_recovery_after_bad";

    int dispatched = 0;
    int errors = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_raw_sentence_callback([&](const nmea_sentence&) { ++dispatched; });
    p.set_error_callback([&](const parse_error_info&) { ++errors; });

    const std::string good1 = make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,");
    const std::string bad   = "$GNGSA,A,3,,,,,,,,,,,,,99.9,99.9,99.9*FF\r\n";  // wrong checksum
    const std::string good2 = make_sentence("GNRMC,092725.00,A,4717.11399,N,00833.91590,E,0.004,77.52,160223,,,A");

    p.feed(to_bytes(good1 + bad + good2));

    if (dispatched != 2) { test_fail(name, "expected 2 good dispatches, got " + std::to_string(dispatched)); return; }
    if (errors != 1)     { test_fail(name, "expected 1 error for bad sentence"); return; }
    test_pass(name);
}

// ─── test: noise bytes before '$' are skipped ────────────────────────────────

static void test_noise_skipped()
{
    const char* name = "stream_noise_skipped";

    int count = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_raw_sentence_callback([&](const nmea_sentence&) { ++count; });

    const std::string sent = make_sentence("GNZDA,092725.00,16,02,2023,00,00");
    // Prepend noise bytes (UBX sync etc.)
    std::vector<uint8_t> data{0xB5u, 0x62u, 0x01u, 0x07u, 0xFFu};
    data.insert(data.end(), sent.begin(), sent.end());

    p.feed(data);
    if (count != 1) { test_fail(name, "expected 1 dispatch despite noise prefix"); return; }
    test_pass(name);
}

// ─── test: sentence too long ──────────────────────────────────────────────────

static void test_sentence_too_long()
{
    const char* name = "stream_sentence_too_long";

    int errors = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_error_callback([&](const parse_error_info& e) {
        if (e.code == parse_error_code::sentence_too_long) ++errors;
    });

    // Build a sentence longer than NMEA_BUFFER_LEN
    std::string overlong = "$GP";
    overlong += std::string(300u, 'X');  // way too long
    overlong += "\r\n";
    p.feed(to_bytes(overlong));

    if (errors != 1) { test_fail(name, "expected sentence_too_long error"); return; }
    test_pass(name);
}

// ─── test: sentence without checksum accepted ────────────────────────────────

static void test_no_checksum_sentence()
{
    const char* name = "stream_no_checksum";

    int count = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_raw_sentence_callback([&](const nmea_sentence& s) {
        if (!s.checksum_present) ++count;
    });

    // Some devices output sentences without checksum
    const std::string nocheck = "$GNVTG,0.0,T,,M,0.0,N,0.0,K,A\r\n";
    p.feed(to_bytes(nocheck));

    if (count != 1) { test_fail(name, "expected 1 dispatch for no-checksum sentence"); return; }
    test_pass(name);
}

// ─── test: new '$' interrupts previous sentence ───────────────────────────────

static void test_truncated_sentence_restarts()
{
    const char* name = "stream_truncated_restart";

    int errors = 0, dispatched = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_raw_sentence_callback([&](const nmea_sentence&) { ++dispatched; });
    p.set_error_callback([&](const parse_error_info& e) {
        if (e.code == parse_error_code::unexpected_start_in_sentence) ++errors;
    });

    // Truncated first sentence + complete second sentence
    const std::string truncated = "$GNGGA,09272";  // no terminator
    const std::string second = make_sentence("GNRMC,092725.00,A,4717.11399,N,00833.91590,E,0.004,77.52,160223,,,A");

    p.feed(to_bytes(truncated + second));

    if (errors != 1)    { test_fail(name, "expected 1 unexpected_start error"); return; }
    if (dispatched != 1) { test_fail(name, "expected 1 successful dispatch after restart"); return; }
    test_pass(name);
}

// ─── test: LF-only sentence termination (no CR) ──────────────────────────────

static void test_lf_only_termination()
{
    const char* name = "stream_lf_only";

    int count = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_raw_sentence_callback([&](const nmea_sentence&) { ++count; });

    std::string content = "GNVTG,0.0,T,,M,0.0,N,0.0,K,A";
    uint8_t cs = nmea_checksum(content);
    char buf[8];
    std::snprintf(buf, sizeof(buf), "*%02X\n", static_cast<unsigned>(cs));
    const std::string lf_only = "$" + content + buf;  // only \n, no \r

    p.feed(to_bytes(lf_only));
    if (count != 1) { test_fail(name, "LF-only termination should be accepted"); return; }
    test_pass(name);
}

// ─── test: statistics counters ────────────────────────────────────────────────

static void test_statistics()
{
    const char* name = "stream_statistics";

    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));

    const std::string good = make_sentence("GNVTG,0.0,T,,M,0.0,N,0.0,K,A");
    const std::string bad  = "$GNVTG,0.0,T,,M,0.0,N,0.0,K,A*FF\r\n";

    p.feed(to_bytes(good));
    p.feed(to_bytes(bad));
    p.feed(to_bytes(good));

    if (p.sentences_decoded()   != 2u) { test_fail(name, "expected 2 decoded"); return; }
    if (p.sentences_discarded() != 1u) { test_fail(name, "expected 1 discarded"); return; }

    p.reset();
    if (p.sentences_decoded()   != 0u) { test_fail(name, "reset should clear decoded"); return; }
    if (p.sentences_discarded() != 0u) { test_fail(name, "reset should clear discarded"); return; }
    test_pass(name);
}

// ─── test: report_unknown_sentences flag ─────────────────────────────────────

static void test_unknown_sentence_reporting()
{
    const char* name = "stream_unknown_sentence_reporting";

    int unknown_errors = 0;
    nmea_sentence_registry reg;
    nmea_parser p(std::move(reg));
    p.set_report_unknown_sentences(true);
    p.set_error_callback([&](const parse_error_info& e) {
        if (e.code == parse_error_code::unknown_sentence_type) ++unknown_errors;
    });

    const std::string s = make_sentence("GNZDA,092725.00,16,02,2023,00,00");
    p.feed(to_bytes(s));

    if (unknown_errors != 1) { test_fail(name, "expected 1 unknown_sentence_type error"); return; }
    test_pass(name);
}

// ─── entry point ──────────────────────────────────────────────────────────────

void run_parser_stream_tests()
{
    std::printf("--- parser stream ---\n");
    test_single_sentence();
    test_multiple_sentences_one_feed();
    test_byte_by_byte();
    test_checksum_mismatch();
    test_recovery_after_bad_sentence();
    test_noise_skipped();
    test_sentence_too_long();
    test_no_checksum_sentence();
    test_truncated_sentence_restarts();
    test_lf_only_termination();
    test_statistics();
    test_unknown_sentence_reporting();
}
