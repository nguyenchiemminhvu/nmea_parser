// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// examples/basic_nmea_generating.cpp
//
// End-to-end demo for the nmea_generator feature.
//
// Three demonstrations:
//   1. Low-level API  — fill DTOs manually, generate individual sentences.
//   2. High-level API — populate gnss:: shared-buffer structs, generate a
//                        full epoch sentence set in one call.
//   3. Configuration  — customise talker ID, enable/disable per-sentence
//                        flags, and change output precision.

#include "generator/nmea_generator_facade.h"
#include "generator/nmea_generator_config.h"
#include "generator/nmea_sentence_inputs.h"

// shared-buffer types required by the high-level API
#include "shared_buffer/location_data_buffer.h"
#include "shared_buffer/satellites_data_buffer.h"
#include "shared_buffer/timing_data_buffer.h"
#include "shared_buffer/dop_data_buffer.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace nmea::generator;

// ─── helpers ─────────────────────────────────────────────────────────────────

static void print_sentences(const char* label, const std::vector<std::string>& sentences)
{
    std::printf("\n-- %s (%zu sentence(s)) --\n", label, sentences.size());
    for (const auto& s : sentences)
        std::printf("  %s\n", s.c_str());
}

static void print_sentence(const char* label, const std::string& sentence)
{
    if (sentence.empty())
        std::printf("  %s: <disabled or invalid>\n", label);
    else
        std::printf("  %s: %s\n", label, sentence.c_str());
}

// ─── demo 1: low-level DTO API ────────────────────────────────────────────────

static void demo_low_level_api()
{
    std::printf("\n=== Demo 1: Low-level DTO API ===\n");

    nmea_generator_facade gen;   // default config: talker="GN", all sentences enabled

    // ── GGA ──────────────────────────────────────────────────────────────────
    gga_input gga;
    gga.hour              = 10u;
    gga.minute            = 30u;
    gga.second            = 15u;
    gga.millisecond       = 500u;
    gga.latitude_deg      = 48.856600;   // Paris
    gga.longitude_deg     = 2.352200;
    gga.fix_quality       = 1u;          // autonomous GPS
    gga.num_satellites    = 9u;
    gga.hdop              = 1.2;
    gga.altitude_msl_m    = 35.0;
    gga.geoid_sep_m       = 48.0;
    gga.dgps_age_s        = -1.0;        // not available
    gga.valid             = true;

    print_sentence("GGA", gen.generate_gga(gga));

    // ── RMC ──────────────────────────────────────────────────────────────────
    rmc_input rmc;
    rmc.hour           = 10u;
    rmc.minute         = 30u;
    rmc.second         = 15u;
    rmc.millisecond    = 500u;
    rmc.status_active  = true;
    rmc.latitude_deg   = 48.856600;
    rmc.longitude_deg  = 2.352200;
    rmc.speed_knots    = 0.5;
    rmc.course_true    = 90.0;
    rmc.day            = 14u;
    rmc.month          = 4u;
    rmc.year           = 2026u;
    rmc.mag_variation  = 2.5;
    rmc.mag_var_valid  = true;
    rmc.mode           = 'A';
    rmc.valid          = true;

    print_sentence("RMC", gen.generate_rmc(rmc));

    // ── GSA ──────────────────────────────────────────────────────────────────
    gsa_input gsa;
    gsa.op_mode   = 'A';
    gsa.nav_mode  = 3u;    // 3D fix
    const uint8_t used_sv_ids[] = { 2u, 5u, 9u, 12u, 15u, 21u, 25u, 29u };
    for (int i = 0; i < 8; ++i)
        gsa.sat_ids[i] = used_sv_ids[i];
    gsa.pdop      = 2.1;
    gsa.hdop      = 1.2;
    gsa.vdop      = 1.7;
    gsa.system_id = 0u;    // omit NMEA 4.11 field (GNGSA covers all constellations)
    gsa.valid     = true;

    print_sentence("GSA", gen.generate_gsa(gsa));

    // ── GSV (GPS, 6 satellites → 2 sentences) ────────────────────────────────
    gsv_input gsv;
    gsv.talker = "GP";
    gsv.satellites = {
        {  2u,  45,  75u, 38u },
        {  5u,  62, 120u, 42u },
        {  9u,  30, 200u, 35u },
        { 12u,  55,  30u, 40u },
        { 15u,  20, 310u, 28u },
        { 21u,  10, 260u, 22u }
    };
    gsv.valid = true;

    print_sentences("GSV (GPS)", gen.generate_gsv(gsv));

    // ── VTG ──────────────────────────────────────────────────────────────────
    vtg_input vtg;
    vtg.course_true      = 90.0;
    vtg.course_mag       = 92.5;
    vtg.course_mag_valid = true;
    vtg.speed_knots      = 0.5;
    vtg.speed_kmh        = 0.926;
    vtg.mode             = 'A';
    vtg.valid            = true;

    print_sentence("VTG", gen.generate_vtg(vtg));

    // ── GLL ──────────────────────────────────────────────────────────────────
    gll_input gll;
    gll.latitude_deg  = 48.856600;
    gll.longitude_deg = 2.352200;
    gll.hour          = 10u;
    gll.minute        = 30u;
    gll.second        = 15u;
    gll.millisecond   = 500u;
    gll.status_active = true;
    gll.mode          = 'A';
    gll.valid         = true;

    print_sentence("GLL", gen.generate_gll(gll));

    // ── ZDA ──────────────────────────────────────────────────────────────────
    zda_input zda;
    zda.hour        = 10u;
    zda.minute      = 30u;
    zda.second      = 15u;
    zda.millisecond = 500u;
    zda.day         = 14u;
    zda.month       = 4u;
    zda.year        = 2026u;
    zda.tz_hour     = 0;
    zda.tz_min      = 0u;
    zda.valid       = true;

    print_sentence("ZDA", gen.generate_zda(zda));
}

