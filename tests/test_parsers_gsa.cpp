// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parsers_gsa.cpp

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/gsa_parser.h"
#include "sentences/nmea_gsa.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;
static const double kEps = 1e-5;

static nmea_gsa parse_one_gsa(const std::string& body)
{
    nmea_gsa result{};
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gsa_parser>(
        new gsa_parser([&](const nmea_gsa& g) { result = g; })));
    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence(body)));
    return result;
}

static void test_gsa_basic_3d_fix()
{
    const char* name = "gsa_basic_3d_fix";
    auto g = parse_one_gsa("GNGSA,A,3,08,01,17,03,28,10,32,,,,,,1.35,1.01,0.91,1");
    if (!g.valid)                                { test_fail(name, "not valid"); return; }
    if (g.op_mode != gsa_op_mode::auto_mode)     { test_fail(name, "op_mode"); return; }
    if (g.nav_mode != gsa_nav_mode::fix_3d)      { test_fail(name, "nav_mode"); return; }
    if (g.sat_ids[0] != 8u)                      { test_fail(name, "sat_ids[0]"); return; }
    if (g.sat_ids[1] != 1u)                      { test_fail(name, "sat_ids[1]"); return; }
    if (g.sat_ids[6] != 32u)                      { test_fail(name, "sat_ids[6]"); return; }
    if (std::fabs(g.pdop - 1.35) > kEps)         { test_fail(name, "pdop"); return; }
    if (std::fabs(g.hdop - 1.01) > kEps)         { test_fail(name, "hdop"); return; }
    if (std::fabs(g.vdop - 0.91) > kEps)         { test_fail(name, "vdop"); return; }
    if (g.system_id != 1u)                       { test_fail(name, "system_id"); return; }
    test_pass(name);
}

static void test_gsa_2d_fix()
{
    const char* name = "gsa_2d_fix";
    auto g = parse_one_gsa("GNGSA,M,2,01,02,03,,,,,,,,,,,2.5,2.0,1.5,2");
    if (!g.valid)                           { test_fail(name, "not valid"); return; }
    if (g.op_mode != gsa_op_mode::manual)   { test_fail(name, "op_mode"); return; }
    if (g.nav_mode != gsa_nav_mode::fix_2d) { test_fail(name, "nav_mode"); return; }
    test_pass(name);
}

static void test_gsa_no_fix()
{
    const char* name = "gsa_no_fix";
    auto g = parse_one_gsa("GPGSA,A,1,,,,,,,,,,,,,,,99.9,99.9,99.9");
    if (!g.valid)                           { test_fail(name, "not valid"); return; }
    if (g.nav_mode != gsa_nav_mode::no_fix) { test_fail(name, "nav_mode"); return; }
    for (int i = 0; i < 12; ++i)
        if (g.sat_ids[static_cast<size_t>(i)] != 0u) { test_fail(name, "empty sat slot should be 0"); return; }
    test_pass(name);
}

void run_parsers_gsa_tests()
{
    std::printf("--- parsers GSA ---\n");
    test_gsa_basic_3d_fix();
    test_gsa_2d_fix();
    test_gsa_no_fix();
}
