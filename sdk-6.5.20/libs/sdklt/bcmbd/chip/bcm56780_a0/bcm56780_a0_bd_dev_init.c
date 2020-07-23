/*! \file bcm56780_a0_bd_dev_init.c
 *
 * Perform basic device initialization.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <sal/sal_sleep.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmbd/bcmbd_config.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_pvt_internal.h>
#include <bcmbd/bcmbd_miim.h>

#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_dev.h>
#include <bcmbd/chip/bcm56780_a0_pvt.h>

#include "bcm56780_a0_drv.h"

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
* Local definitions
 */

#define PIPE_RESET_TIMEOUT_MSEC         50
#define PIPE_RESET_EMUL_TIMEOUT_MSEC    10000

#define NUM_PP_PIPES                    2
#define NUM_LOGICAL_PORTS               80

#define TD4_PIPES_PER_DEV               4

/*******************************************************************************
* Private functions
 */

static int
xlport_pbmp_get(int unit, bcmdrd_pbmp_t *pbmp)
{
    return bcmdrd_dev_blktype_pbmp(unit, BLKTYPE_XLPORT, pbmp);
}

static int
cdport_pbmp_get(int unit, bcmdrd_pbmp_t *pbmp)
{
    return bcmdrd_dev_blktype_pbmp(unit, BLKTYPE_CDPORT, pbmp);
}

static int
dpr_latency_config(int unit)
{
    int ioerr = 0;
    IP_DPR_LATENCY_CONFIGr_t ip_dpr_latency;
    EP_DPR_LATENCY_CONFIGr_t ep_dpr_latency;
    TOP_CORE_PLL_CTRL_0r_t top_core_pll_ctrl_0;
    TOP_PP_PLL_CTRL_0r_t top_pp_pll_0;
    ING_DOI_SER_CONTROL_0r_t ip_doi_ser_ctrl_0;
    ING_DOI_SER_CONTROL_1r_t ip_doi_ser_ctrl_1;
    EGR_DOI_SER_CONTROL_0r_t ep_doi_ser_ctrl_0;
    EGR_DOI_SER_CONTROL_1r_t ep_doi_ser_ctrl_1;
    uint32_t ip_latency;
    uint32_t ep_latency;
    uint32_t core_ndiv;
    uint32_t pp_ndiv;
    uint64_t parity_en = 0;
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    parity_en = bcmbd_config_get(unit, 1,
                                "SER_CONFIG",
                                "SER_ENABLE");

    /* ECC enable.  */
    if (parity_en == 1) {
        ioerr = READ_ING_DOI_SER_CONTROL_0r(unit, &ip_doi_ser_ctrl_0);
        ING_DOI_SER_CONTROL_0r_SLOT_PIPELINE_ECC_ENf_SET(ip_doi_ser_ctrl_0, 1);
        ioerr += WRITE_ING_DOI_SER_CONTROL_0r(unit, ip_doi_ser_ctrl_0);

        ioerr += READ_ING_DOI_SER_CONTROL_1r(unit, &ip_doi_ser_ctrl_1);
        ING_DOI_SER_CONTROL_1r_SLOT_PIPELINE_ECC_ENf_SET(ip_doi_ser_ctrl_1, 1);
        ioerr += WRITE_ING_DOI_SER_CONTROL_1r(unit, ip_doi_ser_ctrl_1);

        ioerr += READ_EGR_DOI_SER_CONTROL_0r(unit, &ep_doi_ser_ctrl_0);
        EGR_DOI_SER_CONTROL_0r_SLOT_PIPELINE_ECC_ENf_SET(ep_doi_ser_ctrl_0, 1);
        ioerr += WRITE_EGR_DOI_SER_CONTROL_0r(unit, ep_doi_ser_ctrl_0);

        ioerr += READ_EGR_DOI_SER_CONTROL_1r(unit, &ep_doi_ser_ctrl_1);
        EGR_DOI_SER_CONTROL_1r_SLOT_PIPELINE_ECC_ENf_SET(ep_doi_ser_ctrl_1, 1);
        ioerr += WRITE_EGR_DOI_SER_CONTROL_1r(unit, ep_doi_ser_ctrl_1);
    }

    ioerr += READ_TOP_CORE_PLL_CTRL_0r(unit, &top_core_pll_ctrl_0);
    core_ndiv = TOP_CORE_PLL_CTRL_0r_NDIV_INTf_GET(top_core_pll_ctrl_0);
    ioerr += READ_TOP_PP_PLL_CTRL_0r(unit, &top_pp_pll_0);
    pp_ndiv = TOP_PP_PLL_CTRL_0r_NDIV_INTf_GET(top_pp_pll_0);

    /* DPR latency.
     * Ref TD4_slot_pipeline_latency.xls
     * -------------------------
     * | Core | PP   |    |    |
     * | Freq | Freq | IP | EP |
     * |------|------|----|----|
     * | 1350 | 1350 | 411| 171|
     * |------|------|----|----|
     * | 1350 | 950  | 612| 248|
     * |------|------|----|----|
     * | 950  | 950  | 411| 171|
     * -------------------------
     * freq<->ndiv ( 1500->116 1350->108,  950<->76)
     */

    if (emul) {
        ip_latency = 983;
        ep_latency = 415;
    } else {
        if ((core_ndiv == 108) && (pp_ndiv == 76)) {
            ip_latency = 726;
            ep_latency = 299;
        } else if ((core_ndiv == 100) && (pp_ndiv == 76)) {
            ip_latency = 562;
            ep_latency = 229;
        } else if ((core_ndiv == 94) && (pp_ndiv == 76)) {
            ip_latency = 524;
            ep_latency = 215;
        } else if ((core_ndiv == 88) && (pp_ndiv == 76)) {
            ip_latency = 487;
            ep_latency = 200;
        } else if ((core_ndiv == 100) && (pp_ndiv == 68)) {
            ip_latency = 635;
            ep_latency = 257;
        } else if ((core_ndiv == 94) && (pp_ndiv == 68)) {
            ip_latency = 593;
            ep_latency = 241;
        } else if ((core_ndiv == 88) && (pp_ndiv == 68)) {
            ip_latency = 552;
            ep_latency = 225;
        } else {
            /* Handle as default. */
            /* 1.350GHz : 1.350GHz */
            /* 950MHz : 950MHz */
            ip_latency = 491;
            ep_latency = 207;
        }
        /* bits [10-1] */
        ip_latency = (ip_latency << 1);
        ep_latency = (ep_latency << 1);
        /* WR_EN */
        ip_latency = ip_latency | 0x1;
        ep_latency = ep_latency | 0x1;
    }

    IP_DPR_LATENCY_CONFIGr_CLR(ip_dpr_latency);
    IP_DPR_LATENCY_CONFIGr_LATENCYf_SET(ip_dpr_latency, ip_latency);
    ioerr += WRITE_IP_DPR_LATENCY_CONFIGr(unit, ip_dpr_latency);

    EP_DPR_LATENCY_CONFIGr_CLR(ep_dpr_latency);
    EP_DPR_LATENCY_CONFIGr_LATENCYf_SET(ep_dpr_latency, ep_latency);
    ioerr += WRITE_EP_DPR_LATENCY_CONFIGr(unit, ep_dpr_latency);

    return ioerr;
}

