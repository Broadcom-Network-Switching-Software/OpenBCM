/*! \file bcm56990_b0_ptm.c
 *
 * Miscellaneous utilities for PTM
 *
 * This file contains the misc utils for PTM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include "./bcmptm_bcm56990_b0_rm_tcam.h"
#include "../bcmptm_common.h"
#include "./bcm56990_b0_ptm_cci.h"
#include "./bcm56990_b0_ptm_uft.h"
#include "./bcm56990_b0_ptm_uft_be.h"
#include "bcmptm_bcm56990_b0_rm_alpm.h"
#include "bcmptm_bcm56990_b0_cth_alpm.h"
#include "./bcm56990_b0_ptm_ser.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC
#define ALPM_ADDR_MODE_FALSE FALSE
#define ALPM_ADDR_MODE_TRUE TRUE

/* HW constants */
#define HW_MIN_BANK_COUNT_PT_GROUP_EM 0
#define HW_MAX_BANK_COUNT_PT_GROUP_EM 4
#define HW_MIN_PT_SIZE_EXACT_MATCH_4 0
#define HW_UFT_BANK_PT_SIZE_EXACT_MATCH_4 \
    (bcmdrd_pt_index_count(unit, EXACT_MATCH_4m) /\
     HW_MAX_BANK_COUNT_PT_GROUP_EM)

/* l2_entry_uft banks */
#define HW_MIN_BANK_COUNT_PT_GROUP_L2_ENTRY_UFT 0
#define HW_MAX_BANK_COUNT_PT_GROUP_L2_ENTRY_UFT 4
#define HW_MIN_PT_SIZE_L2_ENTRY_UFT 0
#define HW_UFT_BANK_PT_SIZE_L2_ENTRY_UFT \
    (bcmdrd_pt_index_count(unit, L2_ENTRY_UFT_SINGLEm) /\
     HW_MAX_BANK_COUNT_PT_GROUP_L2_ENTRY_UFT)

/* mini-uft banks */
#define HW_MIN_BANK_COUNT_PT_GROUP_ALPM_LEVEL2 0
#define HW_MAX_BANK_COUNT_PT_GROUP_ALPM_LEVEL2 6
#define HW_MIN_PT_SIZE_L3_DEFIP_ALPM_LEVEL2 0
#define HW_UFT_BANK_PT_SIZE_L3_DEFIP_ALPM_LEVEL2 \
    (bcmdrd_pt_index_count(unit, L3_DEFIP_ALPM_LEVEL2m) /\
     HW_MAX_BANK_COUNT_PT_GROUP_ALPM_LEVEL2)

#define HW_MIN_BANK_COUNT_PT_GROUP_ALPM_LEVEL3 0
#define HW_MAX_BANK_COUNT_PT_GROUP_ALPM_LEVEL3 8
#define HW_MIN_PT_SIZE_L3_DEFIP_ALPM_LEVEL3 0
#define HW_UFT_BANK_PT_SIZE_L3_DEFIP_ALPM_LEVEL3 \
    (bcmdrd_pt_index_count(unit, L3_DEFIP_ALPM_LEVEL3m) /\
     HW_MAX_BANK_COUNT_PT_GROUP_ALPM_LEVEL3)

#define SER 1


/*******************************************************************************
 * Typedefs
 */
typedef enum {
    PT_GROUP_DEFAULT = BCMPTM_PT_GROUP_DEFAULT, /* Always - for all chips */

    /* non-DEFAULT PT groups */
    PT_GROUP_EM,
    PT_GROUP_L2_ENTRY_UFT,
    PT_GROUP_ALPM_LEVEL2,
    PT_GROUP_ALPM_LEVEL3,

    PT_GROUP_MAX /* Must add more groups before GROUP_MAX */
} bcm56990_pt_group_t;

/* List of PTs which need dfield_format_id.
 * Fill it on-demand basis only. */
typedef struct pt_with_dfid_s {
    bcmdrd_sid_t pt_sid;
    uint8_t size; /* in bytes */
                  /* only 1, 2 are supported */
} pt_with_dfid_t;


/*******************************************************************************
 * Private variables
 */
/* Storage of pt_size for some PTs in PT group.
 * Must be unit-specific because different units of same type may be configured
 * differently. */
static uint32_t pt_size_exact_match_4[BCMDRD_CONFIG_MAX_UNITS][2];
static uint32_t pt_size_l2_entry_uft[BCMDRD_CONFIG_MAX_UNITS][2];
static uint32_t pt_size_l3_defip_alpm_level2[BCMDRD_CONFIG_MAX_UNITS][2];
static uint32_t pt_size_l3_defip_alpm_level3[BCMDRD_CONFIG_MAX_UNITS][2];
static int pt_index_min_l3_defip_alpm_level3[BCMDRD_CONFIG_MAX_UNITS][2];
static bcmdrd_sid_t *s_bcm56990_b0_pt_with_ltid[BCMDRD_CONFIG_MAX_UNITS];
static bcmptm_overlay_info_t *s_bcm56990_b0_overlay_info[BCMDRD_CONFIG_MAX_UNITS];
static pt_with_dfid_t *s_bcm56990_b0_pt_with_dfid[BCMDRD_CONFIG_MAX_UNITS];

/* List of PTs which must NOT be cached */
static const bcmdrd_sid_t bcm56990_b0_dont_cache[] = {
    /* Secret registers */
    MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACINGr,
    MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_PICK_MIN_SPACINGr,

    /* LP views - None in TH4 */

    /* RM-ALPM does not use DATA_ONLY views for L3_DEFIP */
    L3_DEFIP_DATA_LEVEL1m,
    L3_DEFIP_DATA_LEVEL1_WIDEm,

    /* RM-ALPM does not use TCAM_ONLY views for L3_DEFIP, but if we mark them
     * non-cacheable, we cannot do xy conversion for them on interactive path.
    L3_DEFIP_TCAM_LEVEL1m,
    L3_DEFIP_TCAM_LEVEL1_WIDEm,
    */

    /* Unused DATA_ONLY views of Aggr Tcams
     * - must be marked non-cacheable if RM will not be using these
     *   (if they don't have any atomicity issues)
     *
     * - if marking them as non-cacheable, we cannot declare them as overlays
     *   of AGGR Tcam views.
     */
    CPU_COS_MAP_DATA_ONLYm,
    /* EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm, */
    /* IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm, */
    L2_USER_ENTRY_DATA_ONLYm,
    MY_STATION_TCAM_DATA_ONLYm,
    MY_STATION_2_TCAM_DATA_ONLYm,
    ING_MIRROR_EVENT_PROFILE_DATA_ONLYm,
    L3_TUNNEL_TCAM_DATA_ONLYm,

    /* Unused TCAM_ONLY views of Aggr Tcams
     * - must leave them cacheable
     * - Only reason, we leave them cacheable is to be able to do xy conversion
     *   on interactive path.
     */

    /* Unused Aggr TCAM views for which RM will use only individual views
     * - must leave them cacheable
     * - Only reason, we leave them cacheable is to be able to do xy conversion
     *   on interactive path.
     */

    /* Memories in Port blocks */
    XLPORT_WC_UCMEM_DATAm,
    CDPORT_TSC_UCMEM_DATAm,
    AM_TABLEm,
    CDMIB_MEMm,
    CDPORT_TSC_UCMEM_DATAm,
    RX_LKUP_1588_MEM_MPP0m,
    RX_LKUP_1588_MEM_MPP1m,
    SPEED_ID_TABLEm,
    SPEED_PRIORITY_MAP_TBLm,
    TX_LKUP_1588_MEMm,
    UM_TABLEm
};

