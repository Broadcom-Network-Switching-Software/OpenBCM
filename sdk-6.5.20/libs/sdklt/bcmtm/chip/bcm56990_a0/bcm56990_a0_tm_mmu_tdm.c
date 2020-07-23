/*! \file bcm56990_a0_tm_mmu_tdm.c
 *
 * File containing flexport sequence for bcm56990_a0.
 *
 * The code in this file is shared with DV team. This file will only be updated
 * by porting DV code changes. If any extra change needs to be made to the
 * shared code, please also inform the DV team.
 *
 * DV file: $DV/tomahawk4/dv/ngsdk_interface/flexport/bcm56990_a0_tm_mmu_tdm.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmtm/chip/bcm56990_a0_tm.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include "bcm56990_a0_tm_idb_flexport.h"
#include "bcm56990_a0_tm_edb_flexport.h"
#include "bcm56990_a0_tm_mmu_tdm.h"

#define BSL_LOG_MODULE  BSL_LS_BCMTM_FLEXPORT

/*! @fn int th4_mmu_get_speed_decode(int unit, int speed,
 *              int *speed_decode)
 *  @param unit Device number
 *  @param speed Actual speed value
 *  @param *speed_decode Decoded speed ID value
 *  @brief API to return the decoded speed value, used to program
 *         EBCTM registers
 */
int
th4_mmu_get_speed_decode(int unit, int speed, uint32_t *speed_decode)
{
    if (speed >= 400000) {
        *speed_decode = 7;
    } else if (speed >= 200000) {
        *speed_decode = 6;
    } else if (speed >= 100000) {
        *speed_decode = 5;
    } else if (speed >= 50000) {
        *speed_decode = 4;
    } else if (speed >= 40000) {
        *speed_decode = 3;
    } else if (speed >= 25000) {
        *speed_decode = 2;
    } else if (speed >= 1000) {
        *speed_decode = 1;
    } else {
        *speed_decode = 0;
         return SHR_E_FAIL;
    }
    return SHR_E_NONE;
}

/*** START SDKLT API COMMON CODE ***/

static const struct th4_eb_sched_min_cell_spacing_within_pkt_s {
    int val_50g;
    int val_100g;
    int val_200g;
    int val_400g;
} th4_eb_sched_min_cell_spacing_within_pkt[3] = {
    {9, 9, 8, 3}, /* 1325 MHz */
    {9, 9, 4, 3}, /* 1025 MHz */
    {9, 9, 4, 3}  /*  950 MHz */
};

static const struct th4_eb_sched_min_cell_spacing_eop_to_sop_s {
    int val_50g;
    int val_100g;
    int val_200g;
    int val_400g;
} th4_eb_sched_min_cell_spacing_eop_to_sop[3] = {
    {9, 9, 2, 2}, /* 1325 MHz */
    {9, 8, 2, 2}, /* 1025 MHz */
    {9, 8, 2, 2}  /*  950 MHz */
};

static const struct th4_eb_sched_min_cell_spacing_s {
    int val_50g;
    int val_100g;
    int val_200g;
    int val_400g;
} th4_eb_sched_min_cell_spacing[3] = {
    {6, 2, 2, 2}, /* 1325 MHz */
    {6, 2, 2, 2}, /* 1025 MHz */
    {6, 2, 2, 2}  /*  950 MHz */
};

static const struct th4_mmu_ptsch_min_spacing_s {
    int val_50g;
    int val_100g;
    int val_200g;
    int val_400g;
} th4_mmu_ptsch_min_spacing[3] = {
    {16, 16, 8, 4}, /* 1325 MHz */
    {12, 12, 6, 4}, /* 1025 MHz */
    {12, 12, 6, 4}  /*  950 MHz */
};

static const struct th4_mmu_ptsch_penalty_min_spacing_s {
    int val_50g;
    int val_100g;
    int val_200g;
    int val_400g;
} th4_mmu_ptsch_penalty_min_spacing[3] = {
    {200, 100, 50, 25}, /* 1325 MHz */
    {200, 100, 50, 25}, /* 1025 MHz */
    {200, 100, 50, 25}  /*  950 MHz */
};

static const struct th4_tdm_mmu_eb_credit_config_vals_s {
    int val_50g;
    int val_100g;
    int val_200g;
    int val_400g;
} th4_tdm_mmu_eb_credit_config_vals[3] = {
    {54, 69, 99, 160}, /* 1325 MHz */
    {58, 78, 117, 195}, /* 1025 MHz */
    {60, 81, 123, 208}, /* 950 MHz */
};

