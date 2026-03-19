// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_checksum.cpp

#include "test_helpers.h"
#include "internal/checksum.h"

using namespace nmea::parser::internal;

// ─── tests ────────────────────────────────────────────────────────────────────

static void test_checksum_basic()
{
    const char* name = "checksum_basic";
    // Known checksum for "GNGGA,092725.00" — computed manually:
    //   'G'^'N'^'G'^'G'^'A'^','^ '0'^'9'^'2'^'7'^'2'^'5'^'.'^ '0'^'0' = ?
    // We'll use the make_sentence helper and verify our compute_checksum matches.
    std::string content = "GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,";
    uint8_t expected = nmea_checksum(content);

    // Exclude the leading '$' in content for compute_checksum
    // (content has no '$', the make_sentence adds it)
    uint8_t computed = compute_checksum(content.c_str(), content.size());
    if (computed != expected)
    {
        test_fail(name, "checksum mismatch");
        return;
    }
    test_pass(name);
}

static void test_checksum_empty()
{
    const char* name = "checksum_empty";
    uint8_t result = compute_checksum("", 0u);
    if (result != 0u)
    {
        test_fail(name, "expected 0 for empty input");
        return;
    }
    test_pass(name);
}

static void test_checksum_single_byte()
{
    const char* name = "checksum_single_byte";
    const char data[] = "A";
    uint8_t result = compute_checksum(data, 1u);
    if (result != static_cast<uint8_t>('A'))
    {
        test_fail(name, "XOR of single byte should equal that byte");
        return;
    }
    test_pass(name);
}

static void test_checksum_xor_cancels()
{
    const char* name = "checksum_xor_cancels";
    const char data[] = "AA";
    uint8_t result = compute_checksum(data, 2u);
    if (result != 0u)
    {
        test_fail(name, "XOR of identical bytes should be 0");
        return;
    }
    test_pass(name);
}

static void test_parse_hex_byte_valid()
{
    const char* name = "parse_hex_byte_valid";
    uint8_t out = 0u;
    if (!parse_hex_byte('4', 'E', out) || out != 0x4Eu)
    {
        test_fail(name, "failed to parse '4''E'");
        return;
    }
    if (!parse_hex_byte('a', 'f', out) || out != 0xAFu)
    {
        test_fail(name, "failed to parse 'a''f'");
        return;
    }
    test_pass(name);
}

static void test_parse_hex_byte_invalid()
{
    const char* name = "parse_hex_byte_invalid";
    uint8_t out = 0u;
    if (parse_hex_byte('G', '0', out))
    {
        test_fail(name, "should fail for non-hex char G");
        return;
    }
    if (parse_hex_byte('0', 'Z', out))
    {
        test_fail(name, "should fail for non-hex char Z");
        return;
    }
    test_pass(name);
}

// ─── entry point ──────────────────────────────────────────────────────────────

void run_checksum_tests()
{
    std::printf("--- checksum ---\n");
    test_checksum_basic();
    test_checksum_empty();
    test_checksum_single_byte();
    test_checksum_xor_cancels();
    test_parse_hex_byte_valid();
    test_parse_hex_byte_invalid();
}