/* List of PTs which MUST be cached */
static const bcmdrd_sid_t bcm56990_b0_must_cache[] = {
    /* Hit bits are not part of functional entry.
     * - but must allow SW to write to them on modeled path. */
    L2_HITDA_ONLYm,
    L2_HITDA_UFTm,
    L2_HITSA_ONLYm,
    L2_HITSA_UFTm,
    L3_DEFIP_LEVEL1_HIT_ONLYm,
    L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm,
    L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm,
    EXACT_MATCH_HIT_ONLYm,

    /* Cacheable */
    LPM_IP_CONTROLm,

    /* Are cacheable but with SER_ENTRY_CLEAR response type.
     * These will be init on modeled path. */
    DLB_ECMP_FLOWSET_INST0m,
    DLB_ECMP_FLOWSET_INST1m,

    /* By default these mems will be not cacheable, so force
     * them to be cacheable - else we cannot implement them as narrow views
     * of wide functional views */
    EGR_ADAPT_ECCm,
    EXACT_MATCH_ECCm,
    L2_ENTRY_ECCm,
    L3_TUNNEL_ECCm,
    MPLS_ENTRY_ECCm,
    L3_DEFIP_ALPM_LEVEL3_ECCm,
    L3_DEFIP_ALPM_LEVEL2_ECCm,
    L2_ENTRY_UFT_ECCm
};

/* List of PTs for which we must also store LTID for every entry */
static bcmdrd_sid_t bcm56990_b0_pt_with_ltid[] = {
    /* Cannot support LTID for single_entry PTs - Registers for example */

    /* Hash tables */
    EGR_ADAPT_SINGLEm,

    EXACT_MATCH_2m,
    EXACT_MATCH_4m,

    L2_ENTRY_SINGLEm,   /* L2Xm */
    L2_ENTRY_UFT_SINGLEm,

    L3_TUNNEL_SINGLEm,
    L3_TUNNEL_DOUBLEm,
    L3_TUNNEL_QUADm,

    MPLS_ENTRY_SINGLEm,
    MPLS_ENTRY_DOUBLEm,

    /* RM-ALPM does not need ltid storage. */

    /* Aggr Tcam tables
     * NOTE: We must NOT store LTID for TCAM_ONLY, DATA_ONLY views of
     *       aggr Tcams unless we know that RM is going to be using TCAM_ONLY
     *       view.
     */
    CPU_COS_MAPm,
    /* CPU_COS_MAP_ONLYm, */
    EXACT_MATCH_LOGICAL_TABLE_SELECTm,
    /* EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm, */
    IFP_LOGICAL_TABLE_SELECTm,
    /* IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm, */
    ING_MIRROR_EVENT_PROFILEm,
    /* ING_MIRROR_EVENT_PROFILE_ONLYm, */
    L2_USER_ENTRYm,
    /* L2_USER_ENTRY_ONLYm, */
    MY_STATION_TCAMm,
    /* MY_STATION_TCAM_ENTRY_ONLYm, */
    MY_STATION_2_TCAMm,
    /* MY_STATION_2_TCAM_ENTRY_ONLYm, */
    L3_TUNNEL_TCAMm,
    /* L3_TUNNEL_TCAM_ONLYm, */
    L3_TUNNEL_PAIR_TCAMm,
    /* L3_TUNNEL_TCAM_ONLYm, */

    /* Unused TCAM aggr views */

    /* LTID must be managed outside of PTcache for ALPM related TCAMs */

    /* TCAM_ONLY view for aggr TCAMs - for which RM only uses individual views */

    /* TCAM_ONLY view for non-aggr TCAMs */
    EFP_TCAMm,
    FP_UDF_TCAMm,
    IFP_TCAMm,
    UDF_CONDITIONAL_CHECK_TABLE_CAMm,
    VFP_TCAMm,

    /* DATA_ONLY views of aggr, non_aggr TCAMs
     * dont need ltid storage */
};

/* List of PTs which need dfield_format_id.
 * Fill it on-demand basis ONLY */
static pt_with_dfid_t bcm56990_b0_pt_with_dfid[] = {
    {INVALIDm, 0}
};

/* List of overlay PTs, and related info.
 * Note: Views without hit bits are not in this list - these are marked as not
 * cacheable. SW must now be accessing these views on Modeled path.
 */
