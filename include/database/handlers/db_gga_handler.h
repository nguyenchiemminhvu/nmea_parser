// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/handlers/db_gga_handler.h

#pragma once
#include "database/i_sentence_handler.h"
#include "sentences/nmea_gga.h"

namespace nmea { namespace database {

class db_gga_handler : public i_sentence_handler
{
public:
    void update(const parser::nmea_gga& msg) { msg_ = msg; }
    msg_type handled_msg() const override { return MSG_NMEA_GGA; }
    void handle(nmea_field_store& store, uint64_t& mask) override;
private:
    parser::nmea_gga msg_{};
};

} } // nmea::database
