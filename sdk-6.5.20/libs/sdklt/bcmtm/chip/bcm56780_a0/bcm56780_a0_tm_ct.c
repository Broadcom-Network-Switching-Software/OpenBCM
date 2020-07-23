/*! \file bcm56780_a0_tm_ct.c
 *
 * File containing cut-through related defines and internal function for
 * bcm56780_a0.
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
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmtm/generated/bcmtm_ha.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56780_a0_tm.h>

#include "bcm56780_a0_tm_edb_flexport.h"
#include "bcm56780_a0_tm_ct.h"

/*******************************************************************************
 * Local definitions
 */
#define BCMTM_FREQ_MODE_SUPPORT 4
#define BCM56780_A0_CT_VALID_SPEED_COUNT   7
#define BCM56780_A0_SAF_COMMIT_ID_COUNT   20
#define BCM56780_A0_CT_MAX_FACTOR          4
#define BCM56780_A0_CT_MAX_SPEED         400000
#define BCM56780_A0_CT_CLASS_VALID_NUM     8

#define BCM56780_A0_CT_SPEED_CLASS_VALIDATE(class) \
    if (class > BCM56780_A0_CT_CLASS_VALID_NUM) \
        SHR_ERR_EXIT(SHR_E_PARAM);

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

typedef struct
bcm56780_a0_saf_commit_config_s {
    uint16_t saf_commit_id[BCM56780_A0_SAF_COMMIT_ID_COUNT];
}bcm56780_a0_saf_commit_config_t;

static const
bcm56780_a0_saf_commit_config_t
bcm56780_a0_saf_commit_config_tbl = {
    {1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 7, 8, 8, 9, 10, 11, 11, 12, 12, 13}
};

typedef enum bcm56780_a0_ct_speed_encoding_e {
    BCM56780_A0_SPEED_ID_RESERVED = 0,
    BCM56780_A0_SPEED_ID_10G,
    BCM56780_A0_SPEED_ID_25G,
    BCM56780_A0_SPEED_ID_40G,
    BCM56780_A0_SPEED_ID_50G,
    BCM56780_A0_SPEED_ID_100G,
    BCM56780_A0_SPEED_ID_200G,
    BCM56780_A0_SPEED_ID_400G,
    BCM56780_A0_SPEED_ID_ERROR,
}bcm56780_a0_ct_speed_enconding_t;

/*******************************************************************************
 * Private functions
 */
static int
bcm56780_a0_tm_ct_speed_encoding_get(int port_speed)
{
    switch (port_speed) {
        case 10000:
            return BCM56780_A0_SPEED_ID_10G;
        case 25000:
            return BCM56780_A0_SPEED_ID_25G;
        case 40000:
            return BCM56780_A0_SPEED_ID_40G;
        case 50000:
            return BCM56780_A0_SPEED_ID_50G;
        case 100000:
            return BCM56780_A0_SPEED_ID_100G;
        case 200000:
            return BCM56780_A0_SPEED_ID_200G;
        case 400000:
            return BCM56780_A0_SPEED_ID_400G;
        default:
            return BCM56780_A0_SPEED_ID_ERROR;
    }
}