static bcmptm_overlay_info_t bcm56990_b0_overlay_info[] = {
    /* List of PTs which must be accessed on Modeled Path but should not use
     * any cache resources.
     * When read from interactive path - data must be returned from HW.*/
    {BCMPTM_OINFO_MODE_CACHE0, ALPM_ADDR_MODE_FALSE,
        L2_HITDA_ONLYm, INVALIDm, 0, 0, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_CACHE0, ALPM_ADDR_MODE_FALSE,
        L2_HITDA_UFTm, INVALIDm, 0, 0, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_CACHE0, ALPM_ADDR_MODE_FALSE,
        L2_HITSA_ONLYm, INVALIDm, 0, 0, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_CACHE0, ALPM_ADDR_MODE_FALSE,
        L2_HITSA_UFTm, INVALIDm, 0, 0, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_CACHE0, ALPM_ADDR_MODE_FALSE,
        L3_DEFIP_LEVEL1_HIT_ONLYm, INVALIDm, 0, 0, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_CACHE0, ALPM_ADDR_MODE_FALSE,
        L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm, INVALIDm, 0, 0, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_CACHE0, ALPM_ADDR_MODE_FALSE,
        L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm, INVALIDm, 0, 0, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_CACHE0, ALPM_ADDR_MODE_FALSE,
        EXACT_MATCH_HIT_ONLYm, INVALIDm, 0, 0, 0, 0, NULL},

    /* hash table overlays */
    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        EGR_ADAPT_ECCm, EGR_ADAPT_SINGLEm,
        120, 1, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        EXACT_MATCH_ECCm, EXACT_MATCH_4m,
        120, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        EXACT_MATCH_2m, EXACT_MATCH_4m,
        240, 2, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_NONE, ALPM_ADDR_MODE_FALSE,
        EXACT_MATCH_4m, EXACT_MATCH_4m,
        480, 1, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        L2_ENTRY_ECCm, L2_ENTRY_SINGLEm,
        120, 1, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        L2_ENTRY_UFT_ECCm, L2_ENTRY_UFT_SINGLEm,
        120, 1, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        L3_TUNNEL_ECCm, L3_TUNNEL_QUADm,
        120, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        L3_TUNNEL_SINGLEm, L3_TUNNEL_QUADm,
        120, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        L3_TUNNEL_DOUBLEm, L3_TUNNEL_QUADm,
        240, 2, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_NONE, ALPM_ADDR_MODE_FALSE,
        L3_TUNNEL_QUADm, L3_TUNNEL_QUADm,
        480, 1, 0, 0, NULL},

    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        MPLS_ENTRY_ECCm, MPLS_ENTRY_DOUBLEm,
        120, 2, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        MPLS_ENTRY_SINGLEm, MPLS_ENTRY_DOUBLEm,
        120, 2, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_NONE, ALPM_ADDR_MODE_FALSE,
        MPLS_ENTRY_DOUBLEm, MPLS_ENTRY_DOUBLEm,
        240, 1, 0, 0, NULL},

    /* ALPM notes:
     * No gaps in LEVEL2 tables (6K, 24K deep)
     * - because only 6 banks of 1K bkts each
     *
     * No special sbus addressing rules:
     * sbus_addr = {bank[2:0], bkt, sub_entries[1:0]}.
     *
     * wide_entry has 4 sub entries
     *
     * wide_entry_addr = single_entry_addr >> 2
     *
     * sub_entry_addr = singe_entry_addr % 4
     * single_entry_addr = (wide_entry_addr*4) + sub_entry_addr;
     * ecc_entry_addr = (wide_entry_addr*4) + sub_entry_addr;
     *
     * single_entry is ecc_entry
     */

    /* ALPM_LEVEL3 overlays
     * No special addressing - just like HASH - use ALPM_ADDR_MODE_FALSE */
    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        L3_DEFIP_ALPM_LEVEL3_ECCm, L3_DEFIP_ALPM_LEVEL3m,
        120, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        L3_DEFIP_ALPM_LEVEL3_SINGLEm, L3_DEFIP_ALPM_LEVEL3m,
        120, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_NONE, ALPM_ADDR_MODE_FALSE,
        L3_DEFIP_ALPM_LEVEL3m, L3_DEFIP_ALPM_LEVEL3m,
        480, 1, 0, 0, NULL},

    /* ALPM_LEVEL2 overlays
     * No special addressing - just like HASH - use ALPM_ADDR_MODE_FALSE */
    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        L3_DEFIP_ALPM_LEVEL2_ECCm, L3_DEFIP_ALPM_LEVEL2m,
        120, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_HASH, ALPM_ADDR_MODE_FALSE,
        L3_DEFIP_ALPM_LEVEL2_SINGLEm, L3_DEFIP_ALPM_LEVEL2m,
        120, 4, 0, 0, NULL},
    {BCMPTM_OINFO_MODE_NONE, ALPM_ADDR_MODE_FALSE,
        L3_DEFIP_ALPM_LEVEL2m, L3_DEFIP_ALPM_LEVEL2m,
        480, 1, 0, 0, NULL},

    /* RM-ALPM does not use DATA_ONLY, TCAM_ONLY views for L3_DEFIP, _WIDE */

    /* For TCAM overlays, tcam portion must include tcam_ecc bits if present,
     * but must not include ecc/parity bits for data portion. This is because
     * entry = {data_ecc, data_only, tcam_ecc, tcam_only} and we are trying to
     * extract data_only portion from entry. */

    /* Aggr Tcams for which RM also uses DATA_ONLY views */
    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_ADDR_MODE_FALSE,
        EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm, EXACT_MATCH_LOGICAL_TABLE_SELECTm,
        56, 1, 178, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_ADDR_MODE_FALSE,
        IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm, IFP_LOGICAL_TABLE_SELECTm,
        64, 1, 210, 1, NULL},

    /* DATA_ONLY views of TCAM_AGGR views - needed only if TCAMs have atomicity
     * issue.
     * No TCAM atomicity issue in TH4. Mark them non-cacheable */
/*
 * Revise the numbers below for narrow_entry_width, start_bit in wide entry, if
 * uncommenting any of the following overlays.
 *
    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_ADDR_MODE_FALSE,
        CPU_COS_MAP_DATA_ONLYm, CPU_COS_MAPm,
        15, 1, 218, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_ADDR_MODE_FALSE,
        ING_MIRROR_EVENT_PROFILE_DATA_ONLYm, ING_MIRROR_EVENT_PROFILEm,
        11, 1, 17, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_ADDR_MODE_FALSE,
        L2_USER_ENTRY_DATA_ONLYm, L2_USER_ENTRYm,
        30, 1, 178, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_ADDR_MODE_FALSE,
        L3_TUNNEL_TCAM_DATA_ONLYm, L3_TUNNEL_TCAMm,
        57, 1, 324, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_ADDR_MODE_FALSE,
        MY_STATION_TCAM_DATA_ONLYm, MY_STATION_TCAMm,
        8, 1, 178, 1, NULL},

    {BCMPTM_OINFO_MODE_TCAM_DATA, ALPM_ADDR_MODE_FALSE,
        MY_STATION_2_TCAM_DATA_ONLYm, MY_STATION_2_TCAMm,
        8, 1, 178, 1, NULL},
*/

    /* null entry */
    {BCMPTM_OINFO_MODE_NONE, FALSE,
        INVALIDm, INVALIDm,
        0, 0, 0, 0, NULL}
};

/* Note:
 *      Must not make CACHE0 type mems part of non-DEFAULT group.
 *      - We are not allocating mem for them, so no savings of mem is achieved
 *        by declaring their pt_index_count as 0.
 *      - Also, such views have no SER protection (and that is why we don't need
 *        per entry storage for such views in PTcache.
 */
static const bcmdrd_sid_t bcm56990_b0_em_pt_list[] = {
    EXACT_MATCH_4m, /* 32K */
    EXACT_MATCH_2m, /* 64K */
    EXACT_MATCH_ECCm, /* 128K */
    /* EXACT_MATCH_HIT_ONLYm 64K - overlay type CACHE0 */
};

static const bcmdrd_sid_t bcm56990_b0_l2_entry_uft_pt_list[] = {
    L2_ENTRY_UFT_SINGLEm, /* 128K */
    L2_ENTRY_UFT_ECCm /* 128K */
};

static const bcmdrd_sid_t bcm56990_b0_alpm_level2_pt_list[] = {
    L3_DEFIP_ALPM_LEVEL2m, /* 6K */
    /* L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm, 120K (1 bit each) - overlay type CACHE0 */
    L3_DEFIP_ALPM_LEVEL2_SINGLEm, /* 24K */
    L3_DEFIP_ALPM_LEVEL2_ECCm /* 24K */
};

static const bcmdrd_sid_t bcm56990_b0_alpm_level3_pt_list[] = {
    L3_DEFIP_ALPM_LEVEL3m, /* 64K */
    /* L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm, 1280K (1 bit each) - overlay type CACHE0 */
    L3_DEFIP_ALPM_LEVEL3_SINGLEm, /* 256K */
    L3_DEFIP_ALPM_LEVEL3_ECCm /* 256K */
};


/*******************************************************************************
 * Private Functions
 */
static int basic_comp(const void *x, const void *y)
{
    return (*(int *)x - *(int *)y);
}

