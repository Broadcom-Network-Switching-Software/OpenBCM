/*! \file bcm56996_a0_tm_edb_flexport.c
 *
 * File containing flexport sequence for bcm56996_a0.
 *
 * The code in this file is shared with DV team. This file will only be updated
 * by porting DV code changes. If any extra change needs to be made to the
 * shared code, please also inform the DV team.
 *
 * DV file: $DV/trident4/dv/ngsdk_interface/flexport/edb_flexport.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmtm/chip/bcm56996_a0_tm.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include "bcm56996_a0_tm_edb_flexport.h"

#define BSL_LOG_MODULE  BSL_LS_BCMTM_FLEXPORT
/*** START SDKLT API COMMON CODE ***/

/***************************************************************************
 * LOCAL CONSTANT VARIABLES
 */

/* ASF Core Config Table: SAF & CT XMIT_CNT and EP2MMU credit  */
/*
 * 1) ep2mmu credit: pick "PFC Opt 1325MHz" and "General case".
 *    Note that "PFC Opt 1325MHz" covers both 1325MHz and 1500MHz.
 * 2) xmit_start_cnt SAF: pick "PFC Opt 1325MHz" and "General case"
 * 3) xmit_start_cnt CT : pick "PFC Opt 1325MHz" and "General case"
 */

static const th4g_edb_e2e_setting_t
th4g_edb_e2e_tbl[TH4G_EDB_E2E_TBL_ENTRIES] = {
   /* speed                       | ct   | ep2mmu_credit    |  xmit_cnt {saf, ct}       |
    *                             | class| general | pfc_opt|  general | pfc_opt_1325MHz|
    */
    {     0,                         0,     0,       0,     {{  0,   0}, { 0,  0}} },   /*  SAF  */
    { 10000,                         1,    25,       6,     {{  6,  24}, { 4, 22}} },   /* 10GE  */
    { 25000,                         2,    54,      13,     {{ 13,  35}, {10, 30}} },   /* 25GE  */
    { 40000,                         3,    75,      21,     {{ 21,  47}, {15, 39}} },   /* 40GE  */
    { 50000,                         4,    54,      26,     {{ 26,  86}, {19, 75}} },   /* 50GE  */
    {100000,                         5,    54,      51,     {{ 30,  41}, {22, 30}} },   /* 100GE */
    {200000,                         6,   110,      82,     {{ 35,  53}, {25, 39}} },   /* 200GE */
    {400000,                         7,   207,     142,     {{ 29,  52}, {29, 57}} },   /* 400GE */
    {TH4G_EDB_E2E_TBL_SPD_LPBK,       8,    57,      57,     {{  1,   1}, { 1,  1}} },   /* LPBK  */
    {TH4G_EDB_E2E_TBL_SPD_CMIC,       9,    25,      15,     {{  1,   1}, { 1,  1}} },   /* CMIC  */
    {TH4G_EDB_E2E_TBL_SPD_MGMT_10G,  10,     7,       5,     {{ 26,  26}, {16, 16}} },   /* MGMT_10G */
    {TH4G_EDB_E2E_TBL_SPD_MGMT_40G,  11,    27,      17,     {{101, 101}, {61, 61}} }    /* MGMT_40G */
};

/* Speed encoding index in th4_edb_e2e_tbl for special ports */
#define TH4G_EDB_E2E_LPBK_SPD_ENCODE         8
#define TH4G_EDB_E2E_CMIC_SPD_ENCODE         9
#define TH4G_EDB_E2E_MGMT_10G_SPD_ENCODE     10
#define TH4G_EDB_E2E_MGMT_40G_SPD_ENCODE     11
/***************************************************************************
 * LOCAL WRAPPER FUNCTIONS
 */
static int
th4g_edb_pt_write(int unit, int pt_id, int pt_inst, bcmdrd_sid_t mem,
                 bcmdrd_fid_t fid, uint32_t fval)
{
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t     ltmbuf[2] = {0};

    SHR_FUNC_ENTER(unit);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "edb_flexport_api\n")));
    BCMTM_PT_DYN_INFO(pt_info, pt_id, pt_inst);
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, mem, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, mem, ltid, &pt_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
th4g_edb_pt_read(int unit, int pt_id, int pt_inst, bcmdrd_sid_t mem,
                bcmdrd_fid_t fid, uint32_t *fval)
{
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[2] = {0};
    uint32_t fval_tmp;

    SHR_FUNC_ENTER(unit);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "edb_flexport_api\n")));
    BCMTM_PT_DYN_INFO(pt_info, pt_id, pt_inst);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, mem, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_get(unit, mem, fid, ltmbuf, &fval_tmp));
    *fval = fval_tmp;

