// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/nmea_database.h
//
// Core database: owns the field store, the message mask, and the update policy.
//
// Responsibilities:
//   1. Receive calls from sentence handlers via apply_update().
//   2. Write fields into nmea_field_store under an exclusive write lock.
//   3. Consult the update policy after each message.
//   4. On commit: snapshot the store and invoke the per-kind commit callback.
//
// Thread-safety:
//   • apply_update()  acquires an exclusive write lock.
//   • snapshot()      acquires a shared read lock.
//   • std::shared_timed_mutex (C++14) is used for efficient concurrent reads.
//   • Commit callbacks are invoked synchronously under the write lock.
//     Keep them fast and non-blocking.

#pragma once

#include "database/nmea_field_store.h"
#include "database/nmea_snapshot.h"
#include "database/i_sentence_handler.h"
#include "database/i_update_policy.h"
#include "database/default_update_policy.h"

#include <functional>
#include <memory>
#include <shared_mutex>

namespace nmea
{
namespace database
{

using commit_callback = std::function<void(const nmea_snapshot&)>;

// ─── nmea_database ────────────────────────────────────────────────────────────

class nmea_database
{
public:
    /// Construct with a commit policy.  If nullptr, default_update_policy is used.
    explicit nmea_database(std::unique_ptr<i_update_policy> policy = nullptr);

    ~nmea_database() = default;

    // Non-copyable (owns mutex)
    nmea_database(const nmea_database&)            = delete;
    nmea_database& operator=(const nmea_database&) = delete;

    // ── Configuration ─────────────────────────────────────────────────────────

    void set_update_policy(std::unique_ptr<i_update_policy> policy);

    /// Register a callback for a specific commit kind.
    void set_commit_callback(commit_kind kind, commit_callback cb);

    // ── Write path (parser thread) ────────────────────────────────────────────

    /// Call handler.handle(), consult policy, and optionally commit.
    void apply_update(i_sentence_handler& handler);

    // ── Read path (any thread) ────────────────────────────────────────────────

    /// Atomically snapshot the field store under a shared read lock.
    nmea_snapshot snapshot() const;

private:
    mutable std::shared_timed_mutex   mutex_;
    nmea_field_store                  store_;
    uint64_t                         msg_mask_ = 0u;
    std::unique_ptr<i_update_policy>  policy_;
    commit_callback                   hp_cb_;
    commit_callback                   lp_cb_;
};

} // namespace database
} // namespace nmea
