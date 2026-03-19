// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "database/nmea_database.h"
#include <utility>

namespace nmea
{
namespace database
{

nmea_database::nmea_database(std::unique_ptr<i_update_policy> policy)
{
    if (policy)
        policy_ = std::move(policy);
    else
        policy_.reset(new default_update_policy());
}

void nmea_database::set_update_policy(std::unique_ptr<i_update_policy> policy)
{
    std::unique_lock<std::shared_timed_mutex> lk(mutex_);
    if (policy)
        policy_ = std::move(policy);
    else
        policy_.reset(new default_update_policy());
}

void nmea_database::set_commit_callback(commit_kind kind, commit_callback cb)
{
    std::unique_lock<std::shared_timed_mutex> lk(mutex_);
    if (kind == commit_kind::high_priority)
        hp_cb_ = std::move(cb);
    else if (kind == commit_kind::low_priority)
        lp_cb_ = std::move(cb);
}

void nmea_database::apply_update(i_sentence_handler& handler)
{
    std::unique_lock<std::shared_timed_mutex> lk(mutex_);

    // 1. Write fields into the store and update the mask
    handler.handle(store_, msg_mask_);

    // 2. Consult the policy
    const commit_kind ck = policy_->should_commit(handler.handled_msg());
    if (ck == commit_kind::none)
        return;

    // 3. Snapshot the store at commit time
    const nmea_snapshot snap(store_, msg_mask_, ck);

    // 4. Reset the mask after each commit (accumulate fresh per-epoch)
    msg_mask_ = 0u;

    // 5. Fire the appropriate callback (still under the write lock — keep fast)
    if (ck == commit_kind::high_priority && hp_cb_)
        hp_cb_(snap);
    else if (ck == commit_kind::low_priority && lp_cb_)
        lp_cb_(snap);
}

nmea_snapshot nmea_database::snapshot() const
{
    std::shared_lock<std::shared_timed_mutex> lk(mutex_);
    return nmea_snapshot(store_, msg_mask_, commit_kind::none);
}

} // namespace database
} // namespace nmea