exit:
    SHR_FUNC_EXIT();
}



/***************************************************************************
 * LOCAL FUNCTIONS
 */
/*!
 * \brief Get index of table th4g_edb_e2e_tbl by given lport.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 *
 * \retval Pre-configured speed encoding value.
 */
static int
th4g_edb_e2e_tbl_speed_encoding_get(int unit, int lport)
{
    int idx, speed = 0;
    int spd_encoding = 0;
    bcmtm_port_map_info_t *port_map;

    bcmtm_port_map_get(unit, &port_map);

    /* LPBK */
    if (bcmtm_lport_is_lb(unit, lport)) {
        return TH4G_EDB_E2E_LPBK_SPD_ENCODE;
    }
    /* CPU */
    else if (bcmtm_lport_is_cpu(unit, lport)) {
        return TH4G_EDB_E2E_CMIC_SPD_ENCODE;
    }
    /* MGM: 10G or 40G */
    else if (bcmtm_lport_is_mgmt(unit, lport)) {
        if (port_map->lport_map[lport].cur_speed == 40000) {
            return TH4G_EDB_E2E_MGMT_40G_SPD_ENCODE;
        } else {
            return TH4G_EDB_E2E_MGMT_10G_SPD_ENCODE;
        }
    }

    /* FP */
    if (bcmtm_lport_is_fp(unit, lport)) {
        speed = port_map->lport_map[lport].cur_speed;
    }
    for (idx = 0; idx < TH4G_EDB_E2E_TBL_ENTRIES; idx++) {
        if (th4g_edb_e2e_tbl[idx].speed >= speed) {
            spd_encoding = idx;
            break;
        }
    }

    return spd_encoding;
}

/*!
 * \brief Get index of xmit_cnt[] in table th4g_edb_e2e_tbl.
 *
 * \param [in] unit: Device unit.
 *
 * \retval frequency encoding value.
 */
static int
th4g_edb_e2e_tbl_opt_encoding_get(int unit)
{
    bcmtm_drv_info_t *drv_info;
    int core_freq, dpp_freq;
    int opt_encoding = 0;

    bcmtm_drv_info_get(unit, &drv_info);
    core_freq = drv_info->frequency;
    dpp_freq  = drv_info->dpr_clock_frequency;

    if ((core_freq == 1325 && dpp_freq == 1325)) {
        opt_encoding = 1;
    }

    return (opt_encoding % TH4G_EDB_E2E_NUM_PFC_OPT_CFG);
}

/*!
 * \brief Get CT class of the given port in th4g_edb_e2e_tbl.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 *
 * \retval Pre-configured CT class.
 */
static int
th4g_edb_e2e_tbl_ct_class_get(int unit, int lport)
{
    int spd_encoding, ct_class;

    spd_encoding = th4g_edb_e2e_tbl_speed_encoding_get(unit, lport);
    ct_class = th4g_edb_e2e_tbl[spd_encoding].ct_class;

    return ct_class;
}

/*!
 * \brief Get ep2mmu credit of the given port from th4g_edb_e2e_tbl.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 *
 * \retval Pre-configured port's ep2mmu credit.
 */
static int
th4g_edb_e2e_tbl_mmu_credit_get(int unit, int lport)
{
    int spd_encoding, opt_encoding;
    int credit = 0;

    spd_encoding = th4g_edb_e2e_tbl_speed_encoding_get(unit, lport);
    opt_encoding = th4g_edb_e2e_tbl_opt_encoding_get(unit);

    if (opt_encoding == 0) {
        /* General cases */
        credit = th4g_edb_e2e_tbl[spd_encoding].mmu_credit;
    } else {
        /* PFC Optimized cases for 1325MHz */
        credit = th4g_edb_e2e_tbl[spd_encoding].mmu_credit_pfc_opt;
    }

    return credit;
}

/*!
 * \brief Get ep2mmu credit of a given speed.
 *
 * \param [in] unit: Device unit.
 * \param [in] speed: speed.
 *
 * \retval Pre-configured port's ep2mmu credit.
 */
