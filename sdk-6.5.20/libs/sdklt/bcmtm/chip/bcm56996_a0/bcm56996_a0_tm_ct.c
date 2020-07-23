/*! \file bcm56996_a0_tm_ct.c
 *
 * File containing cut-through related defines and internal function for
 * bcm56996_a0.
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
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/generated/bcmtm_ha.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include "bcm56996_a0_tm_edb_flexport.h"
#include <bcmtm/chip/bcm56996_a0_tm.h>
#include "bcm56996_a0_tm_ct.h"

/*******************************************************************************
 * Local definitions
 */
#define BCM56996_A0_CT_VALID_SPEED_COUNT   7
#define BCM56996_A0_SAF_COMMIT_ID_COUNT   16
#define BCM56996_A0_CT_MAX_FACTOR          4
#define BCM56996_A0_CT_MAX_SPEED         400000
#define BCM56996_A0_CT_CLASS_VALID_NUM     8

#define BCM56996_A0_CT_SPEED_CLASS_VALIDATE(class) \
    if (class > BCM56996_A0_CT_CLASS_VALID_NUM) \
        SHR_ERR_EXIT(SHR_E_PARAM);

typedef enum bcm56996_a0_pipe_bandwidth_config_e {
    PIPE_BANDWIDTH_CONFIG_400G = 0,
    PIPE_BANDWIDTH_CONFIG_50G_400G = 1,
    PIPE_BANDWIDTH_CONFIG_16X100G = 2,
    PIPE_BANDWIDTH_CONFIG_OTHER = 3,
    PIPE_BANDWIDTH_CONFIG_MAX = 4
}bcm56996_a0_pipe_bandwidth_config_t;

typedef struct
bcm56996_a0_saf_commit_config_s {
    uint16_t saf_commit_id[BCM56996_A0_SAF_COMMIT_ID_COUNT];
}bcm56996_a0_saf_commit_config_t;

static const
bcm56996_a0_saf_commit_config_t
bcm56996_a0_saf_commit_config_tbl = {
    {1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13}
};

typedef enum bcm56996_a0_ct_speed_encoding_e {
    BCM56996_A0_SPEED_ID_RESERVED = 0,
    BCM56996_A0_SPEED_ID_10G,
    BCM56996_A0_SPEED_ID_25G,
    BCM56996_A0_SPEED_ID_40G,
    BCM56996_A0_SPEED_ID_50G,
    BCM56996_A0_SPEED_ID_100G,
    BCM56996_A0_SPEED_ID_200G,
    BCM56996_A0_SPEED_ID_400G,
    BCM56996_A0_SPEED_ID_ERROR,
}bcm56996_a0_ct_speed_enconding_t;

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP
/*******************************************************************************
 * Private functions
 */
static int
bcm56996_a0_tm_ct_speed_encoding_get(int port_speed)
{
    switch (port_speed) {
        case 10000:
            return BCM56996_A0_SPEED_ID_10G;
        case 25000:
            return BCM56996_A0_SPEED_ID_25G;
        case 40000:
            return BCM56996_A0_SPEED_ID_40G;
        case 50000:
            return BCM56996_A0_SPEED_ID_50G;
        case 100000:
        /* for handling max src port speed for 40G */
        case 160000:
            return BCM56996_A0_SPEED_ID_100G;
        case 200000:
            return BCM56996_A0_SPEED_ID_200G;
        case 400000:
            return BCM56996_A0_SPEED_ID_400G;
        default:
            return BCM56996_A0_SPEED_ID_ERROR;
    }
}

