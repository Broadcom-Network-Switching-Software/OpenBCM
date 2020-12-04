/*! \file bcm56880_a0_tm_ct.c
 *
 * File containing cut-through related defines and internal function for
 * bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <bcmbd/bcmbd.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmtm/generated/bcmtm_ha.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include "bcm56880_a0_tm_edb_flexport.h"
#include <bcmtm/chip/bcm56880_a0_tm.h>
#include "bcm56880_a0_tm_ct.h"

/*******************************************************************************
 * Local definitions
 */
#define BCM56880_A0_CT_VALID_SPEED_COUNT   7
#define BCM56880_A0_SAF_COMMIT_ID_COUNT   16
#define BCM56880_A0_CT_MAX_FACTOR          4
#define BCM56880_A0_CT_MAX_SPEED         400000
#define BCM56880_A0_CT_CLASS_VALID_NUM     8

#define BCM56880_A0_CT_SPEED_CLASS_VALIDATE(class) \
    if (class > BCM56880_A0_CT_CLASS_VALID_NUM) \
        SHR_ERR_EXIT(SHR_E_PARAM);

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

typedef enum bcm56880_a0_pipe_bandwidth_config_e {
    PIPE_BANDWIDTH_CONFIG_400G = 0,
    PIPE_BANDWIDTH_CONFIG_50G_400G = 1,
    PIPE_BANDWIDTH_CONFIG_16X100G = 2,
    PIPE_BANDWIDTH_CONFIG_OTHER = 3,
    PIPE_BANDWIDTH_CONFIG_MAX = 4
}bcm56880_a0_pipe_bandwidth_config_t;

typedef struct
bcm56880_a0_saf_commit_config_s {
    uint16_t saf_commit_id[BCM56880_A0_SAF_COMMIT_ID_COUNT];
}bcm56880_a0_saf_commit_config_t;

static const
bcm56880_a0_saf_commit_config_t
bcm56880_a0_saf_commit_config_tbl[PIPE_BANDWIDTH_CONFIG_MAX] = {
    /* 4X400GE */
    {{0, 0, 0, 4, 4, 4, 4, 8, 8, 8, 8, 12, 12, 12, 12, 16}},
    /* Mixed 50GE, 400GE */
    {{1, 2, 3, 3, 4, 5, 6, 6, 7, 9, 11, 13, 15, 17, 19, 21}},
    /* 16x100GE */
    {{4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20}},
    /* others */
    {{1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13}}
};

typedef enum bcm56880_a0_ct_speed_encoding_e {
    BCM56880_A0_SPEED_ID_RESERVED = 0,
    BCM56880_A0_SPEED_ID_10G,
    BCM56880_A0_SPEED_ID_25G,
    BCM56880_A0_SPEED_ID_40G,
    BCM56880_A0_SPEED_ID_50G,
    BCM56880_A0_SPEED_ID_100G,
    BCM56880_A0_SPEED_ID_200G,
    BCM56880_A0_SPEED_ID_400G,
    BCM56880_A0_SPEED_ID_ERROR,
}bcm56880_a0_ct_speed_enconding_t;

/*******************************************************************************
 * Private functions
 */
static int
bcm56880_a0_tm_ct_speed_encoding_get(int port_speed)
{
    switch (port_speed) {
        case 10000:
            return BCM56880_A0_SPEED_ID_10G;
        case 25000:
            return BCM56880_A0_SPEED_ID_25G;
        case 40000:
            return BCM56880_A0_SPEED_ID_40G;
        case 50000:
            return BCM56880_A0_SPEED_ID_50G;
        case 100000:
        /* for handling max src port speed for 40G */
        case 160000:
            return BCM56880_A0_SPEED_ID_100G;
        case 200000:
            return BCM56880_A0_SPEED_ID_200G;
        case 400000:
            return BCM56880_A0_SPEED_ID_400G;
        default:
            return BCM56880_A0_SPEED_ID_ERROR;
    }
}

