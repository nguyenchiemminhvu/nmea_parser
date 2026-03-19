// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "database/nmea_snapshot.h"

namespace nmea
{
namespace database
{

nmea_snapshot::nmea_snapshot(const nmea_field_store& store,
                             uint64_t               msg_mask,
                             commit_kind            kind)
    : store_(store)
    , msg_mask_(msg_mask)
    , kind_(kind)
{}

} // namespace database
} // namespace nmea
