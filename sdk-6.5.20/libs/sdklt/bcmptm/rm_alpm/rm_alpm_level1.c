/*! \file rm_alpm_level1.c
 *
 * Resource Manager for ALPM Level-1
 *
 * This file contains the resource manager for ALPM Level-1
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
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/bcmltd_lt_types.h>
#include "bcmptm/bcmptm_ser_misc_internal.h"
#include "bcmptm/bcmptm_ser_internal.h"
#include "rm_alpm_common.h"
#include "rm_alpm.h"
#include "rm_alpm_device.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"
#include <bcmptm/generated/bcmptm_rm_alpm_ha.h>
#include <bcmissu/issu_api.h>

/*******************************************************************************
 * Defines
 */

#define UNPAIR  TRUE
#define PAIR    FALSE
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPM

#define INVALID_PFX -1
#define L1_ENTRY_ALLOC_CHUNK_SIZE  4

#define L1_HIT_DT_BITMAP    0
#define L1_DT_BITMAP_BITS      16

/*
 * This translates the single index, also the index used by prefix management,
 * to hardware hit index, in hit version 1. It works for pair entry as well.
 */
#define L1_HIT_INDEX(u, m, index) \
    (HW_INDEX(index) << 1) | IS_HALF_R_INDEX(index)

#define MAX_L1_HIT_BITS  2
/*! Level-1 info */
#define L1_HA(u, m)                         (alpm_l1_ha[u][m])
#define L1_INFO(u, m)                       (alpm_l1_info[u][m])
#define L1_DB(u, m, db)                     (alpm_l1_info[u][m]->l1_db[db])
#define L1_KI(u, m, db, _pkm)               (alpm_l1_info[u][m]->l1_db[db].ki[_pkm])
#define L1_INFO_FD(u, m, _f)                L1_INFO(u, m)->_f
#define L1_INFO_FDM(u, m, _f, _i)           ((L1_INFO(u, m)->_f)[(_i)])
#define KI_BLOCKS(u, m, db, _pkm)           L1_KI(u, m, db, _pkm).blocks
#define KI_SIZE(u, m, db, _pkm)             L1_KI(u, m, db, _pkm).block_size
#define KI_START(u, m, db, _pkm)            L1_KI(u, m, db, _pkm).block_start
#define KI_END(u, m, db, _pkm)              L1_KI(u, m, db, _pkm).block_end
#define KI_VALID(u, m, db, _pkm)            L1_KI(u, m, db, _pkm).valid
#define RANGE1_FIRST(u, m, db, _pkm)        L1_KI(u, m, db, _pkm).range1_first
#define RANGE2_FIRST(u, m, db, _pkm)        L1_KI(u, m, db, _pkm).range2_first
#define RANGE1_LAST(u, m, db, _pkm)         L1_KI(u, m, db, _pkm).range1_last
#define RANGE1_LAST_PFX(u, m, db, _pkm)     L1_KI(u, m, db, _pkm).range1_last_pfx
/*! Valid entry span. */
#define LAST_PFX_SPAN(u, m, db, _pkm)       L1_KI(u, m, db, _pkm).last_pfx_span
#define MAX_PFX(u, m, db, la, _pkm)         L1_KI(u, m, db, _pkm).max_pfx[la]
#define UNPAIR_MAX_PFX(u, m, db, _pkm)      L1_KI(u, m, db, _pkm).max_pfx[IP_VER_6]
#define BASE_PFX(u, m, db, la, _pkm)        L1_KI(u, m, db, _pkm).base_pfx[la]
#define UNPAIR_BASE_PFX(u, m, db, _pkm)     L1_KI(u, m, db, _pkm).base_pfx[IP_VER_6]

#define DUAL_MODE(u, m)                     L1_INFO(u, m)->dual_mode
#define NO_PKM_S(u, m)                      L1_INFO(u, m)->no_pkm_s
#define PAIR_SID(u, m)                      L1_INFO(u, m)->pair_sid
#define UNPAIR_SID(u, m)                    L1_INFO(u, m)->unpair_sid
#define WIDE_PAIR_SID(u, m)                 L1_INFO(u, m)->wide_pair_sid
#define WIDE_UNPAIR_SID(u, m)               L1_INFO(u, m)->wide_unpair_sid
#define TCAM_ONLY_SID(u, m)                 L1_INFO(u, m)->tcam_only_sid
#define DATA_ONLY_SID(u, m)                 L1_INFO(u, m)->data_only_sid
#define WIDE_TCAM_ONLY_SID(u, m)            L1_INFO(u, m)->wide_tcam_only_sid
#define WIDE_DATA_ONLY_SID(u, m)            L1_INFO(u, m)->wide_data_only_sid
#define TCAM_DEPTH(u, m)                    L1_INFO(u, m)->tcam_depth
#define MAX_TCAMS(u, m)                     L1_INFO(u, m)->tcam_blocks
#define L1_BLOCKS(u, m)                     L1_INFO(u, m)->l1_blocks
#define L1_BLOCK_START(u, m)                L1_INFO(u, m)->l1_block_start
#define L1_BLOCK_END(u, m)                  L1_INFO(u, m)->l1_block_end
#define L1_HIT_SID(u, m)                    L1_INFO(u, m)->l1_hit_sid
#define KEY_DATA_SEPARATE(u, m, is_dw)      L1_INFO(u, m)->key_data_separate[(is_dw)]
#define L1_TCAMS_SHARED(u, m)               L1_INFO(u, m)->l1_tcams_shared

/*! PAIR statistics */
#define L1_STATS(u, m, db, _pkm)                  (L1_KI(u, m, db, _pkm).block_stats)
#define L1_USED_COUNT(u, m, db, kt, vt, _pkm)     L1_STATS(u, m, db, _pkm).used_count[kt][vt]
#define L1_MAX_COUNT(u, m, db, kt, _pkm)          L1_STATS(u, m, db, _pkm).max_count[kt]
#define L1_HALF_COUNT(u, m, db, kt, vt, _pkm)     L1_STATS(u, m, db, _pkm).half_count[kt][vt]


/*! UNPAIR states */
#define UNPAIR_STATE(u, m, db, _pkm)             (L1_KI(u, m, db, _pkm).block_state)
#define UNPAIR_STATE_START(u, m, db, pfx, _pkm)  (UNPAIR_STATE(u, m, db, _pkm)[(pfx)].start)
#define UNPAIR_STATE_END(u, m, db, pfx, _pkm)    (UNPAIR_STATE(u, m, db, _pkm)[(pfx)].end)
#define UNPAIR_STATE_PREV(u, m, db, pfx, _pkm)   (UNPAIR_STATE(u, m, db, _pkm)[(pfx)].prev)
#define UNPAIR_STATE_NEXT(u, m, db, pfx, _pkm)   (UNPAIR_STATE(u, m, db, _pkm)[(pfx)].next)
#define UNPAIR_STATE_VENT(u, m, db, pfx, _pkm)   (UNPAIR_STATE(u, m, db, _pkm)[(pfx)].vent)
#define UNPAIR_STATE_FENT(u, m, db, pfx, _pkm)   (UNPAIR_STATE(u, m, db, _pkm)[(pfx)].fent)
#define UNPAIR_STATE_HENT(u, m, db, pfx, _pkm)   (UNPAIR_STATE(u, m, db, _pkm)[(pfx)].hent)

/*! PAIR states */
#define PAIR_STATE(u, m, db, _pkm)               (L1_KI(u, m, db, _pkm).block_state)
#define PAIR_STATE_START(u, m, db, pfx, _pkm)    (PAIR_STATE(u, m, db, _pkm)[(pfx)].start)
#define PAIR_STATE_END(u, m, db, pfx, _pkm)      (PAIR_STATE(u, m, db, _pkm)[(pfx)].end)
#define PAIR_STATE_PREV(u, m, db, pfx, _pkm)     (PAIR_STATE(u, m, db, _pkm)[(pfx)].prev)
#define PAIR_STATE_NEXT(u, m, db, pfx, _pkm)     (PAIR_STATE(u, m, db, _pkm)[(pfx)].next)
#define PAIR_STATE_VENT(u, m, db, pfx, _pkm)     (PAIR_STATE(u, m, db, _pkm)[(pfx)].vent)
#define PAIR_STATE_FENT(u, m, db, pfx, _pkm)     (PAIR_STATE(u, m, db, _pkm)[(pfx)].fent)
#define PAIR_STATE_HENT(u, m, db, pfx, _pkm)     (PAIR_STATE(u, m, db, _pkm)[(pfx)].hent)

#define PVT_INFO_ARRAY(u, m)           (L1_INFO(u, m)->pivot_info)
#define L1_PVT_INFO(u, m, index)       (L1_INFO(u, m)->pivot_info[index])

#define L1_HA_RANGE1_FIRST_DB0(u, m, pkm)   (alpm_l1_ha[u][m]->range1_first_db0[pkm])
#define L1_HA_RANGE1_FIRST_DB1(u, m, pkm)   (alpm_l1_ha[u][m]->range1_first_db1[pkm])
#define L1_HA_RANGE1_FIRST_DB2(u, m, pkm)   (alpm_l1_ha[u][m]->range1_first_db2[pkm])
#define L1_HA_RANGE1_FIRST_DB3(u, m, pkm)   (alpm_l1_ha[u][m]->range1_first_db3[pkm])

#define DIR_UP      1
#define DIR_DOWN    2
#define DIR_BOTH    3

/*! Pivot info */
#define PIVOT_INDEX(u, m, _i)          L1_PVT_INFO(u, m, _i)->index

#ifdef ALPM_DEBUG
#define SANITY_CHECK_PAIR_ENTRY(u, m, de, se, pkm)     \
    sanity_check_pair_entry(u, m, de, se, pkm)
#define SANITY_CHECK_PAIR_STATE(u, m, ldb, pfx, pkm)   \
    sanity_check_pair_state(u, m, ldb, pfx, pkm)
#define SANITY_CHECK_PAIR_PFX(u, m, ldb, pfx, pkm)     \
    sanity_check_pair_pfx(u, m, ldb, pfx, pkm)
#define SANITY_CHECK_PIVOT(u, m, pidx, p, f, c)        \
    if (c) sanity_check_pivot(u, m, pidx, p, f)
#define SANITY_CHECK_UNPAIR_ENTRY(u, m, de, se, pkm)   \
    sanity_check_unpair_entry(u, m, de, se, pkm)
#define SANITY_CHECK_UNPAIR_STATE(u, m, ldb, fx, pkm)  \
    sanity_check_unpair_state(u, m, ldb, pfx, pkm)
#define SANITY_CHECK_UNPAIR_PFX(u, m, ldb, pfx, pkm)   \
    sanity_check_unpair_pfx(u, m, ldb, pfx, pkm)
#else
#define SANITY_CHECK_PAIR_ENTRY(u, m, de, se, pkm)
#define SANITY_CHECK_PAIR_STATE(u, m, ldb, pfx, pkm)
#define SANITY_CHECK_PAIR_PFX(u, m, ldb, pfx, pkm)
#define SANITY_CHECK_PIVOT(u, m, pidx, p, f, c)
#define SANITY_CHECK_UNPAIR_ENTRY(u, m, de, se, pkm)
#define SANITY_CHECK_UNPAIR_STATE(u, m, ldb, fx, pkm)
#define SANITY_CHECK_UNPAIR_PFX(u, m, ldb, pfx, pkm)
#endif

/*******************************************************************************
  Typedefs
 */
typedef struct insert_pkms_s {
    int v[PKM_NUM];
} insert_pkms_t;

typedef struct l1_entry_s {
    L1_ENTRY_t de;
    L1_ENTRY_t se; /* or upper part of IPv6-128 */

    /*!< Pointer to the next element in the list (or NULL). */
    struct l1_entry_s *next;
} l1_entry_t;
/*******************************************************************************
 * Private variables
 */
static const int alpm_pkm_kt_weight[PKM_NUM][KEY_TYPES] = {
/* IPV4 IPV6_S IPV6_D IPV6_Q L3MC_V4 L3MC_V6 COMP0_V4 COMP0_V6 COMP1_V4 COMP1_V6 */
   {1,  1,      0,       0,      0,      0,      0,       0,       0,       0}, /* PKM_S */
   {1,  2,      2,       0,      0,      0,      0,       0,       0,       0}, /* PKM_D */
   {1,  4,      4,       4,      0,      0,      0,       0,       0,       0}, /* PKM_Q */
   {1,  1,      0,       0,      2,      4,      0,       0,       0,       0}, /* PKM_WS */
   {1,  2,      2,       2,      2,      4,      0,       0,       0,       0}, /* PKM_WD */
   {1,  4,      4,       4,      2,      4,      0,       0,       0,       0}, /* PKM_WQ */
   {1,  1,      0,       0,      0,      0,      0,       0,       0,       0}, /* PKM_SS */
   {1,  2,      2,       0,      0,      0,      0,       0,       0,       0}, /* PKM_SD */
   {1,  4,      4,       4,      0,      0,      0,       0,       0,       0}, /* PKM_SQ */
   {0,  0,      0,       0,      0,      0,      2,       4,       4,       4}, /* PKM_FP */
   {0,  0,      0,       0,      0,      0,      2,       4,       4,       4}  /* PKM_SFP */
};
static alpm_l1_info_t *alpm_l1_info[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern shr_lmm_hdl_t bcmptm_rm_alpm_arg_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern alpm_internals_t
    bcmptm_rm_alpm_internals[BCMDRD_CONFIG_MAX_UNITS][ALPMS][DBS][LEVELS];
static bcmptm_rm_alpm_l1_ha_t *alpm_l1_ha[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
/*******************************************************************************
 * Private Functions (declaration)
 */
static int read_l1_raw(int u, int m, bcmdrd_sid_t sid, int index, uint32_t *de,
                       uint32_t *se, uint32_t *dt_bitmap);
static int write_l1_raw(int u, int m, bcmdrd_sid_t sid, int index, uint32_t *de,
                        uint32_t *se, uint8_t hit_op,
                        uint32_t modify_bitmap, uint32_t dt_bitmap);
static int clear_l1_raw(int u, int m, bcmdrd_sid_t sid, int index);
static int pair_free_slot_move_down(int u, int m, uint8_t ldb, int pfx, int free_pfx, int pkm);
static int pair_free_slot_move_up(int u, int m, uint8_t ldb, int pfx, int free_pfx, int pkm);
static int arr_idx_from_pvt_idx(int u, int m, int pivot_index);
static inline int half_entry_key_type(int u, int m, alpm_key_type_t kt, int upkm);
static inline int pair_index_encode(int u, int m, int idx);
static inline bool pfx_is_single_half(int u, int m, uint8_t ldb, int pfx, int pkm);
static int pair_insert(int u, int m, alpm_arg_t *arg, int pkm);
static int read_l1_raw2(int u, int m, alpm_sid_type_t sid_type, int index, uint8_t wide,
                        uint32_t *de, uint32_t *dt_bitmap);
static int write_l1_raw2(int u, int m, alpm_sid_type_t sid_type, int index, uint8_t wide,
                         uint32_t *de, uint32_t dt_bitmap);
static int clear_l1_raw2(int u, int m, alpm_sid_type_t sid_type, int index, uint8_t wide);
static int read_l1_raw3(int u, int m, int index, uint32_t *de, uint8_t wide,
                        uint32_t *dt_bitmap);

/*
 * This is for local usage, i.e., what's allcoated will be freed immediately.
 * Thus not necessary to make it per unit.
 */
static shr_lmm_hdl_t l1_entries_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/* The ordering must not be changed! */
typedef struct prefix_layout_s {
    int num_pfx;
    uint8_t ipv;
    int vrf_type;
    uint8_t pfx_type;
    uint8_t app;
} prefix_layout_t;



#define PFX_MAX_3    3
#define PFX_MAX_2    2
#define PFX_MAX_1    1
#define PFX_NORMAL   0

/*
 * 100 bits for L3MC V4 : 16 (L3IF) + 16 (VRF) + 32 (IP) + 32 (IP) + 4 (Resv)
 * 300 bits for L3MC V6 : 16 (L3IF) + 16 (VRF) + 128 (IP) + 128 (IP) + 12 (Resv)
 * 200 bits for FP_COMP
 * 49 bits for LPM single 33 + 16
 * 145 bits for LPM Quad 129 + 16
 */

#define FPV4_BITS 200
#define FPV6_BITS 200
#define MCV4_BITS 100
#define MCV6_BITS 300
#define IPS_BITS 49
#define IPQ_BITS 145
#define MAX1_BIT 1

static prefix_layout_t prefix_layout[] = {
    {MAX1_BIT,  IP_VER_COUNT,  VRF_TYPES,   PFX_MAX_3,  APP_NUM},
    {FPV6_BITS, IP_VER_6, VRF_PRIVATE, PFX_NORMAL, APP_COMP1}, /* Double */
    {FPV6_BITS, IP_VER_6, VRF_GLOBAL,  PFX_NORMAL, APP_COMP1}, /* Double */
    {FPV4_BITS, IP_VER_4, VRF_PRIVATE, PFX_NORMAL, APP_COMP1}, /* Double */
    {FPV4_BITS, IP_VER_4, VRF_GLOBAL,  PFX_NORMAL, APP_COMP1}, /* Double */
    {MCV6_BITS, IP_VER_6, VRF_DEFAULT, PFX_NORMAL, APP_L3MC},  /* Double */
    {IPQ_BITS,  IP_VER_6, VRF_PRIVATE, PFX_NORMAL, APP_COMP0}, /* Double */
    {IPQ_BITS,  IP_VER_6, VRF_GLOBAL,  PFX_NORMAL, APP_COMP0}, /* Double */

    {MAX1_BIT, IP_VER_COUNT,  VRF_TYPES,    PFX_MAX_2,  APP_NUM},
    {IPQ_BITS, IP_VER_6, VRF_OVERRIDE, PFX_NORMAL, APP_LPM}, /* PKM_Q/PKM_WQ: Double. PKM_WD: Single. PKM_WS: Half */
    {IPQ_BITS, IP_VER_6, VRF_PRIVATE,  PFX_NORMAL, APP_LPM}, /* PKM_Q/PKM_WQ: Double. PKM_WD: Single. PKM_WS: Half */
    {IPQ_BITS, IP_VER_6, VRF_GLOBAL,   PFX_NORMAL, APP_LPM}, /* PKM_Q/PKM_WQ: Double. PKM_WD: Single. PKM_WS: Half */

    {MAX1_BIT,  IP_VER_COUNT,  VRF_TYPES,    PFX_MAX_1,  APP_NUM},
    {MCV4_BITS, IP_VER_4, VRF_DEFAULT,  PFX_NORMAL, APP_L3MC}, /* Single */
    {IPS_BITS,  IP_VER_4, VRF_PRIVATE,  PFX_NORMAL, APP_COMP0},/* Single */
    {IPS_BITS,  IP_VER_4, VRF_GLOBAL,   PFX_NORMAL, APP_COMP0},/* Single */
    {IPS_BITS,  IP_VER_4, VRF_OVERRIDE, PFX_NORMAL, APP_LPM},  /* Half */
    {IPS_BITS,  IP_VER_4, VRF_PRIVATE,  PFX_NORMAL, APP_LPM},  /* Half */
    {IPS_BITS,  IP_VER_4, VRF_GLOBAL,   PFX_NORMAL, APP_LPM}   /* Half */
};


/*******************************************************************************
 * Private Functions (definition)
 */

#ifdef ALPM_DEBUG
/*!
 * \brief Sanity check for pivot
 *
 * \param [in] u Device u.
 * \param [in] pivot_idx Pivot index
 * \param [in] pivot Pivot payload
 * \param [in] found Pivot exists
 *
 * \return nothing
 */
static void
sanity_check_pivot(int u, int m, int pivot_idx, alpm_pivot_t *pivot, int found)
{
    int arr_idx;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    arr_idx = arr_idx_from_pvt_idx(u, m, pivot_idx);

    if (found) {
        ALPM_ASSERT(L1_PVT_INFO(u, m, arr_idx) == pivot);
        ALPM_ASSERT(PIVOT_INDEX(u, m, arr_idx) == pivot->index);
    } else {
        ALPM_ASSERT(L1_PVT_INFO(u, m, arr_idx) == NULL);
    }
}

static void
sanity_check_unpair_pfx(int u, int m, uint8_t ldb, int pfx, int upkm)
{
    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    ALPM_ASSERT(pfx != INVALID_PFX);
    ALPM_ASSERT(pfx <= UNPAIR_MAX_PFX(u, m, ldb, upkm));
}

static void
sanity_check_pair_pfx(int u, int m, uint8_t ldb, int pfx, int pkm)
{
    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    ALPM_ASSERT(pfx != INVALID_PFX);
    ALPM_ASSERT(pfx <= MAX_PFX(u, m, ldb, LAYOUT_D, pkm));
}

/*!
 * \brief Sanity check for unpair state
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm Unpair packing mode
 *
 * \return nothing
 */
static void
sanity_check_unpair_state(int u, int m, uint8_t ldb, int pfx, int upkm)
{
    int vent;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    SANITY_CHECK_UNPAIR_PFX(u, m, ldb, pfx, upkm);
    vent = UNPAIR_STATE_END(u, m, ldb, pfx, upkm) - UNPAIR_STATE_START(u, m, ldb, pfx, upkm) + 1;
    ALPM_ASSERT(vent == UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm));
    ALPM_ASSERT(UNPAIR_STATE_START(u, m, ldb, pfx, upkm) >= KI_START(u, m, ldb, upkm));
    ALPM_ASSERT(UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) >= 0);
    ALPM_ASSERT(UNPAIR_STATE_FENT(u, m, ldb, pfx, upkm) >= 0);
    ALPM_ASSERT(UNPAIR_STATE_HENT(u, m, ldb, pfx, upkm) == 1 ||
                UNPAIR_STATE_HENT(u, m, ldb, pfx, upkm) == 0);

}

/*!
 * \brief Sanity check for pair state
 *
 * \param [in] u Device u.
 * \param [in] ldb Level1 DB
 * \param [in] pfx Translated prefix (Global prefix value).

 * \return nothing
 */
static void
sanity_check_pair_state(int u, int m, uint8_t ldb, int pfx, int pkm)
{
    int vent;
    int start = PAIR_STATE_START(u, m, ldb, pfx, pkm);
    int end = PAIR_STATE_END(u, m, ldb, pfx, pkm);

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    ALPM_ASSERT(pfx != INVALID_PFX);
    if (pfx == RANGE1_LAST_PFX(u, m, ldb, pkm) && LAST_PFX_SPAN(u, m, ldb, pkm)) {
        vent = RANGE1_LAST(u, m, ldb, pkm) - start + 1;
        ALPM_ASSERT(vent >= 0);
        vent += end - RANGE2_FIRST(u, m, ldb, pkm) + 1;
        ALPM_ASSERT(vent == PAIR_STATE_VENT(u, m, ldb, pfx, pkm));
    } else if (pfx_is_single_half(u, m, ldb, pfx, pkm)) {
        vent = end - start;
        vent = (vent < 0) ? 0 : (vent + 1);
        ALPM_ASSERT(vent == PAIR_STATE_VENT(u, m, ldb, pfx, pkm));
    } else {
        if ((end - start) < 0) {
            vent = 0;
        } else {
            start = pair_index_encode(u, m, start);
            end = pair_index_encode(u, m, end);
            vent = end - start + 1;
        }
        ALPM_ASSERT(vent == PAIR_STATE_VENT(u, m, ldb, pfx, pkm));
    }

    ALPM_ASSERT(PAIR_STATE_END(u, m, ldb, pfx, pkm) <= KI_END(u, m, ldb, pkm));
    ALPM_ASSERT(PAIR_STATE_VENT(u, m, ldb, pfx, pkm) >= 0);
    ALPM_ASSERT(PAIR_STATE_FENT(u, m, ldb, pfx, pkm) >= 0);
    ALPM_ASSERT(PAIR_STATE_HENT(u, m, ldb, pfx, pkm) == 1 ||
                PAIR_STATE_HENT(u, m, ldb, pfx, pkm) == 0);

}
/*!
 * \brief Sanity check for unpair entries
 *
 * \param [in] u Device u.
 * \param [in] de DIP unpair entry
 * \param [in] se SIP unpair entry
 * \param [in] pkm Packing mode

 * \return nothing
 */
static void
sanity_check_unpair_entry(int u, int m, uint32_t *de, uint32_t *se, int pkm)
{
    int rv;
    L1_ENTRY_t de2, se2;
    alpm_arg_t arg, arg2;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    sal_memcpy(de2, de, sizeof(de2));
    sal_memcpy(se2, se, sizeof(se2));

    bcmptm_rm_alpm_arg_init(u, m, &arg);
    arg.pkm = pkm;
    rv = bcmptm_rm_alpm_arg_fill(u, m, &arg, de2, se2, L1V_NARROW_0);
    ALPM_ASSERT(SHR_SUCCESS(rv));

    if (half_entry_key_type(u, m, arg.key.kt, pkm)) {
        bcmptm_rm_alpm_arg_init(u, m, &arg2);
        arg2.pkm = pkm;
        rv = bcmptm_rm_alpm_arg_fill(u, m, &arg2, de2, se2, L1V_NARROW_1);
        ALPM_ASSERT(SHR_SUCCESS(rv));
        if (arg.valid && arg2.valid) {
            ALPM_ASSERT(arg2.key.t.len == arg.key.t.len);
        }
    }
}


/*!
 * \brief Sanity check for pair entries
 *
 * \param [in] u Device u.
 * \param [in] de DIP unpair entry
 * \param [in] se SIP unpair entry

 * \return nothing
 */
static void
sanity_check_pair_entry(int u, int m, uint32_t *de, uint32_t *se, int pkm)
{
    int rv;
    L1_ENTRY_t de2, se2;
    alpm_arg_t arg;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    sal_memcpy(de2, de, sizeof(de2));
    sal_memcpy(se2, se, sizeof(se2));

    bcmptm_rm_alpm_arg_init(u, m, &arg);
    arg.pkm = pkm;
    rv = bcmptm_rm_alpm_arg_fill(u, m, &arg, de2, se2, L1V_WIDE);
    ALPM_ASSERT(SHR_SUCCESS(rv));
}

#endif

static void
l1_dt_bitmap_set(int u, int m, int ent_ofs, uint32_t *dt_bitmap, int data_type)
{
    if (bcmptm_rm_alpm_has_4_data_types(u, m)) {
        if (((ent_ofs << 1) + 1) < L1_DT_BITMAP_BITS) {
            if (data_type == DATA_FULL_3) { /* 0b11 */
                SHR_BITSET(dt_bitmap, (ent_ofs << 1) + 1);
                SHR_BITSET(dt_bitmap, (ent_ofs << 1));
            } else if (data_type == DATA_FULL_2) { /* 0b10 */
                SHR_BITSET(dt_bitmap, (ent_ofs << 1) + 1);
                SHR_BITCLR(dt_bitmap, (ent_ofs << 1));
            } else if (data_type == DATA_FULL) {  /* 0b01 */
                SHR_BITCLR(dt_bitmap, (ent_ofs << 1) + 1);
                SHR_BITSET(dt_bitmap, (ent_ofs << 1));
            } else {  /* 0b00 */
                SHR_BITCLR(dt_bitmap, (ent_ofs << 1) + 1);
                SHR_BITCLR(dt_bitmap, (ent_ofs << 1));
            }
        } else {
            ALPM_ASSERT(0);
        }
    } else {
        if (ent_ofs < L1_DT_BITMAP_BITS) {
            if (data_type == DATA_FULL) {
                SHR_BITSET(dt_bitmap, ent_ofs);
            } else {
                SHR_BITCLR(dt_bitmap, ent_ofs);
            }
        } else {
            ALPM_ASSERT(0);
        }
    }
}

static int
l1_dt_bitmap_get(int u, int m, int ent_ofs, uint32_t dt_bitmap)
{
    int bit0, bit1;
    if (bcmptm_rm_alpm_has_4_data_types(u, m)) {
        if (((ent_ofs << 1) + 1) < L1_DT_BITMAP_BITS) {
            bit1 = (dt_bitmap & (1 << ((ent_ofs << 1) + 1))) ? 1 : 0;
            bit0 = (dt_bitmap & (1 << (ent_ofs << 1))) ? 1 : 0;
            bit0 |= bit1 << 1;
            if (bit0 == 0x3) {
                return DATA_FULL_3;
            } else if (bit0 == 0x2) {
                return DATA_FULL_2;
            } else if (bit0 == 0x1) {
                return DATA_FULL;
            } else {
                return DATA_REDUCED;
            }
        } else {
            ALPM_ASSERT(0);
            return DATA_REDUCED;
        }
    } else {
        if (ent_ofs < L1_DT_BITMAP_BITS) {
            return (dt_bitmap & (1 << ent_ofs)) ? DATA_FULL : DATA_REDUCED;
        } else {
            ALPM_ASSERT(0);
            return DATA_REDUCED;
        }

    }
}



static int
ipv6_pkm_length(int u, int m, int pkm)
{
    alpm_dev_info_t *dev;
    int len;

    switch (pkm) {
    case PKM_WQ: len = 128; break;
    case PKM_WD: len = 128; break;
    case PKM_WS: len = 48; break;
    case PKM_Q: case PKM_SQ: len = 128; break;
    case PKM_D: case PKM_SD: len = 64; break;
    case PKM_S: case PKM_SS: len = 32; break;
    default: len = 0; break;
    }

    dev = bcmptm_rm_alpm_device_info_get(u, m);
    if (dev && dev->feature_l1_ipv6_len_nonstandard) {
        switch (pkm) {
        case PKM_Q: case PKM_SQ: len = dev->l1_ipv6_quad_len; break;
        case PKM_D: case PKM_SD: len = dev->l1_ipv6_double_len; break;
        case PKM_S: case PKM_SS: len = dev->l1_ipv6_single_len; break;
        default: break;
        }
    }

    return len;
}


static alpm_key_type_t
key_type_get(int u, int m, int ipv6, int len, uint8_t app, alpm_vrf_type_t vt)
{
    alpm_key_type_t kt;
    int ipv6_single_bits;
    int ipv6_double_bits;

    ipv6_single_bits = ipv6_pkm_length(u, m, PKM_S);
    ipv6_double_bits = ipv6_pkm_length(u, m, PKM_D);
    if (app == APP_LPM) {
        if (bcmptm_rm_alpm_is_large_vrf(u, m) && vt != VRF_OVERRIDE) {
            ipv6_single_bits += bcmptm_rm_alpm_vrf_len(u, m);
            ipv6_double_bits += bcmptm_rm_alpm_vrf_len(u, m);
        }
        if (!ipv6) {
            kt = KEY_IPV4;
        } else if (len <= ipv6_single_bits) {
            kt = NO_PKM_S(u, m) ? KEY_IPV6_DOUBLE : KEY_IPV6_SINGLE;
        } else if (len <= ipv6_double_bits) {
            kt = KEY_IPV6_DOUBLE;
        } else {
            kt = KEY_IPV6_QUAD;
        }
    } else if (app == APP_L3MC) {
        if (ipv6) {
            kt = KEY_L3MC_V6;
        } else {
            kt = KEY_L3MC_V4;
        }
    } else if (app == APP_COMP0) {
        if (ipv6) {
            kt = KEY_COMP0_V6;
        } else {
            kt = KEY_COMP0_V4;
        }
    } else {
        if (ipv6) {
            kt = KEY_COMP1_V6;
        } else {
            kt = KEY_COMP1_V4;
        }
    }
    return kt;
}

static bool
pkm_is_single(int u, int m, int pkm)
{
    /* PKM_WD is double, not single, as it can hold L3MC which is double */
    if (pkm == PKM_D ||
        pkm == PKM_SD) {
        return true;
    }
    return false;
}


/*!
 * \brief Convert index to packing mode (db optional)
 *
 * \param [in] u Device u.
 * \param [in] index Index
 * \param [out] db Corresponding L1 database
 *
 * \return Packing mode
 */
static int
index_to_pkm(int u, int m, int index, int *db)
{
    int pkm = 0;
    uint8_t ldb;
    for (ldb = 0; ldb < L1_DBS; ldb++) {
        if (index <= KI_END(u, m, ldb, PKM_WQ)) {
            pkm = PKM_WQ;
        } else if (index <= KI_END(u, m, ldb, PKM_WD)) {
            pkm = PKM_WD;
        } else if (index <= KI_END(u, m, ldb, PKM_WS)) {
            pkm = PKM_WS;
        } else if (index <= KI_END(u, m, ldb, PKM_Q)) {
            pkm = PKM_Q;
        } else if (index <= KI_END(u, m, ldb, PKM_D)) {
            pkm = PKM_D;
        } else if (index <= KI_END(u, m, ldb, PKM_S)) {
            pkm = PKM_S;
        } else if (index <= KI_END(u, m, ldb, PKM_SQ)) {
            pkm = PKM_SQ;
        } else if (index <= KI_END(u, m, ldb, PKM_SD)) {
            pkm = PKM_SD;
        } else if (index <= KI_END(u, m, ldb, PKM_SS)) {
            pkm = PKM_SS;
        } else if (index <= KI_END(u, m, ldb, PKM_FP)) {
            pkm = PKM_FP;
        } else if (index <= KI_END(u, m, ldb, PKM_SFP)) {
            pkm = PKM_SFP;
        } else {
            continue;
        }
        break;
    }
    ALPM_ASSERT(ldb < L1_DBS);
    ALPM_ASSERT(index <= KI_END(u, m, ldb, pkm) &&
                index >= KI_START(u, m, ldb, pkm));
    if (db) {
        *db = ldb;
    }
    return pkm;
}

static inline int
index_in_pair_tcam(int u, int m, int index)
{
    int pkm;
    pkm = index_to_pkm(u, m, index, NULL);
    return bcmptm_rm_alpm_pkm_is_pair(u, m, pkm);
}

static inline int
is_wide_sid(int u, int m, bcmdrd_sid_t sid)
{
    return (sid == WIDE_PAIR_SID(u, m) ||
            sid == WIDE_UNPAIR_SID(u, m) ||
            sid == WIDE_TCAM_ONLY_SID(u, m) ||
            sid == WIDE_DATA_ONLY_SID(u, m));
}

static inline int
is_pair_sid(int u, int m, bcmdrd_sid_t sid)
{
    return (sid == PAIR_SID(u, m) || sid == WIDE_PAIR_SID(u, m));
}

static inline int
is_unpair_sid(int u, int m, bcmdrd_sid_t sid)
{
    return (sid == UNPAIR_SID(u, m) || sid == WIDE_UNPAIR_SID(u, m));
}

static inline int
is_tcam_only_sid(int u, int m, bcmdrd_sid_t sid)
{
    return (sid == TCAM_ONLY_SID(u, m) ||
            sid == WIDE_TCAM_ONLY_SID(u, m));
}

static inline int
is_data_only_sid(int u, int m, bcmdrd_sid_t sid)
{
    return (sid == DATA_ONLY_SID(u, m) ||
            sid == WIDE_DATA_ONLY_SID(u, m));
}

static inline int
half_entry_key_type(int u, int m, alpm_key_type_t kt, int pkm)
{
    return (kt == KEY_IPV4 ||
            (kt == KEY_IPV6_SINGLE && (pkm == PKM_S || pkm == PKM_SS)));
}

static int
index_to_block(int u, int m, int index)
{
    return index / (TCAM_DEPTH(u, m) * (MAX_TCAMS(u, m) / L1_BLOCKS(u, m)));
}

static int
index_to_block_offset(int u, int m, int index)
{
    return index % (TCAM_DEPTH(u, m) * (MAX_TCAMS(u, m) / L1_BLOCKS(u, m)));
}


static int
block_make_index(int u, int m, int block, int block_offset)
{
    return (block * (TCAM_DEPTH(u, m) * (MAX_TCAMS(u, m) / L1_BLOCKS(u, m)))) + block_offset;
}

static int
phy_block_convert_index(int u, int m, int log_index, bool is_data_index)
{
    int log_block;
    int phy_block;
    int block_offset;
    int data_0;
    int phy_index;
    int log_idx = log_index;
    const alpm_config_t *config = bcmptm_rm_alpm_config(u, m);

    if (is_data_index) {
        data_0 = log_index & 1;
        log_idx = log_index >> 1;
    }

    log_block = index_to_block(u, m, log_idx);
    block_offset = index_to_block_offset(u, m, log_idx);
    phy_block = config->core.l1_blocks_l2p[log_block];

    /* return index based on physical blocks */
    phy_index = block_make_index(u, m, phy_block, block_offset);

    if (is_data_index) {
        phy_index = (phy_index << 1) + data_0;
    }

    if (!config->core.l1_blocks_map_en) {
        assert(phy_index == log_index);
    }
    return phy_index;

}


