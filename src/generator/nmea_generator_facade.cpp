// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// src/generator/nmea_generator_facade.cpp
//
// pImpl implementation: all concrete types live here, away from the public header.
// Service lifetime: impl owns the formatter + checksum, builders hold const-refs.
// Builder lifetime: unique_ptrs inside impl — destroyed before impl itself.

#include "generator/nmea_generator_facade.h"

#include "nmea_checksum_impl.h"
#include "nmea_formatter.h"
#include "shared_buffer_mapper.h"

#include "builders/gga_builder.h"
#include "builders/rmc_builder.h"
#include "builders/gsa_builder.h"
#include "builders/gsv_builder.h"
#include "builders/vtg_builder.h"
#include "builders/gll_builder.h"
#include "builders/zda_builder.h"

#include "shared_buffer/location_data_buffer.h"
#include "shared_buffer/satellites_data_buffer.h"
#include "shared_buffer/timing_data_buffer.h"
#include "shared_buffer/dop_data_buffer.h"

namespace nmea
{
namespace generator
{

// ─── nmea_generator_facade::impl ─────────────────────────────────────────────

struct nmea_generator_facade::impl
{
    nmea_generator_config config;

    // Services declared BEFORE builders — destroyed AFTER builders.
    nmea_xor_checksum checksum;
    nmea_formatter    formatter;

    // Builders declared AFTER services — destroyed BEFORE services.
    std::unique_ptr<gga_builder> gga;
    std::unique_ptr<rmc_builder> rmc;
    std::unique_ptr<gsa_builder> gsa;
    std::unique_ptr<gsv_builder> gsv;
    std::unique_ptr<vtg_builder> vtg;
    std::unique_ptr<gll_builder> gll;
    std::unique_ptr<zda_builder> zda;

    explicit impl(const nmea_generator_config& cfg)
        : config(cfg)
        , formatter(cfg.latlon_decimal_digits, cfg.time_decimal_digits)
    {
        rebuild_builders();
    }