/* Per hardware requirement, the following must be configured before
 * running mem_reset_all mechanism.
 */
static int
hash_init(int unit)
{
    int ioerr = 0;
    IFTA30_E2T_00_HASH_CONTROLm_t i30_hctl0;
    EFTA30_E2T_00_HASH_CONTROLm_t e30_hctl0;
    IFTA40_E2T_00_HASH_CONTROLm_t i40_hctl0;
    IFTA40_E2T_01_HASH_CONTROLm_t i40_hctl1;
    IFTA80_E2T_00_HASH_CONTROLm_t i80_hctl0;
    IFTA80_E2T_01_HASH_CONTROLm_t i80_hctl1;
    IFTA80_E2T_02_HASH_CONTROLm_t i80_hctl2;
    IFTA80_E2T_03_HASH_CONTROLm_t i80_hctl3;
    IFTA90_E2T_00_HASH_CONTROLm_t i90_hctl0;
    IFTA90_E2T_01_HASH_CONTROLm_t i90_hctl1;
    M_E2T_00_HASH_CONTROLm_t m_hctl0;
    M_E2T_01_HASH_CONTROLm_t m_hctl1;

    if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
        return SHR_E_NONE;
    }

    IFTA30_E2T_00_HASH_CONTROLm_CLR(i30_hctl0);
    EFTA30_E2T_00_HASH_CONTROLm_CLR(e30_hctl0);
    IFTA40_E2T_00_HASH_CONTROLm_CLR(i40_hctl0);
    IFTA40_E2T_01_HASH_CONTROLm_CLR(i40_hctl1);
    IFTA80_E2T_00_HASH_CONTROLm_CLR(i80_hctl0);
    IFTA80_E2T_01_HASH_CONTROLm_CLR(i80_hctl1);
    IFTA80_E2T_02_HASH_CONTROLm_CLR(i80_hctl2);
    IFTA80_E2T_03_HASH_CONTROLm_CLR(i80_hctl3);
    IFTA90_E2T_00_HASH_CONTROLm_CLR(i90_hctl0);
    IFTA90_E2T_01_HASH_CONTROLm_CLR(i90_hctl1);
    M_E2T_00_HASH_CONTROLm_CLR(m_hctl0);
    M_E2T_01_HASH_CONTROLm_CLR(m_hctl1);

    IFTA30_E2T_00_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i30_hctl0, 0x1);
    EFTA30_E2T_00_HASH_CONTROLm_ROBUST_HASH_ENf_SET(e30_hctl0, 0x1);
    IFTA40_E2T_00_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i40_hctl0, 0x1);
    IFTA40_E2T_01_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i40_hctl1, 0x1);
    IFTA80_E2T_00_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i80_hctl0, 0x1);
    IFTA80_E2T_01_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i80_hctl1, 0x1);
    IFTA80_E2T_02_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i80_hctl2, 0x1);
    IFTA80_E2T_03_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i80_hctl3, 0x1);
    IFTA90_E2T_00_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i90_hctl0, 0x1);
    IFTA90_E2T_01_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i90_hctl1, 0x1);
    M_E2T_00_HASH_CONTROLm_ROBUST_HASH_ENf_SET(m_hctl0, 0x1);
    M_E2T_01_HASH_CONTROLm_ROBUST_HASH_ENf_SET(m_hctl1, 0x1);

    IFTA80_E2T_00_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(i80_hctl0, 0x2);
    IFTA80_E2T_01_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(i80_hctl1, 0x2);
    IFTA80_E2T_02_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(i80_hctl2, 0x2);
    IFTA80_E2T_03_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(i80_hctl3, 0x2);

    IFTA30_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(i30_hctl0, 0x3);
    EFTA30_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(e30_hctl0, 0x3);
    IFTA40_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(i40_hctl0, 0x3);
    IFTA40_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(i40_hctl1, 0x3);
    IFTA80_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(i80_hctl0, 0x3);
    IFTA80_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(i80_hctl1, 0x3);
    IFTA80_E2T_02_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(i80_hctl2, 0x3);
    IFTA80_E2T_03_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(i80_hctl3, 0x3);
    IFTA90_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE0f_SET(i90_hctl0, 0x0);
    IFTA90_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE0f_SET(i90_hctl1, 0x0);
    IFTA90_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE1f_SET(i90_hctl0, 0x0);
    IFTA90_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE1f_SET(i90_hctl1, 0x0);
    M_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(m_hctl0, 0xFF);
    M_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(m_hctl1, 0x0);

    ioerr += WRITE_IFTA30_E2T_00_HASH_CONTROLm(unit, 0, i30_hctl0);
    ioerr += WRITE_EFTA30_E2T_00_HASH_CONTROLm(unit, 0, e30_hctl0);
    ioerr += WRITE_IFTA40_E2T_00_HASH_CONTROLm(unit, 0, i40_hctl0);
    ioerr += WRITE_IFTA40_E2T_01_HASH_CONTROLm(unit, 0, i40_hctl1);
    ioerr += WRITE_IFTA80_E2T_00_HASH_CONTROLm(unit, 0, i80_hctl0);
    ioerr += WRITE_IFTA80_E2T_01_HASH_CONTROLm(unit, 0, i80_hctl1);
    ioerr += WRITE_IFTA80_E2T_02_HASH_CONTROLm(unit, 0, i80_hctl2);
    ioerr += WRITE_IFTA80_E2T_03_HASH_CONTROLm(unit, 0, i80_hctl3);
    ioerr += WRITE_IFTA90_E2T_00_HASH_CONTROLm(unit, 0, i90_hctl0);
    ioerr += WRITE_IFTA90_E2T_01_HASH_CONTROLm(unit, 0, i90_hctl1);
    if (bcmdrd_dev_flags_get(unit) & CHIP_FLAG_MTOP) {
        ioerr += WRITE_M_E2T_00_HASH_CONTROLm(unit, 0, m_hctl0);
        ioerr += WRITE_M_E2T_01_HASH_CONTROLm(unit, 0, m_hctl1);
    }

    IFTA90_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE0f_SET(i90_hctl0, 0x0);
    IFTA90_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE0f_SET(i90_hctl1, 0x0);
    IFTA90_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE1f_SET(i90_hctl0, 0x0);
    IFTA90_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE1f_SET(i90_hctl1, 0x0);

    ioerr += WRITE_IFTA90_E2T_00_HASH_CONTROLm(unit, 0, i90_hctl0);
    ioerr += WRITE_IFTA90_E2T_01_HASH_CONTROLm(unit, 0, i90_hctl1);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

