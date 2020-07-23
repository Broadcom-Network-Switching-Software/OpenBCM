/*! \file bcm56996_a0_bd_dev_init.c
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
#include <bcmbd/bcmbd_miim.h>
#include <bcmbd/bcmbd_config.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/chip/bcm56996_a0_acc.h>
#include <bcmbd/chip/bcm56996_a0_dev.h>
#include <bcmdrd/chip/bcm56996_a0_defs.h>
#include "bcm56996_a0_drv.h"
#include <bcmlrd/chip/bcm56996_a0/bcm56996_a0_lrd_enum_ctype.h>

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV


/*******************************************************************************
* Local definitions
 */

/* IPEP memory reset waiting time. */
#define IPEP_RESET_DELAY                100
/* IPEP memory reset waiting time for emulator. */
#define IPEP_RESET_DELAY_EMUL           5000000
/* Data pipe number. */
#define DATA_PIPE_NUM                   16
/* PP pipe number. */
#define PP_PIPE_NUM                     4

/* total_latency + pipe_num * maxAccumulation. */
#define DPR_IP_BASE                     213
/* early retrieval + default margin */
#define DPR_IP_ADJUST                   -56
/* total_latency + pipe_num * maxAccumulation */
#define DPR_EP_BASE                     123
/* early retrieval + default margin */
#define DPR_EP_ADJUST                   -7
/*
 * IP DPR Latency calculattion:
 * dpr_latancy = total_latency * R + adjust + R * pipe_num * maxAccumulation
 *             = (total_latency + pipe_num * maxAccumulation) * R + adjust
 *             = base * R + adjust
 * total_latency: sum of module latency + extra latency + Domain Crossing
 * adjust: early retrieval + default margin
 * ch6_mdiv and fix_div are configured same for every frequence, so:
 * R = core_requency / pp_frequency
 * Round up
 */
#define DPR_IP_LATENCY(_cclock, _pclock) ((DPR_IP_BASE * _cclock + (_pclock >> 1)) \
                                          / _pclock + DPR_IP_ADJUST)
#define DPR_EP_LATENCY(_cclock, _pclock) ((DPR_EP_BASE * _cclock + (_pclock >> 1)) \
                                          / _pclock + DPR_EP_ADJUST)

/* Clock frequency enum is used for clk_freq_enum table index. */
#define CLK(mhz)                (BCM56996_A0_LRD_DEVICE_CLK_FREQ_T_T_CLK_##mhz)
/* Default frequency. */
#define PLL_FREQ_DEF            1325
/* Frequency identifier. */
#define PLL_FREQ_DEF_ID         CLK(1325MHZ)

/* FGFS frequencies list. */
static struct {
    uint32_t id;
    uint32_t freq;
} clk_freqs[] = {
    {CLK(1325MHZ), 1325},
    {CLK(1250MHZ), 1250},
    {CLK(1175MHZ), 1175},
    {CLK(1100MHZ), 1100},
    {CLK(1025MHZ), 1025},
    {CLK(950MHZ), 950}
};

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

/* Lookup frequency by ID. */
static int
get_clock(uint32_t id, uint32_t *freq)
{
    int i = 0;
    int count = sizeof(clk_freqs) / sizeof(clk_freqs[0]);

    for (i = 0; i < count; i++) {
        if (clk_freqs[i].id == id) {
            *freq = clk_freqs[i].freq;
            return SHR_E_NONE;
        }
    }

    return SHR_E_NOT_FOUND;
}

