// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/default_update_policy.h
//
// Default commit policy:
//   GGA → high_priority  (primary position fix)
//   RMC → high_priority  (primary position fix)
//   GSV → low_priority   (satellite table update)
//   All others → none

#pragma once

#include "database/i_update_policy.h"

namespace nmea
{
namespace database
{

class default_update_policy : public i_update_policy
{
public:
    commit_kind should_commit(msg_type msg) const override
    {
        switch (msg)
        {
            case MSG_NMEA_GGA: return commit_kind::high_priority;
            case MSG_NMEA_GSV: return commit_kind::low_priority;
            default:           return commit_kind::none;
        }
    }
};

} // namespace database
} // namespace nmea