/* for clearning up _LP hash memories */
static void
bcm56780_a0_mem_address_encode(int unit, bcmdrd_sid_t sid, int index, int tbl_inst,
                               uint32_t *addr, uint32_t *adext)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bcmdrd_sym_info_t sinfo;
    int blktype, blknum, acctype;
    uint32_t offset, pipe_info, pmask, linst, sect_size;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, &sinfo))) {
        *addr = 0;
        *adext = 0;
        return;
    }

    offset = sinfo.offset;
    index *= sinfo.step_size;

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    acctype = bcmdrd_pt_acctype_get(unit, sid);
    blknum = bcmdrd_chip_block_number(cinfo, blktype, 0);

    pipe_info = bcmbd_cmicx_pipe_info(unit, offset, acctype,
                                      blktype, -1);
    /* pmask > 0 if (access type == UNIQUE && (block type == IP/EP/MMU_ITM/MMU_EB)) */
    pmask = BCMBD_CMICX_PIPE_PMASK(pipe_info);
    /* linst > 0 if (block type == MMU_ITM/MMU_EB/MMU_GLB), indicate the number of base index */
    linst = BCMBD_CMICX_PIPE_LINST(pipe_info);
    sect_size = BCMBD_CMICX_PIPE_SECT_SIZE(pipe_info);

    if (pmask != 0) {
        /* Select a specific block instance for unique access type */
        acctype = tbl_inst;
    }

    if (linst != 0 && sect_size != 0) {
        offset += tbl_inst * sect_size;
    }

    *adext = 0;
    BCMBD_CMICX_ADEXT_BLOCK_SET(*adext, blknum);
    BCMBD_CMICX_ADEXT_ACCTYPE_SET(*adext, acctype);

    *addr = bcmbd_block_port_addr(unit, blknum, -1, offset, index);
}

static int
bcm56780_a0_mem_clear(int unit, bcmdrd_sid_t sid, int start)
{
    uint32_t *wmem = NULL;
    bcmdrd_sym_info_t sinfo;
    uint64_t waddr = 0;
    int wsize = bcmdrd_pt_entry_wsize(unit, sid);
    int count, mem_size = 0, end, tbl_inst = 0;
    uint32_t addr, adext;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmdrd_pt_info_get(unit, sid, &sinfo));
    end = sinfo.index_max;
    count = end - start + 1;
    mem_size = count * wsize * sizeof(uint32_t);

    wmem = bcmdrd_hal_dma_alloc(unit, mem_size, "tblDma", &waddr);
    if (wmem == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(wmem, 0, mem_size);

    bcm56780_a0_mem_address_encode(unit, sid, start, tbl_inst, &addr, &adext);
    SHR_IF_ERR_EXIT(bcmbd_cmicx_mem_range_write(unit, adext, addr, wsize, 0, count, waddr, 0));

exit:
    if (wmem != NULL) {
        bcmdrd_hal_dma_free(unit, mem_size, wmem, waddr);
    }
    SHR_FUNC_EXIT();
}