int
th4g_ep_get_ep2mmu_credit_per_speed(int unit, int speed, uint32_t *credit)
{
    int spd_encoding, opt_encoding;
    int idx;

    spd_encoding = 1;
    for (idx = 0; idx < TH4G_EDB_E2E_TBL_ENTRIES; idx++) {
        if (th4g_edb_e2e_tbl[idx].speed >= speed) {
            spd_encoding = idx;
            break;
        }
    }

    opt_encoding = th4g_edb_e2e_tbl_opt_encoding_get(unit);

    if (opt_encoding == 0) {
        /* General cases */
        *credit = th4g_edb_e2e_tbl[spd_encoding].mmu_credit;
    } else {
        /* PFC Optimized cases for 1325MHz */
        *credit = th4g_edb_e2e_tbl[spd_encoding].mmu_credit_pfc_opt;
    }

    return SHR_E_NONE;
}


/*!
 * \brief Get SAF or CT xmit_cnt from th4g_edb_e2e_tbl.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 * \param [in] ct_valid: ct valid. 0->saf xmit_cnt, otherwise->ct xmit_cnt.
 *
 * \retval Pre-configured xmit_cnt value.
 */
static int
th4g_edb_e2e_tbl_xmit_cnt_get(int unit, int lport, int ct_valid)
{
    int spd_encoding, opt_encoding;
    int xmit_cnt = 0;

    opt_encoding = th4g_edb_e2e_tbl_opt_encoding_get(unit);
    spd_encoding = th4g_edb_e2e_tbl_speed_encoding_get(unit, lport);

    /* FP */
    if (bcmtm_lport_is_fp(unit, lport)) {
        if (ct_valid == 0) {
            xmit_cnt = th4g_edb_e2e_tbl[spd_encoding].xmit_cnt[opt_encoding].saf;
        } else {
            xmit_cnt = th4g_edb_e2e_tbl[spd_encoding].xmit_cnt[opt_encoding].ct;
        }
    } else if (bcmtm_lport_is_mgmt(unit, lport)) {
        /* MGM: 10G or 40G SAF */
        xmit_cnt = th4g_edb_e2e_tbl[spd_encoding].xmit_cnt[opt_encoding].saf;
    } else if (bcmtm_lport_is_lb(unit, lport) ||
               bcmtm_lport_is_cpu(unit, lport)) {
        /* LPBK and CPU: low threshold, always SAF */
        xmit_cnt = 1;
    }

    return xmit_cnt;
}

