// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/i_update_policy.h
//
// Abstraction for the commit-trigger decision after each message update.
// Mirrors ubx_parser's i_commit_policy.

#pragma once

#include "database/nmea_data_fields.h"

namespace nmea
{
namespace database
{

// ─── commit_kind ─────────────────────────────────────────────────────────────

enum class commit_kind
{
    high_priority,   ///< Position / navigation fix data (GGA, RMC)
    low_priority,    ///< Supplementary data (GSV satellite table, etc.)
    none,            ///< Update stored but no commit callback fired
};

// ─── i_update_policy ─────────────────────────────────────────────────────────

class i_update_policy
{
public:
    virtual ~i_update_policy() = default;

    i_update_policy()                                  = default;
    i_update_policy(const i_update_policy&)            = delete;
    i_update_policy& operator=(const i_update_policy&) = delete;

    /// Called after a handler has written a message's fields into the store.
    /// Returns the commit kind for this update (may be none).
    virtual commit_kind should_commit(msg_type msg) const = 0;
};

} // namespace database
} // namespace nmea
