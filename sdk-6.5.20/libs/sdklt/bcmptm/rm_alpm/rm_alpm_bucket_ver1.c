/*! \file rm_alpm_bucket_ver1.c
 *
 * Resource Manager for ALPM Level-N bucket
 *
 * This file contains the resource manager for ALPM Level-N bucket
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
#include <shr/shr_util.h>

#include "rm_alpm_common.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"
#include "rm_alpm_device.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_leveln.h"
#include "rm_alpm_level1.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPM

/* Max 9 base buckets to shuffle into a free group */
#define MAX_SHUFFLE_NUM             (8 + 1)
#define RBKT_MAX_ENTRIES_VER1       20
#define BASE_ENTRIES_ALLOC_CHUNK_SIZE  2
#define BASE_BUCKETS_ALLOC_CHUNK_SIZE  8

#define SLIDE_GROUPS_TRAVERSE_STOP  6

#define DT_BITMAP_BITS      16

#define RBKT_EBMP(u, m, _d, _l, _i)        (RBKT(u, m, _d, _l, _i).entry_bitmap)
#define RBKT_VBANKS(u, m, _d, _l, _i)      (RBKT(u, m, _d, _l, _i).valid_banks)
#define RBKT_FBANKS(u, m, _d, _l, _i)      (RBKT(u, m, _d, _l, _i).free_banks)
#define RBKT_FORMAT(u, m, _d, _l, _i)      (RBKT(u, m, _d, _l, _i).format)
#define RBKT_IS_HEAD(u, m, _d, _l, _i)     (RBKT(u, m, _d, _l, _i).is_head)
#define VER1_RBKT_PIVOT(u, m, _d, _l, _i)  (RBKT(u, m, _d, _l, _i).pivot)
#define VER1_LOG_BKT(_g)                (log_bkt_t)(_g)
#define VER1_GROUP(_l)                  (rbkt_group_t)(_l)
#define VER1_MAX_FORMAT(u, m, _p, _d)      (v1_smax_format[u][m][_p][_d])
#define VER1_START_FORMAT(u, m, _p, _d)    (start_format[u][m][_p][_d])

#ifdef ALPM_DEBUG
#define VER1_RBKT_GROUP_SANITY(u, m, db, ln, group, with_holes, vbanks)\
        ver1_rbkt_group_sanity(u, m, db, ln, group, with_holes, vbanks)
#else
#define VER1_RBKT_GROUP_SANITY(u, m, db, ln, group, with_holes, vbanks)
#endif

#define MERGE_STOP_COUNT(f)         (1 - (f)) /* At least 1 */
#define MERGE_STOP_COUNT_AGAIN      100
#define MERGE_STOP_COUNT_SECURE     1000
#define MERGE_RATIO                 0.6

/* min((search_old_fmts + search_new_fmts), (max formats + 8 max banks) */
#define MAX_FORMAT_COUNT  10

/*******************************************************************************
 * Typedefs
 */

typedef struct base_entries_ver1_s {
    ln_base_entry_t entry[ALPM_MAX_BANKS][RBKT_MAX_ENTRIES_VER1];
    /*!< Pointer to the next element in the list (or NULL). */
    struct base_entries_ver1_s *next;
} base_entries_ver1_t;

typedef struct base_buckets_ver1_s {
    LN_ENTRY_t rbuf[ALPM_MAX_BANKS];
    int8_t rbkt_ofs[ALPM_MAX_BANKS];
    /*
     * Bitmap indicates that entries that are modified during the process, and
     * still valid after the process. If entry is invalidated during the process
     * it won't be recorded, in order to skip hit bit force set or clear.
     */
    uint32_t  modify_bitmap[ALPM_MAX_BANKS];
    uint32_t  dt_bitmap[ALPM_MAX_BANKS];
    /*!< Pointer to the next element in the list (or NULL). */
    struct base_buckets_ver1_s *next;
} base_buckets_ver1_t;

typedef struct bucket_merge_ctrl_s {
    int unit;
    int db;
    int m;
    int ln;
    int merges;
    int stop_count;
    uint8_t free_pivot;
    uint8_t again;
    int free_rbkts;
    alpm_pivot_t *pivot[MAX_LEVELS];
    rm_alpm_trie_t *pivot_trie[MAX_LEVELS];
    alpm_arg_t *new_arg;
} bucket_merge_ctrl_t;
/*******************************************************************************
 * Private variables
 */
static bkt_hdl_t *sbkt_hdl[BCMDRD_CONFIG_MAX_UNITS][ALPMS][DBS][LEVELS];
static const bool max_split_len_cal = TRUE;
/*!< Max format value  */
static int v1_smax_format[BCMDRD_CONFIG_MAX_UNITS][ALPMS][FROMAT_TYPES][DATA_TYPES];
static int start_format[BCMDRD_CONFIG_MAX_UNITS][ALPMS][FROMAT_TYPES][DATA_TYPES];
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern shr_lmm_hdl_t bcmptm_rm_alpm_arg_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern alpm_internals_t
    bcmptm_rm_alpm_internals[BCMDRD_CONFIG_MAX_UNITS][ALPMS][DBS][LEVELS];
static shr_lmm_hdl_t base_entries_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static shr_lmm_hdl_t base_buckets_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static LN_ENTRY_t zero_de = {0};
static int search_old_fmts = 8; /* For a small DFS scale. */
static int search_new_fmts = 2; /* For a small DFS scale. */
#ifdef ALPM_DEBUG
/* -1 to verify all, -2 to verify none, others to verify specific VRF. */
static int sanity_vrf_split_insert = -2;
static int sanity_vrf_traverse_merge = -2;
static int sanity_vrf_delete = -2;
static int sanity_vrf_insert = -2;
static int split_dump = 0;
#endif
/*******************************************************************************
 * Private Functions (declaration)
 */

static int
ver1_bucket_recover(int u, int m, int ln, alpm_arg_t *arg);

/*******************************************************************************
 * Private Functions (definition)
 */
static int
ver1_full_to_dt(int u, int m, int full)
{
    int dt;
    switch (full) {
    case 3: dt = DATA_FULL_3; break;
    case 2: dt = DATA_FULL_2; break;
    case 1: dt = DATA_FULL; break;
    default: dt = DATA_REDUCED; break;
    }
    return dt;
}

static void
ver1_dt_bitmap_set(int u, int m, int ent_ofs, uint32_t *dt_bitmap, int data_type)
{
    if (bcmptm_rm_alpm_has_4_data_types(u, m)) {
        if (((ent_ofs << 1) + 1) < DT_BITMAP_BITS) {
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
        if (ent_ofs < DT_BITMAP_BITS) {
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
ver1_dt_bitmap_get(int u, int m, int ent_ofs, uint32_t dt_bitmap)
{
    int bit0, bit1;
    if (bcmptm_rm_alpm_has_4_data_types(u, m)) {
        if (((ent_ofs << 1) + 1) < DT_BITMAP_BITS) {
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
        if (ent_ofs < DT_BITMAP_BITS) {
            return (dt_bitmap & (1 << ent_ofs)) ? DATA_FULL : DATA_REDUCED;
        } else {
            ALPM_ASSERT(0);
            return DATA_REDUCED;
        }

    }
}


static inline int
ver1_bkt_hdl_get(int u, int m, int db, int ln, bkt_hdl_t **bkt_hdl)
{

    ALPM_ASSERT(bkt_hdl);
    ALPM_ASSERT(db < DBS);
    ALPM_ASSERT(ln < LEVELS);
    *bkt_hdl = BKT_HDL(u, m, db, ln);

    if (*bkt_hdl == NULL) {
        return SHR_E_NOT_FOUND;
    } else {
        return SHR_E_NONE;
    }
}

static uint32_t
ver1_hw_idx_from_rbkt_idx(int u, int m, int db, int ln, rbkt_idx_t rbkt)
{
    int log_bank;
    int phy_bank;
    bkt_hdl_t *bkt_hdl;

    ver1_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    log_bank = rbkt % bkt_hdl->max_banks;
    phy_bank = bkt_hdl->l2p_bank_array[log_bank];

    return (phy_bank << bkt_hdl->bucket_bits) | (rbkt / bkt_hdl->max_banks);
}

static inline uint32_t
ver1_hw_idx_from_ent_idx(int u, int m, int db, int ln, ln_index_t ent_idx)
{
    return ver1_hw_idx_from_rbkt_idx(u, m, db, ln, RBKT_FROM_INDEX(ent_idx));
}

static inline rbkt_group_t
prev_group_get(int u, int m, int db, int8_t ln, rbkt_group_t group)
{
    shr_dq_t *dq;
    rbkt_t *prev;
    if (group <= FIRST_GROUP) {
        return INVALID_GROUP;
    } else {
        dq = SHR_DQ_PREV(&RBKT_LIST_NODE(u, m, db, ln, group), shr_dq_t*);
        prev = SHR_DQ_ELEMENT_GET(rbkt_t*, dq, listnode);
        return prev->index;
    }
}


/* Get pivot for a rbkt */
static alpm_pivot_t *
ver1_rbkt_pivot_get(int u, int m, int db, int ln, rbkt_idx_t rbkt_idx)
{
    rbkt_idx_t rbkt_idx2 = rbkt_idx;
    ALPM_ASSERT(rbkt_idx2 >= 0 && rbkt_idx2 < BKT_HDL_RBKTS_TOTAL(u, m, db, ln));
    while (!RBKT_IS_HEAD(u, m, db, ln, rbkt_idx2) && rbkt_idx2 >= 0) {
        rbkt_idx2--;
    }
    ALPM_ASSERT(rbkt_idx2 >= 0);
   /* not true for prepend, where we may move rbkt for a long distance.
    ALPM_ASSERT(rbkt_idx - rbkt_idx2 + 1 <= BKT_HDL_MAX_BANKS(u, m, db, ln)); */
    return VER1_RBKT_PIVOT(u, m, db, ln, rbkt_idx2);
}

static alpm_pivot_t *
ver1_bucket_pivot_get(int u, int m, int db, int ln, ln_index_t ln_index)
{
    rbkt_idx_t rbkt_idx;

    rbkt_idx = RBKT_FROM_INDEX(ln_index);
    return ver1_rbkt_pivot_get(u, m, db, ln, rbkt_idx);
}

/* Set pivot for a rbkt */
static inline void
ver1_rbkt_pivot_set(int u, int m, int db, int ln, rbkt_group_t group,
                    alpm_pivot_t *pivot)
{
    VER1_RBKT_PIVOT(u, m, db, ln, group) = pivot;
}

/*!
 * \brief Calculate the max length for new split point.
 *
 * \param [in] u Device u.
 * \param [in] pivot Pivot to be split.
 *
 * \return Max split length
 */
static void
ver1_trie_msl(int u, int m, alpm_pivot_t *pivot, int *msl, int *msc, int split_len_cal)
{
    shr_dq_t *listptr;
    shr_dq_t *elem;
    alpm_bkt_elem_t *curr;
    int max_key_len = 0;
    int distance;
#define DISTANCE_1  6
#define DISTANCE_2  4

    *msc = 0xffff;
    *msl = pivot->key.t.max_bits;
    if (*msl < pivot->key.t.len) {
        *msl = pivot->key.t.max_bits;
    }
    if (!split_len_cal) {
        return;
    }

    listptr = &pivot->bkt->list;
    SHR_DQ_TRAVERSE(listptr, elem)
        curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        if (max_key_len < curr->key.t.len) {
            max_key_len = curr->key.t.len;
            if (max_key_len == pivot->key.t.max_bits) {
                break;
            }
        }
    SHR_DQ_TRAVERSE_END(listptr, elem);

    distance = (max_key_len - pivot->key.t.len);
    if (distance > DISTANCE_1) {
        *msl = max_key_len - DISTANCE_1;
    } else if (distance > DISTANCE_2) {
        *msl = max_key_len - DISTANCE_2;
    } else {
        *msl = (pivot->key.t.len + max_key_len + 1) / 2;
    }

    return;
}


/*!
 * \brief Encode Level-1 ALPM data
 *
 * \param [in] u Device u.
 * \param [in] ln Database
 * \param [in] ln Level
 * \param [in] default_miss Default_miss
 * \param [in] group Rbkt group
 * \param [in] len Pivot length
 * \param [out] a1 ALPM data for level1
 *
 * \return nothing
 */
static void
ver1_alpm1_data_encode(int u, int m, int db, int ln, int default_miss,
                       rbkt_group_t group,
                       int len, alpm1_data_t *a1)
{
    int i, phy_bank, log_bank;
    bkt_hdl_t *bkt_hdl;

    ver1_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    a1->direct_route = 0; /* to skip Level-1  pivot mgmt */
    a1->default_miss = default_miss;
    a1->phy_bkt = group / bkt_hdl->max_banks;
    log_bank = (group % bkt_hdl->max_banks);
    a1->rofs = bkt_hdl->l2p_bank_array[log_bank];
    a1->kshift = len;
    for (i = 0; i < RBKT_VBANKS(u, m, db, ln, group); i++) {
        phy_bank = bkt_hdl->l2p_bank_array[log_bank];
        ALPM_ASSERT(bkt_hdl->bank_bitmap & (1 << phy_bank));
        a1->fmt[phy_bank] = RBKT_FORMAT(u, m, db, ln, group + i);
        if (++log_bank == bkt_hdl->max_banks) {
            log_bank = 0;
        }
    }
}

/*!
 * \brief Encode Level-2 ALPM data
 *
 * \param [in] u Device u.
 * \param [in] ln Database
 * \param [in] ln Level
 * \param [in] default_miss Default_miss
 * \param [in] group Rbkt group
 * \param [in] len Pivot length
 * \param [out] a2 ALPM data for level2
 *
 * \return nothing
 */
static void
ver1_alpm2_data_encode(int u, int m, int db, int ln, int default_miss,
                       rbkt_group_t group,
                       int len, alpm2_data_t *a2)
{
    int i, phy_bank, log_bank;
    bkt_hdl_t *bkt_hdl;

    ver1_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    a2->default_miss = default_miss;
    a2->phy_bkt = group / bkt_hdl->max_banks;
    log_bank = (group % bkt_hdl->max_banks);
    a2->rofs = bkt_hdl->l2p_bank_array[log_bank];
    a2->kshift = len;
    for (i = 0; i < RBKT_VBANKS(u, m, db, ln, group); i++) {
        phy_bank = bkt_hdl->l2p_bank_array[log_bank];
        ALPM_ASSERT(bkt_hdl->bank_bitmap & (1 << phy_bank));
        a2->fmt[phy_bank] = RBKT_FORMAT(u, m, db, ln, group + i);
        if (++log_bank == bkt_hdl->max_banks) {
            log_bank = 0;
        }
    }
}

/*!
 * \brief Migrate entry indexs from one src_rbkt to dst_rbkt within a group.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] bkt Pivot bucket
 * \param [in] srbkt_idx Source rbkt index
 * \param [in] drbkt_idx Destination rbkt index
 *
 * \return SHR_E_NONE, SHR_E_XXX.
 */
static int
ver1_rbkt_entry_index_migrate(int u, int m, int db, int ln,
                              elem_bkt_t *bkt,
                              rbkt_idx_t srbkt_idx,
                              rbkt_idx_t drbkt_idx,
                              int *map_ent_ofs)
{
    alpm_bkt_elem_t *curr;
    shr_dq_t *elem = NULL;
    int dst_ent_ofs;
    ln_index_t index;
    SHR_FUNC_ENTER(u);

    LOG_D((BSL_META("Migrate: ")));
    SHR_DQ_TRAVERSE(&bkt->list, elem)
        curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        index = curr->index;
        if (RBKT_FROM_INDEX(index) == srbkt_idx) {
            if (map_ent_ofs) {
                dst_ent_ofs = map_ent_ofs[ENT_OFS_FROM_IDX(index)];
            } else {
                dst_ent_ofs = ENT_OFS_FROM_IDX(index);
            }
            curr->index = ALPM_INDEX_ENCODE_VER1(drbkt_idx, dst_ent_ofs);
            LOG_D((BSL_META("[%d -> %d]"), index, curr->index));
        }
    SHR_DQ_TRAVERSE_END(&bkt->list, elem);
    LOG_D((BSL_META("\n")));

    SHR_FUNC_EXIT();
}


/*!
 * \brief Update the free bank for an existing group.
 *
 * Also, update the postion in the usage list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Target group
 * \param [in] diff Number diff of free banks, negative if shrink else postive.
 *
 * \return nothing.
 */
static inline void
ver1_rbkt_group_free_bank_update(int u, int m, int db, int ln, rbkt_group_t group,
                                 int diff)
{
    int free_count;

    RBKT_FBANKS(u, m, db, ln, group) += diff;
    free_count = RBKT_FBANKS(u, m, db, ln, group);
    ALPM_ASSERT(free_count >= 0);
    SHR_DQ_REMOVE(&RBKT_USG_NODE(u, m, db, ln, group));
    free_count = ALPM_MIN(BKT_HDL_MAX_BANKS(u, m, db, ln), free_count);
    SHR_DQ_INSERT_HEAD(&GROUP_USAGE_LIST(u, m, db, ln, free_count),
                       &RBKT_USG_NODE(u, m, db, ln, group));
}

/*!
 * \brief Set free bank for a newly create existing group.
 *
 * Also, add into the usage list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Target group
 * \param [in] free_count Number of free banks.
 *
 * \return nothing.
 */
static inline void
ver1_rbkt_group_free_bank_set(int u, int m, int db, int ln, rbkt_group_t group,
                              uint32_t free_count)
{
    RBKT_FBANKS(u, m, db, ln, group) = free_count;
    free_count = ALPM_MIN(BKT_HDL_MAX_BANKS(u, m, db, ln), free_count);
    SHR_DQ_INSERT_HEAD(&GROUP_USAGE_LIST(u, m, db, ln, free_count),
                       &RBKT_USG_NODE(u, m, db, ln, group));
}

/*!
 * \brief Reset free banks for an existing group.
 *
 * Also, delete from the usage list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Target group
 *
 * \return nothing.
 */
static inline void
ver1_rbkt_group_free_bank_clr(int u, int m, int db, int ln, rbkt_group_t group)
{
    RBKT_FBANKS(u, m, db, ln, group) = 0;
    SHR_DQ_REMOVE(&RBKT_USG_NODE(u, m, db, ln, group));
}

/*!
 * \brief Expand the existing group by one rbkt left.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] prev_group Previous group with at least one free rbkt
 * \param [in] group Group to expand.
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_expand_left(int u, int m, int db, int ln,
                      rbkt_group_t prev_group,
                      rbkt_group_t group)
{
    alpm_pivot_t *pivot;
    rbkt_idx_t rbkt = group - 1;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(RBKT_IS_HEAD(u, m, db, ln, group) == HEAD);
    /*     ALPM_ASSERT(RBKT_IS_HEAD(u, m, db, ln, rbkt) == BODY); */
    ALPM_ASSERT(RBKT_EBMP(u, m, db, ln, rbkt) == 0);

    /* Setup shrinking group (prev) */
    ver1_rbkt_group_free_bank_update(u, m, db, ln, prev_group, -1);

    /* Setup new start (curr) */
    RBKT_VBANKS(u, m, db, ln, rbkt) = RBKT_VBANKS(u, m, db, ln, group) + 1;
    if (rbkt != prev_group) {
        ver1_rbkt_group_free_bank_set(u, m, db, ln, rbkt,
                                      RBKT_FBANKS(u, m, db, ln, group));
    }
    RBKT_FORMAT(u, m, db, ln, rbkt) = DEFAULT_FORMAT;
    VER1_RBKT_PIVOT(u, m, db, ln, rbkt) = VER1_RBKT_PIVOT(u, m, db, ln, group);
    if (rbkt == FIRST_GROUP) {
        /* Do not swap if new head is group 1, as it's already in */
        SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, m, db, ln, group));
    } else {
        SHR_DQ_SWAP(&RBKT_LIST_NODE(u, m, db, ln, group),
                    &RBKT_LIST_NODE(u, m, db, ln, rbkt));
    }
    RBKT_IS_HEAD(u, m, db, ln, rbkt) = RBKT_IS_HEAD(u, m, db, ln, group);
    pivot = VER1_RBKT_PIVOT(u, m, db, ln, rbkt);
    /* Update new group to the pivot */
    pivot->bkt->log_bkt = VER1_LOG_BKT(rbkt);
    ALPM_ASSERT(rbkt != RESERVED_GROUP);
    ALPM_ASSERT(RBKT_IS_HEAD(u, m, db, ln, rbkt) == HEAD);
    BKT_HDL_RBKTS_USED(u, m, db, ln)++;

    /* Setup old start (curr) */
    RBKT_VBANKS(u, m, db, ln, group) = 0;
    ver1_rbkt_group_free_bank_clr(u, m, db, ln, group);
    RBKT_IS_HEAD(u, m, db, ln, group) = BODY;
    VER1_RBKT_PIVOT(u, m, db, ln, group) = NULL;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Expand the existing group by one rbkt right.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to expand.
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_expand_right(int u, int m, int db, int ln, rbkt_group_t group)
{
    rbkt_idx_t rbkt = RBKT_VBANKS(u, m, db, ln, group) + group;
    SHR_FUNC_ENTER(u);

    RBKT_VBANKS(u, m, db, ln, group) += 1;
    BKT_HDL_RBKTS_USED(u, m, db, ln)++;
    RBKT_FORMAT(u, m, db, ln, rbkt) = DEFAULT_FORMAT;
    /* VER1_RBKT_PIVOT(u, m, db, ln, rbkt)  = VER1_RBKT_PIVOT(u, m, db, ln, group); */
    ver1_rbkt_group_free_bank_update(u, m, db, ln, group, -1);

    /* No need to update pivot for right expand */
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy a rbkt from one position to another
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] sbkt_idx Source rbkt index
 * \param [in] dbkt_idx Destination rbkt index
 * \param [in] update_index Migrate the index
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */

static int
ver1_rbkt_copy(int u, int m, int db, int ln,
               rbkt_idx_t srbkt_idx,
               rbkt_idx_t drbkt_idx,
               bool update_index,
               bool write_hw)
{
    rbkt_t *src_rbkt, *dst_rbkt;
    alpm_pivot_t *pivot;
    base_buckets_ver1_t *bbkts = NULL;

    SHR_FUNC_ENTER(u);

    src_rbkt = &RBKT(u, m, db, ln, srbkt_idx);
    dst_rbkt = &RBKT(u, m, db, ln, drbkt_idx);

    if (write_hw) {
        int shw_idx, dhw_idx;
        shw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, srbkt_idx);
        dhw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, drbkt_idx);

        bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
        if (bbkts == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, shw_idx, &bbkts->rbuf[0][0],
                                       &bbkts->dt_bitmap[0]));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, dhw_idx, &bbkts->rbuf[0][0],
                                        HIT_OP_AUTO,
                                        src_rbkt->entry_bitmap,
                                        bbkts->dt_bitmap[0]));
    }

    dst_rbkt->entry_bitmap = src_rbkt->entry_bitmap;
    dst_rbkt->format = src_rbkt->format;
    /* dst_rbkt->pivot  = src_rbkt->pivot; */
    /* ALPM_ASSERT(dst_rbkt->pivot); */
    src_rbkt->format = DISABLED_FORMAT; /* Clear it. */

    if (update_index) {
        pivot = ver1_rbkt_pivot_get(u, m, db, ln, drbkt_idx);
        ALPM_ASSERT(pivot);
        SHR_IF_ERR_EXIT(
            ver1_rbkt_entry_index_migrate(u, m, db, ln, pivot->bkt,
                                          srbkt_idx,
                                          drbkt_idx,
                                          NULL));
    }
