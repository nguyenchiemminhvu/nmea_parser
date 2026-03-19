// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/nmea_database_adapter.h
//
// Wires nmea_sentence_registry callbacks → handlers → nmea_database.
// Mirrors ubx_parser's ubx_database_adapter.

#pragma once

#include "database/nmea_database.h"
#include "database/i_sentence_handler.h"
#include "nmea_sentence_registry.h"

#include <memory>
#include <vector>

namespace nmea
{
namespace database
{

class nmea_database_adapter
{
public:
    explicit nmea_database_adapter(std::shared_ptr<nmea_database> db);
    ~nmea_database_adapter() = default;

    // Non-copyable
    nmea_database_adapter(const nmea_database_adapter&)            = delete;
    nmea_database_adapter& operator=(const nmea_database_adapter&) = delete;

    // ── Handler registration ──────────────────────────────────────────────────

    /// Register a sentence handler.  Call before register_with_parser().
    void add_handler(std::unique_ptr<i_sentence_handler> handler);

    // ── Parser wiring ─────────────────────────────────────────────────────────

    /// For every registered handler, create a sentence parser stub in
    /// @p registry whose callback drives handler + db.
    /// Call this once, after all handlers have been added.
    void register_with_parser(parser::nmea_sentence_registry& registry);

private:
    std::shared_ptr<nmea_database>                   db_;
    std::vector<std::unique_ptr<i_sentence_handler>> handlers_;
};

} // namespace database
} // namespace nmea