static int
log_block_convert_index(int u, int m, int phy_index)
{
    int log_block;
    int phy_block;
    int block_offset;
    int log_index;
    const alpm_config_t *config = bcmptm_rm_alpm_config(u, m);

    phy_block = index_to_block(u, m, phy_index);
    block_offset = index_to_block_offset(u, m, phy_index);
    log_block = config->core.l1_blocks_p2l[phy_block];

    log_index = block_make_index(u, m, log_block, block_offset);

    if (!config->core.l1_blocks_map_en) {
        assert(phy_index == log_index);
    }

    /* return index based on logical blocks */
    return log_index;

}

static int
l1_wide_index_size(int u, int m)
{
    return (KI_SIZE(u, m, L1_DB0, PKM_WQ) +
            KI_SIZE(u, m, L1_DB0, PKM_WD) +
            KI_SIZE(u, m, L1_DB0, PKM_WS));
}

static int
l1_hw_index_adjust_base(int u, int m, bcmdrd_sid_t sid, int index)
{
    int blk_conv_index; /* block converted index */

    blk_conv_index = phy_block_convert_index(u, m, index, false);

    if (!is_wide_sid(u, m, sid)) {
        blk_conv_index -= l1_wide_index_size(u, m);
    }

    if (is_pair_sid(u, m, sid)) {
        blk_conv_index = pair_index_encode(u, m, blk_conv_index);
    }
    return blk_conv_index;
}

static bcmdrd_sid_t
unpair_sid(int u, int m, alpm_key_type_t kt, int pkm)
{
    return WIDE_PKM(pkm) ? WIDE_UNPAIR_SID(u, m) : UNPAIR_SID(u, m);
}

static bcmdrd_sid_t
pair_sid(int u, int m, alpm_key_type_t key_type, int pkm)
{
    if (key_type == KEY_IPV4) {
        return WIDE_PKM(pkm) ? WIDE_UNPAIR_SID(u, m) : UNPAIR_SID(u, m);
    } else if (key_type == KEY_L3MC_V4) {
        return WIDE_UNPAIR_SID(u, m);
    } else if (key_type == KEY_L3MC_V6) {
        return WIDE_PAIR_SID(u, m);
    } else if (key_type == KEY_COMP0_V4) {
        return UNPAIR_SID(u, m);
    } else if (key_type == KEY_COMP0_V6) {
        return PAIR_SID(u, m);
    } else if (key_type == KEY_COMP1_V4 ||
               key_type == KEY_COMP1_V6) {
        return PAIR_SID(u, m);
    } else if (key_type == KEY_IPV6_SINGLE) {
        if (pkm == PKM_WD || pkm == PKM_WS) {
            return WIDE_UNPAIR_SID(u, m);
        } else if (pkm == PKM_WQ) {
            return WIDE_PAIR_SID(u, m);
        } else {
            return PAIR_SID(u, m);
        }
    } else if (key_type == KEY_IPV6_DOUBLE) {
        if (pkm == PKM_WD || pkm == PKM_WS) {
            return WIDE_UNPAIR_SID(u, m);
        } else if (pkm == PKM_WQ) {
            return WIDE_PAIR_SID(u, m);
        } else {
            return PAIR_SID(u, m);
        }
    } else {
        ALPM_ASSERT(pkm != PKM_WS);
        if (pkm == PKM_WD) {
            return WIDE_UNPAIR_SID(u, m);
        } else if (pkm == PKM_WQ) {
            return WIDE_PAIR_SID(u, m);
        } else {
            return PAIR_SID(u, m);
        }
    }
}

static bcmdrd_sid_t
unpair_sid2(int u, int m, int pkm)
{
    return WIDE_PKM(pkm) ? WIDE_UNPAIR_SID(u, m) : UNPAIR_SID(u, m);
}


static bcmdrd_sid_t
pair_sid2(int u, int m, int pkm)
{
    return WIDE_PKM(pkm) ? WIDE_PAIR_SID(u, m) : PAIR_SID(u, m);
}

/*!
 * \brief Encode and write dip & sip entry from common arg.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \param [in] new_add Entry not exists
 *
 * \return SHR_E_XXX.
 */
static int
encode_write_l1_entry(int u, int m, alpm_arg_t *arg, bool new_add, uint8_t unpair)
{
    l1_entry_t *le = NULL;
    int index;
    uint8_t l1v, hit_support, hit_version;
    uint32_t hit_entry[1];
    bcmdrd_sid_t sid;
    uint32_t dt_bitmap = 0;

    SHR_FUNC_ENTER(u);

    if (unpair) {
        sid = unpair_sid(u, m, arg->key.kt, arg->pkm);
    } else {
        sid = pair_sid(u, m, arg->key.kt, arg->pkm);
    }

    index = arg->index[LEVEL1];
    le = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    arg->a1.is_route = bcmptm_rm_alpm_max_levels(u, m, arg->db) == TWO_LEVELS;
    if (IS_HALF_INDEX(index)) {
        if (IS_HALF_L_INDEX(index) && new_add) {
            if (L1_INFO_FDM(u, m, n_template, arg->key.vt)) {
                sal_memcpy(le->de, L1_INFO_FDM(u, m, n_template, arg->key.vt),
                           L1_INFO_FD(u, m, n_size));
            } else {
                sal_memset(le->de, 0, sizeof(le->de));
            }
        } else {
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, m, sid, HW_INDEX(index), le->de, le->se, &dt_bitmap));
        }
        l1v = IS_HALF_R_INDEX(index) ? L1V_NARROW_1 : L1V_NARROW_0;

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_encode(u, m, arg, le->de, le->se, l1v));
    } else {
        if (is_pair_sid(u, m, sid)) {
            l1v = L1V_WIDE;
            if (L1_INFO_FDM(u, m, w_template, arg->key.vt)) {
                sal_memcpy(le->de, L1_INFO_FDM(u, m, w_template, arg->key.vt),
                           L1_INFO_FD(u, m, w_size));
            }
        } else {
            l1v = L1V_NARROW_FULL;
            if (L1_INFO_FDM(u, m, n_template, arg->key.vt)) {
                sal_memcpy(le->de, L1_INFO_FDM(u, m, n_template, arg->key.vt),
                           L1_INFO_FD(u, m, n_size));
            }
        }
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_encode(u, m, arg, le->de, le->se, l1v));
    }

    if (arg->a1.direct_route) {
        l1_dt_bitmap_set(u, m, (l1v == L1V_NARROW_1), &dt_bitmap,
                         arg->ad.user_data_type);
    }
    SHR_IF_ERR_EXIT(
        write_l1_raw(u, m, sid, HW_INDEX(index), le->de, le->se,
                     HIT_OP_NONE, 0, dt_bitmap));

    hit_support = bcmptm_rm_alpm_hit_support(u, m);
    if (hit_support != HIT_MODE_DISABLE) { /* Write hit */
        hit_version = bcmptm_rm_alpm_hit_version(u, m);
        if (hit_version == ALPM_HIT_VERSION_0) {
        } else if (hit_version == ALPM_HIT_VERSION_1) {
            hit_entry[0] = arg->hit;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_hit(u, m, LEVEL1, L1_HIT_INDEX(u, m, index),
                                            hit_entry));
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    if (le) {
        shr_lmm_free(l1_entries_lmem[u][m], le);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Encode and write dip & sip entry from common arg.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \param [in] new_add Entry not exists
 *
 * \return SHR_E_XXX.
 */
static int
encode_write_l1_entry2(int u, int m, alpm_arg_t *arg, bool new_add, uint8_t sid_type,
                       bool unpair)
{
    l1_entry_t *le = NULL;
    int index, hw_idx;
    uint8_t l1v;
    bcmdrd_sid_t sid;
    uint8_t wide;
    uint32_t dt_bitmap = 0;

    SHR_FUNC_ENTER(u);

    index = arg->index[LEVEL1];
    hw_idx = HW_INDEX(index);
    if (unpair) {
        sid = unpair_sid(u, m, arg->key.kt, arg->pkm);
    } else {
        sid = pair_sid(u, m, arg->key.kt, arg->pkm);
    }
    le = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (IS_HALF_INDEX(index)) {
        l1v = IS_HALF_R_INDEX(index) ? L1V_NARROW_1 : L1V_NARROW_0;
        wide = WIDE_HALF;
    } else {
        if (is_pair_sid(u, m, sid)) {
            l1v = L1V_WIDE;
            wide = WIDE_DOUBLE;
        } else {
            l1v = L1V_NARROW_FULL;
            wide = WIDE_SINGLE;
        }
    }

    arg->a1.is_route = bcmptm_rm_alpm_max_levels(u, m, arg->db) == TWO_LEVELS;
    if (sid_type == SID_TYPE_TCAM) {
        arg->encode_key_view = 1;
        if (IS_HALF_INDEX(index)) {
            if (IS_HALF_L_INDEX(index) && new_add) {
                sal_memset(le->de, 0, sizeof(le->de));
            } else {
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, hw_idx, WIDE_HALF, le->de,
                                 &dt_bitmap));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_encode(u, m, arg, le->de, NULL, l1v));
        } else {
            if (wide == WIDE_DOUBLE) {
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_encode(u, m, arg, le->de, le->se, l1v));
            } else {
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_encode(u, m, arg, le->de, NULL, l1v));
            }
        }
    } else {
        arg->encode_data_view = 1;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_encode(u, m, arg, le->de, NULL, l1v));
    }

    if (sid_type == SID_TYPE_DATA) {
        if (l1v == L1V_NARROW_1) {
            hw_idx = (hw_idx << 1) + 1;
        } else if (l1v == L1V_NARROW_0 || alpm_l1_info[u][m]->data_only_x2) {
            hw_idx = (hw_idx << 1);
        }
        if (arg->a1.direct_route) {
            /* One entry one data. */
            l1_dt_bitmap_set(u, m, 0, &dt_bitmap, arg->ad.user_data_type);
        }
    }
    SHR_IF_ERR_EXIT(
        write_l1_raw2(u, m, sid_type, hw_idx, wide, le->de, dt_bitmap));
    if (wide == WIDE_DOUBLE && sid_type == SID_TYPE_TCAM) {
        SHR_IF_ERR_EXIT(
            write_l1_raw2(u, m, SID_TYPE_TCAM2, hw_idx, wide, le->se, 0));
    }


exit:
    if (le) {
        shr_lmm_free(l1_entries_lmem[u][m], le);
    }
    SHR_FUNC_EXIT();
}
/*!
 * \brief Read and decode dip & sip entry to common arg.
 *
 * For unpair, normaly read & decode the first half, unless requesting arg2,
 * in which case, both halves are decoded.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [in] arg2 If set request for both halves, valid for half key type only
 *
 * \return SHR_E_XXX.
 */
static int
decode_read_l1_entry(int u, int m, bcmdrd_sid_t sid, alpm_arg_t *arg, alpm_arg_t *arg2)
{
    l1_entry_t *le = NULL;
    int index;
    int half_kt;
    uint8_t l1v, hit_version, wide;
    uint32_t hit_entry;
    uint32_t dt_bitmap;

    SHR_FUNC_ENTER(u);

    index = arg->index[LEVEL1];

    le = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    /* If request arg2, fill both halves */
    half_kt = half_entry_key_type(u, m, arg->key.kt, arg->pkm);
    if (half_kt && arg2) {
        arg2->pkm = arg->pkm;

        if (KEY_DATA_SEPARATE(u, m, FALSE)) {
            SHR_IF_ERR_EXIT(
                read_l1_raw3(u, m, index, le->de, WIDE_HALF, &dt_bitmap));
        } else {
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, m, sid, HW_INDEX(index), le->de, le->se, &dt_bitmap));
        }
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_arg_fill(u, m, arg, le->de, le->se, L1V_NARROW_0));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_arg_fill(u, m, arg2, le->de, le->se, L1V_NARROW_1));

        if (arg->a1.direct_route) {
            if (bcmptm_rm_alpm_cache_data_type(u, m)) {
                arg->ad.user_data_type = l1_dt_bitmap_get(u, m, 0, dt_bitmap);
            }
            arg->data_type = arg->ad.user_data_type;
        } else {
            arg->data_type = bcmptm_rm_alpm_mid_dt_get(u, m, arg->db, arg->key.t.app);
        }
        if (arg2->a1.direct_route) {
            if (bcmptm_rm_alpm_cache_data_type(u, m)) {
                arg2->ad.user_data_type = l1_dt_bitmap_get(u, m, 0, dt_bitmap);
            }
            arg2->data_type = arg2->ad.user_data_type;
        } else {
            arg2->data_type = bcmptm_rm_alpm_mid_dt_get(u, m, arg2->db, arg2->key.t.app);
        }

        SHR_EXIT();
    }

    if (half_kt) {
        l1v = IS_HALF_R_INDEX(index) ? L1V_NARROW_1 : L1V_NARROW_0;
        wide = WIDE_HALF;
    } else {
        l1v = is_pair_sid(u, m, sid) ? L1V_WIDE : L1V_NARROW_FULL;
        wide = (l1v == L1V_WIDE) ? WIDE_DOUBLE : WIDE_SINGLE;
    }
    if (KEY_DATA_SEPARATE(u, m, wide == WIDE_DOUBLE)) {
        SHR_IF_ERR_EXIT(
            read_l1_raw3(u, m, HW_INDEX(index), le->de, wide, &dt_bitmap));
    } else {
        SHR_IF_ERR_EXIT(
            read_l1_raw(u, m, sid, HW_INDEX(index), le->de, le->se, &dt_bitmap));
    }

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_arg_fill(u, m, arg, le->de, le->se, l1v));
    if (arg->a1.direct_route) {
        if (bcmptm_rm_alpm_cache_data_type(u, m)) {
            arg->ad.user_data_type =
                l1_dt_bitmap_get(u, m, (l1v == L1V_NARROW_1), dt_bitmap);
        }
        arg->data_type = arg->ad.user_data_type;
    } else {
        arg->data_type = bcmptm_rm_alpm_mid_dt_get(u, m, arg->db, arg->key.t.app);
    }

    if (bcmptm_rm_alpm_hit_support(u, m) != HIT_MODE_DISABLE) { /* Read hit */
        hit_version = bcmptm_rm_alpm_hit_version(u, m);
        if (hit_version == ALPM_HIT_VERSION_0) {
        } else if (hit_version == ALPM_HIT_VERSION_1) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_read_ln_hit(u, m, LEVEL1, L1_HIT_INDEX(u, m, index),
                                           &hit_entry));
            arg->hit = hit_entry;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    if (le) {
        shr_lmm_free(l1_entries_lmem[u][m], le);
    }
    SHR_FUNC_EXIT();
}


static inline int
tcam_num(int u, int m, int index)
{
    return ((index) / TCAM_DEPTH(u, m));
}

static inline int
index_in_right_tcam(int u, int m, int index)
{
    return (tcam_num(u, m, index) & 1);
}


/*!
 * \brief Convert pivot index to array index (subscript) of pivot info
 *
 * \param [in] u Device u.
 * \param [in] pivot_index Pivot index
 *
 * \return Array index (subscript) of pivot info
 */
static int
arr_idx_from_pvt_idx(int u, int m, int pivot_index)
{
    int arr_idx;

    if (pivot_index == INVALID_INDEX) {
        arr_idx = INVALID_INDEX;
    } else {
        arr_idx = HW_INDEX(pivot_index) << 1;
        if (IS_HALF_R_INDEX(pivot_index)) {
            arr_idx++;
        }
    }
    return arr_idx;
}


/*!
 * \brief Convert to array index (subscript) of pivot info to pivot index
 *
 * \param [in] u Device u.
 * \param [in] arr_idx Array index (subscript) of pivot info
 * \param [in] full Full key type
 *
 * \return Pivot index
 */
static inline int
pvt_idx_from_arr_idx(int u, int m, int arr_idx, int full)
{
    int pivot_idx = INVALID_INDEX;

    if (arr_idx != INVALID_INDEX) {
        int hw_index = arr_idx >> 1;

        if (full) {
            pivot_idx = FULL_INDEX(hw_index);
        } else if (arr_idx & 1) {
            pivot_idx = HALF_INDEX_R(hw_index);
        } else {
            pivot_idx = HALF_INDEX_L(hw_index);
        }
    }

    return pivot_idx;
}


/*!
 * \brief Allocate and initialize pivot payload
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot_pyld pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_alloc(int u, int m, alpm_arg_t *arg, alpm_pivot_t **pivot_pyld)
{
    int rv;
    rv = bcmptm_rm_alpm_elem_alloc(u, m, arg->db, LEVEL1, arg->a1.direct_route,
                                   &arg->key, pivot_pyld);
    if (SHR_SUCCESS(rv)) {
        (*pivot_pyld)->key.vt = arg->key.vt;
        (*pivot_pyld)->index = arg->index[LEVEL1];
        if (arg->a1.direct_route) {
            sal_memcpy((*pivot_pyld)->ad, &arg->ad, sizeof(arg->ad));
        }
    }
    return rv;
}

/*!
 * \brief Free pivot payload
 *
 * \param [in] u Device u.
 * \param [in] pivot_pyld pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_free(int u, int m, int db, bool direct_route, alpm_pivot_t *pivot)
{
    return bcmptm_rm_alpm_elem_free(u, m, db, LEVEL1, direct_route, pivot);
}


/*!
 * \brief Move pivot from one position to another.
 *
 * Move pivot (sw pointer) from one position to another, and update
 * pivot index accordingly.
 *
 * \param [in] u Device u.
 * \param [in] from Half entry based array index
 * \param [in] to Half entry based array index
 * \param [in] full Is full entry
 * \param [in] clear A clear opertion if set, else a shift operation
 *
 * \return SHR_E_XXX.
 */
static int
pivot_move(int u, int m, int from, int to, int full, int clear)
{
    alpm_pivot_t *from_pivot;
    alpm_pivot_t *to_pivot;
    SHR_FUNC_ENTER(u);

    from_pivot = L1_PVT_INFO(u, m, from);
    to_pivot = L1_PVT_INFO(u, m, to);


    if (clear) { /* from pivot must exist for clear case */
        ALPM_ASSERT(from_pivot != NULL);
    }

    if (from_pivot) {
        if (from == to) {
            ALPM_ASSERT(from_pivot->index == pvt_idx_from_arr_idx(u, m, to, full));
        }
        from_pivot->index = pvt_idx_from_arr_idx(u, m, to, full);

    }
    if (clear) {
        /* Target is supposed to be valid during clear */
        ALPM_ASSERT(to_pivot != NULL);
        /* Just for sanity check */
        to_pivot->index = pvt_idx_from_arr_idx(u, m, from, full);
    } else {
        /* Target is supposed to be empty during shift */
        ALPM_ASSERT(to_pivot == NULL);
    }
    L1_PVT_INFO(u, m, to) = from_pivot;
    L1_PVT_INFO(u, m, from) = NULL;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert pivot payload into Level-1 pivot trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [in] pivot Pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_insert(int u, int m, alpm_arg_t *arg, alpm_pivot_t *pivot)
{
    int rv;
    rv = bcmptm_rm_alpm_pvt_insert(u, m, arg->db, LEVEL1, arg->a1.direct_route,
                                   &arg->key, pivot);
    SANITY_CHECK_PIVOT(u, m, pivot->index, NULL, 0, SHR_SUCCESS(rv));
    return rv;
}

/*!
 * \brief Delete pivot payload from Level-1 pivot trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot Deleted pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_delete(int u, int m, alpm_arg_t *arg, alpm_pivot_t **pivot)
{
    int rv;
    rv = bcmptm_rm_alpm_pvt_delete(u, m, arg->db, LEVEL1, arg->a1.direct_route,
                                   &arg->key.t, pivot);
    if (*pivot && (*pivot)->bkt) {
        /* Save the group so we can free later */
        arg->log_bkt[LEVEL1] = (*pivot)->bkt->log_bkt;
    }
    SANITY_CHECK_PIVOT(u, m, (*pivot)->index, NULL, 0, pivot);
    return rv;
}

/*!
 * \brief Lookup (exact match) pivot payload in Level-1 pivot trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot Pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_lookup(int u, int m, alpm_arg_t *arg, alpm_pivot_t **pivot)
{
    int rv;
    rv = bcmptm_rm_alpm_pvt_lookup(u, m, arg->db, LEVEL1, &arg->key.t, pivot);
    SANITY_CHECK_PIVOT(u, m, (*pivot)->index, *pivot, 1, SHR_SUCCESS(rv));
    return rv;
}

/*!
 * \brief Lookup (LPM match) pivot payload in Level-1 pivot trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot Pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_find(int u, int m, alpm_arg_t *arg, alpm_pivot_t **pivot)
{
    int rv;
    rv = bcmptm_rm_alpm_pvt_find(u, m, arg->db, LEVEL1, &arg->key.t, pivot);
    SANITY_CHECK_PIVOT(u, m, (*pivot)->index, *pivot, 1, SHR_SUCCESS(rv));
    return rv;
}


static inline void
pfx_half_entry_update(int u, int m, uint8_t ldb, int pfx, int upkm)
{
    UNPAIR_STATE_HENT(u, m, ldb, pfx, upkm) = !UNPAIR_STATE_HENT(u, m, ldb, pfx, upkm);
}

static inline int
pfx_half_entry(int u, int m, uint8_t ldb, int pfx, int upkm)
{
    return UNPAIR_STATE_HENT(u, m, ldb, pfx, upkm);
}

static void
prefix_layout_resolve(int u, int m, prefix_layout_t **layout, int *cnt)
{
    *layout = prefix_layout;
    *cnt = COUNTOF(prefix_layout);
}


/*!
 * \brief Decode prefix in unpair TCAM to KEY type & VRF type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [out] kt KEY type
 * \param [out] vt VRF type
 * \param [out] len Prefix length
 * \param [in] upkm unpair packing mode
 *
 * \return nothing
 */
static void
pfx_decode(int u, int m, int pfx, int *ipv,
           alpm_vrf_type_t *vt,
           int *len,
           uint8_t *app)
{
    bcmptm_rm_alpm_l1_pfx_decode(u, m, pfx, ipv, vt, len, NULL, app);
}


/*!
 * \brief Decode prefix in unpair TCAM to KEY type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return KEY type
 */
static alpm_key_type_t
pfx_to_key_type(int u, int m, int pfx)
{
    alpm_key_type_t kt;
    int len;
    uint8_t app;
    alpm_vrf_type_t vt;
    pfx_decode(u, m, pfx, &kt, &vt, &len, &app);
    kt = key_type_get(u, m, kt, len, app, vt);
    return kt;
}


/*!
 * \brief Decode prefix in unpair TCAM to VRF type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return VRF type
 */
static alpm_vrf_type_t
pfx_to_vrf_type(int u, int m, int pfx)
{
    alpm_vrf_type_t vt;
    pfx_decode(u, m, pfx, NULL, &vt, NULL, NULL);
    return vt;

}

/*!
 * \brief Determine if prefix in unpair TCAM is IPv4
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return true/false
 */
static inline bool
pfx_is_single_half(int u, int m, uint8_t ldb, int pfx, int pkm)
{

    return (pfx <= MAX_PFX(u, m, ldb, LAYOUT_SH, pkm));
}


/*!
 * \brief Determine if prefix is LPM IPv6
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return true/false
 */
static inline bool
pfx_is_lpm_ipv6(int u, int m, int pfx)
{
    int ipv;
    uint8_t app;
    pfx_decode(u, m, pfx, &ipv, NULL, NULL, &app);
    return (ipv == IP_VER_6) && (app == APP_LPM);
}


/*!
 * \brief Determine if prefix in unpair TCAM is strict IPv6 type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return true/false
 */
static inline bool
pfx_is_kt(int u, int m, int pfx, alpm_key_type_t kt)
{
    return (pfx_to_key_type(u, m, pfx) == kt);
}


/*!
 * \brief Encode prefix in unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] ipv6 IPv6
 * \param [in] vt VRF type
 * \param [in] len IP MASK length
 * \param [in] upkm unpair packing mode
 *
 * \return Translated prefix (Global prefix value).
 */
static inline int
pfx_encode(int u, int m, int ipv, alpm_vrf_type_t vt, int len, uint8_t app)
{
    return bcmptm_rm_alpm_l1_pfx_encode(u, m, app, ipv, vt, len);
}

/*!
 * \brief Get max prefix in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 *
 * \return MAX prefix in pair TCAM
 */
static inline int
pfx_max(int u, int m, int la, int pkm)
{
    int i, num_pfx = 0;
    int cnt, max_pfx;
    prefix_layout_t *layout;

    prefix_layout_resolve(u, m, &layout, &cnt);

    if (la == LAYOUT_D) {
        max_pfx = PFX_MAX_3;
    } else {
        if (pkm == PKM_WD ||
            pkm == PKM_WS) {
            max_pfx = PFX_MAX_2;
        } else {
            max_pfx = PFX_MAX_1;
        }
    }
    for (i = cnt - 1; i >= 0; i--) {
        num_pfx += layout[i].num_pfx;
        if (layout[i].pfx_type == max_pfx) {
            break;
        }
    }

    return num_pfx - 1;
}

static int
pfx_base(int u, int m, int la, int pkm)
{
    if (la == LAYOUT_D) {
        if (pkm == PKM_WD || pkm == PKM_WS) {
            return bcmptm_rm_alpm_l1_pfx_encode(u, m, APP_L3MC, IP_VER_6, VRF_DEFAULT, 0);
        } else {
            return bcmptm_rm_alpm_l1_pfx_encode(u, m, APP_LPM, IP_VER_6, VRF_GLOBAL, 0);
        }
    } else {
        return bcmptm_rm_alpm_l1_pfx_encode(u, m, APP_LPM, IP_VER_4, VRF_GLOBAL, 0);
    }
}

/*!
 * \brief Get next prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return INVALID_PFX or free pfx
 */
static int
unpair_free_slot_search_next(int u, int m, uint8_t ldb, int pfx, int upkm)
{
    int free_pfx = pfx;

    while (UNPAIR_STATE_FENT(u, m, ldb, free_pfx, upkm) == 0) {
        free_pfx = UNPAIR_STATE_NEXT(u, m, ldb, free_pfx, upkm);
        if (free_pfx == -1) {
            free_pfx = INVALID_PFX;
            break;
        }
    }
    return free_pfx;
}

/*!
 * \brief Get previous prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return INVALID_PFX or free pfx
 */
static int
unpair_free_slot_search_prev(int u, int m, uint8_t ldb, int pfx, int upkm)
{
    int free_pfx = pfx;

    while (UNPAIR_STATE_FENT(u, m, ldb, free_pfx, upkm) == 0) {
        free_pfx = UNPAIR_STATE_PREV(u, m, ldb, free_pfx, upkm);
        if (free_pfx == -1) {
            free_pfx = INVALID_PFX;
            break;
        }
    }
    return free_pfx;
}

/*!
 * \brief Get prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return INVALID_PFX or free pfx
 */
static int
unpair_free_slot_search(int u, int m, uint8_t ldb, int pfx, int upkm)
{
    int free_pfx = unpair_free_slot_search_next(u, m, ldb, pfx, upkm);

    if (free_pfx == INVALID_PFX) {
        free_pfx = unpair_free_slot_search_prev(u, m, ldb, pfx, upkm);
    }
    return free_pfx;
}

/*!
 * \brief Move entry from from_ent to to_ent.
 *
 * A movable prefix is the highest prefix good to move from pair to unpair.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] from_ent From pt index
 * \param [in] to_ent From pt index
 * \param [in] upkm unpair packing mode
 * \param [in] is_last_pfx Set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
unpair_shift(int u, int m, int pfx, int from_ent, int to_ent, int upkm, int is_last_pfx)
{
    l1_entry_t *le = NULL;
    bcmdrd_sid_t sid;
    int key_type;
    int full;
    uint32_t dt_bitmap;

    SHR_FUNC_ENTER(u);

    if (from_ent == to_ent) {
        SHR_EXIT();
    }

    le = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    key_type = pfx_to_key_type(u, m, pfx);

    if (!KEY_DATA_SEPARATE(u, m, FALSE)) {
        sid = unpair_sid(u, m, key_type, upkm);
        SHR_IF_ERR_EXIT(
            read_l1_raw(u, m, sid, from_ent, le->de, le->se, &dt_bitmap));
        SHR_IF_ERR_EXIT(
            write_l1_raw(u, m, sid, to_ent, le->de, le->se,
                         HIT_OP_AUTO, 0x3, dt_bitmap));
    } else {
        int from_data, to_data, wide;
        if (half_entry_key_type(u, m, key_type, upkm)) { /* data_index_x2 */
            from_data = (from_ent << 1);
            to_data = (to_ent << 1);
            wide = WIDE_HALF;
        } else {
            if (alpm_l1_info[u][m]->data_only_x2) {
                from_data = from_ent << 1;
                to_data = to_ent << 1;
            } else {
                from_data = from_ent;
                to_data = to_ent;
            }
            wide = WIDE_SINGLE;
        }
        /* Clear old targeting key */
        SHR_IF_ERR_EXIT(
            clear_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, wide));

        /* Read data_0 */
        SHR_IF_ERR_EXIT(
           read_l1_raw2(u, m, SID_TYPE_DATA, from_data, wide, le->de, &dt_bitmap));
        /* Write data_0 */
        SHR_IF_ERR_EXIT(
           write_l1_raw2(u, m, SID_TYPE_DATA, to_data, wide, le->de, dt_bitmap));


        if (half_entry_key_type(u, m, key_type, upkm)) { /* data_index_x2 */
            from_data++;
            to_data++;
            /* Read data_1 */
            SHR_IF_ERR_EXIT(
                read_l1_raw2(u, m, SID_TYPE_DATA, from_data, wide, le->de, &dt_bitmap));
            /* Write data_1 */
            SHR_IF_ERR_EXIT(
               write_l1_raw2(u, m, SID_TYPE_DATA, to_data, wide, le->de, dt_bitmap));
        }

        /* Read key */
        SHR_IF_ERR_EXIT(
           read_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, wide, le->de, NULL));

        /* Write key */
        SHR_IF_ERR_EXIT(
           write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, wide, le->de, 0));

        if (is_last_pfx) {
            SHR_IF_ERR_EXIT(
                clear_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, wide));
        }
    }

    from_ent <<= 1;
    to_ent <<= 1;

    full = !half_entry_key_type(u, m, key_type, upkm);

    SHR_IF_ERR_EXIT(pivot_move(u, m, from_ent, to_ent, full, 0));
    SHR_IF_ERR_EXIT(pivot_move(u, m, ++from_ent, ++to_ent, full, 0));

exit:
    if (le) {
        shr_lmm_free(l1_entries_lmem[u][m], le);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift prefix entries 1 entry DOWN.
 *
 * Shift prefix entries 1 entry DOWN, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] ldb Database.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 * \param [in] is_last_pfx Set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
unpair_shift_down(int u, int m, uint8_t ldb, int pfx, int upkm, int is_last_pfx)
{
    int from_ent;
    int to_ent;
    alpm_key_type_t kt;
    SHR_FUNC_ENTER(u);

    SANITY_CHECK_UNPAIR_PFX(u, m, ldb, pfx, upkm);

    to_ent = UNPAIR_STATE_END(u, m, ldb, pfx, upkm) + 1;
    kt = pfx_to_key_type(u, m, pfx);
    if (half_entry_key_type(u, m, kt, upkm)) {
        if (pfx_half_entry(u, m, ldb, pfx, upkm)) {
            from_ent = UNPAIR_STATE_END(u, m, ldb, pfx, upkm);
            SHR_IF_ERR_EXIT(
                unpair_shift(u, m, pfx, from_ent, to_ent, upkm, is_last_pfx));
            to_ent--;
        }
    }

    from_ent = UNPAIR_STATE_START(u, m, ldb, pfx, upkm);
    if (from_ent != to_ent) {
        SHR_IF_ERR_EXIT(
            unpair_shift(u, m, pfx, from_ent, to_ent, upkm, is_last_pfx));
    }
    UNPAIR_STATE_START(u, m, ldb, pfx, upkm) += 1;
    UNPAIR_STATE_END(u, m, ldb, pfx, upkm) += 1;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift prefix entries 1 entry UP.
 *
 * Shift prefix entries 1 entry UP, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] ldb Database.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 * \param [in] is_last_pfx Set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
unpair_shift_up(int u, int m, uint8_t ldb, int pfx, int upkm, int is_last_pfx)
{
    int from_ent;
    int to_ent;
    alpm_key_type_t kt;
    SHR_FUNC_ENTER(u);

    SANITY_CHECK_UNPAIR_PFX(u, m, ldb, pfx, upkm);

    to_ent = UNPAIR_STATE_START(u, m, ldb, pfx, upkm) - 1;

    /* Don't move empty prefix . */
    if (UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) == 0) {
        ALPM_ASSERT(0);
        UNPAIR_STATE_START(u, m, ldb, pfx, upkm) = to_ent;
        UNPAIR_STATE_END(u, m, ldb, pfx, upkm) = to_ent - 1;
        SHR_EXIT();
    }

    from_ent = UNPAIR_STATE_END(u, m, ldb, pfx, upkm);

    kt = pfx_to_key_type(u, m, pfx);
    if (half_entry_key_type(u, m, kt, upkm)) {
        if (pfx_half_entry(u, m, ldb, pfx, upkm) && (from_ent - 1 != to_ent)) {
            SHR_IF_ERR_EXIT(
                unpair_shift(u, m, pfx, from_ent - 1, to_ent, upkm, is_last_pfx));
            to_ent = from_ent - 1;
        }
    }

    SHR_IF_ERR_EXIT(
        unpair_shift(u, m, pfx, from_ent, to_ent, upkm, is_last_pfx));

    UNPAIR_STATE_START(u, m, ldb, pfx, upkm) -= 1;
    UNPAIR_STATE_END(u, m, ldb, pfx, upkm) -= 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an empty prefix into the prefix state link.
 *
 * Create an empty prefix into the prefix state link, also take
 * half free entries from previous prefix.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] prev_pfx Prefix that is just larger than pfx
 * \param [in] upkm unpair packing mode
 *
 * \return Nothing.
 */
static void
unpair_group_create(int u, int m, uint8_t ldb, int pfx, int prev_pfx, int upkm)
{
    int next_pfx = INVALID_PFX;
    int start    = INVALID_INDEX;

    if (prev_pfx == INVALID_PFX) {
        ALPM_ASSERT_ERRMSG(0);
        return;
    }
    SANITY_CHECK_UNPAIR_PFX(u, m, ldb, pfx, upkm);
    SANITY_CHECK_UNPAIR_PFX(u, m, ldb, prev_pfx, upkm);

    ALPM_ASSERT(UNPAIR_STATE_FENT(u, m, ldb, prev_pfx, upkm));

    /* Insert new prefix into list */
    next_pfx = UNPAIR_STATE_NEXT(u, m, ldb, prev_pfx, upkm);
    if (next_pfx != -1) {
        UNPAIR_STATE_PREV(u, m, ldb, next_pfx, upkm) = pfx;
    }
    UNPAIR_STATE_NEXT(u, m, ldb, pfx, upkm) = next_pfx;
    UNPAIR_STATE_PREV(u, m, ldb, pfx, upkm) = prev_pfx;
    UNPAIR_STATE_NEXT(u, m, ldb, prev_pfx, upkm) = pfx;

    /* Assign half fent from prev_pfx */
    UNPAIR_STATE_FENT(u, m, ldb, pfx, upkm) =
        (UNPAIR_STATE_FENT(u, m, ldb, prev_pfx, upkm) + 1) >> 1;
    UNPAIR_STATE_FENT(u, m, ldb, prev_pfx, upkm) -= UNPAIR_STATE_FENT(u, m, ldb, pfx, upkm);

    /* Setup new prefix */
    start = UNPAIR_STATE_END(u, m, ldb, prev_pfx, upkm) +
            UNPAIR_STATE_FENT(u, m, ldb, prev_pfx, upkm) + 1;
    UNPAIR_STATE_START(u, m, ldb, pfx, upkm)  = start;
    UNPAIR_STATE_END(u, m, ldb, pfx, upkm)    =  start - 1;
    UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm)   = 0;

    SANITY_CHECK_UNPAIR_STATE(u, m, ldb, prev_pfx, upkm);
    SANITY_CHECK_UNPAIR_STATE(u, m, ldb, pfx, upkm);

}


/*!
 * \brief Remove freed prefix from the prefix state link.
 *
 * Remove empty prefix from the list and give back all free entries.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return Nothing.
 */