exit:
    if (bbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], bbkts);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a rbkt for existing group.
 *
 * Basically, this is to shrink an existing group by one rbkt.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to shrink.
 * \param [in] rbkt Rbkt to be freed.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_rbkt_free(int u, int m, int db, int ln,
               rbkt_group_t group,
               rbkt_idx_t rbkt_idx)
{
    SHR_FUNC_ENTER(u);

    RBKT_VBANKS(u, m, db, ln, group)--;
    BKT_HDL_RBKTS_USED(u, m, db, ln)--;
    ver1_rbkt_group_free_bank_update(u, m, db, ln, group, +1);
    /* Let it changed. */
    RBKT_FORMAT(u, m, db, ln, rbkt_idx) = DISABLED_FORMAT;
    /*  don't reset this for ver1_rbkt_free, since this is head property
     * VER1_RBKT_PIVOT(u, m, db, ln, rbkt_idx) = NULL;
     */
    RBKT_EBMP(u, m, db, ln, rbkt_idx) = 0;
    /*  RBKT_IS_HEAD(u, m, db, ln, rbkt_idx) = BODY; */
    /* ln_stats_incr(c_bankfree); */

    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear a rbkt in HW, and recycle SW resoures.
 *
 * Do not reset the format, as that is not necessary.
 * And do not bother taking care of group vbanks.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] size Requesting size for new group
 * \param [out] free_group Group with free banks of requesting size
 *
 * \return SHR_E_NONE only.
 */
static inline int
ver1_rbkt_clear(int u, int m, int db, int ln, int rbkt_idx)
{
    /*    RBKT_FORMAT(u, m, db, ln, rbkt_idx) = DISABLED_FORMAT; */
    RBKT_EBMP(u, m, db, ln, rbkt_idx) = 0;
    /*
     * Also don't reset for same reason as ver1_rbkt_free.
     * VER1_RBKT_PIVOT(u, m, db, ln, rbkt_idx) = NULL;
     */
    return SHR_E_NONE;
}

/*!
 * \brief Merge 2 rbkts within same group into one.
 *
 * More options: 3->2.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_rbkt_idx Source rbkt index
 * \param [in] dst_rbkt_idx Destination rbkt index
 * \param [in] group Rbkt group for both src and dst.
 * \param [in] try_hard Merge rbkts with different format if true.
 * \param [out] merged True if merge happens
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */

static int
ver1_rbkt_merge(int u, int m, int db, int ln,
                rbkt_idx_t srbkt_idx,
                rbkt_idx_t drbkt_idx,
                rbkt_group_t group,
                bool try_hard,
                bool *merged,
                base_buckets_ver1_t *in_bbkts)
{
    int i, j;
    rbkt_t *src_rbkt, *dst_rbkt;
    format_type_t format_type;
    int src_prefix_len, src_full, src_total;
    int dst_prefix_len, dst_full, dst_total;
    bool ok_to_merge = FALSE;
    int src_num, dst_num;
    alpm_pivot_t *pvt;
    int ent_ofs_map[RBKT_MAX_ENTRIES_VER1];
    uint32_t *sbuf, *dbuf;
    int dst_bank, src_bank;
    alpm_data_type_t dt = 0;

    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(srbkt_idx != drbkt_idx);
    src_rbkt = &RBKT(u, m, db, ln, srbkt_idx);
    dst_rbkt = &RBKT(u, m, db, ln, drbkt_idx);

    if (src_rbkt->entry_bitmap == 0 ||
        dst_rbkt->entry_bitmap == 0) {
        /* Do not merge empty rbkts, leave them the last thing to do */
        SHR_EXIT();
    }
    ALPM_ASSERT(srbkt_idx > drbkt_idx);
    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                          dst_rbkt->format,
                                          &dst_prefix_len,
                                          &dst_full,
                                          &dst_total));
    if (try_hard) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                              src_rbkt->format,
                                              &src_prefix_len,
                                              &src_full,
                                              &src_total));
        if (dst_full == src_full && dst_prefix_len >= src_prefix_len) {
            ok_to_merge = true;
        }
    } else if (src_rbkt->format == dst_rbkt->format) {
        src_total = dst_total;
        ok_to_merge = true;
    }
    if (ok_to_merge) {
        src_num = shr_util_popcount(RBKT_EBMP(u, m, db, ln, srbkt_idx));
        dst_num = shr_util_popcount(RBKT_EBMP(u, m, db, ln, drbkt_idx));
        /* Do merge only if can save a rbkt => merged = 1. */
        ok_to_merge = (src_num + dst_num <= dst_total);
    }

    if (!ok_to_merge) {
        SHR_EXIT();
    }

    *merged = TRUE;

    /* Do merge 2 -> 1 */
    dst_bank = drbkt_idx - group;
    src_bank = srbkt_idx - group;
    sbuf = &in_bbkts->rbuf[srbkt_idx - group][0];
    dbuf = &in_bbkts->rbuf[dst_bank][0];
    in_bbkts->rbkt_ofs[dst_bank] = dst_bank;
    in_bbkts->rbkt_ofs[srbkt_idx - group] = -1;

    j = 0;
    pvt = VER1_RBKT_PIVOT(u, m, db, ln, group);
    for (i = 0; i < dst_total; i++) {
        if (ALPM_BIT_GET(dst_rbkt->entry_bitmap, i)) {
            continue;
        }
        /* Don't reset j for performance */
        while (j < src_total) {
            if (ALPM_BIT_GET(src_rbkt->entry_bitmap, j)) {
                if (format_type == FORMAT_ROUTE) {
                    if (dst_full) {
                        dt = ver1_dt_bitmap_get(u, m, j,
                                                in_bbkts->dt_bitmap[src_bank]);
                    }
                } else {
                    dt = bcmptm_rm_alpm_mid_dt_get(u, m, db, pvt->key.t.app);
                }
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_ln_base_entry_move(u, m, format_type,
                                                      src_rbkt->format,
                                                      j,
                                                      &pvt->key.t,
                                                      sbuf,
                                                      dt,

                                                      dst_rbkt->format,
                                                      i,
                                                      &pvt->key.t,
                                                      dbuf));
                ALPM_BIT_SET(in_bbkts->modify_bitmap[dst_bank], i);
                ALPM_BIT_SET(dst_rbkt->entry_bitmap, i);
                if (format_type == FORMAT_ROUTE) {
                    if (dst_full) {
                        ver1_dt_bitmap_set(u, m, i, &in_bbkts->dt_bitmap[dst_bank], dt);
                    }
                    /* No changes on actual_data_type even with try_hard set. */
                }
                ent_ofs_map[j] = i;
                j++;
                break; /* This dst entry is done, avdance to next */
            }
            j++;
        }
    }
    /* Source rbkt should be empty now */
    src_rbkt->entry_bitmap = 0;
    ALPM_ASSERT(src_num + dst_num == shr_util_popcount(dst_rbkt->entry_bitmap));

    SHR_IF_ERR_EXIT(
        ver1_rbkt_free(u, m, db, ln, group, srbkt_idx));
    /* $ Entry index migration */
    SHR_IF_ERR_EXIT(
        ver1_rbkt_entry_index_migrate(u, m, db, ln,
                                      VER1_RBKT_PIVOT(u, m, db, ln, group)->bkt,
                                      srbkt_idx,
                                      drbkt_idx,
                                      ent_ofs_map));

exit:
    SHR_FUNC_EXIT();
}

#ifdef ALPM_DEBUG
static void
ver1_rbkt_group_sanity(int u, int m, int db, int ln, rbkt_group_t group,
                       bool with_holes, int vbanks)
{
    rbkt_t *rbkt_group, *rbkt;
    bkt_hdl_t *bkt_hdl;
    shr_dq_t *blist, *elem;
    alpm_bkt_elem_t *curr;
    int bkt_count[ALPM_MAX_BANKS] = {0};
    rbkt_idx_t rbkt_idx;
    format_type_t format_type;
    int fmt_len;
    int num_entry;
    int count = 0, i;
    alpm_pivot_t *pivot = NULL;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }
    ver1_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    if (group == INVALID_GROUP) {
        return;
    }
    assert(group != RESERVED_GROUP);

    rbkt_group = &bkt_hdl->rbkt_array[group];

    assert(rbkt_group->valid_banks <= bkt_hdl->max_banks);
    if (!with_holes) {
        vbanks = rbkt_group->valid_banks;
    }

    for (i = 0; i < vbanks; i++) {
        rbkt = rbkt_group + i;
        if (i == 0) {
            assert(rbkt->is_head);
            pivot = rbkt->pivot;
            assert(pivot && pivot->bkt && pivot->bkt->bpm);
            assert(VER1_GROUP(pivot->bkt->log_bkt) == rbkt_group->index);
        } else {
            assert(!rbkt->is_head);
            assert(!rbkt->pivot);
        }
        if (!with_holes) {
            assert(rbkt->entry_bitmap);
        }
        count += shr_util_popcount(rbkt->entry_bitmap);
    }
    assert(!pivot ||
           bcmptm_rm_alpm_trie_count(pivot->bkt->trie) == count ||
           bcmptm_rm_alpm_trie_count(pivot->bkt->trie) == (count + 1));

    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    if (!rbkt_group->pivot) {
        return;
    }
    blist = &rbkt_group->pivot->bkt->list;
    SHR_DQ_TRAVERSE(blist, elem)
        curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        if (curr->index == INVALID_INDEX) {
            continue;
        }
        rbkt_idx = RBKT_FROM_INDEX(curr->index);
        rbkt = &bkt_hdl->rbkt_array[rbkt_idx];
        bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type, rbkt->format,
                                          &fmt_len, NULL, NULL);
        assert(curr->key.t.len - rbkt_group->pivot->key.t.len <= fmt_len);
        bkt_count[rbkt_idx - group]++;
    SHR_DQ_TRAVERSE_END(blist, elem);

    for (i = 0; i < rbkt_group->valid_banks; i++) {
        rbkt = &bkt_hdl->rbkt_array[group + i];
        bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type, rbkt->format,
                                          NULL, NULL, &num_entry);
        assert(bkt_count[i] <= num_entry);
        assert(bkt_count[i] == shr_util_popcount(rbkt->entry_bitmap));
    }
}
#endif

/*!
 * \brief Compress a group contains scattered rbkts with sparse entries.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Rbkt group to compress.
 * \param [in] vbanks Vbanks including holes
 * \param [out] format_update True if update pivot format, eles false.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_rbkt_group_compress(int u, int m, int db, int ln,
                         rbkt_group_t group,
                         int vbanks,
                         bool *format_update,
                         base_buckets_ver1_t *sbbkts)
{
    rbkt_idx_t src_rbkt, dst_rbkt;
    int j, i;
    bool merged;
    SHR_FUNC_ENTER(u);

    VER1_RBKT_GROUP_SANITY(u, m, db, ln, group, true, vbanks);

    /* Backward traverse. */
    j = 0;
    for (i = vbanks - 1; i > 0; i--) {
        src_rbkt = group + i;
        if (!RBKT_EBMP(u, m, db, ln, src_rbkt)) {
            continue;
        }
        for (j = 0; j < i; j++) {
            dst_rbkt = group + j;
            if (!RBKT_EBMP(u, m, db, ln, dst_rbkt)) {
                continue;
            }
            merged = false;
            SHR_IF_ERR_EXIT(
                ver1_rbkt_merge(u, m, db, ln, src_rbkt, dst_rbkt, group, TRUE,
                                &merged, sbbkts));
            if (merged) { /* src_rbkt is gone, advance to next  */
                *format_update = true;
                break;
            }
        }
    }

    /* Backward traverse. */
    j = 0;
    for (i = vbanks - 1; i > j; i--) {
        src_rbkt = group + i;
        if (!RBKT_EBMP(u, m, db, ln, src_rbkt)) {
            continue;
        }
        while (j < i) {
            dst_rbkt = group + j;
            if (!RBKT_EBMP(u, m, db, ln, dst_rbkt)) {
                SHR_IF_ERR_EXIT(
                    ver1_rbkt_copy(u, m, db, ln, src_rbkt, dst_rbkt,
                                   true, false));
                sbbkts->rbkt_ofs[dst_rbkt - group] = src_rbkt - group;
                sbbkts->rbkt_ofs[src_rbkt - group] = -1; /* Raw bucket is cleared */
                sbbkts->modify_bitmap[dst_rbkt - group] =
                    sbbkts->modify_bitmap[src_rbkt - group]; /* Copy */
                sbbkts->modify_bitmap[src_rbkt - group] = 0; /* Clear*/
                j++;
                *format_update = true; /* Active format changed. */
                break; /* Advance to next dst_rbkt */
            }
            j++;
            /*
             * Rbkts just moved, not freed thus don't call ver1_rbkt_free.
             * Original rbkts' will be cleared (ver1_rbkt_clear) after
             * pivot update.
             */
        }
    }

    /*
     * Right here, if new entry belongs to parent pivot, it is
     * to be added later.
     * VER1_RBKT_GROUP_SANITY(u, m, db, ln, group, false, 0);
     */
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Split a new group from the free group.
 *
 * The target group should be with enough free rbkts, thus no shuffle involved.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] free_group Free group
 * \param [in] size Requesting size for new group
 * \param [out] new_group New group split
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_group_split(int u, int m, int db, int ln,
                      rbkt_group_t free_group,
                      uint32_t size,
                      rbkt_group_t *new_group)
{
    rbkt_t *free_rbkt;
    rbkt_t *new_rbkt;
    SHR_FUNC_ENTER(u);

    free_rbkt = &RBKT(u, m, db, ln, free_group);
    /* If no valid banks, usually it is group[0], or group[1].
     * It can be others, as we may have pivots with assigned group with
     * all free rbkts. This is the case when we cannot free that pivot (usually
     * level-2), because otherwise we may endup with ill insert (find wrong
     * second level pivot).
     */
#if 0
    ALPM_ASSERT(free_rbkt->valid_banks ||
                free_rbkt->index == RESERVED_GROUP ||
                free_rbkt->index == FIRST_GROUP);
#endif
    ALPM_ASSERT(size <= free_rbkt->free_banks);

    /* Setup split group */
    new_rbkt = &RBKT(u, m, db, ln, (free_rbkt->index +
                                 free_rbkt->valid_banks +
                                 free_rbkt->free_banks - size));
    /* Group 0 is reserved to all pivots with no bucket */
    ALPM_ASSERT(new_rbkt->index != RESERVED_GROUP);

    if (new_rbkt->index != free_rbkt->index) {
        new_rbkt->is_head = HEAD;
        ver1_rbkt_group_free_bank_set(u, m, db, ln, new_rbkt->index, size);

        /* Setup original group */
        ver1_rbkt_group_free_bank_update(u, m, db, ln, free_rbkt->index, -size);

        /* Link the new and free2 */
        SHR_DQ_INSERT_NEXT(&free_rbkt->listnode, &new_rbkt->listnode);
        BKT_HDL_GROUPS_USED(u, m, db, ln)++;
    } else {
        /* Else take all, and this group is already in the list */
        alpm_pivot_t *original_pivot;
        original_pivot = ver1_rbkt_pivot_get(u, m, db, ln, free_rbkt->index);
        if (original_pivot) {
            /* This group is now officially taken away */
            original_pivot->bkt->log_bkt = INVALID_LOG_BKT;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln),
                                            original_pivot,
                                            NULL, UPDATE_ALL));
            ALPM_ASSERT(original_pivot->bkt->trie->trie == NULL);
        } else {
            ALPM_ASSERT(free_rbkt->index == FIRST_GROUP);
        }
    }

    *new_group = new_rbkt->index;
    ALPM_ASSERT(*new_group != RESERVED_GROUP);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Fuse an existing group with its previous group.
 *
 * Assuming the group is not used any more (empty or others).
 * Merge the resources into previous group. Reset everything
 * in the group, and remove it from the list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to be fused
 * \param [out] free_group Group absorbing all resources of target group
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_group_fuse(int u, int m, int db, int ln,
                     rbkt_group_t group,
                     rbkt_group_t *free_group)
{
    int i;
    int prev_group;
    SHR_FUNC_ENTER(u);

    /* Cannot fuse for the first group */
    ALPM_ASSERT(group > FIRST_GROUP || group != INVALID_GROUP);
    if (group <= FIRST_GROUP) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    prev_group = prev_group_get(u, m, db, ln, group);
    ver1_rbkt_group_free_bank_update(u, m, db, ln, prev_group,
                               RBKT_FBANKS(u, m, db, ln, group) +
                               RBKT_VBANKS(u, m, db, ln, group));

    for (i = 1; i < RBKT_VBANKS(u, m, db, ln, group); i++) {
        rbkt_idx_t rbkt_idx = group + i;
        RBKT_FORMAT(u, m, db, ln, rbkt_idx) = DISABLED_FORMAT;
        RBKT_EBMP(u, m, db, ln, rbkt_idx) = 0;
    }
    RBKT_VBANKS(u, m, db, ln, group) = 0;
    ver1_rbkt_group_free_bank_clr(u, m, db, ln, group);
    RBKT_IS_HEAD(u, m, db, ln, group) = BODY;
    VER1_RBKT_PIVOT(u, m, db, ln, group) = NULL;
    SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, m, db, ln, group));
    BKT_HDL_GROUPS_USED(u, m, db, ln)--;

    if (free_group) {
        *free_group = prev_group;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Move entrie group to the destination location
 *
 * The src and dst locations are guaranteed not to be overlapped.
 * We only move the valid banks in the move group, and leave those free banks.
 * When fuse is set, the src group is fused with its previous group (if exists),
 * and the free group is the fused one (previous group).
 * When fuse is not set, the src group simply becomes an empty group, and the
 * free group is itself.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Group to be moved
 * \param [in] dst_group Group to spare *all* free rbkts to src_group.
 * \param [out] new_group New group after src_group moved.
 * \param [in] fuse To fuse the src_group with its previous group.
 * \param [out] free_group Group with free banks increased.
 *
 * \return SHR_E_NONE, SHR_E_XXX.
 */
static int
ver1_rbkt_group_move(int u, int m, int db, int ln,
                     rbkt_group_t src_group,
                     rbkt_group_t dst_group,
                     rbkt_group_t *new_group,
                     bool fuse,
                     rbkt_group_t *free_group)
{
    uint32_t i, vbanks, fbanks;
    alpm_pivot_t *pivot = VER1_RBKT_PIVOT(u, m, db, ln, src_group);
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(pivot->bkt);
    ALPM_ASSERT(pivot->bkt->log_bkt == VER1_LOG_BKT(src_group));
    ALPM_ASSERT(RBKT_IS_HEAD(u, m, db, ln, src_group) == HEAD);
    ALPM_ASSERT(RBKT_IS_HEAD(u, m, db, ln, dst_group) == HEAD);
    /* Dst group must be with enough free banks to hold src group */
    ALPM_ASSERT(RBKT_FBANKS(u, m, db, ln, dst_group) >=
                RBKT_VBANKS(u, m, db, ln, src_group));
    fbanks = RBKT_FBANKS(u, m, db, ln, dst_group);

    /* $ Give away all the free banks of dst_group */
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_split(u, m, db, ln, dst_group,
                              RBKT_FBANKS(u, m, db, ln, dst_group),
                              new_group));
    ALPM_ASSERT(*new_group ==
                RBKT_VBANKS(u, m, db, ln, dst_group) + dst_group);

    /* $ Update new group to the pivot before pivot update */
    pivot->bkt->log_bkt = VER1_LOG_BKT(*new_group);
    VER1_RBKT_PIVOT(u, m, db, ln, *new_group) = pivot;
    vbanks = RBKT_VBANKS(u, m, db, ln, src_group);
    ALPM_ASSERT(vbanks); /* Why move empty group */
    RBKT_VBANKS(u, m, db, ln, *new_group) = vbanks;
    /* During group_split, new_group has been given the free banks */
    ALPM_ASSERT(RBKT_FBANKS(u, m, db, ln, *new_group) == fbanks);
    ver1_rbkt_group_free_bank_update(u, m, db, ln, *new_group, -vbanks);

    /* $ Copy bucket */
    for (i = 0; i < vbanks; i++) {
        /* Clear old group after pivot updated.
         * Update the entry_index.
         * ver1_rbkt_copy need pivot set in place for new group.
         */
        SHR_IF_ERR_EXIT(
            ver1_rbkt_copy(u, m, db, ln, src_group + i, *new_group + i, TRUE, true));
    }

    /* $ Update PIVOT */
    ALPM_ASSERT(pivot);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln), pivot, NULL,
                                    UPDATE_ALL));

    /* $ Invalidate old rbkts */
    for (i = 0; i < vbanks; i++) {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_clear(u, m, db, ln, src_group + i));
    }
    /* $ The recycle for original postion of move group. */
    /* The move group does not get deleted, it simply becomes empty! */
    if (fuse) {
        if (src_group != FIRST_GROUP) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_fuse(u, m, db, ln, src_group, free_group));
        } else {
            /* All groups except group 1 always have valid banks */
            vbanks = RBKT_VBANKS(u, m, db, ln, src_group);
            for (i = 1; i < vbanks; i++) {
                rbkt_idx_t rbkt_idx = src_group + i;
                RBKT_FORMAT(u, m, db, ln, rbkt_idx) = DISABLED_FORMAT;
                RBKT_EBMP(u, m, db, ln, rbkt_idx) = 0;
            }
            ver1_rbkt_group_free_bank_update(u, m, db, ln, src_group, vbanks);
            RBKT_VBANKS(u, m, db, ln, src_group) = 0;
            VER1_RBKT_PIVOT(u, m, db, ln, src_group) = NULL;
            /*
             * Don't do the following, as we never free group0.
             * RBKT_IS_HEAD(u, m, db, ln, src_group) = BODY;
             * SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, m, db, ln, src_group));
             */
            if (free_group) {
                *free_group = src_group;
            }
        }
    } else if (dst_group < src_group) { /* For append */
        /*
         * This is where src_group and dst_group are adjcent two groups,
         * and the dst_group < src_group.
         * This is assured by caller, and guarded by the following check.
         */
        ALPM_ASSERT(*new_group + (int)fbanks == src_group);
        vbanks = RBKT_VBANKS(u, m, db, ln, src_group);
        fbanks = RBKT_FBANKS(u, m, db, ln, src_group);
        for (i = 1; i < vbanks; i++) {
            rbkt_idx_t rbkt_idx = src_group + i;
            RBKT_FORMAT(u, m, db, ln, rbkt_idx) = DISABLED_FORMAT;
            RBKT_EBMP(u, m, db, ln, rbkt_idx) = 0;
        }
        ver1_rbkt_group_free_bank_update(u, m, db, ln, *new_group, vbanks + fbanks);
        do {
            RBKT_VBANKS(u, m, db, ln, src_group) = 0;
            ver1_rbkt_group_free_bank_clr(u, m, db, ln, src_group);
            RBKT_IS_HEAD(u, m, db, ln, src_group) = BODY;
            VER1_RBKT_PIVOT(u, m, db, ln, src_group) = NULL;
            SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, m, db, ln, src_group));
            BKT_HDL_GROUPS_USED(u, m, db, ln)--;
        } while (0);
        if (free_group) {
            *free_group = *new_group;
        }
    } else {
        /* Not possible, by design. */
        ALPM_ASSERT(0);
    }

    /* group_move++; */

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Move entrie group to append the previous group.
 *  Dest____|Src    (before)
 *  DestSrc_|___    (after)
 *
 * Append current group to previous group.
 * It is possible that after being appended the rbkt is overlapped with
 * itself before being appended.
 * After appended, there is no new group allocated, only the moved group
 * will be with free fbanks.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] curr_group Group to be moved
 * \param [out] free_group Group with free banks
 *
 * \return SHR_E_NONE, SHR_E_XXX.
 */
