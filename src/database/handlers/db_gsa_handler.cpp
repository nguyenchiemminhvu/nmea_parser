// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "database/handlers/db_gsa_handler.h"
#include "database/nmea_field_store.h"

namespace nmea { namespace database {

void db_gsa_handler::handle(nmea_field_store& store, uint64_t& mask)
{
    if (!msg_.valid) return;

    store.set(NMEA_GSA_NAV_MODE,  static_cast<int>(msg_.nav_mode));
    store.set(NMEA_GSA_PDOP,      msg_.pdop);
    store.set(NMEA_GSA_HDOP,      msg_.hdop);
    store.set(NMEA_GSA_VDOP,      msg_.vdop);
    store.set(NMEA_GSA_SYSTEM_ID, static_cast<int>(msg_.system_id));

    mask |= (1ULL << static_cast<uint64_t>(MSG_NMEA_GSA));
}

} }