/*!
 * \brief Drain EDB port buffer by polling num_entries to 0.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_edb_drain_port(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg, mem;
    bcmdrd_fid_t fid;
    uint32_t fval_saved;
    uint32_t fval = 0;
    int pport, pipe_num;
    int pt_id, pt_inst;
    int timeout_us = 0;
    uint32_t num_entries;
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    int timeout_reached;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    pport    = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d\n"),
               lport, pport));

    /*
     * Configure EGR_EDB_MISC_CTRL such that EGR_MAX_USED_ENTRIES will return
     * the current buffer utilization in terms of number of used_entries.
     */
    pt_id   = 0;
    pt_inst = pipe_num;
    reg     = EDB_MISC_CTRLr;
    fid     = SELECT_CURRENT_USED_ENTRIESf;
    fval    = 1;

    SHR_IF_ERR_EXIT(th4g_edb_pt_read(unit, pt_id, pt_inst, reg, fid, &fval_saved));
    SHR_IF_ERR_EXIT(th4g_edb_pt_write(unit, pt_id, pt_inst, reg, fid, fval));

    /* Poll port's buffer utilization until is 0. */
    pt_id   = pport;
    pt_inst = 0;
    mem     = EGR_MAX_USED_ENTRIESm;
    fid     = LEVELf;

    timeout_reached = 0;
    do {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "edb_flexport_api: lport=%3d, pport=%3d, timeout_us=%0d\n"),
                   lport, pport, timeout_us));
        /*  Check number of used entries of port buffer */
        SHR_IF_ERR_EXIT
            (th4g_edb_pt_read(unit, pt_id, pt_inst, mem, fid, &num_entries));
        if (num_entries == 0) break;

        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);

        /* Check timeout. */
        if (timeout_us++ > EDB_DRAIN_TIMEOUT_US) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "timeout (%0d us), pport=%0d, num_entries=%0d\n"),
                       timeout_us, pport, num_entries));
            timeout_reached = 1;
        }
    } while (num_entries > 0);

    /* Configure EDB_MISC_CTRL back to previous value. */
    reg     = EDB_MISC_CTRLr;
    fid     = SELECT_CURRENT_USED_ENTRIESf;
    pt_id   = 0;
    pt_inst = pipe_num;
    fval    = fval_saved;

    SHR_IF_ERR_EXIT(th4g_edb_pt_write(unit, pt_id, pt_inst, reg, fid, fval));

    if (timeout_reached == 1) {
        return SHR_E_TIMEOUT;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable or disable port in EDB.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Disable port when setting to 1, otherwise enable port.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_edb_port_enable_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pport;
    int pt_id, pt_inst;

    SHR_FUNC_ENTER(unit);

    /* Enable or disable port : write EDB_ENABLEm.PRT_ENABLEf. */
    bcmtm_port_map_get(unit, &port_map);
    pport   = port_map->lport_map[lport].pport;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d, rst_on=%0d\n"),
               lport, pport, rst_on));

    pt_id   = pport;
    pt_inst = 0;
    mem     = EGR_ENABLEm;
    fid     = PRT_ENABLEf;
    fval    = (rst_on == 1) ? 0 : 1;

    SHR_IF_ERR_EXIT(th4g_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Soft reset for EDB per port buffer.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset port buffer when setting to 1, otherwise release reset.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_edb_port_buf_sft_rst_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pport;
    int pt_id, pt_inst;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    pport   = port_map->lport_map[lport].pport;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d, rst_on=%0d\n"),
               lport, pport, rst_on));

    pt_id   = pport;
    pt_inst = 0;
    mem     = EGR_PER_PORT_BUFFER_SFT_RESETm;
    fid     = ENABLEf;
    fval    = (rst_on == 1) ? 1 : 0;

    SHR_IF_ERR_EXIT(th4g_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set/clear per port EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGm.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset dev_to_phy mapping value to unused token (default)
 *                    when setting to 1, otherwise set to proper physical port.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_edb_dev2phy_mapping_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pport;
    int pt_id, pt_inst;

    SHR_FUNC_ENTER(unit);

    if (rst_on == 1) {
        pport = 0x1FF;
    } else {
        bcmtm_port_map_get(unit, &port_map);
        pport = port_map->lport_map[lport].pport;
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d\n"),
               lport, pport));

    pt_id   = lport;
    pt_inst = 0;
    mem     = EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGm;
    fid     = DATAf;
    fval    = pport;

    SHR_IF_ERR_EXIT(th4g_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get EDB port's ep2mmu credit.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 *
 * \retval EP2MMU port credit.
 */
static int
th4g_edb_ep2mmu_port_credit_get(int unit, int lport)
{
    int credit = 0;

    /* FP ports */
    if (bcmtm_lport_is_fp(unit, lport)) {
        credit = th4g_edb_e2e_tbl_mmu_credit_get(unit, lport);
    }
    /* AUX ports */
    else if (bcmtm_lport_is_lb(unit, lport) ||
             bcmtm_lport_is_cpu(unit, lport) ||
             bcmtm_lport_is_mgmt(unit, lport)) {
        credit = th4g_edb_e2e_tbl_mmu_credit_get(unit, lport);
    }

    return credit;
}

/*!
 * \brief Get port xmit start cnt settings.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 * \param [out] xmit_cnt: ptr to port's xmit start cnt array.
 * \param [in] xmit_cnt_size: size of port's xmit start cnt array.
 *
 * \retval 0.
 */
static int
th4g_edb_port_xmit_cnt_array_get(int unit, int lport,
                                int *xmit_cnt, int xmit_cnt_size)
{
    int ct_class, src_ct_class;
    int saf_xmit_cnt, ct_xmit_cnt;

    for (ct_class = 0; ct_class < xmit_cnt_size; ct_class++) {
        xmit_cnt[ct_class] = 0;
    }

    if (bcmtm_lport_is_fp(unit, lport)) {
        /* FP */
        saf_xmit_cnt = th4g_edb_e2e_tbl_xmit_cnt_get(unit, lport, 0);
        ct_xmit_cnt  = th4g_edb_e2e_tbl_xmit_cnt_get(unit, lport, 1);

        for (src_ct_class = 0; src_ct_class < xmit_cnt_size; src_ct_class++) {
            xmit_cnt[src_ct_class] = (src_ct_class == 0) ?
                                     saf_xmit_cnt : ct_xmit_cnt;
        }
    } else if (bcmtm_lport_is_lb(unit, lport)  ||
               bcmtm_lport_is_cpu(unit, lport) ||
               bcmtm_lport_is_mgmt(unit, lport)) {
        /* LPBK, CPU and MGM */
        xmit_cnt[0] = th4g_edb_e2e_tbl_xmit_cnt_get(unit, lport, 0);
    }

    return 0;
}

/*!
 * \brief Set/clear EDB mmu_cell_credit per port.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset port credit to zero when setting to 1,
 *                    otherwise set to proper value
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_edb_mmu_cell_credit_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pport;
    int pt_id, pt_inst;
    int credit;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    pport  = port_map->lport_map[lport].pport;
    credit = (rst_on == 1) ? 0 : (th4g_edb_ep2mmu_port_credit_get(unit, lport));

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d, credit=%0d\n"),
               lport, pport, credit));

    pt_id   = pport;
    pt_inst = 0;
    mem     = EGR_MMU_CELL_CREDITm;
    fid     = CREDITf;
    fval    = credit;

    SHR_IF_ERR_EXIT(th4g_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set EDB CT class per port.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset port ct_class to zero when setting to 1,
 *                    otherwise set ct_class properly based on port speed.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_edb_ct_class_set(int unit, int lport, int rst_on)
{
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pt_id, pt_inst;
    int ct_class;

    SHR_FUNC_ENTER(unit);

    /* Only Front Panel ports have CT configured. */
    if (!bcmtm_lport_is_fp(unit, lport)) {
        return 0;
    }

    /*
     * Note: FlexPort doesn't need to reset the entry when port goes down;
     * needs to be touched when port goes up.
     */
    ct_class = (rst_on == 1) ? 0 : (th4g_edb_e2e_tbl_ct_class_get(unit, lport));

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, ct_class=%0d\n"),
               lport, ct_class));

    pt_id   = lport;
    pt_inst = 0;
    mem     = EDB_IP_CUT_THRU_CLASSm;
    fid     = CUT_THRU_CLASSf;
    fval    = ct_class;

    SHR_IF_ERR_EXIT(th4g_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set per port EDB xmit start count.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset per port xmit_cnt values to zero
 *                    when setting to 1, otherwise set to proper values.
 *
 * \note Per port xmit_cnt is an array with size equal to num_ct_classes.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_edb_xmit_start_cnt_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pt_id, pt_inst;
    int ct_class;
    int pport, pipe_num;
    int local_lport;
    int xmit_cnt[TH4G_EDB_MAX_CT_CLASSES];

    SHR_FUNC_ENTER(unit);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "edb_flexport_api: \n")));

    /*
     * Set all XMIT START CNT values for a port.
     * 0->SAF; [1-7]-> CT; 8-15 reserved;
     */
    bcmtm_port_map_get(unit, &port_map);
    pipe_num = port_map->lport_map[lport].pipe;
    pport    = port_map->lport_map[lport].pport;

    if (rst_on == 1) {
        sal_memset(xmit_cnt, 0, TH4G_EDB_MAX_CT_CLASSES * sizeof(int));
    } else {
        th4g_edb_port_xmit_cnt_array_get(unit, lport, xmit_cnt,
                                        TH4G_EDB_MAX_CT_CLASSES);
    }

    /* Write data into PT memory. */
    pt_inst = pipe_num;
    mem     = EDB_XMIT_START_COUNTm;
    fid     = THRESHOLDf;

    for (ct_class = 0; ct_class < TH4G_EDB_MAX_CT_CLASSES; ct_class++) {
        local_lport = lport % TH4G_DEV_PORTS_PER_PIPE;
        pt_id = local_lport * TH4G_EDB_MAX_CT_CLASSES + ct_class;
        fval = xmit_cnt[ct_class];

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "edb_flexport_api: lport=%3d, pport=%3d, xmit_cnt[%02d]=%0d\n"),
                   lport, pport, ct_class, fval));

        SHR_IF_ERR_EXIT(th4g_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * EXTERNAL FUNCTIONS
 */
/*!
 * \brief EDB port-down sequence.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport_bmp Ptr to logical port bitmap.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_port_edb_down(int unit, int lport)
{
    int rst_on = 1;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "th4g_port_edb_down lport=%0d\n"), lport));

    /*
     * Drain EDB per port buffer until empty.
     * 1. Set EDB_EDB_MISC_CTRLr.SELECT_CURRENT_USED_ENTRIESf to 1.
     * 2. Poll until EGR_MAX_USED_ENTRIESm.LEVELf is equal to 0.
     */
    SHR_IF_ERR_EXIT(th4g_edb_drain_port(unit, lport));

    /*
     * Disable EDB to MMU cell request generation.
     * 1. set EDB_ENABLEm.PRT_ENABLEf to 0.
     * 2. set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 1.
     */
    SHR_IF_ERR_EXIT(th4g_edb_port_enable_set(unit, lport, rst_on));
    SHR_IF_ERR_EXIT(th4g_edb_port_buf_sft_rst_set(unit, lport, rst_on));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * EXTERNAL FUNCTIONS
 */
/*!
 * \brief EDB port remove sequence.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport_bmp Ptr to logical port bitmap.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_port_edb_invalidate(int unit, int lport)
{
    int rst_on = 1;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "th4g_port_edb_invalidate lport=%0d\n"), lport));
    /*
     * Configure port related property.
     * 1. Delete Dev_to_Phy mapping.
     * 2. Reset max Ep2MMU credits to 0.
     */
    SHR_IF_ERR_EXIT(th4g_edb_dev2phy_mapping_set(unit, lport, rst_on));
    SHR_IF_ERR_EXIT(th4g_edb_mmu_cell_credit_set(unit, lport, rst_on));

exit:
    SHR_FUNC_EXIT();
}