static void
unpair_group_destroy(int u, int m, uint8_t ldb, int pfx, int upkm)
{
    int prev_pfx;
    int next_pfx;

    SANITY_CHECK_UNPAIR_PFX(u, m, ldb, pfx, upkm);

    if (UNPAIR_STATE_PREV(u, m, ldb, pfx, upkm) == -1) {
        return;
    }

    if (UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) == 0) {
        prev_pfx = UNPAIR_STATE_PREV(u, m, ldb, pfx, upkm); /* Always present */
        SANITY_CHECK_UNPAIR_PFX(u, m, ldb, prev_pfx, upkm);
        next_pfx = UNPAIR_STATE_NEXT(u, m, ldb, pfx, upkm);

        /* Give away fent to prev_pfx */
        UNPAIR_STATE_FENT(u, m, ldb, prev_pfx, upkm) += UNPAIR_STATE_FENT(u, m, ldb, pfx, upkm);

        /* Remove from list */
        UNPAIR_STATE_NEXT(u, m, ldb, prev_pfx, upkm) = next_pfx;
        if (next_pfx != -1) {
            UNPAIR_STATE_PREV(u, m, ldb, next_pfx, upkm) = prev_pfx;
        }

        /* Reset the state */
        UNPAIR_STATE_FENT(u, m, ldb, pfx, upkm)   = 0;
        UNPAIR_STATE_NEXT(u, m, ldb, pfx, upkm)   = -1;
        UNPAIR_STATE_PREV(u, m, ldb, pfx, upkm)   = -1;
        UNPAIR_STATE_START(u, m, ldb, pfx, upkm)  = -1;
        UNPAIR_STATE_END(u, m, ldb, pfx, upkm)    = -1;
    }

    return;
}

/*!
 * \brief Move free slot from free_pfx to pfx (free_pfx > pfx).
 *
 * Move free slot from free_pfx to pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] free_pfx Translated prefix with free entries
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_free_slot_move_down(int u, int m, uint8_t ldb, int pfx, int free_pfx, int upkm)
{
    int next_pfx;
    int is_last_pfx;
    SHR_FUNC_ENTER(u);

    while (free_pfx > pfx) {
        next_pfx = UNPAIR_STATE_NEXT(u, m, ldb, free_pfx, upkm);
        is_last_pfx = (next_pfx == pfx);
        SHR_IF_ERR_EXIT(unpair_shift_up(u, m, ldb, next_pfx, upkm, is_last_pfx));
        UNPAIR_STATE_FENT(u, m, ldb, free_pfx, upkm) -= 1;
        UNPAIR_STATE_FENT(u, m, ldb, next_pfx, upkm) += 1;
        free_pfx = next_pfx;
        SANITY_CHECK_UNPAIR_STATE(u, m, ldb, next_pfx, upkm);
        SANITY_CHECK_UNPAIR_STATE(u, m, ldb, free_pfx, upkm);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Move free slot from free_pfx to pfx (free_pfx < pfx).
 *
 * Move free slot from free_pfx to pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] free_pfx Translated prefix with free entries
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_free_slot_move_up(int u, int m, uint8_t ldb, int pfx, int free_pfx, int upkm)
{
    int prev_pfx;
    int is_last_pfx;
    SHR_FUNC_ENTER(u);

    while (free_pfx < pfx) {
        prev_pfx = UNPAIR_STATE_PREV(u, m, ldb, free_pfx, upkm);
        is_last_pfx = (prev_pfx == pfx);
        SHR_IF_ERR_EXIT(unpair_shift_down(u, m, ldb, free_pfx, upkm, is_last_pfx));
        UNPAIR_STATE_FENT(u, m, ldb, free_pfx, upkm) -= 1;
        UNPAIR_STATE_FENT(u, m, ldb, prev_pfx, upkm) += 1;
        free_pfx = prev_pfx;
        SANITY_CHECK_UNPAIR_STATE(u, m, ldb, prev_pfx, upkm);
        SANITY_CHECK_UNPAIR_STATE(u, m, ldb, free_pfx, upkm);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a slot for the prefix.
 *
 * Free a slot for the prefix, rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] del_slot Translated index
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_free_slot_free(int u, int m, uint8_t ldb, int pfx, int del_slot, int upkm)
{
    int         from_ent;
    int         to_ent;
    l1_entry_t *le1 = NULL;
    l1_entry_t *le2 = NULL;
    uint32_t    *from_de, *from_se, *to_de, *to_se;
    int         src_slot;
    int         full;
    int         half_kt;
    bcmdrd_sid_t sid;
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    int len;
    int from_data;
    int to_data;
    uint8_t app;
    uint32_t dt_bitmap0, dt_bitmap1;

    SHR_FUNC_ENTER(u);

    SANITY_CHECK_UNPAIR_PFX(u, m, ldb, pfx, upkm);

    from_ent = UNPAIR_STATE_END(u, m, ldb, pfx, upkm);
    to_ent = HW_INDEX(del_slot);
    pfx_decode(u, m, pfx, &kt, &vt, &len, &app);
    kt = key_type_get(u, m, kt, len, app, vt);
    sid = unpair_sid(u, m, kt, upkm);

    le1 = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le1 == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    le2 = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le2 == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    to_de = le1->de;
    to_se = le1 ->se;
    if (from_ent == to_ent && !KEY_DATA_SEPARATE(u, m, FALSE)) {
        from_de = to_de;
        from_se = to_se;
    } else {
        from_de = le2->de;
        from_se = le2->se;
    }

    half_kt = half_entry_key_type(u, m, kt, upkm);
    if (half_kt) { /* KEY_IPV4 or KEY_IPV6_SINGLE on PKM_S */
        int from_v1, to_v1;

        from_v1 = !pfx_half_entry(u, m, ldb, pfx, upkm);
        to_v1 = IS_HALF_R_INDEX(del_slot);
        src_slot = (from_ent << 1) + from_v1;
        del_slot = (to_ent << 1) + to_v1;
        ALPM_ASSERT(src_slot >= del_slot);


        if (!KEY_DATA_SEPARATE(u, m, FALSE)) {
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, m, sid, to_ent, to_de, to_se, &dt_bitmap0));
            if (from_ent != to_ent) {
                SHR_IF_ERR_EXIT(
                    read_l1_raw(u, m, sid, from_ent, from_de, from_se, &dt_bitmap1));
            }

            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_half_entry_move(u, m, from_de, to_de, from_v1, to_v1, upkm));

            SHR_IF_ERR_EXIT(
                write_l1_raw(u, m, sid, to_ent, to_de, to_se,
                             HIT_OP_AUTO, 1 << to_v1, dt_bitmap0));

            if (from_ent != to_ent) {
                SHR_IF_ERR_EXIT(
                    write_l1_raw(u, m, sid, from_ent, from_de, from_se,
                                 HIT_OP_NONE, 0, dt_bitmap1));
            }
        } else {
            /* Done */
            if (from_ent != to_ent) {
                /* Read deleting entry key */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, NULL));
                /* Clear deleting entry key content */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_clear(u, m, to_de, to_v1, upkm));
                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));

                /* Read last entry data */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_DATA, src_slot, WIDE_HALF, from_de, &dt_bitmap0));
                /* Write last entry data to deleting posititon */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_DATA, del_slot, WIDE_HALF, from_de, dt_bitmap0));

                /* Read last entry key */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_HALF, from_de, NULL));

                /* Move last entry key to deleting entry key */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_move(u, m, from_de, to_de, from_v1, to_v1, upkm));

                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));

                /* Write last entry key*/
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_HALF, from_de, 0));
            } else if (from_v1 != to_v1) {
                /* Read deleting entry key */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, NULL));
                /* Clear deleting entry key content */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_clear(u, m, to_de, to_v1, upkm));
                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));

                /* Read last entry data */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_DATA, src_slot, WIDE_HALF, from_de, &dt_bitmap0));
                /* Write last entry data to deleting posititon */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_DATA, del_slot, WIDE_HALF, from_de, dt_bitmap0));

                /* Move last entry key to deleting entry key */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_move(u, m, to_de, to_de, from_v1, to_v1, upkm));

                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));
            } else {
                /* Read deleting entry key */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, NULL));
                /* Clear deleting entry key content */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_clear(u, m, to_de, to_v1, upkm));
                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));
            }
        }


        pfx_half_entry_update(u, m, ldb, pfx, upkm);

        if (from_v1) {
            /* First half of full entry is deleted, generating half entry.*/
            L1_HALF_COUNT(u, m, ldb, kt, vt, upkm)++;
        } else {

            /* Remaining half entry got deleted. Decrement half entry count.*/
            L1_HALF_COUNT(u, m, ldb, kt, vt, upkm)--;
            UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) -= 1;
            UNPAIR_STATE_FENT(u, m, ldb, pfx, upkm) += 1;
            UNPAIR_STATE_END(u, m, ldb, pfx, upkm)  -= 1;
        }
    } else { /* IPV6 */
        src_slot = (from_ent << 1);
        del_slot = (to_ent << 1);

        if (!KEY_DATA_SEPARATE(u, m, FALSE)) {
            if (to_ent != from_ent) {
                SHR_IF_ERR_EXIT(
                    read_l1_raw(u, m, sid, from_ent, from_de, from_se, &dt_bitmap0));
                SHR_IF_ERR_EXIT(
                    write_l1_raw(u, m, sid, to_ent, from_de, from_se,
                                 HIT_OP_AUTO, 0x1, dt_bitmap0));
            }
            SHR_IF_ERR_EXIT(clear_l1_raw(u, m, sid, from_ent));
        } else {
            /* Done 2 */
            if (to_ent != from_ent) {

                if (alpm_l1_info[u][m]->data_only_x2) {
                    from_data = src_slot;
                    to_data = del_slot;
                } else {
                    from_data = from_ent;
                    to_data = to_ent;
                }
                /* Clear key of deleted entry to prevent misforwarding */
                SHR_IF_ERR_EXIT(clear_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_SINGLE));
                /* Read data of last */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_DATA, from_data, WIDE_SINGLE,
                                 from_de, &dt_bitmap0));
                /* Write data to deleted position */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_DATA, to_data, WIDE_SINGLE,
                                  from_de, dt_bitmap0));

                /* Read key of last */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_SINGLE,
                                 from_de, NULL));
                /* Write key to deleted position */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_SINGLE,
                                  from_de, 0));
            }
            /* Delete key is sufficient. */
            SHR_IF_ERR_EXIT(clear_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_SINGLE));
        }

        UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) -= 1;
        UNPAIR_STATE_FENT(u, m, ldb, pfx, upkm) += 1;
        UNPAIR_STATE_END(u, m, ldb, pfx, upkm) -= 1;
    }

    full = pfx_to_key_type(u, m, pfx);
    full = !half_entry_key_type(u, m, full, upkm);

    SHR_IF_ERR_EXIT(pivot_move(u, m, src_slot, del_slot, full, 1));

    if (UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) == 0) {
        unpair_group_destroy(u, m, ldb, pfx, upkm);
    }

exit:
    if (le1) {
        shr_lmm_free(l1_entries_lmem[u][m], le1);
    }
    if (le2) {
        shr_lmm_free(l1_entries_lmem[u][m], le2);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Allocate a slot for the new prefix.
 *
 * Allocate a slot for the new prefix rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [out] free_slot Translated index
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_free_slot_alloc(int u, int m, uint8_t ldb, int pfx, int *free_slot, int upkm)
{

    int free_pfx, pfx2;
    int prev_pfx = INVALID_PFX;
    int end;
    int half_kt;
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    uint8_t app;
    int len;
    SHR_FUNC_ENTER(u);

    SANITY_CHECK_UNPAIR_PFX(u, m, ldb, pfx, upkm);

    pfx_decode(u, m, pfx, &kt, &vt, &len, &app);
    kt = key_type_get(u, m, kt, len, app, vt);
    half_kt = half_entry_key_type(u, m, kt, upkm);

    if (half_kt && pfx_half_entry(u, m, ldb, pfx, upkm)) {
        end = UNPAIR_STATE_END(u, m, ldb, pfx, upkm);
        *free_slot = HALF_INDEX_R(end);
        pfx_half_entry_update(u, m, ldb, pfx, upkm);
        SHR_EXIT();
    }

    if (UNPAIR_STATE_FENT(u, m, ldb, pfx, upkm) == 0) {
        pfx2 = pfx;

        if (UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) == 0) {
            /* Find prev pfx */
            prev_pfx = UNPAIR_MAX_PFX(u, m, ldb, upkm);

            while (UNPAIR_STATE_NEXT(u, m, ldb, prev_pfx, upkm) > pfx) {
                prev_pfx = UNPAIR_STATE_NEXT(u, m, ldb, prev_pfx, upkm);
            }
            pfx2 = prev_pfx;
        }

        free_pfx = unpair_free_slot_search(u, m, ldb, pfx2, upkm);

        if (free_pfx == INVALID_PFX) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        } else if (free_pfx > pfx2) {
            SHR_IF_ERR_EXIT(
                unpair_free_slot_move_down(u, m, ldb, pfx2, free_pfx, upkm));
        } else if (free_pfx < pfx2) {
            SHR_IF_ERR_EXIT(
                unpair_free_slot_move_up(u, m, ldb, pfx2, free_pfx, upkm));
        }
    }

    if (UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) == 0 && prev_pfx != INVALID_PFX) {
        unpair_group_create(u, m, ldb, pfx, prev_pfx, upkm);
    }

    UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) += 1;
    UNPAIR_STATE_FENT(u, m, ldb, pfx, upkm) -= 1;
    UNPAIR_STATE_END(u, m, ldb, pfx, upkm)  += 1;
    end = UNPAIR_STATE_END(u, m, ldb, pfx, upkm);
    if (half_kt) {
        pfx_half_entry_update(u, m, ldb, pfx, upkm);
    }

    SANITY_CHECK_UNPAIR_STATE(u, m, ldb, pfx, upkm);

    *free_slot = half_kt ? HALF_INDEX_L(end) : FULL_INDEX(end);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update an entry into unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be inserted
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX
 */
static int
unpair_update(int u, int m, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);

    if (!KEY_DATA_SEPARATE(u, m, FALSE)) {
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry(u, m, arg, FALSE, UNPAIR));
    } else {
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry2(u, m, arg, FALSE, SID_TYPE_DATA, UNPAIR));
    }

    if (arg->a1.direct_route) {
        ALPM_ASSERT(arg->pivot[LEVEL1]);
        *(arg->pivot[LEVEL1]->ad) = arg->ad;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an entry into unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be inserted
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX
 */
static int
unpair_insert(int u, int m, alpm_arg_t *arg, int upkm)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    int rv;
    uint8_t ldb = arg->db;
    SHR_FUNC_ENTER(u);

    if (bcmptm_rm_alpm_pkm_is_pair(u, m, upkm)) {
        SHR_ERR_EXIT(pair_insert(u, m, arg, upkm));
    }

    ALPM_ASSERT(arg->index[LEVEL1] == INVALID_INDEX);
    rv = unpair_free_slot_alloc(u, m, ldb,
                                arg->key.pfx, &arg->index[LEVEL1], upkm);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_RESOURCE) {
        /* Shut error messages prints, as E_RESOURCE is really not an error */
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    arg->pkm = upkm;
    if (!KEY_DATA_SEPARATE(u, m, FALSE)) {
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry(u, m, arg, TRUE, UNPAIR));
    } else {
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry2(u, m, arg, TRUE, SID_TYPE_DATA, UNPAIR));
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry2(u, m, arg, TRUE, SID_TYPE_TCAM, UNPAIR));
    }


    if (!arg->in_move) {
        alpm_pivot_t *pivot;
        int index2;
        SHR_IF_ERR_EXIT(l1_pvt_alloc(u, m, arg, &pivot));
        SHR_IF_ERR_EXIT(l1_pvt_insert(u, m, arg, pivot));
        arg->pivot[LEVEL1] = pivot;
        ALPM_ASSERT(pivot);
        index2 = arr_idx_from_pvt_idx(u, m, arg->index[LEVEL1]);
        L1_PVT_INFO(u, m, index2) = pivot;
    }

    kt = arg->key.kt;
    vt = arg->key.vt;
    ALPM_ASSERT_ERRMSG(kt < KEY_TYPES);
    ALPM_ASSERT_ERRMSG(vt < VRF_TYPES);
    L1_USED_COUNT(u, m, ldb, kt, vt, upkm)++;
    if (half_entry_key_type(u, m, kt, upkm)) {
        if (IS_HALF_R_INDEX(arg->index[LEVEL1])) {
            L1_HALF_COUNT(u, m, ldb, kt, vt, upkm)--;
        } else {
            L1_HALF_COUNT(u, m, ldb, kt, vt, upkm)++;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be deleted
 *
 * \return SHR_E_XXX
 */
static int
unpair_delete(int u, int m, alpm_arg_t *arg)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    alpm_pivot_t    *pivot = NULL;
    int upkm;

    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(arg->index[LEVEL1] != INVALID_INDEX);

    upkm = index_to_pkm(u, m, HW_INDEX(arg->index[LEVEL1]), NULL);

    SHR_IF_ERR_EXIT(
        unpair_free_slot_free(u, m, arg->db,
                              arg->key.pfx, arg->index[LEVEL1], upkm));

    if (!arg->in_move) {
        SHR_IF_ERR_EXIT(l1_pvt_delete(u, m, arg, &pivot));
        SHR_IF_ERR_EXIT(l1_pvt_free(u, m, arg->db, arg->a1.direct_route, pivot));
    }

    kt = arg->key.kt;
    vt = arg->key.vt;
    L1_USED_COUNT(u, m, arg->db, kt, vt, upkm)--;

    /* VRF_PIVOT_REF_DEC(u, m, w_vrf, kt); */
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get args from unpair TCAM for a given index
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM arg with index set
 *
 * \return SHR_E_XXX
 */
static inline int
unpair_get(int u, int m, alpm_arg_t *arg)
{
    bcmdrd_sid_t sid;
    sid = unpair_sid(u, m, arg->key.kt, arg->pkm);
    return decode_read_l1_entry(u, m, sid, arg, NULL);
}

/*!
 * \brief Get the larget prefix in unpair TCAM
 *
 * Get the largest IPv4 prefix, if not exist,
 * get the largest IPv6 prefix.
 *
 * \param [in] u Device u.
 * \param [in] vt VRF type
 * \param [in] upkm Unpair packing mode
 *
 * \return SHR_E_XXX
 */
static void
unpair_largest_lpm_prefix(int u, int m, uint8_t ldb, int pkm,
                          int *v4_pfx, int *v6_pfx)
{
    int pfx;
    *v4_pfx = *v6_pfx = INVALID_PFX;
    pfx = UNPAIR_MAX_PFX(u, m, ldb, pkm);
    pfx = UNPAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    while (pfx != -1) {
        if (pfx_is_lpm_ipv6(u, m, pfx) && *v6_pfx == INVALID_PFX) {
            *v6_pfx = pfx;
        }
        if (pfx_is_kt(u, m, pfx, KEY_IPV4) && *v4_pfx == INVALID_PFX) {
            *v4_pfx = pfx;
            break;
        }
        pfx = UNPAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    }


    return;
}
/*!
 * \brief Get the larget prefix in unpair TCAM
 *
 * Get the largest IPv4 prefix, if not exist,
 * get the largest IPv6 prefix.
 *
 * \param [in] u Device u.
 * \param [in] vt VRF type
 * \param [in] upkm Unpair packing mode
 *
 * \return SHR_E_XXX
 */
static void
unpair_smallest_lpm_prefix(int u, int m, uint8_t ldb, int pkm,
                           int *v4_pfx, int *v6_pfx)
{
    int curr_pfx;

    *v4_pfx = *v6_pfx = INVALID_PFX;
    curr_pfx = MAX_PFX(u, m, ldb, LAYOUT_D, pkm);
    curr_pfx = PAIR_STATE_NEXT(u, m, ldb, curr_pfx, pkm);
    while (curr_pfx != -1) {
        if (pfx_is_lpm_ipv6(u, m, curr_pfx)) {
            *v6_pfx = curr_pfx;
        }
        if (pfx_is_kt(u, m, curr_pfx, KEY_IPV4)) {
            *v4_pfx = curr_pfx;
        }
        curr_pfx = PAIR_STATE_NEXT(u, m, ldb, curr_pfx, pkm);
    }
}


bool
bcmptm_rm_alpm_pkm_is_pair(int u, int m, int pkm)
{
    if (pkm == PKM_Q ||
        pkm == PKM_WS || /* Can hold L3MC_V6 */
        pkm == PKM_WD || /* Can hold L3MC_V6 */
        pkm == PKM_WQ ||
        pkm == PKM_SQ ||
        pkm == PKM_FP ||
        pkm == PKM_SFP) {
        return true;
    }
    return false;
}


static int
unpair_state_init(int u, int m, bool recover)
{
    int i;
    int max_pfx_v6;
    int sz;
    int start = 0, fent;
    int upkm;
    uint8_t ldb;
    l1_db_info_t *l1_db;
    SHR_FUNC_ENTER(u);

    sz = 0;
    for (i = 0; i < COUNTOF(prefix_layout); i++) {
        sz += prefix_layout[i].num_pfx;
    }

    for (ldb = 0; ldb < L1_DBS; ldb++) {
        l1_db = &L1_DB(u, m, ldb);
        if (!l1_db->valid) {
            continue;
        }
        for (upkm = 0; upkm < PKM_NUM; upkm++) {
            if (bcmptm_rm_alpm_pkm_is_pair(u, m, upkm)) {
                continue;
            }
            ALPM_ALLOC(UNPAIR_STATE(u, m, ldb, upkm), sizeof(block_state_t) * sz,
                       "bcmptmRmalpmUnpairstate");
            sal_memset(UNPAIR_STATE(u, m, ldb, upkm), 0, sizeof(block_state_t) * sz);
            for (i = 0; i < sz; i++) {
                UNPAIR_STATE_START(u, m, ldb, i, upkm) = -1;
                UNPAIR_STATE_END(u, m, ldb, i, upkm)   = -1;
                UNPAIR_STATE_PREV(u, m, ldb, i, upkm)  = -1;
                UNPAIR_STATE_NEXT(u, m, ldb, i, upkm)  = -1;
                UNPAIR_STATE_VENT(u, m, ldb, i, upkm)  = 0;
                UNPAIR_STATE_FENT(u, m, ldb, i, upkm)  = 0;
            }
            UNPAIR_MAX_PFX(u, m, ldb, upkm) = pfx_max(u, m, LAYOUT_D, upkm);
            UNPAIR_BASE_PFX(u, m, ldb, upkm) = pfx_base(u, m, LAYOUT_SH, upkm);
            max_pfx_v6 = UNPAIR_MAX_PFX(u, m, ldb, upkm);
            fent = KI_SIZE(u, m, ldb, upkm);
            UNPAIR_STATE_FENT(u, m, ldb, max_pfx_v6, upkm) = fent;
            start = KI_START(u, m, ldb, upkm);
            UNPAIR_STATE_START(u, m, ldb, max_pfx_v6, upkm) = start;
            UNPAIR_STATE_END(u, m, ldb, max_pfx_v6, upkm) = start - 1;
            if (recover) {
                UNPAIR_STATE_FENT(u, m, ldb, max_pfx_v6, upkm) = 0;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static void
unpair_state_cleanup(int u, int m)
{
    int upkm;
    uint8_t ldb;

    if (L1_INFO(u, m)) {
        for (upkm = 0; upkm < PKM_NUM; upkm++) {
            if (bcmptm_rm_alpm_pkm_is_pair(u, m, upkm)) {
                continue;
            }
            for (ldb = 0; ldb < L1_DBS; ldb++) {
                SHR_FREE(UNPAIR_STATE(u, m, ldb, upkm));
            }
        }
    }
}

static void
unpair_stats_cleanup(int u, int m)
{
    return;
}

/*!
 * \brief Map used entries of one KEY type to another
 *
 * Multiple KEY types can coexist within a packing block. The used count
 * for any KEY type are the inserted entries of all KEY types.
 * To evaluate all used count for a given KEY type, we need to map
 * count of all KEY types.
 *
 * \param [in] u Device u.
 * \param [in] from_kt From KEY type
 * \param [in] to_kt To KEY type
 * \param [in] vt VRF type
 * \param [in] upkm Unpair packing mode
 *
 * \return Mapped used count
 */
static int
l1_used_count_map(int u, int m, int ldb, alpm_key_type_t from_kt, alpm_key_type_t to_kt,
                  alpm_vrf_type_t vt, int pkm)
{
    int count, v;
    int used_cnt = L1_USED_COUNT(u, m, ldb, from_kt, vt, pkm);
    int half_cnt = L1_HALF_COUNT(u, m, ldb, from_kt, vt, pkm);
    int from_r, to_r;

    if (from_kt == to_kt) {
        return used_cnt + half_cnt;
    }

    from_r = alpm_pkm_kt_weight[pkm][from_kt];
    to_r = alpm_pkm_kt_weight[pkm][to_kt];

    if (from_r == 0) {
        count = 0;
    } else if (to_r == 0) {
        count = 0;
    } else if (to_r >= from_r) {
        v = to_r / from_r;
        count = (used_cnt + half_cnt + v - 1) / v;
    } else {
        v = from_r / to_r;
        count = (used_cnt + half_cnt) * v;
    }
    return count;
}


/*!
 * \brief Get unpair TCAM block used count.
 *
 * \param [in] u Device u.
 * \param [in] kt Key type
 * \param [in] vt VRF type
 *
 * \return used count.
 */
static int
pkm_used_count(int u, int m, uint8_t ldb, alpm_key_type_t kt, alpm_vrf_type_t vt, int upkm)
{
    int i, used = 0;
    if (!DUAL_MODE(u, m)) {
        for (i = 0; i < KEY_TYPES; i++) {
            used += (l1_used_count_map(u, m, ldb, i, kt, VRF_PRIVATE, upkm));
            used += (l1_used_count_map(u, m, ldb, i, kt, VRF_GLOBAL, upkm));
            used += (l1_used_count_map(u, m, ldb, i, kt, VRF_OVERRIDE, upkm));
        }
    } else if (vt == VRF_PRIVATE) {
        for (i = 0; i < KEY_TYPES; i++) {
            used += (l1_used_count_map(u, m, ldb, i, kt, VRF_PRIVATE, upkm));
        }
    } else {
        for (i = 0; i < KEY_TYPES; i++) {
            used += (l1_used_count_map(u, m, ldb, i, kt, VRF_GLOBAL, upkm));
            used += (l1_used_count_map(u, m, ldb, i, kt, VRF_OVERRIDE, upkm));
        }
    }
    return used;
}

/*!
 * \brief Get unpair TCAM block free count.
 *
 * \param [in] u Device u.
 * \param [in] kt Key type
 * \param [in] vt VRF type
 * \param [in] upkm unpair packing mode
 * \param [in] upkm unpair packing mode
 *
 * \return free count.
 */
static int
pkm_free_count(int u, int m, uint8_t ldb, alpm_key_type_t kt, alpm_vrf_type_t vt, int upkm)
{
    return L1_MAX_COUNT(u, m, ldb, kt, upkm) - pkm_used_count(u, m, ldb, kt, vt, upkm);
}


/*!
 * \brief Determine whether a given index belongs to IPv6 range
 *
 * \param [in] u Device u.
 * \param [in] index PAIR state index
 *
 * \return true/false
 */
static inline int
pair_index_is_in_range1(int u, int m, uint8_t ldb, int index, int pkm)
{
    int v6 = 0;

    if ((index < RANGE1_FIRST(u, m, ldb, pkm)) ||
        (index > RANGE1_LAST(u, m, ldb, pkm) && index < RANGE2_FIRST(u, m, ldb, pkm))) {
        v6 = 1;
    } else {
        v6 = 0;
    }
    return v6;
}

/*!
 * \brief Map single-wide index to dual-wide index
 *
 * \param [in] u Device u.
 * \param [in] idx single-wide index
 *
 * \return Dual-wide index
 */
static inline int
pair_index_encode(int u, int m, int idx)
{
    return (idx & (TCAM_DEPTH(u, m) - 1)) + ((idx >> 1) & ~(TCAM_DEPTH(u, m) - 1));
}

/*!
 * \brief Map dual-wide index to single-wide index
 *
 * \param [in] u Device u.
 * \param [in] idx dual-wide index
 *
 * \return Single-wide index
 */
static inline int
pair_index_decode(int u, int m, int idx)
{
    return (idx & (TCAM_DEPTH(u, m) - 1)) +
           ((idx << 1) & ~(2 * TCAM_DEPTH(u, m) - 1));
}



/*!
 * \brief Get min valid prefix in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 *
 * \return MIN valid prefix in pair TCAM
 */
static inline int
pair_min_pfx(int u, int m, uint8_t ldb, int la, int pkm)
{
    int min_pfx = MAX_PFX(u, m, ldb, la, pkm);

    while (PAIR_STATE_NEXT(u, m, ldb, min_pfx, pkm) != -1) {
        min_pfx = PAIR_STATE_NEXT(u, m, ldb, min_pfx, pkm);
    }
    return min_pfx;
}


/*!
 * \brief Get next pair state index for a given index
 *
 * \param [in] u Device u.
 * \param [in] index state index
 *
 * \return Next state index
 */
static inline int
pair_next_index(int u, int m, uint8_t ldb, int index, int pkm)
{
    int next_idx;

    if (pair_index_is_in_range1(u, m, ldb, index, pkm)) {
        next_idx = index + 1;
        if (index_in_right_tcam(u, m, next_idx)) {
            next_idx += TCAM_DEPTH(u, m);
        }
    } else {
        if (index == RANGE1_LAST(u, m, ldb, pkm)) {
            next_idx = RANGE2_FIRST(u, m, ldb, pkm);
        } else {
            next_idx = index + 1;
        }
    }

    /* The equal is for a special case - max_v4_pfx */
    ALPM_ASSERT(next_idx <= KI_END(u, m, ldb, pkm) + 1);

    return next_idx;
}

/*!
 * \brief Get previous pair state index for a given index
 *
 * \param [in] u Device u.
 * \param [in] index state index
 *
 * \return Previous state index
 */
static int
pair_prev_index(int u, int m, uint8_t ldb, int index, int pkm)
{
    int prev_idx;

    if (pair_index_is_in_range1(u, m, ldb, index, pkm)) {
        prev_idx = index - 1;
        if (index_in_right_tcam(u, m, prev_idx)) {
            prev_idx -= TCAM_DEPTH(u, m);
        }
    } else {

        if (index == RANGE2_FIRST(u, m, ldb, pkm)) {
            prev_idx = RANGE1_LAST(u, m, ldb, pkm);
        } else {
            prev_idx = index - 1;
        }
    }

    return prev_idx;
}


/*!
 * \brief Get previous prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] req_fent Requesting number of free entries.
 *
 * \return INVALID_PFX or free pfx
 */
static int
pair_free_slot_search_prev(int u, int m, uint8_t ldb, int pfx, int req_fent, int pkm)
{
    int free_pfx = pfx;
    int fent = PAIR_STATE_FENT(u, m, ldb, pfx, pkm);

    while (fent < req_fent) {
        free_pfx = PAIR_STATE_PREV(u, m, ldb, free_pfx, pkm);
        if (free_pfx == -1) {
            /* No free entries on left side */
            free_pfx = INVALID_PFX;
            break;
        }
        fent += PAIR_STATE_FENT(u, m, ldb, free_pfx, pkm);
    }
    return free_pfx;
}


/*!
 * \brief Get next prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] req_fent Requesting number of free entries.
 *
 * \return INVALID_PFX or free pfx
 */
static int
pair_free_slot_search_next(int u, int m, uint8_t ldb, int pfx, int req_fent, int pkm)
{
    int free_pfx = pfx;
    int fent = PAIR_STATE_FENT(u, m, ldb, pfx, pkm);

    while (fent < req_fent) {
        free_pfx = PAIR_STATE_NEXT(u, m, ldb, free_pfx, pkm);
        if (free_pfx == -1) {
            /* No free entries on right side as well */
            free_pfx = INVALID_PFX;
            break;
        }
        fent += PAIR_STATE_FENT(u, m, ldb, free_pfx, pkm);
    }
    return free_pfx;
}

/*!
 * \brief Get prefix with free slot for IPv6 in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 *
 * \return INVALID_PFX or free pfx
 */
static int
pair_free_slot_search_for_d(int u, int m, uint8_t ldb, int pfx, int pkm)
{
    int free_pfx, free_pfx2;

    free_pfx = pair_free_slot_search_prev(u, m, ldb, pfx, 2, pkm);

    if (free_pfx == INVALID_PFX) {
        free_pfx = pair_free_slot_search_next(u, m, ldb, pfx, 2, pkm);
    }

    if (free_pfx == INVALID_PFX) {
        int v4_max_pfx;

        v4_max_pfx = MAX_PFX(u, m, ldb, LAYOUT_SH, pkm);

        free_pfx = pair_free_slot_search_next(u, m, ldb, v4_max_pfx, 1, pkm);

        if (free_pfx != INVALID_PFX && PAIR_STATE_FENT(u, m, ldb, free_pfx, pkm) == 1) {
            /* just to ensure free entries are enough, still return
             * the first found free_pfx
             */
            free_pfx2 = PAIR_STATE_NEXT(u, m, ldb, free_pfx, pkm);
            if (free_pfx2 != INVALID_PFX) {
                free_pfx2 = pair_free_slot_search_next(u, m, ldb, free_pfx2, 1, pkm);

                ALPM_ASSERT(free_pfx2 != free_pfx);
            }
            if (free_pfx2 == INVALID_PFX) {
                free_pfx = INVALID_PFX; /* no enough free entries */
            }
        }
    }
    return free_pfx;
}

/*!
 * \brief Get prefix with free slot for IPv4 in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 *
 * \return INVALID_PFX or free pfx
 */
static int
pair_free_slot_search_for_sh(int u, int m, uint8_t ldb, int pfx, int pkm)
{
    int free_pfx;

    free_pfx = pair_free_slot_search_next(u, m, ldb, pfx, 1, pkm);

    if (free_pfx == INVALID_PFX) {
        free_pfx = pair_free_slot_search_prev(u, m, ldb, pfx, 1, pkm);
    }

    if (free_pfx == INVALID_PFX) {
        int v6_min_pfx;

        v6_min_pfx = pair_min_pfx(u, m, ldb, LAYOUT_D, pkm);

        free_pfx = pair_free_slot_search_prev(u, m, ldb, v6_min_pfx, 1, pkm);
    }

    return free_pfx;
}


/*!
 * \brief Move entry from from_ent to to_ent.
 *
 * A movable prefix is the highest prefix good to move from pair to unpair.
 *
 * \param [in] u Device u.
 * \param [in] ldb Database.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] from_ent From pt index
 * \param [in] to_ent From pt index
 * \param [in] erase Clear from entry
 * \param [in] pkm Packing mode.
 * \param [in] is_last_pfx Set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift(int u, int m, uint8_t ldb, int pfx, int from_ent,
           int to_ent, int erase, int pkm, int is_last_pfx)
{
    int full;
    int key_type;
    l1_entry_t *le = NULL;
    bcmdrd_sid_t sid;
    uint32_t dt_bitmap;
    int from_data;
    int to_data;

    SHR_FUNC_ENTER(u);

    if (from_ent == to_ent) {
        SHR_EXIT();
    }

    le = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    key_type = pfx_to_key_type(u, m, pfx);

    if (!KEY_DATA_SEPARATE(u, m, !pfx_is_single_half(u, m, ldb, pfx, pkm))) {
        sid = pair_sid(u, m, key_type, pkm);
        SHR_IF_ERR_EXIT(
            read_l1_raw(u, m, sid, from_ent, le->de, le->se, &dt_bitmap));
        SHR_IF_ERR_EXIT(
            write_l1_raw(u, m, sid, to_ent, le->de, le->se,
                         HIT_OP_AUTO, 0x3, dt_bitmap));

        if (erase) {
            SHR_IF_ERR_EXIT(clear_l1_raw(u, m, sid, from_ent));
        }
    } else if (pfx_is_single_half(u, m, ldb, pfx, pkm)) {
        int wide;
        if (half_entry_key_type(u, m, key_type, pkm)) { /* data_index_x2 */
            from_data = (from_ent << 1);
            to_data = (to_ent << 1);
            wide = WIDE_HALF;
        } else {
            if (alpm_l1_info[u][m]->data_only_x2) {
                from_data = (from_ent << 1);
                to_data = (to_ent << 1);
            } else {
                from_data = from_ent;
                to_data = to_ent;
            }
            wide = WIDE_SINGLE;
        }
        /* Clear old targeting key */
        SHR_IF_ERR_EXIT(
            clear_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, wide));

        /* Read data_0 */
        SHR_IF_ERR_EXIT(
           read_l1_raw2(u, m, SID_TYPE_DATA, from_data, wide, le->de, &dt_bitmap));
        /* Write data_0 */
        SHR_IF_ERR_EXIT(
           write_l1_raw2(u, m, SID_TYPE_DATA, to_data, wide, le->de, dt_bitmap));

        if (half_entry_key_type(u, m, key_type, pkm)) { /* data_index_x2 */
            from_data++;
            to_data++;
            /* Read data_1 */
            SHR_IF_ERR_EXIT(
                read_l1_raw2(u, m, SID_TYPE_DATA, from_data, wide, le->de, &dt_bitmap));
            /* Write data_1 */
            SHR_IF_ERR_EXIT(
               write_l1_raw2(u, m, SID_TYPE_DATA, to_data, wide, le->de, dt_bitmap));
        }

        /* Read key */
        SHR_IF_ERR_EXIT(
           read_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, wide, le->de, NULL));

        /* Write key */
        SHR_IF_ERR_EXIT(
           write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, wide, le->de, 0));
        if (erase || is_last_pfx) {
            SHR_IF_ERR_EXIT(clear_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, wide));
        }
    } else {
        if (alpm_l1_info[u][m]->data_only_x2) {
            from_data = (from_ent << 1);
            to_data = (to_ent << 1);
        } else {
            from_data = from_ent;
            to_data = to_ent;
        }

        /* Clear old targeting key_0 */
        SHR_IF_ERR_EXIT(
            clear_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_DOUBLE));

        /* Read data */
        SHR_IF_ERR_EXIT(
            read_l1_raw2(u, m, SID_TYPE_DATA, from_data, WIDE_DOUBLE, le->de,
                         &dt_bitmap));

        /* Write data */
        SHR_IF_ERR_EXIT(
            write_l1_raw2(u, m, SID_TYPE_DATA, to_data, WIDE_DOUBLE, le->de,
                          dt_bitmap));

        /* Read key_1 */
        SHR_IF_ERR_EXIT(
            read_l1_raw2(u, m, SID_TYPE_TCAM2, from_ent, WIDE_DOUBLE, le->de, NULL));

        /* Write key_1 */
        SHR_IF_ERR_EXIT(
            write_l1_raw2(u, m, SID_TYPE_TCAM2, to_ent, WIDE_DOUBLE, le->de, 0));

        /* Read key_0 */
        SHR_IF_ERR_EXIT(
            read_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_DOUBLE, le->de, NULL));

        /* Write key_0 */
        SHR_IF_ERR_EXIT(
            write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_DOUBLE, le->de, 0));
        if (erase || is_last_pfx) {
            SHR_IF_ERR_EXIT(clear_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_DOUBLE));
            SHR_IF_ERR_EXIT(clear_l1_raw2(u, m, SID_TYPE_TCAM2, from_ent, WIDE_DOUBLE));
        }

    }

    from_ent <<= 1;
    to_ent <<= 1;

    full = !half_entry_key_type(u, m, key_type, pkm);

    SHR_IF_ERR_EXIT(pivot_move(u, m, from_ent, to_ent, full, 0));
    SHR_IF_ERR_EXIT(pivot_move(u, m, ++from_ent, ++to_ent, full, 0));

