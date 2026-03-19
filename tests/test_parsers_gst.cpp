// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parsers_gst.cpp

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/gst_parser.h"
#include "sentences/nmea_gst.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;
static const double kEps = 1e-4;

static void test_gst_basic()
{
    const char* name = "gst_basic";
    nmea_gst result{};
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gst_parser>(
        new gst_parser([&](const nmea_gst& g) { result = g; })));
    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence("GNGST,092725.00,16.6,0.8,0.8,35.87,1.4,1.4,3.0")));

    if (!result.valid)                               { test_fail(name, "not valid"); return; }
    if (std::fabs(result.rms_dev - 16.6) > kEps)    { test_fail(name, "rms_dev"); return; }
    if (std::fabs(result.lat_err - 1.4) > kEps)     { test_fail(name, "lat_err"); return; }
    if (std::fabs(result.lon_err - 1.4) > kEps)     { test_fail(name, "lon_err"); return; }
    if (std::fabs(result.alt_err - 3.0) > kEps)     { test_fail(name, "alt_err"); return; }
    test_pass(name);
}

void run_parsers_gst_tests()
{
    std::printf("--- parsers GST ---\n");
    test_gst_basic();
}
