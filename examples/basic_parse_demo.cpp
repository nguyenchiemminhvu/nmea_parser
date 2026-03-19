// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// examples/basic_parse_demo.cpp
//
// End-to-end integration demo of the nmea_parser library.
//
// Shows both usage modes:
//   1. Parser-only: direct per-sentence callbacks
//   2. Parser + database: epoch-triggered snapshots

#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "nmea_errors.h"
#include "parsers/gga_parser.h"
#include "parsers/rmc_parser.h"
#include "parsers/gsv_parser.h"
#include "parsers/gsa_parser.h"
#include "parsers/vtg_parser.h"
#include "database/nmea_database.h"
#include "database/nmea_database_adapter.h"
#include "database/nmea_data_fields.h"
#include "database/handlers/db_gga_handler.h"
#include "database/handlers/db_rmc_handler.h"
#include "database/handlers/db_gsa_handler.h"
#include "database/handlers/db_gsv_handler.h"
#include "database/handlers/db_vtg_handler.h"

#include <cstdio>
#include <memory>
#include <vector>
#include <string>
#include <cstdint>

using namespace nmea::parser;
using namespace nmea::database;

// ─── Helpers ─────────────────────────────────────────────────────────────────

static uint8_t nmea_cs(const std::string& s)
{
    uint8_t cs = 0u;
    for (char c : s) { if (c == '$') continue; if (c == '*') break; cs ^= (uint8_t)c; }
    return cs;
}

static std::vector<uint8_t> make(const std::string& body)
{
    uint8_t cs = nmea_cs(body);
    char buf[8]; std::snprintf(buf, sizeof(buf), "*%02X\r\n", (unsigned)cs);
    std::string full = "$" + body + buf;
    return std::vector<uint8_t>(full.begin(), full.end());
}

// ─── Demo 1: parser only ─────────────────────────────────────────────────────

static void demo_parser_only()
{
    std::printf("\n=== Demo 1: Parser-only (direct callbacks) ===\n");

    nmea_sentence_registry reg;

    // Register GGA callback
    reg.register_parser(std::unique_ptr<gga_parser>(
        new gga_parser([](const nmea_gga& g) {
            std::printf("GGA: talker=%s  lat=%.6f  lon=%.6f  fix=%d  sats=%d  alt=%.1fm\n",
                g.talker.c_str(), g.latitude, g.longitude,
                static_cast<int>(g.fix_quality), g.num_satellites, g.altitude_msl);
        })));

    // Register RMC callback
    reg.register_parser(std::unique_ptr<rmc_parser>(
        new rmc_parser([](const nmea_rmc& r) {
            std::printf("RMC: status=%s  lat=%.6f  lon=%.6f  speed=%.3f kn  course=%.1f°\n",
                r.status_active ? "ACTIVE" : "VOID",
                r.latitude, r.longitude, r.speed_knots, r.course_true);
        })));

    // Register GSV callback
    reg.register_parser(std::unique_ptr<gsv_parser>(
        new gsv_parser([](const nmea_gsv& g) {
            std::printf("GSV: part %d/%d  sats_in_view=%d\n",
                g.msg_num, g.num_msgs, g.sats_in_view);
            for (uint8_t i = 0u; i < g.sat_count; ++i)
            {
                const auto& sv = g.satellites[i];
                std::printf("   SV#%3u  elev=%3d°  az=%3u°  SNR=%3d dBHz\n",
                    sv.sv_id, sv.elevation, sv.azimuth, sv.snr);
            }
        })));

    nmea_parser parser(std::move(reg));

    parser.set_error_callback([](const parse_error_info& e) {
        std::printf("ERROR: [%s] %s\n", e.sentence_type.c_str(), e.description.c_str());
    });

    // Feed a realistic stream from a u-blox ZED-F9R
    const auto s1 = make("GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,");
    const auto s2 = make("GNRMC,092725.00,A,4717.11399,N,00833.91590,E,0.004,77.52,160223,,,A");
    const auto s3 = make("GPGSV,2,1,05,21,40,083,46,05,35,141,47,25,42,057,50,46,38,103,34");
    const auto s4 = make("GPGSV,2,2,05,16,08,320,19");

    parser.feed(s1);
    parser.feed(s2);
    parser.feed(s3);
    parser.feed(s4);

    std::printf("sentences_decoded=%llu  sentences_discarded=%llu\n",
        (unsigned long long)parser.sentences_decoded(),
        (unsigned long long)parser.sentences_discarded());
}

// ─── Demo 2: parser + database layer ─────────────────────────────────────────

static void demo_database_layer()
{
    std::printf("\n=== Demo 2: Parser + Database (epoch snapshots) ===\n");

    // Create database
    auto db = std::make_shared<nmea_database>();

    // HP commit: triggered by GGA or RMC
    db->set_commit_callback(commit_kind::high_priority, [](const nmea_snapshot& snap) {
        double lat = 0.0, lon = 0.0, alt = 0.0, hdop = 0.0;
        int fix = 0, nsats = 0;
        snap.get(NMEA_GGA_LATITUDE,       lat);
        snap.get(NMEA_GGA_LONGITUDE,      lon);
        snap.get(NMEA_GGA_ALTITUDE_MSL,   alt);
        snap.get(NMEA_GGA_HDOP,           hdop);
        snap.get(NMEA_GGA_FIX_QUALITY,    fix);
        snap.get(NMEA_GGA_NUM_SATELLITES, nsats);
        std::printf("[HP-SNAPSHOT] lat=%.6f  lon=%.6f  alt=%.1fm  hdop=%.2f  fix=%d  sats=%d\n",
            lat, lon, alt, hdop, fix, nsats);
    });

    // LP commit: triggered by last GSV in a set
    db->set_commit_callback(commit_kind::low_priority, [](const nmea_snapshot& snap) {
        int nsats_in_view = 0;
        snap.get(NMEA_GSV_NUM_SATS_IN_VIEW, nsats_in_view);
        std::printf("[LP-SNAPSHOT] sats_in_view=%d\n", nsats_in_view);
    });

    // Build registry and wire adapter
    nmea_sentence_registry reg;
    nmea_database_adapter adapter(db);

    adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_gga_handler()));
    adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_rmc_handler()));
    adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_gsa_handler()));
    adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_gsv_handler()));
    adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_vtg_handler()));

    adapter.register_with_parser(reg);

    nmea_parser parser(std::move(reg));
    parser.set_error_callback([](const parse_error_info& e) {
        std::printf("ERROR: %s\n", e.description.c_str());
    });

    // Simulate a realistic burst: GGA triggers HP, then GSV set triggers LP
    parser.feed(make("GNGGA,092725.00,4717.11399,N,00833.91590,E,4,12,0.5,499.6,M,48.0,M,,"));
    parser.feed(make("GNRMC,092725.00,A,4717.11399,N,00833.91590,E,0.004,77.52,160223,,,A"));
    parser.feed(make("GPGSV,2,1,08,21,40,083,46,05,35,141,47,25,42,057,50,46,38,103,34"));
    parser.feed(make("GPGSV,2,2,08,12,10,020,35,30,55,200,42,11,25,342,28,07,05,089,22"));
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    demo_parser_only();
    demo_database_layer();
    std::printf("\nDone.\n");
    return 0;
}
