#pragma once

#include <cstdint>

namespace gnss
{

/// Populated from UBX-NAV-DOP (high-priority epoch commit).
struct dop_data
{
    uint32_t i_tow_ms;          ///< GPS time of week [ms]
    float   gdop;              ///< Geometric DOP
    float   pdop;              ///< Position DOP
    float   tdop;              ///< Time DOP
    float   vdop;              ///< Vertical DOP
    float   hdop;              ///< Horizontal DOP
    float   ndop;              ///< Northing DOP
    float   edop;              ///< Easting DOP

    bool    valid;             ///< True when populated at least once
};

};