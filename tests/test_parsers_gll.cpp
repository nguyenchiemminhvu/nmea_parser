// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parsers_gll.cpp

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/gll_parser.h"
#include "sentences/nmea_gll.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;
static const double kEps = 1e-5;

static void test_gll_basic()
{
    const char* name = "gll_basic";
    nmea_gll result{};
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gll_parser>(
        new gll_parser([&](const nmea_gll& g) { result = g; })));
    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence("GNGLL,4717.11399,N,00833.91590,E,092725.00,A,A")));

    if (!result.valid)                                  { test_fail(name, "not valid"); return; }
    if (!result.status_active)                          { test_fail(name, "not active"); return; }
    if (std::fabs(result.latitude  - 47.285233) > 1e-4){ test_fail(name, "latitude"); return; }
    if (std::fabs(result.longitude - 8.565265) > 1e-4) { test_fail(name, "longitude"); return; }
    if (result.mode != 'A')                             { test_fail(name, "mode"); return; }
    test_pass(name);
}

void run_parsers_gll_tests()
{
    std::printf("--- parsers GLL ---\n");
    test_gll_basic();
}