static int pt_with_dfid_comp(const void *x, const void *y)
{
    struct pt_with_dfid_s *p1 = (struct pt_with_dfid_s *)x;
    struct pt_with_dfid_s *p2 = (struct pt_with_dfid_s *)y;

    return (int)p1->pt_sid - (int)p2->pt_sid;
}

static int overlay_info_comp(const void *x, const void *y)
{
    bcmptm_overlay_info_t *p1 = (bcmptm_overlay_info_t *)x;
    bcmptm_overlay_info_t *p2 = (bcmptm_overlay_info_t *)y;

    return (int)p1->n_sid - (int)p2->n_sid;
}

static int
bcm56990_b0_pt_attr_is_alpm(int unit, bcmdrd_sid_t sid, bool *is_alpm)
{
    int idx = -1;

    *is_alpm = FALSE;
    BIN_SEARCH(s_bcm56990_b0_overlay_info[unit], .n_sid,
               sid,
               COUNTOF(bcm56990_b0_overlay_info)- 2, /* don't use the last entry */
               idx);
    if (idx != -1) {
        *is_alpm = s_bcm56990_b0_overlay_info[unit][idx].is_alpm;
    }

    return SHR_E_NONE;
}

static int
bcm56990_b0_pt_overlay_info_get(int unit, bcmdrd_sid_t sid,
                                const bcmptm_overlay_info_t **oinfo)
{
    int idx = -1;

    BIN_SEARCH(s_bcm56990_b0_overlay_info[unit], .n_sid,
               sid,
               COUNTOF(bcm56990_b0_overlay_info)- 2, /* Don't use the last entry */
               idx);
    if (idx == -1) {
        idx = COUNTOF(bcm56990_b0_overlay_info) - 1; /* Point to the last element */
    }


    *oinfo = &s_bcm56990_b0_overlay_info[unit][idx];
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "For n_sid:%0d(%s) oinfo: "
                            "w_sid:%0d(%s), width_ne=%0d, "
                            "num_ne=%0d, is_alpm=%0d, "
                            "start_bit_ne_in_we=%0d, num_we_field=%0d, "
                            "we_field_array_ptr=%p, "
                            "oinfo_mode=%0d \n"),
                 sid,
                 bcmdrd_pt_sid_to_name(unit, sid),
                 (*oinfo)->w_sid,
                 bcmdrd_pt_sid_to_name(unit, (*oinfo)->w_sid),
                 (*oinfo)->width_ne,
                 (*oinfo)->num_ne,
                 (*oinfo)->is_alpm,
                 (*oinfo)->start_bit_ne_in_we,
                 (*oinfo)->num_we_field,
                 (void *)(*oinfo)->we_field_array,
                 /* casting to void because %p expects void *
                  * pedantic build will fail without casting */
                 (*oinfo)->mode));
    return SHR_E_NONE;
}

static int
bcm56990_b0_pt_tcam_km_format(int unit, bcmdrd_sid_t sid,
                              bcmptm_pt_tcam_km_format_t *km_format)
{
    switch (sid) {
    case ING_MIRROR_EVENT_PROFILEm:
    case ING_MIRROR_EVENT_PROFILE_ONLYm:
    case UDF_CONDITIONAL_CHECK_TABLE_CAMm:
        *km_format = BCMPTM_TCAM_KM_FORMAT_IS_KM; /* No conversion needed */
        break;
    default:
        *km_format = BCMPTM_TCAM_KM_FORMAT_LPT_7NM;
        break;
    }
    return SHR_E_NONE;
}

static int
bcm56990_b0_hw_does_km_to_xy(int unit, bool *hw_does_km_to_xy)
{
    *hw_does_km_to_xy = FALSE;
    return SHR_E_NONE;
}

static int
bcm56990_b0_tcam_corrupt_bits_enable(int unit, bool *tc_en)
{
    /* For chips where TCAM error detection and hence reporting is done in CMIC
     * module, same error can result in multiple notifications (because millions
     * of pkts could hit same corrupted entry). To suppress these SW stores a
     * bit for every TCAM entry - which gets set when SW read routine fails
     * because of SER and cleared when error is serviced.
     *
     * FALSE for TH4 - because TCAM error is reported via SER_FIFO
     */
    *tc_en = FALSE;
    return SHR_E_NONE;
}

static int
bcm56990_b0_pt_needs_dfid(int unit, bcmdrd_sid_t sid, uint8_t *dfid_bytes)
{
    int idx = -1;

    *dfid_bytes = 0;
    BIN_SEARCH(s_bcm56990_b0_pt_with_dfid[unit], .pt_sid,
               sid,
               COUNTOF(bcm56990_b0_pt_with_dfid) - 1,
               idx);

    if (idx != -1) {
        assert(s_bcm56990_b0_pt_with_dfid[unit][idx].size != 0);
        assert(s_bcm56990_b0_pt_with_dfid[unit][idx].size != 3);
        *dfid_bytes = s_bcm56990_b0_pt_with_dfid[unit][idx].size;
    }

    return SHR_E_NONE;
}

static int
bcm56990_b0_pt_needs_ltid(int unit, bcmdrd_sid_t sid, bool *ltid_en)
{
    int idx = -1;

    *ltid_en = FALSE;
    BIN_SEARCH(s_bcm56990_b0_pt_with_ltid[unit], ,
               sid,
               COUNTOF(bcm56990_b0_pt_with_ltid)- 1,
               idx);
    if (idx != -1) {
        *ltid_en = TRUE; /* is on pt_with_ltid_list */
    }

    return SHR_E_NONE;
}

static int
bcm56990_b0_pt_in_must_cache_list(int unit, bcmdrd_sid_t sid, bool *must_cache)
{
    uint32_t i;
    *must_cache = FALSE;
    for (i = 0; i < COUNTOF(bcm56990_b0_must_cache); i++) {
       if (bcm56990_b0_must_cache[i] == sid) {
           *must_cache = TRUE; /* is on must_cache_list */
           return SHR_E_NONE;
       }
    }
    return SHR_E_NONE;
}

static int
bcm56990_b0_pt_in_dont_cache_list(int unit, bcmdrd_sid_t sid, bool *dont_cache)
{
    uint32_t i;
    *dont_cache = FALSE;
    for (i = 0; i < COUNTOF(bcm56990_b0_dont_cache); i++) {
       if (bcm56990_b0_dont_cache[i] == sid) {
           *dont_cache = TRUE; /* is on dont_cache_list */
           return SHR_E_NONE;
       }
    }
    return SHR_E_NONE;
}

