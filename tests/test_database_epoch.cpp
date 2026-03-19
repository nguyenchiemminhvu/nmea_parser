// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_database_epoch.cpp
//
// Tests the epoch-commit policy, multi-message accumulation, and snapshot API.

#include "test_helpers.h"
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "database/nmea_database.h"
#include "database/nmea_database_adapter.h"
#include "database/nmea_data_fields.h"
#include "database/i_update_policy.h"
#include "database/handlers/db_gga_handler.h"
#include "database/handlers/db_rmc_handler.h"
#include "database/handlers/db_gsa_handler.h"
#include "database/handlers/db_gsv_handler.h"
#include "database/handlers/db_vtg_handler.h"
#include <cmath>
#include <memory>

using namespace nmea::parser;
using namespace nmea::database;

static const double kEps = 1e-4;

// ─── helper: build a full adapter with all 5 handlers ────────────────────────

static std::unique_ptr<nmea_database_adapter>
build_full_adapter(std::shared_ptr<nmea_database> db,
                   nmea_sentence_registry& reg)
{
    auto adapter = std::unique_ptr<nmea_database_adapter>(new nmea_database_adapter(db));
    adapter->add_handler(std::unique_ptr<i_sentence_handler>(new db_gga_handler()));
    adapter->add_handler(std::unique_ptr<i_sentence_handler>(new db_rmc_handler()));
    adapter->add_handler(std::unique_ptr<i_sentence_handler>(new db_gsa_handler()));
    adapter->add_handler(std::unique_ptr<i_sentence_handler>(new db_gsv_handler()));
    adapter->add_handler(std::unique_ptr<i_sentence_handler>(new db_vtg_handler()));
    adapter->register_with_parser(reg);
    return adapter;
}

// ─── test: GGA triggers HP commit, GSA does not ──────────────────────────────