static int
ver1_rbkt_group_append(int u, int m, int db, int ln,
                       rbkt_group_t curr_group,
                       rbkt_group_t *free_group
                      )
{
    int vbanks, fbanks;
    rbkt_idx_t srbkt_idx, drbkt_idx;
    alpm_pivot_t *pivot = VER1_RBKT_PIVOT(u, m, db, ln, curr_group);
    rbkt_group_t prev_group;

    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(curr_group > FIRST_GROUP || curr_group != INVALID_GROUP);
    if (curr_group <= FIRST_GROUP) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    prev_group = prev_group_get(u, m, db, ln, curr_group);
    ALPM_ASSERT(RBKT_FBANKS(u, m, db, ln, prev_group)); /* Otherwise why append. */
    if (RBKT_FBANKS(u, m, db, ln, prev_group) >=
        RBKT_VBANKS(u, m, db, ln, curr_group)) {
        rbkt_group_t new_group;
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_move(u, m, db, ln,
                                 curr_group,
                                 prev_group,
                                 &new_group,
                                 FALSE,
                                 free_group));
        ALPM_ASSERT(new_group == *free_group);
        SHR_EXIT();
    }

    fbanks = RBKT_FBANKS(u, m, db, ln, prev_group);
    /* $ Give away all the free banks of prev_group */
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_split(u, m, db, ln, prev_group,
                              fbanks, free_group));
    ALPM_ASSERT(*free_group ==
                RBKT_VBANKS(u, m, db, ln, prev_group) + prev_group);
    ALPM_ASSERT(*free_group + fbanks == curr_group);

    /* $ Update new group to the pivot before pivot update */
    pivot->bkt->log_bkt = VER1_LOG_BKT(*free_group);
    VER1_RBKT_PIVOT(u, m, db, ln, *free_group) = pivot;
    vbanks = RBKT_VBANKS(u, m, db, ln, curr_group);
    ALPM_ASSERT(vbanks); /* Why move empty group */
    RBKT_VBANKS(u, m, db, ln, *free_group) = vbanks;

    /* $ Copy bucket */
    drbkt_idx = *free_group;
    srbkt_idx = curr_group + vbanks - 1;
    while (drbkt_idx < curr_group && curr_group <= srbkt_idx) {
        /* Clear old group after pivot updated.
         * Update the entry_index.
         * ver1_rbkt_copy need pivot set in place for new group.
         */
        SHR_IF_ERR_EXIT(
            ver1_rbkt_copy(u, m, db, ln, srbkt_idx, drbkt_idx, TRUE, true));
        srbkt_idx--;
        drbkt_idx++;
    }

    /* $ Update PIVOT */
    ALPM_ASSERT(pivot);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln), pivot, NULL,
                                    UPDATE_ALL));

    /* $ Invalidate old rbkts */
    while (++srbkt_idx < curr_group + vbanks) {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_clear(u, m, db, ln, srbkt_idx));

        RBKT_FORMAT(u, m, db, ln, srbkt_idx) = DISABLED_FORMAT;
    }

    ver1_rbkt_group_free_bank_update(u, m, db, ln, *free_group,
                                     RBKT_FBANKS(u, m, db, ln, curr_group));
    do {
    RBKT_VBANKS(u, m, db, ln, curr_group) = 0;
    ver1_rbkt_group_free_bank_clr(u, m, db, ln, curr_group);
    RBKT_IS_HEAD(u, m, db, ln, curr_group) = BODY;
    VER1_RBKT_PIVOT(u, m, db, ln, curr_group) = NULL;
    SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, m, db, ln, curr_group));
    } while (0);
    BKT_HDL_GROUPS_USED(u, m, db, ln)--;

exit:
    bcmptm_rm_alpm_internals[u][m][db][ln].rbkt_append++;
    SHR_FUNC_EXIT();
}

/*!
 * \brief Move entrie group to prepend the next group.
 *
 * Prepend current group to next group.
 * It is possible that after being prepended the rbkt is overlapped with
 * itself before being prepended.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] curr_group Group to be moved
 * \param [out] free_group Group with free banks
 *
 * \return SHR_E_NONE, SHR_E_XXX.
 */
static int
ver1_rbkt_group_prepend(int u, int m, int db, int ln,
                        rbkt_group_t curr_group,
                        rbkt_group_t *free_group
                       )
{
    int vbanks, tmp_idx;
    rbkt_idx_t srbkt_idx, drbkt_idx;
    alpm_pivot_t *pivot = VER1_RBKT_PIVOT(u, m, db, ln, curr_group);
    rbkt_group_t prev_group;
    rbkt_group_t new_group;

    SHR_FUNC_ENTER(u);
    ALPM_ASSERT(RBKT_FBANKS(u, m, db, ln, curr_group)); /* Otherwise why prepend. */

    ALPM_ASSERT(curr_group > FIRST_GROUP || curr_group != INVALID_GROUP);
    prev_group = prev_group_get(u, m, db, ln, curr_group);
    if (prev_group == INVALID_GROUP) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    /* $ Update new group to the pivot before pivot update */
    new_group = curr_group + RBKT_FBANKS(u, m, db, ln, curr_group);

    ALPM_ASSERT(new_group != RESERVED_GROUP);
    pivot->bkt->log_bkt = VER1_LOG_BKT(new_group);
    VER1_RBKT_PIVOT(u, m, db, ln, new_group) = pivot;
    vbanks = RBKT_VBANKS(u, m, db, ln, curr_group);
    ALPM_ASSERT(vbanks); /* Why move empty group */
    RBKT_VBANKS(u, m, db, ln, new_group) = vbanks;

    /* $ Copy bucket */
    drbkt_idx = curr_group +
                RBKT_VBANKS(u, m, db, ln, curr_group) +
                RBKT_FBANKS(u, m, db, ln, curr_group) - 1;
    srbkt_idx = curr_group;
    tmp_idx = curr_group + RBKT_VBANKS(u, m, db, ln, curr_group);
    while (srbkt_idx  < tmp_idx && tmp_idx <= drbkt_idx) {
        /* Clear old group after pivot updated.
         * Update the entry_index.
         * ver1_rbkt_copy need pivot set in place.
         * For now, the pivot can be found, as we only push vbanks to tail of
         * a given group.
         */
        SHR_IF_ERR_EXIT(
            ver1_rbkt_copy(u, m, db, ln, srbkt_idx, drbkt_idx, TRUE, true));
        srbkt_idx++;
        drbkt_idx--;
    }

    /* $ Update PIVOT */
    ALPM_ASSERT(pivot);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln), pivot, NULL,
                                    UPDATE_ALL));

    /* $ Invalidate old rbkts */
    while (--srbkt_idx >= curr_group) {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_clear(u, m, db, ln, srbkt_idx));

        RBKT_FORMAT(u, m, db, ln, srbkt_idx) = DISABLED_FORMAT;
    }

    do {
    rbkt_t *curr_rbkt, *new_rbkt;
    /* Setup split group */
    new_rbkt = &RBKT(u, m, db, ln, new_group);
    curr_rbkt = &RBKT(u, m, db, ln, curr_group);
    new_rbkt->is_head = HEAD;
    ver1_rbkt_group_free_bank_set(u, m, db, ln, new_group, 0);
    ver1_rbkt_group_free_bank_update(u, m, db, ln, prev_group,
                                     RBKT_FBANKS(u, m, db, ln, curr_group));

    /* Link the new and free2 */
    SHR_DQ_INSERT_NEXT(&curr_rbkt->listnode, &new_rbkt->listnode);

    curr_rbkt->valid_banks = 0;
    ver1_rbkt_group_free_bank_clr(u, m, db, ln, curr_group);
    curr_rbkt->is_head = BODY;
    curr_rbkt->pivot = NULL;
    SHR_DQ_REMOVE(&curr_rbkt->listnode);
    } while (0);

    *free_group = prev_group;
exit:
    bcmptm_rm_alpm_internals[u][m][db][ln].rbkt_prepend++;
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find start and stop groups to do ultimate shuffle.
 *
 * A rough traverse range can be provided to speed up the search process.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] start_group Travese start group
 * \param [in] stop_group Travese end group
 * \param [in] size Requesting size for new group
 * \param [out] group_a Defrag start group
 * \param [out] group_c Defrag stop group
 * \param [out] collect_groups Groups between group_a and group c.
 *
 * \return SHR_E_NONE/SHR_E_RESOURCE.
 */
static int
ver1_rbkt_group_shuffle_range(int u, int m, int db, int ln,
                              rbkt_group_t start_group,
                              rbkt_group_t stop_group,
                              uint8_t size,
                              rbkt_group_t *group_a,
                              rbkt_group_t *group_c,
                              int *collect_groups)
{
    shr_dq_t *start;
    shr_dq_t *ielem, *elem;
    int slide_f_rbkts, slide_groups;
    rbkt_t *icurr, *ocurr;
    rbkt_group_t end_group;
    int min_groups = MAX_INT_VALUE;

    SHR_FUNC_ENTER(u);
    end_group = start_group;
    slide_f_rbkts = RBKT_FBANKS(u, m, db, ln, start_group);
    slide_groups = 1;
#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        cli_out("Shuffle traverse from group %6d to %6d size=%d free_rbkts=%5d ",
                start_group, stop_group, size,
                BKT_HDL_RBKTS_TOTAL(u, m, db, ln) - BKT_HDL_RBKTS_USED(u, m, db, ln));
    }
#endif
    SHR_DQ_TRAVERSE_FROM(&BKT_HDL_GROUP_LIST(u, m, db, ln),
                         &RBKT_LIST_NODE(u, m, db, ln, start_group), elem)
        ocurr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        ALPM_ASSERT(ocurr->is_head == HEAD);
        if (slide_f_rbkts < size && end_group != stop_group) {
            start = SHR_DQ_NEXT(&RBKT_LIST_NODE(u, m, db, ln, end_group), void *);
            SHR_DQ_TRAVERSE_FROM2(&BKT_HDL_GROUP_LIST(u, m, db, ln), start, ielem)
                icurr = SHR_DQ_ELEMENT_GET(rbkt_t*, ielem, listnode);
                ALPM_ASSERT(icurr->is_head == HEAD);
                slide_f_rbkts += icurr->free_banks;
                slide_groups++;
                /*
                 * Renew end_group so that next traverse will continue
                 * based on this new end_group.
                 */
                end_group = icurr->index;
                if (end_group == stop_group ||
                    slide_f_rbkts >= size) {
                    break;
                }
            SHR_DQ_TRAVERSE_END(list, ielem);
        }

        if (slide_f_rbkts >= size) {
            if (slide_groups < min_groups) {
                min_groups = slide_groups;
                if (min_groups < SLIDE_GROUPS_TRAVERSE_STOP) {
                    /* If force break, exclude preceding groups without freebanks */
                    start_group = ocurr->index;
                    SHR_DQ_TRAVERSE_FROM2(&BKT_HDL_GROUP_LIST(u, m, db, ln),
                                          &RBKT_LIST_NODE(u, m, db, ln, start_group), elem)
                        ocurr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
                        if (ocurr->free_banks) {
                            break;
                        } else {
                            min_groups--;
                        }
                    SHR_DQ_TRAVERSE_END(list, elem);
                    *group_a = ocurr->index;
                    *group_c = end_group;
                    break;
                } else {
                    *group_a = ocurr->index;
                    *group_c = end_group;
                }
            }
        }
        slide_f_rbkts -= ocurr->free_banks;
        slide_groups--;
    SHR_DQ_TRAVERSE_END(list, elem);

    ALPM_ASSERT(min_groups < MAX_INT_VALUE);
    ALPM_ASSERT(*group_a >= start_group &&
                (stop_group == INVALID_GROUP || *group_c <= stop_group));

    *collect_groups = min_groups;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Defragement existing groups to create a free group with requesting size.
 *
 * This is the last resort to create requesting free rbkts, and can be time
 * consuming.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group_a Shufflb start group
 * \param [in] group_c Shufflb end group
 * \param [in] groups Number of groups between a and c.
 * \param [in] size Requesting size for new group
 * \param [out] free_group Group with free banks of requesting size
 *
 * \return SHR_E_NONE/SHR_E_RESOURCE.
 */
static int
ver1_rbkt_group_shuffle_b(int u, int m, int db, int ln,
                          rbkt_group_t group_a,
                          rbkt_group_t group_c,
                          int groups,
                          uint8_t size,
                          rbkt_group_t *free_group)
{
    shr_dq_t *elem;
    rbkt_t *fcurr, *bcurr;
    int fwd_groups, fwd_groups2;
    shr_dq_t *start;
    SHR_FUNC_ENTER(u);

#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        alpm_cmd_ctrl_t *ctrl;
        alpm_info_t *alpm_info = bcmptm_rm_alpm_info_get(u, m);
        SHR_NULL_CHECK(alpm_info, SHR_E_PARAM);
        ctrl = &alpm_info->cmd_ctrl;
        cli_out("TRANS_ID %d ", ctrl->cseq_id);
    } while (0);
#endif

    fwd_groups = (groups + 1) >> 1;
    fwd_groups2 = groups - fwd_groups;
    /* Advance to second group */
    fwd_groups--;
    group_a += RBKT_VBANKS(u, m, db, ln, group_a) + RBKT_FBANKS(u, m, db, ln, group_a);

#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        cli_out("shuffleb (%5d rbkts) (%5d->%-5d):"
                " append %4d groups, ",
                group_c - group_a, group_a, group_c,
                fwd_groups + 1);
    }
#endif
    start = &RBKT_LIST_NODE(u, m, db, ln, group_a);
    SHR_DQ_TRAVERSE_FROM(&BKT_HDL_GROUP_LIST(u, m, db, ln), start, elem)
        if (fwd_groups-- <= 0) {
            break;
        }
        fcurr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_append(u, m, db, ln, fcurr->index, free_group));
        VER1_RBKT_GROUP_SANITY(u, m, db, ln, *free_group, false, 0);
    SHR_DQ_TRAVERSE_END(list, elem);

#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        cli_out("prepend %4d groups", fwd_groups2);
    }
#endif
    start = &RBKT_LIST_NODE(u, m, db, ln, group_c);
    SHR_DQ_BACK_TRAVERSE_FROM(&BKT_HDL_GROUP_LIST(u, m, db, ln), start, elem)
        if (fwd_groups2-- <= 0) {
            break;
        }
        bcurr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_prepend(u, m, db, ln, bcurr->index, free_group));
        VER1_RBKT_GROUP_SANITY(u, m, db, ln, *free_group, false, 0);
    SHR_DQ_TRAVERSE_END(list, elem);

    ALPM_ASSERT(RBKT_FBANKS(u, m, db, ln, *free_group) >= size);
exit:
    /* Failure is only possible on WAL full */
#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        cli_out("\n");
    }
#endif
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shuffle existing groups to create a free group with requesting size.
 *
 * Shuffle existing groups to create new one with requesting free banks.
 * Shuffle will not move group with no free banks, that means,
 * the target group will not be selected to the light shuffle list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Expanding group, which should be avoided to be moved.
 *             INVALID_GROUP indicates this will be a new group.
 * \param [in] size Requesting size for new group
 * \param [out] free_group Group with free banks of requesting size
 *
 * \return SHR_E_NONE/SHR_E_RESOURCE.
 */
static int
ver1_rbkt_group_shuffle_a(int u, int m, int db,
                          int ln,
                          rbkt_group_t group,
                          uint8_t size,
                          rbkt_group_t *free_group
                          )
{
    rbkt_t *curr, *prev;
    rbkt_t *shuffle_groups[MAX_SHUFFLE_NUM] = {NULL};
    rbkt_t *prev_groups[MAX_SHUFFLE_NUM] = {NULL};
    int collect_count = 0;
    int collect_groups = 0;
    shr_dq_t *elem = NULL;
    shr_dq_t *group_list;
    int free_count;
    int tmp_count, i;

    SHR_FUNC_ENTER(u);

    /*
     * Shuffle groups as follows:
     * - Traverse from the most to the least free list.
     * - Find the first rbkt and store it to a temporay list.
     * - Find a rbkt that can be merged with a previously found rbkt so that
     *   that rbkt can be with free banks more than previously collected
     *   free banks.
     * - Once collected free banks are enough (>= size), job done.
     * - Otherwise, fails.
     */
    for (free_count = BKT_HDL_MAX_BANKS(u, m, db, ln); free_count >= 0;
         free_count--) {
        group_list = &GROUP_USAGE_LIST(u, m, db, ln, free_count);

        SHR_DQ_TRAVERSE(group_list, elem)
            curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
            i = 1;
            while (prev_groups[i] && prev_groups[i] != curr) {
                i++;
            }
            if (prev_groups[i]) {
                continue; /* skip this curr, as it is in prev list */
            }
            if (shuffle_groups[0] == NULL) {
                /* The first group can be used anyway */
                shuffle_groups[0] = curr;
                collect_groups++;
                collect_count = free_count;
            } else if (curr->valid_banks <= collect_count) {
                if (curr->index == FIRST_GROUP) { /* No prev group for first group */
                    tmp_count = curr->valid_banks + curr->free_banks;
                    prev = NULL;
                } else {
                    prev = SHR_DQ_ELEMENT_GET(rbkt_t*,
                                              SHR_DQ_PREV(&curr->listnode,
                                                          shr_dq_t*),
                                              listnode);
                    i = 0;
                    while (shuffle_groups[i] && shuffle_groups[i] != prev) {
                        i++;
                    };
                    tmp_count = curr->valid_banks + curr->free_banks;
                    if (!shuffle_groups[i]) { /* prev not in the collect list */
                        /* Ok to add previous free banks as well */
                        tmp_count += prev->free_banks;
                    }
                }
                if (tmp_count > collect_count) {
                    /*
                     * If curr can be moved, and more free count after it moved.
                     * Increase the free count, and add curr to shuffle list
                     */
                    collect_count = tmp_count;
                    prev_groups[collect_groups] = prev;
                    shuffle_groups[collect_groups++] = curr;
                }
            }
            if (collect_count >= size) {
                /* Collect enough */
                break;
            }
        SHR_DQ_TRAVERSE_END(group_list, elem);

        if (collect_count >= size) {
            break;
        }
    }

    if (collect_count >= size) { /* Shuffle list collected */
        rbkt_group_t src_group;
        rbkt_group_t dst_group;
        rbkt_group_t free_group2 = INVALID_GROUP;
        rbkt_group_t new_group;
        for (i = 1; i < collect_groups; i++) {
            ALPM_ASSERT(shuffle_groups[i - 1]) ;
            /* Merge i to (i - 1) */
            src_group = shuffle_groups[i]->index;

            if (free_group2 != INVALID_GROUP) {
                dst_group = free_group2;
            } else {
                /* Only first occurrence */
                dst_group = shuffle_groups[i - 1]->index;
            }

            /* Fuse them */
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_move(u, m, db, ln, src_group, dst_group,
                                     &new_group,
                                     TRUE, &free_group2));
        }
        if (free_group2 == INVALID_GROUP ) {
            *free_group = shuffle_groups[0]->index;
        } else {
            ALPM_ASSERT(RBKT_FBANKS(u, m, db, ln, free_group2) >= size);
            *free_group = free_group2;
        }
        bcmptm_rm_alpm_internals[u][m][db][ln].shuffle_a++;
    } else {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }


exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Find roung range to do ultimate shuffle.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] size Requesting size for new group
 * \param [in] group Expanding group, which should be avoided to be moved.
 *             INVALID_GROUP indicates this will be a new group.
 * \param [out] start_group Travese start group
 * \param [out] stop_group Travese end group
 *
 * \return SHR_E_NONE/SHR_E_RESOURCE.
 */
static int
ver1_rbkt_group_shuffle_rough_range(int u, int m, int db, int ln,
                                    uint8_t size,
                                    rbkt_group_t group,
                                    rbkt_group_t *start_group,
                                    rbkt_group_t *stop_group)
{
    int free_count;
    rbkt_group_t start = MAX_INT_VALUE, stop = INVALID_GROUP;
    int left_size = 0;
    int right_size = 0;
    shr_dq_t *group_list;
    shr_dq_t *elem;
    rbkt_t *curr;
    for (free_count = BKT_HDL_MAX_BANKS(u, m, db, ln); free_count >= 0;
         free_count--) {
        group_list = &GROUP_USAGE_LIST(u, m, db, ln, free_count);

        SHR_DQ_TRAVERSE(group_list, elem)
            curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
            if (curr->index < group || group == INVALID_GROUP) {
                if (curr->index < start) {
                    start = curr->index;
                }
                left_size += free_count;
                if (left_size >= size) {
                    *start_group = start;
                    return SHR_E_NONE;
                }
            } else if (curr->index > group){
                if (curr->index > stop) {
                    stop = curr->index;
                }
                right_size += free_count;
                if (right_size >= size) {
                    *stop_group = stop;
                    return SHR_E_NONE;
                }
            }
        SHR_DQ_TRAVERSE_END(group_list, elem);
    }
    LOG_V((BSL_META(" Used %d Total %d \n"),
           BKT_HDL_RBKTS_USED(u, m, db, ln),
           BKT_HDL_RBKTS_TOTAL(u, m, db, ln)));
    return SHR_E_RESOURCE;
}

/*!
 * \brief Shuffle existing groups to create a free group with requesting size.
 *
 * Wrapper of two shuffle algorithms: shuffle_1 & shuffle_2.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Expanding group, which should be avoided to be moved.
 *             INVALID_GROUP indicates this will be a new group.
 * \param [in] size Requesting size for new group
 * \param [out] free_group Group with free banks of requesting size
 *
 * \return SHR_E_NONE/SHR_E_RESOURCE.
 */
static int
ver1_free_rbkt_group_shuffle_create(int u, int m, int db,
                                    int ln,
                                    rbkt_group_t group,
                                    uint8_t size,
                                    rbkt_group_t *free_group
                                    )
{
    int l_groups, r_groups;
    int l_group_a = INVALID_GROUP, r_group_a = INVALID_GROUP;
    int l_group_c = INVALID_GROUP, r_group_c = INVALID_GROUP;

    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT_EXCEPT_IF2(
        ver1_rbkt_group_shuffle_a(u, m, db, ln, group, size, free_group),
        SHR_E_RESOURCE) {
        rbkt_group_t start_group = INVALID_GROUP, stop_group = INVALID_GROUP;
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_shuffle_rough_range(u, m, db, ln, size, group,
                                                &start_group,
                                                &stop_group));
        if (start_group != INVALID_GROUP) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_shuffle_range(u, m, db, ln,
                                              start_group,
                                              group,
                                              size,
                                              &l_group_a, &l_group_c,
                                              &l_groups));
        } else {
            l_groups = MAX_INT_VALUE;
        }

        if (stop_group != INVALID_GROUP && group != INVALID_GROUP) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_shuffle_range(u, m, db, ln,
                                              group,
                                              stop_group,
                                              size,
                                              &r_group_a, &r_group_c,
                                              &r_groups));
        } else {
            r_groups = MAX_INT_VALUE;
        }

        if (l_groups <= r_groups && l_groups != MAX_INT_VALUE &&
            l_group_a != INVALID_GROUP && l_group_c != INVALID_GROUP) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_shuffle_b(u, m, db, ln,
                                           l_group_a, l_group_c,
                                           l_groups, size, free_group));
        } else if (r_groups != MAX_INT_VALUE &&
                   r_group_a != INVALID_GROUP &&
                   r_group_c != INVALID_GROUP) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_shuffle_b(u, m, db, ln,
                                          r_group_a, r_group_c,
                                          r_groups, size, free_group));
        } else {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }
exit:
    SHR_FUNC_EXIT();
}


static rbkt_group_t
ver1_free_rbkt_group_first_get(int u, int m, int db, int ln, int size)
{
    int8_t free_count;
    rbkt_t *curr;
    shr_dq_t *elem;
    shr_dq_t *group_list;

    for (free_count = BKT_HDL_MAX_BANKS(u, m, db, ln); free_count >= size;
         free_count--) {
        group_list = &GROUP_USAGE_LIST(u, m, db, ln, free_count);

        SHR_DQ_TRAVERSE(group_list, elem)
            curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
            return curr->index;
        SHR_DQ_TRAVERSE_END(group_list, elem);
    }
    return INVALID_GROUP;
}