static int
bcm56990_b0_pt_group_info_get(int unit, uint32_t group_id, uint32_t *pt_count,
                              bool *bank_start_en, const bcmdrd_sid_t **pt_list)
{
    switch (group_id) {
    case PT_GROUP_DEFAULT:
        *pt_count = PT_GROUP_MAX; /* Return group_count (min can be 1) */
        *pt_list = NULL; /* We don't have list of SIDs in DEFAULT group */
        if (bank_start_en) {
            *bank_start_en = FALSE;
        }
        break;
    case PT_GROUP_EM:
        *pt_count = COUNTOF(bcm56990_b0_em_pt_list);
        *pt_list = bcm56990_b0_em_pt_list;
        if (bank_start_en) {
            *bank_start_en = FALSE;
        }
        break;
    case PT_GROUP_L2_ENTRY_UFT:
        *pt_count = COUNTOF(bcm56990_b0_l2_entry_uft_pt_list);
        *pt_list = bcm56990_b0_l2_entry_uft_pt_list;
        if (bank_start_en) {
            *bank_start_en = FALSE;
        }
        break;
    case PT_GROUP_ALPM_LEVEL2:
        *pt_count = COUNTOF(bcm56990_b0_alpm_level2_pt_list);
        *pt_list = bcm56990_b0_alpm_level2_pt_list;
        if (bank_start_en) {
            *bank_start_en = FALSE;
        }
        break;
    case PT_GROUP_ALPM_LEVEL3:
        *pt_count = COUNTOF(bcm56990_b0_alpm_level3_pt_list);
        *pt_list = bcm56990_b0_alpm_level3_pt_list;
        if (bank_start_en) {
            *bank_start_en = TRUE;
        }
        break;
    default:
        *pt_count = 0;
        *pt_list = NULL;
        if (bank_start_en) {
            *bank_start_en = FALSE;
        }
        return SHR_E_INTERNAL;
    } /* group_id */
    return SHR_E_NONE;
}

/* In:  bank_enum encoded in LT specific enum
 * Out: 0-based uft_bank number
 */
static int
bcm56990_b0_uft_bank_get(int unit, uint8_t bank_enum,
                         uint8_t *uft_bank)
{
    int i;
    uint32_t bank_enum_val;
    const char *bank_enum_str_list[] = {
        "UFT_BANK0", "UFT_BANK1", "UFT_BANK2", "UFT_BANK3",
        "UFT_BANK4", "UFT_BANK5", "UFT_BANK6", "UFT_BANK7"};
    SHR_FUNC_ENTER(unit);
    for (i = 0; i < 8; i++) {
       SHR_IF_ERR_EXIT(
           bcmlrd_field_symbol_to_value(unit, DEVICE_EM_GROUPt,
                                        DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf,
                                        bank_enum_str_list[i], &bank_enum_val));
       if (bank_enum_val == bank_enum) {
           *uft_bank = i;
           SHR_FUNC_EXIT(); /* Found */
       }
    }
    LOG_ERROR(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Could not find uft_bank_num for bank_enum = %0d\n"),
         bank_enum));
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

/* In:  0-based uft_bank number
 * Out: bank_enum encoded in LT specific enum
 */
static int
bcm56990_b0_bank_enum_get(int unit, uint8_t uft_bank,
                          uint8_t *bank_enum)
{
    uint32_t bank_enum_val;
    const char *bank_enum_str_list[] = {
        "UFT_BANK0", "UFT_BANK1", "UFT_BANK2", "UFT_BANK3",
        "UFT_BANK4", "UFT_BANK5", "UFT_BANK6", "UFT_BANK7"};
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(
        bcmlrd_field_symbol_to_value(unit, DEVICE_EM_GROUPt,
                                     DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf,
                                     bank_enum_str_list[uft_bank],
                                     &bank_enum_val));
    SHR_IF_ERR_EXIT(bank_enum_val > 0xFF ? SHR_E_INTERNAL : SHR_E_NONE);
    *bank_enum = bank_enum_val;
exit:
    SHR_FUNC_EXIT();
}

/* Set index_count for important PTs in a PT group.
 * And also set index_min for ALPM PTs. */