exit:
    if (le) {
        shr_lmm_free(l1_entries_lmem[u][m], le);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check and update range1 last pfx if required.
 *
 * Update last pfx and its span state based on a given pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return SHR_E_XXX.
 */
static int
pair_range1_last_pfx_update(int u, int m, uint8_t ldb, int pfx, int pkm)
{
    int prev_pfx;
    int next_pfx;
    int start, end;
    SHR_FUNC_ENTER(u);

    start = PAIR_STATE_START(u, m, ldb, pfx, pkm);
    end = PAIR_STATE_END(u, m, ldb, pfx, pkm);

    if (LAST_PFX_SPAN(u, m, ldb, pkm)) {
        if (RANGE1_LAST_PFX(u, m, ldb, pkm) == pfx) {
            if (end <= RANGE1_LAST(u, m, ldb, pkm)) {
                LAST_PFX_SPAN(u, m, ldb, pkm) = 0;

            } else if (start >= RANGE2_FIRST(u, m, ldb, pkm)) {
                LAST_PFX_SPAN(u, m, ldb, pkm) = 0;
                RANGE1_LAST_PFX(u, m, ldb, pkm) =
                    PAIR_STATE_PREV(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm), pkm);
            }
        }
    } else {
        if (start <= RANGE1_LAST(u, m, ldb, pkm) && end >= RANGE2_FIRST(u, m, ldb, pkm)) {
            LAST_PFX_SPAN(u, m, ldb, pkm) = 1;
            RANGE1_LAST_PFX(u, m, ldb, pkm) = pfx;

        } else if (start <= RANGE1_LAST(u, m, ldb, pkm)) {
            next_pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);

            if (next_pfx == INVALID_PFX ||
                PAIR_STATE_START(u, m, ldb, next_pfx, pkm) >= RANGE2_FIRST(u, m, ldb, pkm)) {
                RANGE1_LAST_PFX(u, m, ldb, pkm) = pfx;
            }

        } else if (start >= RANGE2_FIRST(u, m, ldb, pkm)) {
            SANITY_CHECK_PAIR_PFX(u, m, ldb, pfx, pkm);

            prev_pfx = PAIR_STATE_PREV(u, m, ldb, pfx, pkm);
            ALPM_ASSERT(prev_pfx != INVALID_PFX);
            if (PAIR_STATE_END(u, m, ldb, prev_pfx, pkm) <= RANGE1_LAST(u, m, ldb, pkm)) {
                RANGE1_LAST_PFX(u, m, ldb, pkm) = prev_pfx;
            }
        }
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift IPv4 prefix entries 1 entry DOWN.
 *
 * Shift IPv4 prefix entries 1 entry DOWN, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] ldb Database.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] pkm Packing mode.
 * \param [in] is_last_pfx set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_sh_down(int u, int m, uint8_t ldb, int pfx, int pkm, int is_last_pfx)
{
    int end;
    int end_next;
    int start;
    int start_next;
    int erase = 0;
    SHR_FUNC_ENTER(u);

    start = PAIR_STATE_START(u, m, ldb, pfx, pkm);
    end = PAIR_STATE_END(u, m, ldb, pfx, pkm);

    SANITY_CHECK_PAIR_PFX(u, m, ldb, pfx, pkm);

    end_next   = pair_next_index(u, m, ldb, end, pkm);
    start_next = pair_next_index(u, m, ldb, start, pkm);

    ALPM_ASSERT(PAIR_STATE_VENT(u, m, ldb, pfx, pkm));

    /*
     * If this is first v4 prefix, erase the first entry when shift down.
     * This is necessary only bacause the ptcache are separated for
     * different views.
     */
    if (PAIR_STATE_PREV(u, m, ldb, pfx, pkm) == MAX_PFX(u, m, ldb, LAYOUT_SH, pkm)) {
        erase = 1;
    }

    if (pfx_half_entry(u, m, ldb, pfx, pkm)) {
        /* Last entry is half full -> keep it last. */
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, pfx, end, end_next, 0, pkm, is_last_pfx));
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, pfx, start, end, erase, pkm, is_last_pfx));
    } else {
        /* No need to erase start entry, as it will be done by caller. */
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, pfx, start, end_next, erase, pkm, is_last_pfx));
    }

    PAIR_STATE_START(u, m, ldb, pfx, pkm) = start_next;
    PAIR_STATE_END(u, m, ldb, pfx, pkm)   = end_next;

    pair_range1_last_pfx_update(u, m, ldb, pfx, pkm);

exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief Shift IPv4 prefix entries 1 entry UP.
 *
 * Shift IPv4 prefix entries 1 entry UP, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] ldb Database.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] pkm Packing mode.
 * \param [in] is_last_pfx Set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_sh_up(int u, int m, uint8_t ldb, int pfx, int pkm, int is_last_pfx)
{
    int end;
    int end_prev;
    int start;
    int start_prev = -1;
    SHR_FUNC_ENTER(u);

    start = PAIR_STATE_START(u, m, ldb, pfx, pkm);
    end   = PAIR_STATE_END(u, m, ldb, pfx, pkm);

    end_prev   = pair_prev_index(u, m, ldb, end, pkm);
    start_prev = pair_prev_index(u, m, ldb, start, pkm);

    /* Don't move empty prefix .
     * This condition is hit if pfx is created for the first time
     */
    if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0) {
        ALPM_ASSERT(0);

        PAIR_STATE_START(u, m, ldb, pfx, pkm) = start_prev;
        PAIR_STATE_END(u, m, ldb, pfx, pkm) = pair_prev_index(u, m, ldb, start_prev, pkm);
        pair_range1_last_pfx_update(u, m, ldb, pfx, pkm);
        SHR_EXIT();
    }


    if (pfx_half_entry(u, m, ldb, pfx, pkm)) {
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, pfx, end_prev, start_prev, 0, pkm, is_last_pfx));
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, pfx, end, end_prev, 1, pkm, is_last_pfx));
    } else {
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, pfx, end, start_prev, 1, pkm, is_last_pfx));
    }

    PAIR_STATE_START(u, m, ldb, pfx, pkm) = start_prev;
    PAIR_STATE_END(u, m, ldb, pfx, pkm)   = end_prev;

    pair_range1_last_pfx_update(u, m, ldb, pfx, pkm);


exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift prefix entries 1 entry DOWN.
 *
 * Shift prefix entries 1 entry DOWN, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] ldb Database.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] pkm Packing mode.
 * \param [in] is_last_pfx Set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_down(int u, int m, uint8_t ldb, int pfx, int pkm, int is_last_pfx)
{
    int start;
    int start_next;
    int end;
    int end_next;
    SHR_FUNC_ENTER(u);

    if (pfx_is_single_half(u, m, ldb, pfx, pkm)) {
        SHR_ERR_EXIT(pair_shift_sh_down(u, m, ldb, pfx, pkm, is_last_pfx));
    }

    start = PAIR_STATE_START(u, m, ldb, pfx, pkm);
    end   = PAIR_STATE_END(u, m, ldb, pfx, pkm);
    end_next   = pair_next_index(u, m, ldb, end, pkm);
    start_next = pair_next_index(u, m, ldb, start, pkm);

    SHR_IF_ERR_EXIT(
        pair_shift(u, m, ldb, pfx, start, end_next, 0, pkm, is_last_pfx));

    PAIR_STATE_START(u, m, ldb, pfx, pkm) = start_next;
    PAIR_STATE_END(u, m, ldb, pfx, pkm) = end_next;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift prefix entries 1 entry UP.
 *
 * Shift prefix entries 1 entry UP, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] ldb Database.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] pkm Packing mode.
 * \param [in] is_last_pfx Set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_up(int u, int m, uint8_t ldb, int pfx, int pkm, int is_last_pfx)
{
    int end;
    int end_prev;
    int start;
    int start_prev;
    int erase = 0;
    SHR_FUNC_ENTER(u);

    if (pfx_is_single_half(u, m, ldb, pfx, pkm)) {
        SHR_ERR_EXIT(pair_shift_sh_up(u, m, ldb, pfx, pkm, is_last_pfx));
    }

    start = PAIR_STATE_START(u, m, ldb, pfx, pkm);
    end   = PAIR_STATE_END(u, m, ldb, pfx, pkm);
    start_prev = pair_prev_index(u, m, ldb, start, pkm);
    end_prev = pair_prev_index(u, m, ldb, end, pkm);

    /* Don't move empty prefix .
     * This condition is hit if pfx is created for the first time
     */
    if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0) {
        ALPM_ASSERT(0);
        PAIR_STATE_START(u, m, ldb, pfx, pkm) = start_prev;
        PAIR_STATE_END(u, m, ldb, pfx, pkm) = start_prev - 1;
        SHR_EXIT();
    }

    /*
     * If this is last prefix, erase the last entry when shift up.
     * This is necessary only bacause the ptcache are separated for
     * different views.
     */
    if (PAIR_STATE_NEXT(u, m, ldb, pfx, pkm) == -1) {
        erase = 1;
    }
    SHR_IF_ERR_EXIT(
        pair_shift(u, m, ldb, pfx, end, start_prev, erase, pkm, is_last_pfx));

    PAIR_STATE_START(u, m, ldb, pfx, pkm) = start_prev;
    PAIR_STATE_END(u, m, ldb, pfx, pkm)   = end_prev;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create an empty prefix into the prefix state link.
 *
 * Create an empty prefix into the prefix state link, also take
 * half free entries from previous prefix.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] prev_pfx Prefix that is just larger than pfx
 *
 * \return Nothing.
 */
static void
pair_group_create(int u, int m, uint8_t ldb, int pfx, int prev_pfx, int pkm)
{
    int next_pfx = INVALID_PFX;
    int start    = INVALID_INDEX;
#ifdef ALPM_DEBUG
    int kt = pfx_to_key_type(u, m, pfx);
#endif
    if (prev_pfx == INVALID_PFX) {
        ALPM_ASSERT_ERRMSG(0);
        return;
    }
    ALPM_ASSERT(PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) >=
                half_entry_key_type(u, m, kt, pkm) ? 1 : 2);

    /* Insert new prefix into list */
    next_pfx = PAIR_STATE_NEXT(u, m, ldb, prev_pfx, pkm);
    if (next_pfx != -1) {
        PAIR_STATE_PREV(u, m, ldb, next_pfx, pkm) = pfx;
    }
    PAIR_STATE_NEXT(u, m, ldb, pfx, pkm) = next_pfx;
    PAIR_STATE_PREV(u, m, ldb, pfx, pkm) = prev_pfx;
    PAIR_STATE_NEXT(u, m, ldb, prev_pfx, pkm) = pfx;

    /* Assign half fent from prev_pfx */
    if (!pfx_is_single_half(u, m, ldb, pfx, pkm)) {

        /* To make sure fent are always even for ipv6, also make sure
         * if v6 fent cannot be split evenly ((fent>>1) % 2 != 0), give
         * remaining to new pfx.
         */
        PAIR_STATE_FENT(u, m, ldb, pfx, pkm) = ((PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) >> 1) + 1) >> 1;
        PAIR_STATE_FENT(u, m, ldb, pfx, pkm) <<= 1;
        PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) -= PAIR_STATE_FENT(u, m, ldb, pfx, pkm);

        start = PAIR_STATE_END(u, m, ldb, prev_pfx, pkm);
        start = pair_index_encode(u, m, start);
        start += (PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) >> 1) + 1;
        start = pair_index_decode(u, m, start);
    } else {
        /* If fent cannot be split evenly, give remaining to new_pfx */
        PAIR_STATE_FENT(u, m, ldb, pfx, pkm) = (PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) + 1) >> 1;
        PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) -= PAIR_STATE_FENT(u, m, ldb, pfx, pkm);

        start = PAIR_STATE_END(u, m, ldb, prev_pfx, pkm);
        start += PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) + 1;

        /*
         * If prev_pfx is last_pfx and it does not span, the fent
         * given could span. If the fent does span, start
         * should be compensated with IPv6 space, else the
         * last_pfx should be changed to new pfx.
 */
        if (RANGE1_LAST_PFX(u, m, ldb, pkm) == prev_pfx && !LAST_PFX_SPAN(u, m, ldb, pkm)) {
            if (start > RANGE1_LAST(u, m, ldb, pkm)) {
                start += RANGE2_FIRST(u, m, ldb, pkm) - (RANGE1_LAST(u, m, ldb, pkm) + 1);
            } else {
                RANGE1_LAST_PFX(u, m, ldb, pkm) = pfx;
            }
        }
    }

    /* Setup new prefix */
    PAIR_STATE_START(u, m, ldb, pfx, pkm) = start;
    PAIR_STATE_END(u, m, ldb, pfx, pkm) = pair_prev_index(u, m, ldb, start, pkm);
    PAIR_STATE_VENT(u, m, ldb, pfx, pkm) = 0;

    SANITY_CHECK_PAIR_STATE(u, m, ldb, prev_pfx, pkm);
    SANITY_CHECK_PAIR_STATE(u, m, ldb, pfx, pkm);
}


/*!
 * \brief Remove freed prefix from the prefix state link.
 *
 * Remove empty prefix from the list and give back all free entries.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return Nothing.
 */
static void
pair_group_destroy(int u, int m, uint8_t ldb, int pfx, int pkm)
{
    int prev_pfx;
    int next_pfx;

    if (PAIR_STATE_PREV(u, m, ldb, pfx, pkm) == -1) {
        return;
    }

    if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0) {
        /* prev_pfx always present */
        prev_pfx = PAIR_STATE_PREV(u, m, ldb, pfx, pkm);
        next_pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);

        /* Give away fent to prev_pfx */
        PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) += PAIR_STATE_FENT(u, m, ldb, pfx, pkm);

        /* Remove from list */
        PAIR_STATE_NEXT(u, m, ldb, prev_pfx, pkm) = next_pfx;
        if (next_pfx != -1) {
            PAIR_STATE_PREV(u, m, ldb, next_pfx, pkm) = prev_pfx;
        }

        /* Reset the state */
        PAIR_STATE_FENT(u, m, ldb, pfx, pkm) = 0;
        PAIR_STATE_NEXT(u, m, ldb, pfx, pkm) = -1;
        PAIR_STATE_PREV(u, m, ldb, pfx, pkm) = -1;
        PAIR_STATE_START(u, m, ldb, pfx, pkm) = -1;
        PAIR_STATE_END(u, m, ldb, pfx, pkm) = -1;

        /* Reset last_pfx if possible */
        if (pfx_is_single_half(u, m, ldb, pfx, pkm)) {
            if (RANGE1_LAST_PFX(u, m, ldb, pkm) == pfx) {
                RANGE1_LAST_PFX(u, m, ldb, pkm) = prev_pfx;
            }
        }
    }

    return;
}


/*!
 * \brief Move part of IPv4 prefix entries in right tcam 1 entry DOWN.
 *
 * Shift part2 IPv4 prefix (last pfx & span) entries 1 entry DOWN,
 * while preserving last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] ldb Database.
 * \param [in] last_pfx Translated prefix (Global prefix value).
 * \param [in] pkm Packing mode.
 * \param [in] is_last_pfx Set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
pair_move_sh_part2_down(int u, int m, uint8_t ldb, int part2_pfx,
                        int pkm, int is_last_pfx)
{
    int free_pfx;
    int end, end_next;
    int new_start = INVALID_PFX;

    SHR_FUNC_ENTER(u);

    if (LAST_PFX_SPAN(u, m, ldb, pkm)) {
        new_start = INVALID_PFX;
    } else {
        new_start = pair_next_index(u, m, ldb, RANGE2_FIRST(u, m, ldb, pkm), pkm);
    }

    free_pfx = pair_free_slot_search_for_sh(u, m, ldb, part2_pfx, pkm);

    if (free_pfx == INVALID_PFX) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else if (free_pfx < part2_pfx) {
        SHR_IF_ERR_EXIT(pair_free_slot_move_up(u, m, ldb, part2_pfx, free_pfx, pkm));

    } else if (free_pfx > part2_pfx) {
        SHR_IF_ERR_EXIT(pair_free_slot_move_down(u, m, ldb, part2_pfx, free_pfx, pkm));

        ALPM_ASSERT(new_start != PAIR_STATE_START(u, m, ldb, part2_pfx, pkm));
        new_start = PAIR_STATE_START(u, m, ldb, part2_pfx, pkm);
    }

    end = PAIR_STATE_END(u, m, ldb, part2_pfx, pkm);
    end_next = pair_next_index(u, m, ldb, end, pkm);

    if (end_next == RANGE2_FIRST(u, m, ldb, pkm)) {  /* just freed during move_down */
        SHR_EXIT();
    }

    if (pfx_half_entry(u, m, ldb, part2_pfx, pkm))  {
        /* Last entry is half full -> keep it last. */
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, part2_pfx, end, end_next, 0, pkm, is_last_pfx));
        /*
         * Erase the original entry.
         * This is necessary only bacause the ptcache are separated for
         * different views.
         */
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, part2_pfx, RANGE2_FIRST(u, m, ldb, pkm),
                       end, 1, pkm, is_last_pfx));
    } else {
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, part2_pfx, RANGE2_FIRST(u, m, ldb, pkm),
                       end_next, 1, pkm, is_last_pfx));
    }

    PAIR_STATE_END(u, m, ldb, part2_pfx, pkm) = end_next;
    if (new_start != INVALID_PFX) {
        PAIR_STATE_START(u, m, ldb, part2_pfx, pkm) = new_start;
    }

exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief Shift part of IPv4 prefix entries in right tcam 1 entry UP.
 *
 * Shift part2 IPv4 prefix (last pfx & span) entries 1 entry UP,
 * while preserving last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] ldb Database.
 * \param [in] last_pfx Translated prefix (Global prefix value).
 * \param [in] pkm Packing mode.
 * \param [in] is_last_pfx Set if it's last pfx in shifting.
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_sh_part2_up(int u, int m, uint8_t ldb, int last_pfx,
                       int pkm, int is_last_pfx)
{
    int from_ent, to_ent;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(LAST_PFX_SPAN(u, m, ldb, pkm) && last_pfx == RANGE1_LAST_PFX(u, m, ldb, pkm));

    to_ent   = RANGE2_FIRST(u, m, ldb, pkm);
    from_ent = PAIR_STATE_END(u, m, ldb, last_pfx, pkm);

    ALPM_ASSERT(from_ent > to_ent);

    /* Move half first if that half entry is not the only existing one */
    if (pfx_half_entry(u, m, ldb, last_pfx, pkm) && (from_ent - 1 > to_ent)) {
        SHR_IF_ERR_EXIT(
            pair_shift(u, m, ldb, last_pfx, from_ent - 1, to_ent, 1, pkm, is_last_pfx));
        to_ent = from_ent - 1;
    }
    SHR_IF_ERR_EXIT(
        pair_shift(u, m, ldb, last_pfx, from_ent, to_ent, 1, pkm, is_last_pfx));
    PAIR_STATE_END(u, m, ldb, last_pfx, pkm) -= 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Move free slot from free_pfx to pfx (free_pfx > pfx).
 *
 * Move free slot from free_pfx to pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] free_pfx Translated prefix with free entries
 * \param [in] pkm Packing mode.
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_move_down(int u, int m, uint8_t ldb, int pfx, int free_pfx, int pkm)
{
    int next_pfx = -1;
    int fent_incr = 0;
    int is_last_pfx;
    SHR_FUNC_ENTER(u);

    fent_incr = pfx_is_single_half(u, m, ldb, free_pfx, pkm) ? 1 : 2;

    while (free_pfx > pfx && free_pfx >= 0) {
        next_pfx = PAIR_STATE_NEXT(u, m, ldb, free_pfx, pkm);

        if (next_pfx == -1) {
            int max_v4_pfx;

            max_v4_pfx = MAX_PFX(u, m, ldb, LAYOUT_SH, pkm);

            /* Update RANGE1_FIRST, RANGE1_LAST, RANGE1_LAST_PFX accordingly */

            RANGE1_FIRST(u, m, ldb, pkm)--; /* boundary up */
            if (index_in_right_tcam(u, m, RANGE1_FIRST(u, m, ldb, pkm))) {
                LAST_PFX_SPAN(u, m, ldb, pkm) = 0;
                RANGE1_LAST_PFX(u, m, ldb, pkm) = max_v4_pfx;

                RANGE1_FIRST(u, m, ldb, pkm) -= TCAM_DEPTH(u, m);
                RANGE1_LAST(u, m, ldb, pkm) = RANGE1_FIRST(u, m, ldb, pkm);
                RANGE2_FIRST(u, m, ldb, pkm) = RANGE1_FIRST(u, m, ldb, pkm) + TCAM_DEPTH(u, m);
            } else {
                RANGE2_FIRST(u, m, ldb, pkm) = RANGE1_FIRST(u, m, ldb, pkm) + TCAM_DEPTH(u, m);
            }
            /* Reset position of max_v4_pfx */
            PAIR_STATE_START(u, m, ldb, max_v4_pfx, pkm) = RANGE1_FIRST(u, m, ldb, pkm);
            PAIR_STATE_END(u, m, ldb, max_v4_pfx, pkm) = RANGE1_FIRST(u, m, ldb, pkm) - 1;

            next_pfx = PAIR_STATE_NEXT(u, m, ldb, max_v4_pfx, pkm);
            is_last_pfx = (next_pfx == pfx);

            if (next_pfx != INVALID_PFX) {
                /*
                 * Take care of pfx == max_v4_pfx first to make sure for this
                 * case, the pfx can always have free entries assigned.
                 */
                if (pfx == max_v4_pfx) {
                    PAIR_STATE_FENT(u, m, ldb, max_v4_pfx, pkm) += 1;
                    if (LAST_PFX_SPAN(u, m, ldb, pkm)) {
                        SHR_IF_ERR_EXIT(
                            pair_shift_sh_part2_up(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm),
                                                   pkm, is_last_pfx));
                    }
                    PAIR_STATE_FENT(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm), pkm) += 1;
                    if (RANGE1_LAST_PFX(u, m, ldb, pkm) >= pfx) {
                        next_pfx = RANGE1_LAST_PFX(u, m, ldb, pkm); /* a little thrust */
                    }

                } else if (LAST_PFX_SPAN(u, m, ldb, pkm) &&
                           RANGE1_LAST_PFX(u, m, ldb, pkm) == next_pfx) {
                    SHR_IF_ERR_EXIT(
                        pair_shift_sh_part2_up(u, m, ldb, next_pfx, pkm, is_last_pfx));

                    PAIR_STATE_FENT(u, m, ldb, next_pfx, pkm) += 1;

                    SHR_IF_ERR_EXIT(
                        pair_shift_sh_up(u, m, ldb, next_pfx, pkm, is_last_pfx));
                    PAIR_STATE_FENT(u, m, ldb, next_pfx, pkm) += 1;
                } else {

                    SHR_IF_ERR_EXIT(
                        pair_shift_sh_up(u, m, ldb, next_pfx, pkm, is_last_pfx));
                    PAIR_STATE_FENT(u, m, ldb, next_pfx, pkm) += 1;
                    if (index_in_right_tcam(u, m, PAIR_STATE_START(u, m, ldb, next_pfx, pkm))) {
                        SHR_IF_ERR_EXIT(
                            pair_shift_sh_up(u, m, ldb, next_pfx, pkm, is_last_pfx));
                        PAIR_STATE_FENT(u, m, ldb, next_pfx, pkm) += 1;

                        ALPM_ASSERT(RANGE1_LAST_PFX(u, m, ldb, pkm) == next_pfx);
                    } else {
                        if (LAST_PFX_SPAN(u, m, ldb, pkm)) {
                            SHR_IF_ERR_EXIT(
                                pair_shift_sh_part2_up(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm),
                                                       pkm, is_last_pfx));
                        }
                        PAIR_STATE_FENT(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm), pkm) += 1;
                        if (RANGE1_LAST_PFX(u, m, ldb, pkm) >= pfx) {
                            next_pfx = RANGE1_LAST_PFX(u, m, ldb, pkm); /* a little thrust */
                        }
                    }
                }
                SANITY_CHECK_PAIR_STATE(u, m, ldb, next_pfx, pkm);
            } else {
                PAIR_STATE_FENT(u, m, ldb, max_v4_pfx, pkm) += 2;
            }

            PAIR_STATE_FENT(u, m, ldb, free_pfx, pkm) -= 2;

            SANITY_CHECK_PAIR_STATE(u, m, ldb, max_v4_pfx, pkm);
            SANITY_CHECK_PAIR_STATE(u, m, ldb, free_pfx, pkm);

            fent_incr = 1;
        } else {
            is_last_pfx = (next_pfx == pfx);
            SHR_IF_ERR_EXIT(pair_shift_up(u, m, ldb, next_pfx, pkm, is_last_pfx));

            PAIR_STATE_FENT(u, m, ldb, free_pfx, pkm) -= fent_incr;
            PAIR_STATE_FENT(u, m, ldb, next_pfx, pkm) += fent_incr;
            SANITY_CHECK_PAIR_STATE(u, m, ldb, next_pfx, pkm);
            SANITY_CHECK_PAIR_STATE(u, m, ldb, free_pfx, pkm);
        }

        free_pfx = next_pfx;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Move free slot from free_pfx to pfx (free_pfx < pfx).
 *
 * Move free slot from free_pfx to pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] free_pfx Translated prefix with free entries
 * \param [in] pkm Packing mode.
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_move_up(int u, int m, uint8_t ldb, int pfx, int free_pfx, int pkm)
{
    int prev_pfx = -1;
    int fent_incr = 0;
    int max_v4_pfx;
    int min_v6_pfx;
    int last_next_pfx;
    int is_last_pfx;
    int rv;
    int last_pfx, next_pfx;

    SHR_FUNC_ENTER(u);

    fent_incr = pfx_is_single_half(u, m, ldb, free_pfx, pkm) ? 1 : 2;

    while (free_pfx < pfx) {
        prev_pfx = PAIR_STATE_PREV(u, m, ldb, free_pfx, pkm);

        if (prev_pfx == -1) {
            last_next_pfx = INVALID_PFX;

            min_v6_pfx = pair_min_pfx(u, m, ldb, LAYOUT_D, pkm);
            max_v4_pfx = MAX_PFX(u, m, ldb, LAYOUT_SH, pkm);
            prev_pfx = min_v6_pfx;
            is_last_pfx = (prev_pfx == pfx);

            ALPM_ASSERT(free_pfx == max_v4_pfx);

            ALPM_ASSERT(PAIR_STATE_FENT(u, m, ldb, free_pfx, pkm) >= 1);

            do {
                int start, end;
                start = PAIR_STATE_START(u, m, ldb, max_v4_pfx, pkm);
                end = PAIR_STATE_END(u, m, ldb, max_v4_pfx, pkm);
                PAIR_STATE_START(u, m, ldb, max_v4_pfx, pkm) = pair_next_index(u, m, ldb, start, pkm);
                PAIR_STATE_END(u, m, ldb, max_v4_pfx, pkm)   = pair_next_index(u, m, ldb, end, pkm);
                PAIR_STATE_FENT(u, m, ldb, max_v4_pfx, pkm) -= 1;
            } while (0);

            LOG_D((BSL_META("MOVE UP: Free pfx[%d] down, "), free_pfx));

            if (LAST_PFX_SPAN(u, m, ldb, pkm)) {
                rv = pair_move_sh_part2_down(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm),
                                             pkm, is_last_pfx);
                ALPM_ASSERT(SHR_SUCCESS(rv));
                SHR_IF_ERR_EXIT(rv);

                PAIR_STATE_FENT(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm), pkm) -= 1;
                LOG_D((BSL_META("Last pfx[%d] down, "), RANGE1_LAST_PFX(u, m, ldb, pkm)));

            } else if (PAIR_STATE_START(u, m, ldb, max_v4_pfx, pkm) == RANGE2_FIRST(u, m, ldb, pkm)) {
                /*
                 * Corner case where just shift max_v4_pfx 1 entry DOWN,
                 * and even the max_v4_pfx resides in range2.
                 */
                rv = pair_move_sh_part2_down(u, m, ldb, max_v4_pfx, pkm, is_last_pfx);
                ALPM_ASSERT(SHR_SUCCESS(rv));
                SHR_IF_ERR_EXIT(rv);

                PAIR_STATE_FENT(u, m, ldb, max_v4_pfx, pkm) -= 1;
                LOG_D((BSL_META("Max v4 pfx[%d] down again, "), max_v4_pfx));

            }  else {
                last_next_pfx = PAIR_STATE_NEXT(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm), pkm);

                if (last_next_pfx != INVALID_PFX &&
                    PAIR_STATE_START(u, m, ldb, last_next_pfx, pkm) == RANGE2_FIRST(u, m, ldb, pkm)) {
                    SHR_IF_ERR_EXIT(
                        pair_move_sh_part2_down(u, m, ldb, last_next_pfx,
                                                pkm, is_last_pfx));

                    PAIR_STATE_FENT(u, m, ldb, last_next_pfx, pkm) -= 1;

                } else {
                    PAIR_STATE_FENT(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm), pkm) -= 1;
                }
                LOG_D((BSL_META("Last next pfx[%d] down. last pfx [%d], "),
                       last_next_pfx, RANGE1_LAST_PFX(u, m, ldb, pkm)));
            }

            /* Update RANGE1_FIRST, RANGE1_LAST, RANGE1_LAST_PFX accordingly */

            RANGE1_FIRST(u, m, ldb, pkm)++; /* boundary down */
            if (index_in_right_tcam(u, m, RANGE1_FIRST(u, m, ldb, pkm))) {

                /* Adjust boundary */
                RANGE1_FIRST(u, m, ldb, pkm) += TCAM_DEPTH(u, m);
                RANGE1_LAST(u, m, ldb, pkm)   = RANGE1_FIRST(u, m, ldb, pkm) + TCAM_DEPTH(u, m) - 1;
                RANGE2_FIRST(u, m, ldb, pkm)  = RANGE1_FIRST(u, m, ldb, pkm) + TCAM_DEPTH(u, m);

                /* Search for new range1_last_pfx */
                last_pfx = free_pfx;
                next_pfx = PAIR_STATE_NEXT(u, m, ldb, last_pfx, pkm);

                while (next_pfx != -1 &&
                       PAIR_STATE_START(u, m, ldb, next_pfx, pkm) <= RANGE1_LAST(u, m, ldb, pkm)) {

                    last_pfx = next_pfx;
                    next_pfx = PAIR_STATE_NEXT(u, m, ldb, last_pfx, pkm);
                }
                RANGE1_LAST_PFX(u, m, ldb, pkm) = last_pfx;

                /* Then new last_pfx_span */
                if (PAIR_STATE_END(u, m, ldb, last_pfx, pkm) >= RANGE2_FIRST(u, m, ldb, pkm)) {
                    LAST_PFX_SPAN(u, m, ldb, pkm) = 1;
                } else {
                    LAST_PFX_SPAN(u, m, ldb, pkm) = 0;
                }

            } else {
                /* Adjust boundary */
                RANGE2_FIRST(u, m, ldb, pkm) = RANGE1_FIRST(u, m, ldb, pkm) + TCAM_DEPTH(u, m);
            }

            LOG_D((BSL_META("=>last pfx %d span %d \n"),
                   RANGE1_LAST_PFX(u, m, ldb, pkm), LAST_PFX_SPAN(u, m, ldb, pkm)));


            PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) += 2;

            /* Reset position of max_v4_pfx */
            PAIR_STATE_START(u, m, ldb, max_v4_pfx, pkm) = RANGE1_FIRST(u, m, ldb, pkm);
            PAIR_STATE_END(u, m, ldb, max_v4_pfx, pkm) = RANGE1_FIRST(u, m, ldb, pkm) - 1;

            if (last_next_pfx != INVALID_PFX) {
                SANITY_CHECK_PAIR_STATE(u, m, ldb, last_next_pfx, pkm);
            }
            SANITY_CHECK_PAIR_STATE(u, m, ldb, RANGE1_LAST_PFX(u, m, ldb, pkm), pkm);
            SANITY_CHECK_PAIR_STATE(u, m, ldb, max_v4_pfx, pkm);

            fent_incr = 2;
        } else {
            is_last_pfx = (prev_pfx == pfx);

            SHR_IF_ERR_EXIT(pair_shift_down(u, m, ldb, free_pfx, pkm, is_last_pfx));
            PAIR_STATE_FENT(u, m, ldb, free_pfx, pkm) -= fent_incr;
            PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) += fent_incr;
            SANITY_CHECK_PAIR_STATE(u, m, ldb, prev_pfx, pkm);
            SANITY_CHECK_PAIR_STATE(u, m, ldb, free_pfx, pkm);
        }

        free_pfx = prev_pfx;
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Allocate a slot for the new IPv4 prefix.
 *
 * Allocate a slot for the new IPv4 prefix rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [out] free_slot Translated index
 * \param [in] pkm Packing mode.
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_alloc_sh(int u, int m, uint8_t ldb, int pfx, int *free_slot, int pkm)
{
    int free_pfx, pfx2;
    int prev_pfx = INVALID_PFX;
    int end;
    int half_kt;
    int kt, len;
    uint8_t app;
    alpm_vrf_type_t vt;
    SHR_FUNC_ENTER(u);

    /* Check if half of the entry is free. if yes add to it */
    pfx_decode(u, m, pfx, &kt, &vt, &len, &app);
    kt = key_type_get(u, m, kt, len, app, vt);
    half_kt = half_entry_key_type(u, m, kt, pkm);

    if (half_kt && pfx_half_entry(u, m, ldb, pfx, pkm)) {
        end = PAIR_STATE_END(u, m, ldb, pfx, pkm);
        *free_slot = HALF_INDEX_R(end);
        pfx_half_entry_update(u, m, ldb, pfx, pkm);
        SHR_EXIT();
    }

    if (PAIR_STATE_FENT(u, m, ldb, pfx, pkm) < 1) {
        pfx2 = pfx;

        if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0) {
            /* Find prev pfx */
            prev_pfx = MAX_PFX(u, m, ldb, LAYOUT_SH, pkm);
            while (PAIR_STATE_NEXT(u, m, ldb, prev_pfx, pkm) > pfx) {
                prev_pfx = PAIR_STATE_NEXT(u, m, ldb, prev_pfx, pkm);
            }
            pfx2 = prev_pfx;
        }

        free_pfx = pair_free_slot_search_for_sh(u, m, ldb, pfx2, pkm);

        if (free_pfx == INVALID_PFX) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        } else if (free_pfx < pfx2) {
            SHR_IF_ERR_EXIT(pair_free_slot_move_up(u, m, ldb, pfx2, free_pfx, pkm));
        } else if (free_pfx > pfx2) {
            SHR_IF_ERR_EXIT(pair_free_slot_move_down(u, m, ldb, pfx2, free_pfx, pkm));
        }
    }

    if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0 && prev_pfx != INVALID_PFX) {
        pair_group_create(u, m, ldb, pfx, prev_pfx, pkm);
    }

    end = PAIR_STATE_END(u, m, ldb, pfx, pkm);
    end = pair_next_index(u, m, ldb, end, pkm);
    PAIR_STATE_END(u, m, ldb, pfx, pkm) = end;
    PAIR_STATE_VENT(u, m, ldb, pfx, pkm) += 1;
    PAIR_STATE_FENT(u, m, ldb, pfx, pkm) -= 1;

    ALPM_ASSERT(end <= KI_END(u, m, ldb, pkm));

    if (half_kt) {
        pfx_half_entry_update(u, m, ldb, pfx, pkm);
    }

    pair_range1_last_pfx_update(u, m, ldb, pfx, pkm);

    SANITY_CHECK_PAIR_STATE(u, m, ldb, pfx, pkm);

    *free_slot = half_kt ? HALF_INDEX_L(end) : FULL_INDEX(end);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Allocate a slot for the new prefix.
 *
 * Allocate a slot for the new prefix rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [out] free_slot Translated index
 * \param [in] pkm Packing mode.
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_alloc(int u, int m, uint8_t ldb, int pfx, int *free_slot, int pkm)
{
    int free_pfx, pfx2;
    int prev_pfx = INVALID_PFX;
    int new_end;
    SHR_FUNC_ENTER(u);

    if (pfx_is_single_half(u, m, ldb, pfx, pkm)) {
        SHR_ERR_EXIT(pair_free_slot_alloc_sh(u, m, ldb, pfx, free_slot, pkm));
    }

    if (PAIR_STATE_FENT(u, m, ldb, pfx, pkm) < 2) {
        pfx2 = pfx;

        if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0) {
            /* Find prev pfx */
            prev_pfx = MAX_PFX(u, m, ldb, LAYOUT_D, pkm);
            while (PAIR_STATE_NEXT(u, m, ldb, prev_pfx, pkm) > pfx) {
                prev_pfx = PAIR_STATE_NEXT(u, m, ldb, prev_pfx, pkm);
            }
            pfx2 = prev_pfx;
        }

        free_pfx = pair_free_slot_search_for_d(u, m, ldb, pfx2, pkm);

        if (free_pfx == INVALID_PFX) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        } else if (free_pfx > pfx2) {
            SHR_IF_ERR_EXIT(
                pair_free_slot_move_down(u, m, ldb, pfx2, free_pfx, pkm));
        } else if (free_pfx < pfx2) {
            SHR_IF_ERR_EXIT(
                pair_free_slot_move_up(u, m, ldb, pfx2, free_pfx, pkm));
        }

    }

    if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0 && prev_pfx != INVALID_PFX) {
        pair_group_create(u, m, ldb, pfx, prev_pfx, pkm);
    }

    ALPM_ASSERT((PAIR_STATE_FENT(u, m, ldb, pfx, pkm) >= 2) &&
                (PAIR_STATE_FENT(u, m, ldb, pfx, pkm) & 1) == 0);

    new_end = pair_next_index(u, m, ldb, PAIR_STATE_END(u, m, ldb, pfx, pkm), pkm);
    PAIR_STATE_END(u, m, ldb, pfx, pkm) = new_end;
    PAIR_STATE_VENT(u, m, ldb, pfx, pkm) += 1;
    PAIR_STATE_FENT(u, m, ldb, pfx, pkm) -= 2;

    SANITY_CHECK_PAIR_STATE(u, m, ldb, pfx, pkm);

    *free_slot = FULL_INDEX(new_end);
exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief Free a slot for the IPv4 prefix.
 *
 * Free a slot for the IPv4 prefix, rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] del_slot Translated index
 * \param [in] pkm Packing mode.
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_free_sh(int u, int m, uint8_t ldb, int pfx, int del_slot, int pkm)
{
    int           from_ent;
    int           from_prev;
    int           to_ent;
    l1_entry_t *le1 = NULL;
    l1_entry_t *le2 = NULL;

    uint32_t      *from_de, *from_se, *to_de, *to_se;
    int           from_v1, to_v1;
    int           src_slot;
    int           full;
    int           half_kt;
    bcmdrd_sid_t  sid;
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    int len;
    uint8_t app;
    uint32_t dt_bitmap0, dt_bitmap1;
    int from_data;
    int to_data;

    SHR_FUNC_ENTER(u);
    from_ent = PAIR_STATE_END(u, m, ldb, pfx, pkm);
    to_ent = HW_INDEX(del_slot);
    pfx_decode(u, m, pfx, &kt, &vt, &len, &app);
    kt = key_type_get(u, m, kt, len, app, vt);


    sid = pair_sid(u, m, kt, pkm);
    from_prev = pair_prev_index(u, m, ldb, from_ent, pkm);

    le1 = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le1 == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    le2 = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le2 == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    to_de = le1->de;
    to_se = le1 ->se;
    if (from_ent == to_ent && !KEY_DATA_SEPARATE(u, m, FALSE)) {
        from_de = to_de;
        from_se = to_se;
    } else {
        from_de = le2->de;
        from_se = le2 ->se;
    }

    half_kt = half_entry_key_type(u, m, kt, pkm);
    if (half_kt) {

        from_v1 = !pfx_half_entry(u, m, ldb, pfx, pkm);
        to_v1 = IS_HALF_R_INDEX(del_slot);
        src_slot = (from_ent << 1) + from_v1;
        del_slot = (to_ent << 1) + to_v1;


        ALPM_ASSERT(src_slot >= del_slot);

        if (from_v1) {
            L1_HALF_COUNT(u, m, ldb, kt, vt, pkm)++;
            ALPM_ASSERT(L1_HALF_COUNT(u, m, ldb, kt, vt, pkm) >= 0);
        } else {
            /* Remaining half entry got deleted. Decrement half entry count.*/
            L1_HALF_COUNT(u, m, ldb, kt, vt, pkm)--;
            ALPM_ASSERT(L1_HALF_COUNT(u, m, ldb, kt, vt, pkm) >= 0);
            PAIR_STATE_VENT(u, m, ldb, pfx, pkm) -= 1;
            PAIR_STATE_FENT(u, m, ldb, pfx, pkm) += 1;

            PAIR_STATE_END(u, m, ldb, pfx, pkm) = from_prev;

            pair_range1_last_pfx_update(u, m, ldb, pfx, pkm);
        }


        if (!KEY_DATA_SEPARATE(u, m, FALSE)) {
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, m, sid, to_ent, to_de, to_se, &dt_bitmap0));
            if (from_ent != to_ent) {
                SHR_IF_ERR_EXIT(
                    read_l1_raw(u, m, sid, from_ent, from_de, from_se, &dt_bitmap1));
            }

            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_half_entry_move(u, m, from_de, to_de, from_v1, to_v1, pkm));

            SHR_IF_ERR_EXIT(
                write_l1_raw(u, m, sid, to_ent, to_de, to_se,
                             HIT_OP_AUTO, 1 << to_v1, dt_bitmap0));

            if (from_ent != to_ent) {
                SHR_IF_ERR_EXIT(
                    write_l1_raw(u, m, sid, from_ent, from_de, from_se,
                                 HIT_OP_NONE, 0, dt_bitmap1));
            }
        } else {
            /* Done */
            if (from_ent != to_ent) {
                /* Read deleting entry key */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, NULL));
                /* Clear deleting entry key content */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_clear(u, m, to_de, to_v1, pkm));
                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));

                /* Read last entry data */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_DATA, src_slot, WIDE_HALF, from_de, &dt_bitmap0));
                /* Write last entry data to deleting posititon */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_DATA, del_slot, WIDE_HALF, from_de, dt_bitmap0));

                /* Read last entry key */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_HALF, from_de, NULL));

                /* Move last entry key to deleting entry key */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_move(u, m, from_de, to_de, from_v1, to_v1, pkm));

                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));

                /* Write last entry key*/
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_HALF, from_de, 0));
            } else if (from_v1 != to_v1) {
                /* Read deleting entry key */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, NULL));
                /* Clear deleting entry key content */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_clear(u, m, to_de, to_v1, pkm));
                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));

                /* Read last entry data */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_DATA, src_slot, WIDE_HALF, from_de, &dt_bitmap0));
                /* Write last entry data to deleting posititon */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_DATA, del_slot, WIDE_HALF, from_de, dt_bitmap0));

                /* Move last entry key to deleting entry key */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_move(u, m, to_de, to_de, from_v1, to_v1, pkm));

                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));
            } else {
                /* Read deleting entry key */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, NULL));
                /* Clear deleting entry key content */
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_half_key_clear(u, m, to_de, to_v1, pkm));
                /* Write deleting entry key */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_HALF, to_de, 0));
            }
        }


        /* Update half free entry */
        pfx_half_entry_update(u, m, ldb, pfx, pkm);
    } else { /* full kt */

        src_slot = (from_ent << 1);
        del_slot = (to_ent << 1);

        if (alpm_l1_info[u][m]->data_only_x2) {
            from_data = src_slot;
            to_data = del_slot;
        } else {
            from_data = from_ent;
            to_data = to_ent;
        }

        if (!KEY_DATA_SEPARATE(u, m, FALSE)) {
            if (to_ent != from_ent) {
                SHR_IF_ERR_EXIT(
                    read_l1_raw(u, m, sid, from_ent, from_de, from_se, &dt_bitmap0));
                SHR_IF_ERR_EXIT(
                    write_l1_raw(u, m, sid, to_ent, from_de, from_se,
                                 HIT_OP_AUTO, 0x1, dt_bitmap0));
            }
            SHR_IF_ERR_EXIT(clear_l1_raw(u, m, sid, from_ent));
        } else {
            /* Done 2 */
            if (to_ent != from_ent) {
                /* Clear key of deleted entry to prevent misforwarding */
                SHR_IF_ERR_EXIT(clear_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_SINGLE));
                /* Read data of last */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_DATA, from_data, WIDE_SINGLE,
                                 from_de, &dt_bitmap0));
                /* Write data to deleted position */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_DATA, to_data, WIDE_SINGLE,
                                  from_de, dt_bitmap0));

                /* Read key of last */
                SHR_IF_ERR_EXIT(
                    read_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_SINGLE,
                                 from_de, NULL));
                /* Write key to deleted position */
                SHR_IF_ERR_EXIT(
                    write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_SINGLE,
                                  from_de, 0));
            }
            /* Delete key is sufficient. */
            SHR_IF_ERR_EXIT(clear_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_SINGLE));
        }

        PAIR_STATE_VENT(u, m, ldb, pfx, pkm) -= 1;
        PAIR_STATE_FENT(u, m, ldb, pfx, pkm) += 1;
        PAIR_STATE_END(u, m, ldb, pfx, pkm) = from_prev;

        pair_range1_last_pfx_update(u, m, ldb, pfx, pkm);
    }

    full = pfx_to_key_type(u, m, pfx);
    full = !half_entry_key_type(u, m, full, pkm);

    SHR_IF_ERR_EXIT(pivot_move(u, m, src_slot, del_slot, full, 1));

    if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0) {
        pair_group_destroy(u, m, ldb, pfx, pkm);
    }

exit:
    if (le1) {
        shr_lmm_free(l1_entries_lmem[u][m], le1);
    }
    if (le2) {
        shr_lmm_free(l1_entries_lmem[u][m], le2);
    }
    SHR_FUNC_EXIT();
}



/*!
 * \brief Free a slot for the prefix.
 *
 * Free a slot for the prefix, rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] del_slot Translated index
 * \param [in] pkm Packing mode.
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_free(int u, int m, uint8_t ldb, int pfx, int del_slot, int pkm)
{
    int           from_ent;
    int           to_ent;
    int           full;
    int           key_type;
    l1_entry_t *le = NULL;
    bcmdrd_sid_t  sid;
    uint32_t dt_bitmap;
    int from_data;
    int to_data;

    SHR_FUNC_ENTER(u);

    if (pfx_is_single_half(u, m, ldb, pfx, pkm)) {
        SHR_ERR_EXIT(pair_free_slot_free_sh(u, m, ldb, pfx, del_slot, pkm));
    }

    from_ent = PAIR_STATE_END(u, m, ldb, pfx, pkm);
    to_ent   = HW_INDEX(del_slot);
    PAIR_STATE_VENT(u, m, ldb, pfx, pkm) -= 1;
    PAIR_STATE_FENT(u, m, ldb, pfx, pkm) += 2;

    PAIR_STATE_END(u, m, ldb, pfx, pkm) = pair_prev_index(u, m, ldb, from_ent, pkm);

    le = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    key_type = pfx_to_key_type(u, m, pfx);
    sid = pair_sid(u, m, key_type, pkm);

    if (!KEY_DATA_SEPARATE(u, m, TRUE)) {
        if (to_ent != from_ent) {
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, m, sid, from_ent, le->de, le->se, &dt_bitmap));
            SHR_IF_ERR_EXIT(
                write_l1_raw(u, m, sid, to_ent, le->de, le->se,
                             HIT_OP_AUTO, 0x1, dt_bitmap));
        }

        SHR_IF_ERR_EXIT(clear_l1_raw(u, m, sid, from_ent));
    } else {
        if (alpm_l1_info[u][m]->data_only_x2) {
            from_data = from_ent << 1;
            to_data = to_ent << 1;
        } else {
            from_data = from_ent;
            to_data = to_ent;
        }

        /* Done */
        if (to_ent != from_ent) {
            /* Clear key_0 of deleting entry to prevent misforwarding */
            SHR_IF_ERR_EXIT(
                clear_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_DOUBLE));
            /* Read data of last */
            SHR_IF_ERR_EXIT(
                read_l1_raw2(u, m, SID_TYPE_DATA, from_data, WIDE_DOUBLE, le->de,
                             &dt_bitmap));
            /* Write data to deleted position */
            SHR_IF_ERR_EXIT(
                write_l1_raw2(u, m, SID_TYPE_DATA, to_data, WIDE_DOUBLE, le->de,
                              dt_bitmap));

            /* Read key_1 of last */
            SHR_IF_ERR_EXIT(
                read_l1_raw2(u, m, SID_TYPE_TCAM2, from_ent, WIDE_DOUBLE, le->de, NULL));
            /* Write key_1 to deleting position */
            SHR_IF_ERR_EXIT(
                write_l1_raw2(u, m, SID_TYPE_TCAM2, to_ent, WIDE_DOUBLE, le->de, 0));
            /* Read key_0 of last */
            SHR_IF_ERR_EXIT(
                read_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_DOUBLE, le->de, NULL));
            /* Write key_0 to deleting position */
            SHR_IF_ERR_EXIT(
                write_l1_raw2(u, m, SID_TYPE_TCAM, to_ent, WIDE_DOUBLE, le->de, 0));
        }
        /* Delete key is sufficient. */
        SHR_IF_ERR_EXIT(
            clear_l1_raw2(u, m, SID_TYPE_TCAM, from_ent, WIDE_DOUBLE));
        SHR_IF_ERR_EXIT(
            clear_l1_raw2(u, m, SID_TYPE_TCAM2, from_ent, WIDE_DOUBLE));
    }

    full = !half_entry_key_type(u, m, key_type, pkm);

    SHR_IF_ERR_EXIT(pivot_move(u, m, from_ent << 1, to_ent << 1, full, 1));

    if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0) {
        pair_group_destroy(u, m, ldb, pfx, pkm);
    }

exit:
    if (le) {
        shr_lmm_free(l1_entries_lmem[u][m], le);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the smallest prefix.
 *
 * Get the smallest IPv6_64 prefix, if not exist,
 * get the smallest IPv4 prefix.
 *
 * \param [in] u Device u.
 * \param [in] vt VRF type.
 *
 * \return SHR_E_XXX.
 */
static void
pair_smallest_lpm_prefix(int u, int m, uint8_t ldb, int pkm,
                         int *v4_pfx, int *v6_pfx)
{
    int pfx;

    *v4_pfx = *v6_pfx = INVALID_PFX;
    pfx = MAX_PFX(u, m, ldb, LAYOUT_D, pkm);
    pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    while (pfx != -1) {
        if (pfx_is_lpm_ipv6(u, m, pfx)) {
            *v6_pfx = pfx;
        }
        pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    }

    pfx = MAX_PFX(u, m, ldb, LAYOUT_SH, pkm);
    pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    while (pfx != -1) {
        if (pfx_is_kt(u, m, pfx, KEY_IPV4)) {
            *v4_pfx = pfx;
        }
        pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    }
}

static void
pair_largest_lpm_prefix(int u, int m, uint8_t ldb, int pkm,
                        int *v4_pfx, int *v6_pfx)
{
    int pfx;

    *v4_pfx = *v6_pfx = INVALID_PFX;
    pfx = MAX_PFX(u, m, ldb, LAYOUT_D, pkm);
    pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    while (pfx != -1) {
        if (pfx_is_lpm_ipv6(u, m, pfx) && *v6_pfx == INVALID_PFX) {
            *v6_pfx = pfx;
            break;
        }
        pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    }

    pfx = MAX_PFX(u, m, ldb, LAYOUT_SH, pkm);
    pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    while (pfx != -1) {
        if (pfx_is_kt(u, m, pfx, KEY_IPV4) && *v4_pfx == INVALID_PFX) {
            *v4_pfx = pfx;
            break;
        }
        pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
    }
}


static int
l1_stats_init(int u, int m, bool recover)
{
    alpm_key_type_t kt;
    uint8_t ldb, pkm;
    l1_db_info_t *l1_db;

    SHR_FUNC_ENTER(u);


    for (ldb = 0; ldb < L1_DBS; ldb++) {
        l1_db = &L1_DB(u, m, ldb);
        if (!l1_db->valid) {
            continue;
        }
        for (pkm = 0; pkm < PKM_NUM; pkm++) {
            for (kt = 0; kt < KEY_TYPES; kt++) {
                if (alpm_pkm_kt_weight[pkm][kt]) {
                L1_MAX_COUNT(u, m, ldb, kt, pkm) =
                     KI_SIZE(u, m, ldb, pkm) * 2 / alpm_pkm_kt_weight[pkm][kt];
                }
            }
        }
    }

    SHR_FUNC_EXIT();

}


static int
pair_state_init(int u, int m, bool recover)
{
    int sz;
    int i, fent, start = 0;
    int max_pfx_sh, max_pfx_q;
    uint8_t ldb, pkm;
    l1_db_info_t *l1_db;
    SHR_FUNC_ENTER(u);

    /* Use the biggest number */
    sz = 0;
    for (i = 0; i < COUNTOF(prefix_layout); i++) {
        sz += prefix_layout[i].num_pfx;
    }

    for (ldb = 0; ldb < L1_DBS; ldb++) {
        l1_db = &L1_DB(u, m, ldb);
        if (!l1_db->valid) {
            continue;
        }
        for (pkm = 0; pkm < PKM_NUM; pkm++) {
            if (!bcmptm_rm_alpm_pkm_is_pair(u, m, pkm)) {
                continue;
            }
            ALPM_ALLOC(PAIR_STATE(u, m, ldb, pkm), sizeof(block_state_t) * sz, "bcmptmRmalpmPairstate");
            sal_memset(PAIR_STATE(u, m, ldb, pkm), 0, sizeof(block_state_t) * sz);

            for (i = 0; i < sz; i++) {
                PAIR_STATE_START(u, m, ldb, i, pkm) = -1;
                PAIR_STATE_END(u, m, ldb, i, pkm)   = -1;
                PAIR_STATE_PREV(u, m, ldb, i, pkm)  = -1;
                PAIR_STATE_NEXT(u, m, ldb, i, pkm)  = -1;
                PAIR_STATE_VENT(u, m, ldb, i, pkm)  = 0;
                PAIR_STATE_FENT(u, m, ldb, i, pkm)  = 0;
            }

            fent = KI_SIZE(u, m, ldb, pkm);
            MAX_PFX(u, m, ldb, LAYOUT_D, pkm) = pfx_max(u, m, LAYOUT_D, pkm);
            MAX_PFX(u, m, ldb, LAYOUT_SH, pkm) = pfx_max(u, m, LAYOUT_SH, pkm);
            BASE_PFX(u, m, ldb, LAYOUT_D, pkm) = pfx_base(u, m, LAYOUT_D, pkm);
            BASE_PFX(u, m, ldb, LAYOUT_SH, pkm) = pfx_base(u, m, LAYOUT_SH, pkm);

            start = KI_START(u, m, ldb, pkm);
            max_pfx_q = MAX_PFX(u, m, ldb, LAYOUT_D, pkm);
            PAIR_STATE_FENT(u, m, ldb, max_pfx_q, pkm) = fent;
            PAIR_STATE_START(u, m, ldb, max_pfx_q, pkm) = start;
            PAIR_STATE_END(u, m, ldb, max_pfx_q, pkm) = start - 1;

            max_pfx_sh = MAX_PFX(u, m, ldb, LAYOUT_SH, pkm);
            PAIR_STATE_FENT(u, m, ldb, max_pfx_sh, pkm) = 0;
            PAIR_STATE_START(u, m, ldb, max_pfx_sh, pkm) = start + fent;
            PAIR_STATE_END(u, m, ldb, max_pfx_sh, pkm) = start + fent - 1;
            RANGE1_FIRST(u, m, ldb, pkm)    = PAIR_STATE_START(u, m, ldb, max_pfx_sh, pkm);
            RANGE2_FIRST(u, m, ldb, pkm)    = RANGE1_FIRST(u, m, ldb, pkm) + TCAM_DEPTH(u, m);
            RANGE1_LAST(u, m, ldb, pkm)     = RANGE2_FIRST(u, m, ldb, pkm) - 1;
            RANGE1_LAST_PFX(u, m, ldb, pkm) = max_pfx_sh;
            LAST_PFX_SPAN(u, m, ldb, pkm)   = 0;
    /*
            if (recover) {
                PAIR_STATE_FENT(u, m, ldb, MAX_PFX(u, m, ldb, IP_VER_6, pkm)) = 0;
                PAIR_STATE_FENT(u, m, ldb, MAX_PFX(u, m, ldb, IP_VER_4, pkm)) = 0;
            }
    */
        }
    }

exit:
    SHR_FUNC_EXIT();

}

static void
pair_state_cleanup(int u, int m)
{
    uint8_t ldb, pkm;
    if (L1_INFO(u, m)) {
        for (ldb = 0; ldb < L1_DBS; ldb++) {
            for (pkm = 0; pkm < PKM_NUM; pkm++) {
                if (!bcmptm_rm_alpm_pkm_is_pair(u, m, pkm)) {
                    continue;
                }
                SHR_FREE(PAIR_STATE(u, m, ldb, pkm));
            }
        }
    }

}


static void
pair_stats_cleanup(int u, int m)
{
    return;
}

static bool
pair_is_dw(int u, int m, alpm_arg_t *arg)
{
    ln_index_t index = arg->index[LEVEL1];
    return !IS_HALF_INDEX(index) &&
        is_pair_sid(u, m, pair_sid(u, m, arg->key.kt, arg->pkm));
}

/*!
 * \brief Update an entry into pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be inserted
 *
 * \return SHR_E_XXX
 */
static int
pair_update(int u, int m, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);

    if (!KEY_DATA_SEPARATE(u, m, pair_is_dw(u, m, arg))) {
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry(u, m, arg, FALSE, PAIR));
    } else {
        /* Write data only */
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry2(u, m, arg, FALSE, SID_TYPE_DATA, PAIR));
    }
    if (arg->a1.direct_route) {
        ALPM_ASSERT(arg->pivot[LEVEL1]);
        *(arg->pivot[LEVEL1]->ad) = arg->ad;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Insert an entry into pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be inserted
 *
 * \return SHR_E_XXX
 */
static int
pair_insert(int u, int m, alpm_arg_t *arg, int pkm)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    int rv;
    uint8_t ldb = arg->db;

    SHR_FUNC_ENTER(u);
    ALPM_ASSERT(arg->index[LEVEL1] == INVALID_INDEX);
    rv = pair_free_slot_alloc(u, m, ldb, arg->key.pfx, &arg->index[LEVEL1], pkm);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_RESOURCE) {
        /* Shut error messages prints, as E_RESOURCE is really not an error */
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    arg->pkm = pkm;

    if (!KEY_DATA_SEPARATE(u, m, pair_is_dw(u, m, arg))) {
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry(u, m, arg, TRUE, PAIR));
    } else {
        /* Write data then key */
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry2(u, m, arg, TRUE, SID_TYPE_DATA, PAIR));

        /* Write data then key */
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry2(u, m, arg, TRUE, SID_TYPE_TCAM, PAIR));

    }

    if (!arg->in_move) {
        alpm_pivot_t *pivot;
        int index2;
        SHR_IF_ERR_EXIT(l1_pvt_alloc(u, m, arg, &pivot));
        SHR_IF_ERR_EXIT(l1_pvt_insert(u, m, arg, pivot));
        arg->pivot[LEVEL1] = pivot;
        ALPM_ASSERT(pivot);
        index2 = arr_idx_from_pvt_idx(u, m, arg->index[LEVEL1]);
        L1_PVT_INFO(u, m, index2) = pivot;
    }

    kt = arg->key.kt;
    vt = arg->key.vt;
    ALPM_ASSERT_ERRMSG(kt < KEY_TYPES);
    ALPM_ASSERT_ERRMSG(vt < VRF_TYPES);
    L1_USED_COUNT(u, m, ldb, kt, vt, pkm)++;
    if (half_entry_key_type(u, m, kt, pkm)) {
        ALPM_ASSERT(IS_HALF_INDEX(arg->index[LEVEL1]));
        if (IS_HALF_R_INDEX(arg->index[LEVEL1])) {
            L1_HALF_COUNT(u, m, ldb, kt, vt, pkm)--;
            ALPM_ASSERT(L1_HALF_COUNT(u, m, ldb, kt, vt, pkm) >= 0);
        } else {
            L1_HALF_COUNT(u, m, ldb, kt, vt, pkm)++;
            ALPM_ASSERT(L1_HALF_COUNT(u, m, ldb, kt, vt, pkm) >= 0);
        }
    }
exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Delete an entry from pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be deleted
 *
 * \return SHR_E_XXX
 */
static int
pair_delete(int u, int m, alpm_arg_t *arg)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    alpm_pivot_t    *pivot = NULL;
    uint8_t ldb, pkm;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(arg->index[LEVEL1] != INVALID_INDEX);

    ldb = arg->db;
    pkm = index_to_pkm(u, m, HW_INDEX(arg->index[LEVEL1]), NULL);

    SHR_IF_ERR_EXIT(
        pair_free_slot_free(u, m, ldb, arg->key.pfx, arg->index[LEVEL1], pkm));

    if (!arg->in_move) {
        SHR_IF_ERR_EXIT(l1_pvt_delete(u, m, arg, &pivot));
        SHR_IF_ERR_EXIT(l1_pvt_free(u, m, arg->db, arg->a1.direct_route, pivot));
    }

    kt = arg->key.kt;
    vt = arg->key.vt;
    L1_USED_COUNT(u, m, ldb, kt, vt, pkm)--;

    /* VRF_PIVOT_REF_DEC(u, m, w_vrf, kt); */
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get args from pair TCAM for a given index
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM arg with index set
 *
 * \return SHR_E_XXX
 */
static inline int
pair_get(int u, int m, alpm_arg_t *arg)
{
    bcmdrd_sid_t sid;
    sid = pair_sid(u, m, arg->key.kt, arg->pkm);
    return decode_read_l1_entry(u, m, sid, arg, NULL);
}





/*!
 * \brief Get level1 used count.
 *
 * \param [in] u Device u.
 * \param [in] kt Key type
 * \param [in] vt VRF type
 *
 * \return level 1 used count.
 */
static int
l1_used_count(int u, int m, uint8_t ldb, alpm_key_type_t kt, alpm_vrf_type_t vt)
{
    int pkm;
    int used_count = 0;
    for (pkm = 0; pkm < PKM_NUM; pkm++) {
        if (!KI_VALID(u, m, ldb, pkm)) {
            continue;
        }
        used_count += pkm_used_count(u, m, ldb, kt, vt, pkm);
    }
    return used_count;
}

/*!
 * \brief Get level1 free count.
 *
 * \param [in] u Device u.
 * \param [in] kt Key type
 * \param [in] vt VRF type
 *
 * \return level 1 free count.
 */
static int
l1_free_count(int u, int m, uint8_t ldb, alpm_key_type_t kt, alpm_vrf_type_t vt)
{
    int pkm;
    int max_count = 0;
    for (pkm = 0; pkm < PKM_NUM; pkm++) {
        if (!KI_VALID(u, m, ldb, pkm)) {
            continue;
        }
        max_count += L1_MAX_COUNT(u, m, ldb, kt, pkm);
    }

    return max_count - l1_used_count(u, m, ldb, kt, vt);
}





/*!
 * \brief Deterine if we can move pair 1 entry DOWN
 *
 * \param [in] u Device u.
 * \param [in] in_pfx In pair prefix
 * \param [out] in_pfx Out pair prefix, if it is ok to move
 *
 * \return True if ok to move, else false.
 */
static int
can_pkm_move_down(int u, int m, uint8_t ldb, int in_pfx, int *out_pfx,
                  int from_pkm, int to_pkm)
{
    int v6, len;
    int v4_pfx, v6_pfx;
    alpm_vrf_type_t vt, vt2;

    if (!KI_SIZE(u, m, ldb, to_pkm)) {
        return 0;
    }

    pfx_decode(u, m, in_pfx, &v6, &vt, NULL, NULL);

    if (bcmptm_rm_alpm_pkm_is_pair(u, m, from_pkm)) {
        pair_smallest_lpm_prefix(u, m, ldb, from_pkm, &v4_pfx, &v6_pfx);
    } else {
        unpair_smallest_lpm_prefix(u, m, ldb, from_pkm, &v4_pfx, &v6_pfx);
    }

    if (v6_pfx != INVALID_PFX) {
        pfx_decode(u, m, v6_pfx, NULL, NULL, &len, NULL);
        if (len > ipv6_pkm_length(u, m, to_pkm)) {
            v6_pfx = INVALID_PFX;
        }
    }

    if (v6) {
        /* If unpair pfx is IPv4 then we should move IPv4, as it's more impact */
        if (v4_pfx != INVALID_PFX) {
            *out_pfx = v4_pfx;
            return 1;
        }
        /*
         * Indicating no IPv4.
         * Move the entry of top IPv6 pfx, if that pfx is
         * bigger than insert pfx.
         */
        if (v6_pfx != INVALID_PFX) {
            if (v6_pfx <= in_pfx) {
                *out_pfx = v6_pfx;
                return 1;
            } else {
                pfx_decode(u, m, v6_pfx, NULL, &vt2, NULL, NULL);
                if (vt != vt2) {
                    *out_pfx = v6_pfx;
                    return 1;
                }
            }
        }

    } else {
        if (v4_pfx != INVALID_PFX && v4_pfx <= in_pfx) {
            *out_pfx = v4_pfx;
            return 1;
        }
        if (v6_pfx != INVALID_PFX) {
            *out_pfx = v6_pfx;
            return 1;
        }
    }
    return 0;
}



/*!
 * \brief Deterine if we can move unpair32 1 entry UP
 *
 * \param [in] u Device u.
 * \param [in] in_pfx In unpair32 prefix
 * \param [out] in_pfx Out unpair32 prefix, if it is ok to move
 *
 * \return True if ok to move, else false.
 */
static int
can_pkm_move_up(int u, int m, uint8_t ldb, int in_pfx, int *out_pfx, int from_pkm, int to_pkm)
{
    int v6;
    int v4_pfx, v6_pfx;
    alpm_vrf_type_t vt, vt2;

    if (!KI_SIZE(u, m, ldb, to_pkm)) {
        return 0;
    }

    pfx_decode(u, m, in_pfx, &v6, &vt, NULL, NULL);

    if (bcmptm_rm_alpm_pkm_is_pair(u, m, from_pkm)) {
        pair_largest_lpm_prefix(u, m, ldb, from_pkm, &v4_pfx, &v6_pfx);
    } else {
        unpair_largest_lpm_prefix(u, m, ldb, from_pkm, &v4_pfx, &v6_pfx);
    }

    if (v6) {
        /* If unpair pfx is IPv4 then we should move IPv4, as it's more impact */
        if (v4_pfx != INVALID_PFX) {
            *out_pfx = v4_pfx;
            return 1;
        }
        /*
         * Indicating no IPv4.
         * Move the entry of top IPv6 pfx, if that pfx is
         * bigger than insert pfx.
         */
        if (v6_pfx != INVALID_PFX) {
            if (v6_pfx >= in_pfx) {
                *out_pfx = v6_pfx;
                return 1;
            } else {
                pfx_decode(u, m, v6_pfx, NULL, &vt2, NULL, NULL);
                if (vt != vt2) {
                    *out_pfx = v6_pfx;
                    return 1;
                }
            }
        }
    } else {
        if (v4_pfx != INVALID_PFX && v4_pfx >= in_pfx) {
            *out_pfx = v4_pfx;
            return 1;
        }
        if (v6_pfx != INVALID_PFX) {
            *out_pfx = v6_pfx;
            return 1;
        }
    }
    return 0;
}

/*!
 * \brief Move entries from pair TCAM to unpair-64 TCAM
 *
 * Move IPv6_64 or IPv4 entries from pair TCAM to unpair-64 TCAM if possible.
 * This is to create space for pair TCAM.
 *
 * \param [in] u Device u.
 * \param [in] move_pfx Move pfx
 * \param [out] remain_cnt Remaning number of entries to be moved
 * \param [out] moved_cnt Moved number of entries
 *
 * \return SHR_E_XXX.
 */
static int
ripple_move(int u, int m, uint8_t ldb, int move_pfx, uint8_t *remain_cnt,
            uint8_t *moved_cnt, int from_pkm, int to_pkm)
{
    alpm_arg_t *unpair_arg[2] = {0};
    int count = 0;
    int idx = 0;
    int half_kt;
    alpm_key_type_t kt;
    bcmdrd_sid_t sid;
    SHR_FUNC_ENTER(u);

    unpair_arg[0] = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (unpair_arg[0] == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    unpair_arg[1] = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (unpair_arg[1] == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    bcmptm_rm_alpm_arg_init(u, m, unpair_arg[0]);
    bcmptm_rm_alpm_arg_init(u, m, unpair_arg[1]);

    /* HW index will do */
    unpair_arg[0]->index[LEVEL1] = PAIR_STATE_END(u, m, ldb, move_pfx, from_pkm);
    unpair_arg[0]->key.pfx = unpair_arg[1]->key.pfx = move_pfx;
    unpair_arg[0]->key.kt  = pfx_to_key_type(u, m, move_pfx);
    if (bcmptm_rm_alpm_pkm_is_pair(u, m, from_pkm)) {
        sid = pair_sid(u, m, unpair_arg[0]->key.kt, from_pkm);
    } else {
        sid = unpair_sid(u, m, unpair_arg[0]->key.kt, from_pkm);
    }

    unpair_arg[0]->pkm = unpair_arg[1]->pkm = from_pkm;
    pfx_decode(u, m, move_pfx, NULL, NULL, NULL, &unpair_arg[0]->key.t.app);
    pfx_decode(u, m, move_pfx, NULL, NULL, NULL, &unpair_arg[1]->key.t.app);
    unpair_arg[1]->db = unpair_arg[0]->db = ldb;
    SHR_IF_ERR_EXIT(decode_read_l1_entry(u, m, sid, unpair_arg[0], unpair_arg[1]));
    *remain_cnt = unpair_arg[0]->valid + unpair_arg[1]->valid;

    kt = pfx_to_key_type(u, m, move_pfx);
    half_kt = half_entry_key_type(u, m, kt, from_pkm);
    count = half_kt ? 2 : 1;

    /* Reverse order has a performance advantage */
    for (idx = count - 1; idx >= 0; idx--) {
        if (unpair_arg[idx]->valid) {
            alpm_pivot_t *pivot = NULL;
            int new_index, arr_idx;
            int rv;

            SHR_IF_ERR_EXIT(l1_pvt_lookup(u, m, unpair_arg[idx], &pivot));
            ALPM_ASSERT(HW_INDEX(pivot->index) == unpair_arg[0]->index[LEVEL1]);

            if (pivot->bkt && pivot->bkt->bpm) {
                unpair_arg[idx]->ad.user_data_type = pivot->bkt->bpm->ad->user_data_type;
            }
            /* To skip pivot alloc/insert/free/delete */
            unpair_arg[idx]->in_move = 1;
            unpair_arg[idx]->index[LEVEL1] = INVALID_INDEX;
            rv = unpair_insert(u, m, unpair_arg[idx], to_pkm);
            if (rv == SHR_E_RESOURCE) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
            (*remain_cnt)--;
            (*moved_cnt)++;
            new_index = unpair_arg[idx]->index[LEVEL1];

            unpair_arg[idx]->index[LEVEL1] = pivot->index;
            if (bcmptm_rm_alpm_pkm_is_pair(u, m, from_pkm)) {
                SHR_IF_ERR_EXIT(pair_delete(u, m, unpair_arg[idx]));
            } else {
                SHR_IF_ERR_EXIT(unpair_delete(u, m, unpair_arg[idx]));
            }

            arr_idx = arr_idx_from_pvt_idx(u, m, pivot->index);
            ALPM_ASSERT(L1_PVT_INFO(u, m, arr_idx) == NULL);

            arr_idx = arr_idx_from_pvt_idx(u, m, new_index);
            ALPM_ASSERT(L1_PVT_INFO(u, m, arr_idx) == NULL);

            pivot->index = new_index;
            L1_PVT_INFO(u, m, arr_idx) = pivot;
        }
    }

exit:
    if (unpair_arg[0]) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], unpair_arg[0]);
    }
    if (unpair_arg[1]) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], unpair_arg[1]);
    }
    SHR_FUNC_EXIT();
}