static int
bcm56780_a0_tm_ct_rl_config_set(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf[2] = {0, 0};
    static struct bcm56780_a0_mmu_rl_config_s {
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

    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT(
        bcmtm_pt_indexed_write(unit, sid, -1, (void*)&pt_info, ltmbuf));

    /* MMU_RL_CT_TILE_ACTIVITY2 */
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    sid = MMU_RL_CT_TILE_ACTIVITY2r;

    fid = STOP_COOLOFFf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, ltmbuf, &(rl_config.stop_cooloff)));

    fid = HIGH_ACTIVITY_THf;
    SHR_IF_ERR_EXIT(
        bcmtm_field_set(unit, sid, fid, ltmbuf, &(rl_config.high_thd)));

    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT(
        bcmtm_pt_indexed_write(unit, sid, -1, (void*)&pt_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_ct_fifo_thd_set(int unit)
{
    bcmdrd_sid_t sid = MMU_EBCTM_CT_SPEED_CFGr;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid = CT_FIFO_THRESHOLDf;
    uint32_t ltmbuf = 0;
    uint32_t fval[BCMTM_FREQ_MODE_SUPPORT][BCM56780_A0_CT_VALID_SPEED_COUNT]= {
        /* Frequency 1350. */
        {18, 18, 18, 18, 25, 40, 71},
        /* Frequency 1175. */
        {20, 20, 20, 20, 28, 45, 80},
        /* Frequency 1025. */
        {22, 22, 22, 22, 31, 51, 92},
        /* Frequency 950. */
        {23, 23, 23, 23, 33, 55, 98},
    };
    int idx, freq_id;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_info_get(unit, &drv_info));
    freq_id = (drv_info->frequency == 950) ? 3 :
              (drv_info->frequency == 1025) ? 2 :
              (drv_info->frequency == 1175) ? 1 : 0;
    for (idx = 0; idx < BCM56780_A0_CT_VALID_SPEED_COUNT; idx++) {
        ltmbuf = 0;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, &ltmbuf, &(fval[freq_id][idx])));

        BCMTM_PT_DYN_INFO(pt_info, idx, 0)
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_ebpts_ebshp_config_set (int unit)
{
    bcmdrd_sid_t sid[] = { MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_0r,
                           MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_1r,
                           MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_2r,
                           MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_3r,
                           MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_4r,
                           MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_5r,
                           MMU_EBPTS_EBSHP_PORT_SPEED_CFG0_6r} ;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[2] = {0};
    /* Throughput loss detection mode (lagging mode) */
    uint16_t refresh_bytes_lag_mode[] = {20, 50, 80, 100, 200, 399, 799};
    bcmtm_pt_info_t pt_info = {0};
    int idx;
    uint32_t fval = 0;

    SHR_FUNC_ENTER(unit);
    fid = REFRESH_BYTESf;
    for (idx = 0; idx < BCM56780_A0_CT_VALID_SPEED_COUNT; idx++) {
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid[idx], -1, &pt_info, ltmbuf));
        fval = refresh_bytes_lag_mode[idx];
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid[idx], fid, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid[idx], -1, &pt_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_tm_tct_config_set(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[2] = {0, 0};
    static struct bcm56780_a0_tct_config_s {
        uint32_t itm_pkt_thd;
        uint32_t mmu_pkt_thd;
        uint32_t itm_cell_thd;
        uint32_t mmu_cell_thd;
        uint32_t cell_reduction_cycle;
        uint32_t cell_reduction_num;
        uint32_t exit_tct_timer;
        uint32_t exit_cell_thd;
    } tct_config [BCMTM_FREQ_MODE_SUPPORT][BCM56780_A0_CT_VALID_SPEED_COUNT] = {
        /* frequency 1350 */
        {
            { 20,  20,  30,  30, 4096, 8, 40000,  60},
            { 20,  20,  30,  30, 4096, 8, 40000,  60},
            { 20,  20,  30,  30, 4096, 8, 40000,  60},
            { 20,  20,  30,  30, 4096, 8, 40000,  60},
            { 40,  40,  40,  40, 4096, 8, 40000,  70},
            { 80,  80,  80,  80, 4096, 8, 40000, 110},
            {161, 161, 161, 161, 4096, 8, 40000, 191}
        },
        /* frequency 1175 */
        {
            { 23,  23,  30,  30, 4096, 8, 40000,  60},
            { 23,  23,  30,  30, 4096, 8, 40000,  60},
            { 23,  23,  30,  30, 4096, 8, 40000,  60},
            { 23,  23,  30,  30, 4096, 8, 40000,  60},
            { 46,  46,  46,  46, 4096, 8, 40000,  76},
            { 92,  92,  92,  92, 4096, 8, 40000, 122},
            {185, 185, 185, 185, 4096, 8, 40000, 215}
        },
        /* frequency 1025 */
        {
            { 26,  26,  30,  30, 4096, 8, 40000,  60},
            { 26,  26,  30,  30, 4096, 8, 40000,  60},
            { 26,  26,  30,  30, 4096, 8, 40000,  60},
            { 26,  26,  30,  30, 4096, 8, 40000,  60},
            { 53,  53,  53,  53, 4096, 8, 40000,  87},
            {106, 106, 106, 106, 4096, 8, 40000, 144},
            {212, 212, 212, 212, 4096, 8, 40000, 259}
        },
        /* frequency 950 */
        {
            { 28,  28,  30,  30, 4096, 8, 40000,  60},
            { 28,  28,  30,  30, 4096, 8, 40000,  60},
            { 28,  28,  30,  30, 4096, 8, 40000,  60},
            { 28,  28,  30,  30, 4096, 8, 40000,  60},
            { 57,  57,  57,  57, 4096, 8, 40000,  87},
            {114, 114, 114, 114, 4096, 8, 40000, 144},
            {229, 229, 229, 229, 4096, 8, 40000, 259}
        },
    };
    int idx, freq_id;
    bcmtm_pt_info_t pt_info = {0};
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_info_get(unit, &drv_info));
    freq_id = (drv_info->frequency == 950) ? 3 :
              (drv_info->frequency == 1025) ? 2 :
              (drv_info->frequency == 1175) ? 1 : 0;

    for (idx = 0; idx < BCM56780_A0_CT_VALID_SPEED_COUNT; idx++) {
        /*
         * Configuration settings to enter Transtion CT state for the given
         * speed.
         */
        sid = MMU_EBCTM_TCT_ENTER_SPEED_CFGr;
        sal_memset( ltmbuf, 0, sizeof(ltmbuf));

        fid = ENTER_TCT_ITM_PKT_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].itm_pkt_thd)));
        fid = ENTER_TCT_MMU_PKT_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].mmu_pkt_thd)));
        fid =  ENTER_TCT_ITM_CELL_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].itm_cell_thd)));
        fid = ENTER_TCT_MMU_CELL_THRESHOLDf;
        SHR_IF_ERR_EXIT(
            bcmtm_field_set(unit, sid, fid, ltmbuf,
                (&tct_config[freq_id][idx].mmu_cell_thd)));

        BCMTM_PT_DYN_INFO(pt_info, idx, 0);
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, ltmbuf));
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

        BCMTM_PT_DYN_INFO(pt_info, idx, 0);
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_saf_commitment_config_set(int unit, int pipe)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf = 0;
    int idx;
    const bcm56780_a0_saf_commit_config_t *saf_config;

    SHR_FUNC_ENTER(unit);

    /* Store and Forward bandwidth reservation commitment configurations. */
    sid = MMU_EBCTM_CT_BUDGET_SAF_COMMITMENT_CFGr;
    fid = CTBUDGET_SAFCOMMITMENTf;

    saf_config = &bcm56780_a0_saf_commit_config_tbl;

    for (idx = 0; idx < BCM56780_A0_SAF_COMMIT_ID_COUNT; idx++) {
        SHR_IF_ERR_EXIT(
                bcmtm_field_set(unit, sid, fid, &ltmbuf,
                    (uint32_t *) &(saf_config->saf_commit_id[idx])));
        BCMTM_PT_DYN_INFO(pt_info, idx, pipe);
        SHR_IF_ERR_EXIT(
           bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_ct_port_enable(int unit,
                              bcmlrd_sid_t ltid,
                              bcmpc_lport_t lport,
                              int port_speed,
                              uint32_t ct_enable)
{
    uint32_t dst_speed_encode, max_src_speed_encode;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf;
    int mport;
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    dst_speed_encode = bcm56780_a0_tm_ct_speed_encoding_get(port_speed);
    max_src_speed_encode = bcm56780_a0_tm_ct_speed_encoding_get(
            MIN((port_speed * BCM56780_A0_CT_MAX_FACTOR), BCM56780_A0_CT_MAX_SPEED));

    BCM56780_A0_CT_SPEED_CLASS_VALIDATE(dst_speed_encode);
    BCM56780_A0_CT_SPEED_CLASS_VALIDATE(max_src_speed_encode);

    mport =  port_map->lport_map[lport].mport;
    if (mport == -1) {
        /* logical port to physical port mapping not found. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    /* CT configuration for each Front panel port in EB */
    sid = MMU_EBCTM_EPORT_CT_CFGr;
    BCMTM_PT_DYN_INFO(pt_info, 0, mport);
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    fid = DST_PORT_SPEEDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &dst_speed_encode));
    fid = MAX_SRC_PORT_SPEEDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &max_src_speed_encode));
    fid = CT_ENABLEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &ct_enable));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

    /* programming TCT cfg register */
    sid = MMU_EBCTM_EPORT_TCT_CFGr;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    fid = TCT_ENABLEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &ct_enable));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
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
bcm56780_a0_tm_ct_port_mode_set(int unit,
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
            bcm56780_a0_tm_ct_port_enable(unit, ltid, lport,
                port_speed, ct_enable));
    } else {
        /* Disable Cut-Through */
        SHR_IF_ERR_EXIT(
            bcm56780_a0_tm_ct_port_enable(unit, ltid, lport,
                port_speed, ct_enable));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM cut-through port information get.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] flags PT read flags.
 * \param [in] lport Logical port ID.
 * \param [out] ct_info Cut-through information.
 */
