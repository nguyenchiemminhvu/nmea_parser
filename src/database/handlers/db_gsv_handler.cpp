// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu

#include "database/handlers/db_gsv_handler.h"
#include "database/nmea_field_store.h"
#include "database/nmea_data_fields.h"
#include "nmea_types.h"

namespace nmea { namespace database {

void db_gsv_handler::update(const parser::nmea_gsv& part)
{
    if (!part.valid) return;

    // Detect start of a new GSV set
    if (part.msg_num == 1u)
    {
        parts_.clear();
        expected_parts_ = part.num_msgs;
        set_complete_   = false;
    }

    parts_.push_back(part);

    // Check if we have all parts of the set
    if (part.msg_num == expected_parts_ && !parts_.empty())
        set_complete_ = true;
}

void db_gsv_handler::handle(nmea_field_store& store, uint64_t& mask)
{
    if (parts_.empty()) return;

    // Count total visible sats from the first part header
    const uint8_t total_in_view = parts_[0u].sats_in_view;
    store.set(NMEA_GSV_NUM_SATS_IN_VIEW, static_cast<int>(total_in_view));

    // Write individual satellite data — parallel arrays, one per attribute
    size_t sat_slot = 0u;
    for (const auto& part : parts_)
    {
        for (uint8_t i = 0u; i < part.sat_count; ++i)
        {
            if (sat_slot >= parser::NMEA_MAX_SATS_IN_VIEW)
                break;

            const auto&   sv  = part.satellites[i];
            const uint8_t idx = static_cast<uint8_t>(sat_slot);

            store.set(nmea_gsv_sat_field(NMEA_GSV_SAT_SVID,      idx), static_cast<int>(sv.sv_id));
            store.set(nmea_gsv_sat_field(NMEA_GSV_SAT_ELEVATION,  idx), static_cast<int>(sv.elevation));
            store.set(nmea_gsv_sat_field(NMEA_GSV_SAT_AZIMUTH,    idx), static_cast<int>(sv.azimuth));
            store.set(nmea_gsv_sat_field(NMEA_GSV_SAT_SNR,        idx), static_cast<int>(sv.snr));

            ++sat_slot;
        }
    }

    mask |= (1ULL << static_cast<uint64_t>(MSG_NMEA_GSV));
    set_complete_ = false;  // Reset for next set
    parts_.clear();
}

} }