    void rebuild_builders()
    {
        gga.reset(); rmc.reset(); gsa.reset(); gsv.reset();
        vtg.reset(); gll.reset(); zda.reset();

        formatter = nmea_formatter(config.latlon_decimal_digits,
                                   config.time_decimal_digits);

        const std::string& t = config.talker_id;

        gga.reset(new gga_builder(t, formatter, checksum, config.enable_gga));
        rmc.reset(new rmc_builder(t, formatter, checksum, config.enable_rmc));
        gsa.reset(new gsa_builder(t, formatter, checksum, config.enable_gsa));
        gsv.reset(new gsv_builder(t, formatter, checksum, config.enable_gsv));
        vtg.reset(new vtg_builder(t, formatter, checksum, config.enable_vtg));
        gll.reset(new gll_builder(t, formatter, checksum, config.enable_gll));
        zda.reset(new zda_builder(t, formatter, checksum, config.enable_zda));
    }
};

// ─── nmea_generator_facade ───────────────────────────────────────────────────

nmea_generator_facade::nmea_generator_facade()
    : impl_(new impl(nmea_generator_config{}))
{}

nmea_generator_facade::nmea_generator_facade(const nmea_generator_config& config)
    : impl_(new impl(config))
{}

nmea_generator_facade::~nmea_generator_facade() = default;

void nmea_generator_facade::configure(const nmea_generator_config& config)
{
    impl_->config = config;
    impl_->rebuild_builders();
}

const nmea_generator_config& nmea_generator_facade::config() const
{
    return impl_->config;
}

std::vector<std::string> nmea_generator_facade::generate(
    const gnss::location_data& loc,
    const gnss::satellites_data& sats,
    const gnss::dop_data& dop)
{
    std::vector<std::string> result;
    result.reserve(16u);

    auto push = [&result](const std::string& s)
    {
        if (!s.empty()) result.push_back(s);
    };

    push(generate_gga(shared_buffer_mapper::to_gga(loc, dop)));
    push(generate_rmc(shared_buffer_mapper::to_rmc(loc)));
    push(generate_gsa(shared_buffer_mapper::to_gsa(loc, sats, dop)));

    if (impl_->config.enable_gsv)
    {
        auto gsv_inputs = shared_buffer_mapper::to_gsv_all(sats);
        for (const auto& gi : gsv_inputs)
        {
            bool enabled = true;
            const std::string& tk = gi.talker;
            if      (tk == "GP") enabled = impl_->config.enable_gsv_gps;
            else if (tk == "GL") enabled = impl_->config.enable_gsv_glonass;
            else if (tk == "GA") enabled = impl_->config.enable_gsv_galileo;
            else if (tk == "GB") enabled = impl_->config.enable_gsv_beidou;
            else if (tk == "GQ") enabled = impl_->config.enable_gsv_qzss;

            if (enabled)
            {
                auto sentences = generate_gsv(gi);
                for (auto& s : sentences)
                    push(s);
            }
        }
    }

    push(generate_vtg(shared_buffer_mapper::to_vtg(loc)));
    push(generate_gll(shared_buffer_mapper::to_gll(loc)));
    push(generate_zda(shared_buffer_mapper::to_zda(loc)));

    return result;
}

std::vector<std::string> nmea_generator_facade::generate(
    const gnss::location_data& loc,
    const gnss::satellites_data& sats,
    const gnss::timing_data& tim,
    const gnss::dop_data& dop)
{
    std::vector<std::string> result = generate(loc, sats, dop);

    if (impl_->config.enable_zda)
    {
        std::string zda_s = generate_zda(shared_buffer_mapper::to_zda(tim));
        if (!zda_s.empty())
        {
            for (auto& s : result)
            {
                if (s.find("ZDA,") != std::string::npos)
                {
                    s = zda_s;
                    return result;
                }
            }
            result.push_back(zda_s);
        }
    }

    return result;
}

std::vector<std::string> nmea_generator_facade::generate(
    const gnss::location_data& loc,
    const gnss::satellites_data& sats)
{
    return generate(loc, sats, gnss::dop_data{});
}

std::string nmea_generator_facade::generate_gga(const gga_input& input) const
{
    if (!impl_->gga) return std::string();
    impl_->gga->prepare(input);
    return impl_->gga->build();
}

std::string nmea_generator_facade::generate_rmc(const rmc_input& input) const
{
    if (!impl_->rmc) return std::string();
    impl_->rmc->prepare(input);
    return impl_->rmc->build();
}

std::string nmea_generator_facade::generate_gsa(const gsa_input& input) const
{
    if (!impl_->gsa) return std::string();
    impl_->gsa->prepare(input);
    return impl_->gsa->build();
}

std::string nmea_generator_facade::generate_vtg(const vtg_input& input) const
{
    if (!impl_->vtg) return std::string();
    impl_->vtg->prepare(input);
    return impl_->vtg->build();
}

std::string nmea_generator_facade::generate_gll(const gll_input& input) const
{
    if (!impl_->gll) return std::string();
    impl_->gll->prepare(input);
    return impl_->gll->build();
}

std::string nmea_generator_facade::generate_zda(const zda_input& input) const
{
    if (!impl_->zda) return std::string();
    impl_->zda->prepare(input);
    return impl_->zda->build();
}

std::vector<std::string> nmea_generator_facade::generate_gsv(
    const gsv_input& input) const
{
    if (!impl_->gsv) return {};
    impl_->gsv->prepare(input);
    return impl_->gsv->build_all();
}

std::vector<std::string> nmea_generator_facade::generate_gsv_all(
    const gnss::satellites_data& sats) const
{
    std::vector<std::string> result;
    auto gsv_inputs = shared_buffer_mapper::to_gsv_all(sats);
    for (const auto& gi : gsv_inputs)
    {
        auto sentences = generate_gsv(gi);
        for (auto& s : sentences)
            result.push_back(s);
    }
    return result;
}

} // namespace generator
} // namespace nmea