/* Some memory above 16K require clean up after reset. This is not done by the HW */
static int
bcm56780_a0_lp_clear(int unit)
{
    int i, lp_cnt;
    int ioerr = 0;
    bcmdrd_sid_t lp_sids[] = {
        IFTA90_E2T_00_B0_LPm,
        IFTA90_E2T_00_B1_LPm,
        IFTA90_E2T_00_B2_LPm,
        IFTA90_E2T_00_B3_LPm,
        IFTA90_E2T_01_B0_LPm,
        IFTA90_E2T_01_B1_LPm,
        IFTA90_E2T_01_B2_LPm,
        IFTA90_E2T_01_B3_LPm,
        IFTA90_E2T_00_B1_SINGLEm,
        IFTA90_E2T_00_B0_SINGLEm,
        IFTA90_E2T_00_B2_SINGLEm,
        IFTA90_E2T_00_B3_SINGLEm,
        IFTA90_E2T_01_B1_SINGLEm,
        IFTA90_E2T_01_B0_SINGLEm,
        IFTA90_E2T_01_B2_SINGLEm,
        IFTA90_E2T_01_B3_SINGLEm,
        M_E2T_B0_LPm,
        M_E2T_B1_LPm,
        M_E2T_B2_LPm,
        M_E2T_B3_LPm,
        M_E2T_B4_LPm,
        M_E2T_B5_LPm,
        M_E2T_B6_LPm,
        M_E2T_B7_LPm,
    };

    lp_cnt = COUNTOF(lp_sids);
    for (i = 0; i < lp_cnt; i++) {
        /* Only need to clear index range 16K - max */
        ioerr += bcm56780_a0_mem_clear(unit, lp_sids[i], 0);
    }
    return ioerr;
}