static int
bcm56990_b0_pt_group_index_count_set(int unit, uint32_t group_id,
                                     bcmptm_index_count_set_info_t
                                     *index_count_set_info,
                                     uint8_t *bank_count,
                                     uint8_t *start_bank_enum)
{
    uint32_t old_index_count;
    bool init, for_ser, for_non_ser;
    SHR_FUNC_ENTER(unit);
    init = index_count_set_info->init;
    for_ser = index_count_set_info->for_ser;
    for_non_ser = index_count_set_info->for_non_ser;

    if (!for_ser && !for_non_ser) {
        /* Cannot be here if no updates are needed. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    } else if (for_ser && !for_non_ser) {
        /* Only update SER values (to be in sync with non_ser values).
         * Return values are dont-care. */
        pt_size_exact_match_4[unit][SER] =
            pt_size_exact_match_4[unit][0];

        pt_size_l2_entry_uft[unit][SER] =
            pt_size_l2_entry_uft[unit][0];

        pt_size_l3_defip_alpm_level2[unit][SER] =
            pt_size_l3_defip_alpm_level2[unit][0];

        pt_size_l3_defip_alpm_level3[unit][SER] =
            pt_size_l3_defip_alpm_level3[unit][0];

        pt_index_min_l3_defip_alpm_level3[unit][SER] =
            pt_index_min_l3_defip_alpm_level3[unit][0];

        SHR_EXIT();
    }

    /* From here onwards:
     *      for_non_ser = 1 for sure.
     *      for_ser = 0 or 1
     */
    switch (group_id) {
    case PT_GROUP_DEFAULT:
        /* Nothing to adjust - all PTs in DEFAULT group have fixed size */
        break;

    case PT_GROUP_EM:
        if (init) {
            *bank_count = HW_MIN_BANK_COUNT_PT_GROUP_EM;
            pt_size_exact_match_4[unit][0] = HW_MIN_PT_SIZE_EXACT_MATCH_4;
        } else if (*bank_count <= HW_MAX_BANK_COUNT_PT_GROUP_EM) {
            old_index_count = pt_size_exact_match_4[unit][0];
            pt_size_exact_match_4[unit][0] = HW_MIN_PT_SIZE_EXACT_MATCH_4 +
                ((*bank_count - HW_MIN_BANK_COUNT_PT_GROUP_EM) *
                 HW_UFT_BANK_PT_SIZE_EXACT_MATCH_4);
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\nChange: PT_GROUP_EM: New bank_count "
                            "= %0d, widest_index_count change (%0d to %0d)\n"),
                 *bank_count, old_index_count, pt_size_exact_match_4[unit][0]));
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (for_ser) {
            pt_size_exact_match_4[unit][SER] =
                pt_size_exact_match_4[unit][0];
        }
        break; /* PT_GROUP_EM */

    case PT_GROUP_L2_ENTRY_UFT:
        if (init) {
            *bank_count = HW_MIN_BANK_COUNT_PT_GROUP_L2_ENTRY_UFT;
            pt_size_l2_entry_uft[unit][0] = HW_MIN_PT_SIZE_L2_ENTRY_UFT;
        } else if (*bank_count <= HW_MAX_BANK_COUNT_PT_GROUP_L2_ENTRY_UFT) {
            old_index_count = pt_size_l2_entry_uft[unit][0];
            pt_size_l2_entry_uft[unit][0] = HW_MIN_PT_SIZE_L2_ENTRY_UFT +
                ((*bank_count - HW_MIN_BANK_COUNT_PT_GROUP_L2_ENTRY_UFT) *
                 HW_UFT_BANK_PT_SIZE_L2_ENTRY_UFT);
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\nChange: PT_GROUP_L2_ENTRY_UFT: New bank_count "
                            "= %0d, widest_index_count change (%0d to %0d)\n"),
                 *bank_count, old_index_count, pt_size_l2_entry_uft[unit][0]));
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (for_ser) {
            pt_size_l2_entry_uft[unit][SER] =
                pt_size_l2_entry_uft[unit][0];
        }
        break; /* PT_GROUP_L2_ENTRY_UFT */

    case PT_GROUP_ALPM_LEVEL2:
        if (init) {
            *bank_count = HW_MAX_BANK_COUNT_PT_GROUP_ALPM_LEVEL2;
            pt_size_l3_defip_alpm_level2[unit][0] =
                *bank_count * HW_UFT_BANK_PT_SIZE_L3_DEFIP_ALPM_LEVEL2;
        } else if (*bank_count <= HW_MAX_BANK_COUNT_PT_GROUP_ALPM_LEVEL2) {
            old_index_count = pt_size_l3_defip_alpm_level2[unit][0];
            pt_size_l3_defip_alpm_level2[unit][0] =
                HW_MIN_PT_SIZE_L3_DEFIP_ALPM_LEVEL2 +
                ((*bank_count - HW_MIN_BANK_COUNT_PT_GROUP_ALPM_LEVEL2) *
                 HW_UFT_BANK_PT_SIZE_L3_DEFIP_ALPM_LEVEL2);
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\nChange: PT_GROUP_ALPM_LEVEL2: New "
                            "bank_count = %0d, widest_index_count change "
                            "(%0d to %0d)\n"),
                 *bank_count, old_index_count,
                 pt_size_l3_defip_alpm_level2[unit][0]));
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (for_ser) {
            pt_size_l3_defip_alpm_level2[unit][SER] =
                pt_size_l3_defip_alpm_level2[unit][0];
        }
        break; /* PT_GROUP_ALPM_LEVEL2 */

    case PT_GROUP_ALPM_LEVEL3:
        if (!start_bank_enum) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (init) {
            *bank_count = HW_MIN_BANK_COUNT_PT_GROUP_ALPM_LEVEL3;
            SHR_IF_ERR_EXIT(
                bcm56990_b0_bank_enum_get(unit, 0, /* init uft_bank */
                                          start_bank_enum));
            /* init uft_bank, start_bank_enum should be x as bank_count is 0 */

            pt_size_l3_defip_alpm_level3[unit][0] =
                HW_MIN_PT_SIZE_L3_DEFIP_ALPM_LEVEL3;
            pt_index_min_l3_defip_alpm_level3[unit][0] = 0; /* x as index_count = 0 */

        } else if (*bank_count <= HW_MAX_BANK_COUNT_PT_GROUP_ALPM_LEVEL3) {
            uint32_t old_index_min;
            uint8_t uft_bank_num;
            old_index_count = pt_size_l3_defip_alpm_level3[unit][0];
            pt_size_l3_defip_alpm_level3[unit][0] =
                HW_MIN_PT_SIZE_L3_DEFIP_ALPM_LEVEL3 +
                ((*bank_count - HW_MIN_BANK_COUNT_PT_GROUP_ALPM_LEVEL3) *
                 HW_UFT_BANK_PT_SIZE_L3_DEFIP_ALPM_LEVEL3);

            old_index_min = pt_index_min_l3_defip_alpm_level3[unit][0];

            if (*bank_count) {
                SHR_IF_ERR_EXIT(
                    bcm56990_b0_uft_bank_get(unit, *start_bank_enum,
                                             &uft_bank_num));
            } else {
                uft_bank_num = 0; /* x as *bank_count is 0 */
            }
            pt_index_min_l3_defip_alpm_level3[unit][0] =
                uft_bank_num * HW_UFT_BANK_PT_SIZE_L3_DEFIP_ALPM_LEVEL3;

            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\nChange: PT_GROUP_ALPM_LEVEL3: New "
                            "bank_count = %0d, "
                            "index_min change (%0d to %0d), "
                            "widest_index_count change (%0d to %0d). \n"),
                 *bank_count,
                 old_index_min, pt_index_min_l3_defip_alpm_level3[unit][0],
                 old_index_count, pt_size_l3_defip_alpm_level3[unit][0]));
        } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (for_ser) {
            pt_size_l3_defip_alpm_level3[unit][SER] =
                pt_size_l3_defip_alpm_level3[unit][0];

            pt_index_min_l3_defip_alpm_level3[unit][SER] =
                pt_index_min_l3_defip_alpm_level3[unit][0];
        }
        break; /* PT_GROUP_ALPM_LEVEL3 */

    default: /* UNSUPPORTED group */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    } /* group_id */

exit:
    SHR_FUNC_EXIT();
}

static int /* not exported */
bcm56990_b0_pt_index_count_get(int unit, bcmdrd_sid_t sid, bool acc_for_ser,
                               uint32_t *index_count)
{
    int s = 0;
    if (acc_for_ser) {
        s = SER;
    }
    switch (sid) {
    /* PT_GROUP_EM */
    case EXACT_MATCH_4m: /* 32K */
        *index_count = pt_size_exact_match_4[unit][s];
        break;
    case EXACT_MATCH_2m: /* 64K */
        *index_count = pt_size_exact_match_4[unit][s] * 2;
        break;
    case EXACT_MATCH_ECCm: /* 128K */
        *index_count = pt_size_exact_match_4[unit][s] * 4;
        break;

    /* PT_GROUP_L2_ENTRY_UFT */
    case L2_ENTRY_UFT_SINGLEm: /* 128K */
        *index_count = pt_size_l2_entry_uft[unit][s];
        break;
    case L2_ENTRY_UFT_ECCm: /* 128K */
        *index_count = pt_size_l2_entry_uft[unit][s];
        break;

    /* PT_GROUP_ALPM_LEVEL2 */
    case L3_DEFIP_ALPM_LEVEL2m: /* 6K */
        *index_count = pt_size_l3_defip_alpm_level2[unit][s];
        break;
    case L3_DEFIP_ALPM_LEVEL2_SINGLEm: /* 24K */
        *index_count = pt_size_l3_defip_alpm_level2[unit][s] * 4;
        break;
    case L3_DEFIP_ALPM_LEVEL2_ECCm: /* 24K */
        *index_count = pt_size_l3_defip_alpm_level2[unit][s] * 4;
        break;

    /* PT_GROUP_ALPM_LEVEL3 */
    case L3_DEFIP_ALPM_LEVEL3m: /* 64K */
        *index_count = pt_size_l3_defip_alpm_level3[unit][s];
        break;
    case L3_DEFIP_ALPM_LEVEL3_SINGLEm: /* 256K */
        *index_count = pt_size_l3_defip_alpm_level3[unit][s] * 4;
        break;
    case L3_DEFIP_ALPM_LEVEL3_ECCm: /* 256K */
        *index_count = pt_size_l3_defip_alpm_level3[unit][s] * 4;
        break;

    default:
        return SHR_E_UNAVAIL;
    } /* sid */
    return SHR_E_NONE;
}

