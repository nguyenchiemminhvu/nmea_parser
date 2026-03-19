// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parsers_vtg.cpp

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/vtg_parser.h"
#include "sentences/nmea_vtg.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;
static const double kEps = 1e-5;

static void test_vtg_basic()
{
    const char* name = "vtg_basic";
    nmea_vtg result{};
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<vtg_parser>(
        new vtg_parser([&](const nmea_vtg& v) { result = v; })));
    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence("GNVTG,77.52,T,,M,0.004,N,0.007,K,A")));

    if (!result.valid)                               { test_fail(name, "not valid"); return; }
    if (std::fabs(result.course_true - 77.52) > kEps){ test_fail(name, "course_true"); return; }
    if (result.course_mag_valid)                     { test_fail(name, "course_mag empty should be valid=false"); return; }
    if (std::fabs(result.speed_knots - 0.004) > kEps){ test_fail(name, "speed_knots"); return; }
    if (std::fabs(result.speed_kmh   - 0.007) > kEps){ test_fail(name, "speed_kmh"); return; }
    if (result.mode != 'A')                          { test_fail(name, "mode"); return; }
    test_pass(name);
}

static void test_vtg_stationary()
{
    const char* name = "vtg_stationary";
    nmea_vtg result{};
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<vtg_parser>(
        new vtg_parser([&](const nmea_vtg& v) { result = v; })));
    nmea_parser p(std::move(reg));
    // All-zero speed/course
    p.feed(to_bytes(make_sentence("GNVTG,0.0,T,,M,0.0,N,0.0,K,A")));
    if (!result.valid)                               { test_fail(name, "not valid"); return; }
    if (std::fabs(result.speed_knots) > kEps)        { test_fail(name, "speed should be 0"); return; }
    test_pass(name);
}

void run_parsers_vtg_tests()
{
    std::printf("--- parsers VTG ---\n");
    test_vtg_basic();
    test_vtg_stationary();
}