static const struct th4_mmu_ebpcc_portdeficit_s {
    int portdeficit_sample_limit[3][7];
    int portdeficit_sample_period[3][7];
    int eport_cell_threshold[3][7];
    int portdeficit_backoff_limit[3][7];
    int warm_up_base_cnt[3][7];
    int warm_up_mult_cnt[3][7];
} th4_mmu_ebpcc_portdeficit = {
    {   /* 10G   25G   40G   50G   100G  200G  400G */
        { 2480,  992,  620,  496,  248,  124,   62}, /* 1325 MHz */
        { 1919,  768,  480,  384,  192,   96,   96}, /* 1025 MHz */
        { 1779,  712,  445,  356,  178,   89,   89}  /*  950 MHz */
    },
    {   /* 10G   25G   40G   50G   100G  200G  400G */
        { 1960, 1960, 1960, 1960,  980,  490,  245}, /* 1325 MHz */
        { 1517, 1517, 1517, 1517,  759,  380,  380}, /* 1025 MHz */
        { 1406, 1406, 1406, 1406,  704,  353,  353}  /*  950 MHz */
    },
    {   /* 10G   25G   40G   50G   100G  200G  400G */
        {   27,   27,   27,   27,   34,   49,   80}, /* 1325 MHz */
        {   29,   29,   29,   29,   39,   58,   58}, /* 1025 MHz */
        {   30,   30,   30,   30,   40,   61,   61}  /*  950 MHz */
    },
    {   /* 10G   25G   40G   50G   100G  200G  400G */
        { 5880, 5880, 5880, 5880, 2940, 1470,  735}, /* 1325 MHz */
        { 4549, 4549, 4549, 4549, 2275, 1138, 1138}, /* 1025 MHz */
        { 4217, 4217, 4217, 4217, 2109, 1055, 1055}  /*  950 MHz */
    },
    {   /* 10G   25G   40G   50G   100G  200G  400G */
        {  146,  146,  146,  146,  128,  141,  162}, /* 1325 MHz */
        {  113,  113,  113,  113,  100,  110,  110}, /* 1025 MHz */
        {   88,   88,   88,   88,   78,   86,   86}  /*  950 MHz */
    },
    {   /* 10G   25G   40G   50G   100G  200G  400G */
        {   10,   10,   10,   10,   8,     6,    4}, /* 1325 MHz */
        {   10,   10,   10,   10,   8,     6,    6}, /* 1025 MHz */
        {   10,   10,   10,   10,   8,     6,    6}  /*  950 MHz */
    }
};

static int
th4_get_pipe_pm_from_pport(int pnum)
{
    return (((pnum - 1) & 0xf) >> 2);
}

static int
th4_get_subp_from_pport(int pnum)
{
    return ((pnum - 1) & 0x3);
}

int
th4_tdm_global_to_local_phy_num(int pport, uint32_t *local_phy_num)
{
    *local_phy_num = 0x3f;

    if (0 <= pport && pport <= TH4_PHYS_PORTS_PER_PIPE) {
        *local_phy_num = pport;
    } else if (((TH4_PHYS_PORTS_PER_PIPE + 1) <= pport) &&
               (pport <= (TH4_PHYS_PORTS_PER_PIPE * TH4_PIPES_PER_DEV))) {
        *local_phy_num = ((pport - 1) % TH4_PHYS_PORTS_PER_PIPE);
    } else if (pport == TH4_PHY_PORT_MNG0 || pport == TH4_PHY_PORT_MNG1) {
        *local_phy_num = TH4_MGMT_LOCAL_PHYS_PORT;
    } else if (TH4_PHY_PORT_LPBK0 <= pport && pport <= TH4_PHY_PORT_LPBK7) {
        *local_phy_num = TH4_LB_LOCAL_PHYS_PORT;
    } else {
        return SHR_E_PARAM;
    }

    return SHR_E_NONE;
}

static int
th4_tdm_set_cal_config(int unit,
                       int lport,
                       mmu_sched_inst_name_e sched_inst_name,
                       int is_port_down)
{
    int pm_num;
    int subp;
    int num_50g_slots;
    int is_special_slot;
    int lane, slot;
    int port_sched_table[TH4_TDM_LENGTH];
    int pm_mapping[8] = {0,4,2,6,1,5,3,7};
    int this_port_number;
    int this_slot_valid;
    int this_port_speed;
    int pipe, pport;

    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    uint32_t fval_64[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t this_port_cal_slot_bitmap[2];

    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    pport = port_map->lport_map[lport].pport;
    pipe = port_map->lport_map[lport].pipe;
    pm_num = th4_get_pipe_pm_from_pport(pport);
    subp = th4_get_subp_from_pport(pport);
    num_50g_slots =
        (port_map->lport_map[lport].cur_speed + TDM_SPEED_40G) / TDM_SPEED_50G;

    /*
     * Check for MMU:
     * Special port detection:
     * Port is not front panel port (local IDB port number >= 16).
     */
    is_special_slot =
        (port_map->pport_map[pport].mlocal_port >= TH4_FP_PORTS_PER_PIPE) ? 1 : 0;
    for (slot = 0; slot < TH4_TDM_LENGTH; slot++) {
        port_sched_table[slot] = 0;
    }
    if (is_special_slot == 0) {
        for (lane = (2 * subp); lane < (2 * subp) + num_50g_slots; lane++) {
            slot = pm_mapping[lane] * TH4_PBLKS_PER_PIPE + pm_num;
            port_sched_table[slot] = 1;
        }
    } else {
        port_sched_table[TH4_TDM_LENGTH - 1] = 1;
    }

    this_port_number = (is_port_down == 1) ? 0 :
        ((is_special_slot == 1) ? TH4_TDM_SCHED_SPECIAL_SLOT :
        port_map->pport_map[pport].mport & 0x1f);

    this_slot_valid  = (is_port_down == 1) ? 0 : 1;

    if (is_port_down == 1) {
        this_port_speed = 0;
    } else if (is_special_slot == 1) {
        if (bcmtm_lport_is_lb(unit, lport)){
            this_port_speed = 2; /* Speed 100G for lpbk port */
        } else {
            this_port_speed = 3; /* Speed 50G for Aux ports */
        }
    } else {
        switch (port_map->lport_map[lport].cur_speed) {
        case TDM_SPEED_400G:
            this_port_speed = 0;
            break;
        case TDM_SPEED_200G:
            this_port_speed = 1;
            break;
        case TDM_SPEED_100G:
            this_port_speed = 2;
            break;
        case TDM_SPEED_50G:
            this_port_speed = 3;
            break;
        default: /* Speed < 50G */
            this_port_speed = 3;
            break;
        }
    }

    SHR_BITCLR_RANGE(this_port_cal_slot_bitmap, 0, 64);
    if (is_port_down == 0) {
        for (slot = 0; slot < TH4_TDM_LENGTH; slot++) {
            if (port_sched_table[slot]==1) {
                SHR_BITSET(this_port_cal_slot_bitmap, slot);
            }
        }
    }

    /* Select the right reg based on sched instance. */
    switch (sched_inst_name) {
    case TDM_MMU_MAIN_SCHED:
        reg = MMU_PTSCH_CAL_CONFIGr;
        break;
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_CAL_CONFIGr;
        break;
    case TDM_MMU_EB_PKT_SCHED:
        reg = MMU_EBPTS_PKSCH_CAL_CONFIGr;
        break;
    default:
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "scheduler instance name is not supported %d\n"),
                   sched_inst_name));
        return SHR_E_PARAM;
        break;
    }
    for (slot = 0; slot < TH4_TDM_LENGTH; slot++) {
        if (port_sched_table[slot] == 1) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, slot, pipe);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, ltmbuf));

            fid = THIS_PORT_NUMBERf;
            fval = this_port_number;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = THIS_PORT_CAL_SLOT_BITMAPf;
            fval_64[0] = this_port_cal_slot_bitmap[0];
            fval_64[1] = this_port_cal_slot_bitmap[1];
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, fval_64));

            fid = THIS_SLOT_VALIDf;
            fval = this_slot_valid;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = THIS_PORT_SPEEDf;
            fval = this_port_speed;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            BCMTM_PT_DYN_INFO(pt_dyn_info, slot, pipe);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, ltmbuf));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