static int
ipep_init(int unit)
{
    int rv;
    int ioerr = 0;
    int in_progress;
    int idx, pipe;
    uint32_t pipe_map, ipep_map;
    int pipe_init_msec;
    bool sim = bcmdrd_feature_is_sim(unit);
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    ING_DII_HW_RESET_CONTROL_0r_t ing_hreset_ctrl_0;
    EGR_DII_HW_RESET_CONTROL_0r_t egr_hreset_ctrl_0;
    DLB_ECMP_HW_RESET_CONTROLr_t  dlb_hreset_ctrl;

    if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
        return SHR_E_NONE;
    }

    /* Initialize IPIPE memory */
    ING_DII_HW_RESET_CONTROL_0r_CLR(ing_hreset_ctrl_0);
    ING_DII_HW_RESET_CONTROL_0r_VALIDf_SET(ing_hreset_ctrl_0, 1);
    ING_DII_HW_RESET_CONTROL_0r_COUNTf_SET(ing_hreset_ctrl_0, 0xFFFF);
    ioerr += WRITE_ING_DII_HW_RESET_CONTROL_0r_ALL(unit, ing_hreset_ctrl_0);

    /* Initialize DLB memory */
    DLB_ECMP_HW_RESET_CONTROLr_CLR(dlb_hreset_ctrl);
    DLB_ECMP_HW_RESET_CONTROLr_RESET_ALLf_SET(dlb_hreset_ctrl, 1);
    DLB_ECMP_HW_RESET_CONTROLr_VALIDf_SET(dlb_hreset_ctrl, 1);
    DLB_ECMP_HW_RESET_CONTROLr_COUNTf_SET(dlb_hreset_ctrl, 0x1FFF);
    ioerr += WRITE_DLB_ECMP_HW_RESET_CONTROLr(unit, dlb_hreset_ctrl);

    /* Initialize EPIPE memory */
    EGR_DII_HW_RESET_CONTROL_0r_CLR(egr_hreset_ctrl_0);
    EGR_DII_HW_RESET_CONTROL_0r_VALIDf_SET(egr_hreset_ctrl_0, 1);
    EGR_DII_HW_RESET_CONTROL_0r_COUNTf_SET(egr_hreset_ctrl_0, 0xFFFF);
    ioerr += WRITE_EGR_DII_HW_RESET_CONTROL_0r_ALL(unit, egr_hreset_ctrl_0);

    /* Wait for IPIPE memory initialization done. */
    rv = bcmdrd_dev_valid_pipes_get(unit, &pipe_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_IPIPE, &ipep_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    in_progress = ipep_map;

    pipe_init_msec = PIPE_RESET_TIMEOUT_MSEC;
    if (emul) {
        /* Emulation requires larger timeout */
        pipe_init_msec = PIPE_RESET_EMUL_TIMEOUT_MSEC;
    }

    for (idx = 0; idx < pipe_init_msec; idx++) {
        if (sim) {
            break;
        }
        for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
            if (in_progress & (1 << pipe)) {
                /* not done yet */
                ioerr += READ_ING_DII_HW_RESET_CONTROL_0r(unit, pipe,
                                                      &ing_hreset_ctrl_0);
                if (ING_DII_HW_RESET_CONTROL_0r_DONEf_GET(ing_hreset_ctrl_0)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (in_progress == 0) {
            break;
        }
        sal_usleep(1000);
    }
    if (idx >= pipe_init_msec) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "ING_HW_RESET timeout. "
                             "pipe_map=%"PRIu32", in_progress=%"PRIu32"\n"),
                  pipe_map, in_progress));
        return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
    }

    /* Wait for DLB_ECMP memory initialization done. */
    for (idx = 0; idx < pipe_init_msec; idx ++) {
        if (sim) {
            break;
        }

        /* not done yet */
        ioerr += READ_DLB_ECMP_HW_RESET_CONTROLr(unit, &dlb_hreset_ctrl);
        if (DLB_ECMP_HW_RESET_CONTROLr_DONEf_GET(dlb_hreset_ctrl)) {
            break;
        }

        sal_usleep(1000);
    }
    if (idx >= pipe_init_msec) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "DLB_ECMP_RESET timeout\n")));
        return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
    }

    /* Wait for EPIPE memory initialization done. */
    in_progress = ipep_map;
    for (idx = 0; idx < pipe_init_msec; idx ++) {
        if (sim) {
            break;
        }
        for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
            if (in_progress & (1 << pipe)) {
                /* not done yet */
                ioerr += READ_EGR_DII_HW_RESET_CONTROL_0r(unit, pipe,
                                                      &egr_hreset_ctrl_0);
                if (EGR_DII_HW_RESET_CONTROL_0r_DONEf_GET(egr_hreset_ctrl_0)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (in_progress == 0) {
            break;
        }
        sal_usleep(1000);
    }
    if (idx >= pipe_init_msec) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "EGR_HW_RESET timeout\n")));
        return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
    }

    ING_DII_HW_RESET_CONTROL_0r_CLR(ing_hreset_ctrl_0);
    ioerr += WRITE_ING_DII_HW_RESET_CONTROL_0r_ALL(unit, ing_hreset_ctrl_0);
    EGR_DII_HW_RESET_CONTROL_0r_CLR(egr_hreset_ctrl_0);
    ioerr += WRITE_EGR_DII_HW_RESET_CONTROL_0r_ALL(unit, egr_hreset_ctrl_0);
    DLB_ECMP_HW_RESET_CONTROLr_CLR(dlb_hreset_ctrl);
    ioerr += WRITE_DLB_ECMP_HW_RESET_CONTROLr(unit, dlb_hreset_ctrl);

    
    if (!(sim || emul)) {
        ioerr += bcm56780_a0_lp_clear(unit);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
bcm56780_a0_tm_mem_init(int unit)
{
    int rv;
    int ioerr = 0;
    IDB_HW_RESET_CONTROLr_t idb_rst_ctl;
    MMU_GLBCFG_MISCCONFIGr_t mmu_misccfg;
    MMU_EBCFP_MXM_TAG_MEMm_t mxm_tag_mem;
    EDB_XMIT_START_COUNTm_t xmit_start_count;
    EDB_IP_CUT_THRU_CLASSm_t ct_class;
    MMU_CFAP_BSTTHRSr_t cfap_bstthd;
    int in_progress;
    int idx, pipe;
    uint32_t pipe_map;
    int pipe_init_msec;
    bool sim = bcmdrd_feature_is_sim(unit);
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    bcmdrd_sym_info_t sinfo;
    uint32_t port_idx;
    uint32_t emul_time_factor = 1;
    rv = bcmdrd_dev_valid_pipes_get(unit, &pipe_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Initialize IDB memory */
    for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            ioerr += READ_IDB_HW_RESET_CONTROLr(unit, pipe, &idb_rst_ctl);
            IDB_HW_RESET_CONTROLr_RESET_ALLf_SET(idb_rst_ctl, 1);
            IDB_HW_RESET_CONTROLr_VALIDf_SET(idb_rst_ctl, 1);
            IDB_HW_RESET_CONTROLr_COUNTf_SET(idb_rst_ctl, 0x3FFFF);
            ioerr += WRITE_IDB_HW_RESET_CONTROLr(unit, pipe, idb_rst_ctl);
        }
    }

    pipe_init_msec = PIPE_RESET_TIMEOUT_MSEC;
    if (emul) {
        /* Emulation requires larger timeout */
        pipe_init_msec = PIPE_RESET_EMUL_TIMEOUT_MSEC;
        emul_time_factor = 10000;
    }

    in_progress = pipe_map;
    for (idx = 0; idx < pipe_init_msec; idx++) {
        if (sim) {
            break;
        }
        for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
            if (in_progress & (1 << pipe)) {
                /* Not done yet */
                ioerr += READ_IDB_HW_RESET_CONTROLr(unit, pipe, &idb_rst_ctl);
                if (IDB_HW_RESET_CONTROLr_DONEf_GET(idb_rst_ctl)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (in_progress == 0) {
            break;
        }
        sal_usleep(1000);
    }
    if (idx >= pipe_init_msec) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "IDB_HW_RESET timeout. "
                             "pipe_map=%"PRIu32", in_progress=%"PRIu32"\n"),
                  pipe_map, in_progress));
        return SHR_E_TIMEOUT;
    }

    /* Enable parity before initializing MMU memory */
    ioerr += READ_MMU_GLBCFG_MISCCONFIGr(unit, &mmu_misccfg);
    MMU_GLBCFG_MISCCONFIGr_ECCP_ENf_SET(mmu_misccfg, 1);
    ioerr += WRITE_MMU_GLBCFG_MISCCONFIGr(unit, mmu_misccfg);
    sal_usleep(20 * emul_time_factor);

    /* Initialize MMU memory */
    MMU_GLBCFG_MISCCONFIGr_INIT_MEMf_SET(mmu_misccfg, 1);
    ioerr += WRITE_MMU_GLBCFG_MISCCONFIGr(unit, mmu_misccfg);
    sal_usleep(20);
    MMU_GLBCFG_MISCCONFIGr_INIT_MEMf_SET(mmu_misccfg, 0);
    ioerr += WRITE_MMU_GLBCFG_MISCCONFIGr(unit, mmu_misccfg);
    sal_usleep(30 * emul_time_factor);

    /* Initialize tag memory */
    MMU_EBCFP_MXM_TAG_MEMm_CLR(mxm_tag_mem);
    MMU_EBCFP_MXM_TAG_MEMm_DATAf_SET(mxm_tag_mem, 1);
    for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            ioerr += WRITE_MMU_EBCFP_MXM_TAG_MEMm(unit, pipe, 0, mxm_tag_mem);
        }
    }

    /* Initialize EDB_XMIT_START_COUNT memory */
    EDB_XMIT_START_COUNTm_CLR(xmit_start_count);

    ioerr += bcmdrd_pt_info_get(unit,
                EDB_XMIT_START_COUNTm, &sinfo);

    for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            for (port_idx = 0; port_idx <= sinfo.index_max; port_idx++) {
                ioerr += WRITE_EDB_XMIT_START_COUNTm(unit, pipe, port_idx, xmit_start_count);
            }
        }
    }

    /* Initialize EDB_IP_CUT_THRU_CLASS memory. */
    EDB_IP_CUT_THRU_CLASSm_CLR(ct_class);
    ioerr += bcmdrd_pt_info_get(unit, EDB_IP_CUT_THRU_CLASSm, &sinfo);
    for (port_idx = sinfo.index_min; port_idx < sinfo.index_max; port_idx++) {
        ioerr += WRITE_EDB_IP_CUT_THRU_CLASSm(unit, port_idx, ct_class);
    }

    /* Update BST CFAP threshold values. */
    MMU_CFAP_BSTTHRSr_CLR(cfap_bstthd);
    MMU_CFAP_BSTTHRSr_BST_THRESHOLDf_SET(cfap_bstthd, 0x7ffff);
    ioerr += WRITE_MMU_CFAP_BSTTHRSr(unit, cfap_bstthd);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
