// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_database_gga.cpp
//
// Tests the DB handler field-mapping for GGA.

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "database/nmea_database.h"
#include "database/nmea_database_adapter.h"
#include "database/nmea_data_fields.h"
#include "database/handlers/db_gga_handler.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;
using namespace nmea::database;
static const double kEps = 1e-5;

static void test_db_gga_field_mapping()
{
    const char* name = "db_gga_field_mapping";

    auto db = std::make_shared<nmea_database>();

    nmea_snapshot snap;
    db->set_commit_callback(commit_kind::high_priority, [&](const nmea_snapshot& s) {
        snap = s;
    });

    nmea_sentence_registry reg;
    nmea_database_adapter adapter(db);
    adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_gga_handler()));
    adapter.register_with_parser(reg);

    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,")));

    double lat = 0.0, lon = 0.0, hdop = 0.0, alt = 0.0;
    int fix = -1, nsats = -1;

    if (!snap.get(NMEA_GGA_LATITUDE, lat))        { test_fail(name, "LATITUDE missing"); return; }
    if (!snap.get(NMEA_GGA_LONGITUDE, lon))       { test_fail(name, "LONGITUDE missing"); return; }
    if (!snap.get(NMEA_GGA_FIX_QUALITY, fix))     { test_fail(name, "FIX_QUALITY missing"); return; }
    if (!snap.get(NMEA_GGA_NUM_SATELLITES, nsats)){ test_fail(name, "NUM_SATELLITES missing"); return; }
    if (!snap.get(NMEA_GGA_HDOP, hdop))           { test_fail(name, "HDOP missing"); return; }
    if (!snap.get(NMEA_GGA_ALTITUDE_MSL, alt))    { test_fail(name, "ALTITUDE missing"); return; }

    if (std::fabs(lat  - 47.285233) > 1e-4) { test_fail(name, "lat value"); return; }
    if (std::fabs(lon  - 8.565265) > 1e-4)  { test_fail(name, "lon value"); return; }
    if (fix   != 1)                          { test_fail(name, "fix_quality should be 1"); return; }
    if (nsats != 8)                          { test_fail(name, "num_satellites should be 8"); return; }
    if (std::fabs(hdop - 1.01) > kEps)      { test_fail(name, "hdop"); return; }
    if (std::fabs(alt  - 499.6) > kEps)     { test_fail(name, "altitude"); return; }

    test_pass(name);
}

static void test_db_gga_snapshot_kind()
{
    const char* name = "db_gga_snapshot_kind";

    auto db = std::make_shared<nmea_database>();
    commit_kind k = commit_kind::none;
    db->set_commit_callback(commit_kind::high_priority, [&](const nmea_snapshot& s) {
        k = s.kind();
    });

    nmea_sentence_registry reg;
    nmea_database_adapter adapter(db);
    adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_gga_handler()));
    adapter.register_with_parser(reg);

    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,")));

    if (k != commit_kind::high_priority) { test_fail(name, "expected high_priority commit kind"); return; }
    test_pass(name);
}

static void test_db_no_callback_no_crash()
{
    const char* name = "db_no_callback_no_crash";
    auto db = std::make_shared<nmea_database>();
    // No commit callback registered
    nmea_sentence_registry reg;
    nmea_database_adapter adapter(db);
    adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_gga_handler()));
    adapter.register_with_parser(reg);
    nmea_parser p(std::move(reg));
    p.feed(to_bytes(make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,")));
    test_pass(name);
}

void run_database_gga_tests()
{
    std::printf("--- database GGA ---\n");
    test_db_gga_field_mapping();
    test_db_gga_snapshot_kind();
    test_db_no_callback_no_crash();
}