/*!
 * \brief EDB port add sequence: configure up-ports.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport_bmp Ptr to logical port bitmap.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_port_edb_add(int unit, int lport)
{
    int rst_on = 0;
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "th4g_port_edb_add lport=%0d\n"), lport));

    /*
     * Configure port properties for UP ports.
     * 1. Map Dev to Phy for added port.
     * 2. Configure max Ep2MMU port credits.
     * 3. Configure CT_class for added port.
     * 4. Configure XMIT_START_CNT entries for added port.
     */
    SHR_IF_ERR_EXIT(th4g_edb_dev2phy_mapping_set(unit, lport, rst_on));
    SHR_IF_ERR_EXIT(th4g_edb_mmu_cell_credit_set(unit, lport, rst_on));
    SHR_IF_ERR_EXIT(th4g_edb_ct_class_set(unit, lport, rst_on));
    SHR_IF_ERR_EXIT(th4g_edb_xmit_start_cnt_set(unit, lport, rst_on));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief EDB port-up sequence: enable up-ports
 *
 * \param [in] unit: Device unit.
 * \param [in] lport_bmp Ptr to logical port bitmap.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
th4g_port_edb_up(int unit, int lport)
{
    int rst_on = 0;
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "th4g_port_edb_up lport=%0d\n"), lport));

    /*
     * Toggle EDB port buffer reset (FP ports only),
     * and enable cell request generation in EP.
     * 1.1 Set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 1. (Assert reset)
     * 1.2 Set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 0. (Des-Assert reset)
     * 2.  Set EDB_ENABLEm.PRT_ENABLEf to 1.
     */

    if (bcmtm_lport_is_fp(unit, lport) ||
        bcmtm_lport_is_mgmt(unit, lport) ||
        bcmtm_lport_is_lb(unit, lport)) {
        rst_on = 1;
        SHR_IF_ERR_EXIT(th4g_edb_port_buf_sft_rst_set(unit, lport, rst_on));
        rst_on = 0;
        SHR_IF_ERR_EXIT(th4g_edb_port_buf_sft_rst_set(unit, lport, rst_on));
    }


    SHR_IF_ERR_EXIT(th4g_edb_port_enable_set(unit, lport, rst_on));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * END SDKLT-DV SHARED CODE
 */

/*******************************************************************************
 * BCMTM INTERNAL PUBLIC FUNCTIONS
 */
int
bcm56996_a0_tm_port_edb_down(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    SHR_IF_ERR_EXIT
        (th4g_port_edb_down(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_port_edb_delete(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    SHR_IF_ERR_EXIT
        (th4g_port_edb_invalidate(unit, lport));

exit:
    SHR_FUNC_EXIT();
}


int
bcm56996_a0_tm_port_edb_up(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    SHR_IF_ERR_EXIT
        (th4g_port_edb_up(unit, lport));

exit:
    SHR_FUNC_EXIT();
}


int
bcm56996_a0_tm_port_edb_add(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    SHR_IF_ERR_EXIT
        (th4g_port_edb_add(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

/*** END SDKLT API COMMON CODE ***/