static int
dpr_latency_config(int unit)
{
    int ioerr = 0;
    ING_DOI_SER_CONTROLr_t ip_doi_ser_ctrl;
    EGR_DOI_SER_CONTROLr_t ep_doi_ser_ctrl;
    IP_DPR_LATENCY_CONFIGr_t ip_dpr_latency;
    EP_DPR_LATENCY_CONFIGr_t ep_dpr_latency;
    IP_DPR_LATENCY_CONFIG_WR_ENr_t ip_dpr_wr_en;
    EP_DPR_LATENCY_CONFIG_WR_ENr_t ep_dpr_wr_en;
    ING_DII_NULL_SLOT_CFGr_t ing_dii_null_slot_cfg;
    EGR_DII_NULL_SLOT_CFGr_t egr_dii_null_slot_cfg;

    uint32_t ip_latency;
    uint32_t ep_latency;
    uint32_t id, core_clock, pp_clock;
    uint32_t is_ecc, es_ecc;
    uint32_t threshold;

    id = bcmbd_config_get(unit, PLL_FREQ_DEF_ID,
                          "DEVICE_CONFIG", "CORE_CLK_FREQ");
    if (SHR_FAILURE(get_clock(id, &core_clock))) {
        id = PLL_FREQ_DEF_ID;
        core_clock = PLL_FREQ_DEF;
     }
    id = bcmbd_config_get(unit, id,
                          "DEVICE_CONFIG", "PP_CLK_FREQ");
    if (SHR_FAILURE(get_clock(id, &pp_clock)) || (pp_clock > core_clock)) {
        pp_clock = core_clock;
    }

    ip_latency = DPR_IP_LATENCY(core_clock, pp_clock);
    ep_latency = DPR_EP_LATENCY(core_clock, pp_clock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "IP_DPR=%"PRIu32", EP_DPR=%"PRIu32"\n"),
                 ip_latency, ep_latency));

    /* ECC enable.  */
    ioerr = READ_ING_DOI_SER_CONTROLr(unit, &ip_doi_ser_ctrl);
    is_ecc = ING_DOI_SER_CONTROLr_SLOT_PIPELINE_ECC_ENf_GET(ip_doi_ser_ctrl);
    ING_DOI_SER_CONTROLr_SLOT_PIPELINE_ECC_ENf_SET(ip_doi_ser_ctrl, 1);
    ioerr += WRITE_ING_DOI_SER_CONTROLr(unit, ip_doi_ser_ctrl);
    ioerr += READ_EGR_DOI_SER_CONTROLr(unit, &ep_doi_ser_ctrl);
    es_ecc = EGR_DOI_SER_CONTROLr_SLOT_PIPELINE_ECC_ENf_GET(ep_doi_ser_ctrl);
    EGR_DOI_SER_CONTROLr_SLOT_PIPELINE_ECC_ENf_SET(ep_doi_ser_ctrl, 1);
    ioerr += WRITE_EGR_DOI_SER_CONTROLr(unit, ep_doi_ser_ctrl);

    IP_DPR_LATENCY_CONFIGr_CLR(ip_dpr_latency);
    IP_DPR_LATENCY_CONFIGr_LATENCYf_SET(ip_dpr_latency, ip_latency);
    ioerr += WRITE_IP_DPR_LATENCY_CONFIGr(unit, ip_dpr_latency);
    IP_DPR_LATENCY_CONFIG_WR_ENr_WR_ENf_SET(ip_dpr_wr_en, 1);
    ioerr += WRITE_IP_DPR_LATENCY_CONFIG_WR_ENr(unit, ip_dpr_wr_en);

    EP_DPR_LATENCY_CONFIGr_CLR(ep_dpr_latency);
    EP_DPR_LATENCY_CONFIGr_LATENCYf_SET(ep_dpr_latency, ep_latency);
    ioerr += WRITE_EP_DPR_LATENCY_CONFIGr(unit, ep_dpr_latency);
    EP_DPR_LATENCY_CONFIG_WR_ENr_WR_ENf_SET(ep_dpr_wr_en, 1);
    ioerr += WRITE_EP_DPR_LATENCY_CONFIG_WR_ENr(unit, ep_dpr_wr_en);

    /* Restore ECC. */
    ING_DOI_SER_CONTROLr_SLOT_PIPELINE_ECC_ENf_SET(ip_doi_ser_ctrl, is_ecc);
    ioerr += WRITE_ING_DOI_SER_CONTROLr(unit, ip_doi_ser_ctrl);
    EGR_DOI_SER_CONTROLr_SLOT_PIPELINE_ECC_ENf_SET(ep_doi_ser_ctrl, es_ecc);
    ioerr += WRITE_EGR_DOI_SER_CONTROLr(unit, ep_doi_ser_ctrl);

    /*
     * ING_DII_NULL_SLOT_CFG.COUNTER_THRESHOLD = DPPClockFreq/12
     * ING_DII_NULL_SLOT_CFG.FLEX_CTR_NULL_THRESHOLD = DPPClockFreq/3.81
     * EGR_DII_NULL_SLOT_CFG.COUNTER_THRESHOLD = DPPClockFreq/5
     * EGR_DII_NULL_SLOT_CFG.FLEX_CTR_NULL_THRESHOLD = DPPClockFreq/3.81
     */

    threshold = pp_clock / 12;
    ING_DII_NULL_SLOT_CFGr_COUNTER_THRESHOLDf_SET(ing_dii_null_slot_cfg,
                                                  threshold);
    threshold = (pp_clock * 100) / 381;
    ING_DII_NULL_SLOT_CFGr_FLEX_CTR_NULL_THRESHOLDf_SET(ing_dii_null_slot_cfg,
                                                        threshold);
    ioerr += WRITE_ING_DII_NULL_SLOT_CFGr_ALL(unit, ing_dii_null_slot_cfg);

    threshold = pp_clock / 5;
    EGR_DII_NULL_SLOT_CFGr_COUNTER_THRESHOLDf_SET(egr_dii_null_slot_cfg,
                                                  threshold);
    threshold = (pp_clock * 100) / 381;
    EGR_DII_NULL_SLOT_CFGr_FLEX_CTR_NULL_THRESHOLDf_SET(egr_dii_null_slot_cfg,
                                                        threshold);
    ioerr += WRITE_EGR_DII_NULL_SLOT_CFGr_ALL(unit, egr_dii_null_slot_cfg);

    return ioerr;
}