debug_counter_init(int unit)
{
    int ioerr = 0;
    int rv;
    EGR_TIMESTAMP_EGR_1588_LINK_DELAY_64m_t egr_1588_link_delay;
    int port;
    bcmdrd_pbmp_t pbmp;

    rv = bcmdrd_dev_phys_pbmp(unit, &pbmp);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    BCMDRD_PBMP_ITER(pbmp, port) {
        ioerr += READ_EGR_TIMESTAMP_EGR_1588_LINK_DELAY_64m(unit, port, &egr_1588_link_delay);
        EGR_TIMESTAMP_EGR_1588_LINK_DELAY_64m_LINK_DELAYf_SET(egr_1588_link_delay, 0);
        ioerr += WRITE_EGR_TIMESTAMP_EGR_1588_LINK_DELAY_64m(unit, port, egr_1588_link_delay);
    }
    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
port_mib_reset (int unit)
{
    int ioerr = 0;
    int port;
    bcmdrd_pbmp_t pbmp;
    XLPORT_MIB_RESETr_t xlport_mib_reset;
    CDMAC_MIB_COUNTER_CTRLr_t cdmac_mib_ctrl;

    XLPORT_MIB_RESETr_CLR(xlport_mib_reset);
    CDMAC_MIB_COUNTER_CTRLr_CLR(cdmac_mib_ctrl);

    ioerr += xlport_pbmp_get(unit, &pbmp);

    BCMDRD_PBMP_ITER(pbmp, port) {
        XLPORT_MIB_RESETr_CLR_CNTf_SET(xlport_mib_reset, 0xf);
        ioerr += WRITE_XLPORT_MIB_RESETr(unit, xlport_mib_reset, port);
        XLPORT_MIB_RESETr_CLR(xlport_mib_reset);
        ioerr += WRITE_XLPORT_MIB_RESETr(unit, xlport_mib_reset, port);
    }

    ioerr += cdport_pbmp_get(unit, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        ioerr += READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &cdmac_mib_ctrl);
        CDMAC_MIB_COUNTER_CTRLr_CNT_CLEARf_SET(cdmac_mib_ctrl, 1);
        ioerr += WRITE_CDMAC_MIB_COUNTER_CTRLr(unit, port, cdmac_mib_ctrl);
    }
    sal_usleep(1);
    BCMDRD_PBMP_ITER(pbmp, port) {
        ioerr += READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &cdmac_mib_ctrl);
        CDMAC_MIB_COUNTER_CTRLr_CNT_CLEARf_SET(cdmac_mib_ctrl, 0);
        CDMAC_MIB_COUNTER_CTRLr_ENABLEf_SET(cdmac_mib_ctrl, 1);
        ioerr += WRITE_CDMAC_MIB_COUNTER_CTRLr(unit, port, cdmac_mib_ctrl);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
pvt_channel_reset(int unit)
{
    int ioerr = 0;
    TOP_TMON_CHANNELS_CTRL_0r_t tmon_ctl0;
    TOP_PVTMON_CTRL_0r_t pvtmon_ctl0;
    int ix;

    /* Put PVTMON in reset without resetting AVS */
    TOP_PVTMON_CTRL_0r_CLR(pvtmon_ctl0);
    TOP_PVTMON_CTRL_0r_AVS_RESET_Nf_SET(pvtmon_ctl0, 1);
    for (ix = 0; ix < CTRL_REG_CNT - 1; ix++) {
        ioerr += WRITE_TOP_PVTMON_CTRL_0r(unit, ix, pvtmon_ctl0);
    }
    sal_usleep(200);

    /* Reset history min and max register for all sensors. */
    TOP_TMON_CHANNELS_CTRL_0r_CLR(tmon_ctl0);
    ioerr += WRITE_TOP_TMON_CHANNELS_CTRL_0r(unit, tmon_ctl0);
    sal_usleep(200);

    /* Pull PVTMON out of reset */
    TOP_PVTMON_CTRL_0r_RESET_Nf_SET(pvtmon_ctl0, 0x1);
    for (ix = 0; ix < CTRL_REG_CNT; ix++) {
        ioerr += WRITE_TOP_PVTMON_CTRL_0r(unit, ix, pvtmon_ctl0);
    }
    sal_usleep(200);

    /* Restart stick data registers. */
    TOP_TMON_CHANNELS_CTRL_0r_CLR(tmon_ctl0);
    TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MAX_RST_Lf_SET(tmon_ctl0, 0xffff);
    TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MIN_RST_Lf_SET(tmon_ctl0, 0xffff);
    ioerr += WRITE_TOP_TMON_CHANNELS_CTRL_0r(unit, tmon_ctl0);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
pvt_set_hw_reset_threshold(int unit)
{
    int ioerr = 0;
    TOP_PVTMON_HW_RST_THRESHOLDr_t threshold;
    int ix;
    uint32_t temp;

    /* Temp = -0.23751*data + 356.01
     * data = (356.01 - temp) / 0.23751
     *      = (356010 - (temp * 1000)) / 238
     */
    temp = (356010 - (HW_RESET_TEMPERATURE * 1000)) / 238;

    TOP_PVTMON_HW_RST_THRESHOLDr_CLR(threshold);
    TOP_PVTMON_HW_RST_THRESHOLDr_MIN_HW_RST_THRESHOLDf_SET(threshold, temp);
    for (ix = 0; ix < PVT_SENSOR_CNT; ix++) {
        ioerr += WRITE_TOP_PVTMON_HW_RST_THRESHOLDr(unit, ix, threshold);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
pvt_set_intr_threshold(int unit)
{
    int ioerr = 0;
    TOP_PVTMON_INTR_THRESHOLDr_t threshold;
    int ix;
    uint32_t temp;

    TOP_PVTMON_INTR_THRESHOLDr_CLR(threshold);

     /* Temp = -0.23751*data + 356.01
      * data = (356.01 - temp)/0.23751
             = (356010 - (temp * 1000)) / 238
      */
    /* Max threshold is default to chip specified value. */
    TOP_PVTMON_INTR_THRESHOLDr_MAX_THRESHOLDf_SET(threshold,
                                                  SW_MAX_INTERRUPT_THRESHOLD);

    temp = (356010 - (SW_MIN_INTERRUPT_TEMPERATURE * 1000)) / 238;
    TOP_PVTMON_INTR_THRESHOLDr_MIN_THRESHOLDf_SET(threshold, temp);

    for (ix = 0; ix < PVT_SENSOR_CNT; ix++) {
        ioerr += WRITE_TOP_PVTMON_INTR_THRESHOLDr(unit, ix, threshold);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
pvt_enable_hw_reset(int unit, int enable)
{
    int ioerr = 0;
    TOP_TMON_CHANNELS_CTRL_1r_t reg;

    TOP_TMON_CHANNELS_CTRL_1r_CLR(reg);

    /* Configure hardware reset protection only. */
    if (enable) {
        /* AVS sensor is used for voltage tracking, skip interrupt reporting. */
        TOP_TMON_CHANNELS_CTRL_1r_TMON_HW_RST_HIGHTEMP_CTRL_ENf_SET(reg, 0x7fff);
    } else {
        TOP_TMON_CHANNELS_CTRL_1r_TMON_HW_RST_HIGHTEMP_CTRL_ENf_SET(reg, 0);
    }

    ioerr += WRITE_TOP_TMON_CHANNELS_CTRL_1r(unit, reg);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

int
bcm56780_a0_pvt_init(int unit)
{
    int ioerr = 0;
    int rv;
    TOP_PVTMON_HW_RST_STATUSr_t reg;
    TOP_MISC_CONTROL_1r_t misc;
    int ix;
    int chan;
    int reset = 0;

    /* Check if overheat happened from last boot */
    TOP_PVTMON_HW_RST_STATUSr_CLR(reg);
    ioerr += READ_TOP_PVTMON_HW_RST_STATUSr(unit, &reg);
    chan = TOP_PVTMON_HW_RST_STATUSr_MIN_HW_RST_STATUSf_GET(reg);
    for (ix = 0; ix < PVT_SENSOR_CNT; ix++) {
        if (chan & (1 << ix)) {
            reset = 1;
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "HMON Sensor %d experienced overheat reset\n"),
                      ix));

        }
    }

    /* Now clear the sticky bit if needed */
    if (reset) {
        TOP_MISC_CONTROL_1r_CLR(misc);
        ioerr += READ_TOP_MISC_CONTROL_1r(unit, &misc);
        TOP_MISC_CONTROL_1r_PVTMON_HIGHTEMP_STAT_CLEARf_SET(misc, 1);
        ioerr += WRITE_TOP_MISC_CONTROL_1r(unit, misc);
        sal_usleep(100);
        TOP_MISC_CONTROL_1r_PVTMON_HIGHTEMP_STAT_CLEARf_SET(misc, 0);
        ioerr += WRITE_TOP_MISC_CONTROL_1r(unit, misc);
    }

    /* Check if the stick bit is indeed cleared */
    TOP_PVTMON_HW_RST_STATUSr_CLR(reg);
    ioerr += READ_TOP_PVTMON_HW_RST_STATUSr(unit, &reg);
    chan = TOP_PVTMON_HW_RST_STATUSr_MIN_HW_RST_STATUSf_GET(reg);
    for (ix = 0; ix < PVT_SENSOR_CNT; ix++) {
        if (chan & (1 << ix )) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "HMON Sensor %d sticky bit can't be reset.\n"),
                      ix));

        }
    }

    /* Reset channels from fresh start */
    rv = pvt_channel_reset(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Set hardware reset threshold */
    rv = pvt_set_hw_reset_threshold(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Set software interrupt threshold */
    rv = pvt_set_intr_threshold(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Turn on hardware protection. */
    rv = pvt_enable_hw_reset(unit, 1);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
miim_init(int unit)
{
    bcmbd_miim_rate_config_t rate_config, *ratecfg = &rate_config;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_miim_init(unit));

    sal_memset(ratecfg, 0, sizeof(*ratecfg));

    /* Configure internal MDIO buses */
    ratecfg->dividend = 1;
    ratecfg->divisor = 25;
    SHR_IF_ERR_EXIT
        (bcmbd_miim_rate_config_set(unit, true, -1, ratecfg));

    /* Configure external MDIO buses */
    ratecfg->dividend = 1;
    ratecfg->divisor = 50;
    SHR_IF_ERR_EXIT
        (bcmbd_miim_rate_config_set(unit, false, -1, ratecfg));

exit:
    SHR_FUNC_EXIT();
}

static int
ep_to_cpu_dma_hdr_flex_config_init (int unit)
{
    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_t ep_to_cpu_hdr_config;

    SHR_FUNC_ENTER(unit);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_CLR(ep_to_cpu_hdr_config);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_0f_SET
        (ep_to_cpu_hdr_config, 0);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_1f_SET
        (ep_to_cpu_hdr_config, 1);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_2f_SET
        (ep_to_cpu_hdr_config, 2);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_3f_SET
        (ep_to_cpu_hdr_config, 3);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_4f_SET
        (ep_to_cpu_hdr_config, 4);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_5f_SET
        (ep_to_cpu_hdr_config, 5);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_6f_SET
        (ep_to_cpu_hdr_config, 6);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_7f_SET
        (ep_to_cpu_hdr_config, 7);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_8f_SET
        (ep_to_cpu_hdr_config, 8);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_9f_SET
        (ep_to_cpu_hdr_config, 9);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_10f_SET
        (ep_to_cpu_hdr_config, 10);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_11f_SET
        (ep_to_cpu_hdr_config, 11);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_12f_SET
        (ep_to_cpu_hdr_config, 12);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_13f_SET
        (ep_to_cpu_hdr_config, 13);

    EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm_MUX0_SIZE0_SEL_14f_SET
        (ep_to_cpu_hdr_config, 14);

    SHR_IF_ERR_VERBOSE_EXIT
        (WRITE_EPRE_EDEV_CONFIG_CPU_DMA_FLEX_WORD_MUX_PROFILEm
         (unit,
          0,
          ep_to_cpu_hdr_config));

exit:
    SHR_FUNC_EXIT();
}

static int
ifta100_acc_mode_init(int unit)
{
    MEMDB_IFTA100_ACC_MODESr_t reg;

    SHR_FUNC_ENTER(unit);

    MEMDB_IFTA100_ACC_MODESr_CLR(reg);
    /*
     * Set select mode to 1 to enable 2K entries
     * in tile 00.
     */
    MEMDB_IFTA100_ACC_MODESr_ACC_MODE0f_SET(reg, 1);
    MEMDB_IFTA100_ACC_MODESr_ACC_MODE1f_SET(reg, 1);
    MEMDB_IFTA100_ACC_MODESr_ACC_MODE2f_SET(reg, 1);
    MEMDB_IFTA100_ACC_MODESr_ACC_MODE3f_SET(reg, 1);

    SHR_IF_ERR_VERBOSE_EXIT
        (WRITE_MEMDB_IFTA100_ACC_MODESr(unit, reg));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */

int
bcm56780_a0_bd_dev_init(int unit)
{
    bool emul;

    SHR_FUNC_ENTER(unit);

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_IF_ERR_EXIT
        (dpr_latency_config(unit));

    SHR_IF_ERR_EXIT
        (hash_init(unit));

    SHR_IF_ERR_EXIT
        (ipep_init(unit));

    SHR_IF_ERR_EXIT
        (miim_init(unit));

    if (!emul) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_pvt_init(unit));
    }

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_mem_init(unit));

    SHR_IF_ERR_EXIT
        (debug_counter_init(unit));

    SHR_IF_ERR_EXIT
        (port_mib_reset(unit));

    SHR_IF_ERR_EXIT
        (ep_to_cpu_dma_hdr_flex_config_init(unit));

    SHR_IF_ERR_EXIT
        (ifta100_acc_mode_init(unit));

exit:
    SHR_FUNC_EXIT();
}