// ─── demo 2: high-level shared-buffer API ─────────────────────────────────────

static gnss::location_data make_location()
{
    gnss::location_data loc;
    std::memset(&loc, 0, sizeof(loc));

    loc.latitude_deg            = 48.856600;
    loc.longitude_deg           = 2.352200;
    loc.altitude_msl_mm         = 35000.0;      // 35.0 m
    loc.altitude_ellipsoid_mm   = 83000.0;      // 83.0 m → geoid sep = 48 m
    loc.ground_speed_mmps       = 257.22f;      // ≈ 0.5 knots
    loc.motion_heading_deg      = 90.0f;
    loc.magnetic_declination_deg = 2.5f;
    loc.fix_type                = 3u;           // 3D fix
    loc.num_sv                  = 9u;
    loc.flags                   = 0x01u;        // gnssFixOK
    loc.flags2                  = 0x00u;        // no RTK
    loc.year                    = 2026u;
    loc.month                   = 4u;
    loc.day                     = 14u;
    loc.hour                    = 10u;
    loc.minute                  = 30u;
    loc.second                  = 15u;
    loc.millisecond             = 500u;
    loc.valid                   = true;
    return loc;
}

static gnss::satellites_data make_satellites()
{
    gnss::satellites_data sats;
    std::memset(&sats, 0, sizeof(sats));

    // GPS satellites (gnss_id = 0)
    const struct { uint8_t gnss_id; uint8_t sv_id; uint8_t cno; int8_t el; int16_t az; bool used; } sv_table[] = {
        { 0u,  2u, 38u,  45,  75, true  },
        { 0u,  5u, 42u,  62, 120, true  },
        { 0u,  9u, 35u,  30, 200, true  },
        { 0u, 12u, 40u,  55,  30, true  },
        { 0u, 15u, 28u,  20, 310, false },
        { 0u, 21u, 22u,  10, 260, false },
        // GLONASS satellites (gnss_id = 6)
        { 6u, 65u, 36u,  50,  90, true  },
        { 6u, 66u, 31u,  35, 155, true  },
        { 6u, 67u, 25u,  15, 240, false },
        // Galileo satellites (gnss_id = 2)
        { 2u,  1u, 39u,  70,  45, true  },
        { 2u,  2u, 33u,  40, 300, true  },
    };
    const int sv_count = static_cast<int>(sizeof(sv_table) / sizeof(sv_table[0]));

    sats.num_svs = static_cast<uint8_t>(sv_count);
    for (int i = 0; i < sv_count; ++i)
    {
        sats.svs[i].gnss_id       = sv_table[i].gnss_id;
        sats.svs[i].sv_id         = sv_table[i].sv_id;
        sats.svs[i].cno           = sv_table[i].cno;
        sats.svs[i].elevation_deg = sv_table[i].el;
        sats.svs[i].azimuth_deg   = sv_table[i].az;
        sats.svs[i].used          = sv_table[i].used;
    }
    sats.valid = true;
    return sats;
}