th4_tdm_set_max_spacing(int unit,
                        mmu_sched_inst_name_e sched_inst_name,
                        int pipe)
{
    int max_spacing_special_slot;
    int max_spacing_all_speeds, skip_instance;
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    /* Select the right reg based on sched instance. */
    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_MAX_SPACINGr;
        break;
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unsupported sched_inst_name\n")));
        return SHR_E_PARAM;
        break;
    }

    if (skip_instance == 1) {
        return SHR_E_NONE;
    }

    max_spacing_all_speeds = (drv_info->frequency == 1325) ? 54 :
                             ((drv_info->frequency >= 1025) ? 42 : 39);
    max_spacing_special_slot = 33;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
    fid = MAX_SPACING_ALL_SPEEDSf;
    fval = max_spacing_all_speeds;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    fid = MAX_SPACING_SPECIAL_SLOTf;
    fval = max_spacing_special_slot;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
th4_tdm_set_feature_ctrl(int unit,
                         mmu_sched_inst_name_e sched_inst_name,
                         int pipe)
{
    int skip_instance;
    int disable_max_spacing;
    int enable_bubble_mop;
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_FEATURE_CTRLr;
        enable_bubble_mop = 0; /* It's don't care; set to 0. */
        break;
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unsupported sched_inst_name\n")));
        return SHR_E_PARAM;
        break;
    }

    if (skip_instance == 1) {
        return SHR_E_NONE;
    }

    disable_max_spacing = 0;
    enable_bubble_mop = 1;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    fid = DISABLE_MAX_SPACINGf;
    fval = disable_max_spacing;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ENABLE_BUBBLE_MOP_400Gf;
    fval = enable_bubble_mop;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ENABLE_BUBBLE_MOP_200Gf;
    fval = enable_bubble_mop;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ENABLE_BUBBLE_MOP_100Gf;
    fval = enable_bubble_mop;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ENABLE_BUBBLE_MOP_50Gf;
    fval = enable_bubble_mop;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
th4_tdm_set_cbmg_value(int unit,
                       mmu_sched_inst_name_e sched_inst_name,
                       int pipe_num)
{
    int skip_instance;
    int terminal_counter_value, mmu_null_slot_counter_value;

    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_CBMG_VALUEr;
        break;
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unsupported sched_inst_name\n")));
        return SHR_E_PARAM;
        break;
    }

    if (skip_instance == 1) {
        return SHR_E_NONE;
    }

    /*
     * TH4 update: TH4_MMU_PortSch_uArch_delta.pdf
     * 4.1.5 MMU Idle slot handling
     * Minimum idle slot requirement for MMU needs to be configured in both the
     * Schedulers towards a common packet processing pipeline to guarantee SBUS
     * access slots for MMU. Scheduler has 16-bit config register to control
     * the MMU idle slot requirement, and it needs to be programmed with the
     * value of 5000 after reset. This will guarantee that each scheduler
     * will generate idle slots once every 5000 cycles, and SBUS access
     * for MMU will not time out.
     */
    mmu_null_slot_counter_value = 5000;
    /*  terminal_counter_value - Not used in TH4. */
    terminal_counter_value = 0;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
    fid = TERMINAL_COUNTER_VALUEf;
    fval = terminal_counter_value;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    fid = MMU_NULL_SLOT_COUNTER_VALUEf;
    fval = mmu_null_slot_counter_value;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