/*!
 * \brief Allocate a rbkt for existing group.
 *
 * Basically, this is to expand an existing group by one rbkt.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to expand.
 * \param [out] rbkt New rbkt allocated.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_rbkt_alloc(int u, int m, int db, int ln,
                rbkt_group_t group,
                rbkt_idx_t *rbkt,
                bool *format_update)
{
    rbkt_group_t prev_group;
    rbkt_group_t new_group;

    SHR_FUNC_ENTER(u);
    ALPM_ASSERT(RBKT_VBANKS(u, m, db, ln, group) < BKT_HDL_MAX_BANKS(u, m, db, ln));
    ALPM_ASSERT(RBKT_FBANKS(u, m, db, ln, group) == 0);

    if (BKT_HDL_RBKTS_TOTAL(u, m, db, ln) == BKT_HDL_RBKTS_USED(u, m, db, ln)) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    prev_group = prev_group_get(u, m, db, ln, group);
    if (prev_group != INVALID_GROUP &&
        RBKT_FBANKS(u, m, db, ln, prev_group)) { /* Expand left */
        SHR_IF_ERR_EXIT(
            ver1_rbkt_expand_left(u, m, db, ln, prev_group, group));
        *rbkt = group - 1;
        /* Update pivot as ROFS changed */
        if (format_update) {
            *format_update = true;
        }
    } else { /* Move entire group */
        int size = RBKT_VBANKS(u, m, db, ln, group) + 1;
        rbkt_group_t dest_group;

        dest_group = ver1_free_rbkt_group_first_get(u, m, db, ln, size);
        if (dest_group == INVALID_GROUP) {
            SHR_IF_ERR_VERBOSE_EXIT(
                ver1_free_rbkt_group_shuffle_create(u, m, db, ln, group, size,
                                                    &dest_group));
        }
        ALPM_ASSERT(dest_group != INVALID_GROUP);
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_move(u, m, db, ln,
                                 group,
                                 dest_group,
                                 &new_group,
                                 TRUE,
                                 NULL));
        /* During the group move, pivot is updated, format is renewed. */
        *rbkt = RBKT_VBANKS(u, m, db, ln, new_group) + new_group;
        SHR_IF_ERR_EXIT(
            ver1_rbkt_expand_right(u, m, db, ln, new_group));
    }
exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Free an existing group.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to be freed
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_group_free(int u, int m, int db, int ln, rbkt_group_t group)
{
    rbkt_group_t prev_group;

    SHR_FUNC_ENTER(u);
    if (group == INVALID_GROUP) { /* Invalid group does not exist */
        SHR_EXIT();
    }

    ALPM_ASSERT(RBKT_VBANKS(u, m, db, ln, group) == 0);
    if (group == FIRST_GROUP ||
        group == RESERVED_GROUP) { /* Don't fuse group 0 & 1 */
        VER1_RBKT_PIVOT(u, m, db, ln, group) = NULL;
    } else {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_fuse(u, m, db, ln, group, &prev_group));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a new group.
 *
 * If shuffle is not set, just search free list to find a one group to
 * carve out a new one from it.
 * If shuffle is set, simply shuffle existing groups to create new one.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] size Requesting size for new group
 * \param [bool] shuffle Shuffle groups to create new
 * \param [out] new_group New group allocated
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_group_alloc(int u, int m, int db,
                      int ln,
                      uint8_t size,
                      bool shuffle,
                      rbkt_group_t *new_group)
{
    int free_count;
    int max_banks;
    shr_dq_t *groups_ptr;
    shr_dq_t *elem = NULL;
    rbkt_t *free_rbkt;
    rbkt_idx_t free_group;
    bkt_hdl_t *bkt_hdl;

    SHR_FUNC_ENTER(u);

    if (size == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(
        ver1_bkt_hdl_get(u, m, db, ln, &bkt_hdl));

    if (bkt_hdl->rbkts_total - bkt_hdl->rbkts_used < size) {
        LOG_V(("DB %d level %d remaining %d rbkts not enough "\
               "for requesting %d \n",
               db, ln, bkt_hdl->rbkts_total - bkt_hdl->rbkts_used, size));
        SHR_IF_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (shuffle) {
        /* No new_group can spare bank of requesting size */
        SHR_IF_ERR_VERBOSE_EXIT(
            ver1_free_rbkt_group_shuffle_create(u, m, db, ln, INVALID_GROUP, size,
                                                &free_group));
        ALPM_ASSERT(free_group == RBKT_IDX(u, m, db, ln, free_group));
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_split(u, m, db, ln, free_group, size, new_group));
        SHR_EXIT();
    }

    max_banks = bkt_hdl->max_banks;

    /*
     * Traverse the GROUP_USAGE array starting from max free count.
     * Each elem in the array has a list attached. Each list connects all
     * groups with same number of free base buckets. The subscript of the
     * elem equals to the count of free base buckets.
     * The max free count equals to the max banks available for the given
     * db & ln, if any of the new_group has free base buckets more than max
     * banks,
     * it will join those groups with just max banks of free buckets.
     * This is because, a new_group can only have up to max banks of free
     * buckets,
     * any free count beyond that does not make it 'more free'.
     */
    for (free_count = max_banks; free_count >= size; free_count--) {
        groups_ptr = &GROUP_USAGE_LIST(u, m, db, ln, free_count);

        if (!SHR_DQ_EMPTY(groups_ptr)) {
            /* Stop at the first new_group */
            elem = SHR_DQ_HEAD(groups_ptr, shr_dq_t*);
            break;
        }
    }

    if (elem) {
        free_rbkt = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
        free_group = free_rbkt->index;
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_split(u, m, db, ln, free_group, size, new_group));
    } else {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Allocate a new group with various sizes.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] best_size Best requesting size (biggest size)
 * \param [in] shuffle_size Size kicks off shuffle (middle size)
 * \param [in] least_size  Least size (Guaranteed size)
 * \param [out] new_group New group allocated
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_group_alloc_best_effort(int u, int m, int db, int ln,
                                  uint8_t best_size,
                                  uint8_t shuffle_size,
                                  uint8_t least_size,
                                  alpm_data_type_t dt,
                                  rbkt_group_t *new_group)
{
    int rv = SHR_E_RESOURCE;
    SHR_FUNC_ENTER(u);

    while (rv == SHR_E_RESOURCE) {
        rv = ver1_rbkt_group_alloc(u, m, db, ln, best_size, FALSE, new_group);
        if (best_size <= least_size) {
            if (rv == SHR_E_RESOURCE) {
                rv = ver1_rbkt_group_alloc(u, m, db, ln,
                                           shuffle_size,
                                           TRUE,
                                           new_group);
                if (rv == SHR_E_RESOURCE && least_size < shuffle_size) {
                    rv = ver1_rbkt_group_alloc(u, m, db, ln,
                                               least_size,
                                               FALSE,
                                               new_group);
                }
            }
            /* Succeed on shuffle or last try */
            break;
        }
        best_size >>= 1;
    }
    SHR_ERR_EXIT(rv);
    /* Succeed */
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_rbkt_recover(int u, int m, int db, int ln, rbkt_idx_t rbkt_idx,
                  ln_base_entry_t *base_entry,
                  uint32_t dt_bitmap,
                  uint8_t *entry_count,
                  alpm_arg_t *arg)

{
    format_type_t format_type;
    int num_entries, i;
    uint8_t format = RBKT_FORMAT(u, m, db, ln, rbkt_idx);
    alpm_pivot_t *pivot;
    alpm_arg_t *entry_arg = NULL;
    int full;

    SHR_FUNC_ENTER(u);
    pivot = arg->pivot[PREV_LEVEL(ln)];
    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                          format,
                                          NULL,
                                          &full,
                                          &num_entries));
    entry_arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (entry_arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    bcmptm_rm_alpm_arg_init(u, m, entry_arg);
    entry_arg->db = db;
    for (i = 0; i < num_entries; i++) {
        entry_arg->key = arg->key;
        sal_memcpy(entry_arg->pivot, arg->pivot, sizeof(entry_arg->pivot));
        entry_arg->key.t.trie_ip = pivot->key.t.trie_ip;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_decode(u, m, format_type,
                                                format,
                                                pivot->key.t.len,
                                                entry_arg,
                                                base_entry[i]));
        entry_arg->ad.actual_data_type = ver1_full_to_dt(u, m, full);
        if (format_type == FORMAT_ROUTE) {
            if (full) {
                if (bcmptm_rm_alpm_cache_data_type(u, m)) {
                    entry_arg->ad.user_data_type =
                        ver1_dt_bitmap_get(u, m, i, dt_bitmap);
                }
            } else {
                entry_arg->ad.user_data_type = DATA_REDUCED;
            }
        }

        /* No need to do.
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_alpm_mask_to_len(u, m, arg->key.t.ipv,
                                        arg->key.ip_mask,
                                        &arg->key.t.len));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_key_create(u, m, entry_arg->key.t.ipv,
                                           entry_arg->key.ip_addr,
                                           entry_arg->key.t.len,
                                           &entry_arg->key.t.trie_ip));*/
        if (entry_arg->valid) {
            ALPM_BIT_SET(RBKT_EBMP(u, m, db, ln, rbkt_idx), i);
            entry_arg->index[ln] = ALPM_INDEX_ENCODE_VER1(rbkt_idx, i);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_pivot_recover(u, m, ln, entry_arg));
            (*entry_count)++;
            if (format_type == FORMAT_PIVOT) {
                int next_ln = NEXT_LEVEL(ln);
                SHR_IF_ERR_EXIT(
                    ver1_bucket_recover(u, m, next_ln, entry_arg));
                ALPM_ASSERT(entry_arg->pivot[ln] &&
                            entry_arg->pivot[ln]->bkt);
                entry_arg->pivot[ln]->bkt->log_bkt =
                    entry_arg->log_bkt[ln];
                ALPM_ASSERT(entry_arg->log_bkt[ln] !=
                            VER1_LOG_BKT(RESERVED_GROUP));
            }
        }
    }
exit:
    if (entry_arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], entry_arg);
    }
    SHR_FUNC_EXIT();
}


static int
ver1_rbkt_group_recover(int u, int m, int db, int ln, rbkt_group_t group,
                        alpm_arg_t *arg)
{
    ln_base_entry_t *base_entry;
    format_type_t format_type;
    base_entries_ver1_t *base_entries = NULL;
    uint8_t i, log_bank, phy_bank;
    uint8_t *fmts;
    alpm_pivot_t *pivot;
    base_buckets_ver1_t *bbkts = NULL;
    bkt_hdl_t *bkt_hdl;

    SHR_FUNC_ENTER(u);
    base_entries = shr_lmm_alloc(base_entries_lmem[u][m]);
    if (base_entries == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (bbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);

    RBKT_IS_HEAD(u, m, db, ln, group) = true;
    pivot = arg->pivot[PREV_LEVEL(ln)];
    ALPM_ASSERT(pivot);
    ALPM_ASSERT(!VER1_RBKT_PIVOT(u, m, db, ln, group) || group == RESERVED_GROUP);
    ver1_rbkt_pivot_set(u, m, db, ln, group, pivot);
    arg->log_bkt[PREV_LEVEL(ln)] = VER1_LOG_BKT(group);
    fmts = (PREV_LEVEL(ln) == LEVEL1)? arg->a1.fmt : arg->a2.fmt;

    SHR_IF_ERR_EXIT(
        ver1_bkt_hdl_get(u, m, db, ln, &bkt_hdl));
    log_bank = group % bkt_hdl->max_banks;
    for (i = 0; i < bkt_hdl->max_banks; i++) {
        uint32_t hw_idx;
        uint8_t fmt;
        rbkt_idx_t rbkt_idx;
        uint8_t entry_count;

        phy_bank = bkt_hdl->l2p_bank_array[log_bank];
        ALPM_ASSERT(bkt_hdl->bank_bitmap & (1 << phy_bank));
        fmt = fmts[phy_bank];
        if (fmt == DISABLED_FORMAT) {
            break;
        }
        if (++log_bank == bkt_hdl->max_banks) {
            log_bank = 0;
        }

        hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, group + i);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[i][0],
                                       &bbkts->dt_bitmap[i]));

        base_entry = &base_entries->entry[i][0];
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_raw_bucket_decode(u, m, format_type,
                                                fmt,
                                                base_entry,
                                                RBKT_MAX_ENTRIES_VER1,
                                                &bbkts->rbuf[i][0]));
        rbkt_idx = group + i;
        RBKT_FORMAT(u, m, db, ln, rbkt_idx) = fmt;
        ALPM_ASSERT(RBKT_VBANKS(u, m, db, ln, group) <= bkt_hdl->max_banks);
        entry_count = 0;
        SHR_IF_ERR_EXIT(
            ver1_rbkt_recover(u, m, db, ln, rbkt_idx, base_entry,
                              bbkts->dt_bitmap[i],
                              &entry_count, arg));
        if (entry_count) {
            RBKT_VBANKS(u, m, db, ln, group)++;
            bkt_hdl->rbkts_used++;
        }
    }
exit:
    if (base_entries) {
        shr_lmm_free(base_entries_lmem[u][m], base_entries);
    }
    if (bbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], bbkts);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback to group bkt elems based on IP Length.
 *
 * \param [in] node Trie node
 * \param [in] user_data User data
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_bkt_elems_connect_cb(rm_alpm_trie_node_t *node, void *user_data)
{
    shr_dq_t *elem;
    bucket_traverse_data_t *trvs_data;
    alpm_bkt_elem_t *curr;
    uint8_t dt = 0;

    trvs_data = (bucket_traverse_data_t *) user_data;
    if (node->type == PAYLOAD) {
        alpm_bkt_elem_t *bkt_elem;
        bkt_elem = TRIE_ELEMENT_GET(alpm_bkt_elem_t*, node, bktnode);
        if (bkt_elem->ad) {
            dt = bkt_elem->ad->user_data_type;
        }

        if (SHR_DQ_EMPTY(trvs_data->dq[dt])) {
            SHR_DQ_INSERT_HEAD(trvs_data->dq[dt], &bkt_elem->dqnode);
        } else {
            curr = NULL; /* For coverity */
            SHR_DQ_TRAVERSE(trvs_data->dq[dt], elem)
                curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
                if (curr->key.t.len >= bkt_elem->key.t.len) {
                    SHR_DQ_INSERT_PREV(elem, &bkt_elem->dqnode);
                    break;
                }
            SHR_DQ_TRAVERSE_END(trvs_data->dq[dt], elem);
            if (curr && curr->key.t.len < bkt_elem->key.t.len) {
                SHR_DQ_INSERT_TAIL(trvs_data->dq[dt], &bkt_elem->dqnode);
            }
        }
        if (bkt_elem->index == INVALID_INDEX) {
            trvs_data->found_new = true;
        }
        trvs_data->count++;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Connect bkt elems to bucket_list with IP length group sorted.
 *
 * \param [in] u Device u.
 * \param [in] list Bucket element List
 * \param [in] trie Bucket trie
 *
 * \return SHR_E_NONE.
 */
static int
ver1_bkt_elems_connect(int u, int m, shr_dq_t *list, rm_alpm_trie_node_t *root,
                       bool *found_new)
{
    shr_dq_t list2;
    bucket_traverse_data_t trvs_data =
        /* List with REDUCED, list2 with FULL. */
        {.dq = {list, &list2}, .count = 0, .found_new = 0};

    SHR_FUNC_ENTER(u);
    SHR_DQ_INIT(list);
    SHR_DQ_INIT(&list2);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_traverse(u, m,
                                     root,
                                     ver1_bkt_elems_connect_cb,
                                     &trvs_data,
                                     TRAVERSE_BFS));
    if (found_new) {
        *found_new = trvs_data.found_new;
    }
    /* REDUCED first, then followed by FULL. */
    if (!SHR_DQ_EMPTY(&list2)) {
        SHR_DQ_JOIN_TAIL(list, &list2);
    }
    ALPM_ASSERT((root && (trvs_data.count == root->count)) ||
                (root == NULL && trvs_data.count == 0));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Free used entries' slots for given group.
 *
 * Do not rearrange group resource, once done rbkts holes may exist.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to alloc base entry
 * \param [in] ent_idx Entry index array
 * \param [in] count Number of array elements
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_base_entries_free(int u, int m, int db, int ln,
                       rbkt_group_t group,
                       ln_index_t *ent_idx,
                       int count,
                       bool *format_update)
{
    rbkt_idx_t remove_bkt;
    rbkt_t *rbkt;
    int i;

    SHR_FUNC_ENTER(u);
    for (i = 0; i < count; i++) {
        if (ent_idx[i] == INVALID_INDEX) {
            continue;
        }
        remove_bkt = RBKT_FROM_INDEX(ent_idx[i]);
        rbkt = &RBKT(u, m, db, ln, remove_bkt);

        ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap,
                                 ENT_OFS_FROM_IDX(ent_idx[i])));
        ALPM_BIT_CLR(rbkt->entry_bitmap, ENT_OFS_FROM_IDX(ent_idx[i]));

        if (rbkt->entry_bitmap == 0) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_free(u, m, db, ln, group, remove_bkt));
            if (format_update) {
                *format_update = true;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Free a used entry slot for given group.
 *
 * Rearrange group resource if necessary, as to keep rbkts contiguous.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to alloc base entry
 * \param [in] ent_idx Entry index
 * \param [out] format_update True if update pivot format, eles false.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_base_entry_free(int u, int m, int db, int ln,
                     rbkt_group_t group,
                     ln_index_t ent_idx,
                     bool compress,
                     bool *format_update)
{
    rbkt_idx_t remove_bkt;
    rbkt_t *rbkt;

    SHR_FUNC_ENTER(u);
    remove_bkt = RBKT_FROM_INDEX(ent_idx);
    rbkt = &RBKT(u, m, db, ln, remove_bkt);

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ENT_OFS_FROM_IDX(ent_idx)));
    ALPM_BIT_CLR(rbkt->entry_bitmap, ENT_OFS_FROM_IDX(ent_idx));
    if (rbkt->entry_bitmap == 0) {
        rbkt_idx_t last_bkt = group + RBKT_VBANKS(u, m, db, ln, group) - 1;
        if (compress && last_bkt != remove_bkt) {
            /*
             * If a mid rbkt becomes empty, copy the last to that empty place.
             * but we don't clear the last immediatelly, as to prevent packet
             * misses. We should clear it after pivot update done.
             */

            SHR_IF_ERR_EXIT(
                ver1_rbkt_copy(u, m, db, ln, last_bkt, remove_bkt, TRUE, true));
            SHR_IF_ERR_EXIT(
                ver1_rbkt_free(u, m, db, ln, group, last_bkt));
        } else {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_free(u, m, db, ln, group, remove_bkt));
        }
        if (format_update) {
            *format_update = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
static inline bool
ver1_base_entry_format_is_new(int *formats, int format_count, int format)
{
    int i;
    for (i = 0; i < format_count; i++) {
        if (formats[i] == format) {
            return false;
        }
    }
    return true;
}
/*!
 * \brief Assign a free entry slot for given rbkt.
 *
 * Traverse the existing rbkt resources and assign free entry.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] entry_len IP length of requesting entry.
 * \param [in] entry_full Data type (full/reduced) of requesting entry.
 * \param [out] ent_idx Entry index
 * \param [out] format_update True if update pivot format, eles false.
 * \param [out] format_value New format value.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_base_entry_new_rbkt_assign(int u, int m, int db, int ln,
                                 int entry_len,
                                 int max_bits,
                                 int entry_full,
                                 int rbkt_idx,
                                 int *ent_idx,
                                 bool *format_update,
                                 int *format_value)
{
    int prefix_len;
    int full;
    int format;
    int max_format, format_type;
    SHR_FUNC_ENTER(u);

    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    if (bcmptm_rm_alpm_is_large_data_type(u, m, db)) {
        max_format = VER1_MAX_FORMAT(u, m, format_type, DATA_FULL_3);
    } else {
        max_format = VER1_MAX_FORMAT(u, m, format_type, DATA_FULL);
    }

    if (entry_len == 0 && ln == DB_LAST_LEVEL(u, m, db)) {
        entry_len = max_bits;
    }

    /* From most efficient format to least efficient format */
    for (format = VER1_START_FORMAT(u, m, format_type, entry_full);
         format <= max_format; format++) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                              format,
                                              &prefix_len,
                                              &full,
                                              NULL));
        if (prefix_len >= entry_len && full >= entry_full) {
            RBKT_FORMAT(u, m, db, ln, rbkt_idx) = format;
            ALPM_BIT_SET(RBKT_EBMP(u, m, db, ln, rbkt_idx), 0);
            *ent_idx = ALPM_INDEX_ENCODE_VER1(rbkt_idx, 0);

            if (format_update) {
                *format_update = TRUE;
            }
            *format_value = format;
            SHR_EXIT();
        }
    }
    ALPM_ASSERT(0);
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_base_entry_old_rbkt_assign(int u, int m, int db, int ln,
                                int rbkt_idx,
                                int format,
                                int *ent_idx,
                                bool *format_update
                               )

{
    int i, full, prefix_len, number;
    format_type_t format_type;

    SHR_FUNC_ENTER(u);
    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                          format,
                                          &prefix_len,
                                          &full,
                                          &number));
    for (i = 0; i < number; i++) {
        if (!ALPM_BIT_GET(RBKT_EBMP(u, m, db, ln, rbkt_idx), i)) {
            *ent_idx = ALPM_INDEX_ENCODE_VER1(rbkt_idx, i);
            ALPM_BIT_SET(RBKT_EBMP(u, m, db, ln, rbkt_idx), i);
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT();
}


static int
ver1_base_entry_new_formats_get(int u, int m, int db, int ln,
                                int entry_len,
                                int entry_full,
                                int *formats,
                                int *format_count)
{
    int prefix_len;
    int full;
    int format;
    int max_format, format_type;
    int num_new_fmts = 0;
    SHR_FUNC_ENTER(u);

    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    /* Reduce search scale by searching same dt only. */
    max_format = VER1_MAX_FORMAT(u, m, format_type, entry_full);

    /* From most efficient format to least efficient format */
    for (format = VER1_START_FORMAT(u, m, format_type, entry_full);
         format <= max_format; format++) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                              format,
                                              &prefix_len,
                                              &full,
                                              NULL));
        if (prefix_len >= entry_len && full >= entry_full) {
            /* Reduce search scale. */
            if (++num_new_fmts > search_new_fmts) {
                SHR_EXIT();
            }
            if ((*format_count) >= MAX_FORMAT_COUNT) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            formats[(*format_count)++] = format;
        }
    }
exit:
    SHR_FUNC_EXIT();
}



