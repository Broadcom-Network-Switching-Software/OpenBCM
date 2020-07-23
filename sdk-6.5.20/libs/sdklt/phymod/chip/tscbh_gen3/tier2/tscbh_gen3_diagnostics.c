
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
#include <phymod/chip/tscbh_gen3.h>
#include <phymod/chip/tscbh_gen3_diagnostics.h>
#include "tscbh_gen3/tier1/tbhmod_gen3.h"
#include "blackhawk7_v1l8p1/tier1/common/srds_api_enum.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_common.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_interface.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_dependencies.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_internal.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_access.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_types.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_cfg_seq.h"

#define PATTERN_MAX_LENGTH 240
#ifdef PHYMOD_BLACKHAWK_SUPPORT

int tscbh_gen3_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("tscbh_phy_rx_slicer_position_set function is NOT SUPPORTED!!\n"));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("tscbh_phy_rx_slicer_position_get function is NOT SUPPORTED!!\n"));

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("tscbh_phy_rx_slicer_position_max_get function is NOT SUPPORTED!!\n"));

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_fec_cl91_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
       (tbhmod_gen3_fec_correctable_counter_get(&phy_copy.access, count));
    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_fec_cl91_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
       (tbhmod_gen3_fec_uncorrectable_counter_get(&phy_copy.access, count));
    return PHYMOD_E_NONE;

}

/* Get RS FEC symbol error count.*/
int tscbh_gen3_phy_rsfec_symbol_error_counter_get(const phymod_phy_access_t* phy,
                                             int max_count,
                                             int* actual_count,
                                             uint32_t* error_count)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, speed_id, i;
    tbhmod_gen3_spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 0x1 << start_lane;
    /* first read speed id from resolved status */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_speed_id_get(&phy_copy.access, &speed_id));

    /* first read the speed entry and then decode the speed and FEC type */
    phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

    /*decode speed entry */
    tbhmod_gen3_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    switch (speed_config_entry.num_lanes) {
        case 0: num_lane = 1;
            break;
        case 1: num_lane = 2;
            break;
        case 2: num_lane = 4;
            break;
        case 3: num_lane = 8;
            break;
        case 4: num_lane = 3;
            break;
        case 5: num_lane = 6;
            break;
        case 6: num_lane = 7;
            break;
        default:
            PHYMOD_DEBUG_ERROR(("Unsupported number of lane \n"));
            return PHYMOD_E_UNAVAIL;
    }

    /* Update lane mask. During AN, lane mask might change. */
    phy_copy.access.lane_mask = 0;
    for (i = 0; i < num_lane; i ++) {
        phy_copy.access.lane_mask |= 1 << (start_lane + i);
    }

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_rsfec_symbol_error_counter_get(&phy_copy.access,
                                               speed_config_entry.bit_mux_mode,
                                               max_count,
                                               actual_count,
                                               error_count));

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_BLACKHAWK_SUPPORT */