th4_tdm_set_urg_cell_spacing(int unit,
                             mmu_sched_inst_name_e sched_inst_name)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int core_clock_freq, skip_instance;
    int urg_pick_50g_spacing;
    int urg_pick_100g_spacing;
    int urg_pick_200g_spacing;
    int urg_pick_400g_spacing;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    /* Select the right reg based on sched instance. */
    skip_instance = 0;

    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_URG_CELL_SPACINGr;
        break;
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unsupported sched_inst_name\n")));
        return SHR_E_PARAM;
        break;
    }

    if (skip_instance == 1) {
        return SHR_E_NONE;
    }

    core_clock_freq = drv_info->frequency;
    if (core_clock_freq == 1325) {
        urg_pick_50g_spacing  = 16;
        urg_pick_100g_spacing = 8;
        urg_pick_200g_spacing = 4;
        urg_pick_400g_spacing = 2;
    } else if (core_clock_freq >= 1025){
        urg_pick_50g_spacing  = 12;
        urg_pick_100g_spacing = 6;
        urg_pick_200g_spacing = 3;
        urg_pick_400g_spacing = 2; /* N/A */
    } else { /* 950 MHz */
        urg_pick_50g_spacing  = 12;
        urg_pick_100g_spacing = 6;
        urg_pick_200g_spacing = 3;
        urg_pick_400g_spacing = 2; /* N/A */
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    fid = URG_PICK_400G_SPACINGf;
    fval = urg_pick_400g_spacing;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = URG_PICK_200G_SPACINGf;
    fval = urg_pick_200g_spacing;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = URG_PICK_100G_SPACINGf;
    fval = urg_pick_100g_spacing;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = URG_PICK_50G_SPACINGf;
    fval = urg_pick_50g_spacing;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}


static int
th4_tdm_set_min_spacing(int unit,
                        mmu_sched_inst_name_e sched_inst_name)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int skip_instance, freq_indx;

    SHR_FUNC_ENTER(unit);

    /* Select the right reg based on sched instance. */
    skip_instance = 0;

    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        skip_instance = 0;
        break;
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unsupported sched_inst_name\n")));
        return SHR_E_PARAM;
        break;
    }

    if (skip_instance == 1) {
        return SHR_E_NONE;
    }

    bcmtm_drv_info_get(unit, &drv_info);

    freq_indx = (drv_info->frequency == 1325) ? 0 :
                ((drv_info->frequency >= 1025) ? 1 : 2);

    reg = MMU_EBPTS_MIN_CELL_SPACINGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    fid = ANY_CELL_50G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing[freq_indx].val_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ANY_CELL_100G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing[freq_indx].val_100g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ANY_CELL_200G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing[freq_indx].val_200g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ANY_CELL_400G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing[freq_indx].val_400g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    reg = MMU_EBPTS_MIN_CELL_SPACING_EOP_TO_SOPr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    fid = INTER_PKT_50G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing_eop_to_sop[freq_indx].val_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = INTER_PKT_100G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing_eop_to_sop[freq_indx].val_100g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = INTER_PKT_200G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing_eop_to_sop[freq_indx].val_200g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = INTER_PKT_400G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing_eop_to_sop[freq_indx].val_400g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    reg = MMU_EBPTS_MIN_PORT_PICK_SPACING_WITHIN_PKTr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    fid = MID_PKT_50G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing_within_pkt[freq_indx].val_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = MID_PKT_100G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing_within_pkt[freq_indx].val_100g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = MID_PKT_200G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing_within_pkt[freq_indx].val_200g;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = MID_PKT_400G_MIN_SPACINGf;
    fval = th4_eb_sched_min_cell_spacing_within_pkt[freq_indx].val_400g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/* Applies to MMU_PTSCH
 * Regs configured:
 * - REGULAR_MIN_SPACING
 * - PENALTY_MIN_SPACING
 */
static int
th4_tdm_set_ptsch_min_spacing(
    int unit,
    mmu_sched_inst_name_e sched_inst_name)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int skip_instance, freq_indx;

    SHR_FUNC_ENTER(unit);

    /* Select the right reg based on sched instance. */
    skip_instance = 0;

    switch (sched_inst_name) {
    case TDM_MMU_MAIN_SCHED:
        skip_instance = 0;
        break;
    case TDM_MMU_EB_PORT_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unsupported sched_inst_name\n")));
        return SHR_E_PARAM;
        break;
    }

    if (skip_instance == 1) {
        return SHR_E_NONE;
    }

    bcmtm_drv_info_get(unit, &drv_info);

    freq_indx = (drv_info->frequency == 1325) ? 0 :
                ((drv_info->frequency >= 1025) ? 1 : 2);

    reg = MMU_PTSCH_REGULAR_MIN_SPACINGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    fid = REGULAR_50G_MIN_SPACINGf;
    fval = th4_mmu_ptsch_min_spacing[freq_indx].val_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = REGULAR_100G_MIN_SPACINGf;
    fval = th4_mmu_ptsch_min_spacing[freq_indx].val_100g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = REGULAR_200G_MIN_SPACINGf;
    fval = th4_mmu_ptsch_min_spacing[freq_indx].val_200g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = REGULAR_400G_MIN_SPACINGf;
    fval = th4_mmu_ptsch_min_spacing[freq_indx].val_400g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    reg = MMU_PTSCH_PENALTY_MIN_SPACINGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    fid = PENALTY_50G_MIN_SPACINGf;
    fval = th4_mmu_ptsch_penalty_min_spacing[freq_indx].val_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = PENALTY_100G_MIN_SPACINGf;
    fval = th4_mmu_ptsch_penalty_min_spacing[freq_indx].val_100g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = PENALTY_200G_MIN_SPACINGf;
    fval = th4_mmu_ptsch_penalty_min_spacing[freq_indx].val_200g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = PENALTY_400G_MIN_SPACINGf;
    fval = th4_mmu_ptsch_penalty_min_spacing[freq_indx].val_400g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}