static gnss::dop_data make_dop()
{
    gnss::dop_data dop;
    std::memset(&dop, 0, sizeof(dop));
    dop.pdop  = 2.1f;
    dop.hdop  = 1.2f;
    dop.vdop  = 1.7f;
    dop.valid = true;
    return dop;
}

static gnss::timing_data make_timing()
{
    gnss::timing_data tim;
    std::memset(&tim, 0, sizeof(tim));
    tim.utc_year   = 2026u;
    tim.utc_month  = 4u;
    tim.utc_day    = 14u;
    tim.utc_hour   = 10u;
    tim.utc_min    = 30u;
    tim.utc_sec    = 15u;
    tim.utc_nano_ns = 500000000;    // 500 ms
    tim.valid      = true;
    return tim;
}

static void demo_high_level_api()
{
    std::printf("\n=== Demo 2: High-level shared-buffer API ===\n");

    nmea_generator_config cfg;
    cfg.talker_id             = "GN";
    cfg.enable_gsv_gps        = true;
    cfg.enable_gsv_glonass    = true;
    cfg.enable_gsv_galileo    = true;
    cfg.enable_gsv_beidou     = false;
    cfg.enable_gsv_qzss       = false;
    cfg.latlon_decimal_digits = 5;
    cfg.time_decimal_digits   = 2;

    nmea_generator_facade gen(cfg);

    const gnss::location_data   loc  = make_location();
    const gnss::satellites_data sats = make_satellites();
    const gnss::dop_data        dop  = make_dop();
    const gnss::timing_data     tim  = make_timing();

    // Generate full epoch without timing_data (ZDA derives from location_data)
    std::vector<std::string> epoch = gen.generate(loc, sats, dop);
    print_sentences("Full epoch (loc + sats + dop)", epoch);

    // Generate full epoch with high-accuracy timing_data (overrides ZDA)
    std::vector<std::string> epoch_with_tim = gen.generate(loc, sats, tim, dop);
    print_sentences("Full epoch (loc + sats + tim + dop)", epoch_with_tim);
}

// ─── demo 3: configuration ────────────────────────────────────────────────────

static void demo_configuration()
{
    std::printf("\n=== Demo 3: Configuration and runtime reconfigure ===\n");

    // Start with GPS-only, minimal sentence set
    nmea_generator_config cfg;
    cfg.talker_id  = "GP";
    cfg.enable_gga = true;
    cfg.enable_rmc = true;
    cfg.enable_gsa = false;
    cfg.enable_gsv = false;
    cfg.enable_vtg = false;
    cfg.enable_gll = false;
    cfg.enable_zda = false;

    nmea_generator_facade gen(cfg);

    const gnss::location_data   loc  = make_location();
    const gnss::satellites_data sats = make_satellites();
    const gnss::dop_data        dop  = make_dop();

    std::vector<std::string> minimal = gen.generate(loc, sats, dop);
    print_sentences("Minimal GPS-only epoch (GGA + RMC only)", minimal);

    // Reconfigure at runtime: switch to combined GNSS, enable all sentences
    nmea_generator_config full_cfg;
    full_cfg.talker_id             = "GN";
    full_cfg.latlon_decimal_digits = 4;   // standard 4-digit precision
    full_cfg.time_decimal_digits   = 1;

    gen.configure(full_cfg);

    std::vector<std::string> full = gen.generate(loc, sats, dop);
    print_sentences("Full GNGXX epoch after reconfigure", full);

    // Low-level DTO with the reconfigured generator
    std::printf("\n  Low-level GGA after reconfigure (4-digit lat/lon, 1-digit time):\n");
    gga_input gga;
    gga.hour           = 10u;
    gga.minute         = 30u;
    gga.second         = 15u;
    gga.millisecond    = 500u;
    gga.latitude_deg   = 48.856600;
    gga.longitude_deg  = 2.352200;
    gga.fix_quality    = 1u;
    gga.num_satellites = 9u;
    gga.hdop           = 1.2;
    gga.altitude_msl_m = 35.0;
    gga.geoid_sep_m    = 48.0;
    gga.dgps_age_s     = -1.0;
    gga.valid          = true;
    print_sentence("  GGA", gen.generate_gga(gga));
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    demo_low_level_api();
    demo_high_level_api();
    demo_configuration();

    std::printf("\nDone.\n");
    return 0;
}