static int
read_l1_raw3(int u, int m, int index, uint32_t *de, uint8_t wide, uint32_t *dt_bitmap)
{
    uint32_t dt_bitmap1;
    alpm_dev_info_t *dev_info;
    uint32_t tcam[ALPM_L1_ENT_MAX] = {0};
    uint32_t data[ALPM_L1_ENT_MAX] = {0};
    uint32_t entry[ALPM_L1_ENT_MAX] = {0};
    int data_index;

    SHR_FUNC_ENTER(u);


    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INTERNAL);


    if (wide == WIDE_HALF || alpm_l1_info[u][m]->data_only_x2) {
        data_index = index << 1;
    } else {
        data_index = index;
    }

    if (wide == WIDE_DOUBLE) {
        SHR_IF_ERR_EXIT(
            read_l1_raw2(u, m, SID_TYPE_DATA, data_index, wide, data, dt_bitmap));
        SHR_IF_ERR_EXIT(
            read_l1_raw2(u, m, SID_TYPE_TCAM, index, wide, tcam, NULL));
        SHR_IF_ERR_EXIT(
            read_l1_raw2(u, m, SID_TYPE_TCAM2, index, wide, entry, NULL));
        bcmdrd_field_set(de,
                         dev_info->double_tcam_0_start_bit,
                         dev_info->double_tcam_0_end_bit,
                         &tcam[0]);
        bcmdrd_field_set(de,
                         dev_info->double_tcam_1_start_bit,
                         dev_info->double_tcam_1_end_bit,
                         &entry[0]);
        bcmdrd_field_set(de,
                         dev_info->double_data_start_bit,
                         dev_info->double_data_end_bit,
                         &data[0]);
    } else {
        SHR_IF_ERR_EXIT(
            read_l1_raw2(u, m, SID_TYPE_TCAM, index, wide, tcam, NULL));
        if (wide == WIDE_HALF) {
            SHR_IF_ERR_EXIT(
                read_l1_raw2(u, m, SID_TYPE_DATA, data_index, wide, data, dt_bitmap));
            SHR_IF_ERR_EXIT(
                read_l1_raw2(u, m, SID_TYPE_DATA, data_index + 1, wide, entry,
                             &dt_bitmap1));
            if (dt_bitmap) {
                *dt_bitmap |= (dt_bitmap1 << 1);
            }
        } else {
            SHR_IF_ERR_EXIT(
                read_l1_raw2(u, m, SID_TYPE_DATA, data_index, wide, data, dt_bitmap));
        }
        bcmdrd_field_set(de,
                         dev_info->single_tcam_start_bit,
                         dev_info->single_tcam_end_bit,
                         &tcam[0]);
        bcmdrd_field_set(de,
                         dev_info->single_data_0_start_bit,
                         dev_info->single_data_0_end_bit,
                         &data[0]);
        bcmdrd_field_set(de,
                         dev_info->single_data_1_start_bit,
                         dev_info->single_data_1_end_bit,
                         &entry[0]);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
multi_depth_combined_sid_index_map(int u, int m, uint8_t wide, int index,
                                   bcmdrd_sid_t *sid,
                                   int *sid_index)
{
    alpm_sid_index_map_t map_info;

    SHR_FUNC_ENTER(u);
    map_info.index = index;
    map_info.sid_type = SID_TYPE_COMBINED;
    map_info.wide = wide;

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_l1_sid_index_map(u, m, &map_info));
    *sid_index = map_info.sid_index;
    *sid = map_info.sid;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read dip/sip entry from cmdproc for a given sid & index.
 *
 * Expect index as raw pt index of single-wide view.
 *
 * \param [in] u Device u.
 * \param [in] sid DRD symbol id
 * \param [in] index raw pt index of single_wide view
 * \param [out] de dip entry
 * \param [out] se sip entry
 *
 * \return SHR_E_XXX.
 */
static int
read_l1_raw(int u, int m, bcmdrd_sid_t sid, int index, uint32_t *de, uint32_t *se,
            uint32_t *dt_bitmap)
{
    int hw_idx;
    bcmdrd_sid_t hw_sid;
    int phy_blk_index;
    uint8_t wide;

    SHR_FUNC_ENTER(u);

    if (L1_TCAMS_SHARED(u, m)) {
        if (is_pair_sid(u, m, sid)) {
            wide = WIDE_DOUBLE;
        } else {
            wide = WIDE_SINGLE; /* No need to distinguish SINGLE and HALF.*/
        }
        phy_blk_index = phy_block_convert_index(u, m, index, false);
        SHR_IF_ERR_EXIT(
            multi_depth_combined_sid_index_map(u, m, wide, phy_blk_index,
                                               &hw_sid, &hw_idx));
    } else {
        hw_idx = l1_hw_index_adjust_base(u, m, sid, index);
        hw_sid = sid;
    }

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read(u, m, hw_sid, hw_idx, ALPM_L1_ENT_MAX, de, dt_bitmap));
    bcmptm_rm_alpm_internals[u][m][DB0][LEVEL1].read++;

#ifdef ALPM_DEBUG
{
    int rv;
    LOG_D((" Read [sid %d index %d [", sid, index));
    for (rv = 0; rv < bcmdrd_pt_entry_wsize(u, sid); rv++) {
        LOG_D((" %08x", de[rv]));
    }
    LOG_D(("]\n"));
}
#endif
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Write dip/sip entry to cmdproc for a given sid & index.
 *
 * Expect index as raw pt index of single-wide view.
 *
 * \param [in] u Device u.
 * \param [in] sid DRD symbol id
 * \param [in] index raw pt index of single_wide view
 * \param [in] de dip entry
 * \param [in] se sip entry
 * \param [in] pfx Translated prefix for target entry.
 *
 * \return SHR_E_XXX.
 */
static int
write_l1_raw(int u, int m, bcmdrd_sid_t sid, int index,
             uint32_t *de, uint32_t *se,
             uint8_t hit_op, uint32_t modify_bitmap,
             uint32_t dt_bitmap)
{
    int hw_idx, i;
    bcmptm_rm_op_t op_type;
    static uint32_t hit_entry_clear[1] = {0};
    static uint32_t hit_entry_set[1] = {0x1};
    uint32_t *hit_entry;
    bcmdrd_sid_t hw_sid;
    int phy_blk_index; /* block converted index */
    uint8_t wide;

    SHR_FUNC_ENTER(u);
#ifdef ALPM_DEBUG
    LOG_D((" Write[%d] sid %d index %d [",
          bcmptm_rm_alpm_internals[u][m][DB0][LEVEL1].write, sid, index));
    for (i = 0; i < bcmdrd_pt_entry_wsize(u, sid); i++) {
        LOG_D((" %08x", de[i]));
    }
    LOG_D(("]\n"));
#endif

    if (is_pair_sid(u, m, sid)) {
        op_type = L1_INFO_FD(u, m, slice_mode_en) ? BCMPTM_RM_OP_EM_WIDE
                                               : BCMPTM_RM_OP_NORMAL;
        SANITY_CHECK_PAIR_ENTRY(u, m, de, se, index_to_pkm(u, m, index, NULL));
    } else {
        op_type = L1_INFO_FD(u, m, slice_mode_en) ? BCMPTM_RM_OP_EM_NARROW
                                               : BCMPTM_RM_OP_NORMAL;
        if (op_type == BCMPTM_RM_OP_EM_NARROW) {
            const alpm_config_t * config = bcmptm_rm_alpm_config(u, m);
            /* Do not trigger entry mode change if already in narrow slice */
            if (config->core.l1_key_input[tcam_num(u, m, index) >> 1] != ALPM_KEY_INPUT_LPM_DST_Q) {
                op_type = BCMPTM_RM_OP_NORMAL;
            }
        }
        SANITY_CHECK_UNPAIR_ENTRY(u, m, de, se, index_to_pkm(u, m, index, NULL));
    }

    phy_blk_index = phy_block_convert_index(u, m, index, false);
    if (L1_TCAMS_SHARED(u, m)) {
        if (is_pair_sid(u, m, sid)) {
            wide = WIDE_DOUBLE;
        } else {
            wide = WIDE_SINGLE; /* No need to distinguish SINGLE and HALF.*/
        }
        /* Use blk_conv_index for calculating hw_idx. */
        SHR_IF_ERR_EXIT(
            multi_depth_combined_sid_index_map(u, m, wide, phy_blk_index,
                                               &hw_sid, &hw_idx));
    } else {
        /* index is block converted inside the function.  */
        hw_idx = l1_hw_index_adjust_base(u, m, sid, index);
        hw_sid = sid;
        ALPM_ASSERT(hw_idx >= 0);
    }

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write(u, m, hw_sid, hw_idx, de, ALPM_L1_ENT_MAX, op_type,
                             dt_bitmap));
    bcmptm_rm_alpm_internals[u][m][DB0][LEVEL1].write++;
#ifdef ALPM_DEBUG
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        L1_ENTRY_t de2, se2;

        SHR_IF_ERR_EXIT(read_l1_raw(u, m, sid, index, de2, se2, NULL));
        for (i = 0; i < bcmdrd_pt_entry_wsize(u, sid); i++) {
            assert(de[i] == de2[i]);
        }
    }
#endif

    if (bcmptm_rm_alpm_hit_version(u, m) == ALPM_HIT_VERSION_1 &&
        hit_op == HIT_OP_AUTO) {
        uint8_t hit_support = bcmptm_rm_alpm_hit_support(u, m);
        if (hit_support != HIT_MODE_DISABLE) {
            hit_entry = hit_support == HIT_MODE_FORCE_SET ?
                        hit_entry_set : hit_entry_clear;
            ALPM_ASSERT(modify_bitmap < (1 << MAX_L1_HIT_BITS));
            for (i = 0; i < MAX_L1_HIT_BITS && modify_bitmap; i++) {
                if (ALPM_BIT_GET(modify_bitmap, i)) {
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_alpm_write(u, m, L1_HIT_SID(u, m),
                                              (HW_INDEX(phy_blk_index) << 1) | i,
                                              hit_entry,
                                              1, BCMPTM_RM_OP_NORMAL,
                                              L1_HIT_DT_BITMAP));
                    bcmptm_rm_alpm_internals[u][m][DB0][LEVEL1].write++;
                    ALPM_BIT_CLR(modify_bitmap, i);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Clear dip/sip entry to cmdproc for a given sid & index.
 *
 * Expect index as raw pt index of single-wide view.
 *
 * \param [in] u Device u.
 * \param [in] sid DRD symbol id
 * \param [in] index raw pt index of single_wide view
 *
 * \return SHR_E_XXX.
 */
static int
clear_l1_raw(int u, int m, bcmdrd_sid_t sid, int index)
{
    static L1_ENTRY_t de = {0};
    static L1_ENTRY_t se = {0};
    return write_l1_raw(u, m, sid, index, &de[0], &se[0],
                        HIT_OP_NONE, 0, 0);
}


static int
read_l1_raw2(int u, int m, alpm_sid_type_t sid_type, int index, uint8_t wide,
             uint32_t *de, uint32_t *dt_bitmap)
{
    bool is_half_data;
    alpm_sid_index_map_t map_info;

    SHR_FUNC_ENTER(u);
    is_half_data = sid_type == SID_TYPE_DATA &&
                   (wide == WIDE_HALF || alpm_l1_info[u][m]->data_only_x2);
    map_info.index = phy_block_convert_index(u, m, index, is_half_data);
    map_info.sid_type = sid_type;
    map_info.wide = wide;

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_l1_sid_index_map(u, m, &map_info));

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read(u, m, map_info.sid, map_info.sid_index,
                            ALPM_L1_ENT_MAX, de, dt_bitmap));
    bcmptm_rm_alpm_internals[u][m][DB0][LEVEL1].read++;

#ifdef ALPM_DEBUG
{
    int rv;
    LOG_D((" Read [sid %d index %d [", map_info.sid, map_info.sid_index));
    for (rv = 0; rv < bcmdrd_pt_entry_wsize(u, map_info.sid); rv++) {
        LOG_D((" %08x", de[rv]));
    }
    LOG_D(("]\n"));
}
#endif
exit:
    SHR_FUNC_EXIT();
}

static int
write_l1_raw2(int u, int m, alpm_sid_type_t sid_type, int index, uint8_t wide,
              uint32_t *de, uint32_t dt_bitmap)
{
    bool is_half_data;
    alpm_sid_index_map_t map_info;

    SHR_FUNC_ENTER(u);
    is_half_data = sid_type == SID_TYPE_DATA &&
                   (wide == WIDE_HALF || alpm_l1_info[u][m]->data_only_x2);
    map_info.index = phy_block_convert_index(u, m, index, is_half_data);
    map_info.sid_type = sid_type;
    map_info.wide = wide;

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_l1_sid_index_map(u, m, &map_info));

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write(u, m, map_info.sid, map_info.sid_index, de,
                             ALPM_L1_ENT_MAX, BCMPTM_RM_OP_NORMAL,
                             dt_bitmap));
    bcmptm_rm_alpm_internals[u][m][DB0][LEVEL1].write++;


#ifdef ALPM_DEBUG
{
    int i;
    LOG_D((" Write[%d] sid %d index %d [",
           bcmptm_rm_alpm_internals[u][m][DB0][LEVEL1].write,
           map_info.sid, map_info.sid_index));
    for (i = 0; i < bcmdrd_pt_entry_wsize(u, map_info.sid); i++) {
        LOG_D((" %08x", de[i]));
    }
    LOG_D(("]\n"));
}
#endif
exit:
    SHR_FUNC_EXIT();
}

static int
clear_l1_raw2(int u, int m, alpm_sid_type_t sid_type, int index, uint8_t wide)
{
    static L1_ENTRY_t de = {0};
    return write_l1_raw2(u, m, sid_type, index, wide, &de[0], 0);
}

static void
l1_info_cleanup(int u, int m)
{
    SHR_FREE(PVT_INFO_ARRAY(u, m));
    SHR_FREE(L1_INFO(u, m));
}


/*!
 * \brief Initiliaze ALPM level-1 info
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 *
 * \return SHR_E_XXX.
 */