static int
bcm56780_a0_tm_ct_port_info_get(int unit,
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
    local_idx = (lport % TD4_X9_DEV_PORTS_PER_PIPE) * TD4_X9_EDB_MAX_CT_CLASSES;
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
bcm56780_a0_tm_ct_drv_get(int unit, void *ct_drv)
{
    bcmtm_ct_drv_t bcm56780_a0_ct_drv = {
        .port_set = bcm56780_a0_tm_ct_port_mode_set,
        .port_info_get = bcm56780_a0_tm_ct_port_info_get,
    };
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ct_drv, SHR_E_INTERNAL);
    *((bcmtm_ct_drv_t *)ct_drv) = bcm56780_a0_ct_drv;
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_ct_init(int unit)
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
        (bcm56780_a0_tm_ct_rl_config_set(unit));

    /* CT FIFO threshold configurations. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_ct_fifo_thd_set(unit));

    /* Transition CT configurations. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_tct_config_set(unit));

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_ebpts_ebshp_config_set(unit));

    /* SAF commitment configurations. */
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
    while (pipe_map) {
        if (pipe_map & 1) {
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_saf_commitment_config_set(unit, pipe));
            mport_offset = TD4_X9_MMU_PORTS_PER_PIPE * pipe;
            for (idx = 0; idx < TD4_X9_FP_PORTS_PER_PIPE; idx++) {
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