static int
ver1_base_entry_assign(int u, int m, int db, int ln,
                       rbkt_group_t group,
                       rbkt_idx_t rbkt_idx,
                       int format,
                       int *ent_idx)
{
    SHR_FUNC_ENTER(u);
    if (rbkt_idx < group + RBKT_VBANKS(u, m, db, ln, group)) {
        SHR_IF_ERR_EXIT(
            ver1_base_entry_old_rbkt_assign(u, m, db, ln, rbkt_idx,
                                            format, ent_idx, NULL));
    } else {
        ALPM_ASSERT(rbkt_idx == group + RBKT_VBANKS(u, m, db, ln, group));
        SHR_IF_ERR_EXIT(
            ver1_rbkt_expand_right(u, m, db, ln, group));
        RBKT_FORMAT(u, m, db, ln, rbkt_idx) = format;
        ALPM_BIT_SET(RBKT_EBMP(u, m, db, ln, rbkt_idx), 0);
        *ent_idx = ALPM_INDEX_ENCODE_VER1(rbkt_idx, 0);
    }
exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Allocate a free entry slot for given group.
 *
 * Assign resources that group currently have. Does not try
 * to expand the group resource.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to alloc base entry
 * \param [in] entry_len IP length of requesting entry.
 * \param [in] entry_full Data type (full/reduced) of requesting entry.
 * \param [out] ent_idx Entry index
 * \param [out] format_update True if update pivot format, eles false.
 * \param [out] format_value Format value for this base entry
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_base_entry_alloc(int u, int m, int db, int ln,
                      rbkt_group_t group,
                      int entry_len,
                      int max_bits,
                      int entry_full,
                      ln_index_t *ent_idx,
                      bool *format_update,
                      int *format_value)
{
    int i;
    int min_format = INVALID_FORMAT;
    int prefix_len;
    int full;
    int min_full = -1;
    int number;
    rbkt_idx_t rbkt_idx = 0;
    format_type_t format_type;
    SHR_FUNC_ENTER(u);

    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    for (i = 0; i < RBKT_VBANKS(u, m, db, ln, group); i++) {
        rbkt_t *rbkt = &RBKT(u, m, db, ln, group + i);

        /* Less format value, more compact */
        if (rbkt->format < min_format) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                                  rbkt->format,
                                                  &prefix_len,
                                                  &full,
                                                  &number));
            if (prefix_len >= entry_len &&
                rbkt->entry_bitmap != (uint32_t)((1 << number) - 1) &&
                full >= entry_full) {
                min_format = rbkt->format;
                min_full = full;
                rbkt_idx = group + i;
            }
        }
    }

    /* Free entry in valid banks, and dt matches. */
    if (min_format != INVALID_FORMAT && entry_full == min_full) {
        *format_value = min_format;
        SHR_ERR_EXIT(
            ver1_base_entry_old_rbkt_assign(u, m, db, ln, rbkt_idx,
                                            min_format, ent_idx, format_update));
    }

    ALPM_ASSERT(i == RBKT_VBANKS(u, m, db, ln, group));

    /* Assign free banks */
    if (RBKT_FBANKS(u, m, db, ln, group) && i < BKT_HDL_MAX_BANKS(u, m, db, ln)) {
        rbkt_idx = group + RBKT_VBANKS(u, m, db, ln, group);
        SHR_IF_ERR_EXIT(
            ver1_rbkt_expand_right(u, m, db, ln, group));
        SHR_IF_ERR_EXIT(
            ver1_base_entry_new_rbkt_assign(u, m, db, ln,
                                             entry_len,
                                             max_bits,
                                             entry_full,
                                             rbkt_idx,
                                             ent_idx,
                                             format_update,
                                             format_value));
    } else {
        /* Free entry in valid banks, but dt unmatches. Full to hold reduced. */
        if (min_format != INVALID_FORMAT) {
            *format_value = min_format;
            SHR_ERR_EXIT(
                ver1_base_entry_old_rbkt_assign(u, m, db, ln, rbkt_idx,
                                                min_format, ent_idx, format_update));
        } else {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Clear one base entry in HW.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] ent_idx Entry index
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_base_entry_clear(int u, int m, int db, int ln, ln_index_t ent_idx)
{
    int rbkt_idx;
    static ln_base_entry_t base_entry = {0};
    rbkt_t *rbkt;
    format_type_t format_type;
    int ent_ofs;
    int hw_idx;
    base_buckets_ver1_t *bbkts = NULL;
    SHR_FUNC_ENTER(u);

    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, rbkt_idx);
    bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (bbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0],
                                   &bbkts->dt_bitmap[0]));
    rbkt = &RBKT(u, m, db, ln, rbkt_idx);

    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_set(u, m, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &bbkts->rbuf[0][0]));
    /* No need to set dt_bitmap on base_entry_clear. */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0],
                                    HIT_OP_NONE, 0, bbkts->dt_bitmap[0]));
    ALPM_ASSERT(!ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));
exit:
    if (bbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], bbkts);
    }
    SHR_FUNC_EXIT();
}



static int
ver1_base_entry_formats_get(int u, int m, int db, int ln,
                            rbkt_group_t group,
                            int entry_len,
                            int entry_full,
                            rbkt_idx_t *rbkt_idxes,
                            int *formats,
                            int *format_count)
{
    int i;
    int prefix_len;
    int full;
    int number;
    format_type_t format_type;
    int free_count = 0;
    rbkt_t *rbkt;

    SHR_FUNC_ENTER(u);

    /* $ Collect formats from valid rbkts */
    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    for (i = 0; i < RBKT_VBANKS(u, m, db, ln, group); i++) {
        rbkt = &RBKT(u, m, db, ln, group + i);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                              rbkt->format,
                                              &prefix_len,
                                              &full,
                                              &number));
        if (prefix_len >= entry_len &&
            rbkt->entry_bitmap != (uint32_t)((1 << number) - 1) &&
            full >= entry_full) {

            if (ver1_base_entry_format_is_new(formats, free_count, rbkt->format)) {
                rbkt_idxes[free_count] = group + i;
                formats[free_count] = rbkt->format;
                free_count++;

                /* Reduce search scale. */
                if (free_count == search_old_fmts) {
                    break;
                }
            }
        }
    }

    /* $ Collect formats from free rbkts. */
    if (RBKT_FBANKS(u, m, db, ln, group) &&
        RBKT_VBANKS(u, m, db, ln, group) < BKT_HDL_MAX_BANKS(u, m, db, ln)) {
        i = free_count;
        SHR_IF_ERR_EXIT(
            ver1_base_entry_new_formats_get(u, m, db, ln,
                                            entry_len,
                                            entry_full,
                                            formats,
                                            &free_count));
        for (; i < free_count; i++) {
            rbkt_idxes[i] = group + RBKT_VBANKS(u, m, db, ln, group);
        }
    }
    (*format_count) = free_count;
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_bucket_indexes_free(int u, int m, int db, int ln,
                         rbkt_group_t dst_group,
                         shr_dq_t *list, int count)
{
    shr_dq_t *elem;
    alpm_bkt_elem_t *bkt_elem;

    SHR_FUNC_ENTER(u);
    SHR_DQ_BACK_TRAVERSE(list, elem)
        bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        SHR_IF_ERR_EXIT(
            ver1_base_entry_free(u, m, db, ln, dst_group, bkt_elem->tmp_index,
                                 FALSE, NULL));
        count--;
    SHR_DQ_TRAVERSE_END(list, elem);
    ALPM_ASSERT(count == 0);
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_base_entry_recursive_alloc(int u, int m, int db, int ln,
                                rbkt_group_t dst_group,
                                key_tuple_t *dst_t,
                                shr_dq_t * list,
                                shr_dq_t *prev,
                                int *trys)
{
    shr_dq_t *curr;
    rbkt_idx_t rbkt_idxes[MAX_FORMAT_COUNT];
    int format_values[MAX_FORMAT_COUNT];
    int format_count = 0, i;
    int full;
    ln_index_t ent_idx;
    alpm_bkt_elem_t *bkt_elem;
    int rv;
#define MAX_TRYS   100000

    SHR_FUNC_ENTER(u);
    curr = SHR_DQ_PREV(prev, void *);
    if (curr == list) {
        SHR_EXIT(); /* Bucket list ends here. */
    }

    bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, curr, dqnode);
    full = (ln == DB_LAST_LEVEL(u, m, db)) ?
            bkt_elem->ad->user_data_type :
            bcmptm_rm_alpm_mid_dt_get(u, m, db, bkt_elem->key.t.app);

    SHR_IF_ERR_EXIT(
        ver1_base_entry_formats_get(u, m, db, ln,
                                    dst_group,
                                    bkt_elem->key.t.len - dst_t->len,
                                    full,
                                    rbkt_idxes,
                                    format_values,
                                    &format_count));

    for (i = 0; i < format_count; i++) {
        if (*trys > MAX_TRYS) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        SHR_IF_ERR_EXIT(
            ver1_base_entry_assign(u, m, db, ln, dst_group, rbkt_idxes[i],
                                   format_values[i], &ent_idx));
        (*trys)++;
        rv = ver1_base_entry_recursive_alloc(u, m, db, ln, dst_group,
                                             dst_t, list, curr, trys);
        if (SHR_FAILURE(rv)) {
            ver1_base_entry_free(u, m, db, ln, dst_group, ent_idx, FALSE, NULL);
            continue;
        } else {
            bkt_elem->tmp_index = ent_idx;
            break;
        }
    }
    if (i == format_count) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_bucket_indexes_alloc(int u, int m, int db, int ln,
                          rbkt_group_t dst_group,
                          key_tuple_t *dst_t,
                          shr_dq_t *list,
                          int count,
                          bool recursive
                          )
{
    ln_index_t dst_idx;
    shr_dq_t *elem;
    alpm_bkt_elem_t *bkt_elem;
    int dst_format;
    int i = 0;
    int rv = SHR_E_RESOURCE;
    int full = 0, full_1 = -1;

    SHR_FUNC_ENTER(u);
    SHR_DQ_BACK_TRAVERSE(list, elem)
        bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        full = (ln == DB_LAST_LEVEL(u, m, db)) ?
                bkt_elem->ad->user_data_type :
                bcmptm_rm_alpm_mid_dt_get(u, m, db, bkt_elem->key.t.app);
        if (full_1 == -1) { /* Save the first full. */
            full_1 = full;
        }
        rv = ver1_base_entry_alloc(u, m, db, ln, dst_group,
                                   bkt_elem->key.t.len - dst_t->len,
                                   bkt_elem->key.t.max_bits,
                              /* format may change to more impact during copy */
                                   full,
                                   &dst_idx,
                                   NULL, /* Will write anyway */
                                   &dst_format);
        if (SHR_FAILURE(rv)) {
            if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                cli_out("ver1_base_entry_alloc failed "\
                        "with group=%d full=%d\n",
                        dst_group, full);
            }
            break;
        }
        i++;
        bkt_elem->tmp_index = dst_idx;
    SHR_DQ_TRAVERSE_END(list, elem);

    ALPM_ASSERT(SHR_FAILURE(rv) || i == count);
    if (SHR_FAILURE(rv) && i) {
        ALPM_ASSERT(i < count); /* Assume just entry alloc would fail */
        count = i;
        i = 0;
        SHR_DQ_BACK_TRAVERSE(list, elem)
            bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
            SHR_IF_ERR_EXIT(
                ver1_base_entry_free(u, m, db, ln, dst_group, bkt_elem->tmp_index,
                                     FALSE, NULL));
            if (++i == count) {
                break;
            }
        SHR_DQ_TRAVERSE_END(list, elem);
        if (recursive && full != full_1) { /* Mixed data types. */
            int trys = 0;
            rv = ver1_base_entry_recursive_alloc(u, m, db, ln,
                                                 dst_group,
                                                 dst_t,
                                                 list,
                                                 list,
                                                 &trys
                                                 );
            if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                cli_out("ln %d dst_group %d trys %d rv %d\n",
                        ln, dst_group, trys, rv);
            }
        }
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static void
ver1_bucket_indexes_set(int u, int m, int db, int ln, shr_dq_t *list)
{
    shr_dq_t *elem;
    alpm_bkt_elem_t *bkt_elem;
    SHR_DQ_BACK_TRAVERSE(list, elem)
        bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        bkt_elem->index = bkt_elem->tmp_index;
    SHR_DQ_TRAVERSE_END(list, elem);
}

/*!
 * \brief (HW) Copy split bkt elems from old bucket to new bucket
 *
 * The ent_index in the elems are updated to new locations, and old
 * ent_index are saved to an array.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Source group
 * \param [in] src_t Source tuple
 * \param [in] dst_group Dest group
 * \param [in] dst_t Dest tuple
 * \param [in] list Copy list
 * \param [in] count Count of bak_idx array
 * \param [in] arg New entry arg, in case new entry in split bucket.
 * \param [out] old_idx Array to save original indexes
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_bucket_copy(int u, int m, int db, int ln,
                 rbkt_group_t src_group,
                 key_tuple_t *src_t,
                 base_buckets_ver1_t *sbbkts,
                 int *src_rbkts,
                 ln_index_t *src_idx_arr,

                 rbkt_group_t dst_group,
                 key_tuple_t *dst_t,
                 base_buckets_ver1_t *dbbkts,
                 int *dst_rbkts,

                 shr_dq_t *list,
                 int count,
                 alpm_arg_t *arg,
                 uint8_t dst_valid_banks
                 )
{
    ln_index_t src_idx, dst_idx;
    shr_dq_t *elem;
    alpm_bkt_elem_t *bkt_elem;
    int src_format, dst_format;
    rbkt_idx_t src_rbkt_idx, dst_rbkt_idx;
    int format_type;
    int src_bank, dst_bank;
    int src_ent, dst_ent;
    int i = 0, hw_idx, full;
    ln_base_entry_t base_entry;
    alpm_data_type_t dt = 0;
    SHR_FUNC_ENTER(u);

    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);

    if (dst_valid_banks == 0) { /* When arg is set, the dest buckets should be empty. */
        /* Must zero dst buf */
        sal_memset(dbbkts->rbuf, 0, sizeof(dbbkts->rbuf));
        /* Not necessary. */
        sal_memset(dbbkts->dt_bitmap, 0, sizeof(dbbkts->dt_bitmap));
    } else {
        for (dst_bank = 0; dst_bank < dst_valid_banks;
             dst_bank++) {
            hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, dst_bank + dst_group);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx,
                                           &dbbkts->rbuf[dst_bank][0],
                                           &dbbkts->dt_bitmap[dst_bank]));
        }
        sal_memset(&dbbkts->rbuf[dst_bank][0], 0,
                   (ALPM_MAX_BANKS - dst_valid_banks) * sizeof(dbbkts->rbuf[0]));
    }
    sal_memset(dbbkts->modify_bitmap, 0, sizeof(dbbkts->modify_bitmap));

    for (src_bank = 0; src_bank < RBKT_VBANKS(u, m, db, ln, src_group); src_bank++) {
        hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, src_bank + src_group);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx,
                                       &sbbkts->rbuf[src_bank][0],
                                       &sbbkts->dt_bitmap[src_bank]));
        sbbkts->rbkt_ofs[src_bank] = -1;
    }

    SHR_DQ_BACK_TRAVERSE(list, elem)
        bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        src_idx = bkt_elem->index;
        dst_idx = bkt_elem->tmp_index;
        src_idx_arr[i++] = src_idx;
        dst_rbkt_idx = RBKT_FROM_INDEX(dst_idx);
        dst_format = RBKT_FORMAT(u, m, db, ln, dst_rbkt_idx);
        dst_bank = dst_rbkt_idx - dst_group;
        dst_ent = ENT_OFS_FROM_IDX(dst_idx);

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                              dst_format,
                                              NULL, &full, NULL));
        if (src_idx != INVALID_INDEX) {
            src_rbkt_idx = RBKT_FROM_INDEX(src_idx);
            src_ent = ENT_OFS_FROM_IDX(src_idx);
            src_bank = src_rbkt_idx - src_group;
            ALPM_ASSERT(src_bank >= 0 && src_bank < ALPM_MAX_BANKS);

            src_format = RBKT_FORMAT(u, m, db, ln, src_rbkt_idx);
            if (format_type == FORMAT_ROUTE) {
                if (full) {
                    /* dt not used when auto_data_type=1 */
                    dt = ver1_dt_bitmap_get(u, m, src_ent, sbbkts->dt_bitmap[src_bank]);
                }
            } else {
                dt = bcmptm_rm_alpm_mid_dt_get(u, m, db, bkt_elem->key.t.app);
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_move(u, m, format_type,
                                                  src_format,
                                                  src_ent,
                                                  src_t,
                                                  &sbbkts->rbuf[src_bank][0],
                                                  dt,

                                                  dst_format,
                                                  dst_ent,
                                                  dst_t,
                                                  &dbbkts->rbuf[dst_bank][0]
                                                  ));
            sbbkts->rbkt_ofs[src_bank] = src_bank; /* Mark rbkts need update */
        } else {
            arg->orig_index = src_idx;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_encode(u, m, format_type,
                                                    dst_format,
                                                    dst_t->len,
                                                    arg,
                                                    &base_entry[0]));

            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_raw_bucket_entry_set(u, m, format_type,
                                                       dst_format,
                                                       &base_entry[0],
                                                       dst_ent,
                                                       &dbbkts->rbuf[dst_bank][0]
                                                       ));
            arg->index[ln] = dst_idx;
            arg->ad.actual_data_type = ver1_full_to_dt(u, m, full);
        }
        if (format_type == FORMAT_ROUTE) {
            if (full) {
                ver1_dt_bitmap_set(u, m, dst_ent, &dbbkts->dt_bitmap[dst_bank],
                                   bkt_elem->ad->user_data_type);
            }
            bkt_elem->ad->actual_data_type = ver1_full_to_dt(u, m, full);
        }
        ALPM_BIT_SET(dbbkts->modify_bitmap[dst_bank], dst_ent);
        if (dst_rbkts[dst_bank] == 0) {
            dst_rbkts[dst_bank] = dst_rbkt_idx;
        }
    SHR_DQ_TRAVERSE_END(list, elem);

    ALPM_ASSERT(i == count);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief (HW) Move bucket elems from old bucket to new bucket
 *
 * The ent_index in the elems are updated to new locations.
 * Elems moved are deleted from old bucket.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Source group
 * \param [in] src_t Source tuple
 * \param [in] dst_group Dest group
 * \param [in] dst_t Dest tuple
 * \param [in] list Move list
 * \param [in] count Count of bak_idx array
 * \param [in] arg New entry arg, in case new entry in split bucket.
 * \param [in] split_arg Arg for split pivot.
 * \param [out] format_update True if update pivot format, eles false.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_bucket_move(int u, int m, int db, int ln,
                 rbkt_group_t src_group,
                 key_tuple_t *src_t,
                 rbkt_group_t dst_group,
                 key_tuple_t *dst_t,
                 shr_dq_t *list,
                 int count,
                 alpm_arg_t *arg,
                 alpm_arg_t *split_arg,
                 bool *format_update,
                 bkt_split_aux_t *split_aux,
                 base_buckets_ver1_t *sbbkts,
                 uint8_t dst_valid_banks)
{
    alpm_pivot_t *dst_pivot;
    ln_index_t *src_idx = NULL;
    alpm_pivot_t *split_pivot;
    uint8_t i;
    int hw_idx;
    base_buckets_ver1_t *dbbkts = NULL;
    int src_rbkts[ALPM_MAX_BANKS] = {0}; /* assuming RESERVED_GROUP = 0. */
    int dst_rbkts[ALPM_MAX_BANKS] = {0}; /* assuming RESERVED_GROUP = 0. */
    enum {BUCKET_MOVE_NONE, BUCKET_MOVE_COPY} done = BUCKET_MOVE_NONE;

    SHR_FUNC_ENTER(u);

    dbbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (dbbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* $ Save old ent_idx of prefixes */
    ALPM_ALLOC(src_idx, count * sizeof(ln_index_t), "bcmptmRmalpmV1idx");

    /*
     * We should be able to save this op, as all members will be filled.
     * sal_memset(src_idx, INVALID_INDEX, count * sizeof(ln_index_t));
     */

    /* $ (HW) Copy split prefixes from old bucket to new bucket */
    SHR_IF_ERR_VERBOSE_EXIT(
        ver1_bucket_copy(u, m, db, ln,
                         src_group,
                         src_t,
                         sbbkts,
                         src_rbkts,
                         src_idx,

                         dst_group,
                         dst_t,
                         dbbkts,
                         dst_rbkts,

                         list,
                         count,
                         arg,
                         dst_valid_banks
                         ));
    /* Write dest rbkts */
    for (i = 0; i < ALPM_MAX_BANKS; i++) {
        if (dst_rbkts[i]) {
            if (split_aux) {
                bcmptm_rm_alpm_internals[u][m][db][ln].split_write1++;
            }
            hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, dst_rbkts[i]);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx,
                                            &dbbkts->rbuf[i][0],
                                            HIT_OP_AUTO,
                                            dbbkts->modify_bitmap[i],
                                            dbbkts->dt_bitmap[i]
                                            ));
        }
    }
    done = BUCKET_MOVE_COPY;


    /* $ Activate the pivot */
    if(split_arg) {
        SHR_IF_ERR_VERBOSE_EXIT2(
            bcmptm_rm_alpm_split_pivot_add(u, m, db, PREV_LEVEL(ln), split_arg,
                                           &split_pivot, split_aux),
                                           SHR_E_PARTIAL);
    } else {
        dst_pivot = ver1_rbkt_pivot_get(u, m, db, ln, dst_group);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln), dst_pivot,
                                        NULL,
                                        UPDATE_ALL));
    }

    /*
     * Do not write source right now.
     * - We will do compress if split case.
     * - We can leave them if alloc_and_move or merge case.
     */

    /* $ Free those moved elems from parent buckets */
    SHR_IF_ERR_EXIT(
        ver1_base_entries_free(u, m, db, ln, src_group, src_idx, count,
                               format_update));


exit:
    /* Assume bucket copy and split_pivot_add are subject to fail */
    if (SHR_FUNC_ERR()) {
        if (done == BUCKET_MOVE_COPY) {
            /* Restore dest rbkts */
            for (i = 0; i < ALPM_MAX_BANKS; i++) {
                if (dst_rbkts[i]) {
                    hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, dst_rbkts[i]);
                    if (SHR_FAILURE(
                        bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx, zero_de,
                                                    HIT_OP_NONE, 0, 0))) {
                        break;
                    }
                }
            }
        }
    }
    if (src_idx != NULL) {
        SHR_FREE(src_idx);
    }
    if (dbbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], dbbkts);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Allcoate a new bucket and move entries in old bucket to this new one.
 *
 * The original bucket will be freed, and elems in new bucket are likely
 * reorganized to be more efficient.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Source group
 * \param [in] arg New entry arg, in case new entry in split bucket.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_bucket_alloc_and_move(int u, int m, int db, int ln,
                           rbkt_group_t src_group,
                           alpm_arg_t *arg)
{
    int count, max_banks;
    rbkt_group_t new_group;
    alpm_pivot_t *src_pivot;
    enum {
        ALLOC_AND_MOVE_NONE,
        ALLOC_AND_MOVE_ALLOC,
        ALLOC_AND_MOVE_INDEX_ALLOC,
        ALLOC_AND_MOVE_MOVE
    } done = ALLOC_AND_MOVE_NONE;
    base_buckets_ver1_t *sbbkts = NULL;

    SHR_FUNC_ENTER(u);

    /*
     * Found a new place to hold remaining entries in parent bucket.
     * This parent group should be skipped in shuffle process
     * if required during group alloc.
     * Note: this group should be already full so as not to be
     * be shuffled, otherwise we will end up with ill state.
     */
    if (RBKT_FBANKS(u, m, db, ln, src_group)) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    max_banks = BKT_HDL_MAX_BANKS(u, m, db, ln);
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_alloc_best_effort(u, m, db, ln,
                                          max_banks,
                                          max_banks,
                                          max_banks,
                                          arg->data_type,
                                          &new_group));

    src_pivot = VER1_RBKT_PIVOT(u, m, db, ln, src_group);
    count = bcmptm_rm_alpm_trie_count(src_pivot->bkt->trie);
    VER1_RBKT_PIVOT(u, m, db, ln, new_group) = src_pivot;

    src_pivot->bkt->log_bkt = VER1_LOG_BKT(new_group);
    done = ALLOC_AND_MOVE_ALLOC;

    sbbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (sbbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        ver1_bucket_indexes_alloc(u, m, db, ln,
                                  new_group,
                                  &src_pivot->key.t,
                                  &src_pivot->bkt->list,
                                  count,
                                  true
                                  ));
    done = ALLOC_AND_MOVE_INDEX_ALLOC;

    SHR_IF_ERR_VERBOSE_EXIT(
        ver1_bucket_move(u, m, db, ln,
                         src_group,
                         &src_pivot->key.t,
                         new_group,
                         &src_pivot->key.t,
                         &src_pivot->bkt->list,
                         count,
                         arg,
                         NULL, /* Not split move */
                         NULL, /* No need to update format for alloc_and_move */
                         NULL,
                         sbbkts,
                         0
                         ));

    done = ALLOC_AND_MOVE_MOVE;
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_free(u, m, db, ln, src_group));
    ver1_bucket_indexes_set(u, m, db, ln, &src_pivot->bkt->list);
/*    src_pivot->bkt->log_bkt = VER1_LOG_BKT(new_group); */

exit:
    if (SHR_FUNC_ERR()) {
        switch (done) {
        case ALLOC_AND_MOVE_MOVE:
            ALPM_ASSERT(0); /* not supposed to fail */
        case ALLOC_AND_MOVE_INDEX_ALLOC:
            ver1_bucket_indexes_free(u, m, db, ln, new_group,
                                     &src_pivot->bkt->list, count);
        case ALLOC_AND_MOVE_ALLOC:
            src_pivot->bkt->log_bkt = VER1_LOG_BKT(src_group);
            ver1_rbkt_group_free(u, m, db, ln, new_group);
        case ALLOC_AND_MOVE_NONE:
        default:
            break;
        }

    }
    if (sbbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], sbbkts);
    }
    SHR_FUNC_EXIT();
}


