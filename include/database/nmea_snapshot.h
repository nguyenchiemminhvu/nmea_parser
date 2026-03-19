// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/nmea_snapshot.h
//
// Thread-safe, immutable snapshot of the field store.
// Mirrors ubx_parser's ubx_location_snapshot.

#pragma once

#include "database/nmea_field_store.h"
#include "database/i_update_policy.h"

namespace nmea
{
namespace database
{

class nmea_snapshot
{
public:
    explicit nmea_snapshot(const nmea_field_store& store,
                           uint64_t               msg_mask,
                           commit_kind            kind);

    nmea_snapshot()                               = default;
    nmea_snapshot(const nmea_snapshot&)           = default;
    nmea_snapshot& operator=(const nmea_snapshot&)= default;

    // ── Accessors ─────────────────────────────────────────────────────────────

    commit_kind kind()     const { return kind_;     }
    uint64_t    msg_mask() const { return msg_mask_; }

    template <typename T>
    bool get(data_field field, T& out) const { return store_.get(field, out); }

    bool is_valid(data_field field) const { return store_.is_valid(field); }

private:
    nmea_field_store store_;
    uint64_t         msg_mask_ = 0u;
    commit_kind      kind_     = commit_kind::none;
};

} // namespace database
} // namespace nmea
