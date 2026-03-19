#pragma once

#include <cstdint>

namespace gnss
{

/// Populated from UBX-TIM-TP, UBX-NAV-TIMEGPS, UBX-NAV-STATUS,
/// UBX-NAV-TIMEUTC, and UBX-NAV-CLOCK (committed with HP epoch).
struct timing_data
{
    // ── From UBX-TIM-TP ───────────────────────────────────────────────────
    int64_t  tow_ms;             ///< Time-pulse TOW [ms]
    int32_t  q_err_ps;           ///< Quantisation error [picoseconds]
    uint8_t  time_base;          ///< Clock source: 0=GNSS 1=UTC 2=GLONASS 3=BeiDou
    uint8_t  time_ref;           ///< GNSS reference: bits 7:4 of ref_info
    uint8_t  utc_standard;       ///< UTC standard: bits 3:0 of ref_info
    bool     utc_available;      ///< UTC time base available
    bool     q_err_valid;        ///< q_err_ps field is valid

    // ── From UBX-NAV-TIMEGPS ──────────────────────────────────────────────
    uint32_t gps_tow_ms;         ///< GPS time of week [ms]
    uint16_t gps_week;           ///< GPS week number
    int8_t   leap_seconds;       ///< UTC-GPS leap seconds

    // ── From UBX-NAV-STATUS ─────────────────────────────────────────────────────
    uint64_t ttff_ms;            ///< Time to first fix [ms]

    // ── From UBX-NAV-TIMEUTC ─────────────────────────────────────────────
    uint32_t utc_i_tow_ms;      ///< GPS TOW at time of UTC calculation [ms]
    uint32_t utc_t_acc_ns;      ///< Time accuracy estimate [ns]
    int32_t  utc_nano_ns;       ///< Sub-second UTC fraction [ns]; range -1e9..1e9
    uint16_t utc_year;          ///< UTC year
    uint8_t  utc_month;         ///< UTC month (1..12)
    uint8_t  utc_day;           ///< UTC day (1..31)
    uint8_t  utc_hour;          ///< UTC hour (0..23)
    uint8_t  utc_min;           ///< UTC minute (0..59)
    uint8_t  utc_sec;           ///< UTC second (0..60)
    uint8_t  utc_valid_flags;   ///< validTOW/validWKN/validUTC + utcStandard bits

    // ── From UBX-NAV-CLOCK ────────────────────────────────────────────────
    uint32_t clock_i_tow_ms;    ///< GPS TOW [ms]
    int32_t  clock_bias_ns;     ///< Receiver clock bias [ns]
    int32_t  clock_drift_nsps;  ///< Receiver clock drift [ns/s]
    uint32_t clock_t_acc_ns;    ///< Time accuracy estimate [ns]
    uint32_t clock_f_acc_psps;  ///< Frequency accuracy estimate [ps/s]

    bool     valid;              ///< True when populated at least once
};

} // namespace gnss
