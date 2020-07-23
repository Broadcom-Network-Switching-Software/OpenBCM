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
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/phymod_util.h>
#include <phymod/chip/tsce7_diagnostics.h>
#include "tsce16/tier1/temod16_enum_defines.h"
#include "tsce16/tier1/temod16.h"
#include "tsce16/tier1/te16PCSRegEnums.h"
#include "merlin7/tier1/merlin7_tsc_diag.h"
#include "merlin7/tier1/merlin7_tsc_debug_functions.h"
#include "merlin7/tier1/common/srds_api_enum.h"

#ifdef PHYMOD_TSCE7_SUPPORT

int tsce7_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    int start_lane, num_lane;
    int tmp_lane_mask;
    phymod_phy_access_t phy_copy;

    /* struct merlin7_detailed_lane_status_st lane_st[4]; */
    int i = 0;
    uint32_t cmd_type;
    int is_in_reset = 0;


    if (!type) {
        cmd_type = (uint32_t)TEMOD16_DIAG_DSC;
    } else if ((!PHYMOD_STRCMP(type, "ber")) ||
               (!PHYMOD_STRCMP(type, "Ber")) ||
               (!PHYMOD_STRCMP(type, "BER"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_BER;
    } else if ((!PHYMOD_STRCMP(type, "config")) ||
               (!PHYMOD_STRCMP(type, "Config")) ||
               (!PHYMOD_STRCMP(type, "CONFIG"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_CFG;
    } else if ((!PHYMOD_STRCMP(type, "cl72")) ||
               (!PHYMOD_STRCMP(type, "Cl72")) ||
               (!PHYMOD_STRCMP(type, "CL72"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_CL72;
    } else if ((!PHYMOD_STRCMP(type, "debug")) ||
               (!PHYMOD_STRCMP(type, "Debug")) ||
               (!PHYMOD_STRCMP(type, "DEBUG"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_DEBUG;
    } else if ((!PHYMOD_STRCMP(type, "state")) ||
               (!PHYMOD_STRCMP(type, "State")) ||
               (!PHYMOD_STRCMP(type, "STATE"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_STATE;
    } else if ((!PHYMOD_STRCMP(type, "verbose")) ||
               (!PHYMOD_STRCMP(type, "Verbose")) ||
               (!PHYMOD_STRCMP(type, "VERBOSE"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_ALL;
    } else if (!PHYMOD_STRCMP(type, "STD")) {
        cmd_type = (uint32_t)TEMOD16_DIAG_DSC_STD;
    } else {
        cmd_type = (uint32_t)TEMOD16_DIAG_STATE;
    }

    PHYMOD_DEBUG_ERROR((" %s:%d type = %d laneMask  = 0x%X\n", __func__, __LINE__, cmd_type, phy->access.lane_mask));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if (cmd_type == TEMOD16_DIAG_DSC) {
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_display_core_state(&phy_copy.access));
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_display_lane_state_hdr(&phy_copy.access));
        for (i = 0; i < 4; i++) {
            phy_copy.access.lane_mask = 0x1 << i ;
            PHYMOD_IF_ERR_RETURN
                (temod16_pmd_x4_reset_get(&phy_copy.access, &is_in_reset));
            if (is_in_reset) {
                PHYMOD_DIAG_OUT(("    - skip lane=%0d\n", i));
               continue;
            }
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_display_lane_state(&phy_copy.access));
       }
    } else if (cmd_type == TEMOD16_DIAG_DSC_STD) {
        PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
        PHYMOD_DIAG_OUT(("    | DSC Phy: 0x%03x lane_mask: 0x%02x                                  |\n", phy->access.addr, phy->access.lane_mask));
        PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_display_core_state(&phy_copy.access));
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_display_lane_state_hdr(&phy_copy.access));
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (temod16_pmd_x4_reset_get(&phy_copy.access, &is_in_reset));
            if (is_in_reset) {
                PHYMOD_DIAG_OUT(("    - skip lane=%0d\n", (i + start_lane)));
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_display_lane_state(&phy_copy.access));
        }

        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_read_event_log(&phy_copy.access));
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);

            switch (cmd_type) {
            case TEMOD16_DIAG_CFG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if (i == 0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (merlin7_tsc_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_lane_config(&phy_copy.access));
                break;
            case TEMOD16_DIAG_CL72:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CL72\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_cl72_status(&phy_copy.access));
                break;
            case TEMOD16_DIAG_DEBUG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_lane_debug_status(&phy_copy.access));
                break;
            case TEMOD16_DIAG_BER:
                PHYMOD_DEBUG_ERROR((" %s:%d type = BER\n", __func__, __LINE__));
                break;
            case TEMOD16_DIAG_ALL:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if (i==0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (merlin7_tsc_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_lane_config(&phy_copy.access));

                PHYMOD_DEBUG_ERROR((" %s:%d type = CL72\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_cl72_status(&phy_copy.access));

                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_lane_debug_status(&phy_copy.access));
                break;
            case TEMOD16_DIAG_STATE:
            default:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DEF\n", __func__, __LINE__));
                 PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_core_state_hdr(&phy_copy.access));
                 PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_core_state_line(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_core_state(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_lane_state_hdr(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_lane_state(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_read_event_log(&phy_copy.access));
                break;
            }
        }
    }
    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_TSCE7_SUPPORT */