static int
th4_tdm_set_ebctm_min_spacing(
    int unit)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int freq_indx;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);

    freq_indx = (drv_info->frequency == 1325) ? 0 :
                ((drv_info->frequency >= 1025) ? 1 : 2);

    /* 400G */
    reg = MMU_EBCTM_MIN_MOP_SPACING_SPEED_BUCKET_0_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = NUM_CLK_PER_MOP_CELLf;
    fval = th4_eb_sched_min_cell_spacing_within_pkt[freq_indx].val_400g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    /* 200G */
    reg = MMU_EBCTM_MIN_MOP_SPACING_SPEED_BUCKET_1_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = NUM_CLK_PER_MOP_CELLf;
    fval = th4_eb_sched_min_cell_spacing_within_pkt[freq_indx].val_200g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    /* <= 100G */
    reg = MMU_EBCTM_MIN_MOP_SPACING_SPEED_BUCKET_2_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = NUM_CLK_PER_MOP_CELLf;
    fval = th4_eb_sched_min_cell_spacing_within_pkt[freq_indx].val_100g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}


static int
th4_tdm_set_ebpcc_tct_speed(
    int unit)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int freq_indx;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);

    freq_indx = (drv_info->frequency == 1325) ? 0 :
                ((drv_info->frequency >= 1025) ? 1 : 2);

    /* 10G */
    reg = MMU_EBPCC_TCT_SPEED_0_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = EPORT_CELL_CREDITSf;
    fval = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    /* 25G */
    reg = MMU_EBPCC_TCT_SPEED_1_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = EPORT_CELL_CREDITSf;
    fval = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    /* 40G */
    reg = MMU_EBPCC_TCT_SPEED_2_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = EPORT_CELL_CREDITSf;
    fval = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    /* 50G */
    reg = MMU_EBPCC_TCT_SPEED_3_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = EPORT_CELL_CREDITSf;
    fval = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    /* 100G */
    reg = MMU_EBPCC_TCT_SPEED_4_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = EPORT_CELL_CREDITSf;
    fval = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_100g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    /* 200G */
    reg = MMU_EBPCC_TCT_SPEED_5_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = EPORT_CELL_CREDITSf;
    fval = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_200g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    /* 400G */
    reg = MMU_EBPCC_TCT_SPEED_6_CFGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    fid = EPORT_CELL_CREDITSf;
    fval = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_400g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}


static int
th4_tdm_set_ebpcc_tct_portdeficit(
    int unit)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int freq_indx, speed_indx;

    const bcmdrd_sid_t mmu_ebpcc_portdeficit_th0[7]= {
        MMU_EBPCC_PORTDEFICIT_THRESHOLD0_SPEED_0_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD0_SPEED_1_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD0_SPEED_2_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD0_SPEED_3_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD0_SPEED_4_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD0_SPEED_5_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD0_SPEED_6_CFGr
    };
    const bcmdrd_sid_t mmu_ebpcc_portdeficit_th1[7]= {
        MMU_EBPCC_PORTDEFICIT_THRESHOLD1_SPEED_0_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD1_SPEED_1_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD1_SPEED_2_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD1_SPEED_3_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD1_SPEED_4_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD1_SPEED_5_CFGr,
        MMU_EBPCC_PORTDEFICIT_THRESHOLD1_SPEED_6_CFGr
    };
    const bcmdrd_sid_t mmu_ebpcc_portdeficit_warm_up[7]= {
        MMU_EBPCC_PORTDEFICIT_WARM_UP_SPEED_0_CFGr,
        MMU_EBPCC_PORTDEFICIT_WARM_UP_SPEED_1_CFGr,
        MMU_EBPCC_PORTDEFICIT_WARM_UP_SPEED_2_CFGr,
        MMU_EBPCC_PORTDEFICIT_WARM_UP_SPEED_3_CFGr,
        MMU_EBPCC_PORTDEFICIT_WARM_UP_SPEED_4_CFGr,
        MMU_EBPCC_PORTDEFICIT_WARM_UP_SPEED_5_CFGr,
        MMU_EBPCC_PORTDEFICIT_WARM_UP_SPEED_6_CFGr
    };


    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);

    freq_indx = (drv_info->frequency == 1325) ? 0 :
                ((drv_info->frequency >= 1025) ? 1 : 2);

    /* MMU_EBPCC_PORTDEFICIT_THRESHOLD0 */
    for (speed_indx = 0; speed_indx < 7; speed_indx++) {
        reg = mmu_ebpcc_portdeficit_th0[speed_indx];
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
        fid = PORTDEFICIT_SAMPLE_LIMITf;
        fval = th4_mmu_ebpcc_portdeficit.portdeficit_sample_limit[freq_indx][speed_indx];
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
        fid = PORTDEFICIT_SAMPLE_PERIODf;
        fval = th4_mmu_ebpcc_portdeficit.portdeficit_sample_period[freq_indx][speed_indx];
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
    }

    /* MMU_EBPCC_PORTDEFICIT_THRESHOLD1 */
    for (speed_indx = 0; speed_indx < 7; speed_indx++) {
        reg = mmu_ebpcc_portdeficit_th1[speed_indx];
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
        fid = EPORT_CELL_THRESHOLDf;
        fval = th4_mmu_ebpcc_portdeficit.eport_cell_threshold[freq_indx][speed_indx];
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
        fid = PORTDEFICIT_BACKOFF_LIMITf;
        fval = th4_mmu_ebpcc_portdeficit.portdeficit_backoff_limit[freq_indx][speed_indx];
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
    }

    /* MMU_EBPCC_PORTDEFICIT_WARM_UP */
    for (speed_indx = 0; speed_indx < 7; speed_indx++) {
        reg = mmu_ebpcc_portdeficit_warm_up[speed_indx];
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
        fid = WARM_UP_BASE_CNTf;
        fval = th4_mmu_ebpcc_portdeficit.warm_up_base_cnt[freq_indx][speed_indx];
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
        fid = WARM_UP_MULT_CNTf;
        fval = th4_mmu_ebpcc_portdeficit.warm_up_mult_cnt[freq_indx][speed_indx];
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
    }