static void test_gga_triggers_hp_not_gsa()
{
    const char* name = "epoch_gga_triggers_hp_not_gsa";

    auto db = std::make_shared<nmea_database>();
    int hp_count = 0, lp_count = 0;
    db->set_commit_callback(commit_kind::high_priority, [&](const nmea_snapshot&) { ++hp_count; });
    db->set_commit_callback(commit_kind::low_priority,  [&](const nmea_snapshot&) { ++lp_count; });

    nmea_sentence_registry reg;
    auto adapter = build_full_adapter(db, reg);
    nmea_parser p(std::move(reg));

    // GSA alone — no commit
    p.feed(to_bytes(make_sentence("GNGSA,A,3,08,01,17,03,28,10,32,,,,,,1.35,1.01,0.91,1")));
    if (hp_count != 0 || lp_count != 0) { test_fail(name, "GSA alone should not commit"); return; }

    // GGA → HP commit
    p.feed(to_bytes(make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,")));
    if (hp_count != 1) { test_fail(name, "GGA should trigger HP commit"); return; }
    if (lp_count != 0) { test_fail(name, "no LP commit expected yet"); return; }

    test_pass(name);
}

// ─── test: GSV triggers LP commit ────────────────────────────────────────────

static void test_gsv_triggers_lp()
{
    const char* name = "epoch_gsv_triggers_lp";

    auto db = std::make_shared<nmea_database>();
    int lp_count = 0;
    db->set_commit_callback(commit_kind::low_priority, [&](const nmea_snapshot&) { ++lp_count; });

    nmea_sentence_registry reg;
    auto adapter = build_full_adapter(db, reg);
    nmea_parser p(std::move(reg));

    // Send a complete 1-message GSV set
    p.feed(to_bytes(make_sentence("GPGSV,1,1,02,21,40,083,46,25,42,057,50")));
    if (lp_count != 1) { test_fail(name, "GSV(1/1) should trigger LP commit"); return; }
    test_pass(name);
}

// ─── test: multi-part GSV only commits on last part ──────────────────────────

static void test_gsv_multipart_commits_on_last()
{
    const char* name = "epoch_gsv_multipart_commit";

    auto db = std::make_shared<nmea_database>();
    int lp_count = 0;
    db->set_commit_callback(commit_kind::low_priority, [&](const nmea_snapshot&) { ++lp_count; });

    nmea_sentence_registry reg;
    auto adapter = build_full_adapter(db, reg);
    nmea_parser p(std::move(reg));

    // Part 1 of 3 — should NOT commit
    p.feed(to_bytes(make_sentence("GPGSV,3,1,10,21,40,083,46,05,35,141,47,25,42,057,50,46,38,103,34")));
    if (lp_count != 0) { test_fail(name, "part 1/3 should not commit"); return; }

    // Part 2 of 3 — should NOT commit
    p.feed(to_bytes(make_sentence("GPGSV,3,2,10,12,10,020,35,30,55,200,42,11,25,342,28,07,05,089,22")));
    if (lp_count != 0) { test_fail(name, "part 2/3 should not commit"); return; }

    // Part 3 of 3 — SHOULD commit
    p.feed(to_bytes(make_sentence("GPGSV,3,3,10,16,08,320,19,18,70,180,40")));
    if (lp_count != 1) { test_fail(name, "part 3/3 should commit once"); return; }

    test_pass(name);
}

// ─── test: snapshot() is thread-safe readable read ───────────────────────────

static void test_snapshot_read_after_gga()
{
    const char* name = "epoch_snapshot_read_after_gga";

    auto db = std::make_shared<nmea_database>();
    nmea_sentence_registry reg;
    auto adapter = build_full_adapter(db, reg);
    nmea_parser p(std::move(reg));

    p.feed(to_bytes(make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,")));

    const nmea_snapshot snap = db->snapshot();
    double lat = 0.0;
    if (!snap.get(NMEA_GGA_LATITUDE, lat))       { test_fail(name, "lat not in snapshot"); return; }
    if (std::fabs(lat - 47.285233) > kEps)       { test_fail(name, "lat value"); return; }
    test_pass(name);
}

// ─── test: custom policy overrides default ────────────────────────────────────

static void test_custom_policy()
{
    const char* name = "epoch_custom_policy";

    // Custom policy: nothing commits
    struct no_commit_policy : public i_update_policy {
        commit_kind should_commit(msg_type) const override { return commit_kind::none; }
    };

    auto db = std::make_shared<nmea_database>(
        std::unique_ptr<i_update_policy>(new no_commit_policy()));

    int hp_count = 0, lp_count = 0;
    db->set_commit_callback(commit_kind::high_priority, [&](const nmea_snapshot&) { ++hp_count; });
    db->set_commit_callback(commit_kind::low_priority,  [&](const nmea_snapshot&) { ++lp_count; });

    nmea_sentence_registry reg;
    auto adapter = build_full_adapter(db, reg);
    nmea_parser p(std::move(reg));

    p.feed(to_bytes(make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,")));
    p.feed(to_bytes(make_sentence("GPGSV,1,1,02,21,40,083,46,25,42,057,50")));

    if (hp_count != 0 || lp_count != 0) { test_fail(name, "custom policy should prevent all commits"); return; }
    test_pass(name);
}

// ─── test: multiple GGA commits = multiple snapshots with updated data ────────

static void test_multiple_gga_commits()
{
    const char* name = "epoch_multiple_gga_commits";

    auto db = std::make_shared<nmea_database>();
    int hp_count = 0;
    double last_lat = 0.0;
    db->set_commit_callback(commit_kind::high_priority, [&](const nmea_snapshot& s) {
        ++hp_count;
        s.get(NMEA_GGA_LATITUDE, last_lat);
    });

    nmea_sentence_registry reg;
    auto adapter = build_full_adapter(db, reg);
    nmea_parser p(std::move(reg));

    p.feed(to_bytes(make_sentence("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,")));
    p.feed(to_bytes(make_sentence("GNGGA,092726.00,4800.00000,N,00900.00000,E,1,10,0.9,500.0,M,48.0,M,,")));

    if (hp_count != 2) { test_fail(name, "expected 2 HP commits"); return; }
    // Last lat should be lat from second GGA: 48 + 0/60 = 48.0
    if (std::fabs(last_lat - 48.0) > kEps) { test_fail(name, "last snapshot lat should be 48.0"); return; }
    test_pass(name);
}

// ─── test: GSV per-satellite fields stored in parallel arrays ────────────────

static void test_gsv_satellite_fields()
{
    const char* name = "epoch_gsv_satellite_fields";

    auto db = std::make_shared<nmea_database>();
    nmea_sentence_registry reg;
    auto adapter = build_full_adapter(db, reg);
    nmea_parser p(std::move(reg));

    // Single-part GSV with 2 satellites:
    //   SV#21  elev=40  az=083  snr=46
    //   SV#25  elev=42  az=057  snr=50
    p.feed(to_bytes(make_sentence("GPGSV,1,1,02,21,40,083,46,25,42,057,50")));

    const nmea_snapshot snap = db->snapshot();

    // num_sats_in_view
    int nsats = 0;
    if (!snap.get(NMEA_GSV_NUM_SATS_IN_VIEW, nsats))  { test_fail(name, "num_sats_in_view missing"); return; }
    if (nsats != 2)                                    { test_fail(name, "num_sats_in_view should be 2"); return; }

    // Satellite 0 (slot index 0)
    int sv0 = 0, el0 = 0, az0 = 0, snr0 = 0;
    if (!snap.get(nmea_gsv_sat_field(NMEA_GSV_SAT_SVID,      0u), sv0))  { test_fail(name, "sat[0] svid missing"); return; }
    if (!snap.get(nmea_gsv_sat_field(NMEA_GSV_SAT_ELEVATION,  0u), el0))  { test_fail(name, "sat[0] elev missing"); return; }
    if (!snap.get(nmea_gsv_sat_field(NMEA_GSV_SAT_AZIMUTH,    0u), az0))  { test_fail(name, "sat[0] az missing"); return; }
    if (!snap.get(nmea_gsv_sat_field(NMEA_GSV_SAT_SNR,        0u), snr0)) { test_fail(name, "sat[0] snr missing"); return; }
    if (sv0  != 21) { test_fail(name, "sat[0] svid should be 21"); return; }
    if (el0  != 40) { test_fail(name, "sat[0] elev should be 40"); return; }
    if (az0  != 83) { test_fail(name, "sat[0] az should be 83"); return; }
    if (snr0 != 46) { test_fail(name, "sat[0] snr should be 46"); return; }

    // Satellite 1 (slot index 1)
    int sv1 = 0, el1 = 0, az1 = 0, snr1 = 0;
    if (!snap.get(nmea_gsv_sat_field(NMEA_GSV_SAT_SVID,      1u), sv1))  { test_fail(name, "sat[1] svid missing"); return; }
    if (!snap.get(nmea_gsv_sat_field(NMEA_GSV_SAT_ELEVATION,  1u), el1))  { test_fail(name, "sat[1] elev missing"); return; }
    if (!snap.get(nmea_gsv_sat_field(NMEA_GSV_SAT_AZIMUTH,    1u), az1))  { test_fail(name, "sat[1] az missing"); return; }
    if (!snap.get(nmea_gsv_sat_field(NMEA_GSV_SAT_SNR,        1u), snr1)) { test_fail(name, "sat[1] snr missing"); return; }
    if (sv1  != 25) { test_fail(name, "sat[1] svid should be 25"); return; }
    if (el1  != 42) { test_fail(name, "sat[1] elev should be 42"); return; }
    if (az1  != 57) { test_fail(name, "sat[1] az should be 57"); return; }
    if (snr1 != 50) { test_fail(name, "sat[1] snr should be 50"); return; }

    test_pass(name);
}

void run_database_epoch_tests()
{
    std::printf("--- database epoch ---\n");
    test_gga_triggers_hp_not_gsa();
    test_gsv_triggers_lp();
    test_gsv_multipart_commits_on_last();
    test_snapshot_read_after_gga();
    test_custom_policy();
    test_multiple_gga_commits();
    test_gsv_satellite_fields();
}
