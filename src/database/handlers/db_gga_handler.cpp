// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "database/handlers/db_gga_handler.h"
#include "database/nmea_field_store.h"

namespace nmea { namespace database {

void db_gga_handler::handle(nmea_field_store& store, uint64_t& mask)
{
    if (!msg_.valid) return;

    store.set(NMEA_GGA_UTC_TIME,       msg_.utc_time);
    store.set(NMEA_GGA_LATITUDE,       msg_.latitude);
    store.set(NMEA_GGA_LONGITUDE,      msg_.longitude);
    store.set(NMEA_GGA_FIX_QUALITY,    static_cast<int>(msg_.fix_quality));
    store.set(NMEA_GGA_NUM_SATELLITES, msg_.num_satellites);
    store.set(NMEA_GGA_HDOP,           msg_.hdop);
    store.set(NMEA_GGA_ALTITUDE_MSL,   msg_.altitude_msl);
    store.set(NMEA_GGA_GEOID_SEP,      msg_.geoid_sep);
    if (msg_.dgps_age >= 0.0)
        store.set(NMEA_GGA_DGPS_AGE,   msg_.dgps_age);

    mask |= (1ULL << static_cast<uint64_t>(MSG_NMEA_GGA));
}

} }