exit:
    SHR_FUNC_EXIT();
}


static int
th4_tdm_set_pksch_pkt_credits(int unit,
                              mmu_sched_inst_name_e sched_inst_name)
{
    int skip_instance;
    int pkt_credit_count;
    int pkt_credit_50g;
    int pkt_credit_100g;
    int pkt_credit_200g;
    int pkt_credit_400g;
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    /* TH4 recommended */
    pkt_credit_count = 20;
    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_MMU_EB_PKT_SCHED:
        reg = MMU_EBPTS_PKSCH_PKT_CREDITS_PER_PIPEr;
        break;
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PORT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unsupported sched_inst_name\n")));
        return SHR_E_PARAM;
        break;
    }

    if (skip_instance == 1) {
        return SHR_E_NONE;
    }

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
    fid = PKT_CREDIT_COUNTf;
    fval = pkt_credit_count;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));

    reg = MMU_EBPTS_PKSCH_PKT_CREDITS_PER_PORTr;
    pkt_credit_50g = 1;
    pkt_credit_100g = 2;
    pkt_credit_200g = 3;
    pkt_credit_400g = 4;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
    fid = PKT_CREDITS_FOR_50Gf;
    fval = pkt_credit_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = PKT_CREDITS_FOR_100Gf;
    fval = pkt_credit_100g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = PKT_CREDITS_FOR_200Gf;
    fval = pkt_credit_200g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = PKT_CREDITS_FOR_400Gf;
    fval = pkt_credit_400g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}


static int
th4_tdm_set_eb_credit_config(int unit,
                             int lport,
                             mmu_sched_inst_name_e sched_inst_name,
                             int is_port_down)
{
    int inst, freq_indx;
    int mmu_port, pport;
    uint32_t num_eb_credits;
    uint32_t rval;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmtm_port_map_info_t *port_map;
    bcmtm_drv_info_t *drv_info;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    if (sched_inst_name != TDM_MMU_MAIN_SCHED) {
        return SHR_E_NONE;
    }

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);

    pport = port_map->lport_map[lport].pport;
    mmu_port = port_map->pport_map[pport].mport;
    freq_indx = (drv_info->frequency == 1325) ? 0 :
                ((drv_info->frequency >= 1025) ? 1 : 2);

    reg = MMU_PTSCH_EB_CREDIT_CONFIGr;

    if (is_port_down == 1) {
        num_eb_credits = 0;
    } else {
        switch (port_map->lport_map[lport].cur_speed) {
        case TDM_SPEED_400G:
            num_eb_credits = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_400g;
            break;
        case TDM_SPEED_200G:
            num_eb_credits = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_200g;
            break;
        case TDM_SPEED_100G:
            num_eb_credits = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_100g;
            break;
        default: /* any speed <= 50G; */
            num_eb_credits = th4_tdm_mmu_eb_credit_config_vals[freq_indx].val_50g;
            break;
        }
    }

    rval = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, NUM_EB_CREDITSf, &rval, &num_eb_credits));

    inst = mmu_port;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &rval));

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int th4_tdm_set_ebshp_port_cfg(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    mmu_sched_inst_name_e sched_inst_name,
 *    int pipe,
 *    int pport,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @param pport pport to configure.
 *  @param pport is_port_down. boolean to indicate if port down
 *  @brief API to initialize/clear per port MMU_EBPTS_EBSHP_PORT_CFG
 * Description:
 *      API to initialize/clear per port MMU_EBPTS_EBSHP_PORT_CFG
 *      Regs/Mems configured: MMU MAIN SCHED MMU_EBPTS_EBSHP_PORT_CFG
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
static int
th4_tdm_set_ebshp_port_cfg(int unit,
                           int lport,
                           mmu_sched_inst_name_e sched_inst_name,
                           int is_port_down)
{
    int mmu_port, mmu_port_lport;
    uint32_t port_speed = 0;
    int inst, pipe, pport;
    uint32_t rval;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);

    if (sched_inst_name != TDM_MMU_EB_PORT_SCHED) {
        return SHR_E_NONE;
    }

    bcmtm_port_map_get(unit, &port_map);

    reg = MMU_EBPTS_EBSHP_PORT_CFGr;
    pipe = port_map->lport_map[lport].pipe;
    pport = port_map->lport_map[lport].pport;
    mmu_port = port_map->pport_map[pport].mport;
    mmu_port_lport = port_map->lport_map[lport].mlocal_port;

    if (is_port_down == 1) {
        port_speed = 0;
    } else {
        SHR_IF_ERR_EXIT
            (th4_mmu_get_speed_decode(unit, port_map->lport_map[lport].cur_speed,
                                      &port_speed));
    }

    rval = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, PORT_SPEEDf, &rval, &port_speed));

    inst = mmu_port;
    LOG_DEBUG(BSL_LS_SOC_PORT,
             (BSL_META_U(unit,
                         "pipe = %0d, mmu_port = %0d, inst = %0d, "
                         "pport = %0d, lport = %0d, "
                         "mmu_port_lport = %0d\n"),
              pipe, mmu_port, inst, pport, lport, mmu_port_lport));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &rval));