static int
ver1_bucket_merge(int u, int m, int db, int ln,
                  alpm_pivot_t *parent_pivot,
                  alpm_pivot_t *child_pivot,
                  uint8_t parent_valid_banks)
{
    rbkt_group_t parent_group, child_group;
    base_buckets_ver1_t *sbbkts = NULL;
    alpm_arg_t *prev_arg = NULL;
    int count;

    SHR_FUNC_ENTER(u);
    parent_group = VER1_GROUP(parent_pivot->bkt->log_bkt);
    child_group = VER1_GROUP(child_pivot->bkt->log_bkt);

    sbbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (sbbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    VER1_RBKT_GROUP_SANITY(u, m, db, ln, child_group, false, 0);

    count = bcmptm_rm_alpm_trie_count(child_pivot->bkt->trie);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_merge(u, m, parent_pivot->bkt->trie,
                                  child_pivot->bkt->trie->trie,
                                  &child_pivot->key.t.trie_ip.key[0],
                                  child_pivot->key.t.len));

    SHR_IF_ERR_VERBOSE_EXIT(
        ver1_bucket_move(u, m, db, ln,
                         child_group,
                         &child_pivot->key.t,

                         parent_group,
                         &parent_pivot->key.t,
                         &child_pivot->bkt->list,
                         count,
                         NULL, /* No new entry */
                         NULL, /* Not split move */
                         NULL, /* Will delete pivot anyway */
                         NULL,
                         sbbkts,
                         parent_valid_banks));

    child_pivot->bkt->trie->trie = NULL;

    ver1_bucket_indexes_set(u, m, db, ln, &child_pivot->bkt->list);
    SHR_IF_ERR_EXIT(
        ver1_bkt_elems_connect(u, m, &parent_pivot->bkt->list,
                               parent_pivot->bkt->trie->trie,
                               NULL));
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_free(u, m, db, ln, child_group));

    prev_arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (prev_arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    bcmptm_rm_alpm_pivot_arg_init(u, m, db, PREV_LEVEL(ln),
                                  child_pivot, UPDATE_KEY,
                                  prev_arg);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_delete(u, m, PREV_LEVEL(ln), prev_arg));
exit:
/*    ALPM_ASSERT(!SHR_FUNC_ERR()); */
    VER1_RBKT_GROUP_SANITY(u, m, db, ln, parent_group, false, 0);
    VER1_RBKT_GROUP_SANITY(u, m, db, ln, child_group, false, 0);
    if (sbbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], sbbkts);
    }
    if (prev_arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], prev_arg);
    }
    SHR_FUNC_EXIT();
}

static int
ver1_bucket_merge_can_insert_l1(int u, int m, int db, alpm_pivot_t *child_pivot,
                                alpm_arg_t *new_arg, bool *ok)
{
    *ok = true;
    return SHR_E_NONE;
}


static int
ver1_bucket_merge_cb(rm_alpm_trie_node_t *ptrie, rm_alpm_trie_node_t *trie,
                     rm_alpm_trie_traverse_states_e_t *state, void *info)
{
    int u, m, rv;
    int db, ln, prev_ln;
    alpm_pivot_t *parent_pivot, *child_pivot, *l1_pivot;
    bucket_merge_ctrl_t *merge_ctrl = (bucket_merge_ctrl_t *) info;
    rbkt_group_t parent_group;
    rbkt_group_t child_group;
    int count;
    uint8_t parent_valid_banks;
    int freed_rbkts;
    bool ok;

    SHR_FUNC_ENTER(merge_ctrl->unit);
    /* Always reset state */
    *state = TRAVERSE_STATE_NONE;

    db = merge_ctrl->db;
    ln = merge_ctrl->ln;
    u = merge_ctrl->unit;
    m = merge_ctrl->m;
    prev_ln = PREV_LEVEL(ln);

    parent_pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, ptrie, pvtnode);
    child_pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, trie, pvtnode);
    if (!parent_pivot || !child_pivot ||
        merge_ctrl->pivot[prev_ln] == parent_pivot ||
        merge_ctrl->pivot[prev_ln] == child_pivot
       ) {
        return SHR_E_NONE;
    }
#ifdef ALPM_DEBUG
    {
    rm_alpm_trie_node_t *lpm;
    bcmptm_rm_alpm_trie_find_lpm2(merge_ctrl->pivot_trie[prev_ln],
                                  &child_pivot->key.t.trie_ip.key[0],
                                  child_pivot->key.t.len, &lpm);
    assert(lpm == ptrie);
    }
#endif
    if (ln == LEVEL3) {
        bcmptm_rm_alpm_pvt_find(u, m, db, LEVEL1, &child_pivot->key.t, &l1_pivot);
        if (l1_pivot->key.t.len > parent_pivot->key.t.len) {
            SHR_EXIT();
        }
    }

    parent_group = VER1_GROUP(parent_pivot->bkt->log_bkt);
    child_group = VER1_GROUP(child_pivot->bkt->log_bkt);
    if (parent_group == INVALID_GROUP ||
        child_group == INVALID_GROUP) {
        SHR_EXIT();
    }
    count = bcmptm_rm_alpm_trie_count(child_pivot->bkt->trie);

    VER1_RBKT_GROUP_SANITY(u, m, db, ln, child_group, false, 0);
    VER1_RBKT_GROUP_SANITY(u, m, db, ln, parent_group, false, 0);
    parent_valid_banks = RBKT_VBANKS(u, m, db, ln, parent_group);
    rv = ver1_bucket_indexes_alloc(u, m, db, ln,
                                   parent_group,
                                   &parent_pivot->key.t,
                                   &child_pivot->bkt->list,
                                   count,
                                   false
                                   );
    if (SHR_SUCCESS(rv)) {
        LOG_V(("Merge: Parent_vbanks=%d->%d, free child_vbanks=%d ",
               parent_valid_banks,
               RBKT_VBANKS(u, m, db, ln, parent_group),
               RBKT_VBANKS(u, m, db, ln, child_group)));

        freed_rbkts = RBKT_VBANKS(u, m, db, ln, child_group) -
                    (RBKT_VBANKS(u, m, db, ln, parent_group) - parent_valid_banks);
        if (merge_ctrl->free_pivot) {
            ok = false;
            SHR_IF_ERR_EXIT(
                ver1_bucket_merge_can_insert_l1(u, m, db, child_pivot,
                                                merge_ctrl->new_arg, &ok));
            if (ok && freed_rbkts) {
                SHR_IF_ERR_EXIT(
                    ver1_bucket_merge(u, m, db, ln, parent_pivot, child_pivot,
                                      parent_valid_banks));
                *state = TRAVERSE_STATE_DELETED;
                merge_ctrl->merges++;
                merge_ctrl->free_rbkts += freed_rbkts;
                *state = TRAVERSE_STATE_DONE;
            } else {
                ver1_bucket_indexes_free(u, m, db, ln, parent_group,
                                         &child_pivot->bkt->list, count);
            }
        } else {
            if ((merge_ctrl->again && freed_rbkts >= 0) ||
                (!merge_ctrl->again && freed_rbkts >= merge_ctrl->stop_count)) {
                SHR_IF_ERR_EXIT(
                    ver1_bucket_merge(u, m, db, ln, parent_pivot, child_pivot,
                                      parent_valid_banks));
                *state = TRAVERSE_STATE_DELETED;
                merge_ctrl->free_rbkts += freed_rbkts;
                if (merge_ctrl->free_rbkts >= merge_ctrl->stop_count) {
                    *state = TRAVERSE_STATE_DONE;
                }
            } else {
                ver1_bucket_indexes_free(u, m, db, ln, parent_group,
                                         &child_pivot->bkt->list, count);
            }
        }
        LOG_V(("After:used=%d total=%d\n",
               BKT_HDL_RBKTS_USED(u, m, db, ln),
               BKT_HDL_RBKTS_TOTAL(u, m, db, ln)));
    } else {
        VER1_RBKT_GROUP_SANITY(u, m, db, ln, child_group, false, 0);
        VER1_RBKT_GROUP_SANITY(u, m, db, ln, parent_group, false, 0);
    }

exit:
    if (SHR_FUNC_ERR()) {
        ver1_bucket_indexes_free(u, m, db, ln, parent_group,
                                 &child_pivot->bkt->list, count);
        VER1_RBKT_GROUP_SANITY(u, m, db, ln, child_group, false, 0);
        VER1_RBKT_GROUP_SANITY(u, m, db, ln, parent_group, false, 0);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse and do merge
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] stop_count Merge stop count.
 * \param [out] merge_count Merge count.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_bucket_traverse_merge(int u, int m, int db, int ln, int stop_count,
                           bool free_pivot,
                           alpm_pivot_t **pivot,
                           int *freed_rbkts,
                           bool again,
                           alpm_arg_t *new_arg)
{
    int ipv;
    bool ok = false;
    rm_alpm_trie_t *pivot_trie;
    bucket_merge_ctrl_t ctrl = {0};
    alpm_vrf_hdl_t *vrf_hdl;
    shr_dq_t *elem;
    bcmptm_rm_alpm_ln_stat_t *stat;
    int l3_freed_rbkts;

    SHR_FUNC_ENTER(u);
    if (stop_count == 0) {
        SHR_EXIT();
    }
    ctrl.unit = u;
    ctrl.db = db;
    ctrl.ln = ln;
    ctrl.stop_count = stop_count;
    ctrl.again = again;
    ctrl.free_pivot = free_pivot;
    ctrl.new_arg = new_arg;
    sal_memcpy(ctrl.pivot, pivot, sizeof(ctrl.pivot));
    for (ipv = IP_VER_4; ipv < IP_VER_COUNT; ipv++) {
        SHR_DQ_TRAVERSE(&DB_VRF_HDL_LIST(u, m, db, ipv), elem)
            vrf_hdl = SHR_DQ_ELEMENT_GET(alpm_vrf_hdl_t *, elem, dqnode);
            if (!vrf_hdl->in_use) {
                continue;
            }
            pivot_trie = vrf_hdl->pivot_trie[PREV_LEVEL(ln)];
            SHR_NULL_CHECK(pivot_trie, SHR_E_INTERNAL);
            ALPM_ASSERT(pivot_trie);
            sal_memcpy(ctrl.pivot_trie, vrf_hdl->pivot_trie, sizeof(vrf_hdl->pivot_trie));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_trie_traverse2(pivot_trie->trie,
                                              ver1_bucket_merge_cb,
                                              &ctrl,
                                              TRAVERSE_POSTORDER));
            ok = (ctrl.free_pivot && ctrl.merges) ||
                 (ctrl.free_rbkts >= ctrl.stop_count);
            if (ok) {
                break;
            }
        SHR_DQ_TRAVERSE_END(&DB_VRF_HDL_LIST(u, m, db, ipv), elem);
        if (ok) {
            break;
        }

    }
    stat = bcmptm_rm_alpm_ln_stats_get(u, m, db);
    if (ln != DB_LAST_LEVEL(u, m, db) && !again) {
        if (ok) {
            /*
             * Only merge the last level when level-2 merge succeeds,
             * this will ensure that resource check pass, and back out
             * won't be needed.
             */
            l3_freed_rbkts = 0;
            SHR_IF_ERR_EXIT(
                ver1_bucket_traverse_merge(u, m, db, NEXT_LEVEL(ln),
                                           MERGE_STOP_COUNT_SECURE, false,
                                           pivot, &l3_freed_rbkts, true, new_arg));
            /*
             * Further do level-2 buckets merges, so that we won't
             * kick-off level-3 merge frequently.
             */
            SHR_IF_ERR_EXIT(
                ver1_bucket_traverse_merge(u, m, db, ln,
                                           MERGE_STOP_COUNT_AGAIN, false,
                                           pivot, freed_rbkts, true, new_arg));
            if ((*freed_rbkts + ctrl.free_rbkts) < ctrl.stop_count) {
                ALPM_ASSERT(0);
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }
    if (freed_rbkts) {
        *freed_rbkts += ctrl.free_rbkts;
    }
#ifdef ALPM_DEBUG
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        bcmptm_rm_alpm_buckets_sanity(u, m, db, ln, sanity_vrf_traverse_merge, false);
    }
#endif
    stat->c_traverse_merge++;
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Callback Functions
 */

/*!
 * \brief Encode and write the base entry into HW.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] arg ALPM arg
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_base_entry_encode_write(int u, int m, int ln, alpm_arg_t *arg)
{
    int rbkt_idx;
    ln_base_entry_t base_entry;
    rbkt_t *rbkt;
    format_type_t format_type;
    ln_index_t ent_idx;
    int ent_ofs, hw_idx;
    int pivot_len, db;
    base_buckets_ver1_t *bbkts = NULL;
    uint8_t hit_support, hit_version;
    int full;
    uint32_t hit_entry[1];

    SHR_FUNC_ENTER(u);
    ent_idx = arg->index[ln];

    db = arg->db;
    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, rbkt_idx);
    bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (bbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    if (arg->new_insert && RBKT_EBMP(u, m, db, ln, rbkt_idx) == 1) {
        sal_memset(&bbkts->rbuf[0][0], 0, sizeof(bbkts->rbuf[0]));
        /* Not necessary. */
        bbkts->dt_bitmap[0] = 0;
    } else {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0],
                                       &bbkts->dt_bitmap[0]));
    }

    rbkt = &RBKT(u, m, db, ln, rbkt_idx);

    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);

    if (arg->encode_data_only) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_raw_bucket_entry_get(u, m, format_type,
                                                   rbkt->format,
                                                   &base_entry[0],
                                                   ent_ofs,
                                                   &bbkts->rbuf[0][0]));
        pivot_len = 0; /* Write data only */
    } else {
        sal_memset(base_entry, 0, sizeof(base_entry));
        pivot_len = arg->pivot[PREV_LEVEL(ln)]->key.t.len;
    }

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_encode(u, m, format_type,
                                            rbkt->format,
                                            pivot_len,
                                            arg,
                                            &base_entry[0]));
#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE) && !arg->encode_data_only) {
        alpm_arg_t arg2;
        bcmptm_rm_alpm_arg_init(u, m, &arg2);
        /* Decode needs: ipv, previous level trie_ip */
        arg2.key.t.max_bits = arg->key.t.max_bits;
        sal_memcpy(&arg2.key.t.trie_ip,
                   &arg->pivot[PREV_LEVEL(ln)]->key.t.trie_ip,
                   sizeof(trie_ip_t));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_decode(u, m, format_type,
                                                rbkt->format,
                                                pivot_len,
                                                &arg2,
                                                &base_entry[0]));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_alpm_api_key_create(u, m, arg2.key.t.max_bits,
                                           &arg2.key.t.trie_ip,
                                           arg2.key.t.len,
                                           arg2.key.ip_addr));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_len_to_mask(u, m, arg2.key.t.max_bits, arg2.key.t.len,
                                       &arg2.key.ip_mask[0]));

        ALPM_ASSERT(!sal_memcmp(arg2.key.ip_addr, arg->key.ip_addr,
                                sizeof(alpm_ip_t)));
        ALPM_ASSERT(!sal_memcmp(arg2.key.ip_mask, arg->key.ip_mask,
                                sizeof(alpm_ip_t)));
        arg2.key.t.w_vrf = arg->key.t.w_vrf;
        arg2.key.t.ipv = arg->key.t.ipv;
        arg2.key.t.app = arg->key.t.app;
        ALPM_ASSERT(!sal_memcmp(&arg2.key.t, &arg->key.t, sizeof(key_tuple_t)));
    }
#endif

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_set(u, m, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &bbkts->rbuf[0][0]));

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                          rbkt->format,
                                          NULL, &full, NULL));

    if (format_type == FORMAT_ROUTE) {
        if (full) {
            ver1_dt_bitmap_set(u, m, ent_ofs, &bbkts->dt_bitmap[0], arg->ad.user_data_type);
        }
        /* Set actual data type */
        arg->ad.actual_data_type = ver1_full_to_dt(u, m, full);
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0],
                                    HIT_OP_NONE, 0, bbkts->dt_bitmap[0]));

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));

    hit_support = bcmptm_rm_alpm_hit_support(u, m);
    if (hit_support != HIT_MODE_DISABLE) { /* Write hit */
        hit_version = bcmptm_rm_alpm_hit_version(u, m);
        if (hit_version == ALPM_HIT_VERSION_0) {
            if (hit_support == HIT_MODE_FORCE_CLEAR) {
                hit_entry[0] = 0;
            } else {
                hit_entry[0] = 0xFFFF;
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_hit_set(u, m, ln, ent_ofs, hit_entry, arg->hit));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_hit(u, m, ln, hw_idx, hit_entry));
        } else if (hit_version == ALPM_HIT_VERSION_1) {
            hit_entry[0] = arg->hit;
            hw_idx = LN_HIT_INDEX(hw_idx, ent_ofs);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_hit(u, m, ln, hw_idx, hit_entry));
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        bcmptm_rm_alpm_internals[u][m][db][ln].write++;
    }


exit:
    if (bbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], bbkts);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read and decode the base entry from HW.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [out] arg ALPM arg
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_base_entry_decode_read(int u, int m, int ln, alpm_arg_t *arg)
{
    int rbkt_idx;
    ln_base_entry_t base_entry; /* = {0}; */
    rbkt_t *rbkt;
    format_type_t format_type;
    int ent_ofs, hw_idx;
    ln_index_t ent_idx;
    alpm_pivot_t *pivot;
    int db;
    uint8_t pvt_len;
    base_buckets_ver1_t *bbkts = NULL;
    uint32_t hit_entry;
    uint8_t hit, hit_version;
    int full;

    SHR_FUNC_ENTER(u);

    /* Decode needs: ipv, previous level trie_ip */
    db = arg->db;
    if (!arg->decode_data_only) {
        pivot = arg->pivot[PREV_LEVEL(ln)];
        arg->key.t.trie_ip = pivot->key.t.trie_ip;
        arg->key.t.max_bits = pivot->key.t.max_bits;
        pvt_len = pivot->key.t.len;
    } else {
        pvt_len = 0;
    }
    ent_idx = arg->index[ln];
    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, rbkt_idx);
    bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (bbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0],
                                   &bbkts->dt_bitmap[0]));
    rbkt = &RBKT(u, m, db, ln, rbkt_idx);

    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);
    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_get(u, m, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &bbkts->rbuf[0][0]));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_decode(u, m, format_type,
                                            rbkt->format,
                                            pvt_len,
                                            arg,
                                            &base_entry[0]));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                          rbkt->format,
                                          NULL,
                                          &full,
                                          NULL));
    if (format_type == FORMAT_ROUTE) {
        if (full) {
            if (bcmptm_rm_alpm_cache_data_type(u, m)) {
                arg->ad.user_data_type =
                    ver1_dt_bitmap_get(u, m, ent_ofs, bbkts->dt_bitmap[0]);
            }
        } else {
            arg->ad.user_data_type = DATA_REDUCED;
        }
        arg->data_type = arg->ad.user_data_type;
    } else {
        arg->data_type = bcmptm_rm_alpm_mid_dt_get(u, m, db, arg->key.t.app);
    }

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));

    if (!arg->decode_data_only) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_api_key_create(u, m, arg->key.t.max_bits,
                                          &arg->key.t.trie_ip,
                                          arg->key.t.len,
                                          &arg->key.ip_addr[0]));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_len_to_mask(u, m, arg->key.t.max_bits,
                                       arg->key.t.len,
                                       &arg->key.ip_mask[0]));
    }

    if (bcmptm_rm_alpm_hit_support(u, m) != HIT_MODE_DISABLE) { /* Read hit */
        hit_version = bcmptm_rm_alpm_hit_version(u, m);
        if (hit_version == ALPM_HIT_VERSION_0) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_read_ln_hit(u, m, ln, hw_idx, &hit_entry));
            /* Get hit bit */
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_hit_get(u, m, ln, ent_ofs, &hit_entry, &hit));
            arg->hit = hit;
        } else if (hit_version == ALPM_HIT_VERSION_1) {
            hw_idx = LN_HIT_INDEX(hw_idx, ent_ofs);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_read_ln_hit(u, m, ln, hw_idx, &hit_entry));
            arg->hit = hit_entry;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    if (bbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], bbkts);
    }
    SHR_FUNC_EXIT();
}