static int
bcm56880_a0_tm_ct_rl_config_set(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf[2] = {0, 0};
    static struct bcm56880_a0_mmu_rl_config_s {
        uint32_t buffer_activity;
        uint32_t high_thd;
        uint32_t ebq_backup_thd;
        uint32_t ebq_backup;
        uint32_t stop_cooloff;
    }rl_config = {
        1024, 5120, 64, 128, 512
    };

    SHR_FUNC_ENTER(unit);
    /* MMU_RL_CT_TILE_ACTIVITY */
    sid = MMU_RL_CT_TILE_ACTIVITYr;

    fid = STOP_DUE_TO_EBQ_BACKUPf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, ltmbuf, &(rl_config.ebq_backup)));

    fid = STOP_DUE_TO_BUFFER_ACTIVITYf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, ltmbuf, &(rl_config.buffer_activity)));

    fid = EBQ_BACKUP_THRESHOLDf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, ltmbuf, &(rl_config.ebq_backup_thd)));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(
        bcmtm_pt_indexed_write(unit, sid, -1, (void*)&pt_dyn_info, ltmbuf));

    /* MMU_RL_CT_TILE_ACTIVITY2 */
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    sid = MMU_RL_CT_TILE_ACTIVITY2r;

    fid = STOP_COOLOFFf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, ltmbuf, &(rl_config.stop_cooloff)));

    fid = HIGH_ACTIVITY_THf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, ltmbuf, &(rl_config.high_thd)));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(
        bcmtm_pt_indexed_write(unit, sid, -1, (void*)&pt_dyn_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_ct_fifo_thd_set(int unit)
{
    bcmdrd_sid_t sid = MMU_EBCTM_CT_SPEED_CFGr;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid = CT_FIFO_THRESHOLDf;
    uint32_t ltmbuf = 0;
    uint32_t fval[] = {16, 16, 16, 16, 24, 38, 66};
    int idx;

    SHR_FUNC_ENTER(unit);
    for (idx = 0; idx < BCM56880_A0_CT_VALID_SPEED_COUNT; idx++) {
        ltmbuf = 0;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, &ltmbuf, &(fval[idx])));

        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, 0)
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, -1,
                (void*)&pt_dyn_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_ebpts_ebshp_config_set(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[2] = { 0 };

    bcmdrd_sid_t sid_list0[] = { MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_0r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_1r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_2r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_3r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_4r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_5r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_6r };

    bcmdrd_sid_t sid_list1[] = { MMU_EBPTS_EBSHP_PORT_SPEED_CFG1_0r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG1_1r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG1_2r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG1_3r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG1_4r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG1_5r,
                                 MMU_EBPTS_EBSHP_PORT_SPEED_CFG1_6r };

    static struct bcm56880_a0_ebpts_ebshp_config_s {
        uint32_t no_demand;
        uint32_t excess_thd;
        uint16_t refresh_bytes_lag_mode;
        uint32_t lag_thd;
        uint32_t exit_lag_thd;
    } ebpts_ebshp_config[BCM56880_A0_CT_VALID_SPEED_COUNT] = {
        {  7394,  7514,  20, 6769,  8019},
        {  7394,  7694,  50, 6769,  8019},
        { 10752, 11052,  80, 10127, 11377},
        { 10752, 11052, 100, 10127, 11377},
        { 21842, 22442, 200, 20592, 23092},
        { 39988, 41188, 399, 37488, 42488},
        { 76616, 79016, 799, 71616, 81616},
    };
    int idx;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    for (idx = 0; idx < BCM56880_A0_CT_VALID_SPEED_COUNT; idx++) {
        sid = sid_list0[idx];
        /* MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_x */
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, -1, &pt_dyn_info, ltmbuf));
        fid = EXCESS_THDf;
        fval = ebpts_ebshp_config[idx].excess_thd;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
        fid = NO_DEMAND_LEVELf;
        fval = ebpts_ebshp_config[idx].no_demand;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
        fid = REFRESH_BYTESf;
        fval = ebpts_ebshp_config[idx].refresh_bytes_lag_mode;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, -1, &pt_dyn_info, ltmbuf));

        /* MMU_EBPTS_EBSHP_PORT_SPEED_CFG1_X */
        sid = sid_list1[idx];
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));

        fid = LAGGING_THDf;
        fval = ebpts_ebshp_config[idx].lag_thd;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
        fid = EXIT_LAGGING_THDf;
        fval = ebpts_ebshp_config[idx].exit_lag_thd;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, -1, &pt_dyn_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_tct_config_set(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[2] = {0, 0};
    static struct bcm56880_a0_tct_config_s {
        uint32_t itm_pkt_thd;
        uint32_t mmu_pkt_thd;
        uint32_t itm_cell_thd;
        uint32_t mmu_cell_thd;
        uint32_t cell_reduction_cycle;
        uint32_t cell_reduction_num;
        uint32_t exit_tct_timer;
        uint32_t exit_cell_thd;
    } tct_config [BCM56880_A0_CT_VALID_SPEED_COUNT] = {
        {16, 16, 38, 38, 4096, 8, 32000, 76},
        {16, 16, 38, 38, 4096, 8, 32000, 76},
        {16, 16, 38, 38, 4096, 8, 32000, 76},
        {16, 16, 38, 38, 4096, 8, 32000, 76},
        {31, 31, 38, 38, 4096, 8, 32000, 76},
        {62, 62, 62, 62, 4096, 8, 16000, 100},
        {124, 124, 124, 124, 4096, 8, 8000, 162},
    };
    int idx;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);
    for (idx = 0; idx < BCM56880_A0_CT_VALID_SPEED_COUNT; idx++) {
        /*
         * Configuration settings to enter Transtion CT state for the given
         * speed.
         */
        sid = MMU_EBCTM_TCT_ENTER_SPEED_CFGr;
        sal_memset( ltmbuf, 0, sizeof(ltmbuf));

        fid = ENTER_TCT_ITM_PKT_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[idx].itm_pkt_thd)));
        fid = ENTER_TCT_MMU_PKT_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[idx].mmu_pkt_thd)));
        fid =  ENTER_TCT_ITM_CELL_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[idx].itm_cell_thd)));
        fid = ENTER_TCT_MMU_CELL_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[idx].mmu_cell_thd)));

        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, 0);
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, -1, (void*)&pt_dyn_info, ltmbuf));
        /*
         * Configurations settings to exit Transtion CT state for the given
         * speed.
         */
        sid = MMU_EBCTM_TCT_EXIT_SPEED_CFGr;
        sal_memset( ltmbuf, 0, sizeof(ltmbuf));
        fid = TCT_BUFFER_CELL_REDUCTION_CYCLESf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                &tct_config[idx].cell_reduction_cycle));
        fid = TCT_BUFFER_CELL_REDUCTION_NUMBERf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                &tct_config[idx].cell_reduction_num));
        fid = EXIT_TCT_TIMERf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                &tct_config[idx].exit_tct_timer));
        fid = EXIT_TCT_MMU_CELL_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                &tct_config[idx].exit_cell_thd));

        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, 0);
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, -1, (void*)&pt_dyn_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_ct_pipe_config(int unit, int pipe, int *pipe_config)
{
    int port_speed;
    bcmpc_lport_t lport;
    struct pipe_config {
        int num_50g;
        int num_100g;
        int num_200g;
        int num_400g;
    } p_config = {0, 0, 0, 0};
    bcmtm_port_map_info_t *port_map;
    int pport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    *pipe_config = PIPE_BANDWIDTH_CONFIG_OTHER;

    for (lport = 0; lport < TD4_DEV_PORTS_PER_DEV; lport++) {
        pport = port_map->lport_map[lport].pport;
        if ((pport == -1) || (pipe != port_map->lport_map[lport].pipe)) {
            continue;
        }
        /* configurations for fp ports only. */
        if (!(bcmtm_lport_is_fp(unit, lport))) {
            continue;
        }

        /* get the port speed */
        port_speed = port_map->lport_map[lport].max_speed;
        switch (port_speed) {
            /* 50G */
            case 50000:
                p_config.num_50g++;
                break;
                /* 100G */
            case 100000:
                p_config.num_100g++;
                break;
                /* 200G */
            case 200000:
                p_config.num_200g++;
                break;
                /* 400G */
            case 400000:
                p_config.num_400g++;
                break;
            default:
                break;
        }
    }
    if (p_config.num_400g == 4) {
        *pipe_config = PIPE_BANDWIDTH_CONFIG_400G; /* 4x400 GE */
    } else if (p_config.num_100g == 16) {
        *pipe_config = PIPE_BANDWIDTH_CONFIG_16X100G; /* 16x100 GE */
    } else if ((p_config.num_400g == 2) && (p_config.num_50g == 16) &&
               (p_config.num_100g == 0) && (p_config.num_200g == 0)) {
        *pipe_config = PIPE_BANDWIDTH_CONFIG_50G_400G; /* 16x50GE and 2x400GE */
    }
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_saf_commitment_config_set(int unit, int pipe)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf = 0;
    int idx, pipe_config;
    const bcm56880_a0_saf_commit_config_t *saf_config;

    SHR_FUNC_ENTER(unit);

    /* Store and Forward bandwidth reservation commitment configurations. */
    sid = MMU_EBCTM_CT_BUDGET_SAF_COMMITMENT_CFGr;
    fid = CTBUDGET_SAFCOMMITMENTf;

    SHR_IF_ERR_EXIT(bcm56880_a0_tm_ct_pipe_config(unit, pipe, &pipe_config));
    saf_config = &(bcm56880_a0_saf_commit_config_tbl[pipe_config]);

    for (idx = 0; idx < BCM56880_A0_SAF_COMMIT_ID_COUNT; idx++) {
        SHR_IF_ERR_EXIT(
                bcmtm_field_set(unit, sid, fid, &ltmbuf,
                    (uint32_t *) &(saf_config->saf_commit_id[idx])));
        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, pipe);
        SHR_IF_ERR_EXIT(
           bcmtm_pt_indexed_write(unit, sid, -1, (void*)&pt_dyn_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_ct_port_enable(int unit,
                              bcmlrd_sid_t ltid,
                              bcmpc_lport_t lport,
                              int port_speed,
                              uint32_t ct_enable)
{
    uint32_t dst_speed_encode, max_src_speed_encode;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf;
    int mport;
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    dst_speed_encode = bcm56880_a0_tm_ct_speed_encoding_get(port_speed);
    max_src_speed_encode = bcm56880_a0_tm_ct_speed_encoding_get(
            MIN((port_speed * BCM56880_A0_CT_MAX_FACTOR), BCM56880_A0_CT_MAX_SPEED));

    BCM56880_A0_CT_SPEED_CLASS_VALIDATE(dst_speed_encode);
    BCM56880_A0_CT_SPEED_CLASS_VALIDATE(max_src_speed_encode);

    /* CT configuration for each Front panel port in EB */
    sid = MMU_EBCTM_EPORT_CT_CFGr;
    mport =  port_map->lport_map[lport].mport;
    if (mport == -1) {
        /* logical port to physical port mapping not found. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
    SHR_IF_ERR_EXIT(
        bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    fid = DST_PORT_SPEEDf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, &ltmbuf, &dst_speed_encode));
    fid = MAX_SRC_PORT_SPEEDf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, &ltmbuf, &max_src_speed_encode));
    fid = CT_ENABLEf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, &ltmbuf, &ct_enable));
    SHR_IF_ERR_EXIT(
        bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Enable/disable the cut-through mode for the given logical port.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   lport      Logical port.
 * \param [in]   ct_enable  Enable/disable.
 *
 * \retval SHR_E_NONE       No error.
 * \retval SHR_E_NOT_FOUND  Port not found.
 * \retval SHR_E_PARAM      Cut-through speed class invalid.
 */
static int
bcm56880_a0_tm_ct_port_mode_set(int unit,
                                bcmlrd_sid_t ltid,
                                bcmpc_lport_t lport,
                                uint32_t ct_enable)
{
    int port_speed;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmtm_lport_max_speed_get(unit, lport, &port_speed));

    if (ct_enable) {
        /* Cut Through Enable*/
        SHR_IF_ERR_EXIT(
            bcm56880_a0_tm_ct_port_enable(unit, ltid, lport,
                port_speed, ct_enable));
    } else {
        /* Disable Cut-Through */
        SHR_IF_ERR_EXIT(
            bcm56880_a0_tm_ct_port_enable(unit, ltid, lport,
                port_speed, ct_enable));
    }
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56880_a0_tm_ct_port_info_get(int unit,
                                bcmlrd_sid_t ltid,
                                uint64_t flags,
                                bcmpc_lport_t lport,
                                bcmtm_ct_port_info_t *ct_info)
{
    bcmdrd_sid_t sid;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf, fval, dst_speed;
    int mport, pport, pipe;
    int idx, local_idx;
    uint32_t port_speed_map[] = { 0, 10000, 25000, 40000,
                                50000, 100000, 200000, 400000};

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.flags = flags;

    sid = MMU_EBCTM_EPORT_CT_CFGr;
    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mport));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, MAX_SRC_PORT_SPEEDf, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, DST_PORT_SPEEDf, &ltmbuf, &dst_speed));
    ct_info->src_port_max_speed = port_speed_map[fval];

    sid = EDB_IP_CUT_THRU_CLASSm;
    BCMTM_PT_DYN_INFO(pt_dyn_info, lport, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &fval));
    ct_info->ct_class = fval;

    sid = EGR_MMU_CELL_CREDITm;
    SHR_IF_ERR_EXIT
        (bcmtm_lport_pport_get(unit, lport, &pport));
    BCMTM_PT_DYN_INFO(pt_dyn_info, pport, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    ct_info->max_credit_cells = ltmbuf;

    /* transmit count for EDB in bytes.*/
    sid = EDB_XMIT_START_COUNTm;
    local_idx = (lport % TD4_DEV_PORTS_PER_PIPE) * TD4_EDB_MAX_CT_CLASSES;
    SHR_IF_ERR_EXIT
        (bcmtm_lport_pipe_get(unit, lport, &pipe));
    for (idx = 0; idx < MAX_TM_CUT_THROUGH_CLASS; idx++) {
        BCMTM_PT_DYN_INFO(pt_dyn_info, local_idx, pipe);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, THRESHOLDf, &ltmbuf, &fval));

        /* 16B granularity. */
        ct_info->xmit_start_cnt[idx] = fval * 16;
        local_idx++;
    }

    sid = MMU_EBCTM_CT_SPEED_CFGr;
    /* 0th index is reserved. */
    if (dst_speed > 0) {
        BCMTM_PT_DYN_INFO(pt_dyn_info, (dst_speed -1), 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
        ct_info->fifo_thd_cells = ltmbuf;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcm56880_a0_tm_ct_drv_get(int unit, void *ct_drv)
{
    bcmtm_ct_drv_t bcm56880_a0_ct_drv = {
        .port_set = bcm56880_a0_tm_ct_port_mode_set,
        .port_info_get = bcm56880_a0_tm_ct_port_info_get,
    };
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ct_drv, SHR_E_INTERNAL);
    *((bcmtm_ct_drv_t *)ct_drv) = bcm56880_a0_ct_drv;
exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_ct_init(int unit)
{
    uint32_t pipe_map;
    int pipe = 0;
    int idx, mport_offset = 0;
    bcmdrd_sid_t sid = MMU_EBCTM_EPORT_CT_CFGr;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf = 0;

    SHR_FUNC_ENTER(unit);

    /* read launcher configurations. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_ct_rl_config_set(unit));

    /* CT FIFO threshold configurations. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_ct_fifo_thd_set(unit));

    /* Transition CT configurations. */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_tct_config_set(unit));

    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_ebpts_ebshp_config_set(unit));

    /* SAF commitment configurations. */
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
    while (pipe_map) {
        if (pipe_map & 1) {
            SHR_IF_ERR_EXIT
                (bcm56880_a0_tm_saf_commitment_config_set(unit, pipe));
            mport_offset = TD4_MMU_PORTS_PER_PIPE * pipe;
            for (idx = 0; idx < TD4_FP_PORTS_PER_PIPE; idx++) {
                BCMTM_PT_DYN_INFO(pt_info, 0, (idx + mport_offset));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, &ltmbuf));
            }
        }
        pipe_map >>= 1;
        pipe++;
    }
exit:
    SHR_FUNC_EXIT();
}
