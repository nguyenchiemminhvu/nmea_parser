// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/handlers/db_gsa_handler.h

#pragma once
#include "database/i_sentence_handler.h"
#include "sentences/nmea_gsa.h"

namespace nmea { namespace database {

class db_gsa_handler : public i_sentence_handler
{
public:
    void update(const parser::nmea_gsa& msg) { msg_ = msg; }
    msg_type handled_msg() const override { return MSG_NMEA_GSA; }
    void handle(nmea_field_store& store, uint64_t& mask) override;
private:
    parser::nmea_gsa msg_{};
};

} }