static int
ver1_bucket_resource_check_level(int u, int m, int db, int8_t ln,
                                 alpm_pivot_t **pivot, int extra)
{
    int free_rbkts;
    int freed_rbkts = 0;
    int min_free;
    bcmptm_rm_alpm_ln_stat_t *stat;

    SHR_FUNC_ENTER(u);

    min_free = BKT_HDL_THRD_BANKS(u, m, db, ln) + extra;
    free_rbkts = BKT_HDL_RBKTS_TOTAL(u, m, db, ln) -
                 BKT_HDL_RBKTS_USED(u, m, db, ln);
    LOG_D(("Check resource: level=%d free_rbkts=%d \n", ln, free_rbkts));
    if (free_rbkts <= min_free) {
        stat = bcmptm_rm_alpm_ln_stats_get(u, m, db);
        if ((stat->c_last_full == 0) || /* No full yet. */
            (stat->c_curr < stat->c_last_full * MERGE_RATIO)) {
            SHR_IF_ERR_EXIT(
                ver1_bucket_traverse_merge(u, m, db, ln,
                                           MERGE_STOP_COUNT(free_rbkts - min_free),
                                           false, /* Free pivot */
                                           pivot,
                                           &freed_rbkts,
                                           false,
                                           NULL));
            free_rbkts = BKT_HDL_RBKTS_TOTAL(u, m, db, ln) -
                         BKT_HDL_RBKTS_USED(u, m, db, ln);


            /*
             * MERGE_STOP_COUNT must be sufficient to let resource check pass.
             * Merge partially done, but resource check still fails will
             * result in ill state.
             */
            ALPM_ASSERT(freed_rbkts == 0 || free_rbkts > min_free);
        }
    }
    if (free_rbkts <= min_free) {
        if (freed_rbkts > 0) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "Some merge done, but freed_rbkts %d, min_free %d.\n"),
                            freed_rbkts, min_free));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(u, "Level %d resource full\n"), ln + 1));
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_bucket_resource_check(int u, int m, alpm_arg_t *arg, int extra)
{
    int8_t ln, db = arg->db;
    alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, m, db);

    SHR_FUNC_ENTER(u);
    if (bcmptm_rm_alpm_l1_can_insert(u, m, arg) == 0) {
        /*
         * When level-1 resource is full, we just return full.
         * It's true that traverse merge may free some level-1 pivots,
         * but we cannot tell if the freed pivot will be useful
         * for the possible new generating level-1 pivot.
         * The SW state must not be changed unless we are 100% sure resource
         * will be sufficient for the given new entry.
         * If user wants to exploit the resource beyond the safe check,
         * it's likely that they may end up with resource full & state changed,
         * in which case, abort must be processed to restore correct SW state.
         * This is only possible when enable_atomic_trans=1, and user has
         * been given the option to do this.
         */
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(u, "Level 1 resource full\n")));
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    for (ln = LEVEL2; ln < database_ptr->max_levels; ln++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            ver1_bucket_resource_check_level(u, m, db, ln, &arg->pivot[0], extra));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Insert an entry into a bucket.
 *
 * This routine insert both SW and HW resoureces.
 * Ideally, just take free base entry. If that is not possible,
 * take free bank, and pick the optimal format. If that is again
 * not possible, expand the bucket group.
 * By expanding the group, it could expand left/right, or move, or
 * shuffle then move.
 * Must ensure no packet misses during expand/move/shuffle process.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] pivot Pivot pointing the target bucket.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_bucket_insert(int u, int m, int ln, alpm_arg_t *arg,
                   alpm_pivot_t *pivot)
{
    bool format_update = FALSE;
    int format_value;
    int bucket_len = arg->key.t.len - pivot->key.t.len;
    rbkt_group_t group;
    rbkt_idx_t new_rbkt = 0;
    ln_index_t ent_idx = INVALID_INDEX;
    int rv;
    int db = arg->db;
    SHR_FUNC_ENTER(u);

    if (pivot->bkt->log_bkt == INVALID_LOG_BKT) {
        /*
         * If pivot exists, but bucket group is invalid,
         * this implies the pivot is just itself without a bucket.
         * So we should alloc a bucket group for it first.
         */
        int max_banks;

        /* Only one being the to-be-insert entry */
        ALPM_ASSERT(pivot->bkt->trie->trie == NULL ||
                    pivot->bkt->trie->trie->count == 1);

        if (ln == DB_LAST_LEVEL(u, m, db)) {
            /*
             * Only check the last level, as this will be an empty bucket,
             * meaning, split won't be required for this entry.
             */
            rv = ver1_bucket_resource_check_level(u, m, db, ln, &arg->pivot[0],
                                                  arg->dt_update ?
                                                  EXTRA_UPDATE_DT:
                                                  EXTRA_NEW_INSERT);
            if (rv == SHR_E_RESOURCE) {
                arg->resouce_full = 1;
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            SHR_IF_ERR_EXIT(rv);
        }

        max_banks = BKT_HDL_MAX_BANKS(u, m, db, ln);
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_alloc_best_effort(u, m, db, ln,
                                              max_banks,
                                              max_banks,
                                              max_banks,
                                              arg->data_type,
                                              &group));
        format_update = TRUE;

        VER1_RBKT_PIVOT(u, m, db, ln, group) = pivot;
        pivot->bkt->log_bkt = VER1_LOG_BKT(group);
        ALPM_ASSERT(group != RESERVED_GROUP);
    } else {
        group = VER1_GROUP(pivot->bkt->log_bkt);
        ALPM_ASSERT(group != RESERVED_GROUP);
    }
    rv = ver1_base_entry_alloc(u, m, db, ln, group,
                               bucket_len,
                               arg->key.t.max_bits,
                               arg->data_type,
                               &ent_idx,
                               &format_update,
                               &format_value);
    while (rv == SHR_E_RESOURCE) {
        if (ln == DB_LAST_LEVEL(u, m, db)) {
            rv = ver1_bucket_resource_check(u, m, arg,
                                            arg->dt_update ?
                                            EXTRA_UPDATE_DT:
                                            EXTRA_NEW_INSERT);
            if (rv == SHR_E_RESOURCE) {
                arg->resouce_full = 1;
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            SHR_IF_ERR_EXIT(rv);
            if (RBKT_FBANKS(u, m, db, ln, group) &&
                RBKT_VBANKS(u, m, db, ln, group) < BKT_HDL_MAX_BANKS(u, m, db, ln)) {
                rv = ver1_base_entry_alloc(u, m, db, ln, group,
                                           bucket_len,
                                           arg->key.t.max_bits,
                                           arg->data_type,
                                           &ent_idx,
                                           &format_update,
                                           &format_value);
                break;
            }
        }

        if (RBKT_VBANKS(u, m, db, ln, group) ==
            BKT_HDL_MAX_BANKS(u, m, db, ln)) {
            /* Already a full group, this is where split should kick in */
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }

        /*
         * We've run out of banks, expand group with free banks..
         */
        SHR_IF_ERR_EXIT(
            ver1_rbkt_alloc(u, m, db, ln, group, &new_rbkt, &format_update));
        ALPM_ASSERT(RBKT_EBMP(u, m, db, ln, new_rbkt) == 0);
        SHR_IF_ERR_EXIT(
            ver1_base_entry_new_rbkt_assign(u, m, db, ln,
                                             bucket_len,
                                             arg->key.t.max_bits,
                                             arg->data_type,
                                             new_rbkt,
                                             &ent_idx,
                                             &format_update,
                                             &format_value));
        rv = SHR_E_NONE; /* Loop break. */
    };
    SHR_IF_ERR_EXIT(rv);

    ALPM_ASSERT(ent_idx != INVALID_INDEX);
    arg->orig_index = arg->pivot[ln]->index;
    arg->pivot[ln]->index = ent_idx;
    arg->index[ln] = ent_idx;
    arg->pivot[PREV_LEVEL(ln)] = pivot; /* Set pivot for pivot_len */
    arg->new_insert = 1;
    SHR_IF_ERR_EXIT(
        ver1_base_entry_encode_write(u, m, ln, arg));
    if (format_update) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln), pivot, NULL,
                                        UPDATE_ALL));
    }
#ifdef ALPM_DEBUG
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        bcmptm_rm_alpm_buckets_sanity(u, m, db, ln, sanity_vrf_insert, false);
    }
#endif

exit:
    SHR_FUNC_EXIT();
}




/*!
 * \brief Delete an entry within a bucket.
 *
 * This routine delete both SW and HW resoureces. After entry deleted
 * the rbkts should continue to be contiguous.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] pivot Pivot pointing the target bucket.
 * \param [in] ent_idx Entry index to delete
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_bucket_delete(int u, int m, int db, int ln,
                   alpm_pivot_t *pivot,
                   ln_index_t ent_idx)
{
    rbkt_group_t group = VER1_GROUP(pivot->bkt->log_bkt);
    bool format_update = FALSE;
    rbkt_idx_t vbanks;
    SHR_FUNC_ENTER(u);

    vbanks = RBKT_VBANKS(u, m, db, ln, group);
    SHR_IF_ERR_EXIT(
        ver1_base_entry_free(u, m, db, ln, group, ent_idx, TRUE, &format_update));

    if (format_update) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln), pivot, NULL,
                                        UPDATE_ALL));
        /* Clear rbkt after format update */
        if (RBKT_VBANKS(u, m, db, ln, group) < vbanks) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_clear(u, m, db, ln, group + vbanks - 1));
        }
    } else {
        SHR_IF_ERR_EXIT(
            ver1_base_entry_clear(u, m, db, ln, ent_idx));
    }
#ifdef ALPM_DEBUG
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        bcmptm_rm_alpm_buckets_sanity(u, m, db, ln, sanity_vrf_delete, false);
    }
#endif
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_bucket_special_split(int u, int m, int ln,
                          alpm_pivot_t *parent_pivot,
                          rm_alpm_trie_node_t **split_root,
                          trie_ip_t *pivot_key,
                          uint32_t *pivot_len,
                          bkt_split_aux_t *split_aux
                          )
{
    int rv;
    uint32_t tmp_pivot_len;
    trie_ip_t tmp_pivot_key;
    /*
     * Special split -
     * A split point is already decided, and we just use it.
     * split at that point. This should be guaranteed to
     * succeed.
     */
#ifdef ALPM_DEBUG
    if (split_dump) {
        cli_out(" Level %d trie before special bucket split\n", ln + 1);
        bcmptm_rm_alpm_trie_dump(parent_pivot->bkt->trie, NULL, NULL);
        if (split_aux->aux_pivot_len < parent_pivot->key.t.len) {
            cli_out(" Something not right. split_len %d less"
                    " than parent_len %d \n",
                    split_aux->aux_pivot_len,
                    parent_pivot->key.t.len);
            assert(0);
        }
    }
#endif
    rv = bcmptm_rm_alpm_trie_split2(u, m, parent_pivot->bkt->trie,
                                    &split_aux->aux_pivot_key.key[0],
                                    split_aux->aux_pivot_len,

                                    &tmp_pivot_key.key[0],
                                    &tmp_pivot_len,
                                    split_root);
#ifdef ALPM_DEBUG
    if (split_dump) {
        cli_out(" Level %d trie after special bucket split2\n", ln + 1);
        cli_out(" Parent remains:\n");
        bcmptm_rm_alpm_trie_dump(parent_pivot->bkt->trie, NULL, NULL);
        cli_out(" New bucket :\n");
        bcmptm_rm_alpm_trie_dump2(*split_root, NULL, NULL, parent_pivot->bkt->trie->mkl);
    }
#endif
    /* Not expect to fail */
    if (SHR_FAILURE(rv)) {
        ALPM_ASSERT(0);
        return rv;
    }
    ALPM_ASSERT(split_aux->aux_pivot_len == tmp_pivot_len);
    ALPM_ASSERT(*split_root);

    *pivot_key = split_aux->aux_pivot_key;
    *pivot_len = split_aux->aux_pivot_len;
    return SHR_E_NONE;
}



/*!
 * \brief Callback for test propagation
 *
 * \param [in] node Trie node
 * \param [in] info Callback helper data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static int
test_propa_cb(rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info)
{
    int *affected = info->user_data;
    *affected = 1;
    return SHR_E_LIMIT; /* Break early. */
}

static int
ver1_bucket_trie_split(int u, int m, int db, int ln,
                       alpm_pivot_t *parent_pivot,
                       rm_alpm_trie_node_t **split_root,
                       trie_ip_t *pivot_key,
                       uint32_t *pivot_len,
                       bkt_split_aux_t *split_aux
                       )
{
    int rv;
    int msl, msc;
    uint32_t middle_level = (ln < DB_LAST_LEVEL(u, m, db));
    rm_alpm_trie_node_t *split_node;

    SHR_FUNC_ENTER(u);
    if (split_aux->epartial_reason == EPARTIAL_REASON_L3BKTAC_SPLIT) {
        SHR_ERR_EXIT(ver1_bucket_special_split(u, m, ln, parent_pivot,
                                             split_root,
                                             pivot_key,
                                             pivot_len,
                                             split_aux));
        /*
         * The epartial_reason is a valid signal for split_aux taking
         * effect. The split_aux is in/out parameter, we need
         * reset the signal once it has been used.
         * This is a double insurance, as for double split
         * case, it is already reset after it being used.
         * if (middle_level) {
         *     split_aux->epartial_reason = 0;
         * }
 */

    }

#ifdef ALPM_DEBUG
        sal_memset(pivot_key, 0, sizeof(*pivot_key));
#endif


    ver1_trie_msl(u, m, parent_pivot, &msl, &msc, (!middle_level && max_split_len_cal));

#ifdef ALPM_DEBUG
    if (split_dump) {
        cli_out(" Level %d trie before bucket split\n", ln + 1);
        bcmptm_rm_alpm_trie_dump(parent_pivot->bkt->trie, NULL, NULL);
    }
#endif
    rv = bcmptm_rm_alpm_trie_split(u, m, parent_pivot->bkt->trie,
                                   msl,
                                   &pivot_key->key[0],
                                   pivot_len,
                                   split_root,
                                   &split_node,
                                   NULL,
                                   middle_level ? split_aux->delayed_payload_spilt: 0,
                                   msc);
#ifdef ALPM_DEBUG
    if (split_dump) {
        cli_out(" Level %d trie after bucket split\n", ln + 1);
        cli_out(" Parent remains:\n");
        bcmptm_rm_alpm_trie_dump(parent_pivot->bkt->trie, NULL, NULL);
        cli_out(" New bucket :\n");
        bcmptm_rm_alpm_trie_dump2(*split_root, NULL, NULL, parent_pivot->bkt->trie->mkl);
    }
#endif

    if (middle_level) {
        if (rv == SHR_E_NOT_FOUND && split_aux->delayed_payload_spilt) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_trie_split(u, m, parent_pivot->bkt->trie,
                                          msl,
                                          &pivot_key->key[0],
                                          pivot_len,
                                          split_root,
                                          &split_node,
                                          NULL,
                                          0,
                                          msc));
        }
    } else {
        SHR_ERR_EXIT(rv);
    }

    if (split_node->type != PAYLOAD) {
        /* The spivot is internal node, we must make it payload */

        /*
         * This non-payload split point comes with two scenarios:
         *
         * a) The internal (level-2) split point will affect a level-3 bucket
         * in a way that, some l3 entries in that bucket can be now
         * in the wrong bucket. To fix this,
         * that level-3 bucket should also be spilt, and those l3 entries
         * should be moved to the new split bucket.
         * We now have to split two level-3 buckets: the one holding
         * to-be-inserted entry (first split), and the one holding entrie that is
         * affected by the first split (second split).
         * The solution is to find the second split point, and
         * then fall back to the first split entrance (the last level) and
         * conduct both splits.
         * We choose a special error code SHR_E_PARTIAL to flag
         * this case.
         *
         * b) The internal split point does not affect any other l3 bucket.
         * This can be that, there is no affected pivot at all, or the pivot
         * exits but there is no entry affected.
         * Either way, this only implies no second split needed. But we
         * still need to fall back to entrance point, and redo the split
         * based on this new split point. This is necessary to ensure
         * that level-1 pivot always be a payload in level-2.
         */


        rm_alpm_trie_node_t *split_lpm_node;
        rv = bcmptm_rm_alpm_trie_find_lpm2(parent_pivot->bkt->trie,
                                           &pivot_key->key[0],
                                           *pivot_len,
                                           &split_lpm_node);
        if (SHR_SUCCESS(rv)) {
            /* Extract those prefixes in found_lpm */
            alpm_pivot_t *l3bktc_pivot;
            rm_alpm_trie_bpm_cb_info_t cb_info;
            int l3bktc_pivot_newly_create = 0;
            int affected = 0;
            /* The result is same as pivot find to level-2 pivot trie. */
            l3bktc_pivot = TRIE_ELEMENT_GET(alpm_pivot_t*,
                            split_lpm_node, bktnode);

            /*
             * split_aux->l3_split_root
             * !NULL - double split.
             * NULL  - delayed pivot insert.
             *
             * Two cases for sub_parent_pivot->bkt->trie->trie == NULL.
             * 1. When the sub_parent_pivot is newly created it has
             *    no bucket attached. In this case, bpm == NULL.
             * 2. The sub_parent_pivot is not newly created, bpm != NULL.
             */
            if (split_aux->l3_split_root) {
                if (l3bktc_pivot->bkt->trie->trie == NULL &&
                    l3bktc_pivot->bkt->bpm == NULL) {
                    /*
                     * This L3BKTC pivot is newly created.
                     * There might be entries affected in the new bucket.
                     */
                    l3bktc_pivot->bkt->trie->trie = split_aux->l3_split_root;
                    split_aux->l3_split_root = NULL;
                    l3bktc_pivot_newly_create = 1;
                } /* Otherwise L3BKTC pivot is an existing one with no bucket */
            } else {
                /* This is delayed pivot_insert. */
                if (l3bktc_pivot->bkt->trie->trie == NULL && \
                    l3bktc_pivot->bkt->bpm == NULL) {
                    /* This L3BKTC pivot is newly created. No entries affected. */
                    SHR_EXIT();
                }
            }

            if (l3bktc_pivot->bkt->trie) {
                cb_info.user_data = &affected;
                rv = bcmptm_rm_alpm_trie_propagate(l3bktc_pivot->bkt->trie,
                                                   &pivot_key->key[0],
                                                   *pivot_len,
                                                   PROPOGATE_DONT_CARE,
                                                   test_propa_cb,
                                                   &cb_info,
                                                   false);
            }
            if (l3bktc_pivot_newly_create) { /* Reset back */
                l3bktc_pivot->bkt->trie->trie = NULL;
            }

            if (affected) {
                split_aux->aux_pivot_len = *pivot_len;
                split_aux->aux_pivot_key = *pivot_key;
                if (l3bktc_pivot_newly_create) {
                    split_aux->l3_pivot = NULL;
                } else {
                    split_aux->l3_pivot = l3bktc_pivot;
                }
                split_aux->epartial_reason = EPARTIAL_REASON_L3BKTAC_SPLIT;

                SHR_ERR_EXIT(SHR_E_PARTIAL);
            } else {
                /* LPM exists, but no affected entries */
                split_aux->epartial_reason = EPARTIAL_REASON_L3BKT_DETACHED;

                /* $ merge back */
                /* SHR_ERR_EXIT(SHR_E_PARTIAL); */
            }
        } else { /* LPM does not exists */
            split_aux->epartial_reason = EPARTIAL_REASON_L3BKT_DETACHED;
            /* $ merge back */
            /* SHR_ERR_EXIT(SHR_E_PARTIAL); */
        }

    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Split a bucket into two
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] arg ALPM arg for new entry
 * \param [in] parent_pivot Pivot whose bucket to be split
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_bucket_split_insert(int u, int m, int ln, alpm_arg_t *arg,
                         alpm_pivot_t *parent_pivot,
                         bkt_split_aux_t *split_aux)
{
    int rv;
    rbkt_group_t new_group;
    alpm_pivot_t *split_pivot = NULL;
    bool format_update = FALSE;
    rm_alpm_trie_node_t *split_root;
    uint32_t pivot_len;
    trie_ip_t pivot_key;
    int prev_ln = ln - 1;
    int8_t vbanks, rbkt_ofs;
    uint8_t db = arg->db, i;
    rbkt_group_t parent_group;
    enum {
        SPLIT_NONE,
        SPLIT_ALLOC_GROUP,
        SPLIT_TRIE_SPLIT,
        SPLIT_BUCKET_CONNECT,
        SPLIT_BUCKET_INDEXES_ALLOC,
        SPLIT_BUCKET_MOVE_AND_ACTIVATE,
        SPLIT_OLD_BUCKET_CLEANUP,
    } done = SPLIT_NONE;
    bool new_in_parent = false, new_in_split = false;
    alpm_arg_t *split_arg = NULL;
    shr_dq_t split_list;
    base_buckets_ver1_t *sbbkts = NULL;
    uint32_t hw_idx;
    int delete_rbkts;
    rbkt_idx_t last_rbkt;

    SHR_FUNC_ENTER(u);

    /*
     * $ Allocate a new group.
     * First try with simple alloc (no shuffle) for several sizes.
     * The shuffle alloc kicks in right before trying with least size.
     */
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_alloc_best_effort(u, m, db, ln,
                                          BKT_HDL_MAX_BANKS(u, m, db, ln),
                                          BKT_HDL_MAX_BANKS(u, m, db, ln),
                                          BKT_HDL_MAX_BANKS(u, m, db, ln),
                                          arg->data_type,
                                          &new_group
                                          ));
    arg->log_bkt[prev_ln] = VER1_LOG_BKT(new_group);
    done = SPLIT_ALLOC_GROUP;

    /* $ Trie split */
    /*
     * max_split_len and max_split_count.
     * The larger the split_len, the less efficient format it takes from
     * previous level.
     * The more split payloads, the more rbkts it needs.
     * Ideally, the max_split_count should be a result of function considering
     * new group rbkts number, and the optimal formats for split prefixes.
     * However the split prefixes are a result of the split process accepting
     * max_split_count as input.
     * Ideally, the max_split_len should be just perfect to fit the formats
     * for split prefixes, again the split prefixes are a result of the split
     * process accepting max_split_len as input.
     * We are facing with a situation called causality dilemma, aka,
     * a chicken or the egg situation.
     * The possible best way is to try different parameters until a optimial
     * solution found. But, as a SDK algorithm, we don't have time bandwith
     * for such complex algorithm.
     */
    /*    parent_pivot = (alpm_pivot_t *)arg->pivot[prev_ln]; */
    parent_group = VER1_GROUP(parent_pivot->bkt->log_bkt);
    VER1_RBKT_GROUP_SANITY(u, m, db, ln, parent_group, false, 0);

    ALPM_ASSERT(split_aux);

    rv = ver1_bucket_trie_split(u, m, db, ln, parent_pivot,
                                &split_root,
                                &pivot_key,
                                &pivot_len,
                                split_aux);
    if (rv == SHR_E_PARTIAL) {
        done = SPLIT_TRIE_SPLIT;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    done = SPLIT_TRIE_SPLIT;

    /* $ Connect bkt elems in new split bucket */
    SHR_IF_ERR_EXIT(
        ver1_bkt_elems_connect(u, m, &split_list, split_root,
                               &new_in_split));

    /* $ Connect bkt elems in parent bucket */
    SHR_IF_ERR_EXIT(
        ver1_bkt_elems_connect(u, m, &parent_pivot->bkt->list,
                               parent_pivot->bkt->trie->trie,
                               &new_in_parent));

    done = SPLIT_BUCKET_CONNECT;

    /* $ Split arg alloc and init */
    split_arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (split_arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    bcmptm_rm_alpm_split_arg_init(u, m, db, prev_ln,
                                  parent_pivot->index,
                                  &parent_pivot->key,
                                  &pivot_key,
                                  pivot_len,
                                  arg->log_bkt[prev_ln],
                                  split_arg);

    sbbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (sbbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(sbbkts->modify_bitmap, 0, sizeof(sbbkts->modify_bitmap));

    rv = ver1_bucket_indexes_alloc(u, m, db, ln,
                                   new_group,
                                   &split_arg->key.t,
                                   &split_list,
                                   split_root->count,
                                   true
                                   );
    if (rv == SHR_E_RESOURCE) {
        ALPM_ASSERT(split_aux->retry == SPLIT_RETRY_NONE);
        split_aux->retry = SPLIT_RETRY_TWO;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    done = SPLIT_BUCKET_INDEXES_ALLOC;

    if (DB_LAST_LEVEL(u, m, db) == ln) {
        split_aux->l3_split_root = split_root;
    }
    vbanks = RBKT_VBANKS(u, m, db, ln, parent_group);
    rv = ver1_bucket_move(u, m, db, ln,
                          parent_group,
                          &parent_pivot->key.t,
                          new_group,
                          &split_arg->key.t,
                          &split_list,
                          split_root->count,
                          arg,
                          split_arg,
                          &format_update,
                          split_aux,
                          sbbkts,
                          0);
    if (rv == SHR_E_PARTIAL) {
        ALPM_ASSERT(ln == LEVEL3);
        if (split_aux->epartial_reason == EPARTIAL_REASON_L3BKT_DETACHED) {
            ALPM_ASSERT(split_aux->l3_pivot == NULL);
            split_aux->aux_pivot_key = pivot_key;
            split_aux->aux_pivot_len = pivot_len;
            split_aux->l3_pivot = parent_pivot;
        }
        /* Must properly roll back on E_PARTIAL. */
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    SHR_IF_ERR_EXIT(rv);

    split_pivot = split_arg->pivot[prev_ln];
    SHR_DQ_SWAP_HEAD(&split_pivot->bkt->list, &split_list);
    if (new_in_split) {
        arg->pivot[prev_ln] = split_pivot;
    }

    ALPM_ASSERT(split_pivot->bkt->trie);
    split_pivot->bkt->trie->trie = split_root;
    VER1_RBKT_PIVOT(u, m, db, ln, new_group) = split_pivot;
    done = SPLIT_BUCKET_MOVE_AND_ACTIVATE;

    /* $ Compress the parent buckets to make them continuous */
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_compress(u, m, db, ln, parent_group,
                                 vbanks, &format_update, sbbkts));

    /* $ Invalidate moved elems from old bucket in HW */
    /* Write source rbkts */
    for (i = 0; i < RBKT_VBANKS(u, m, db, ln, parent_group); i++) {
        rbkt_ofs = sbbkts->rbkt_ofs[i];
        if (rbkt_ofs != -1) {
            hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, parent_group + i);
            bcmptm_rm_alpm_internals[u][m][db][ln].split_write2++;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx,
                                            &sbbkts->rbuf[rbkt_ofs][0],
                                            HIT_OP_AUTO,
                                            sbbkts->modify_bitmap[rbkt_ofs],
                                            sbbkts->dt_bitmap[rbkt_ofs]
                                            ));
        }
    }

    /* $ After the compression, we may find that the parent bucket is empty */
    if (RBKT_VBANKS(u, m, db, ln, parent_group) == 0 && !new_in_parent) {
        ALPM_ASSERT(parent_pivot->bkt->trie->trie == NULL);
        /* The parent_pivot might be freed if parent bucket is empty indeed. */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_clean(u, m, db, ln, &parent_pivot));
    }

    /* $ Update parent pivot if required */
    if (format_update && parent_pivot) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln),
                                        parent_pivot, NULL,
                                        UPDATE_ALL));
    }

    /*
     * Clear those just-become-empty (due to compression) rbtks
     * after pivot updated.
     */
    delete_rbkts = vbanks - RBKT_VBANKS(u, m, db, ln, parent_group);
    /* When there is rbkt deleted, format update must be set. */
    ALPM_ASSERT(delete_rbkts == 0 || format_update);
    last_rbkt = parent_group + vbanks - 1;
    for (i = 0; i < delete_rbkts; i++) {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_clear(u, m, db, ln, last_rbkt - i));
        /* No need to do this.
         *   ver1_rbkt_free(u, m, db, ln, parent_group, last_rbkt - i));
         */
    }

    /* Set indexes after everything is ok. */
    ver1_bucket_indexes_set(u, m, db, ln, &split_pivot->bkt->list);
    done = SPLIT_OLD_BUCKET_CLEANUP;

    /* $ Entry in parent, insert after compression (insert assumption). */
    /* Blending in is too complex. */
    if (new_in_parent) {
        rv = ver1_bucket_insert(u, m, ln, arg, parent_pivot);
        if (rv == SHR_E_RESOURCE) {
            /* Take the chance to reorganize (also try to insert again) */
            rv = ver1_bucket_alloc_and_move(u, m, db, ln, parent_group, arg);
            if (rv == SHR_E_RESOURCE) {
                ALPM_ASSERT(split_aux->retry == SPLIT_RETRY_NONE);
                split_aux->retry = SPLIT_RETRY_ONE;
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    VER1_RBKT_GROUP_SANITY(u, m, db, ln, new_group, false, 0);
    VER1_RBKT_GROUP_SANITY(u, m, db, ln, parent_group, false, 0);

#ifdef ALPM_DEBUG
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        bcmptm_rm_alpm_buckets_sanity(u, m, db, ln, sanity_vrf_split_insert, false);
    }
#endif
    bcmptm_rm_alpm_internals[u][m][db][ln].split++;
exit:
    if (SHR_FUNC_ERR()) {
        switch (done) {
        case SPLIT_OLD_BUCKET_CLEANUP:
            /* That's ok once split is done. Break. */
            break;
        case SPLIT_BUCKET_MOVE_AND_ACTIVATE:
            split_pivot->bkt->trie->trie = NULL;
            /*
             * VER1_RBKT_PIVOT(u, m, db, ln, new_group) = NULL;  will be done in
             * ver1_rbkt_group_free.
             */
            /*
             * If E_PARTIAL, must properly rollback, if E_RESOURCE (not expecting)
             * we might break immediatelly to trigger abort early.
             */
            ALPM_ASSERT(SHR_FUNC_VAL_IS(SHR_E_PARTIAL) ||
                        SHR_FUNC_VAL_IS(SHR_E_RESOURCE) ||
                        SHR_FUNC_VAL_IS(SHR_E_MEMORY));
            SHR_DQ_SWAP_HEAD(&split_list, &split_pivot->bkt->list);
        case SPLIT_BUCKET_INDEXES_ALLOC:
            ver1_bucket_indexes_free(u, m, db, ln, new_group, &split_list,
                                     split_root->count);
        case SPLIT_BUCKET_CONNECT:
            /* Elems will be reconnected in following op */
        case SPLIT_TRIE_SPLIT:
            rv = bcmptm_rm_alpm_trie_merge(u, m, parent_pivot->bkt->trie,
                                           split_root,
                                           &pivot_key.key[0],
                                           pivot_len);
            ALPM_ASSERT(rv == SHR_E_NONE);
#ifdef ALPM_DEBUG
            if (split_dump) {
                cli_out(" After level %d merge \n", ln + 1);
                bcmptm_rm_alpm_trie_dump(parent_pivot->bkt->trie, NULL, NULL);
            }
#endif
            ver1_bkt_elems_connect(u, m, &parent_pivot->bkt->list,
                                   parent_pivot->bkt->trie->trie,
                                   &new_in_parent);
            /* Fall through */
        case SPLIT_ALLOC_GROUP:
            ver1_rbkt_group_free(u, m, db, ln, new_group);
            break;
        default:
            assert(done == SPLIT_NONE);
            /* nothing to roll back */
            break;
        }
    } else if (new_in_split || new_in_parent) {
        ALPM_ASSERT(arg->index[ln] >= 0);
    }
    if (split_arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], split_arg);
    }
    if (sbbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], sbbkts);
    }
    SHR_FUNC_EXIT();
}

