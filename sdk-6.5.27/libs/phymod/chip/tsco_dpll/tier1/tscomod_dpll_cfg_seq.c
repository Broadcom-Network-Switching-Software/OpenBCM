/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tscomod_cfg_seq.c
 * Description: c functions implementing Tier1s for TSCOMOD Serdes Driver
 *---------------------------------------------------------------------*/

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tsco_dpll_xgxs_defs.h>
#include <tsco/tier1/tscomod.h>
#include <tsco_dpll/tier1/tscomod_dpll.h>
#include <tsco/tier1/tscomod_sc_lkup_table.h>

/*
 * Forward declarations:
*/


int tscomod_dpll_fclk_select_set(PHYMOD_ST *pc, int pll_index)
{
   MAIN0_SETUP_DPLLr_t MAIN0_SETUPr_reg;
    int start_lane, num_lane;
    phymod_access_t pa_copy;

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    /* decide which PML to use */
    if (start_lane < 4) {
        pa_copy.lane_mask = 0x1 << 0;
        PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUP_DPLLr(&pa_copy, &MAIN0_SETUPr_reg));
        MAIN0_SETUP_DPLLr_FCLK_SELf_SET(MAIN0_SETUPr_reg, pll_index);
        PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUP_DPLLr(&pa_copy, MAIN0_SETUPr_reg));

        pa_copy.lane_mask = 0x1 << 2;
        PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUP_DPLLr(&pa_copy, &MAIN0_SETUPr_reg));
        MAIN0_SETUP_DPLLr_FCLK_SELf_SET(MAIN0_SETUPr_reg, pll_index);
        PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUP_DPLLr(&pa_copy, MAIN0_SETUPr_reg));
    } else {
        pa_copy.lane_mask = 0x1 << 4;
        PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUP_DPLLr(&pa_copy, &MAIN0_SETUPr_reg));
        MAIN0_SETUP_DPLLr_FCLK_SELf_SET(MAIN0_SETUPr_reg, pll_index);
        PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUP_DPLLr(&pa_copy, MAIN0_SETUPr_reg));

        pa_copy.lane_mask = 0x1 << 6;
        PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUP_DPLLr(&pa_copy, &MAIN0_SETUPr_reg));
        MAIN0_SETUP_DPLLr_FCLK_SELf_SET(MAIN0_SETUPr_reg, pll_index);
        PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUP_DPLLr(&pa_copy, MAIN0_SETUPr_reg));
    }

    return PHYMOD_E_NONE;
}

int tscomod_dpll_fclk_select_get(PHYMOD_ST *pc, int* pll_index)
{
    phymod_access_t pa_copy;
    MAIN0_SETUP_DPLLr_t MAIN0_SETUPr_reg;

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    pa_copy.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUP_DPLLr(&pa_copy, &MAIN0_SETUPr_reg));
    *pll_index = MAIN0_SETUP_DPLLr_FCLK_SELf_GET(MAIN0_SETUPr_reg);

    return PHYMOD_E_NONE;
}

