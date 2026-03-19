// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/handlers/db_vtg_handler.h

#pragma once
#include "database/i_sentence_handler.h"
#include "sentences/nmea_vtg.h"

namespace nmea { namespace database {

class db_vtg_handler : public i_sentence_handler
{
public:
    void update(const parser::nmea_vtg& msg) { msg_ = msg; }
    msg_type handled_msg() const override { return MSG_NMEA_VTG; }
    void handle(nmea_field_store& store, uint64_t& mask) override;
private:
    parser::nmea_vtg msg_{};
};

} }