static inline int
ver1_bucket_free(int u, int m, int db, int ln, rbkt_group_t group)
{
    return ver1_rbkt_group_free(u, m, db, ln, group);
}

static int
ver1_bucket_alloc(int u, int m, int ln, alpm_arg_t *arg, log_bkt_t *log_bkt)
{
    int max_banks, rv;
    rbkt_group_t group;
    max_banks = BKT_HDL_MAX_BANKS(u, m, arg->db, ln);
    rv = ver1_rbkt_group_alloc_best_effort(u, m, arg->db, ln,
                                           max_banks,
                                           max_banks >> 1,
                                           1,
                                           arg->data_type,
                                           &group);
    if (SHR_SUCCESS(rv)) {
        *log_bkt = VER1_LOG_BKT(group);
    }
    return rv;
}

static void
ver1_alpm_data_encode(int u, int m, int ln, alpm_arg_t *arg, void *adata)
{
    rbkt_group_t group;
    ALPM_ASSERT(ln == LEVEL1 || ln == LEVEL2);
    if (ln == LEVEL1) {
        if (arg->log_bkt[LEVEL1] == INVALID_LOG_BKT) {
            /* Pivot stay, bucket removed */
            group = RESERVED_GROUP;
        } else {
            group = VER1_GROUP(arg->log_bkt[LEVEL1]);
        }
        ver1_alpm1_data_encode(u, m, arg->db, NEXT_LEVEL(ln), arg->default_miss,
                               group,
                               arg->key.t.len, adata);
    } else {
        if (arg->log_bkt[LEVEL2] == INVALID_LOG_BKT) {
            group = RESERVED_GROUP;
        } else {
            group = VER1_GROUP(arg->log_bkt[LEVEL2]);
        }
        ver1_alpm2_data_encode(u, m, arg->db, NEXT_LEVEL(ln), arg->default_miss,
                               group,
                               arg->key.t.len, adata);
    }
}

static int
ver1_bucket_recover(int u, int m, int ln, alpm_arg_t *arg)
{
    uint32_t phy_bkt;
    int8_t rofs;
    rbkt_group_t group;
    int db = arg->db;
    bkt_hdl_t *bkt_hdl;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(
        ver1_bkt_hdl_get(u, m, db, ln, &bkt_hdl));

    if (PREV_LEVEL(ln) == LEVEL1) {
        phy_bkt = arg->a1.phy_bkt;
        rofs = arg->a1.rofs;
    } else {
        phy_bkt = arg->a2.phy_bkt;
        rofs = arg->a2.rofs;
    }
    group = (phy_bkt * bkt_hdl->max_banks) + bkt_hdl->p2l_bank_array[rofs];
    if (group == RESERVED_GROUP) {
        arg->log_bkt[PREV_LEVEL(ln)] = INVALID_LOG_BKT;
    } else {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_recover(u, m, db, ln, group, arg));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_bucket_recover_done(int u, int m, int db, int ln)
{
    uint32_t free_banks;
    rbkt_t *curr_rbkt, *prev_rbkt;
    rbkt_group_t prev_group;
    rbkt_group_t group;

    /* Skip group 1 */
    prev_group = FIRST_GROUP;
    for (group = FIRST_GROUP + 1;
         group < BKT_HDL_RBKTS_TOTAL(u, m, db, ln); group++) {
        if (!RBKT_IS_HEAD(u, m, db, ln, group)) {
            continue;
        }
        BKT_HDL_GROUPS_USED(u, m, db, ln)++;
        curr_rbkt = &RBKT(u, m, db, ln, group);
        prev_rbkt = &RBKT(u, m, db, ln, prev_group);
        SHR_DQ_INSERT_NEXT(&prev_rbkt->listnode, &curr_rbkt->listnode);
        free_banks =
            curr_rbkt->index - prev_rbkt->index - prev_rbkt->valid_banks;
        ALPM_ASSERT((int)free_banks >= 0);
        ver1_rbkt_group_free_bank_set(u, m, db, ln, prev_rbkt->index, free_banks);
        prev_group = group;
    }
    prev_rbkt = &RBKT(u, m, db, ln, prev_group);
    free_banks = BKT_HDL_RBKTS_TOTAL(u, m, db, ln) -
                 prev_rbkt->index - prev_rbkt->valid_banks;
    ALPM_ASSERT((int)free_banks >= 0);
    ver1_rbkt_group_free_bank_set(u, m, db, ln, prev_rbkt->index, free_banks);

    return SHR_E_NONE;
}

static int
ver1_bkt_hdl_init(int u, int m, bool recover)
{
    alpm_dev_info_t *dev_info;
    int db_idx, level;
    int db_count;
    int db_max_levels[DBS] = {0};
    int rv;
    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    VER1_MAX_FORMAT(u, m, FORMAT_PIVOT, DATA_FULL_3) = dev_info->pivot_max_format_3;
    VER1_MAX_FORMAT(u, m, FORMAT_ROUTE, DATA_FULL_3) = dev_info->route_max_format_3;
    VER1_MAX_FORMAT(u, m, FORMAT_PIVOT, DATA_FULL_2) = dev_info->pivot_max_format_2;
    VER1_MAX_FORMAT(u, m, FORMAT_ROUTE, DATA_FULL_2) = dev_info->route_max_format_2;

    VER1_MAX_FORMAT(u, m, FORMAT_PIVOT, DATA_FULL) = dev_info->pivot_max_format_1;
    VER1_MAX_FORMAT(u, m, FORMAT_ROUTE, DATA_FULL) = dev_info->route_max_format_1;
    VER1_MAX_FORMAT(u, m, FORMAT_PIVOT, DATA_REDUCED) = dev_info->pivot_max_format_0;
    VER1_MAX_FORMAT(u, m, FORMAT_ROUTE, DATA_REDUCED) = dev_info->route_max_format_0;

    VER1_START_FORMAT(u, m, FORMAT_PIVOT, DATA_FULL_3) = dev_info->pivot_min_format_3;
    VER1_START_FORMAT(u, m, FORMAT_ROUTE, DATA_FULL_3) = dev_info->route_min_format_3;
    VER1_START_FORMAT(u, m, FORMAT_PIVOT, DATA_FULL_2) = dev_info->pivot_min_format_2;
    VER1_START_FORMAT(u, m, FORMAT_ROUTE, DATA_FULL_2) = dev_info->route_min_format_2;
    VER1_START_FORMAT(u, m, FORMAT_PIVOT, DATA_FULL) = dev_info->pivot_min_format_1;
    VER1_START_FORMAT(u, m, FORMAT_ROUTE, DATA_FULL) = dev_info->route_min_format_1;
    VER1_START_FORMAT(u, m, FORMAT_PIVOT, DATA_REDUCED) = dev_info->pivot_min_format_0;
    VER1_START_FORMAT(u, m, FORMAT_ROUTE, DATA_REDUCED) = dev_info->route_min_format_0;

    db_count = bcmptm_rm_alpm_database_count(u, m);
    for (db_idx = 0; db_idx < db_count; db_idx++) {
        alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, m, db_idx);
        db_max_levels[db_idx] = database_ptr->max_levels;
        if (db_max_levels[db_idx] > 3) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }
    for (db_idx = DB0; db_idx < db_count; db_idx++) {
        /* Start from Level2 */
        for (level = LEVEL2; level < db_max_levels[db_idx]; level++) {
            int i, j;
            bkt_hdl_t *bkt_hdl = NULL;
            int max_banks;
            int alloc_sz;
            rbkt_t *rbkt;
            uint32_t bank_bitmap;

            ALPM_ALLOC(bkt_hdl, sizeof(bkt_hdl_t), "bcmptmRmalpmV1bkthdl");
            sal_memset(bkt_hdl, 0, sizeof(bkt_hdl_t));
            BKT_HDL(u, m, db_idx, level) = bkt_hdl;

            /* Bucket info */
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_uft_banks(u, m, db_idx, level,
                                         &bkt_hdl->max_banks,
                                         &bkt_hdl->start_bank,
                                         &bkt_hdl->bucket_bits,
                                         &bkt_hdl->bank_bitmap));

            bkt_hdl->rbkts_total = bkt_hdl->max_banks * (1 << bkt_hdl->bucket_bits);
            bkt_hdl->base_rbkt = bkt_hdl->start_bank * (1 << bkt_hdl->bucket_bits);
            bkt_hdl->groups_used = 1;
            bkt_hdl->rbkts_used = 1; /* Reserved group */
            bkt_hdl->thrd_banks = bkt_hdl->max_banks * 2;
            bkt_hdl->format_type = (level == (db_max_levels[db_idx] - 1)) ?
                                    FORMAT_ROUTE : FORMAT_PIVOT;


            bkt_hdl->last_bank = bkt_hdl->start_bank;
            bank_bitmap = bkt_hdl->bank_bitmap >> bkt_hdl->start_bank;
            while (bank_bitmap >>= 1) {
                bkt_hdl->last_bank++;
            }

            for (i = 0, j = 0; i < ALPM_LN_BANKS_MAX; i++) {
                if (bkt_hdl->bank_bitmap & (1 << i)) {
                    bkt_hdl->p2l_bank_array[i] = j;
                    bkt_hdl->l2p_bank_array[j] = i;
                    j++;
                }
            }

            SHR_DQ_INIT(&bkt_hdl->global_group_list);
            max_banks = bkt_hdl->max_banks;
            alloc_sz = (max_banks + 1) * sizeof(usage_list_t);
            ALPM_ASSERT(bkt_hdl->group_usage == NULL);
            ALPM_ALLOC(bkt_hdl->group_usage, alloc_sz, "bcmptmRmalpmV1grpusg");
            sal_memset(bkt_hdl->group_usage, 0, alloc_sz);
            for (i = 0; i <= max_banks; i++) {
                SHR_DQ_INIT(&bkt_hdl->group_usage[i].groups);
            }
            bkt_hdl->group_usage_count = (max_banks + 1);

            alloc_sz = bkt_hdl->rbkts_total * sizeof(rbkt_t);
            ALPM_ASSERT(bkt_hdl->rbkt_array == NULL);
            ALPM_ALLOC(bkt_hdl->rbkt_array, alloc_sz,
                       "bcmptmRmalpmV1bktarray");
            sal_memset(bkt_hdl->rbkt_array, 0, alloc_sz);

            /* Setup rbkt[1] as the first group assigned with all free rbkts */
            rbkt = &bkt_hdl->rbkt_array[FIRST_GROUP];
            rbkt->is_head = HEAD;
            /* Do not skip if recover */
            SHR_DQ_INSERT_HEAD(&bkt_hdl->global_group_list,
                               &rbkt->listnode);
            /* Skip if recover, as it will be handled in later recovery process */
            if (!recover) {
                rbkt->free_banks = bkt_hdl->rbkts_total - bkt_hdl->rbkts_used;
                SHR_DQ_INSERT_HEAD(&bkt_hdl->group_usage[max_banks].groups,
                                   &rbkt->usgnode);
            }

            for (i = 0; i < bkt_hdl->rbkts_total; i++) {
                rbkt_t *bbt = &bkt_hdl->rbkt_array[i];
                bbt->index = i;
            }
        }
    }
    ALPM_LMM_INIT(base_entries_ver1_t,
                  base_entries_lmem[u][m],
                  BASE_ENTRIES_ALLOC_CHUNK_SIZE,
                  false,
                  rv);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    ALPM_LMM_INIT(base_buckets_ver1_t,
                  base_buckets_lmem[u][m],
                  BASE_BUCKETS_ALLOC_CHUNK_SIZE,
                  false,
                  rv);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmptm_rm_alpm_bkt_hdl_cleanup(u, m);
    }
    SHR_FUNC_EXIT();
}

static void
ver1_bkt_hdl_cleanup(int u, int m)
{
    int i, j;
    int db_count;
    bkt_hdl_t *bkt_hdl;

    if (base_entries_lmem[u][m]) {
        shr_lmm_delete(base_entries_lmem[u][m]);
        base_entries_lmem[u][m] = NULL;
    }

    if (base_buckets_lmem[u][m]) {
        shr_lmm_delete(base_buckets_lmem[u][m]);
        base_buckets_lmem[u][m] = NULL;
    }

    db_count = bcmptm_rm_alpm_database_count(u, m);
    for (i = 0; i < db_count; i++) {
        alpm_db_t *database_ptr;
        database_ptr = bcmptm_rm_alpm_database_get(u, m, i);

        /* Start from Level2 */
        for (j = LEVEL2; j < database_ptr->max_levels; j++) {
            bkt_hdl = BKT_HDL(u, m, i, j);
            if (bkt_hdl) {
                SHR_FREE(bkt_hdl->group_usage);
                SHR_FREE(bkt_hdl->rbkt_array);
                SHR_FREE(bkt_hdl);
                BKT_HDL(u, m, i, j) = NULL;
            }
        }
    }
}

static void
ver1_bucket_sanity(int u, int m, int db, int ln, log_bkt_t log_bkt)
{
    VER1_RBKT_GROUP_SANITY(u, m, db, ln, VER1_GROUP(log_bkt), false, 0);
}

/*******************************************************************************
 * Public Functions
 */

static int
ver1_bucket_table_dump(int u, int m, int ln, int lvl_cnt, alpm_arg_t *arg, int *count)
{
    int rv = SHR_E_NONE;
    ln_base_entry_t *base_entry;
    format_type_t format_type;
    base_entries_ver1_t *base_entries = NULL;
    uint8_t i, log_bank, phy_bank;
    uint8_t *fmts;
    base_buckets_ver1_t *bbkts = NULL;
    char ipstr[128] = {0};
    int pvt_len;
    trie_ip_t pvt_trie_ip;
    uint32_t phy_bkt;
    int8_t rofs;
    rbkt_group_t group;
    int db = arg->db;
    alpm_dev_info_t *dev_info;
    bkt_hdl_t *bkt_hdl;

    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    base_entries = shr_lmm_alloc(base_entries_lmem[u][m]);
    if (base_entries == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (bbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    format_type = bcmptm_rm_alpm_format_type_get(u, m, db, ln);

    SHR_IF_ERR_EXIT(
        ver1_bkt_hdl_get(u, m, db, ln, &bkt_hdl));

    /* Save PREV_LEVEL(ln) pivot: key.t.trie_ip and key.t.len */
    sal_memcpy(&pvt_trie_ip, &arg->key.t.trie_ip, sizeof(trie_ip_t));
    pvt_len = arg->key.t.len;

    if (PREV_LEVEL(ln) == LEVEL1) {
        phy_bkt = arg->a1.phy_bkt;
        rofs = arg->a1.rofs;
    } else {
        phy_bkt = arg->a2.phy_bkt;
        rofs = arg->a2.rofs;
    }
    group = (phy_bkt * bkt_hdl->max_banks) + bkt_hdl->p2l_bank_array[rofs];

    fmts = (PREV_LEVEL(ln) == LEVEL1)? arg->a1.fmt : arg->a2.fmt;
    log_bank = group % bkt_hdl->max_banks;
    for (i = 0; i < bkt_hdl->max_banks; i++) {
        uint32_t hw_idx;
        uint8_t fmt;
        uint8_t eid;
        int k;
        int num_entry = 0;

        phy_bank = bkt_hdl->l2p_bank_array[log_bank];
        ALPM_ASSERT(bkt_hdl->bank_bitmap & (1 << phy_bank));
        fmt = fmts[phy_bank];
        if (fmt == DISABLED_FORMAT) {
            break;
        }
        if (++log_bank == bkt_hdl->max_banks) {
            log_bank = 0;
        }
        hw_idx = ver1_hw_idx_from_rbkt_idx(u, m, db, ln, group + i);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[i][0],
                                       &bbkts->dt_bitmap[i]));

        base_entry = &base_entries->entry[i][0];
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_raw_bucket_decode(u, m, format_type,
                                                fmt,
                                                base_entry,
                                                RBKT_MAX_ENTRIES_VER1,
                                                &bbkts->rbuf[i][0]));

        (void)bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type, fmt,
                                                NULL, NULL, &num_entry);

        for (eid = 0; eid < num_entry; eid++) {

            /* Clear arg->valid & restore saved pivot's arg->key.t.trie_ip */
            arg->valid = 0;
            sal_memcpy(&arg->key.t.trie_ip, &pvt_trie_ip, sizeof(trie_ip_t));

            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_decode(u, m, format_type,
                                                    fmt,
                                                    pvt_len,
                                                    arg,
                                                    base_entry[eid]));

            if (arg->valid == 0) {
                continue;
            }

            sal_memset(arg->key.ip_addr, 0, sizeof(arg->key.ip_addr));
            SHR_IF_ERR_EXIT(
               bcmptm_rm_alpm_api_key_create(u, m, arg->key.t.max_bits,
                                             &arg->key.t.trie_ip,
                                             arg->key.t.len,
                                             &arg->key.ip_addr[0]));

            bcmptm_rm_alpm_ipaddr_str(ipstr, arg->key.t.ipv, arg->key.ip_addr);

            for (k = 0; k < ln; k++) {
                cli_out("\t");
            }

            cli_out("L%d-[%-5d] | f%-2db%de%-2d %-3d | %s/%d %s| AD:%d (%d-%d-%d-%d)\n",
                    ln+1, hw_idx,
                    format_type == FORMAT_PIVOT ? fmt :
                        (dev_info->l2p_route_fmts ? dev_info->l2p_route_fmts[fmt] : fmt),
                    phy_bank, eid,
                    format_type == FORMAT_PIVOT ? 0 : *count, ipstr, arg->key.t.len,
                    (format_type == FORMAT_PIVOT && arg->a2.default_miss) ? "DM:1 " : "",
                    arg->ad.fmt_ad[0], arg->ad.fmt_ad[1], arg->ad.fmt_ad[2],
                    arg->ad.fmt_ad[3], arg->ad.fmt_ad[4]);

            if (format_type == FORMAT_ROUTE) {
                *count += 1;
                continue; /* end of recursion */
            }

            if (lvl_cnt >= 3) {
                rv = ver1_bucket_table_dump(u, m, LEVEL3, lvl_cnt, arg, count);
            }
        } /* for eid */
    } /* for i */

    SHR_ERR_EXIT(rv);
exit:
    if (base_entries) {
        shr_lmm_free(base_entries_lmem[u][m], base_entries);
    }
    if (bbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], bbkts);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_bucket_ver1_register(int u, bucket_mgmt_t *bmgmt)
{
    SHR_FUNC_ENTER(u);

    if (bmgmt) {
        bmgmt->bkt_hdl_cleanup = ver1_bkt_hdl_cleanup;
        bmgmt->bkt_hdl_init = ver1_bkt_hdl_init;
        bmgmt->bkt_hdl_get = ver1_bkt_hdl_get;
        bmgmt->alpm_data_encode = ver1_alpm_data_encode;
        bmgmt->bucket_pivot_get = ver1_bucket_pivot_get;
        bmgmt->bucket_pivot_set = ver1_rbkt_pivot_set;
        bmgmt->bucket_free = ver1_bucket_free;
        bmgmt->bucket_alloc = ver1_bucket_alloc;
        bmgmt->bucket_delete = ver1_bucket_delete;
        bmgmt->bucket_insert = ver1_bucket_insert;
        bmgmt->bucket_split_insert = ver1_bucket_split_insert;
        bmgmt->entry_write = ver1_base_entry_encode_write;
        bmgmt->entry_read = ver1_base_entry_decode_read;
        bmgmt->entry_hw_idx_get = ver1_hw_idx_from_ent_idx;
        bmgmt->bucket_sanity= ver1_bucket_sanity;
        bmgmt->bucket_recover = ver1_bucket_recover;
        bmgmt->bucket_recover_done = ver1_bucket_recover_done;
        bmgmt->bucket_resource_check = ver1_bucket_resource_check;
        bmgmt->bucket_table_dump = ver1_bucket_table_dump;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

