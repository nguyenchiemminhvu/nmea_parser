// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/nmea_database_adapter.cpp
//
// Creates sentence-parser stubs for each registered handler and installs them
// into the sentence registry.
//
// For GGA:
//   A gga_parser stub is registered whose callback calls:
//     gga_handler->update(msg);
//     db->apply_update(*gga_handler);
//
// The same pattern is used for each supported message type.

#include "database/nmea_database_adapter.h"
#include "database/handlers/db_gga_handler.h"
#include "database/handlers/db_rmc_handler.h"
#include "database/handlers/db_gsa_handler.h"
#include "database/handlers/db_gsv_handler.h"
#include "database/handlers/db_vtg_handler.h"

#include "parsers/gga_parser.h"
#include "parsers/rmc_parser.h"
#include "parsers/gsa_parser.h"
#include "parsers/gsv_parser.h"
#include "parsers/vtg_parser.h"

#include <utility>

namespace nmea
{
namespace database
{

nmea_database_adapter::nmea_database_adapter(std::shared_ptr<nmea_database> db)
    : db_(std::move(db))
{}

void nmea_database_adapter::add_handler(std::unique_ptr<i_sentence_handler> handler)
{
    handlers_.push_back(std::move(handler));
}

void nmea_database_adapter::register_with_parser(parser::nmea_sentence_registry& registry)
{
    for (auto& h : handlers_)
    {
        i_sentence_handler* raw_h = h.get();
        auto db = db_;

        switch (h->handled_msg())
        {
        case MSG_NMEA_GGA:
        {
            auto* gga_h = static_cast<db_gga_handler*>(raw_h);
            registry.register_parser(std::unique_ptr<parser::i_sentence_parser>(
                new parser::gga_parser([gga_h, db](const parser::nmea_gga& msg) {
                    gga_h->update(msg);
                    db->apply_update(*gga_h);
                })));
            break;
        }
        case MSG_NMEA_RMC:
        {
            auto* rmc_h = static_cast<db_rmc_handler*>(raw_h);
            registry.register_parser(std::unique_ptr<parser::i_sentence_parser>(
                new parser::rmc_parser([rmc_h, db](const parser::nmea_rmc& msg) {
                    rmc_h->update(msg);
                    db->apply_update(*rmc_h);
                })));
            break;
        }
        case MSG_NMEA_GSA:
        {
            auto* gsa_h = static_cast<db_gsa_handler*>(raw_h);
            registry.register_parser(std::unique_ptr<parser::i_sentence_parser>(
                new parser::gsa_parser([gsa_h, db](const parser::nmea_gsa& msg) {
                    gsa_h->update(msg);
                    db->apply_update(*gsa_h);
                })));
            break;
        }
        case MSG_NMEA_GSV:
        {
            auto* gsv_h = static_cast<db_gsv_handler*>(raw_h);
            registry.register_parser(std::unique_ptr<parser::i_sentence_parser>(
                new parser::gsv_parser([gsv_h, db](const parser::nmea_gsv& msg) {
                    gsv_h->update(msg);
                    // Only apply_update (and potentially commit) after the
                    // last part of the GSV set arrives.
                    if (gsv_h->is_set_complete())
                        db->apply_update(*gsv_h);
                })));
            break;
        }
        case MSG_NMEA_VTG:
        {
            auto* vtg_h = static_cast<db_vtg_handler*>(raw_h);
            registry.register_parser(std::unique_ptr<parser::i_sentence_parser>(
                new parser::vtg_parser([vtg_h, db](const parser::nmea_vtg& msg) {
                    vtg_h->update(msg);
                    db->apply_update(*vtg_h);
                })));
            break;
        }
        default:
            break;
        }
    }
}

} // namespace database
} // namespace nmea