exit:
    SHR_FUNC_EXIT();
}


/*! @fn int th4_tdm_clear_edb_credit_counter_per_port(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    mmu_sched_inst_name_e sched_inst_name,
 *    int pipe,
 *    int pport,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @param pport pport to configure.
 *  @param is_port_down. boolean to indicate if port down
 *  @brief API to clear per port EDB_CREDIT_COUNTER
 * Description:
 *      API to initialize/clear per port EDB_CREDIT_COUNTER
 *      Regs/Mems configured: MMU EB_PORT_SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
static int
th4_tdm_clear_edb_credit_counter_per_port(int unit,
                                          int lport,
                                          mmu_sched_inst_name_e name,
                                          int is_port_down)
{
    int inst;
    int mmu_port, pport;
    uint32_t num_edb_credits;
    uint32_t rval;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);

    if (name != TDM_MMU_EB_PORT_SCHED) {
        return SHR_E_NONE;
    }

    bcmtm_port_map_get(unit, &port_map);

    reg = MMU_EBPTS_EDB_CREDIT_COUNTERr;

    pport = port_map->lport_map[lport].pport;
    mmu_port = port_map->pport_map[pport].mport;
    num_edb_credits = 0;

    rval = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, NUM_EDB_CREDITSf, &rval, &num_edb_credits));

   inst = mmu_port;
   BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
   SHR_IF_ERR_EXIT
       (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &rval));

exit:
    SHR_FUNC_EXIT();
}


/*! @fn int th4_tdm_set_mmu_port_to_pport_mapping(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    mmu_sched_inst_name_e sched_inst_name,
 *    int pipe,
 *    int pport,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @param pport pport to configure.
 *  @param pport is_port_down. boolean to indicate if port down
 *  @brief API to initialize/clear per port MMU_PORT_TO_PHY_PORT_MAPPING
 * Description:
 *      API to initialize/clear per port MMU_PORT_TO_PHY_PORT_MAPPING
 *      Regs/Mems configured: MMU EB PRT
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
static int
th4_tdm_set_mmu_port_to_pport_mapping(int unit,
                                         int lport,
                                         mmu_sched_inst_name_e sched_inst_name,
                                         int is_port_down)
{
    int mmu_port;
    int pport;
    uint32_t pport_num;
    int inst;
    uint32_t rval;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmtm_port_map_info_t *port_map;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    if (sched_inst_name != TDM_MMU_EB_PORT_SCHED) {
        return SHR_E_NONE;
    }

    bcmtm_port_map_get(unit, &port_map);

    reg = MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPINGr;
    pport = port_map->lport_map[lport].pport;
    mmu_port = port_map->pport_map[pport].mport;

    if (is_port_down == 1) {
        pport_num = 0x3f;
    } else {
        SHR_IF_ERR_EXIT
            (th4_tdm_global_to_local_phy_num(pport, &pport_num));
    }

    rval = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, PHY_PORT_NUMf, &rval, &pport_num));

    inst = mmu_port;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, &rval));

exit:
    SHR_FUNC_EXIT();
}

static int
th4_mmu_tdm_sched_general_init(
    int unit,
    mmu_sched_inst_name_e sched_inst_name)
{
    uint32_t pipe;
    uint32_t pipe_map;
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    /* Takes effect only in MMU_PTSCH scheduler */
    SHR_IF_ERR_EXIT(th4_tdm_set_ptsch_min_spacing(unit, sched_inst_name));
    /* Takes effect only in MMU EB   PRT scheduler */
    SHR_IF_ERR_EXIT(th4_tdm_set_min_spacing(unit, sched_inst_name));
    /* Takes effect only in MMU EB   PRT scheduler */
    SHR_IF_ERR_EXIT(th4_tdm_set_urg_cell_spacing(unit, sched_inst_name));
    /* Takes effect only in PKT SCH*/
    SHR_IF_ERR_EXIT(th4_tdm_set_pksch_pkt_credits(unit, sched_inst_name));

    if (sched_inst_name == TDM_MMU_EB_PORT_SCHED) {
        SHR_IF_ERR_EXIT(th4_tdm_set_ebctm_min_spacing(unit));
        SHR_IF_ERR_EXIT(th4_tdm_set_ebpcc_tct_speed(unit));
        SHR_IF_ERR_EXIT(th4_tdm_set_ebpcc_tct_portdeficit(unit));
    }

    /* Pick the valid pipes */
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));

    /* Per pipe configs; at init time only; not part of FlexPort */
    for (pipe = 0; pipe < TH4_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            /* Takes effect only in IDB/MMU_EB PRT SCHED*/
            SHR_IF_ERR_EXIT(th4_tdm_set_max_spacing(unit, sched_inst_name, pipe));
            SHR_IF_ERR_EXIT(th4_tdm_set_feature_ctrl(unit,sched_inst_name, pipe));
            SHR_IF_ERR_EXIT(th4_tdm_set_cbmg_value(unit, sched_inst_name, pipe));
        }
    }

