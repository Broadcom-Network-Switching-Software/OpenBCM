
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_config.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/phymod_acc.h>
#include <phymod/chip/tscbh_gen2.h>
#include <phymod/chip/tscbh_gen2_diagnostics.h>
#include "tscbh/tier1/tbhmod.h"

#define PATTERN_MAX_LENGTH 240
#ifdef PHYMOD_BLACKHAWK_SUPPORT


int tscbh_gen2_phy_fec_error_inject_set(const phymod_phy_access_t* phy,
                                        uint16_t error_control_map,
                                        phymod_fec_error_mask_t bit_error_mask)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    phymod_phy_speed_config_t speed_config;
    uint16_t control_map = 0;
    uint8_t valid_error_control = 0;
    tbhmod_fec_error_mask_t fec_error_mask;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    fec_error_mask.error_mask_bit_31_0 = bit_error_mask.error_mask_bit_31_0;
    fec_error_mask.error_mask_bit_63_32 = bit_error_mask.error_mask_bit_63_32;
    fec_error_mask.error_mask_bit_79_64 = bit_error_mask.error_mask_bit_79_64;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN(tscbh_gen2_phy_speed_config_get(phy, &speed_config));
    switch (speed_config.fec_type) {
        case phymod_fec_CL74 :
            /* valid error_control:
             * single lane port:  [0:0]
             * dual lane port  :  [1:0]
             * 4-lane port     :  [3:0]
             */
            if (0x1 == num_lane) {
                valid_error_control = 0x1;
            } else if (0x2 == num_lane) {
                valid_error_control = 0x3;
            } else if (0x4 == num_lane) {
                valid_error_control = 0xf;
            } else {
                return PHYMOD_E_UNAVAIL;
            }
            TBHMOD_CL74_EN_FLAG_SET(control_map);
            break;
        case phymod_fec_CL91 :
        case phymod_fec_RS544 :
        case phymod_fec_RS272 :
            valid_error_control = 0xf;
            TBHMOD_RSFEC_1XN_EN_FLAG_SET(control_map);
            break;
        case phymod_fec_RS544_2XN :
        case phymod_fec_RS272_2XN :
            valid_error_control = 0xff;
            TBHMOD_RSFEC_2XN_EN_FLAG_SET(control_map);
            break;
        case phymod_fec_None :
        default :
            return PHYMOD_E_CONFIG;
    }

    /* 0xff valid for all fec types.
     * BASER-FEC: It indicates error injection happening on all lanes of this port.
     * RS-FEC   : It indicates error injection happening on all 80-bit blocks of this port.
     */
    control_map |= (error_control_map == 0xff) ? valid_error_control : error_control_map;

    if ((control_map & 0xff) & (~valid_error_control)) {
        PHYMOD_DEBUG_ERROR((" %s:%d error_control_map is wrong! valid mask range 0x%x\n", __func__, __LINE__, valid_error_control));
        return PHYMOD_E_CONFIG;
    }

    phy_copy.access.lane_mask = phy->access.lane_mask;
    PHYMOD_IF_ERR_RETURN(tbhmod_fec_error_inject_set(&phy_copy.access, control_map, fec_error_mask));

    return PHYMOD_E_NONE;
}

int tscbh_gen2_phy_fec_error_inject_get(const phymod_phy_access_t* phy,
                                        uint16_t* error_control_map,
                                        phymod_fec_error_mask_t* bit_error_mask)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    phymod_phy_speed_config_t speed_config;
    uint16_t control_map = 0;
    tbhmod_fec_error_mask_t fec_error_mask;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN(tscbh_gen2_phy_speed_config_get(phy, &speed_config));
    switch (speed_config.fec_type) {
        case phymod_fec_CL74 :
            TBHMOD_CL74_EN_FLAG_SET(control_map);
            break;
        case phymod_fec_CL91 :
        case phymod_fec_RS544 :
        case phymod_fec_RS272 :
            TBHMOD_RSFEC_1XN_EN_FLAG_SET(control_map);
            break;
        case phymod_fec_RS544_2XN :
        case phymod_fec_RS272_2XN :
            TBHMOD_RSFEC_2XN_EN_FLAG_SET(control_map);
            break;
        default :
            return PHYMOD_E_CONFIG;
    }
    phy_copy.access.lane_mask = phy->access.lane_mask;
    PHYMOD_IF_ERR_RETURN(tbhmod_fec_error_inject_get(&phy_copy.access, &control_map, &fec_error_mask));

    bit_error_mask->error_mask_bit_31_0 = fec_error_mask.error_mask_bit_31_0;
    bit_error_mask->error_mask_bit_63_32 = fec_error_mask.error_mask_bit_63_32;
    bit_error_mask->error_mask_bit_79_64 = fec_error_mask.error_mask_bit_79_64;
    *error_control_map = control_map & 0xff;

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_BLACKHAWK_SUPPORT */

