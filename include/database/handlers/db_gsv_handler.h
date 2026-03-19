// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/handlers/db_gsv_handler.h
//
// Accumulates multi-part GSV sentences.  Fires only when the last part
// of a complete GSV set has been received (when msg_num == num_msgs).

#pragma once
#include "database/i_sentence_handler.h"
#include "sentences/nmea_gsv.h"
#include "nmea_types.h"
#include <vector>

namespace nmea { namespace database {

class db_gsv_handler : public i_sentence_handler
{
public:
    db_gsv_handler() : parts_(), expected_parts_(0u) {}

    /// Called for each individual GSV part sentence.
    void update(const parser::nmea_gsv& part);

    msg_type handled_msg() const override { return MSG_NMEA_GSV; }
    void handle(nmea_field_store& store, uint64_t& mask) override;

    /// True after the last part of a set has been received.
    bool is_set_complete() const { return set_complete_; }

private:
    std::vector<parser::nmea_gsv> parts_;
    uint8_t expected_parts_ = 0u;
    bool    set_complete_   = false;
};

} }
