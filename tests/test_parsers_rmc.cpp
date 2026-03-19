// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parsers_rmc.cpp

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/rmc_parser.h"
#include "sentences/nmea_rmc.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;
static const double kEps = 1e-5;

static nmea_rmc parse_one_rmc(const std::string& body)
{
    nmea_rmc result{};
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<rmc_parser>(
        new rmc_parser([&](const nmea_rmc& r) { result = r; })));
    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence(body)));
    return result;
}

static void test_rmc_basic()
{
    const char* name = "rmc_basic";
    auto r = parse_one_rmc("GNRMC,092725.00,A,4717.11399,N,00833.91590,E,0.004,77.52,160223,,,A");
    if (!r.valid)                                   { test_fail(name, "not valid"); return; }
    if (!r.status_active)                           { test_fail(name, "expected active"); return; }
    if (std::fabs(r.utc_time - 92725.0) > kEps)    { test_fail(name, "utc_time"); return; }
    if (std::fabs(r.latitude  - 47.285233) > 1e-4) { test_fail(name, "latitude"); return; }
    if (std::fabs(r.longitude - 8.565265) > 1e-4)  { test_fail(name, "longitude"); return; }
    if (std::fabs(r.speed_knots - 0.004) > kEps)   { test_fail(name, "speed"); return; }
    if (std::fabs(r.course_true - 77.52) > kEps)   { test_fail(name, "course_true"); return; }
    if (r.date != 160223u)                          { test_fail(name, "date"); return; }
    if (r.mode != rmc_mode::autonomous)             { test_fail(name, "mode"); return; }
    test_pass(name);
}

static void test_rmc_void_status()
{
    const char* name = "rmc_void_status";
    auto r = parse_one_rmc("GNRMC,000000.00,V,0000.00000,N,00000.00000,E,0.0,0.0,010100,,,N");
    if (!r.valid)      { test_fail(name, "struct valid"); return; }
    if (r.status_active) { test_fail(name, "should be inactive (V)"); return; }
    test_pass(name);
}

static void test_rmc_mag_variation_west()
{
    const char* name = "rmc_mag_variation_west";
    auto r = parse_one_rmc("GPRMC,120000.00,A,5130.00000,N,00014.00000,W,0.0,0.0,010124,5.0,W,A");
    if (!r.valid)                                 { test_fail(name, "not valid"); return; }
    if (r.mag_variation >= 0.0)                   { test_fail(name, "expected negative mag variation"); return; }
    if (std::fabs(r.mag_variation + 5.0) > kEps) { test_fail(name, "mag_variation value"); return; }
    test_pass(name);
}

void run_parsers_rmc_tests()
{
    std::printf("--- parsers RMC ---\n");
    test_rmc_basic();
    test_rmc_void_status();
    test_rmc_mag_variation_west();
}
