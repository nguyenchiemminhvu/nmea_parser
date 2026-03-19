// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// database/nmea_field_store.h
//
// Flat double-precision field store indexed by data_field.
// Mirrors ubx_parser's ubx_field_store exactly in design.

#pragma once

#include "database/nmea_data_fields.h"
#include <cstring>
#include <cstdint>

namespace nmea
{
namespace database
{

class nmea_field_store
{
public:
    nmea_field_store() { reset(); }

    // ── Write ─────────────────────────────────────────────────────────────────

    template <typename T>
    void set(data_field field, T value)
    {
        const size_t idx = static_cast<size_t>(field);
        if (idx < static_cast<size_t>(DATA_NUM))
        {
            values_[idx] = static_cast<double>(value);
            valid_[idx]  = true;
        }
    }

    // ── Read ──────────────────────────────────────────────────────────────────

    template <typename T>
    bool get(data_field field, T& out) const
    {
        const size_t idx = static_cast<size_t>(field);
        if (idx < static_cast<size_t>(DATA_NUM) && valid_[idx])
        {
            out = static_cast<T>(values_[idx]);
            return true;
        }
        return false;
    }

    bool is_valid(data_field field) const
    {
        const size_t idx = static_cast<size_t>(field);
        return (idx < static_cast<size_t>(DATA_NUM)) && valid_[idx];
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    void reset()
    {
        std::memset(values_, 0, sizeof(values_));
        std::memset(valid_,  0, sizeof(valid_));
    }

private:
    double values_[DATA_NUM];
    bool   valid_ [DATA_NUM];
};

} // namespace database
} // namespace nmea
