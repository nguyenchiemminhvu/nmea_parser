// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "database/handlers/db_vtg_handler.h"
#include "database/nmea_field_store.h"

namespace nmea { namespace database {

void db_vtg_handler::handle(nmea_field_store& store, uint64_t& mask)
{
    if (!msg_.valid) return;

    store.set(NMEA_VTG_COURSE_TRUE,  msg_.course_true);
    store.set(NMEA_VTG_SPEED_KNOTS,  msg_.speed_knots);
    store.set(NMEA_VTG_SPEED_KMPH,   msg_.speed_kmh);
    if (msg_.course_mag_valid)
        store.set(NMEA_VTG_COURSE_MAG, msg_.course_mag);

    mask |= (1ULL << static_cast<uint64_t>(MSG_NMEA_VTG));
}

} }