static int
l1_info_init(int u, int m, alpm_dev_info_t *dev_info, bool recover)
{
    int sz;
    int i, ldb;
    alpm_info_t *alpm_info;
    int block_start;
    uint8_t pkm;
    l1_db_info_t *db;
    l1_key_input_info_t *l1_ki;
    SHR_FUNC_ENTER(u);

    sz = sizeof(alpm_l1_info_t);
    ALPM_ALLOC(L1_INFO(u, m), sz, "bcmptmRmalpmL1info");
    sal_memset(L1_INFO(u, m), 0, sz);

    alpm_info = bcmptm_rm_alpm_info_get(u, m);
    SHR_NULL_CHECK(alpm_info, SHR_E_INIT);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    MAX_TCAMS(u, m) = dev_info->tcam_blocks;
    TCAM_DEPTH(u, m) = dev_info->tcam_depth;
    PAIR_SID(u, m) = dev_info->pair_sid;
    UNPAIR_SID(u, m) = dev_info->unpair_sid;
    TCAM_ONLY_SID(u, m) = dev_info->tcam_only_sid;
    DATA_ONLY_SID(u, m) = dev_info->data_only_sid;
    WIDE_PAIR_SID(u, m) = dev_info->wide_pair_sid;
    WIDE_UNPAIR_SID(u, m) = dev_info->wide_unpair_sid;
    WIDE_TCAM_ONLY_SID(u, m) = dev_info->wide_tcam_only_sid;
    WIDE_DATA_ONLY_SID(u, m) = dev_info->wide_data_only_sid;
    L1_HIT_SID(u, m) = dev_info->l1_hit_sid;
    L1_BLOCKS(u, m) = dev_info->l1_blocks;
    L1_TCAMS_SHARED(u, m) = dev_info->l1_tcams_shared;
    KEY_DATA_SEPARATE(u, m, FALSE) = dev_info->key_data_separate[0];
    KEY_DATA_SEPARATE(u, m, TRUE) = dev_info->key_data_separate[1];
    alpm_l1_info[u][m]->data_only_x2 = dev_info->l1_data_only_x2;

    if (L1_TCAMS_SHARED(u, m)) {
        uint8_t tcam_banks = alpm_info->config.core.tot.num_l1_banks;
        if (m == ALPM_0) {
            MAX_TCAMS(u, m) = tcam_banks >> 1;
            L1_BLOCKS(u, m) = MAX_TCAMS(u, m) >> 1;
        } else {
            MAX_TCAMS(u, m) = tcam_banks;
            L1_BLOCKS(u, m) = MAX_TCAMS(u, m) >> 1;
        }
        /* L1_BLOCK_START always start from 0 */
    }
    L1_BLOCK_END(u, m) = L1_BLOCK_START(u, m) + L1_BLOCKS(u, m);
    assert(MAX_TCAMS(u, m) / L1_BLOCKS(u, m) == 2);

    L1_INFO_FDM(u, m, w_template, VRF_OVERRIDE) = dev_info->wo_template;
    L1_INFO_FDM(u, m, w_template, VRF_PRIVATE) = dev_info->wp_template;
    L1_INFO_FDM(u, m, w_template, VRF_GLOBAL) = dev_info->wg_template;
    L1_INFO_FDM(u, m, n_template, VRF_OVERRIDE) = dev_info->no_template;
    L1_INFO_FDM(u, m, n_template, VRF_PRIVATE) = dev_info->np_template;
    L1_INFO_FDM(u, m, n_template, VRF_GLOBAL) = dev_info->ng_template;
    L1_INFO_FD(u, m, n_size) = dev_info->n_size;
    L1_INFO_FD(u, m, w_size) = dev_info->w_size;
    L1_INFO_FD(u, m, slice_mode_en) = (dev_info->alpm_ver == ALPM_VERSION_0);

    NO_PKM_S(u, m) = (dev_info->feature_pkms == 2);
    for (ldb = L1_DB0; ldb < L1_DBS; ldb++) {
        db = &L1_DB(u, m, ldb);
        for (pkm = 0; pkm < PKM_NUM; pkm++) {
            db->ki[pkm].block_start = -1;
            db->ki[pkm].block_end = -1;
        }
    }

    block_start = L1_BLOCK_START(u, m) * (TCAM_DEPTH(u, m) * (MAX_TCAMS(u, m) / L1_BLOCKS(u, m)));
    for (i = L1_BLOCK_START(u, m); i < L1_BLOCK_END(u, m); i++) {
        ldb = alpm_info->config.core.l1_db[i];
        ALPM_ASSERT(ldb < L1_DBS);
        db = &L1_DB(u, m, ldb);
        db->valid = 1;

        switch (alpm_info->config.core.l1_key_input[i]) {
        case ALPM_KEY_INPUT_LPM_DST_Q: pkm = PKM_Q; break;
        case ALPM_KEY_INPUT_LPM_DST_D: pkm = PKM_D; break;
        case ALPM_KEY_INPUT_LPM_DST_S: pkm = PKM_S; break;
        case ALPM_KEY_INPUT_LPM_L3MC_Q: pkm = PKM_WQ; break;
        case ALPM_KEY_INPUT_LPM_L3MC_D: pkm = PKM_WD; break;
        case ALPM_KEY_INPUT_LPM_L3MC_S: pkm = PKM_WS; break;
        case ALPM_KEY_INPUT_LPM_SRC_Q: pkm = PKM_SQ; break;
        case ALPM_KEY_INPUT_LPM_SRC_D: pkm = PKM_SD; break;
        case ALPM_KEY_INPUT_LPM_SRC_S: pkm = PKM_SS; break;
        case ALPM_KEY_INPUT_FP_COMP_DST: pkm = PKM_FP; break;
        case ALPM_KEY_INPUT_FP_COMP_SRC: pkm = PKM_SFP; break;
        default: SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        if (pkm == PKM_Q) {
            alpm_info->config.ver0_128_enable = true;
        }
        if (db->ki[pkm].block_start == -1) {
            db->ki[pkm].block_start = block_start;
        }
        db->ki[pkm].blocks++;
        db->ki[pkm].valid = 1;

        block_start += (TCAM_DEPTH(u, m) * (MAX_TCAMS(u, m) / L1_BLOCKS(u, m)));
    }

    DUAL_MODE(u, m) = bcmptm_rm_alpm_is_parallel(u, m);

    /* Initialize pivot info. sz: half entry based */
    for (ldb = L1_DB0; ldb < L1_DBS; ldb++) {
        db = &L1_DB(u, m, ldb);
        if (!db->valid) {
            continue;
        }
        for (pkm = 0; pkm < PKM_NUM; pkm++) {
            l1_ki = &db->ki[pkm];
            if (!l1_ki->valid) {
                l1_ki->block_start = -1;
                l1_ki->block_end = -1;
                continue;
            }
            l1_ki->block_size = l1_ki->blocks * TCAM_DEPTH(u, m) *
                                    (MAX_TCAMS(u, m) / L1_BLOCKS(u, m));

            l1_ki->block_end = l1_ki->block_start + l1_ki->block_size - 1;
        }
    }
    sz = (block_start << 1) * sizeof(alpm_pivot_t *);
    ALPM_ALLOC(PVT_INFO_ARRAY(u, m), sz, "bcmptmRmalpmL1pvts");
    sal_memset(PVT_INFO_ARRAY(u, m), 0, sz);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initiliaze ALPM level-1 HA info
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 * \param [in] boot_type Boot type.
 *
 * \return SHR_E_XXX.
 */
static int
l1_ha_init(int u, int m, bool recover, uint8_t boot_type)
{
    shr_ha_sub_id sub_id;
    uint32_t req_size = sizeof(bcmptm_rm_alpm_l1_ha_t);
    uint32_t alloc_size = sizeof(bcmptm_rm_alpm_l1_ha_t);
    int pkm;

    SHR_FUNC_ENTER(u);

    sub_id = m ? BCMPTM_HA_SUBID_M_RM_ALPM_L1 : BCMPTM_HA_SUBID_RM_ALPM_L1;
    L1_HA(u, m) = shr_ha_mem_alloc(u,
                                BCMMGMT_RM_ALPM_COMP_ID,
                                sub_id,
                                "bcmptmRmAlpmLevel1Ha",
                                &alloc_size);
    SHR_NULL_CHECK(L1_HA(u, m), SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (recover) {
        ALPM_ASSERT(L1_HA(u, m));

    } else {
        for (pkm = 0; pkm < PKM_NUM; pkm++) {
            L1_HA_RANGE1_FIRST_DB0(u, m, pkm) = RANGE1_FIRST(u, m, L1_DB0, pkm);
            L1_HA_RANGE1_FIRST_DB1(u, m, pkm) = RANGE1_FIRST(u, m, L1_DB1, pkm);
            L1_HA_RANGE1_FIRST_DB2(u, m, pkm) = RANGE1_FIRST(u, m, L1_DB2, pkm);
            L1_HA_RANGE1_FIRST_DB3(u, m, pkm) = RANGE1_FIRST(u, m, L1_DB3, pkm);
        }

        if (boot_type == ALPM_WARM_BOOT || boot_type == ALPM_COLD_BOOT) {
            SHR_IF_ERR_EXIT(
                bcmissu_struct_info_report(u, BCMMGMT_RM_ALPM_COMP_ID,
                                           sub_id,
                                           0, /* offset */
                                           req_size, 1,
                                           BCMPTM_RM_ALPM_L1_HA_T_ID));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
l1_ha_cleanup(int u, int m, bool graceful)
{
    if (graceful) {
    }
}

/*!
 * \brief Register slice mode for level-1 tcam.
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 *
 * \return SHR_E_XXX.
 */
static int
l1_ser_slice_register(int u, int m, bool recover)
{
    int *slice_depth = NULL;
    bcmdrd_sid_t sid_list[] = {UNPAIR_SID(u, m), PAIR_SID(u, m)};
    bcmptm_rm_slice_change_t sc_info;
    alpm_info_t *alpm_info;
    uint8_t slice;
    uint32_t l1_slice_group_id;
    SHR_FUNC_ENTER(u);

    alpm_info = bcmptm_rm_alpm_info_get(u, m);
    SHR_NULL_CHECK(alpm_info, SHR_E_INIT);

    if (recover) {
        SHR_EXIT();
    }
    if (m == ALPM_1) {
        SHR_EXIT();
    }

    ALPM_ALLOC(slice_depth, (MAX_TCAMS(u, m) * sizeof(int)),
               "bcmptmRmalpmL1depths");
    for (slice = 0; slice < MAX_TCAMS(u, m); slice++) {
        slice_depth[slice] = TCAM_DEPTH(u, m);
    }

    SHR_IF_ERR_EXIT(
        bcmptm_ser_slice_group_register(u,
                                        slice_depth,
                                        MAX_TCAMS(u, m),
                                        sid_list,
                                        COUNTOF(sid_list),
                                        BCMPTM_TCAM_SLICE_DISABLED,
                                        true, /* overlay tcam */
                                        &l1_slice_group_id));

    sc_info.slice_group_id = l1_slice_group_id;
    for (slice = 0; slice < MAX_TCAMS(u, m); slice += 2) {
        sc_info.slice_num0 = slice;
        sc_info.slice_num1 = slice + 1;
        if (alpm_info->config.core.l1_key_input[slice >> 1] == ALPM_KEY_INPUT_LPM_DST_Q) {
            sc_info.slice_mode = BCMPTM_TCAM_SLICE_WIDE;
        } else {
            sc_info.slice_mode = BCMPTM_TCAM_SLICE_NARROW;
        }
        SHR_IF_ERR_EXIT(
            bcmptm_ser_slice_mode_change(u, &sc_info));
    }

exit:
    if (slice_depth) {
        SHR_FREE(slice_depth);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get start index of first half-entry for L1 blocks referenced by base_index
 *
 * Follow the next index from longest MAX_PFX
 * Half-entry is specified by half_entry_key_type (APP_LPM).
 *
 * \param [in] u Device u.
 * \param [in] index Base index
 *
 * \return start_index if exist, otherwise SHR_E_XXX.
 */
static int
l1_half_entry_start_index(int u, int m, int ldb, int pkm)
{
    int half_start = INVALID_INDEX;
    int v4_pfx = INVALID_PFX, v6_pfx;

    if (pkm == PKM_S || pkm == PKM_SS) {
        half_start = KI_START(u, m, ldb, pkm);
    } else if (bcmptm_rm_alpm_pkm_is_pair(u, m, pkm)) {
        pair_largest_lpm_prefix(u, m, ldb, pkm, &v4_pfx, &v6_pfx);
    } else {
        unpair_largest_lpm_prefix(u, m, ldb, pkm, &v4_pfx, &v6_pfx);
    }
    if (v4_pfx != INVALID_PFX) {
        half_start = UNPAIR_STATE_START(u, m, ldb, v4_pfx, pkm);
    }

    return half_start;
}

/*!
 * \brief Register ser call for level-1 tcam.
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 *
 * \return SHR_E_XXX.
 */
static int
alpm_ser_sid_index_map_cb3(int u, int m, bcmdrd_sid_t hw_sid,
                           bcmbd_pt_dyn_info_t hw_dyn_info,
                           int in_count,
                           bcmdrd_sid_t *out_sid,
                           bcmbd_pt_dyn_info_t *out_dyn_info,
                           int *out_count)
{
    alpm_sid_index_out_t out_info;
    int half_start, half_entry;
    int pkm, ldb;
    int flag = 0;
    int data_covered_by_pair = 0;
    int phy_blk_normal_index;
    int rv;
    int log_blk_base_index;
    int log_blk_normal_index;

    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(alpm_l1_info[u][m], SHR_E_INIT);
    SHR_NULL_CHECK(out_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(out_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(out_count, SHR_E_PARAM);
    if (in_count == 0) {
        *out_count = 0;
        SHR_EXIT();
    }
    *out_count = 1;

    rv = bcmptm_rm_alpm_l1_sid_index_reverse_map(u, m, hw_sid, &out_info);
    if (rv == SHR_E_NOT_FOUND) {
        *out_count = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    out_dyn_info[0] = hw_dyn_info;

    phy_blk_normal_index = out_info.base_index + hw_dyn_info.index;

    log_blk_normal_index = log_block_convert_index(u, m, phy_blk_normal_index);
    log_blk_base_index = log_block_convert_index(u, m, out_info.base_index);
    pkm = index_to_pkm(u, m, log_blk_base_index, &ldb);

    /* find possible half_entry start index */
    half_start = l1_half_entry_start_index(u, m, ldb, pkm);
    half_entry = FALSE;
    if (is_data_only_sid(u, m, out_info.base_sid)) {
        if (half_start != INVALID_INDEX) {
            /*
             * There are three possible cases:
             *  1) base > half_start: it's HALF_ENTRY
             *  2) base <= half_start <= base + depth:
             *     if (hw_index >= (half_start - base) * 2) then it's HALF_ENTRY.
             *     else NO_HALF_ENTRY.
             *  3) half > base + depth: NO_HALF_ENTRY.
             */
            if (bcmptm_rm_alpm_pkm_is_pair(u, m, pkm) &&
                index_in_right_tcam(u, m, log_blk_base_index)) {
                half_start += TCAM_DEPTH(u, m);
            }
            if (log_blk_base_index > half_start) {
                half_entry = TRUE;
            } else if (half_start <= (log_blk_base_index + out_info.bank_depth)) {
                if (hw_dyn_info.index >= ((half_start - log_blk_base_index) * 2)) {
                    half_entry = TRUE;
                } else {
                    if (hw_dyn_info.index < (half_start - log_blk_base_index)) {
                        flag = 1;
                    }
                }
            }
        }
        if (half_entry == TRUE) {
            log_blk_normal_index = log_blk_base_index + (hw_dyn_info.index / 2);
        }
        if (bcmptm_rm_alpm_pkm_is_pair(u, m, pkm) &&
            index_in_right_tcam(u, m, log_blk_normal_index) &&
            !half_entry && flag) {
            data_covered_by_pair = TRUE;
        }
        if (data_covered_by_pair) {
            SHR_IF_ERR_EXIT(
                multi_depth_combined_sid_index_map(u, m, WIDE_DOUBLE, phy_blk_normal_index,
                                                   out_sid,
                                                   &out_dyn_info->index));
        } else {
            out_sid[0] = hw_sid;
        }
    } else if (is_tcam_only_sid(u, m, out_info.base_sid)) {
        if (index_in_pair_tcam(u, m, log_blk_normal_index)) {
            SHR_IF_ERR_EXIT(
                multi_depth_combined_sid_index_map(u, m, WIDE_DOUBLE, phy_blk_normal_index,
                                                   out_sid,
                                                   &out_dyn_info->index));
        } else {
            out_sid[0] = hw_sid;
        }
    } else if (is_pair_sid(u, m, out_info.base_sid)) {
        out_sid[0] = hw_sid;
    } else if (is_unpair_sid(u, m, out_info.base_sid)) {
        if (index_in_pair_tcam(u, m, log_blk_normal_index)) {
            SHR_IF_ERR_EXIT(
                multi_depth_combined_sid_index_map(u, m, WIDE_DOUBLE, phy_blk_normal_index,
                                                   out_sid,
                                                   &out_dyn_info->index));
            if (!index_in_right_tcam(u, m, log_blk_normal_index)) {
                alpm_sid_index_map_t map_info = {0};
                if (in_count < 2) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                if (half_start != INVALID_INDEX) {
                    if (log_blk_normal_index >= half_start) {
                        half_entry = TRUE;
                    }
                }
                map_info.index = phy_blk_normal_index;
                map_info.sid_type = SID_TYPE_DATA;
                map_info.wide = half_entry ? WIDE_HALF : WIDE_DOUBLE;
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_l1_sid_index_map(u, m, &map_info));
                out_sid[1] = map_info.sid;
                out_dyn_info[1] = hw_dyn_info;
                out_dyn_info[1].index = map_info.sid_index;

                *out_count = 2;
            }
        } else {
            out_sid[0] = hw_sid;
        }
    } else {
        *out_count = 0;
    }
exit:
    SHR_FUNC_EXIT();
}


static int
alpm_ser_sid_index_map_cb2(int u, bcmdrd_sid_t hw_sid,
                           bcmbd_pt_dyn_info_t hw_dyn_info,
                           int in_count,
                           bcmdrd_sid_t *out_sid,
                           bcmbd_pt_dyn_info_t *out_dyn_info,
                           int *out_count)
{
    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(out_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(out_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(out_count, SHR_E_PARAM);
    if (in_count == 0) {
        *out_count = 0;
        SHR_EXIT();
    }
    *out_count = 1;
    out_sid[0] = hw_sid;
    out_dyn_info[0] = hw_dyn_info;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register ser call for level-1 tcam.
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 *
 * \return SHR_E_XXX.
 */
static int
alpm_ser_sid_index_map_cb(int u, int m, bcmdrd_sid_t hw_sid,
                          bcmbd_pt_dyn_info_t hw_dyn_info,
                          int in_count,
                          bcmdrd_sid_t *out_sid,
                          bcmbd_pt_dyn_info_t *out_dyn_info,
                          int *out_count)
{
    int pkm, ldb;
    int log_blk_hw_index;
    int hw_idx;
    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(alpm_l1_info[u][m], SHR_E_INIT);
    SHR_NULL_CHECK(out_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(out_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(out_count, SHR_E_PARAM);
    if (in_count == 0) {
        *out_count = 0;
        SHR_EXIT();
    }

    if (!is_data_only_sid(u, m, hw_sid) &&
        !is_tcam_only_sid(u, m, hw_sid) &&
        !is_pair_sid(u, m, hw_sid) &&
        !is_unpair_sid(u, m, hw_sid)) {
        *out_count = 0;
        SHR_EXIT();
    }
    *out_count = 1;

    hw_idx = hw_dyn_info.index;
    if (!is_wide_sid(u, m, hw_sid)) {
        hw_idx += l1_wide_index_size(u, m);
    }
    /* no multidepth sid case */
    log_blk_hw_index = log_block_convert_index(u, m, hw_idx);
    pkm = index_to_pkm(u, m, log_blk_hw_index, &ldb);
    out_dyn_info[0] = hw_dyn_info;
    if (is_data_only_sid(u, m, hw_sid)) {

        if (index_in_pair_tcam(u, m, log_blk_hw_index) &&
            pair_index_is_in_range1(u, m, ldb, log_blk_hw_index, pkm)) {
            if (index_in_right_tcam(u, m, log_blk_hw_index)) {
                out_sid[0] = hw_sid; /* DATA_ONLY to clear */
            } else {
                out_sid[0] = pair_sid2(u, m, pkm);
                out_dyn_info[0].index =
                    l1_hw_index_adjust_base(u, m, out_sid[0], log_blk_hw_index);
            }
        } else {
            out_sid[0] = unpair_sid2(u, m, pkm);
        }
    } else if (is_tcam_only_sid(u, m, hw_sid)) {
        if (index_in_pair_tcam(u, m, log_blk_hw_index) &&
            pair_index_is_in_range1(u, m, ldb, log_blk_hw_index, pkm)) {
            out_sid[0] = pair_sid2(u, m, pkm);
            out_dyn_info[0].index =
                l1_hw_index_adjust_base(u, m, out_sid[0], log_blk_hw_index);

        } else {
            out_sid[0] = unpair_sid2(u, m, pkm);
        }
    } else if (is_pair_sid(u, m, hw_sid)) {
        out_sid[0] = hw_sid;
    } else if (is_unpair_sid(u, m, hw_sid)) {
        if (index_in_pair_tcam(u, m, log_blk_hw_index) &&
            pair_index_is_in_range1(u, m, ldb, log_blk_hw_index, pkm)) {
            out_sid[0] = pair_sid2(u, m, pkm);
            out_dyn_info[0].index =
                l1_hw_index_adjust_base(u, m, out_sid[0], log_blk_hw_index);
            if (index_in_right_tcam(u, m, log_blk_hw_index)) {
                if (in_count < 2) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                out_sid[1] = DATA_ONLY_SID(u, m);
                out_dyn_info[1] = hw_dyn_info;
                *out_count = 2;
            }
        } else {
            out_sid[0] = hw_sid;
        }
    } else {
        *out_count = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
alpm_ser_sid_index_map_cb3_m0(int u, bcmdrd_sid_t hw_sid,
                              bcmbd_pt_dyn_info_t hw_dyn_info,
                              int in_count,
                              bcmdrd_sid_t *out_sid,
                              bcmbd_pt_dyn_info_t *out_dyn_info,
                              int *out_count)
{
    return alpm_ser_sid_index_map_cb3(u, ALPM_0, hw_sid, hw_dyn_info, in_count,
                                      out_sid, out_dyn_info, out_count);
}

static int
alpm_ser_sid_index_map_cb3_m1(int u, bcmdrd_sid_t hw_sid,
                              bcmbd_pt_dyn_info_t hw_dyn_info,
                              int in_count,
                              bcmdrd_sid_t *out_sid,
                              bcmbd_pt_dyn_info_t *out_dyn_info,
                              int *out_count)
{
    return alpm_ser_sid_index_map_cb3(u, ALPM_1, hw_sid, hw_dyn_info, in_count,
                                      out_sid, out_dyn_info, out_count);
}

static int
alpm_ser_sid_index_map_cb_m0(int u, bcmdrd_sid_t hw_sid,
                             bcmbd_pt_dyn_info_t hw_dyn_info,
                             int in_count,
                             bcmdrd_sid_t *out_sid,
                             bcmbd_pt_dyn_info_t *out_dyn_info,
                             int *out_count)
{
    return alpm_ser_sid_index_map_cb(u, ALPM_0, hw_sid, hw_dyn_info, in_count,
                                     out_sid, out_dyn_info, out_count);
}

static int
alpm_ser_sid_index_map_cb_m1(int u, bcmdrd_sid_t hw_sid,
                             bcmbd_pt_dyn_info_t hw_dyn_info,
                             int in_count,
                             bcmdrd_sid_t *out_sid,
                             bcmbd_pt_dyn_info_t *out_dyn_info,
                             int *out_count)
{
    return alpm_ser_sid_index_map_cb(u, ALPM_1, hw_sid, hw_dyn_info, in_count,
                                     out_sid, out_dyn_info, out_count);
}

/*!
 * \brief Check if priorities will continue to be qualified after entry inserted
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return True: ok to go. False: check fail.
 */
static bool
priorities_check(int u, int m, alpm_arg_t *arg)
{
    /*
     * No support for routes matching AFTER vrf specific.
     * By not allowing VRF_GLOBAL for 128-V6 routes,
     * we make sure that 64-V6 private entries dont get
     * priotized over 128-V6 public/global entries.
     */

#if 0
    if ((VRF_GLOBAL == arg->key.vt) && (arg->key.t.len > 64)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if ((VRF_OVERRIDE == arg->key.vt) && (arg->key.t.len <= 64)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
#endif
    return true;
}

/*!
 * \brief Get the min and max pfxs for IPv4
 *
 * \param [in] u Device u.
 * \param [in] pkm Packing mode.
 * \param [in] vt VRF type
 * \param [out] pfx_max Max IPv4 pfx
 * \param [out] min_pfx Min IPv4 pfx
 *
 * \return nothing
 */
static void
lpm_ipv4_bound_pfx_get(int u, int m, uint8_t ldb, int pkm, int vt,
                       int *max_pfx, int *min_pfx)
{
    int pfx;
    int max_v4_pfx = INVALID_PFX;
    int min_v4_pfx = INVALID_PFX;

    if (bcmptm_rm_alpm_pkm_is_pair(u, m, pkm)) {
        pfx = MAX_PFX(u, m, ldb, LAYOUT_SH, pkm);
        pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);

        while (pfx != INVALID_PFX) {
            if (pfx_is_kt(u, m, pfx, KEY_IPV4)) {
                if (max_v4_pfx == INVALID_PFX) {
                    max_v4_pfx = pfx;
                }
                min_v4_pfx = pfx;
            }
            pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
        }
    } else {
        pfx = UNPAIR_MAX_PFX(u, m, ldb, pkm);
        pfx = UNPAIR_STATE_NEXT(u, m, ldb, pfx, pkm);

        while (pfx != INVALID_PFX) {
            if (pfx_is_kt(u, m, pfx, KEY_IPV4)) {
                if (max_v4_pfx == INVALID_PFX) {
                    max_v4_pfx = pfx;
                }
                min_v4_pfx = pfx;
            }
            pfx = UNPAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
        }
    }

    if (max_pfx) {
        *max_pfx = max_v4_pfx;
    }
    if (min_pfx) {
        *min_pfx = min_v4_pfx;
    }
}

/*!
 * \brief Get the min and max pfxs for IPv6 strict type.
 *
 * \param [in] u Device u.
 * \param [in] pkm Packing mode.
 * \param [in] kt KEY type (strict)
 * \param [in] vt VRF type
 * \param [out] pfx_max Max IPv4 pfx
 * \param [out] min_pfx Min IPv4 pfx
 *
 * \return nothing
 */
static void
lpm_ipv6_bound_pfx_get(int u, int m,
                       uint8_t ldb,
                       int pkm,
                       alpm_key_type_t kt,
                       alpm_vrf_type_t vt,
                       int *max_pfx,
                       int *min_pfx)
{
    int pfx;
    int max_v6_pfx = INVALID_PFX;
    int min_v6_pfx = INVALID_PFX;

    if (bcmptm_rm_alpm_pkm_is_pair(u, m, pkm)) {
        pfx = MAX_PFX(u, m, ldb, LAYOUT_D, pkm);
        pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);

        while (pfx != INVALID_PFX) {
            if (pfx_is_kt(u, m, pfx, kt)) {
                if (max_v6_pfx == INVALID_PFX) {
                    max_v6_pfx = pfx;
                }
                min_v6_pfx = pfx;
            }
            pfx = PAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
        }

    } else {
        pfx = UNPAIR_MAX_PFX(u, m, ldb, pkm);
        pfx = UNPAIR_STATE_NEXT(u, m, ldb, pfx, pkm);

        while (pfx != INVALID_PFX) {
            if (pfx_is_kt(u, m, pfx, kt)) {
                if (max_v6_pfx == INVALID_PFX) {
                    max_v6_pfx = pfx;
                }
                min_v6_pfx = pfx;
            }
            pfx = UNPAIR_STATE_NEXT(u, m, ldb, pfx, pkm);
        }
    }

    if (max_pfx) {
        *max_pfx = max_v6_pfx;
    }
    if (min_pfx) {
        *min_pfx = min_v6_pfx;
    }
}

/*!
 * \brief Get the single-wide entry number with the given prefix and packing mode.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop.
 * \param [in] pfx ALPM prefix.
 * \param [in] pkm Packing mode.
 *
 * \return singile-wide entry count.
 */
static int
ripple_entry_num(int u, int m, int pfx, int pkm)
{
    int weight;
    alpm_key_type_t kt;

    kt = pfx_to_key_type(u, m, pfx);

    weight = alpm_pkm_kt_weight[pkm][kt];

    return ((weight == 1) ? 1 : (weight / 2));
}

/*!
 * \brief Retry insert after rippling entries from unpair64 to other blocks
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
ripple_insert(int u, int m, uint8_t ldb, int pfx, int dir, int pkm)
{
    int move_pfx;
    uint8_t rem_cnt, moved_cnt = 0, count_need;
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    static const int pkms1[] = {PKM_S, PKM_D, PKM_Q, PKM_WD, PKM_WQ};
    static const int pkms2[] = {PKM_SS, PKM_SD, PKM_SQ};
    const int *pkms;
    int pkms_cnt;
    int target_i, i;
    int rv;
    int rem_entry;

    SHR_FUNC_ENTER(u);

    if (bcmptm_rm_alpm_db_is_src(u, m, ldb)) {
        pkms = pkms2;
        pkms_cnt = COUNTOF(pkms2);
    } else {
        pkms = pkms1;
        pkms_cnt = COUNTOF(pkms1);
    }
    for (target_i = 0; target_i < pkms_cnt; target_i++) {
        if (pkms[target_i] == pkm) {
            break;
        }
    }

    /* Least number of single-wide entries needed for one entry with the given pfx*/
    rem_entry = ripple_entry_num(u, m, pfx, pkm);

    if (dir & DIR_DOWN) {
        for (i = target_i - 1; i >= 0; i--) {
            if (can_pkm_move_down(u, m, ldb, pfx, &move_pfx, pkm, pkms[i])) {
                break;
            }
        }
        if (i >= 0) {
            do {
                kt = pfx_to_key_type(u, m, move_pfx);
                vt = pfx_to_vrf_type(u, m, move_pfx);
                count_need = half_entry_key_type(u, m, kt, pkm) ? 2 : 1;

                if (pkm_free_count(u, m, ldb, kt, vt, pkms[i]) >= count_need) {
                    SHR_IF_ERR_EXIT(
                        ripple_move(u, m, ldb, move_pfx, &rem_cnt,
                                    &moved_cnt, pkm, pkms[i]));

                    rem_entry -= ripple_entry_num(u, m, move_pfx, pkm);
                } else {
                    rv = ripple_insert(u, m, ldb, move_pfx, DIR_DOWN, pkms[i]);
                    if (SHR_SUCCESS(rv)) {
                        ALPM_ASSERT(pkm_free_count(u, m, ldb, kt, vt, pkms[i]) >= count_need);
                        SHR_IF_ERR_EXIT(
                            ripple_move(u, m, ldb, move_pfx, &rem_cnt,
                                        &moved_cnt, pkm, pkms[i]));

                        rem_entry -= ripple_entry_num(u, m, move_pfx, pkm);
                    } else {
                        break;
                    }
                }

                if (!can_pkm_move_down(u, m, ldb, pfx, &move_pfx, pkm, pkms[i])) {
                    break;
                }
            } while (rem_entry > 0);
        }
    }

    if (rem_entry > 0 && (dir & DIR_UP)) {
        for (i = target_i + 1; i < pkms_cnt; i++) {
            if (can_pkm_move_up(u, m, ldb, pfx, &move_pfx, pkm, pkms[i])) {
                break;
            }
        }
        if (i < pkms_cnt) {
            do {
                kt = pfx_to_key_type(u, m, move_pfx);
                vt = pfx_to_vrf_type(u, m, move_pfx);
                count_need = half_entry_key_type(u, m, kt, pkm) ? 2 : 1;

                if (pkm_free_count(u, m, ldb, kt, vt, pkms[i]) >= count_need) {
                    SHR_IF_ERR_EXIT(
                        ripple_move(u, m, ldb, move_pfx, &rem_cnt,
                                    &moved_cnt, pkm, pkms[i]));

                    rem_entry -= ripple_entry_num(u, m, move_pfx, pkm);
                } else {
                    rv = ripple_insert(u, m, ldb, move_pfx, DIR_UP, pkms[i]);
                    if (SHR_SUCCESS(rv)) {
                        ALPM_ASSERT(pkm_free_count(u, m, ldb, kt, vt, pkms[i]) >= count_need);
                        SHR_IF_ERR_EXIT(
                            ripple_move(u, m, ldb, move_pfx, &rem_cnt,
                                        &moved_cnt, pkm, pkms[i]));

                        rem_entry -= ripple_entry_num(u, m, move_pfx, pkm);
                    } else {
                        break;
                    }
                }

                if (!can_pkm_move_up(u, m, ldb, pfx, &move_pfx, pkm, pkms[i])) {
                    break;
                }
            } while (rem_entry > 0);
        }
    }

    if (rem_entry > 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    SHR_FUNC_EXIT();
}

static void
lpm_insert_pkm_get(int u, int m, alpm_arg_t *arg, insert_pkms_t *pkms,
                   alpm_key_type_t kt, int *pkm, int bound_count)
{
#define MAX_BOUND 10
    int bound[MAX_BOUND];

    int first_bound = -1;
    int upper_bound = -1;
    int lower_bound = -1;
    int cmp_pfx;
    int i, sub, a, b;
    int pkm_cnt = bound_count >> 1;
    uint8_t ldb = arg->db;

    ALPM_ASSERT(bound_count <= MAX_BOUND);
    sal_memset(pkms, 0, sizeof(*pkms));

    cmp_pfx = arg->key.pfx;
    for (i = 0; i < pkm_cnt; i++) {
        sub = 2 * i;
        if (KI_VALID(u, m, ldb, pkm[i])) {
            if (kt == KEY_IPV4) {
                lpm_ipv4_bound_pfx_get(u, m, ldb, pkm[i], arg->key.vt,
                                   &bound[sub], &bound[sub + 1]);
            } else {
                lpm_ipv6_bound_pfx_get(u, m, ldb, pkm[i], kt, arg->key.vt,
                                   &bound[sub], &bound[sub + 1]);

            }
        } else {
            bound[sub] = -1;
            bound[sub + 1] = -1;
        }
        ALPM_ASSERT(bound[sub] >= bound[sub + 1]);
        ALPM_ASSERT(i == 0 || (bound[sub - 1] >= bound[sub]) ||
                    (bound[sub - 1] == -1));
    }

    for (i = 0; i < bound_count; i++) {
        if (bound[i] != INVALID_PFX) {
            first_bound = i;
            break;
        }
    }

    if (i == bound_count) {
        a = 0;
        b = pkm_cnt - 1;
    } else {
        ALPM_ASSERT(bound[i] != INVALID_PFX);
        for (i = first_bound; i < bound_count && i >= 0; i++) {
            if (bound[i] > cmp_pfx) {
                upper_bound = i;
                lower_bound = -1;
            } else if (bound[i] != -1 && bound[i] < cmp_pfx) {
                if (lower_bound == -1) {
                    lower_bound = i;
                }
            }
        }
        a = (upper_bound == -1) ? 0 : (upper_bound / 2);
        b = (lower_bound == -1) ? (pkm_cnt - 1) : (lower_bound / 2);
        ALPM_ASSERT(a <= b);
    }
    for (i = a; i <= b; i++) {
        pkms->v[pkm[i]] = KI_VALID(u, m, ldb, pkm[i]);
    }
}

static bool
lpm_key_type(int u, int m, alpm_key_type_t kt)
{
    return (kt == KEY_IPV4 ||
            kt == KEY_IPV6_SINGLE ||
            kt == KEY_IPV6_DOUBLE ||
            kt == KEY_IPV6_QUAD);
}
/*!
 * \brief Insert strict IPv4 route
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
l1_insert_key_type(int u, int m, alpm_arg_t *arg, int *pkm, int pkm_count)
{
    int rv = SHR_E_RESOURCE, i;
    insert_pkms_t pkms;
    int pfx;
    uint8_t ldb;
    SHR_FUNC_ENTER(u);

    /*
     * 1. Find pkms that can hold the new entry.
     * 2. Try directly inserting into eligible pkms, starting from most
     *    compacteo pkm. No ripple process.
     * 3. If direct insertion fails, do ripple.
     */
    pfx = arg->key.pfx;
    ldb = arg->db;

    if (lpm_key_type(u, m, arg->key.kt)) {
        lpm_insert_pkm_get(u, m, arg, &pkms, arg->key.kt, pkm, pkm_count * 2);
    } else {
        for (i = 0; i < pkm_count; i++) {
            pkms.v[pkm[i]] = KI_VALID(u, m, ldb, pkm[i]);
        }
    }

    /* Fill half entry first. */
    for (i = pkm_count - 1; i >= 0; i--) {
        if (pkms.v[pkm[i]]) {
            if (pfx_half_entry(u, m, ldb, pfx, pkm[i])) {
                SHR_ERR_EXIT(unpair_insert(u, m, arg, pkm[i]));
            }
        }
    }

    for (i = pkm_count - 1; i >= 0; i--) {
        if (rv == SHR_E_RESOURCE) {
            if (pkms.v[pkm[i]]) {
                rv = unpair_insert(u, m, arg, pkm[i]);
            }
        } else {
            break;
        }
    }

    for (i = pkm_count - 1; i >= 0; i--) {
        if (rv == SHR_E_RESOURCE) {
            if (pkms.v[pkm[i]]) {
                rv = ripple_insert(u, m, ldb, pfx, DIR_BOTH, pkm[i]);
                if (SHR_SUCCESS(rv)) {
                    SHR_ERR_EXIT(unpair_insert(u, m, arg, pkm[i]));
                }
            }
        } else {
            break;
        }
    }

    if (rv == SHR_E_RESOURCE) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Recover l1 pivot
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
l1_pivot_recover(int u, int m, alpm_arg_t *arg)
{
    alpm_pivot_t *pivot;
    int index;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(l1_pvt_alloc(u, m, arg, &pivot));
    SHR_IF_ERR_EXIT(l1_pvt_insert(u, m, arg, pivot));
    ALPM_ASSERT(pivot);
    index = arr_idx_from_pvt_idx(u, m, arg->index[LEVEL1]);
    L1_PVT_INFO(u, m, index) = pivot;
    arg->pivot[LEVEL1] = pivot;
    if (!arg->a1.direct_route) {
        SHR_IF_ERR_EXIT(bcmptm_rm_alpm_ln_recover(u, m, LEVEL2, arg));
        ALPM_ASSERT(pivot->bkt);
        pivot->bkt->log_bkt = arg->log_bkt[LEVEL1];
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retry insert after rippling entries from unpair64 to other blocks
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
can_ripple_insert(int u, int m, uint8_t ldb, int pfx, int dir, int pkm)
{
    int move_pfx;
    uint8_t count_need;
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    static const int pkms1[] = {PKM_S, PKM_D, PKM_Q, PKM_WD, PKM_WQ};
    static const int pkms2[] = {PKM_SS, PKM_SD, PKM_SQ};
    const int *pkms;
    int pkms_cnt;
    int target_i, i;
    int rv;
    int rem_entry;

    SHR_FUNC_ENTER(u);

    if (bcmptm_rm_alpm_db_is_src(u, m, ldb)) {
        pkms = pkms2;
        pkms_cnt = COUNTOF(pkms2);
    } else {
        pkms = pkms1;
        pkms_cnt = COUNTOF(pkms1);
    }
    for (target_i = 0; target_i < pkms_cnt; target_i++) {
        if (pkms[target_i] == pkm) {
            break;
        }
    }

    /* Least number of single-wide entries needed for one entry with the given pfx*/
    rem_entry = ripple_entry_num(u, m, pfx, pkm);

    if (dir & DIR_DOWN) {
        for (i = target_i - 1; i >= 0; i--) {
            if (can_pkm_move_down(u, m, ldb, pfx, &move_pfx, pkm, pkms[i])) {
                break;
            }
        }
        if (i >= 0) {
            do {
                kt = pfx_to_key_type(u, m, move_pfx);
                vt = pfx_to_vrf_type(u, m, move_pfx);
                count_need = half_entry_key_type(u, m, kt, pkm) ? 2 : 1;
                if (pkm_free_count(u, m, ldb, kt, vt, pkms[i]) >= count_need) {
                    rem_entry -= ripple_entry_num(u, m, move_pfx, pkm);
                }else {
                    rv = can_ripple_insert(u, m, ldb, move_pfx, DIR_DOWN, pkms[i]);
                    if (SHR_SUCCESS(rv)) {
                        rem_entry -= ripple_entry_num(u, m, move_pfx, pkm);
                    } else {
                        break;
                    }
                }

                if (!can_pkm_move_down(u, m, ldb, pfx, &move_pfx, pkm, pkms[i])) {
                    break;
                }
            } while (rem_entry > 0);
        }
    }

    if (rem_entry > 0 && (dir & DIR_UP)) {
        for (i = target_i + 1; i < pkms_cnt; i++) {
            if (can_pkm_move_up(u, m, ldb, pfx, &move_pfx, pkm, pkms[i])) {
                break;
            }
        }
        if (i < pkms_cnt) {
            do {
                kt = pfx_to_key_type(u, m, move_pfx);
                vt = pfx_to_vrf_type(u, m, move_pfx);
                count_need = half_entry_key_type(u, m, kt, pkm) ? 2 : 1;
                if (pkm_free_count(u, m, ldb, kt, vt, pkms[i]) >= count_need) {
                    rem_entry -= ripple_entry_num(u, m, move_pfx, pkm);
                } else {
                    rv = can_ripple_insert(u, m, ldb, move_pfx, DIR_UP, pkms[i]);
                    if (SHR_SUCCESS(rv)) {
                        rem_entry -= ripple_entry_num(u, m, move_pfx, pkm);
                    } else {
                        break;
                    }
                }

                if (!can_pkm_move_up(u, m, ldb, pfx, &move_pfx, pkm, pkms[i])) {
                    break;
                }
            } while (rem_entry > 0);
        }
    }

    if (rem_entry > 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    SHR_FUNC_EXIT();
}

static bool
can_insert_key_type(int u, int m, alpm_arg_t *arg, int *pkm, int pkm_count)
{
    insert_pkms_t pkms;
    int i;
    bool ok = false;
    uint8_t ldb = arg->db;
    alpm_vrf_type_t vt = arg->key.vt;

    arg->key.pfx = pfx_encode(u, m, arg->key.t.ipv, arg->key.vt,
                              arg->key.t.len, arg->key.t.app);

    if (lpm_key_type(u, m, arg->key.kt)) {
        lpm_insert_pkm_get(u, m, arg, &pkms, arg->key.kt, pkm, pkm_count * 2);
    } else {
        for (i = 0; i < pkm_count; i++) {
            pkms.v[pkm[i]] = KI_VALID(u, m, ldb, pkm[i]);
        }
    }
    for (i = pkm_count - 1; i >= 0; i--) {
        if (!ok) {
            if (pkms.v[pkm[i]]) {
                if (pfx_half_entry(u, m, ldb, arg->key.pfx, pkm[i])) {
                    ok = true;
                } else {
                    ok = pkm_free_count(u, m, ldb, arg->key.kt, vt, pkm[i]);
                }
            }
        } else {
            break;
        }
    }

    for (i = pkm_count - 1; i >= 0; i--) {
        if (!ok) {
            if (pkms.v[pkm[i]]) {
                ok = SHR_SUCCESS(
                    can_ripple_insert(u, m, ldb, arg->key.pfx, DIR_BOTH, pkm[i]));
            }
        } else {
            break;
        }
    }
    return ok;
}

/*!
 * \brief Get l1 entry based on pivot (with key)
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
l1_get(int u, int m, alpm_arg_t *arg)
{
    alpm_info_t *info;
    alpm_pivot_t *pivot = arg->pivot[LEVEL1];

    SHR_FUNC_ENTER(u);
    info = bcmptm_rm_alpm_info_get(u, m);
    arg->index[LEVEL1] = pivot->index;
    arg->key.kt = key_type_get(u, m, pivot->key.t.ipv, pivot->key.t.len,
                               pivot->key.t.app, pivot->key.vt);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (info->cmd_ctrl.req_flags & BCMLT_ENT_ATTR_GET_FROM_HW) {
        arg->pkm = index_to_pkm(u, m, HW_INDEX(pivot->index), NULL);
        if (bcmptm_rm_alpm_pkm_is_pair(u, m, arg->pkm)) {
            SHR_IF_ERR_EXIT(pair_get(u, m, arg));
        } else {
            SHR_IF_ERR_EXIT(unpair_get(u, m, arg));
        }
        /* For LT encode. */
        arg->ad.user_data_type = pivot->ad->user_data_type;
    } else {
        if (!arg->decode_data_only) {
            ALPM_ASSERT(pivot->key.t.max_bits ==
                bcmptm_rm_alpm_max_key_bits(u, m, pivot->key.t.ipv,
                                            pivot->key.t.app, pivot->key.vt));
            (void)bcmptm_rm_alpm_api_key_create(u, m, pivot->key.t.max_bits,
                                                &pivot->key.t.trie_ip,
                                                pivot->key.t.len,
                                                arg->key.ip_addr);
            (void)bcmptm_rm_alpm_len_to_mask(u, m, pivot->key.t.max_bits,
                                             pivot->key.t.len,
                                             &arg->key.ip_mask[0]);
        }
        arg->ad = *(pivot->ad);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
unpair_state_recover(int u, int m, alpm_arg_t *arg, int upkm)
{
    int pfx;
    int idx = HW_INDEX(arg->index[LEVEL1]);
    alpm_key_type_t kt = arg->key.kt;
    alpm_vrf_type_t vt = arg->key.vt;
    uint8_t ldb;

    SHR_FUNC_ENTER(u);
    ldb = arg->db;
    pfx = pfx_encode(u, m, arg->key.t.ipv, vt,
                     arg->key.t.len, arg->key.t.app);
    if (UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm) == 0) {
        UNPAIR_STATE_START(u, m, ldb, pfx, upkm) = idx;
        UNPAIR_STATE_END(u, m, ldb, pfx, upkm) = idx;
    } else {
        UNPAIR_STATE_END(u, m, ldb, pfx, upkm) = idx;
    }

    L1_USED_COUNT(u, m, ldb, kt, vt, upkm)++;
    if (!IS_HALF_R_INDEX(arg->index[LEVEL1])) {
        UNPAIR_STATE_VENT(u, m, ldb, pfx, upkm)++;
    }
    if (half_entry_key_type(u, m, kt, upkm)) {
        if (IS_HALF_R_INDEX(arg->index[LEVEL1])) {
            L1_HALF_COUNT(u, m, ldb, kt, vt, upkm)--;
            ALPM_ASSERT(L1_HALF_COUNT(u, m, ldb, kt, vt, upkm) >= 0);
        } else {
            L1_HALF_COUNT(u, m, ldb, kt, vt, upkm)++;
        }
        pfx_half_entry_update(u, m, ldb, pfx, upkm);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover the unpair block's fent/next/prev info in a bottom up manner.
 *
 * \param [in] u Device u.
 * \param [in] ldb Level1 DB
 * \param [in] upkm Unpair packing mode.
 *
 * \return SHR_E_XXX.
 */
static int
unpair_state_recover_done(int u, int m, uint8_t ldb, int upkm)
{
    int pfx, prev_pfx;
    int low_bound, low_bound2;
    int max_pfx = UNPAIR_MAX_PFX(u, m, ldb, upkm);
    int base_pfx = UNPAIR_BASE_PFX(u, m, ldb, upkm);

    low_bound = KI_END(u, m, ldb, upkm) + 1;

    ALPM_ASSERT(max_pfx != -1);
    UNPAIR_STATE_PREV(u, m, ldb, max_pfx, upkm) = -1;
    low_bound2 = -1;
    for (pfx = prev_pfx = max_pfx; pfx >= base_pfx; pfx--) {
        if (-1 == UNPAIR_STATE_START(u, m, ldb, pfx, upkm)) {
            continue;
        }

        if (low_bound2 == -1) {
            low_bound2 = UNPAIR_STATE_START(u, m, ldb, pfx, upkm);
        }
        if (prev_pfx != pfx) {
            UNPAIR_STATE_PREV(u, m, ldb, pfx, upkm) = prev_pfx;
            UNPAIR_STATE_NEXT(u, m, ldb, prev_pfx, upkm) = pfx;
            UNPAIR_STATE_FENT(u, m, ldb, prev_pfx, upkm) =
                UNPAIR_STATE_START(u, m, ldb, pfx, upkm) -
                UNPAIR_STATE_END(u, m, ldb, prev_pfx, upkm) - 1;
            ALPM_ASSERT(UNPAIR_STATE_FENT(u, m, ldb, prev_pfx, upkm) >= 0);
        }
        prev_pfx = pfx;
    }
    UNPAIR_STATE_NEXT(u, m, ldb, prev_pfx, upkm) = -1;
    UNPAIR_STATE_FENT(u, m, ldb, prev_pfx, upkm) =
        low_bound - UNPAIR_STATE_END(u, m, ldb, prev_pfx, upkm) - 1;

    return SHR_E_NONE;
}


static int
unpair_recover(int u, int m, uint8_t ldb, int pkm)
{
    alpm_arg_t *arg = NULL, *arg2 = NULL;
    int index;
    bcmdrd_sid_t sid = WIDE_PKM(pkm) ? WIDE_UNPAIR_SID(u, m) : UNPAIR_SID(u, m);
    int low_bound;
    bool is_single;
    l1_entry_t *le = NULL;
    bool v6_crossed = false;
    uint32_t dt_bitmap;

    SHR_FUNC_ENTER(u);
    index = KI_START(u, m, ldb, pkm);
    if (index == -1) {
        SHR_EXIT();
    }
    low_bound = KI_END(u, m, ldb, pkm) + 1;

    le = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    arg2 = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (arg2 == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    bcmptm_rm_alpm_arg_init(u, m, arg);
    bcmptm_rm_alpm_arg_init(u, m, arg2);
    arg->pkm = arg2->pkm = pkm;
    arg->db = arg2->db = ldb;
    is_single = pkm_is_single(u, m, pkm);
    while (index < low_bound) {
        if (is_single) {

            if (KEY_DATA_SEPARATE(u, m, FALSE)) {
                SHR_IF_ERR_EXIT(
                    read_l1_raw3(u, m, index, le->de, WIDE_SINGLE, &dt_bitmap));
            } else {
                SHR_IF_ERR_EXIT(
                    read_l1_raw(u, m, sid, index, le->de, le->se, &dt_bitmap));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, m, arg, le->de, le->se, L1V_NARROW_FULL));
            if (arg->valid) {
                if (arg->key.t.ipv == IP_VER_4 &&
                    arg->key.t.app == APP_LPM) {
                    is_single = false;
                    /* Clear ip mask to prevent pollution on crossing boundary */
                    sal_memset(arg->key.ip_mask, 0, sizeof(arg->key.ip_mask));
                    continue;
                }
                if (arg->a1.direct_route &&
                    bcmptm_rm_alpm_cache_data_type(u, m)) {
                    arg->ad.user_data_type = l1_dt_bitmap_get(u, m, 0, dt_bitmap);
                }
                arg->index[LEVEL1] = FULL_INDEX(index);
                SHR_IF_ERR_EXIT(unpair_state_recover(u, m, arg, pkm));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, m, arg));
                sal_memset(&arg->key.t.trie_ip, 0, sizeof(arg->key.t.trie_ip));
            } else {
                sal_memset(arg->key.ip_mask, 0, sizeof(arg->key.ip_mask));
            }
        } else {
            if (KEY_DATA_SEPARATE(u, m, FALSE)) {
                SHR_IF_ERR_EXIT(
                    read_l1_raw3(u, m, index, le->de, WIDE_HALF, &dt_bitmap));
            } else {
                SHR_IF_ERR_EXIT(
                    read_l1_raw(u, m, sid, index, le->de, le->se, &dt_bitmap));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, m, arg, le->de, le->se, L1V_NARROW_0));
            if (arg->valid) {
                if (arg->key.t.ipv == IP_VER_4 && !v6_crossed &&
                    arg->key.t.app == APP_LPM) {
                    v6_crossed = true;
                    /* Clear ip mask to prevent pollution on crossing boundary */
                    sal_memset(arg->key.ip_mask, 0, sizeof(arg->key.ip_mask));
                    /* Need clear both halves */
                    sal_memset(arg2->key.ip_mask, 0, sizeof(arg2->key.ip_mask));
                    continue;
                }
                if (arg->a1.direct_route &&
                    bcmptm_rm_alpm_cache_data_type(u, m)) {
                    arg->ad.user_data_type = l1_dt_bitmap_get(u, m, 0, dt_bitmap);
                }
                arg->index[LEVEL1] = HALF_INDEX_L(index);
                SHR_IF_ERR_EXIT(unpair_state_recover(u, m, arg, pkm));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, m, arg));
                sal_memset(&arg->key.t.trie_ip, 0, sizeof(arg->key.t.trie_ip));
            } else {
                sal_memset(arg->key.ip_mask, 0, sizeof(arg->key.ip_mask));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, m, arg2, le->de, le->se, L1V_NARROW_1));
            if (arg2->valid) {
                if (arg2->a1.direct_route &&
                    bcmptm_rm_alpm_cache_data_type(u, m)) {
                    arg2->ad.user_data_type = l1_dt_bitmap_get(u, m, 1, dt_bitmap);
                }
                arg2->index[LEVEL1] = HALF_INDEX_R(index);
                SHR_IF_ERR_EXIT(unpair_state_recover(u, m, arg2, pkm));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, m, arg2));
                sal_memset(&arg2->key.t.trie_ip, 0, sizeof(arg2->key.t.trie_ip));
            } else {
                sal_memset(arg2->key.ip_mask, 0, sizeof(arg2->key.ip_mask));
            }
        }
        index++;
    }
    SHR_IF_ERR_EXIT(unpair_state_recover_done(u, m, ldb, pkm));

exit:
    if (le) {
        shr_lmm_free(l1_entries_lmem[u][m], le);
    }
    if (arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg);
    }
    if (arg2) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg2);
    }
    SHR_FUNC_EXIT();
}





static int
pair_state_recover(int u, int m, alpm_arg_t * arg, int pkm)
{
    int pfx;
    int idx = HW_INDEX(arg->index[LEVEL1]);
    alpm_key_type_t kt = arg->key.kt;
    alpm_vrf_type_t vt = arg->key.vt;
    uint8_t ldb;

    SHR_FUNC_ENTER(u);
    ldb = arg->db;
    pfx = pfx_encode(u, m, arg->key.t.ipv, vt,
                     arg->key.t.len, arg->key.t.app);
    if (PAIR_STATE_VENT(u, m, ldb, pfx, pkm) == 0) {
        PAIR_STATE_START(u, m, ldb, pfx, pkm) = idx;
        PAIR_STATE_END(u, m, ldb, pfx, pkm) = idx;
    } else {
        PAIR_STATE_END(u, m, ldb, pfx, pkm) = idx;
    }
    if (!IS_HALF_R_INDEX(arg->index[LEVEL1])) {
        PAIR_STATE_VENT(u, m, ldb, pfx, pkm)++;
    }

    L1_USED_COUNT(u, m, ldb, kt, vt, pkm)++;
    if (half_entry_key_type(u, m, kt, pkm)) {
        pfx_half_entry_update(u, m, ldb, pfx, pkm);
        if (IS_HALF_R_INDEX(arg->index[LEVEL1])) {
            L1_HALF_COUNT(u, m, ldb, kt, vt, pkm)--;
            ALPM_ASSERT(L1_HALF_COUNT(u, m, ldb, kt, vt, pkm) >= 0);
        } else {
            L1_HALF_COUNT(u, m, ldb, kt, vt, pkm)++;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover the pair block's fent/next/prev info in a bottom up manner.
 *
 * \param [in] u Device u.
 * \param [in] ldb Level1 DB
 *
 * \return SHR_E_XXX.
 */
static int
pair_state_recover_done(int u, int m, uint8_t ldb, int pkm)
{
    int pfx, prev_pfx, la;
    int prev_end, curr_start;
    int low_bound, low_bound2;
    bool is_single_half;
    int max_pfx[LAYOUT_NUM] = {
        MAX_PFX(u, m, ldb, LAYOUT_SH, pkm),
        MAX_PFX(u, m, ldb, LAYOUT_D, pkm)
    };
    int base_pfx[LAYOUT_NUM] = {
        BASE_PFX(u, m, ldb, LAYOUT_SH, pkm),
        BASE_PFX(u, m, ldb, LAYOUT_D, pkm)
    };


    low_bound = KI_END(u, m, ldb, pkm) + 1;
    for (la = 0; la < LAYOUT_NUM; la++) {
        ALPM_ASSERT(max_pfx[la] != -1);
        PAIR_STATE_PREV(u, m, ldb, max_pfx[la], pkm) = -1;
        is_single_half = (la == LAYOUT_SH);
        low_bound2 = -1;
        for (pfx = prev_pfx = max_pfx[la]; pfx >= base_pfx[la]; pfx--) {
            if (-1 == PAIR_STATE_START(u, m, ldb, pfx, pkm)) {
                continue;
            }
            if (low_bound2 == -1) {
                low_bound2 = PAIR_STATE_START(u, m, ldb, pfx, pkm);
                /*
                 * The upper bound and lower bound for a given pfx zone (V4,
                 * V4_Global, V6, V6_Global) do not have to be exactly
                 * same as the state in the previous run.
                 * Example, when the fent(max_pfx) != 0, those free entries,
                 * when recovered, will be shifted to the previous zone.
                 * The only exception is the toppest max_pfx in the tcam block.
                 */
                if (la == LAYOUT_SH) {
                    PAIR_STATE_START(u, m, ldb, max_pfx[la], pkm) = low_bound2;
                    PAIR_STATE_END(u, m, ldb, max_pfx[la], pkm) = low_bound2 - 1;
                }
            }
            if (prev_pfx != pfx) {
                PAIR_STATE_PREV(u, m, ldb, pfx, pkm) = prev_pfx;
                PAIR_STATE_NEXT(u, m, ldb, prev_pfx, pkm) = pfx;
                ALPM_ASSERT(pfx_is_single_half(u, m, ldb, pfx, pkm) == is_single_half);
                if (is_single_half) {
                    curr_start = PAIR_STATE_START(u, m, ldb, pfx, pkm);
                    prev_end = PAIR_STATE_END(u, m, ldb, prev_pfx, pkm);
                    if (!LAST_PFX_SPAN(u, m, ldb, pkm) &&
                        prev_pfx == RANGE1_LAST_PFX(u, m, ldb, pkm)) {
                        PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) =
                            RANGE1_LAST(u, m, ldb, pkm) - prev_end;
                        PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) +=
                            curr_start - RANGE2_FIRST(u, m, ldb, pkm);
                    } else {
                        PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) =
                            curr_start - prev_end - 1;
                    }
                } else {
                    curr_start = pair_index_encode(u, m, PAIR_STATE_START(u, m, ldb, pfx, pkm));
                    prev_end = pair_index_encode(u, m, PAIR_STATE_END(u, m, ldb, prev_pfx, pkm));
                    PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) = (curr_start - prev_end - 1);
                    PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) <<= 1;
                }
                ALPM_ASSERT(PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) >= 0);
            }
            prev_pfx = pfx;
        }
        PAIR_STATE_NEXT(u, m, ldb, prev_pfx, pkm) = -1;
        if (is_single_half) {
            if (!LAST_PFX_SPAN(u, m, ldb, pkm) &&
                prev_pfx == RANGE1_LAST_PFX(u, m, ldb, pkm)) {
                PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) =
                    RANGE1_LAST(u, m, ldb, pkm) - PAIR_STATE_END(u, m, ldb, prev_pfx, pkm);
                PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) +=
                    low_bound - RANGE2_FIRST(u, m, ldb, pkm);
            } else {
                PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) =
                    low_bound - PAIR_STATE_END(u, m, ldb, prev_pfx, pkm) - 1;
            }
        } else {
            PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) =
               (pair_index_encode(u, m, low_bound) -
                pair_index_encode(u, m, PAIR_STATE_END(u, m, ldb, prev_pfx, pkm)) - 1) << 1;
        }
        ALPM_ASSERT(PAIR_STATE_FENT(u, m, ldb, prev_pfx, pkm) >= 0);
        low_bound = low_bound2;
    }

    return SHR_E_NONE;
}


static int
pair_recover(int u, int m, uint8_t ldb, int pkm)
{
    alpm_arg_t *arg = NULL, *arg2 = NULL;
    int index;
    bcmdrd_sid_t sid = WIDE_PKM(pkm) ? WIDE_PAIR_SID(u, m) : PAIR_SID(u, m);
    int low_bound;
    bool is_double, is_single;
    l1_entry_t *le = NULL;
    int max_sh_pfx;
    int range1_first;
    uint32_t dt_bitmap;

    SHR_FUNC_ENTER(u);
    index = KI_START(u, m, ldb, pkm);
    if (index == -1) {
        SHR_EXIT();
    }
    low_bound = KI_END(u, m, ldb, pkm) + 1;

    le = shr_lmm_alloc(l1_entries_lmem[u][m]);
    if (le == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    arg2 = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (arg2 == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    bcmptm_rm_alpm_arg_init(u, m, arg);
    bcmptm_rm_alpm_arg_init(u, m, arg2);
    arg->pkm = arg2->pkm = pkm;
    arg->db = arg2->db = ldb;
    is_double = true;
    is_single = true;
    switch (ldb) {
    case L1_DB0: range1_first = L1_HA_RANGE1_FIRST_DB0(u, m, pkm); break;
    case L1_DB1: range1_first = L1_HA_RANGE1_FIRST_DB1(u, m, pkm); break;
    case L1_DB2: range1_first = L1_HA_RANGE1_FIRST_DB2(u, m, pkm); break;
    case L1_DB3: range1_first = L1_HA_RANGE1_FIRST_DB3(u, m, pkm); break;
    default: SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    while (index < low_bound) {
        if (is_double) {
            if (index == range1_first) {
                /* If low_bound equals to zero, none matters. */
                sid = WIDE_PKM(pkm) ? WIDE_UNPAIR_SID(u, m) : UNPAIR_SID(u, m);
                RANGE1_FIRST(u, m, ldb, pkm) = index;
                RANGE2_FIRST(u, m, ldb, pkm) = RANGE1_FIRST(u, m, ldb, pkm) + TCAM_DEPTH(u, m);
                RANGE1_LAST(u, m, ldb, pkm) =
                    ((tcam_num(u, m, RANGE1_FIRST(u, m, ldb, pkm)) + 1) * TCAM_DEPTH(u, m)) - 1;
                max_sh_pfx = MAX_PFX(u, m, ldb, LAYOUT_SH, pkm);
                PAIR_STATE_START(u, m, ldb, max_sh_pfx, pkm) = RANGE1_FIRST(u, m, ldb, pkm);
                PAIR_STATE_END(u, m, ldb, max_sh_pfx, pkm) = RANGE1_FIRST(u, m, ldb, pkm) - 1;

                is_double = false;
                /* Clear ip mask to prevent pollution on crossing boundary */
                sal_memset(arg->key.ip_mask, 0, sizeof(arg->key.ip_mask));
                continue;
            }
            if (KEY_DATA_SEPARATE(u, m, TRUE)) {
                SHR_IF_ERR_EXIT(
                    read_l1_raw3(u, m, index, le->de, WIDE_DOUBLE, &dt_bitmap));
            } else {
                SHR_IF_ERR_EXIT(
                    read_l1_raw(u, m, sid, index, le->de, le->se, &dt_bitmap));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, m, arg, le->de, le->se, L1V_WIDE));
            if (arg->valid) {
                if (arg->a1.direct_route &&
                    bcmptm_rm_alpm_cache_data_type(u, m)) {
                    arg->ad.user_data_type = l1_dt_bitmap_get(u, m, 0, dt_bitmap);
                }
                arg->index[LEVEL1] = FULL_INDEX(index);
                SHR_IF_ERR_EXIT(pair_state_recover(u, m, arg, pkm));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, m, arg));
                sal_memset(&arg->key.t.trie_ip, 0, sizeof(arg->key.t.trie_ip));
            } else {
                sal_memset(arg->key.ip_mask, 0, sizeof(arg->key.ip_mask));
            }
            index++;
            if (index_in_right_tcam(u, m, index)) {
                index += TCAM_DEPTH(u, m);
            }
        } else if (is_single) { /* single */
            if (KEY_DATA_SEPARATE(u, m, FALSE)) {
                SHR_IF_ERR_EXIT(
                    read_l1_raw3(u, m, index, le->de, WIDE_SINGLE, &dt_bitmap));
            } else {
                SHR_IF_ERR_EXIT(
                    read_l1_raw(u, m, sid, index, le->de, le->se, &dt_bitmap));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, m, arg, le->de, le->se, L1V_NARROW_FULL));
            if (arg->valid) {
                /* We can also check if encoded prefix is half for scalability. */
                if ((arg->key.t.ipv == IP_VER_4 &&
                     arg->key.t.app == APP_LPM) || PKM_WS == pkm) {
                    is_single = false;
                    /* Clear ip mask to prevent pollution on crossing boundary */
                    sal_memset(arg->key.ip_mask, 0, sizeof(arg->key.ip_mask));
                    continue;
                }
                if (arg->a1.direct_route &&
                    bcmptm_rm_alpm_cache_data_type(u, m)) {
                    arg->ad.user_data_type = l1_dt_bitmap_get(u, m, 0, dt_bitmap);
                }
                arg->index[LEVEL1] = FULL_INDEX(index);
                SHR_IF_ERR_EXIT(pair_state_recover(u, m, arg, pkm));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, m, arg));
                sal_memset(&arg->key.t.trie_ip, 0, sizeof(arg->key.t.trie_ip));
            } else {
                sal_memset(arg->key.ip_mask, 0, sizeof(arg->key.ip_mask));
            }
            if (arg->valid) {
                if (index <= RANGE1_LAST(u, m, ldb, pkm)) {
                    RANGE1_LAST_PFX(u, m, ldb, pkm) =
                        pfx_encode(u, m, arg->key.t.ipv, arg->key.vt, arg->key.t.len,
                                   arg->key.t.app);
                    if (index == RANGE1_LAST(u, m, ldb, pkm)) {
                        index = RANGE2_FIRST(u, m, ldb, pkm);
                        continue;
                    }
                } else if (index == RANGE2_FIRST(u, m, ldb, pkm)) {
                    if (RANGE1_LAST_PFX(u, m, ldb, pkm) ==
                        pfx_encode(u, m, arg->key.t.ipv, arg->key.vt, arg->key.t.len,
                                   arg->key.t.app)) {
                        LAST_PFX_SPAN(u, m, ldb, pkm) = 1;
                    }
                }
            }
            if (index == RANGE1_LAST(u, m, ldb, pkm)) {
                index = RANGE2_FIRST(u, m, ldb, pkm);
            } else {
                index++;
            }
        } else { /* half */
            if (KEY_DATA_SEPARATE(u, m, FALSE)) {
                SHR_IF_ERR_EXIT(
                    read_l1_raw3(u, m, index, le->de, WIDE_HALF, &dt_bitmap));
            } else {
                SHR_IF_ERR_EXIT(
                    read_l1_raw(u, m, sid, index, le->de, le->se, &dt_bitmap));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, m, arg, le->de, le->se, L1V_NARROW_0));
            if (arg->valid) {
                if (arg->a1.direct_route &&
                    bcmptm_rm_alpm_cache_data_type(u, m)) {
                    arg->ad.user_data_type = l1_dt_bitmap_get(u, m, 0, dt_bitmap);
                }
                arg->index[LEVEL1] = HALF_INDEX_L(index);
                SHR_IF_ERR_EXIT(pair_state_recover(u, m, arg, pkm));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, m, arg));
                sal_memset(&arg->key.t.trie_ip, 0, sizeof(arg->key.t.trie_ip));
            } else {
                sal_memset(arg->key.ip_mask, 0, sizeof(arg->key.ip_mask));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, m, arg2, le->de, le->se, L1V_NARROW_1));
            if (arg2->valid) {
                if (arg2->a1.direct_route &&
                    bcmptm_rm_alpm_cache_data_type(u, m)) {
                    arg2->ad.user_data_type = l1_dt_bitmap_get(u, m, 1, dt_bitmap);
                }
                arg2->index[LEVEL1] = HALF_INDEX_R(index);
                SHR_IF_ERR_EXIT(pair_state_recover(u, m, arg2, pkm));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, m, arg2));
                sal_memset(&arg2->key.t.trie_ip, 0, sizeof(arg2->key.t.trie_ip));
            } else {
                sal_memset(arg2->key.ip_mask, 0, sizeof(arg2->key.ip_mask));
            }
            if (arg->valid) {
                if (index <= RANGE1_LAST(u, m, ldb, pkm)) {
                    RANGE1_LAST_PFX(u, m, ldb, pkm) =
                        pfx_encode(u, m, arg->key.t.ipv, arg->key.vt, arg->key.t.len,
                                   arg->key.t.app);
                    if (index == RANGE1_LAST(u, m, ldb, pkm)) {
                        index = RANGE2_FIRST(u, m, ldb, pkm);
                        continue;
                    }
                } else if (index == RANGE2_FIRST(u, m, ldb, pkm)) {
                    if (RANGE1_LAST_PFX(u, m, ldb, pkm) ==
                        pfx_encode(u, m, arg->key.t.ipv, arg->key.vt, arg->key.t.len,
                                   arg->key.t.app)) {
                        LAST_PFX_SPAN(u, m, ldb, pkm) = 1;
                    }
                }
            }
            if (index == RANGE1_LAST(u, m, ldb, pkm)) {
                index = RANGE2_FIRST(u, m, ldb, pkm);
            } else {
                index++;
            }
        }
    }
    SHR_IF_ERR_EXIT(pair_state_recover_done(u, m, ldb, pkm));
