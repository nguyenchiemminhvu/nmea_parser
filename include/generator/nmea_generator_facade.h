// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// include/generator/nmea_generator_facade.h
//
// High-level entry point for NMEA sentence generation.
//
// Responsibilities:
//   1. Accept configuration (talker ID, per-sentence enable/disable).
//   2. Provide typed generate_xxx() methods for callers that construct DTOs
//      explicitly.
//   3. Provide high-level generate() overloads that accept gnss:: shared-buffer
//      structs directly and perform the mapping internally.
//   4. Delegate to per-sentence concrete builders.
//
// Thread-safety:
//   nmea_generator_facade is NOT thread-safe.  Callers that invoke generate()
//   from multiple threads must synchronise externally.
//   (Typical use: called from the HP-commit callback, which already runs in the
//    parser worker thread; no additional lock is required.)
//
// Typical usage:
//
//   #include "generator/nmea_generator_facade.h"
//   using namespace nmea::generator;
//
//   nmea_generator_config cfg;
//   cfg.talker_id   = "GN";
//   cfg.enable_gsv  = true;
//
//   nmea_generator_facade gen(cfg);
//
//   // High-level API — maps from gnss:: shared-buffer structs automatically:
//   auto sentences = gen.generate(location, satellites, timing);
//   for (const auto& s : sentences)
//       uart_write(s.data(), s.size());
//
//   // Low-level API — fill DTOs manually:
//   gga_input gga;
//   gga.hour = 12; gga.minute = 0; gga.second = 0;
//   gga.latitude_deg = 48.8566; gga.longitude_deg = 2.3522;
//   gga.fix_quality  = 1; gga.num_satellites = 8;
//   gga.altitude_msl_m = 35.0; gga.geoid_sep_m = 48.0;
//   gga.valid = true;
//   std::string sentence = gen.generate_gga(gga);

#pragma once

#include "generator/nmea_generator_config.h"
#include "generator/nmea_sentence_inputs.h"

#include <memory>
#include <string>
#include <vector>

// Forward-declare gnss:: shared-buffer types to avoid pulling in their headers
// for callers that only use the low-level DTO API.
namespace gnss
{
    struct location_data;
    struct satellites_data;
    struct timing_data;
    struct dop_data;
}

namespace nmea
{
namespace generator
{

// ─── nmea_generator_facade ───────────────────────────────────────────────────

class nmea_generator_facade
{
public:
    nmea_generator_facade();
    explicit nmea_generator_facade(const nmea_generator_config& config);
    ~nmea_generator_facade();

    // Non-copyable — owns pImpl.
    nmea_generator_facade(const nmea_generator_facade&)            = delete;
    nmea_generator_facade& operator=(const nmea_generator_facade&) = delete;

    // ── Configuration ─────────────────────────────────────────────────────────

    /// Replace the current configuration.
    /// Rebuilds all builder instances and applies enable flags.
    void configure(const nmea_generator_config& config);

    /// Return the active configuration.
    const nmea_generator_config& config() const;

    // ── High-level API — shared-buffer overloads ──────────────────────────────
    //
    // Internally map gnss:: structs to DTOs, then call the low-level builders.
    // @p dop  Optional DOP data.  Leave default-constructed (all zeros) when
    //         UBX-NAV-DOP has not been received yet.
    //
    // Returned sentences are in the canonical NMEA epoch order:
    //   GGA, RMC, GSA, GPGSV … GLGSV …, VTG, GLL, ZDA
    // Disabled sentences are omitted from the output vector.

    std::vector<std::string> generate(const gnss::location_data& loc,
                                      const gnss::satellites_data& sats,
                                      const gnss::dop_data& dop);

    std::vector<std::string> generate(const gnss::location_data& loc,
                                      const gnss::satellites_data& sats,
                                      const gnss::timing_data& tim,
                                      const gnss::dop_data& dop);

    std::vector<std::string> generate(const gnss::location_data& loc,
                                      const gnss::satellites_data& sats);

    // ── Low-level API — explicit DTO overloads ────────────────────────────────
    //
    // Returns an empty string if the corresponding sentence is disabled or
    // if input.valid == false.

    std::string generate_gga(const gga_input& input) const;
    std::string generate_rmc(const rmc_input& input) const;
    std::string generate_gsa(const gsa_input& input) const;
    std::string generate_vtg(const vtg_input& input) const;
    std::string generate_gll(const gll_input& input) const;
    std::string generate_zda(const zda_input& input) const;

    /// Generate the full GSV sentence set for one constellation.
    /// Returns zero or more sentences (one per 4 satellites).
    std::vector<std::string> generate_gsv(const gsv_input& input) const;

    /// Generate GSV sentences for all constellations from satellites_data.
    std::vector<std::string> generate_gsv_all(const gnss::satellites_data& sats) const;

private:
    // pImpl: hides all builder/service includes from this public header.
    // The impl struct is defined in nmea_generator_facade.cpp.
    struct impl;
    std::unique_ptr<impl> impl_;
};

} // namespace generator
} // namespace nmea
