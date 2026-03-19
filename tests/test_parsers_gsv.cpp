// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parsers_gsv.cpp

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/gsv_parser.h"
#include "sentences/nmea_gsv.h"
#include <memory>
#include <vector>

using namespace nmea::parser;

static void test_gsv_single_part()
{
    const char* name = "gsv_single_part";
    std::vector<nmea_gsv> parts;
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gsv_parser>(
        new gsv_parser([&](const nmea_gsv& g) { parts.push_back(g); })));
    nmea_parser p(std::move(reg));

    // 1 message, message 1 of 1, 2 satellites in view
    p.feed(to_bytes(make_sentence("GPGSV,1,1,02,21,40,083,46,25,42,057,50")));

    if (parts.size() != 1u)                { test_fail(name, "expected 1 part"); return; }
    const auto& g = parts[0];
    if (!g.valid)                          { test_fail(name, "not valid"); return; }
    if (g.num_msgs != 1u)                  { test_fail(name, "num_msgs"); return; }
    if (g.msg_num  != 1u)                  { test_fail(name, "msg_num"); return; }
    if (g.sats_in_view != 2u)             { test_fail(name, "sats_in_view"); return; }
    if (g.sat_count != 2u)                { test_fail(name, "sat_count"); return; }
    if (g.satellites[0].sv_id    != 21u)  { test_fail(name, "sv[0].sv_id"); return; }
    if (g.satellites[0].elevation != 40)  { test_fail(name, "sv[0].elevation"); return; }
    if (g.satellites[0].azimuth  != 83u)  { test_fail(name, "sv[0].azimuth"); return; }
    if (g.satellites[0].snr      != 46)   { test_fail(name, "sv[0].snr"); return; }
    if (g.satellites[1].sv_id    != 25u)  { test_fail(name, "sv[1].sv_id"); return; }
    test_pass(name);
}

static void test_gsv_multi_part()
{
    const char* name = "gsv_multi_part";
    std::vector<nmea_gsv> parts;
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gsv_parser>(
        new gsv_parser([&](const nmea_gsv& g) { parts.push_back(g); })));
    nmea_parser p(std::move(reg));

    // 3 message set, 10 satellites total
    p.feed(to_bytes(make_sentence("GPGSV,3,1,10,21,40,083,46,05,35,141,47,25,42,057,50,46,38,103,34")));
    p.feed(to_bytes(make_sentence("GPGSV,3,2,10,12,10,020,35,30,55,200,42,11,25,342,28,07,05,089,22")));
    p.feed(to_bytes(make_sentence("GPGSV,3,3,10,16,08,320,19,18,70,180,40")));

    if (parts.size() != 3u) { test_fail(name, "expected 3 parts"); return; }
    if (parts[0].sat_count != 4u) { test_fail(name, "part 1 should have 4 sats"); return; }
    if (parts[1].sat_count != 4u) { test_fail(name, "part 2 should have 4 sats"); return; }
    if (parts[2].sat_count != 2u) { test_fail(name, "part 3 should have 2 sats"); return; }
    test_pass(name);
}

static void test_gsv_no_snr_tracking()
{
    const char* name = "gsv_no_snr";
    std::vector<nmea_gsv> parts;
    nmea_sentence_registry reg;
    reg.register_parser(std::unique_ptr<gsv_parser>(
        new gsv_parser([&](const nmea_gsv& g) { parts.push_back(g); })));
    nmea_parser p(std::move(reg));

    // SNR field is empty (not tracking)
    p.feed(to_bytes(make_sentence("GLGSV,1,1,02,65,10,020,,66,15,180,")));

    if (parts.empty())               { test_fail(name, "no part received"); return; }
    if (parts[0].satellites[0].snr != -1) { test_fail(name, "empty snr should be -1"); return; }
    test_pass(name);
}

void run_parsers_gsv_tests()
{
    std::printf("--- parsers GSV ---\n");
    test_gsv_single_part();
    test_gsv_multi_part();
    test_gsv_no_snr_tracking();
}
