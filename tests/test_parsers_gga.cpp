// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parsers_gga.cpp

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/gga_parser.h"
#include "sentences/nmea_gga.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;

static const double kEps = 1e-5;

// ─── helpers ──────────────────────────────────────────────────────────────────

static nmea_gga parse_one_gga(const std::string& sentence_body)
{
    nmea_gga result{};
    bool called = false;

    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gga_parser>(
        new gga_parser([&](const nmea_gga& g) { result = g; called = true; })));

    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence(sentence_body)));
    return result;
}

// ─── tests ────────────────────────────────────────────────────────────────────

static void test_gga_basic()
{
    const char* name = "gga_basic_decode";
    // Real-world sentence from u-blox ZED-F9R
    auto g = parse_one_gga("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,");

    if (!g.valid)                                   { test_fail(name, "not valid"); return; }
    if (g.talker != "GN")                           { test_fail(name, "talker"); return; }
    if (std::fabs(g.utc_time - 92725.0) > kEps)    { test_fail(name, "utc_time"); return; }
    // Lat: 4717.11399N → 47 + 17.11399/60 = 47.285233...
    if (std::fabs(g.latitude  - 47.285233) > 1e-4) { test_fail(name, "latitude"); return; }
    // Lon: 00833.91590E → 8 + 33.91590/60 = 8.565265...
    if (std::fabs(g.longitude - 8.565265) > 1e-4)  { test_fail(name, "longitude"); return; }
    if (g.fix_quality != gga_fix_quality::gps_sps)  { test_fail(name, "fix_quality"); return; }
    if (g.num_satellites != 8u)                     { test_fail(name, "num_satellites"); return; }
    if (std::fabs(g.hdop - 1.01) > kEps)            { test_fail(name, "hdop"); return; }
    if (std::fabs(g.altitude_msl - 499.6) > kEps)   { test_fail(name, "altitude"); return; }
    if (std::fabs(g.geoid_sep - 48.0) > kEps)       { test_fail(name, "geoid_sep"); return; }
    test_pass(name);
}

static void test_gga_south_west()
{
    const char* name = "gga_south_west";
    auto g = parse_one_gga("GPGGA,000000.00,3320.50000,S,07030.50000,W,1,10,0.9,5.0,M,0.0,M,,");
    if (!g.valid)                                    { test_fail(name, "not valid"); return; }
    if (g.latitude  >= 0.0)                          { test_fail(name, "expected negative lat"); return; }
    if (g.longitude >= 0.0)                          { test_fail(name, "expected negative lon"); return; }
    // Lat: -(33 + 20.5/60) = -33.341666...
    if (std::fabs(g.latitude  + 33.34167) > 1e-4)   { test_fail(name, "lat value"); return; }
    test_pass(name);
}

static void test_gga_rtk_fixed()
{
    const char* name = "gga_rtk_fixed";
    auto g = parse_one_gga("GNGGA,120000.00,5130.00000,N,00014.00000,W,4,12,0.5,100.0,M,50.0,M,1.5,1234");
    if (!g.valid)                                          { test_fail(name, "not valid"); return; }
    if (g.fix_quality != gga_fix_quality::rtk_fixed)       { test_fail(name, "expected rtk_fixed"); return; }
    if (std::fabs(g.dgps_age - 1.5) > kEps)               { test_fail(name, "dgps_age"); return; }
    if (g.dgps_station_id != 1234u)                        { test_fail(name, "dgps_station_id"); return; }
    test_pass(name);
}

static void test_gga_empty_optional_fields()
{
    const char* name = "gga_empty_optional";
    // Empty DGPS fields (no DGPS)
    auto g = parse_one_gga("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,");
    if (!g.valid)         { test_fail(name, "not valid"); return; }
    if (g.dgps_age >= 0.0){ test_fail(name, "dgps_age should be -1 for empty field"); return; }
    test_pass(name);
}

static void test_gga_too_few_fields()
{
    const char* name = "gga_too_few_fields";
    bool called = false;
    int errors = 0;
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gga_parser>(
        new gga_parser([&](const nmea_gga&) { called = true; })));
    nmea_parser p(std::move(reg));
    p.set_error_callback([&](const parse_error_info& e) {
        if (e.code == parse_error_code::malformed_fields) ++errors;
    });
    // Only 5 fields — too few for GGA (needs 14)
    p.feed(to_bytes(make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590")));
    if (called)   { test_fail(name, "callback should not fire for too-few fields"); return; }
    if (errors != 1) { test_fail(name, "expected malformed_fields error"); return; }
    test_pass(name);
}

static void test_gga_callback_not_called_when_null()
{
    const char* name = "gga_null_callback_nocrash";
    // Pass null callback — parser should not crash
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gga_parser>(
        new gga_parser(nullptr)));
    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,")));
    test_pass(name);
}

// ─── entry point ──────────────────────────────────────────────────────────────

void run_parsers_gga_tests()
{
    std::printf("--- parsers GGA ---\n");
    test_gga_basic();
    test_gga_south_west();
    test_gga_rtk_fixed();
    test_gga_empty_optional_fields();
    test_gga_too_few_fields();
    test_gga_callback_not_called_when_null();
}