/* All hash memories are expected to be reset by software before reset
 * all memories. TH4G hash memory list
 *     EXACT_MATCH_HASH_CONTROL
 *     L2_ENTRY_HASH_CONTROL
 *     L3_TUNNEL_HASH_CONTROL
 *     MPLS_ENTRY_HASH_CONTROL
 *     EGR_ADAPT_HASH_CONTROL
 *     MINI_UFT_SHARED_BANKS_CONTROL
 *     UFT_SHARED_BANKS_CONTROL
 */
static int
hash_init(int unit)
{
    int ioerr = 0;
    L2_ENTRY_HASH_CONTROLm_t l2_entry_hash_control;
    L3_TUNNEL_HASH_CONTROLm_t l3_tunnel_hash_control;
    MPLS_ENTRY_HASH_CONTROLm_t mpls_entry_hash_control;
    EXACT_MATCH_HASH_CONTROLm_t exact_match_hash_control;
    EGR_ADAPT_HASH_CONTROLm_t egr_adapt_hash_control;
    MINI_UFT_SHARED_BANKS_CONTROLm_t mini_uft_shared_banks_control;
    UFT_SHARED_BANKS_CONTROLm_t uft_shared_banks_control;
    LPM_IP_CONTROLm_t lpm_ip_control;

    if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
        return SHR_E_NONE;
    }

    L2_ENTRY_HASH_CONTROLm_CLR(l2_entry_hash_control);
    L2_ENTRY_HASH_CONTROLm_ROBUST_HASH_ENf_SET(l2_entry_hash_control, 1);
    L2_ENTRY_HASH_CONTROLm_HASH_OFFSET_DEDICATED_BANK_1f_SET(l2_entry_hash_control, 32);
    L2_ENTRY_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(l2_entry_hash_control, 3);
    L2_ENTRY_HASH_CONTROLm_LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf_SET(l2_entry_hash_control, 1);
    L2_ENTRY_HASH_CONTROLm_LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf_SET(l2_entry_hash_control, 2);
    L2_ENTRY_HASH_CONTROLm_LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf_SET(l2_entry_hash_control, 3);
    ioerr += WRITE_L2_ENTRY_HASH_CONTROLm(unit, 0, l2_entry_hash_control);

    L3_TUNNEL_HASH_CONTROLm_CLR(l3_tunnel_hash_control);
    L3_TUNNEL_HASH_CONTROLm_ROBUST_HASH_ENf_SET(l3_tunnel_hash_control, 1);
    L3_TUNNEL_HASH_CONTROLm_HASH_OFFSET_DEDICATED_BANK_1f_SET(l3_tunnel_hash_control, 10);
    L3_TUNNEL_HASH_CONTROLm_HASH_OFFSET_DEDICATED_BANK_2f_SET(l3_tunnel_hash_control, 32);
    L3_TUNNEL_HASH_CONTROLm_HASH_OFFSET_DEDICATED_BANK_3f_SET(l3_tunnel_hash_control, 42);
    L3_TUNNEL_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(l3_tunnel_hash_control, 15);
    ioerr += WRITE_L3_TUNNEL_HASH_CONTROLm(unit, 0, l3_tunnel_hash_control);

    MPLS_ENTRY_HASH_CONTROLm_CLR(mpls_entry_hash_control);
    MPLS_ENTRY_HASH_CONTROLm_ROBUST_HASH_ENf_SET(mpls_entry_hash_control, 1);
    MPLS_ENTRY_HASH_CONTROLm_HASH_OFFSET_DEDICATED_BANK_1f_SET(mpls_entry_hash_control, 11);
    MPLS_ENTRY_HASH_CONTROLm_HASH_OFFSET_DEDICATED_BANK_2f_SET(mpls_entry_hash_control, 32);
    MPLS_ENTRY_HASH_CONTROLm_HASH_OFFSET_DEDICATED_BANK_3f_SET(mpls_entry_hash_control, 43);
    MPLS_ENTRY_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(mpls_entry_hash_control, 15);
    ioerr += WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, 0, mpls_entry_hash_control);

    EXACT_MATCH_HASH_CONTROLm_CLR(exact_match_hash_control);
    EXACT_MATCH_HASH_CONTROLm_ROBUST_HASH_ENf_SET(exact_match_hash_control, 1);
    EXACT_MATCH_HASH_CONTROLm_LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf_SET(exact_match_hash_control, 1);
    EXACT_MATCH_HASH_CONTROLm_LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf_SET(exact_match_hash_control, 2);
    EXACT_MATCH_HASH_CONTROLm_LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf_SET(exact_match_hash_control, 3);
    ioerr += WRITE_EXACT_MATCH_HASH_CONTROLm(unit, 0, exact_match_hash_control);

    EGR_ADAPT_HASH_CONTROLm_CLR(egr_adapt_hash_control);
    EGR_ADAPT_HASH_CONTROLm_ROBUST_HASH_ENf_SET(egr_adapt_hash_control, 1);
    EGR_ADAPT_HASH_CONTROLm_HASH_OFFSET_DEDICATED_BANK_1f_SET(egr_adapt_hash_control, 32);
    EGR_ADAPT_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(egr_adapt_hash_control, 3);
    ioerr += WRITE_EGR_ADAPT_HASH_CONTROLm(unit, 0, egr_adapt_hash_control);

    UFT_SHARED_BANKS_CONTROLm_CLR(uft_shared_banks_control);
    UFT_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(uft_shared_banks_control, 6);
    UFT_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(uft_shared_banks_control, 12);
    UFT_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(uft_shared_banks_control, 18);
    UFT_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(uft_shared_banks_control, 24);
    ioerr += WRITE_UFT_SHARED_BANKS_CONTROLm(unit, 0, uft_shared_banks_control);

    MINI_UFT_SHARED_BANKS_CONTROLm_CLR(mini_uft_shared_banks_control);
    ioerr += WRITE_MINI_UFT_SHARED_BANKS_CONTROLm(unit, 0, mini_uft_shared_banks_control);

    LPM_IP_CONTROLm_CLR(lpm_ip_control);
    LPM_IP_CONTROLm_LEVEL2_BANK_CONFIG_BLOCK_0f_SET(lpm_ip_control, 3);
    LPM_IP_CONTROLm_LEVEL2_BANK_CONFIG_BLOCK_1f_SET(lpm_ip_control, 4);
    LPM_IP_CONTROLm_LEVEL2_BANK_CONFIG_BLOCK_2f_SET(lpm_ip_control, 24);
    LPM_IP_CONTROLm_LEVEL2_BANK_CONFIG_BLOCK_3f_SET(lpm_ip_control, 32);
    LPM_IP_CONTROLm_LEVEL2_KEY_INPUT_SEL_BLOCK_0f_SET(lpm_ip_control, 8);
    LPM_IP_CONTROLm_LEVEL2_KEY_INPUT_SEL_BLOCK_1f_SET(lpm_ip_control, 8);
    LPM_IP_CONTROLm_LEVEL2_KEY_INPUT_SEL_BLOCK_2f_SET(lpm_ip_control, 8);
    LPM_IP_CONTROLm_LEVEL2_KEY_INPUT_SEL_BLOCK_3f_SET(lpm_ip_control, 8);
    LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_0f_SET(lpm_ip_control, 3);
    LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_1f_SET(lpm_ip_control, 12);
    LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_2f_SET(lpm_ip_control, 48);
    LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_3f_SET(lpm_ip_control, 192);
    LPM_IP_CONTROLm_LEVEL3_KEY_INPUT_SEL_BLOCK_0f_SET(lpm_ip_control, 8);
    LPM_IP_CONTROLm_LEVEL3_KEY_INPUT_SEL_BLOCK_1f_SET(lpm_ip_control, 8);
    LPM_IP_CONTROLm_LEVEL3_KEY_INPUT_SEL_BLOCK_2f_SET(lpm_ip_control, 8);
    LPM_IP_CONTROLm_LEVEL3_KEY_INPUT_SEL_BLOCK_3f_SET(lpm_ip_control, 8);
    ioerr += WRITE_LPM_IP_CONTROLm(unit, 0, lpm_ip_control);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