exit:
    SHR_FUNC_EXIT();

}

int
th4_mmu_tdm_general_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (th4_mmu_tdm_sched_general_init(unit, TDM_MMU_MAIN_SCHED));
    SHR_IF_ERR_EXIT
        (th4_mmu_tdm_sched_general_init(unit, TDM_MMU_EB_PORT_SCHED));
    SHR_IF_ERR_EXIT
        (th4_mmu_tdm_sched_general_init(unit, TDM_MMU_EB_PKT_SCHED));

exit:
    SHR_FUNC_EXIT();
}

static int
th4_tdm_port_config(int unit,
                       int lport,
                       mmu_sched_inst_name_e sched_inst_name,
                       int ports_down)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);


    /* Configs slots allocated to this port from scheduler. */
    SHR_IF_ERR_EXIT
        (th4_tdm_set_cal_config(unit,
                                lport,
                                sched_inst_name,
                                ports_down));
    /* Configs port from mmu to phy mapping. */
    SHR_IF_ERR_EXIT
        (th4_tdm_set_mmu_port_to_pport_mapping(unit,
                                                  lport,
                                                  sched_inst_name,
                                                  ports_down));
    /* Configs port_speed in EB SHAPER. Only front panel ports */
    if (bcmtm_lport_is_fp(unit, lport)) {
        SHR_IF_ERR_EXIT
            (th4_tdm_set_ebshp_port_cfg(unit,
                                        lport,
                                        sched_inst_name,
                                        ports_down));
    }

    if (ports_down == 1) { /* remove ports from scheduler. */
        /* Clears EDB to MMU credits counter. Only when port is removed */
        SHR_IF_ERR_EXIT
            (th4_tdm_clear_edb_credit_counter_per_port(unit,
                                                       lport,
                                                       sched_inst_name,
                                                       ports_down));
    } else {               /* Add port to scheduler. */
        /* Configure per port EB credits. Only when port is added */
        SHR_IF_ERR_EXIT
            (th4_tdm_set_eb_credit_config(unit,
                                          lport,
                                          sched_inst_name,
                                          ports_down));
    }

 exit:
    SHR_FUNC_EXIT();
}

int
th4_mmu_tdm_port_delete(int unit, int lport)
{
    int down;

    SHR_FUNC_ENTER(unit);

    /* Remove DOWN ports. */
    down = 1;
    SHR_IF_ERR_EXIT
        (th4_tdm_port_config(unit, lport, TDM_MMU_MAIN_SCHED, down));
    SHR_IF_ERR_EXIT
        (th4_tdm_port_config(unit, lport, TDM_MMU_EB_PORT_SCHED, down));
    SHR_IF_ERR_EXIT
        (th4_tdm_port_config(unit, lport, TDM_MMU_EB_PKT_SCHED, down));

 exit:
    SHR_FUNC_EXIT();

}

int
th4_mmu_tdm_port_add(int unit, int lport)
{
    int down;

    SHR_FUNC_ENTER(unit);

    /* Add UP ports. */
    down = 0;
    SHR_IF_ERR_EXIT
        (th4_tdm_port_config(unit, lport, TDM_MMU_MAIN_SCHED, down));
    SHR_IF_ERR_EXIT
        (th4_tdm_port_config(unit, lport, TDM_MMU_EB_PORT_SCHED, down));
    SHR_IF_ERR_EXIT
        (th4_tdm_port_config(unit, lport, TDM_MMU_EB_PKT_SCHED, down));

 exit:
    SHR_FUNC_EXIT();

}

int
bcm56990_a0_tm_tdm_mmu_init(int unit)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (th4_mmu_tdm_general_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_mmu_port_clear_edb_credit_counter(
    int unit,
    bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int ports_down, lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    ports_down = 1;
    lport = port_map->pport_map[pport].lport;
    SHR_IF_ERR_EXIT
        (th4_tdm_clear_edb_credit_counter_per_port(unit,
                                                   lport,
                                                   TDM_MMU_EB_PORT_SCHED,
                                                   ports_down));

exit:
    SHR_FUNC_EXIT();
}

/*** END SDKLT API COMMON CODE ***/
