// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "database/handlers/db_rmc_handler.h"
#include "database/nmea_field_store.h"

namespace nmea { namespace database {

void db_rmc_handler::handle(nmea_field_store& store, uint64_t& mask)
{
    if (!msg_.valid) return;

    store.set(NMEA_RMC_UTC_TIME,       msg_.utc_time);
    store.set(NMEA_RMC_STATUS_ACTIVE,  msg_.status_active ? 1.0 : 0.0);
    store.set(NMEA_RMC_LATITUDE,       msg_.latitude);
    store.set(NMEA_RMC_LONGITUDE,      msg_.longitude);
    store.set(NMEA_RMC_SPEED_KNOTS,    msg_.speed_knots);
    store.set(NMEA_RMC_COURSE_TRUE,    msg_.course_true);
    store.set(NMEA_RMC_DATE,           static_cast<double>(msg_.date));
    store.set(NMEA_RMC_MAG_VARIATION,  msg_.mag_variation);

    mask |= (1ULL << static_cast<uint64_t>(MSG_NMEA_RMC));
}

} }