ipep_init(int unit)
{
    int rv;
    int ioerr = 0;
    int pipe;
    uint32_t pipe_map;
    bool sim = bcmdrd_feature_is_sim(unit);
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    ING_DII_HW_RESET_CONTROL_0r_t ing_hreset_ctrl_0;
    EGR_DII_HW_RESET_CONTROL_0r_t egr_hreset_ctrl_0;
    DLB_ECMP_HW_RESET_CONTROLr_t dlb_hreset_ctrl;
    FLEX_CTR_ING_MEM_RST_CTRLr_t ifc_mreset_ctrl;
    FLEX_CTR_ING_MEM_RST_STATUSr_t ifc_mreset_status;
    FLEX_CTR_EGR_MEM_RST_CTRLr_t efc_mreset_ctrl;
    FLEX_CTR_EGR_MEM_RST_STATUSr_t efc_mreset_status;

    if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
        return SHR_E_NONE;
    }

    /* Initialize IPIPE memory */
    ING_DII_HW_RESET_CONTROL_0r_CLR(ing_hreset_ctrl_0);
    ING_DII_HW_RESET_CONTROL_0r_VALIDf_SET(ing_hreset_ctrl_0, 1);
    /* Configure for 32K NEXT HOP */
    ING_DII_HW_RESET_CONTROL_0r_COUNTf_SET(ing_hreset_ctrl_0, 0x8000);
    ioerr += WRITE_ING_DII_HW_RESET_CONTROL_0r_ALL(unit, ing_hreset_ctrl_0);

    /* Initialize DLB memory */
    DLB_ECMP_HW_RESET_CONTROLr_CLR(dlb_hreset_ctrl);
    DLB_ECMP_HW_RESET_CONTROLr_RESET_ALLf_SET(dlb_hreset_ctrl, 1);
    DLB_ECMP_HW_RESET_CONTROLr_VALIDf_SET(dlb_hreset_ctrl, 1);
    /* Configure for 4K PORT_QUALITY_MAPPING */
    DLB_ECMP_HW_RESET_CONTROLr_COUNTf_SET(dlb_hreset_ctrl, 0x1000);
    ioerr += WRITE_DLB_ECMP_HW_RESET_CONTROLr(unit, dlb_hreset_ctrl);

    /* Initialize EPIPE memory */
    EGR_DII_HW_RESET_CONTROL_0r_CLR(egr_hreset_ctrl_0);
    EGR_DII_HW_RESET_CONTROL_0r_VALIDf_SET(egr_hreset_ctrl_0, 1);
    /* Configure for 32K NEXT HOP */
    EGR_DII_HW_RESET_CONTROL_0r_COUNTf_SET(egr_hreset_ctrl_0, 0x8000);
    ioerr += WRITE_EGR_DII_HW_RESET_CONTROL_0r_ALL(unit, egr_hreset_ctrl_0);

    /* Initialize FLEX CTR memory */
    FLEX_CTR_ING_MEM_RST_CTRLr_CLR(ifc_mreset_ctrl);
    FLEX_CTR_ING_MEM_RST_CTRLr_MAX_ADDRf_SET(ifc_mreset_ctrl, 0x7FF);
    FLEX_CTR_ING_MEM_RST_CTRLr_MEM_RST_ACTf_SET(ifc_mreset_ctrl, 1);
    ioerr += WRITE_FLEX_CTR_ING_MEM_RST_CTRLr_ALL(unit, ifc_mreset_ctrl);
    FLEX_CTR_EGR_MEM_RST_CTRLr_CLR(efc_mreset_ctrl);
    FLEX_CTR_EGR_MEM_RST_CTRLr_MAX_ADDRf_SET(efc_mreset_ctrl, 0x7FF);
    FLEX_CTR_EGR_MEM_RST_CTRLr_MEM_RST_ACTf_SET(efc_mreset_ctrl, 1);
    ioerr += WRITE_FLEX_CTR_EGR_MEM_RST_CTRLr_ALL(unit, efc_mreset_ctrl);

    /*
     * H/W reset scheme does not support polling HW_RESET_CONTROL.DONE any more,
     * before reset done register access will trigger error.
     */

    if (emul) {
        /* Add 5 sec delay to evade read failure issue observed in emulation. */
        sal_usleep(IPEP_RESET_DELAY_EMUL);
    } else {
        sal_usleep(IPEP_RESET_DELAY);
    }

    if (!sim) {
        /* Device specific PP pipe map, shared by IPIPE/EPIPE/FLEX_CTR */
        rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_IPIPE, &pipe_map);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        for (pipe = 0; pipe < PP_PIPE_NUM; pipe++) {
            if (!(pipe_map & (0x1 << pipe))) {
                continue;
            }

            ioerr += READ_ING_DII_HW_RESET_CONTROL_0r(unit, pipe,
                                                      &ing_hreset_ctrl_0);
            if (!ING_DII_HW_RESET_CONTROL_0r_DONEf_GET(ing_hreset_ctrl_0)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Pipe %d ING DII reset failed.\n"), pipe));
                return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
            }

            ioerr += READ_EGR_DII_HW_RESET_CONTROL_0r(unit, pipe,
                                                      &egr_hreset_ctrl_0);
            if (!EGR_DII_HW_RESET_CONTROL_0r_DONEf_GET(egr_hreset_ctrl_0)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Pipe %d EGR DII reset failed.\n"), pipe));
                return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
            }

            ioerr += READ_FLEX_CTR_ING_MEM_RST_STATUSr(unit, pipe,
                                                       &ifc_mreset_status);
            if (!FLEX_CTR_ING_MEM_RST_STATUSr_MEM_RST_DONEf_GET(ifc_mreset_status)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Pipe %d ING FLEX CTR reset failed.\n"),
                                     pipe));
                return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
            }

            ioerr += READ_FLEX_CTR_EGR_MEM_RST_STATUSr(unit, pipe,
                                                       &efc_mreset_status);
            if (!FLEX_CTR_EGR_MEM_RST_STATUSr_MEM_RST_DONEf_GET(efc_mreset_status)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Pipe %d EGR FLEX CTR reset failed.\n"),
                                     pipe));
                return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
            }
        }

        ioerr += READ_DLB_ECMP_HW_RESET_CONTROLr(unit, &dlb_hreset_ctrl);
        if (!DLB_ECMP_HW_RESET_CONTROLr_DONEf_GET(dlb_hreset_ctrl)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "DLB_ECMP reset failed.\n")));
            return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
        }
    }

    ING_DII_HW_RESET_CONTROL_0r_CLR(ing_hreset_ctrl_0);
    ioerr += WRITE_ING_DII_HW_RESET_CONTROL_0r_ALL(unit, ing_hreset_ctrl_0);
    EGR_DII_HW_RESET_CONTROL_0r_CLR(egr_hreset_ctrl_0);
    ioerr += WRITE_EGR_DII_HW_RESET_CONTROL_0r_ALL(unit, egr_hreset_ctrl_0);
    DLB_ECMP_HW_RESET_CONTROLr_CLR(dlb_hreset_ctrl);
    ioerr += WRITE_DLB_ECMP_HW_RESET_CONTROLr(unit, dlb_hreset_ctrl);
    FLEX_CTR_ING_MEM_RST_STATUSr_CLR(ifc_mreset_status);
    ioerr += WRITE_FLEX_CTR_ING_MEM_RST_STATUSr_ALL(unit, ifc_mreset_status);
    FLEX_CTR_EGR_MEM_RST_STATUSr_CLR(efc_mreset_status);
    ioerr += WRITE_FLEX_CTR_EGR_MEM_RST_STATUSr_ALL(unit, efc_mreset_status);

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
bcm56996_a0_tm_mem_init(int unit)
{
    int rv;
    int ioerr = 0;
    int pipe;
    uint32_t pipe_map;
    MMU_GLBCFG_MISCCONFIGr_t mmu_misccfg;
    MMU_EBCFP_MXM_TAG_MEMm_t mxm_tag_mem;
    EDB_XMIT_START_COUNTm_t xmit_start_count;
    MMU_CFAP_BSTTHRSr_t cfap_bstthd;
    bcmdrd_sym_info_t sinfo;
    uint32_t port_idx;

    /* Enable parity before initializing MMU memory */
    ioerr += READ_MMU_GLBCFG_MISCCONFIGr(unit, &mmu_misccfg);
    MMU_GLBCFG_MISCCONFIGr_ECCP_ENf_SET(mmu_misccfg, 1);
    ioerr += WRITE_MMU_GLBCFG_MISCCONFIGr(unit, mmu_misccfg);
    sal_usleep(20);

    /* Initialize MMU memory */
    MMU_GLBCFG_MISCCONFIGr_INIT_MEMf_SET(mmu_misccfg, 1);
    ioerr += WRITE_MMU_GLBCFG_MISCCONFIGr(unit, mmu_misccfg);
    MMU_GLBCFG_MISCCONFIGr_INIT_MEMf_SET(mmu_misccfg, 0);
    ioerr += WRITE_MMU_GLBCFG_MISCCONFIGr(unit, mmu_misccfg);
    sal_usleep(30);

    rv = bcmdrd_dev_valid_pipes_get(unit, &pipe_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Initialize tag memory */
    MMU_EBCFP_MXM_TAG_MEMm_CLR(mxm_tag_mem);
    MMU_EBCFP_MXM_TAG_MEMm_DATAf_SET(mxm_tag_mem, 1);
    for (pipe = 0; pipe < DATA_PIPE_NUM; pipe++) {
        if (pipe_map & (1 << pipe)) {
            ioerr += WRITE_MMU_EBCFP_MXM_TAG_MEMm(unit, pipe, 0, mxm_tag_mem);
        }
    }

    /* Initialize EDB_XMIT_START_COUNT memory */
    EDB_XMIT_START_COUNTm_CLR(xmit_start_count);
    ioerr += bcmdrd_pt_info_get(unit,
                 EDB_XMIT_START_COUNTm, &sinfo);

    for (pipe = 0; pipe < DATA_PIPE_NUM; pipe++) {
        if (pipe_map & (1 << pipe)) {
            for (port_idx = 0; port_idx <= sinfo.index_max; port_idx++) {
                ioerr += WRITE_EDB_XMIT_START_COUNTm(unit, pipe, port_idx,
                                                     xmit_start_count);
            }
        }
    }

    /* Update BST CFAP threshold values. */
    MMU_CFAP_BSTTHRSr_CLR(cfap_bstthd);
    MMU_CFAP_BSTTHRSr_BST_THRESHOLDf_SET(cfap_bstthd, 0x7ffff);

    /* ITM 0*/
    if (pipe_map & 0xf00f) {
        ioerr += WRITE_MMU_CFAP_BSTTHRSr(unit, 0, cfap_bstthd);
    }
    /* ITM 1*/
    if (pipe_map & 0xff0) {
        ioerr += WRITE_MMU_CFAP_BSTTHRSr(unit, 1, cfap_bstthd);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
debug_counter_init(int unit)
{
    int ioerr = 0;
    int port;
    int rv;
    bcmdrd_pbmp_t pbmp;
    RUC_64r_t ruc;
    RPORTD_64r_t rportd;
    RDBGC0_64r_t rdbgc0;
    RDBGC1_64r_t rdbgc1;
    RDBGC2_64r_t rdbgc2;
    RDBGC3_64r_t rdbgc3;
    RDBGC4_64r_t rdbgc4;
    RDBGC5_64r_t rdbgc5;
    RDBGC6_64r_t rdbgc6;
    RDBGC7_64r_t rdbgc7;
    RDBGC8_64r_t rdbgc8;
    RDBGC9_64r_t rdbgc9;
    RDBGC10_64r_t rdbgc10;
    RDBGC11_64r_t rdbgc11;
    RDBGC12_64r_t rdbgc12;
    RDBGC13_64r_t rdbgc13;
    RDBGC14_64r_t rdbgc14;
    RDBGC15_64r_t rdbgc15;
    TDBGC0_64r_t tdbgc0;
    TDBGC1_64r_t tdbgc1;
    TDBGC2_64r_t tdbgc2;
    TDBGC3_64r_t tdbgc3;
    TDBGC4_64r_t tdbgc4;
    TDBGC5_64r_t tdbgc5;
    TDBGC6_64r_t tdbgc6;
    TDBGC7_64r_t tdbgc7;
    TDBGC8_64r_t tdbgc8;
    TDBGC9_64r_t tdbgc9;
    TDBGC10_64r_t tdbgc10;
    TDBGC11_64r_t tdbgc11;
    ING_ECN_COUNTER_64r_t ing_ecn_counter;
    EGR_1588_LINK_DELAY_64r_t egr_1588_link_delay;
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);

    RUC_64r_CLR(ruc);
    ING_ECN_COUNTER_64r_CLR(ing_ecn_counter);
    RPORTD_64r_CLR(rportd);
    RDBGC0_64r_CLR(rdbgc0);
    RDBGC1_64r_CLR(rdbgc1);
    RDBGC2_64r_CLR(rdbgc2);
    RDBGC3_64r_CLR(rdbgc3);
    RDBGC4_64r_CLR(rdbgc4);
    RDBGC5_64r_CLR(rdbgc5);
    RDBGC6_64r_CLR(rdbgc6);
    RDBGC7_64r_CLR(rdbgc7);
    RDBGC8_64r_CLR(rdbgc8);
    RDBGC9_64r_CLR(rdbgc9);
    RDBGC10_64r_CLR(rdbgc10);
    RDBGC11_64r_CLR(rdbgc11);
    RDBGC12_64r_CLR(rdbgc12);
    RDBGC13_64r_CLR(rdbgc13);
    RDBGC14_64r_CLR(rdbgc14);
    RDBGC15_64r_CLR(rdbgc15);
    TDBGC0_64r_CLR(tdbgc0);
    TDBGC1_64r_CLR(tdbgc1);
    TDBGC2_64r_CLR(tdbgc2);
    TDBGC3_64r_CLR(tdbgc3);
    TDBGC4_64r_CLR(tdbgc4);
    TDBGC5_64r_CLR(tdbgc5);
    TDBGC6_64r_CLR(tdbgc6);
    TDBGC7_64r_CLR(tdbgc7);
    TDBGC8_64r_CLR(tdbgc8);
    TDBGC9_64r_CLR(tdbgc9);
    TDBGC10_64r_CLR(tdbgc10);
    TDBGC11_64r_CLR(tdbgc11);

    rv = bcmdrd_dev_logic_pbmp(unit, &pbmp);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* coverity[unreachable] */
    BCMDRD_PBMP_ITER(pbmp, port) {
        ioerr += READ_EGR_1588_LINK_DELAY_64r(unit, port, &egr_1588_link_delay);
        EGR_1588_LINK_DELAY_64r_LINK_DELAYf_SET(egr_1588_link_delay, 0);
        ioerr += WRITE_EGR_1588_LINK_DELAY_64r(unit, port, egr_1588_link_delay);

        if (!asim) {
            continue;
        }

        /* The following debug counters have default value of 0 */
        ioerr += WRITE_RUC_64r(unit, port, ruc);
        ioerr += WRITE_RPORTD_64r(unit, port, rportd);
        ioerr += WRITE_RDBGC0_64r(unit, port, rdbgc0);
        ioerr += WRITE_RDBGC1_64r(unit, port, rdbgc1);
        ioerr += WRITE_RDBGC2_64r(unit, port, rdbgc2);
        ioerr += WRITE_RDBGC3_64r(unit, port, rdbgc3);
        ioerr += WRITE_RDBGC4_64r(unit, port, rdbgc4);
        ioerr += WRITE_RDBGC5_64r(unit, port, rdbgc5);
        ioerr += WRITE_RDBGC6_64r(unit, port, rdbgc6);
        ioerr += WRITE_RDBGC7_64r(unit, port, rdbgc7);
        ioerr += WRITE_RDBGC8_64r(unit, port, rdbgc8);
        ioerr += WRITE_RDBGC9_64r(unit, port, rdbgc9);
        ioerr += WRITE_RDBGC10_64r(unit, port, rdbgc10);
        ioerr += WRITE_RDBGC11_64r(unit, port, rdbgc11);
        ioerr += WRITE_RDBGC12_64r(unit, port, rdbgc12);
        ioerr += WRITE_RDBGC13_64r(unit, port, rdbgc13);
        ioerr += WRITE_RDBGC14_64r(unit, port, rdbgc14);
        ioerr += WRITE_RDBGC15_64r(unit, port, rdbgc15);
        ioerr += WRITE_TDBGC0_64r(unit, port, tdbgc0);
        ioerr += WRITE_TDBGC1_64r(unit, port, tdbgc1);
        ioerr += WRITE_TDBGC2_64r(unit, port, tdbgc2);
        ioerr += WRITE_TDBGC3_64r(unit, port, tdbgc3);
        ioerr += WRITE_TDBGC4_64r(unit, port, tdbgc4);
        ioerr += WRITE_TDBGC5_64r(unit, port, tdbgc5);
        ioerr += WRITE_TDBGC6_64r(unit, port, tdbgc6);
        ioerr += WRITE_TDBGC7_64r(unit, port, tdbgc7);
        ioerr += WRITE_TDBGC8_64r(unit, port, tdbgc8);
        ioerr += WRITE_TDBGC9_64r(unit, port, tdbgc9);
        ioerr += WRITE_TDBGC10_64r(unit, port, tdbgc10);
        ioerr += WRITE_TDBGC11_64r(unit, port, tdbgc11);
    }
    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
port_mib_reset(int unit)
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

/*! Workaround.
 * Update hardware default value to expected.
 */
static int
default_behavior_update(int unit)
{
    int ioerr = 0;
    int port;
    PORT_TABm_t port_table;
    EGR_PORTm_t eport_table;
    PARS_SER_CONTROLr_t ing_ser;
    EGR_EPARS_SER_CONTROLr_t egr_ser;
    uint32_t pt_ecc, ept_ecc;

    SHR_FUNC_ENTER(unit);

    /* Default enable NON_GSH and GSH traffic on CPU port table */
    port = 0;
    /* Enable parity before configure port_table and egr_port */
    ioerr += READ_PARS_SER_CONTROLr(unit, &ing_ser);
    pt_ecc = PARS_SER_CONTROLr_PORT_TABLE_ECC_ENf_GET(ing_ser);
    PARS_SER_CONTROLr_PORT_TABLE_ECC_ENf_SET(ing_ser, 1);
    ioerr += WRITE_PARS_SER_CONTROLr(unit, ing_ser);
    ioerr += READ_EGR_EPARS_SER_CONTROLr(unit, &egr_ser);
    ept_ecc = EGR_EPARS_SER_CONTROLr_EGR_PORT_ECC_ENf_GET(egr_ser);
    EGR_EPARS_SER_CONTROLr_EGR_PORT_ECC_ENf_SET(egr_ser, 1);
    ioerr += WRITE_EGR_EPARS_SER_CONTROLr(unit, egr_ser);
    sal_usleep(20);

    ioerr += READ_PORT_TABm(unit, port, &port_table);
    PORT_TABm_ALLOW_NON_GSHf_SET(port_table, 1);
    PORT_TABm_ALLOW_GSHf_SET(port_table, 1);
    ioerr += WRITE_PORT_TABm(unit, port, port_table);
    ioerr += READ_EGR_PORTm(unit, port, &eport_table);
    EGR_PORTm_ALLOW_NON_GSHf_SET(eport_table, 1);
    EGR_PORTm_ALLOW_GSHf_SET(eport_table, 1);
    ioerr += WRITE_EGR_PORTm(unit, port, eport_table);
    sal_usleep(30);

    /* Restore parity */
    PARS_SER_CONTROLr_PORT_TABLE_ECC_ENf_SET(ing_ser, pt_ecc);
    ioerr += WRITE_PARS_SER_CONTROLr(unit, ing_ser);
    EGR_EPARS_SER_CONTROLr_EGR_PORT_ECC_ENf_SET(egr_ser, ept_ecc);
    ioerr += WRITE_EGR_EPARS_SER_CONTROLr(unit, egr_ser);

    if (ioerr) {
        SHR_ERR_EXIT(SHR_E_IO);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */

int
bcm56996_a0_bd_dev_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (dpr_latency_config(unit));

    SHR_IF_ERR_EXIT
        (hash_init(unit));

    SHR_IF_ERR_EXIT
        (ipep_init(unit));

    SHR_IF_ERR_EXIT
        (miim_init(unit));

    SHR_IF_ERR_EXIT
        (bcm56996_a0_tm_mem_init(unit));

    SHR_IF_ERR_EXIT
        (debug_counter_init(unit));

    SHR_IF_ERR_EXIT
        (port_mib_reset(unit));

    SHR_IF_ERR_EXIT
        (default_behavior_update(unit));

    SHR_IF_ERR_EXIT
        (bcm56996_a0_bd_pvt_drv_init(unit));

exit:
    SHR_FUNC_EXIT();
}
