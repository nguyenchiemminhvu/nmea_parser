// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parsers_gbs.cpp

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/gbs_parser.h"
#include "sentences/nmea_gbs.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;
static const double kEps = 1e-5;

static void test_gbs_basic()
{
    const char* name = "gbs_basic";
    nmea_gbs result{};
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gbs_parser>(
        new gbs_parser([&](const nmea_gbs& g) { result = g; })));
    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence("GNGBS,092725.00,1.4,1.4,3.0,,,,0")));

    if (!result.valid)                               { test_fail(name, "not valid"); return; }
    if (std::fabs(result.err_lat - 1.4) > kEps)     { test_fail(name, "err_lat"); return; }
    if (std::fabs(result.err_lon - 1.4) > kEps)     { test_fail(name, "err_lon"); return; }
    if (std::fabs(result.err_alt - 3.0) > kEps)     { test_fail(name, "err_alt"); return; }
    test_pass(name);
}

void run_parsers_gbs_tests()
{
    std::printf("--- parsers GBS ---\n");
    test_gbs_basic();
}