exit:
    if (le) {
        shr_lmm_free(l1_entries_lmem[u][m], le);
    }
    if (arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg);
    }
    if (arg2) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg2);
    }
    SHR_FUNC_EXIT();
}


static void
key_type_qualify_pkm(int u, int m, uint8_t is_src,
                     alpm_key_type_t kt, int *pkm, int *pkm_count)
{
    int i = 0;
    switch (kt) {
    case KEY_IPV6_QUAD:
        if (is_src) {
            pkm[i++] = PKM_SQ;
        } else {
            pkm[i++] = PKM_WQ;
            pkm[i++] = PKM_WD;
            pkm[i++] = PKM_Q;
        }
        break;
    case KEY_IPV6_DOUBLE:
        if (is_src) {
            pkm[i++] = PKM_SQ;
            pkm[i++] = PKM_SD;
        } else {
            pkm[i++] = PKM_WQ;
            pkm[i++] = PKM_WD;
            pkm[i++] = PKM_Q;
            pkm[i++] = PKM_D;
        }
        break;
    case KEY_IPV6_SINGLE:
        if (is_src) {
            pkm[i++] = PKM_SQ;
            pkm[i++] = PKM_SD;
            pkm[i++] = PKM_SS;
        } else {
            pkm[i++] = PKM_WQ;
            pkm[i++] = PKM_WD;
            pkm[i++] = PKM_Q;
            pkm[i++] = PKM_D;
            pkm[i++] = PKM_S;
        }
        break;
    case KEY_IPV4:
        if (is_src) {
            pkm[i++] = PKM_SQ;
            pkm[i++] = PKM_SD;
            pkm[i++] = PKM_SS;
        } else {
            pkm[i++] = PKM_WQ;
            pkm[i++] = PKM_WD;
            pkm[i++] = PKM_Q;
            pkm[i++] = PKM_D;
            pkm[i++] = PKM_S;
        }
        break;
    case KEY_L3MC_V4:
    case KEY_L3MC_V6:
        pkm[i++] = PKM_WQ;
        pkm[i++] = PKM_WD;
        break;
    case KEY_COMP0_V4:
    case KEY_COMP0_V6:
    case KEY_COMP1_V4:
    case KEY_COMP1_V6:
        pkm[i++] = (is_src ? PKM_SFP : PKM_FP);
        break;
    default:
        assert(0);
        break;
    }
    *pkm_count = i;
}

/*******************************************************************************
 * Public Functions
 */
alpm_key_type_t
bcmptm_rm_alpm_key_type_get(int u, int m, int ipv6, int len, uint8_t app,
                            alpm_vrf_type_t vt)
{
    return key_type_get(u, m, ipv6, len, app, vt);
}


int
bcmptm_rm_alpm_l1_max_count(int u, int m, uint8_t ldb, alpm_key_type_t kt,
                            alpm_vrf_type_t vt)
{
    int pkm;
    int max_count = 0;

    for (pkm = 0; pkm < PKM_NUM; pkm++) {
        if (!KI_VALID(u, m, ldb, pkm)) {
            continue;
        }
        max_count += L1_MAX_COUNT(u, m, ldb, kt, pkm);
    }

    return max_count;
}


void
bcmptm_rm_alpm_l1_pfx_decode(int u, int m, int pfx,
                             int *ipv,
                             alpm_vrf_type_t *vt,
                             int *len,
                             uint8_t *pfx_type,
                             uint8_t *app)
{
    int length;
    int i;
    prefix_layout_t *layout;
    int cnt;

    prefix_layout_resolve(u, m, &layout, &cnt);

    for (i = cnt - 1; i >= 0; i--) {
        pfx -= layout[i].num_pfx;
        if (pfx < 0) {
            length = pfx + layout[i].num_pfx;
            if (ipv) {
                *ipv = layout[i].ipv;
            }
            if (vt) {
                *vt = layout[i].vrf_type;
            }
            if (len) {
                *len = length;
            }
            if (pfx_type) {
                *pfx_type = layout[i].pfx_type;
            }
            if (app) {
                *app = layout[i].app;
            }
            break;
        }
    }
    assert(i >= 0);
    return;
}

int
bcmptm_rm_alpm_l1_pfx_encode(int u, int m, uint8_t app, int ipv,
                             alpm_vrf_type_t vt,
                             int len)
{
    int base = 0, i;
    int cnt;
    prefix_layout_t *layout;

    prefix_layout_resolve(u, m, &layout, &cnt);
    for (i = cnt - 1; i >= 0; i--) {
        if (layout[i].ipv == ipv &&
            layout[i].vrf_type == vt &&
            layout[i].pfx_type == PFX_NORMAL &&
            layout[i].app == app) {
            break;
        }
        base += layout[i].num_pfx;
    }

    return base + len;
}

void
bcmptm_rm_alpm_l1_ha_sync(int u, int m)
{
    int pkm;
    if (alpm_l1_ha[u][m] && alpm_l1_info[u][m]) {
        for (pkm = 0; pkm < PKM_NUM; pkm++) {
            L1_HA_RANGE1_FIRST_DB0(u, m, pkm) = RANGE1_FIRST(u, m, L1_DB0, pkm);
            L1_HA_RANGE1_FIRST_DB1(u, m, pkm) = RANGE1_FIRST(u, m, L1_DB1, pkm);
            L1_HA_RANGE1_FIRST_DB2(u, m, pkm) = RANGE1_FIRST(u, m, L1_DB2, pkm);
            L1_HA_RANGE1_FIRST_DB3(u, m, pkm) = RANGE1_FIRST(u, m, L1_DB3, pkm);
        }
    }
}

int
bcmptm_rm_alpm_l1_insert(int u, int m, alpm_arg_t *arg)
{
    int rv = SHR_E_RESOURCE;
    int pkm[PKM_NUM], pkm_count;
    uint8_t is_src;
    SHR_FUNC_ENTER(u);

    if (!priorities_check(u, m, arg)) {
        SHR_IF_ERR_EXIT(rv);
    }

    /* Override key type with Level-1 only */
    arg->key.kt = key_type_get(u, m, arg->key.t.ipv, arg->key.t.len,
                               arg->key.t.app, arg->key.vt);

    /* Assume ltm will do lookup ahead as always */
    if (arg->pivot[LEVEL1] && arg->pivot[LEVEL1]->index != INVALID_INDEX) {
        /* Replace operation */
        arg->index[LEVEL1] = arg->pivot[LEVEL1]->index;
        arg->pkm = index_to_pkm(u, m, HW_INDEX(arg->index[LEVEL1]), NULL);
        if (bcmptm_rm_alpm_pkm_is_pair(u, m, arg->pkm)) {
            rv = pair_update(u, m, arg);
        } else {
            rv = unpair_update(u, m, arg);
        }
        if (arg->a1.direct_route) {
            arg->req_update = 1;
        }

        arg->state_changed = 1;
        SHR_ERR_EXIT(rv);
    }

    if (arg->a1.direct_route) {
        if (bcmptm_rm_alpm_l1_can_insert(u, m, arg) == 0) {
            arg->resouce_full = 1;
            LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(u, "Resource Check: Level 1 resource full\n")));
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }
    arg->state_changed = 1;

    /* From now on, new add operation */
    arg->key.pfx = pfx_encode(u, m, arg->key.t.ipv, arg->key.vt,
                              arg->key.t.len, arg->key.t.app);
    is_src = bcmptm_rm_alpm_db_is_src(u, m, arg->db);
    key_type_qualify_pkm(u, m, is_src, arg->key.kt, &pkm[0], &pkm_count);
    rv = l1_insert_key_type(u, m, arg, pkm, pkm_count);

    if (rv == SHR_E_RESOURCE) {
        ALPM_ASSERT(0);
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    /* SHR_IF_ERR_EXIT(pair_state_verify(u, m)); */

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_l1_delete(int u, int m, alpm_arg_t *arg)
{
    int rv;
    SHR_FUNC_ENTER(u);

    if (arg->index[LEVEL1] == INVALID_INDEX) {
        alpm_pivot_t *pivot;
        SHR_IF_ERR_VERBOSE_EXIT(
            l1_pvt_lookup(u, m, arg, &pivot));
        arg->index[LEVEL1] = pivot->index;
        arg->key.kt = key_type_get(u, m, arg->key.t.ipv, arg->key.t.len,
                                   arg->key.t.app, arg->key.vt);
    }

    arg->key.pfx = pfx_encode(u, m, arg->key.t.ipv, arg->key.vt,
                              arg->key.t.len, arg->key.t.app);
    if (index_in_pair_tcam(u, m, HW_INDEX(arg->index[LEVEL1]))) {
        rv = pair_delete(u, m, arg);
    } else {
        rv = unpair_delete(u, m, arg);
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_l1_read(int u, int m, alpm_arg_t *arg)
{
    arg->key.kt = key_type_get(u, m, arg->key.t.ipv, arg->key.t.len,
                               arg->key.t.app, arg->key.vt);
    arg->pkm = index_to_pkm(u, m, HW_INDEX(arg->index[LEVEL1]), NULL);
    if (bcmptm_rm_alpm_pkm_is_pair(u, m, arg->pkm)) {
        return pair_get(u, m, arg);
    } else {
        return unpair_get(u, m, arg);
    }
}

int
bcmptm_rm_alpm_l1_match(int u, int m, alpm_arg_t *arg)
{
    int rv;

    if (!arg->match_ready) {
        rv = l1_pvt_lookup(u, m, arg, &arg->pivot[LEVEL1]);
    } else {
        rv = SHR_E_NONE;
    }

    if (SHR_SUCCESS(rv)) {
        return l1_get(u, m, arg);
    } else {
        return rv;
    }
}


int
bcmptm_rm_alpm_l1_find(int u, int m, alpm_arg_t *arg)
{
    int rv;

    rv = l1_pvt_find(u, m, arg, &arg->pivot[LEVEL1]);
    if (SHR_SUCCESS(rv)) {
        return l1_get(u, m, arg);
    } else {
        return rv;
    }
}



int
bcmptm_rm_alpm_l1_can_insert(int u, int m, alpm_arg_t *arg)
{
    alpm_key_type_t kt = arg->key.kt;
    alpm_vrf_type_t vt = arg->key.vt;
    int pkm[PKM_NUM];
    int pkm_count;
    uint8_t is_src;

    if (l1_free_count(u, m, arg->db, kt, vt) >= 4) {
        return true;
    }

    /*
     * If this is not a direct route, the arg that is used here
     * to check resource is the route arg, not the pivot arg
     * that will be actually used to insert.
     * However the qualify check done below is assuming it's the
     * same arg as insert, thus we have to skip for non-direct route.
     */
    if (arg->a1.direct_route || arg->vrf_init) {
        is_src = bcmptm_rm_alpm_db_is_src(u, m, arg->db);
        key_type_qualify_pkm(u, m, is_src, kt, &pkm[0], &pkm_count);
        return can_insert_key_type(u, m, arg, &pkm[0], pkm_count);
    }

    return false;
}

void
bcmptm_rm_alpm_l1_cleanup(int u, int m, bool graceful)
{
    if (L1_INFO(u, m) && L1_INFO_FD(u, m, slice_mode_en)) {
    } else {
        bcmptm_ser_alpm_info_cb_reg(u, m, NULL);
    }
    if (L1_HA(u, m)) {
        l1_ha_cleanup(u, m, graceful);
    }

    if (L1_INFO(u, m)) {
        unpair_state_cleanup(u, m);
        unpair_stats_cleanup(u, m);

        pair_state_cleanup(u, m);
        pair_stats_cleanup(u, m);

        l1_info_cleanup(u, m);
    }

    if (l1_entries_lmem[u][m]) {
        shr_lmm_delete(l1_entries_lmem[u][m]);
        l1_entries_lmem[u][m] = NULL;
    }
}

int
bcmptm_rm_alpm_l1_init(int u, int m, bool recover, uint8_t boot_type)
{
    int rv;
    alpm_dev_info_t *dev_info;
    bcmptm_ser_alpm_info_get_f cb[ALPMS] = {
        alpm_ser_sid_index_map_cb_m0,
        alpm_ser_sid_index_map_cb_m1
    };
    bcmptm_ser_alpm_info_get_f cb3[ALPMS] = {
        alpm_ser_sid_index_map_cb3_m0,
        alpm_ser_sid_index_map_cb3_m1
    };
    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    if (dev_info->l1_tcams_shared) {
        uint8_t num_tcams;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_uft_banks(u, m, DBS, LEVEL1, &num_tcams, NULL, NULL,
                                     NULL));
        if (num_tcams == 0) {
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(l1_info_init(u, m, dev_info, recover));

    SHR_IF_ERR_EXIT(unpair_state_init(u, m, recover));
    SHR_IF_ERR_EXIT(pair_state_init(u, m, recover));
    SHR_IF_ERR_EXIT(l1_stats_init(u, m, false));

    SHR_IF_ERR_EXIT(l1_ha_init(u, m, recover, boot_type));

    if (L1_INFO_FD(u, m, slice_mode_en)) {
        SHR_IF_ERR_EXIT(l1_ser_slice_register(u, m, recover));
    } else {
        if (!KEY_DATA_SEPARATE(u, m, FALSE) &&
            !KEY_DATA_SEPARATE(u, m, TRUE)) {
            /* Both combined. */
            bcmptm_ser_alpm_info_cb_reg(u, m, cb[m]);
        } else if (KEY_DATA_SEPARATE(u, m, FALSE) &&
                   KEY_DATA_SEPARATE(u, m, TRUE)) {
            /* Both separated. */
            bcmptm_ser_alpm_info_cb_reg(u, m, alpm_ser_sid_index_map_cb2);
        } else if (KEY_DATA_SEPARATE(u, m, FALSE) &&
                   !KEY_DATA_SEPARATE(u, m, TRUE)) {
            /* Narrow separated, wide combined. */
            bcmptm_ser_alpm_info_cb_reg(u, m, cb3[m]);
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    ALPM_LMM_INIT(l1_entry_t,
                  l1_entries_lmem[u][m],
                  L1_ENTRY_ALLOC_CHUNK_SIZE,
                  false,
                  rv);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_l1_recover(int u, int m, bool full)
{
    uint8_t ldb;
    l1_db_info_t *db;
    int pkm;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(full ? SHR_E_NONE: SHR_E_UNAVAIL);

    for (ldb = L1_DB0; ldb < L1_DBS; ldb++) {
        db = &L1_DB(u, m, ldb);
        if (!db->valid) {
            continue;
        }
        for (pkm = 0; pkm < PKM_NUM; pkm++) {
            if (!KI_VALID(u, m, ldb, pkm)) {
                continue;
            }
            if (bcmptm_rm_alpm_pkm_is_pair(u, m, pkm)) {
                SHR_IF_ERR_EXIT(pair_recover(u, m, ldb, pkm));
            } else {
                SHR_IF_ERR_EXIT(unpair_recover(u, m, ldb, pkm));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

alpm_l1_info_t *
bcmptm_rm_alpm_l1_info_get(int u, int m)
{
    return L1_INFO(u, m);
}


int
bcmptm_rm_alpm_arg_fill(int u, int m, alpm_arg_t *arg, void *de, void *de2, uint8_t l1v)
{
    SHR_FUNC_ENTER(u);

    arg->a1.is_route = bcmptm_rm_alpm_max_levels(u, m, arg->db) == TWO_LEVELS;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_alpm_l1_decode(u, m, arg, de, de2, l1v));
    if (arg->decode_lt_view == 1) {
        arg->data_type = arg->ad.user_data_type;
    }

    if (!arg->valid) {
        SHR_EXIT();
    }

    if (!arg->decode_data_only) {
        arg->key.t.w_vrf =
            bcmptm_rm_alpm_wvrf_get(u, m, arg->key.t.app, arg->key.vt, arg->key.t.w_vrf);

        arg->key.t.max_bits =
            bcmptm_rm_alpm_max_key_bits(u, m, arg->key.t.ipv, arg->key.t.app,
                                        arg->key.vt);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_alpm_mask_to_len(u, m, arg->key.t.max_bits,
                                        arg->key.ip_mask,
                                        &arg->key.t.len));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_alpm_trie_key_create(u, m, arg->key.t.max_bits,
                                            arg->key.ip_addr,
                                            arg->key.t.len,
                                            &arg->key.t.trie_ip));
        arg->key.kt = key_type_get(u, m, arg->key.t.ipv, arg->key.t.len,
                                   arg->key.t.app, arg->key.vt);
        arg->default_route = (arg->key.t.len == 0);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_l1_sid_hidx_get(int u, int m, ln_index_t index, alpm_key_type_t kt,
                               bcmdrd_sid_t *sid, uint32_t *hw_index,
                               bcmdrd_sid_t *sid_data, uint32_t *hw_index_data,
                               uint8_t *sid_cnt,
                               uint8_t *sid_cnt_data
                               )
{
    int pkm, sid_index, phy_blk_data_index;
    bcmdrd_sid_t agg_sid;
    alpm_sid_index_map_t map_info;
    int is_dw;
    int hw_idx;
    int phy_blk_index;
    uint8_t wide;

    SHR_FUNC_ENTER(u);
    hw_idx = HW_INDEX(index);
    phy_blk_index = phy_block_convert_index(u, m, hw_idx, false);
    *hw_index = phy_blk_index;
    pkm = index_to_pkm(u, m, hw_idx, NULL);
    if (bcmptm_rm_alpm_pkm_is_pair(u, m, pkm)) {
        agg_sid = pair_sid(u, m, kt, pkm);
    } else {
        agg_sid = unpair_sid(u, m, kt, pkm);
    }

    is_dw = is_pair_sid(u, m, agg_sid);
    if (KEY_DATA_SEPARATE(u, m, is_dw)) {
        if (IS_HALF_INDEX(index) || alpm_l1_info[u][m]->data_only_x2) {
            phy_blk_data_index = phy_blk_index << 1;
            phy_blk_data_index += IS_HALF_R_INDEX(index);
        } else {
            phy_blk_data_index = phy_blk_index;
        }

        if (is_pair_sid(u, m, agg_sid)) {
            map_info.index = phy_blk_index;
            map_info.sid_type = SID_TYPE_TCAM;
            map_info.wide = WIDE_DOUBLE;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_sid_index_map(u, m, &map_info));
            *(sid++) = map_info.sid;
            *(hw_index++) = map_info.sid_index;
            (*sid_cnt)++;

            map_info.sid_type = SID_TYPE_TCAM2;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_sid_index_map(u, m, &map_info));
            *(sid++) = map_info.sid;
            *(hw_index++) = map_info.sid_index;
            (*sid_cnt)++;

            map_info.sid_type = SID_TYPE_DATA;
            map_info.index = phy_blk_data_index;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_sid_index_map(u, m, &map_info));
            *(sid_data++) = map_info.sid;
            *(hw_index_data++) = map_info.sid_index;
            (*sid_cnt_data)++;
        } else {
            if (half_entry_key_type(u, m, kt, pkm)) {
                map_info.wide = WIDE_HALF;
            } else {
                map_info.wide = WIDE_SINGLE;
            }
            map_info.index = phy_blk_index;
            map_info.sid_type = SID_TYPE_TCAM;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_sid_index_map(u, m, &map_info));
            *(sid++) = map_info.sid;
            *(hw_index++) = map_info.sid_index;
            (*sid_cnt)++;


            map_info.sid_type = SID_TYPE_DATA;
            map_info.index = phy_blk_data_index;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_sid_index_map(u, m, &map_info));
            *(sid_data++) = map_info.sid;
            *(hw_index_data++) = map_info.sid_index;
            (*sid_cnt_data)++;
        }
    } else {
        if (L1_TCAMS_SHARED(u, m)) {
            if (is_pair_sid(u, m, agg_sid)) {
                wide = WIDE_DOUBLE;
            } else {
                wide = WIDE_SINGLE; /* No need to distinguish SINGLE and HALF.*/
            }
            SHR_IF_ERR_EXIT(
                multi_depth_combined_sid_index_map(u, m, wide, phy_blk_index,
                                                   &agg_sid, &sid_index));
            *hw_index = sid_index;
        } else {
            *hw_index = l1_hw_index_adjust_base(u, m, agg_sid, hw_idx);
        }
        *sid = agg_sid;
        (*sid_cnt)++;
    }
exit:
    SHR_FUNC_EXIT();
}

/* Dump ALPM table from top level 1 (idx) to bottom (up to lvl_cnt) */
int
bcmptm_rm_alpm_l1_dump(int u, int m, alpm_arg_t *arg, int idx, int lvl_cnt)
{
    int rv = SHR_E_NONE;
    int count = 0;
    int half_kt, half, half_cnt = 1;
    char ipstr[128] = {0};

    arg->key.kt = key_type_get(u, m, arg->key.t.ipv, arg->key.t.len,
                               arg->key.t.app, arg->key.vt);
    half_kt = half_entry_key_type(u, m, arg->key.kt, arg->pkm);

    if (half_kt) {
        half_cnt = 2;
    }

    for (half = 0; half < half_cnt; half++) {

        arg->index[LEVEL1] = (!half_kt ? FULL_INDEX(idx) :
                              (half ? HALF_INDEX_R(idx) : HALF_INDEX_L(idx)));

        if (bcmptm_rm_alpm_pkm_is_pair(u, m, arg->pkm)) {
            rv = pair_get(u, m, arg);
        } else {
            rv = unpair_get(u, m, arg);
        }

        if (arg->valid == 0 || SHR_FAILURE(rv)) {
            continue;
        }

        bcmptm_rm_alpm_ipaddr_str(ipstr, arg->key.t.ipv, arg->key.ip_addr);

        if (half_kt) {
            cli_out("L1-[%4d.%d] | ", idx, half);
        } else {
            cli_out("L1-[%4d] | ", idx);
        }

        if (arg->key.vt == VRF_GLOBAL) {
            cli_out("GLo ");
        } else if (arg->key.vt == VRF_OVERRIDE) {
            cli_out("GHi ");
        } else { /* VRF_PRIVATE or VRF_DEFAULT */
            if (arg->key.t.app == APP_COMP0) {
                cli_out("L4_PORT:%d ", arg->key.t.w_vrf);
            } else {
                cli_out("VRF:%d ", arg->key.t.w_vrf);
            }
        }

        cli_out("| %s/%d | SBR:%d %s%s",
                ipstr, arg->key.t.len,
                arg->profile_index,
                arg->a1.direct_route ? "DR:1 " : "",
                arg->a1.default_miss ? "DM:1 " : "");

        cli_out("| AD:%d (%d-%d-%d-%d)\n", arg->ad.fmt_ad[0],
                arg->ad.fmt_ad[1], arg->ad.fmt_ad[2], arg->ad.fmt_ad[3],
                arg->ad.fmt_ad[4]);

        if (arg->a1.direct_route) {
            continue;
        }

        if (lvl_cnt >= 2) {
            rv = bcmptm_rm_alpm_bucket_table_dump(u, m, LEVEL2, lvl_cnt, arg, &count);
        }
    }

    return rv;
}

void
bcmptm_rm_alpm_l1_usage_get(int u, int m, bcmptm_cth_alpm_usage_info_t *info)
{
    int pkm;
    alpm_l1_info_t *l1_info;
    int max_count = 0;
    int used_count = 0;
    uint8_t ldb = 0;
    l1_db_info_t *l1_db;
    int alpm_mode;
    int chk_dbs[L1_DBS] = {0};

    info->l1_entries_utilization = 0;

    l1_info = bcmptm_rm_alpm_l1_info_get(u, m);
    if (!l1_info) {
        return;
    }

    alpm_mode = bcmptm_rm_alpm_mode(u, m);

    if ((alpm_mode & ALPM_MODE_PARALLEL) &&
         lpm_key_type(u, m, info->kt)) {
        /*
         * In parallel mode,
         * private routes and global routes have their individual dbs.
         */

        if (alpm_mode & ALPM_MODE_FP_COMP) {
            if (info->vt == VRF_PRIVATE) {
                chk_dbs[L1_DB0] = 1;
            } else {
                chk_dbs[L1_DB1] = 1;
            }
        } else if (alpm_mode & ALPM_MODE_RPF) {
            if (info->vt == VRF_PRIVATE) {
                chk_dbs[L1_DB0] = 1;
                chk_dbs[L1_DB2] = 1;
            } else {
                chk_dbs[L1_DB1] = 1;
                chk_dbs[L1_DB3] = 1;
            }
        } else {
            if (info->vt == VRF_PRIVATE) {
                chk_dbs[L1_DB0] = 1;
            } else {
                chk_dbs[L1_DB1] = 1;
            }
        }
    } else {
        chk_dbs[L1_DB0] = 1;
        chk_dbs[L1_DB1] = 1;
        chk_dbs[L1_DB2] = 1;
        chk_dbs[L1_DB3] = 1;
    }


    for (ldb = 0; ldb < L1_DBS; ldb++) {
        l1_db = &L1_DB(u, m, ldb);
        if (!l1_db->valid) {
            continue;
        }

        if (!chk_dbs[ldb]) {
            continue;
        }

        for (pkm = 0; pkm < PKM_NUM; pkm++) {
            if (!KI_VALID(u, m, ldb, pkm)) {
                continue;
            }
            max_count += L1_MAX_COUNT(u, m, ldb, info->kt, pkm);
            used_count += pkm_used_count(u, m, ldb, info->kt, info->vt, pkm);
        }
    }

    info->l1_max_entries = max_count;
    info->l1_used_entries = used_count;

    if (max_count) {
        info->l1_entries_utilization = used_count * 100 / max_count;
    }
}
