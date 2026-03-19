// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/i_sentence_handler.h
//
// Handler interface: write one sentence's fields into the field store.
// Mirrors ubx_parser's i_msg_update_handler.

#pragma once

#include "database/nmea_data_fields.h"

namespace nmea
{
namespace database
{

class nmea_field_store;

// ─── i_sentence_handler ───────────────────────────────────────────────────────

class i_sentence_handler
{
public:
    virtual ~i_sentence_handler() = default;

    i_sentence_handler()                                     = default;
    i_sentence_handler(const i_sentence_handler&)            = delete;
    i_sentence_handler& operator=(const i_sentence_handler&) = delete;

    /// Which msg_type does this handler serve?
    virtual msg_type handled_msg() const = 0;

    /// Write decoded fields into @p store and set the msg bit in @p mask.
    /// Called while the write lock on nmea_database is held.  Must not block.
    virtual void handle(nmea_field_store& store, uint64_t& mask) = 0;
};

} // namespace database
} // namespace nmea