static int
bcm56990_b0_pt_index_min_get(int unit, bcmdrd_sid_t sid, bool acc_for_ser)
{
    int index_min;
    int s = 0;
    if (acc_for_ser) {
        s = SER;
    }
    switch (sid) {
    /* PT_GROUP_ALPM_LEVEL3 */
    case L3_DEFIP_ALPM_LEVEL3m: /* 64K */
        index_min = pt_index_min_l3_defip_alpm_level3[unit][s];
        break;
    case L3_DEFIP_ALPM_LEVEL3_SINGLEm: /* 256K */
    case L3_DEFIP_ALPM_LEVEL3_ECCm: /* 256K */
        index_min = pt_index_min_l3_defip_alpm_level3[unit][s] * 4;
        break;
    default:
        index_min = bcmdrd_pt_index_min(unit, sid);
        break;
    } /* sid */
    return index_min;
}

static int
bcm56990_b0_pt_index_max_get(int unit, bcmdrd_sid_t sid, bool acc_for_ser)
{
    int index_min, tmp_rv;
    uint32_t index_count = 0;
    tmp_rv = bcm56990_b0_pt_index_count_get(unit, sid, acc_for_ser,
                                            &index_count);
    if (tmp_rv == SHR_E_NONE) {
        /* we have bank based index_count, so return bank based index_max */
        index_min = bcm56990_b0_pt_index_min_get(unit, sid, acc_for_ser);
        if (index_min >= 0) {
            return index_min + index_count - 1;
        } else {
            return index_min; /* some error */
        }
    } else {
        return bcmdrd_pt_index_max(unit, sid);
    }
}