static int
bcm56996_a0_tm_ct_rl_config_set(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf[2] = {0, 0};
    static struct bcm56996_a0_mmu_rl_config_s {
        uint32_t buffer_activity;
        uint32_t high_thd;
        uint32_t ebq_backup_thd;
        uint32_t ebq_backup;
        uint32_t stop_cooloff;
    }rl_config = {
        1024, 9216, 82, 1280, 512
    };

    SHR_FUNC_ENTER(unit);
    /* MMU_RL_CT_TILE_ACTIVITY */
    sid = MMU_RL_CT_TILE_ACTIVITYr;

    fid = STOP_DUE_TO_EBFF_BACKUPf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, ltmbuf, &(rl_config.ebq_backup)));

    fid = STOP_DUE_TO_BUFFER_ACTIVITYf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, ltmbuf, &(rl_config.buffer_activity)));

    fid = EBFF_BACKUP_THRESHOLDf;
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
bcm56996_a0_tm_ct_fifo_thd_set(int unit)
{
    bcmdrd_sid_t sid = MMU_EBCTM_CT_SPEED_CFGr;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid = CT_FIFO_THRESHOLDf;
    uint32_t ltmbuf = 0;
    uint32_t fval[3][BCM56996_A0_CT_VALID_SPEED_COUNT] = {
        /* frequence 950 */
        {25, 25, 25, 25, 37, 60, 107},
        /* frequence 1050 */
        {24, 24, 24, 24, 35, 57, 100},
        /* frequence 1325 */
        {21, 21, 21, 21, 29, 46, 80}
    };
    int idx, freq_id;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    freq_id = (drv_info->frequency < 1025) ? 0:
              (drv_info->frequency < 1325) ? 1: 2;

    for (idx = 0; idx < BCM56996_A0_CT_VALID_SPEED_COUNT; idx++) {
        ltmbuf = 0;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, &ltmbuf, &(fval[freq_id][idx])));

        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, 0);
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, -1,
                (void*)&pt_dyn_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_tm_ebpts_ebshp_config_set(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[2] = { 0 };

    bcmdrd_sid_t sid_list[] = { MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_0r,
                                MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_1r,
                                MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_2r,
                                MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_3r,
                                MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_4r,
                                MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_5r,
                                MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_6r };

    /* Throughput loss detection mode (lagging mode) */
    uint16_t refresh_bytes_lag_mode[] = {20, 50, 80, 99, 199, 399, 799};
    int idx;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t fval = 0;

    SHR_FUNC_ENTER(unit);

    fid = REFRESH_BYTESf;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    for (idx = 0; idx < BCM56996_A0_CT_VALID_SPEED_COUNT; idx++) {
        sid = sid_list[idx];
        /* MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_x */
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, -1, &pt_dyn_info, ltmbuf));
        fid = REFRESH_BYTESf;
        fval = refresh_bytes_lag_mode[idx];
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, -1, &pt_dyn_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_tm_tct_config_set(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[2] = {0, 0};
    bcmtm_drv_info_t *drv_info;
    static struct bcm56996_a0_tct_config_s {
        uint32_t itm_pkt_thd;
        uint32_t mmu_pkt_thd;
        uint32_t itm_cell_thd1;
        uint32_t mmu_cell_thd1;
        uint32_t itm_cell_thd2;
        uint32_t mmu_cell_thd2;
        uint32_t cell_reduction_cycle;
        uint32_t cell_reduction_num;
        uint32_t exit_tct_timer;
        uint32_t exit_cell_thd;
    } tct_config [3][BCM56996_A0_CT_VALID_SPEED_COUNT] = {
        { /*   frequency 950 */
         {9,  9,  37,  37,  68,  68,  4096, 8, 8000, 105},
         {9,  9,  37,  37,  68,  68,  4096, 8, 8000, 105},
         {9,  9,  37,  37,  68,  68,  4096, 8, 8000, 105},
         {9,  9,  37,  37,  68,  68,  4096, 8, 8000, 105},
         {13, 13, 72,  72,  98,  98,  4096, 8, 8000, 135},
         {18, 18, 117, 117, 112, 112, 4096, 8, 8000, 154},
         {40, 40, 208, 208, 140, 140, 4096, 8, 8000, 245}},
        { /*   frequency 1050 */
         {8,  8,  33,  33,  65,  65,  4096, 8, 8000, 102},
         {8,  8,  33,  33,  65,  65,  4096, 8, 8000, 102},
         {8,  8,  33,  33,  65,  65,  4096, 8, 8000, 102},
         {8,  8,  33,  33,  65,  65,  4096, 8, 8000, 102},
         {12, 12, 65,  65,  93,  93,  4096, 8, 8000, 130},
         {17, 17, 106, 106, 105, 105, 4096, 8, 8000, 143},
         {36, 36, 188, 188, 131, 131, 4096, 8, 8000, 225}},
        { /*   frequency 1325 */
         {7,  7,  27,  27,  59,  59,  4096, 8, 8000, 96},
         {7,  7,  27,  27,  59,  59,  4096, 8, 8000, 96},
         {7,  7,  27,  27,  59,  59,  4096, 8, 8000, 96},
         {7,  7,  27,  27,  59,  59,  4096, 8, 8000, 96},
         {10, 10, 52,  52,  81,  81,  4096, 8, 8000, 118},
         {14, 14, 84,  84,  91,  91,  4096, 8, 8000, 128},
         {30, 30, 149, 149, 111, 111, 4096, 8, 8000, 186}},
    };
    int idx, freq_id;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    freq_id = (drv_info->frequency < 1025) ? 0:
              (drv_info->frequency < 1325) ? 1: 2;
    for (idx = 0; idx < BCM56996_A0_CT_VALID_SPEED_COUNT; idx++) {
        /*
         * Configuration settings to enter Transtion CT state for the given
         * speed.
         */
        sid = MMU_EBCTM_TCT_ENTER_SPEED_CFG_1r;
        sal_memset( ltmbuf, 0, sizeof(ltmbuf));
        fid = ENTER_TCT_ITM_PKT_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].itm_pkt_thd)));
        fid =  ENTER_TCT_ITM_CELL_THRESHOLD_1f;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].itm_cell_thd1)));
        fid =  ENTER_TCT_ITM_CELL_THRESHOLD_2f;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].itm_cell_thd2)));
        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, 0);
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, -1, (void*)&pt_dyn_info, ltmbuf));

        sid = MMU_EBCTM_TCT_ENTER_SPEED_CFG_0r;
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        fid = ENTER_TCT_MMU_PKT_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].mmu_pkt_thd)));
        fid = ENTER_TCT_MMU_CELL_THRESHOLD_1f;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].mmu_cell_thd1)));
        fid = ENTER_TCT_MMU_CELL_THRESHOLD_2f;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].mmu_cell_thd2)));

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
                &tct_config[freq_id][idx].cell_reduction_cycle));
        fid = TCT_BUFFER_CELL_REDUCTION_NUMBERf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                &tct_config[freq_id][idx].cell_reduction_num));
        fid = EXIT_TCT_TIMERf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                &tct_config[freq_id][idx].exit_tct_timer));
        fid = EXIT_TCT_MMU_CELL_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                &tct_config[freq_id][idx].exit_cell_thd));

        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, 0);
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, -1, (void*)&pt_dyn_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_tm_saf_commitment_config_set(int unit, int pipe)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf = 0;
    int idx;
    const bcm56996_a0_saf_commit_config_t *saf_config;

    SHR_FUNC_ENTER(unit);

    /* Store and Forward bandwidth reservation commitment configurations. */
    sid = MMU_EBCTM_CT_BUDGET_SAF_COMMITMENT_CFGr;
    fid = CTBUDGET_SAFCOMMITMENTf;

    saf_config = &bcm56996_a0_saf_commit_config_tbl;

    for (idx = 0; idx < BCM56996_A0_SAF_COMMIT_ID_COUNT; idx++) {
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
bcm56996_a0_tm_ct_port_enable(int unit,
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

    dst_speed_encode = bcm56996_a0_tm_ct_speed_encoding_get(port_speed);
    max_src_speed_encode = bcm56996_a0_tm_ct_speed_encoding_get(
            MIN((port_speed * BCM56996_A0_CT_MAX_FACTOR), BCM56996_A0_CT_MAX_SPEED));

    BCM56996_A0_CT_SPEED_CLASS_VALIDATE(dst_speed_encode);
    BCM56996_A0_CT_SPEED_CLASS_VALIDATE(max_src_speed_encode);

    /* CT configuration for each Front panel port in EB */
    sid = MMU_EBCTM_EPORT_CT_CFGr;
    mport = port_map->lport_map[lport].mport;
    if (mport == -1) {
        /* logical port to physical port mapping not found. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info, &ltmbuf));
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
            bcmtm_pt_indexed_write(unit, sid, ltid, (void*)&pt_dyn_info, &ltmbuf));

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
bcm56996_a0_tm_ct_port_mode_set(int unit,
                                bcmlrd_sid_t ltid,
                                bcmpc_lport_t lport,
                                uint32_t ct_enable)
{
    int port_speed;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmtm_lport_max_speed_get(unit, lport, &port_speed));

    /* Bubble Mop Settings done in IDB flexport API. */
    if (ct_enable) {
        /* Cut Through Enable*/
        SHR_IF_ERR_EXIT(
            bcm56996_a0_tm_ct_port_enable(unit, ltid, lport,
                port_speed, ct_enable));
    } else {
        /* Disable Cut-Through */
        SHR_IF_ERR_EXIT(
            bcm56996_a0_tm_ct_port_enable(unit, ltid, lport,
                port_speed, ct_enable));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_tm_ct_port_info_get(int unit,
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
    local_idx = (lport % TH4G_DEV_PORTS_PER_PIPE) * TH4G_EDB_MAX_CT_CLASSES;
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
bcm56996_a0_tm_ct_drv_get(int unit, void *ct_drv)
{
    bcmtm_ct_drv_t bcm56996_a0_ct_drv = {
        .port_set = bcm56996_a0_tm_ct_port_mode_set,
        .port_info_get = bcm56996_a0_tm_ct_port_info_get,
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ct_drv, SHR_E_INTERNAL);
    *((bcmtm_ct_drv_t *)ct_drv) = bcm56996_a0_ct_drv;
exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_ct_init(int unit)
{
    uint32_t pipe_map;
    int pipe = 0;
    int idx, mport_offset = 0;
    bcmdrd_sid_t sid = MMU_EBCTM_EPORT_CT_CFGr;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf = 0;

    SHR_FUNC_ENTER(unit);

    /* read launcher configurations. */
    SHR_IF_ERR_EXIT(bcm56996_a0_tm_ct_rl_config_set(unit));

    /* CT FIFO threshold configurations. */
    SHR_IF_ERR_EXIT(bcm56996_a0_tm_ct_fifo_thd_set(unit));

    /* Transition CT configurations. */
    SHR_IF_ERR_EXIT(bcm56996_a0_tm_tct_config_set(unit));

    SHR_IF_ERR_EXIT
        (bcm56996_a0_tm_ebpts_ebshp_config_set(unit));

    /* SAF commitment configurations. */
    SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
    while (pipe_map) {
        if (pipe_map & 1) {
            SHR_IF_ERR_EXIT(
                bcm56996_a0_tm_saf_commitment_config_set(unit, pipe));
            mport_offset = TH4G_MMU_PORTS_PER_PIPE * pipe;
            for (idx = 0; idx < TH4G_FP_PORTS_PER_PIPE; idx++) {
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
