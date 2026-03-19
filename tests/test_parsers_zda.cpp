// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parsers_zda.cpp

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/zda_parser.h"
#include "sentences/nmea_zda.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;

static void test_zda_basic()
{
    const char* name = "zda_basic";
    nmea_zda result{};
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<zda_parser>(
        new zda_parser([&](const nmea_zda& z) { result = z; })));
    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence("GNZDA,092725.00,16,02,2023,00,00")));

    if (!result.valid)          { test_fail(name, "not valid"); return; }
    if (result.day   != 16u)    { test_fail(name, "day"); return; }
    if (result.month != 2u)     { test_fail(name, "month"); return; }
    if (result.year  != 2023u)  { test_fail(name, "year"); return; }
    if (result.tz_hour != 0)    { test_fail(name, "tz_hour"); return; }
    if (result.tz_min  != 0u)   { test_fail(name, "tz_min"); return; }
    test_pass(name);
}

void run_parsers_zda_tests()
{
    std::printf("--- parsers ZDA ---\n");
    test_zda_basic();
}