static int
bcm56990_b0_wal_info_get(int unit, bcmptm_wal_info_t *wal_info)
{
    wal_info->msg_max_count = 12300;
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_bcm56990_b0_fnptr_attach(int unit, fn_ptr_all_t *fn_ptr_all)
{
    void *tmp_alloc_ptr;
    SHR_FUNC_ENTER(unit);
    fn_ptr_all->fn_rsp_info_get = NULL; /* use default logic */
    fn_ptr_all->fn_wal_info_get = &bcm56990_b0_wal_info_get;
    fn_ptr_all->fn_pt_attr_is_alpm = &bcm56990_b0_pt_attr_is_alpm;
    fn_ptr_all->fn_pt_overlay_info_get = &bcm56990_b0_pt_overlay_info_get;
    fn_ptr_all->fn_pt_tcam_km_format = &bcm56990_b0_pt_tcam_km_format;
    fn_ptr_all->fn_hw_does_km_to_xy = &bcm56990_b0_hw_does_km_to_xy;
    fn_ptr_all->fn_tcam_corrupt_bits_enable = &bcm56990_b0_tcam_corrupt_bits_enable;
    fn_ptr_all->fn_pt_needs_ltid = &bcm56990_b0_pt_needs_ltid;
    fn_ptr_all->fn_pt_needs_dfid = &bcm56990_b0_pt_needs_dfid;
    fn_ptr_all->fn_pt_in_must_cache_list = &bcm56990_b0_pt_in_must_cache_list;
    fn_ptr_all->fn_pt_in_dont_cache_list = &bcm56990_b0_pt_in_dont_cache_list;
    fn_ptr_all->fn_pt_group_index_count_set = &bcm56990_b0_pt_group_index_count_set;
    fn_ptr_all->fn_pt_index_min_get = &bcm56990_b0_pt_index_min_get;
    fn_ptr_all->fn_pt_index_max_get = &bcm56990_b0_pt_index_max_get;
    fn_ptr_all->fn_pt_group_info_get = &bcm56990_b0_pt_group_info_get;
    fn_ptr_all->fn_pt_rm_tcam_memreg_oper = &bcm56990_b0_rm_tcam_memreg_oper;
    fn_ptr_all->fn_pt_rm_tcam_prio_only_hw_key_info_init =
            &bcm56990_b0_rm_tcam_prio_only_hw_key_info_init;
    /* CCI */
    fn_ptr_all->fn_pt_cci_ctrtype_get = &bcm56990_b0_pt_cci_ctrtype_get;
    fn_ptr_all->fn_pt_cci_frmt_sym_get = &bcm56990_b0_pt_cci_frmt_sym_get;
    fn_ptr_all->fn_pt_cci_ctr_sym_map = &bcm56990_b0_pt_cci_ctr_sym_map;
    fn_ptr_all->fn_pt_cci_frmt_index_from_port =
            &bcm56990_b0_pt_cci_frmt_index_from_port;
    fn_ptr_all->fn_pt_cci_index_valid = &bcm56990_b0_pt_cci_index_valid;
    fn_ptr_all->fn_pt_cci_reg_addr_gap_get =
            &bcm56990_b0_pt_cci_reg_addr_gap_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_control_get =
            &bcm56990_b0_pt_cci_ctr_evict_control_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_enable_field_get =
            &bcm56990_b0_pt_cci_ctr_evict_enable_field_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_clr_on_read_field_get =
            &bcm56990_b0_pt_cci_ctr_evict_clr_on_read_field_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_default_enable_list_get =
            &bcm56990_b0_pt_cci_ctr_evict_default_enable_list_get;
    fn_ptr_all->fn_pt_cci_ctr_is_bypass = &bcm56990_b0_pt_cci_ctr_is_bypass;
    fn_ptr_all->fn_pt_cci_fifodma_chan_get =
            &bcm56990_b0_pt_cci_fifodma_chan_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_fifo_enable_sym_get =
            &bcm56990_b0_pt_cci_ctr_evict_fifo_enable_sym_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_fifo_sym_get =
            &bcm56990_b0_pt_cci_ctr_evict_fifo_sym_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_fifo_entry_get =
            &bcm56990_b0_pt_cci_ctr_evict_fifo_entry_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_fifo_buf_size_get =
            &bcm56990_b0_pt_cci_ctr_evict_fifo_buf_size_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_field_num_get =
            &bcm56990_b0_pt_cci_ctr_evict_field_num_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_field_width_get =
            &bcm56990_b0_pt_cci_ctr_evict_field_width_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_normal_val_get =
            &bcm56990_b0_pt_cci_ctr_evict_normal_val_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_wide_val_get =
            &bcm56990_b0_pt_cci_ctr_evict_wide_val_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_slim_val_get =
            &bcm56990_b0_pt_cci_ctr_evict_slim_val_get;
    fn_ptr_all->fn_pt_cci_ctr_evict_update_mode_num_get =
            &bcm56990_b0_pt_cci_ctr_evict_update_mode_num_get;
    /* SER */
    fn_ptr_all->fn_ser_data_driver_register = bcm56990_b0_ptm_ser_data_driver_register;
    /* SER END */

    /* UFT */
    fn_ptr_all->fn_uft_driver_register = &bcm56990_b0_uft_driver_register;
    fn_ptr_all->fn_uft_be_driver_register = &bcm56990_b0_uft_be_driver_register;
    fn_ptr_all->fn_cth_alpm_driver_register = &bcm56990_b0_cth_alpm_driver_register;
    fn_ptr_all->fn_alpm_driver_register = &bcm56990_b0_rm_alpm_driver_register;

    /* sort large arrays for quick (binary) lookup */
tmp_alloc_ptr = NULL;
    SHR_ALLOC(tmp_alloc_ptr, sizeof(bcm56990_b0_pt_with_ltid), "bcmptmPtWithLtid");
    SHR_NULL_CHECK(tmp_alloc_ptr, SHR_E_MEMORY);
    sal_memcpy(tmp_alloc_ptr, bcm56990_b0_pt_with_ltid,
               sizeof(bcm56990_b0_pt_with_ltid));
    s_bcm56990_b0_pt_with_ltid[unit] = tmp_alloc_ptr;

    tmp_alloc_ptr = NULL;
    SHR_ALLOC(tmp_alloc_ptr, sizeof(bcm56990_b0_overlay_info), "bcmptmPtOverlayInfo");
    SHR_NULL_CHECK(tmp_alloc_ptr, SHR_E_MEMORY);
    sal_memcpy(tmp_alloc_ptr, bcm56990_b0_overlay_info,
               sizeof(bcm56990_b0_overlay_info));
    s_bcm56990_b0_overlay_info[unit] = tmp_alloc_ptr;

    tmp_alloc_ptr = NULL;
    SHR_ALLOC(tmp_alloc_ptr, sizeof(bcm56990_b0_pt_with_dfid), "bcmptmPtWithDfid");
    SHR_NULL_CHECK(tmp_alloc_ptr, SHR_E_MEMORY);
    sal_memcpy(tmp_alloc_ptr, bcm56990_b0_pt_with_dfid,
               sizeof(bcm56990_b0_pt_with_dfid));
    s_bcm56990_b0_pt_with_dfid[unit] = tmp_alloc_ptr;

    sal_qsort(s_bcm56990_b0_pt_with_ltid[unit],
              COUNTOF(bcm56990_b0_pt_with_ltid),
              sizeof(bcm56990_b0_pt_with_ltid[0]),
              basic_comp);

    sal_qsort(s_bcm56990_b0_overlay_info[unit],
              COUNTOF(bcm56990_b0_overlay_info)-1, /* Keep the last element in place */
              sizeof(bcm56990_b0_overlay_info[0]),
              overlay_info_comp);

    sal_qsort(s_bcm56990_b0_pt_with_dfid[unit],
              COUNTOF(bcm56990_b0_pt_with_dfid),
              sizeof(bcm56990_b0_pt_with_dfid[0]),
              pt_with_dfid_comp);

    /* assign all members of fn_ptr_all[unit] as needed */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_bcm56990_b0_fnptr_detach(int unit)
{
    if (s_bcm56990_b0_pt_with_ltid[unit]) {
        SHR_FREE(s_bcm56990_b0_pt_with_ltid[unit]);
    }
    if (s_bcm56990_b0_overlay_info[unit]) {
        SHR_FREE(s_bcm56990_b0_overlay_info[unit]);
    }
    if (s_bcm56990_b0_pt_with_dfid[unit]) {
        SHR_FREE(s_bcm56990_b0_pt_with_dfid[unit]);
    }
    return SHR_E_NONE;
}

/******************************************************************************
 UFT banks:

Totally 8 UFT banks: 32K x 120

L2_ENTRY_UFT_SINGLEm: 128K x 120.
    Can only use UFT banks B0-B3
    4 banks => each bank is 32K x 120
    Can be configured to work in 0 banks (dedicated 8K),
                                 4 banks (128K x 120),
                                 2 banks (64K x 120) modes

EM can also only use 4 banks: B0-B3
    EXACT_MATCH_2m: 64K x 240, 4 banks => each bank = 16K x 240
    EXACT_MATCH_4m: 32K x 480, 4 banks => each bank = 8K x 480

L3_DEFIP_ALPM_LEVEL3_SINGLEm, QUADm: 256K x 120, 64K x 480b
    Can use all 8 banks => each bank is 32K x 120 (or 8K x 480b)
    When B0-B3 are used by EM, ALPM can use upper banks b4-b7 (will have
                                                               non-zero start
                                                               addr)
 */

/******************************************************************************
# Hit only views
L2_HITDA_ONLYm,
L2_HITDA_UFTm,
L2_HITSA_ONLYm,
L2_HITSA_UFTm,
L3_DEFIP_LEVEL1_HIT_ONLYm,
L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm,
L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm,
EXACT_MATCH_HIT_ONLYm,

# Hash views
EGR_ADAPT
EXACT_MATCH
L2_ENTRY
L2_ENTRY_UFT
L3_TUNNEL
MPLS_ENTRY

# Tcam aggregates:
CPU_COS_MAP (=CPU_COS_MAP_ONLY +)
EXACT_MATCH_LOGICAL_TABLE_SELECT (=EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY +
assoc_data)
IFP_LOGICAL_TABLE_SELECT (=IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY + assoc_data)
ING_MIRROR_EVENT_PROFILE (=ING_MIRROR_EVENT_PROFILE_ONLY + assoc_data)
L2_USER_ENTRY (=MEMORY L2_USER_ENTRY_ONLY + assoc_data)
MY_STATION_TCAM (=MY_STATION_TCAM_ENTRY_ONLY + assoc_data)
MY_STATION_2_TCAM (=MY_STATION_2_TCAM_ENTRY_ONLY + assoc_data)

L3_TUNNEL_TCAM (=L3_TUNNEL_TCAM_ONLY + assoc_data)
L3_TUNNEL_PAIR_TCAM (=paired view of L3_TUNNEL_TCAM + associated data). There is
no _ONLY paired view). It appears like another way of accessing L3_TUNNEL_TCAM.

# Tcam Non aggregates:
EFP_TCAM
UDF_TCAM
IFP_TCAM
UDF_CONDITIONAL_CHECK_TABLE_CAM
VFP_TCAM

# ALPM related TCAMs - not sure if they are TCAM_ONLY or TCAM+SRAM views
L3_DEFIP_LEVEL1_WIDE
L3_DEFIP_PAIR_LEVEL1_WIDE
L3_DEFIP_LEVEL1
L3_DEFIP_TCAM_LEVEL1_WIDE
L3_DEFIP_PAIR_LEVEL1
L3_DEFIP_TCAM_LEVEL1

# TCAMs with encoding NONE
ING_MIRROR_EVENT_PROFILE
ING_MIRROR_EVENT_PROFILE_ONLY
UDF_CONDITIONAL_CHECK_TABLE_CAM

# _ECC views
EGR_ADAPT_ECC
EXACT_MATCH_ECC
L2_ENTRY_ECC
L3_TUNNEL_ECC
MPLS_ENTRY_ECC
L3_DEFIP_ALPM_LEVEL3_ECC
L3_DEFIP_ALPM_LEVEL2_ECC
L2_ENTRY_UFT_ECC
 */
