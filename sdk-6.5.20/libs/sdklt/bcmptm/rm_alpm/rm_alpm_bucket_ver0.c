/*! \file rm_alpm_bucket_ver0.c
 *
 * Bucket Manager for ALPM Level-N bucket
 *
 * This file contains the bucket manager for ALPM Level-N bucket
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
#define RBKT_MAX_ENTRIES_VER0       6
#define BASE_ENTRIES_ALLOC_CHUNK_SIZE  2
#define BASE_BUCKETS_ALLOC_CHUNK_SIZE  8

#define RBKT_EBMP(u, m, _d, _l, _i)        (RBKT(u, m, _d, _l, _i).entry_bitmap)
#define RBKT_VBANKS(u, m, _d, _l, _i)          (RBKT(u, m, _d, _l, _i).valid_banks)
#define RBKT_FBANKS(u, m, _d, _l, _i)          (RBKT(u, m, _d, _l, _i).free_banks)
#define RBKT_FORMAT(u, m, _d, _l, _i)          (RBKT(u, m, _d, _l, _i).format)
#define RBKT_IS_HEAD(u, m, _d, _l, _i)         (RBKT(u, m, _d, _l, _i).is_head)
#define RBKT_SUB_BKTS(u, m, _d, _l, _i)        (RBKT(u, m, _d, _l, _i).sub_bkts)
#define RBKT_ECOUNT(u, m, _d, _l, _i)          (RBKT(u, m, _d, _l, _i).count)
#define VER0_RBKT_PIVOT(u, m, _d, _l, _i, _s)  (RBKT(u, m, _d, _l, _i).pivots[_s])

#define VER0_SUB_BKT_SHIFT          (3)
#define VER0_SUB_BKT_MASK           (7) /* ((1U << VER0_SUB_BKT_SHIFT) - 1) */
#define VER0_GROUP(u, m, _l)           ((_l) >> VER0_SUB_BKT_SHIFT)
#define VER0_SUB_BKT(u, m, _l)         ((_l) & VER0_SUB_BKT_MASK)
#define VER0_LOG_BKT(_p, _s)        (((_p) << VER0_SUB_BKT_SHIFT) | \
                                     ((_s) & VER0_SUB_BKT_MASK))

#define RBKT_SUB_BKT_SET(u, m, _d, _l, _p, _s)   \
    (RBKT_SUB_BKTS(u, m, _d, _l, _p) |= (1U << (_s)))
#define RBKT_SUB_BKT_CLR(u, m, _d, _l, _p, _s)   \
    (RBKT_SUB_BKTS(u, m, _d, _l, _p) &= ~(1U << (_s)))
#define RBKT_SUB_BKT_IS_SET(u, m, _d, _l, _p, _s)  \
    (RBKT_SUB_BKTS(u, m, _d, _l, _p) & (1U << (_s)))

#define VER0_CAN_BUCKET_SHARE(u, m, _d, _l, _g1, _g2)      \
    (fast_popcount4[RBKT_SUB_BKTS(u, m, _d, _l, _g1)] +    \
     fast_popcount4[RBKT_SUB_BKTS(u, m, _d, _l, _g2)] <=   \
     MAX_SUB_BKTS)

#define VER0_MAX_FORMAT(u, m, _p)             (smax_format[u][m][_p])

#ifdef ALPM_DEBUG
#define VER0_RBKT_GROUP_SANITY(u, m, db, ln, group) \
        ver0_rbkt_group_sanity(u, m, db, ln, group)
#else
#define VER0_RBKT_GROUP_SANITY(u, m, db, ln, group)
#endif
/*******************************************************************************
 * Typedefs
 */

typedef struct base_entries_ver0_s {
    ln_base_entry_t entry[ALPM_MAX_BANKS][RBKT_MAX_ENTRIES_VER0];
    /*!< Pointer to the next element in the list (or NULL). */
    struct base_entries_ver0_s *next;
} base_entries_ver0_t;

typedef struct base_buckets_ver0_s {
    LN_ENTRY_t rbuf[ALPM_MAX_BANKS];
    /*!< Pointer to the next element in the list (or NULL). */
    struct base_buckets_ver0_s *next;
} base_buckets_ver0_t;
/*******************************************************************************
 * Private variables
 */

static bkt_hdl_t *sbkt_hdl[BCMDRD_CONFIG_MAX_UNITS][DBS][LEVELS][ALPMS];
static uint8_t bucket_scale[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
/*!< Max format value  */
static int smax_format[BCMDRD_CONFIG_MAX_UNITS][ALPMS][FROMAT_TYPES];
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern shr_lmm_hdl_t bcmptm_rm_alpm_arg_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static const format_type_t format_type = FORMAT_ROUTE;
static uint8_t ver0_bank_bits[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static shr_lmm_hdl_t base_entries_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static shr_lmm_hdl_t base_buckets_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern alpm_internals_t
    bcmptm_rm_alpm_internals[BCMDRD_CONFIG_MAX_UNITS][ALPMS][DBS][LEVELS];

/*******************************************************************************
 * Private Functions (declaration)
 */

static uint8_t ver0_base_entry_count(int u, int m, int db, int ln, uint8_t format_value);

/*******************************************************************************
 * Private Functions (definition)
 */

static uint32_t
ver0_hw_idx_from_rbkt_idx(int u, int m, int db, int ln, rbkt_idx_t rbkt)
{
    int max_banks;
    max_banks = BKT_HDL_MAX_BANKS(u, m, db, ln);
    if (max_banks == 4) {
        return rbkt + BKT_HDL_BASE_RBKT(u, m, db, ln);
    } else {
        return ((rbkt % max_banks) | ((rbkt / max_banks) << 2)) +
               BKT_HDL_BASE_RBKT(u, m, db, ln);
    }
}

static uint32_t
ver0_hw_idx_from_ent_idx(int u, int m, int db, int ln, ln_index_t ent_idx)
{
    return ver0_hw_idx_from_rbkt_idx(u, m, db, ln, RBKT_FROM_INDEX(ent_idx));
}

static uint32_t
ver0_hw_eidx_from_ent_idx(int u, int m, int db, int ln, ln_index_t ent_idx)
{
    uint8_t bkt_bank_bits;
    uint8_t ent_ofs, bank;
    uint32_t phy_bkt;

    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    phy_bkt = RBKT_FROM_INDEX(ent_idx) / BKT_HDL_MAX_BANKS(u, m, db, ln);
    bank = RBKT_FROM_INDEX(ent_idx) % BKT_HDL_MAX_BANKS(u, m, db, ln);
    bkt_bank_bits = BKT_HDL_BUCKET_BITS(u, m, db, LEVEL2) + ver0_bank_bits[u][m];
    return (ent_ofs << bkt_bank_bits) | (phy_bkt << ver0_bank_bits[u][m]) | bank;
}

static int
ver0_bkt_hdl_get(int u, int m, int db, int ln, bkt_hdl_t **bkt_hdl)
{
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(bkt_hdl);
    ALPM_ASSERT(db < DBS);
    ALPM_ASSERT(ln < LEVELS);
    *bkt_hdl = BKT_HDL(u, m, db, ln);
    SHR_NULL_CHECK(*bkt_hdl, SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

static inline int
ver0_sub_bucket_alloc(int u, int m, int db, int ln, rbkt_group_t group,
                      uint8_t *sub_bkt)
{
    uint8_t i;
    ALPM_ASSERT(RBKT_SUB_BKTS(u, m, db, ln, group) != 0xF);
    for (i = 0; i < MAX_SUB_BKTS; i++) {
        if (!RBKT_SUB_BKT_IS_SET(u, m, db, ln, group, i)) {
            RBKT_SUB_BKT_SET(u, m, db, ln, group, i);
            *sub_bkt = i;
            return SHR_E_NONE;
        }
    }
    return SHR_E_INTERNAL;
}

static inline void
ver0_sub_bucket_free(int u, int m, int db, int ln, rbkt_group_t group, uint8_t sub_bkt)
{
    RBKT_SUB_BKT_CLR(u, m, db, ln, group, sub_bkt);
}

static inline void
ver0_bucket_pivot_set(int u, int m, int db, int ln, rbkt_group_t group,
                      uint8_t sub_bkt, alpm_pivot_t *pivot)
{
    VER0_RBKT_PIVOT(u, m, db, ln, group, sub_bkt) = pivot;
    /* Assume bucket alloc before pivot set */
    ALPM_ASSERT(RBKT_SUB_BKT_IS_SET(u, m, db, ln, group, sub_bkt));
}

static inline void
ver0_bucket_pivot_clr(int u, int m, int db, int ln, rbkt_group_t group,
                      uint8_t sub_bkt)
{
    /* Assume bucket freed after pivot clr */
    ALPM_ASSERT(RBKT_SUB_BKT_IS_SET(u, m, db, ln, group, sub_bkt));
    VER0_RBKT_PIVOT(u, m, db, ln, group, sub_bkt) = NULL;
    ver0_sub_bucket_free(u, m, db, ln, group, sub_bkt);
}

static alpm_pivot_t *
ver0_bucket_pivot_get(int u, int m, int db, int ln, rbkt_group_t group,
                      uint8_t sub_bkt)
{
    ALPM_ASSERT(RBKT_IS_HEAD(u, m, db, ln, group));
    return VER0_RBKT_PIVOT(u, m, db, ln, group, sub_bkt);
}

/*!
 * \brief Calculate the max length for new split point.
 *
 * \param [in] u Device u.
 * \param [in] pivot Pivot to be split.
 *
 * \return Max split length
 */
static inline uint32_t
ver0_trie_msl(int u, int m, alpm_ip_ver_t ipv)
{
    return (ipv == IP_VER_4) ? 31 :
           (bcmptm_rm_alpm_pkm128_is_set(u, m)) ? 127 : 63;
}

/*!
 * \brief Calculate the max count for new split point.
 *
 * \param [in] u Device u.
 * \param [in] pivot Pivot to be split.
 *
 * \return Max split count
 */
static inline uint32_t
ver0_trie_msc(int u, int m, int db, int ln, uint8_t fmt, rbkt_group_t group)
{
    return ver0_base_entry_count(u, m, db, ln, fmt) -
           RBKT_ECOUNT(u, m, db, ln, group);
}

#ifdef ALPM_DEBUG
static void
ver0_rbkt_group_sanity(int u, int m, int db, int ln, rbkt_group_t group)
{
    rbkt_t *rbkt_group, *rbkt;
    bkt_hdl_t *bkt_hdl;
    shr_dq_t *blist, *elem;
    alpm_bkt_elem_t *curr;
    int bkt_count[ALPM_MAX_BANKS] = {0};
    rbkt_idx_t rbkt_idx;
    int num_entry, i;
    uint8_t vbanks, sub_bkt;
    int count = 0, count2 = 0;
    alpm_pivot_t *pivots[MAX_SUB_BKTS] = {NULL};

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }
    ver0_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    if (group == INVALID_GROUP) {
        return;
    }
    rbkt_group = &bkt_hdl->rbkt_array[group];
    assert(rbkt_group->valid_banks == bkt_hdl->max_banks * bucket_scale[u][m]);
    vbanks = rbkt_group->valid_banks;

    for (i = 0; i < vbanks; i++) {
        rbkt = rbkt_group + i;
        if (i == 0) {
            assert(rbkt->is_head);
            for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
                pivots[sub_bkt] = rbkt->pivots[sub_bkt];
                if (rbkt->sub_bkts & (1 << sub_bkt)) {
                    assert(pivots[sub_bkt] && pivots[sub_bkt]->bkt);
                    assert(VER0_GROUP(u, m, pivots[sub_bkt]->bkt->log_bkt) ==
                           rbkt_group->index);
                    if (rbkt->format == VER0_FORMAT_1 ||
                        rbkt->format == VER0_FORMAT_3) {
                        assert(pivots[sub_bkt]->key.t.ipv == IP_VER_4);
                    } else {
                        if (rbkt->format == VER0_FORMAT_5) {
                            assert(bcmptm_rm_alpm_pkm128_is_set(u, m));
                        } else {
                            assert(!bcmptm_rm_alpm_pkm128_is_set(u, m));
                        }
                        assert(pivots[sub_bkt]->key.t.ipv == IP_VER_6);
                    }
                } else {
                    assert(!pivots[sub_bkt]);
                }
            }
        } else {
            assert(!rbkt->is_head);
        }
        count += shr_util_popcount(rbkt->entry_bitmap);
    }

    for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
        if (!pivots[sub_bkt]) {
            continue;
        }
        count2 += bcmptm_rm_alpm_trie_count(pivots[sub_bkt]->bkt->trie);
        blist = &pivots[sub_bkt]->bkt->list;
        SHR_DQ_TRAVERSE(blist, elem)
            curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
            if (curr->index == INVALID_INDEX) {
                continue;
            }
            rbkt_idx = RBKT_FROM_INDEX(curr->index);
            bkt_count[rbkt_idx - group]++;
        SHR_DQ_TRAVERSE_END(blist, elem);
    }
    assert(count == count2);

    for (i = 0; i < rbkt_group->valid_banks; i++) {
        rbkt = &bkt_hdl->rbkt_array[group + i];
        bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type, rbkt->format,
                                          NULL, NULL, &num_entry);
        assert(bkt_count[i] <= num_entry);
        assert(bkt_count[i] == shr_util_popcount(rbkt->entry_bitmap));
    }
}
#endif

/* Update bucket usage bookeeping info */
static void
ver0_rbkt_group_free_count_update(int u, int m, int db, int ln, rbkt_group_t group,
                                  int incr)
{
    int count;
    uint8_t fmt;

    if (incr == 0) {
        return;
    }
    count = RBKT_ECOUNT(u, m, db, ln, group);
    ALPM_ASSERT(count >= 0);
    fmt = RBKT_FORMAT(u, m, db, ln, group);
    ALPM_ASSERT(fmt != DISABLED_FORMAT);
    if (!SHR_DQ_NULL(&RBKT_FMT_USG_NODE(u, m, db, ln, group))) {
        SHR_DQ_REMOVE(&RBKT_FMT_USG_NODE(u, m, db, ln, group));
    } else {
        ALPM_ASSERT(count == 0);
    }

    RBKT_ECOUNT(u, m, db, ln, group) += incr;

    count = RBKT_ECOUNT(u, m, db, ln, group);
    if (RBKT_SUB_BKTS(u, m, db, ln, group) == 0xF) { /* For performance */
        ALPM_ASSERT(&FMT_GROUP_USAGE_LIST(u, m, db, ln, fmt, 0) !=
                    &RBKT_FMT_USG_NODE(u, m, db, ln, group));
        SHR_DQ_INSERT_HEAD(&FMT_GROUP_USAGE_LIST(u, m, db, ln, fmt, 0),
                           &RBKT_FMT_USG_NODE(u, m, db, ln, group));
    } else {
        ALPM_ASSERT(&FMT_GROUP_USAGE_LIST(u, m, db, ln, fmt, count) !=
                    &RBKT_FMT_USG_NODE(u, m, db, ln, group));
        SHR_DQ_INSERT_HEAD(&FMT_GROUP_USAGE_LIST(u, m, db, ln, fmt, count),
                           &RBKT_FMT_USG_NODE(u, m, db, ln, group));
    }
    ALPM_ASSERT(!SHR_DQ_NULL(&RBKT_FMT_USG_NODE(u, m, db, ln, group)));
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
static void
ver0_rbkt_group_free_bank_update(int u, int m, int db, int ln, rbkt_group_t group,
                                 int diff)
{
    int free_count;

    RBKT_FBANKS(u, m, db, ln, group) += diff;
    free_count = RBKT_FBANKS(u, m, db, ln, group);
    ALPM_ASSERT(free_count >= 0);
    SHR_DQ_REMOVE(&RBKT_USG_NODE(u, m, db, ln, group));
    free_count = ALPM_MIN(bucket_scale[u][m],
                          free_count / BKT_HDL_MAX_BANKS(u, m, db, ln));
    ALPM_ASSERT(free_count < GROUP_USAGE_CNT(u, m, db, ln));
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
static void
ver0_rbkt_group_free_bank_set(int u, int m, int db, int ln, rbkt_group_t group,
                              uint32_t free_count)
{
    RBKT_FBANKS(u, m, db, ln, group) = free_count;
    free_count = ALPM_MIN(bucket_scale[u][m],
                          free_count / BKT_HDL_MAX_BANKS(u, m, db, ln));
    ALPM_ASSERT(free_count < GROUP_USAGE_CNT(u, m, db, ln));
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
static void
ver0_rbkt_group_free_bank_clr(int u, int m, int db, int ln, rbkt_group_t group)
{
    RBKT_FBANKS(u, m, db, ln, group) = 0;
    SHR_DQ_REMOVE(&RBKT_USG_NODE(u, m, db, ln, group));
    if (!SHR_DQ_NULL(&RBKT_FMT_USG_NODE(u, m, db, ln, group))) {
        SHR_DQ_REMOVE(&RBKT_FMT_USG_NODE(u, m, db, ln, group));
    }
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
ver0_rbkt_group_free(int u, int m, int db, int ln, rbkt_group_t group)
{
    rbkt_t *prev;
    shr_dq_t *dq;
    int i;
    int prev_group;

    SHR_FUNC_ENTER(u);
    if (group == INVALID_GROUP) { /* Invalid group does not exist */
        SHR_EXIT();
    }

    ALPM_ASSERT(RBKT_VBANKS(u, m, db, ln, group) != 0);
    if (group == FIRST_GROUP || group == RESERVED_GROUP) { /* Don't fuse group */
#ifdef ALPM_DEBUG
        do {
            uint8_t sub_bkt;
            for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
                ALPM_ASSERT(VER0_RBKT_PIVOT(u, m, db, ln, group, sub_bkt) == NULL);
            }
        } while (0);
#endif
    } else {
        /* Cannot fuse for the first group */
        ALPM_ASSERT(group != FIRST_GROUP || group != INVALID_GROUP ||
                    group != RESERVED_GROUP);

        dq = SHR_DQ_PREV(&RBKT_LIST_NODE(u, m, db, ln, group), shr_dq_t*);
        prev = SHR_DQ_ELEMENT_GET(rbkt_t*, dq, listnode);
        prev_group = prev->index;

        ver0_rbkt_group_free_bank_update(u, m, db, ln, prev_group,
                                   RBKT_FBANKS(u, m, db, ln, group) +
                                   RBKT_VBANKS(u, m, db, ln, group));

        for (i = 1; i < RBKT_VBANKS(u, m, db, ln, group); i++) {
            rbkt_idx_t rbkt_idx = group + i;
            RBKT_FORMAT(u, m, db, ln, rbkt_idx) = DISABLED_FORMAT;
            RBKT_EBMP(u, m, db, ln, rbkt_idx) = 0;
        }
        RBKT_VBANKS(u, m, db, ln, group) = 0;
        ver0_rbkt_group_free_bank_clr(u, m, db, ln, group);
        RBKT_IS_HEAD(u, m, db, ln, group) = BODY;
#ifdef ALPM_DEBUG
        do {
            uint8_t sub_bkt;
            for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
                ALPM_ASSERT(VER0_RBKT_PIVOT(u, m, db, ln, group, sub_bkt) == NULL);
            }
        } while (0);
#endif
        SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, m, db, ln, group));
        BKT_HDL_GROUPS_USED(u, m, db, ln)--;
    }
exit:
    SHR_FUNC_EXIT();
}

static inline bool
ver0_rbkt_group_splitable(int u, int m, int db, int ln, rbkt_t *free_group,
                          uint8_t size, rbkt_idx_t *new_group,
                          uint16_t *take_count)
{
    uint8_t max_banks = BKT_HDL_MAX_BANKS(u, m, db, ln);
    int next_group_begin;


    next_group_begin = free_group->index + free_group->valid_banks +
                       free_group->free_banks;
    /*
     * When ALPM_DEBUG, we can make this to be a index allocation for mixed case
     * with different size buckets.
     * Currently, this mixed case does not hold, and all buckets are with
     * same size, all scale equal to 1, or 2. Therefore whenere a free group
     * is found, it is always splitable.
     * next_group_begin = (next_group_begin / max_banks) * max_banks;
     */
    ALPM_ASSERT(next_group_begin == (next_group_begin / max_banks) * max_banks);
    *new_group = next_group_begin - (size * max_banks);
    ALPM_ASSERT(*new_group >= free_group->valid_banks + free_group->index);
    *take_count = next_group_begin - *new_group;
    return *new_group; /* False if group 0 */
}


/*!
 * \brief Allocate a new group.
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
ver0_rbkt_group_alloc(int u, int m, int db,
                      int ln, uint8_t fmt,
                      uint8_t size,
                      bool shuffle,
                      rbkt_group_t *new_group)
{
    uint8_t free_count, max_banks;
    uint16_t take_count;
    shr_dq_t *groups_ptr;
    shr_dq_t *elem = NULL;
    rbkt_t *free_rbkt, *new_rbkt;
    uint8_t i;

    SHR_FUNC_ENTER(u);

    if (size == 0) {
        SHR_EXIT();
    }
    /*
     * Traverse the FMT_GROUP_USAGE array starting from max free count.
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
    max_banks = BKT_HDL_MAX_BANKS(u, m, db, ln);
    for (free_count = bucket_scale[u][m]; free_count >= size; free_count--) {
        groups_ptr = &GROUP_USAGE_LIST(u, m, db, ln, free_count);
        if (!SHR_DQ_EMPTY(groups_ptr)) {
            /* Stop at the first new_group */
            elem = SHR_DQ_HEAD(groups_ptr, shr_dq_t*);
            free_rbkt = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
            if (!ver0_rbkt_group_splitable
                    (u, m, db, ln, free_rbkt, size, new_group, &take_count)) {
                    continue;
            }

            /* If no valid banks, must be group[0].
             * Update: not true any more, as we may have pivot with assigned group with
             * all free rbkts. This is the case when we cannot free that pivot (usually
             * level-2), because otherwise we may endup with ill insert (find wrong
             * second level pivot).
             */
            /* ALPM_ASSERT(free_rbkt->valid_banks || free_rbkt->index == FIRST_GROUP); */
            /* Setup split group */
            new_rbkt = &RBKT(u, m, db, ln, *new_group);
            if (new_rbkt->index != free_rbkt->index) {
                new_rbkt->is_head = HEAD;
                size *= max_banks; /* Scale to bank size */
                ALPM_ASSERT(take_count == size);
                new_rbkt->valid_banks = size;
                SHR_DQ_ELEM_INIT(&RBKT_FMT_USG_NODE(u, m, db, ln, new_rbkt->index));
                ver0_rbkt_group_free_bank_set(u, m, db, ln, new_rbkt->index,
                                              take_count - size);
                /* Setup original group */
                ver0_rbkt_group_free_bank_update(u, m, db, ln, free_rbkt->index,
                                                 -take_count);
                /* Link the new and free2 */
                SHR_DQ_INSERT_NEXT(&free_rbkt->listnode,
                                   &new_rbkt->listnode);
                BKT_HDL_GROUPS_USED(u, m, db, ln)++;

                for (i = 0; i < size; i++, new_rbkt++) {
                    new_rbkt->format = fmt;
                }
            } else {
                /* Else take all, and this group is already in the list */
                alpm_pivot_t *pivot;
                uint8_t sub_bkt;
                for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
                    pivot = ver0_bucket_pivot_get(u, m, db, ln, free_rbkt->index,
                                                  sub_bkt);
                    if (pivot) {
                        /* This group is now officially taken away */
                        pivot->bkt->log_bkt = INVALID_LOG_BKT;
                    }
                }
            }
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_RESOURCE);
exit:
    SHR_FUNC_EXIT();
}

static int
ver0_rbkt_recover(int u, int m, int db, int ln, rbkt_idx_t rbkt_idx,
                  uint8_t pivot_len,
                  ln_base_entry_t *base_entry,
                  uint8_t *entry_count,
                  alpm_arg_t *arg,
                  uint8_t sub_bkt)
{
    int num_entries, i;
    uint8_t format = RBKT_FORMAT(u, m, db, ln, rbkt_idx);
    alpm_arg_t *entry_arg = NULL;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                          format,
                                          NULL,
                                          NULL,
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
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_decode(u, m, format_type,
                                                format,
                                                pivot_len,
                                                entry_arg,
                                                base_entry[i]));
        /* No need to do.
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_alpm_mask_to_len(u, m, entry_arg->key.t.max_bits,
                                        arg->key.ip_mask,
                                        &arg->key.t.len));
                                        */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_key_create(u, m, entry_arg->key.t.max_bits,
                                           entry_arg->key.ip_addr,
                                           entry_arg->key.t.len,
                                           &entry_arg->key.t.trie_ip));

        if (entry_arg->valid && sub_bkt == entry_arg->a1.sub_bkt) {
            ALPM_BIT_SET(RBKT_EBMP(u, m, db, ln, rbkt_idx), i);
            entry_arg->index[ln] =
                ALPM_INDEX_ENCODE_VER0(rbkt_idx, entry_arg->a1.sub_bkt, i);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_pivot_recover(u, m, ln, entry_arg));
            (*entry_count)++;
        }
    }
exit:
    if (entry_arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], entry_arg);
    }
    SHR_FUNC_EXIT();
}

static int
ver0_log_bucket_recover(int u, int m, int db, int ln,
                        rbkt_group_t group, uint8_t sub_bkt,
                        alpm_arg_t *arg)
{
    ln_base_entry_t *base_entry;
    base_entries_ver0_t *base_entries = NULL;
    uint8_t pivot_len, i, entry_count, max_banks;
    alpm_pivot_t *pivot;
    base_buckets_ver0_t *bbkts = NULL;

    SHR_FUNC_ENTER(u);
    base_entries = shr_lmm_alloc(base_entries_lmem[u][m]);
    if (base_entries == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    RBKT_IS_HEAD(u, m, db, ln, group) = true;
    pivot = arg->pivot[PREV_LEVEL(ln)];
    ALPM_ASSERT(pivot);
#if 0
    for (i = 0; i < MAX_SUB_BKTS; i++) {
        if (ver0_bucket_pivot_get(u, m, db, ln, group, i)) {
            /*
             * Performance boost approach.
             * This group is traversed by another pivot,
             * which means everything about the sub bucket is already recovered
             * except for its pivot pointer.
             */
            RBKT_SUB_BKT_SET(u, m, db, ln, group, sub_bkt);
            ver0_bucket_pivot_set(u, m, db, ln, group, sub_bkt, pivot);
            arg->log_bkt[PREV_LEVEL(ln)] = VER0_LOG_BKT(group, sub_bkt);
            SHR_EXIT();
        }
    }
#endif
    RBKT_SUB_BKT_SET(u, m, db, ln, group, sub_bkt);
    ALPM_ASSERT(!ver0_bucket_pivot_get(u, m, db, ln, group, sub_bkt) ||
                group == RESERVED_GROUP);
    ver0_bucket_pivot_set(u, m, db, ln, group, sub_bkt, pivot);
    arg->log_bkt[PREV_LEVEL(ln)] = VER0_LOG_BKT(group, sub_bkt);
    pivot_len = pivot->key.t.len;
    entry_count = 0;
    max_banks = bucket_scale[u][m] * BKT_HDL_MAX_BANKS(u, m, db, ln);
    bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (bbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    for (i = 0; i < max_banks; i++) {
        uint32_t hw_idx;
        rbkt_idx_t rbkt_idx;
        hw_idx = ver0_hw_idx_from_rbkt_idx(u, m, db, ln, group + i);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[i][0], NULL));

        base_entry = &base_entries->entry[i][0];
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_raw_bucket_decode(u, m, format_type,
                                                arg->fmt,
                                                base_entry,
                                                RBKT_MAX_ENTRIES_VER0,
                                                &bbkts->rbuf[i][0]));
        rbkt_idx = group + i;
        RBKT_FORMAT(u, m, db, ln, rbkt_idx) = arg->fmt;
        RBKT_VBANKS(u, m, db, ln, group) = max_banks;
        ALPM_ASSERT(RBKT_VBANKS(u, m, db, ln, group) <= max_banks);
        SHR_IF_ERR_EXIT(
            ver0_rbkt_recover(u, m, db, ln, rbkt_idx, pivot_len, base_entry,
                              &entry_count, arg, sub_bkt));
    }
    if (entry_count) {
        if (RBKT_ECOUNT(u, m, db, ln, group) == 0) {
            SHR_DQ_INIT(&RBKT_FMT_USG_NODE(u, m, db, ln, group));
        }
        ver0_rbkt_group_free_count_update(u, m, db, ln, group, entry_count);
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
ver0_bkt_elems_connect_cb(rm_alpm_trie_node_t *node, void *user_data)
{
    bucket_traverse_data_t *trvs_data;
    trvs_data = (bucket_traverse_data_t *) user_data;
    if (node->type == PAYLOAD) {
        alpm_bkt_elem_t *bkt_elem;
        bkt_elem = TRIE_ELEMENT_GET(alpm_bkt_elem_t*, node, bktnode);
        if (bkt_elem->index == INVALID_INDEX) {
            trvs_data->found_new = true;
            SHR_DQ_INSERT_TAIL(trvs_data->dq[0], &bkt_elem->dqnode);
        } else {
            SHR_DQ_INSERT_HEAD(trvs_data->dq[0], &bkt_elem->dqnode);
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
ver0_bkt_elems_connect(int u, int m, shr_dq_t *list, rm_alpm_trie_node_t *trie,
                       bool *found_new)
{
    bucket_traverse_data_t trvs_data =
        {.dq[0] = list, .count = 0, .found_new = 0};
    SHR_FUNC_ENTER(u);
    SHR_DQ_INIT(list);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_traverse(u, m,
                                     trie,
                                     ver0_bkt_elems_connect_cb,
                                     &trvs_data,
                                     TRAVERSE_BFS));
    *found_new = trvs_data.found_new;
    ALPM_ASSERT((trie && (trvs_data.count == trie->count)) ||
                (trie == NULL && trvs_data.count == 0));
exit:
    SHR_FUNC_EXIT();
}

/* Get entry No. per bucket based on various setting */
/*!
 * \brief Get base entry count per group based on various setting.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] format_value Format value for base entry
 *
 * \return Base entry count per group
 */
static uint8_t
ver0_base_entry_count(int u, int m, int db, int ln, uint8_t format_value)
{
    uint8_t bkt_ent_max = 0;
    int full;
    int number;

    /* Base entry number */
    full = bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                           format_value,
                                           NULL,
                                           &full,
                                           &number);
    ALPM_ASSERT(full == SHR_E_NONE);

    bkt_ent_max = number * BKT_HDL_MAX_BANKS(u, m, db, ln);
    bkt_ent_max *= bucket_scale[u][m];
    /*
     * if (bcmptm_rm_alpm_is_parallel(u, m) &&
     *     is_urpf) {
     *     bkt_ent_max >>= 1;
     * }
     */
    return bkt_ent_max;
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
ver0_base_entries_free(int u, int m, int db, int ln,
                       rbkt_group_t group,
                       ln_index_t *ent_idx,
                       int count)
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
    }

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
ver0_base_entry_free(int u, int m, int db, int ln,
                     rbkt_group_t group,
                     ln_index_t ent_idx,
                     bool *format_update)
{
    rbkt_idx_t remove_bkt;
    rbkt_t *rbkt;

    SHR_FUNC_ENTER(u);
    remove_bkt = RBKT_FROM_INDEX(ent_idx);
    rbkt = &RBKT(u, m, db, ln, remove_bkt);

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ENT_OFS_FROM_IDX(ent_idx)));
    ALPM_BIT_CLR(rbkt->entry_bitmap, ENT_OFS_FROM_IDX(ent_idx));

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
ver0_base_entry_alloc(int u, int m, int db, int ln,
                      rbkt_group_t group,
                      uint8_t sub_bkt,
                      int entry_len,
                      int entry_full,
                      ln_index_t *ent_idx,
                      bool *format_update,
                      int *format_value)
{
    int i;
    int min_format_value = INVALID_FORMAT;
    int full;
    int number;
    rbkt_idx_t rbkt_idx = 0;
    SHR_FUNC_ENTER(u);

    for (i = 0; i < RBKT_VBANKS(u, m, db, ln, group); i++) {
        rbkt_t *rbkt = &RBKT(u, m, db, ln, group + i);
        /* All rbkts' formats should be same as head */
        ALPM_ASSERT(rbkt->format != DISABLED_FORMAT);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type,
                                              rbkt->format,
                                              NULL,
                                              &full,
                                              &number));
        if (rbkt->entry_bitmap != (uint32_t)((1 << number) - 1) &&
            full >= entry_full) {
            min_format_value = rbkt->format;
            rbkt_idx = group + i;
            break;
        }
    }

    if (min_format_value != INVALID_FORMAT) {/* Free entry in valid banks */
        for (i = 0; i < number; i++) {
            if (!ALPM_BIT_GET(RBKT_EBMP(u, m, db, ln, rbkt_idx), i)) {
                *ent_idx = ALPM_INDEX_ENCODE_VER0(rbkt_idx, sub_bkt, i);
                if (format_update) {
                    *format_update = FALSE;
                }
                ALPM_BIT_SET(RBKT_EBMP(u, m, db, ln, rbkt_idx), i);
                *format_value = min_format_value;
                SHR_EXIT();
            }
        }
        ALPM_ASSERT(0); /* Should exit already */
    }

    ALPM_ASSERT(i == RBKT_VBANKS(u, m, db, ln, group));
    SHR_ERR_EXIT(SHR_E_RESOURCE);
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
ver0_base_entry_clear(int u, int m, int db, int ln, ln_index_t ent_idx)
{
    int rbkt_idx;
    ln_base_entry_t base_entry = {0};
    rbkt_t *rbkt;
    int ent_ofs;
    int hw_idx;
    base_buckets_ver0_t *bbkts = NULL;
    SHR_FUNC_ENTER(u);

    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver0_hw_idx_from_rbkt_idx(u, m, db, ln, rbkt_idx);
    bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (bbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0], NULL));
    rbkt = &RBKT(u, m, db, ln, rbkt_idx);

    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_set(u, m, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &bbkts->rbuf[0][0]));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0],
                                    HIT_OP_NONE, 0, 0));

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));
exit:
    if (bbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], bbkts);
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Callback Functions
 */

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
ver0_bucket_copy(int u, int m, int db, int ln,
                 rbkt_group_t src_group,
                 uint8_t src_sub,
                 base_buckets_ver0_t *sbbkts,
                 int *src_rbkts,
                 ln_index_t *src_idx_arr,

                 rbkt_group_t dst_group,
                 uint8_t dst_sub,
                 base_buckets_ver0_t *dbbkts,
                 int *dst_rbkts,
                 ln_index_t *dst_idx_arr,
                 base_buckets_ver0_t *dbbkts_copy,

                 shr_dq_t *list,
                 int count,
                 alpm_arg_t *arg
                 )
{
    ln_index_t src_idx, dst_idx;
    shr_dq_t *elem;
    alpm_bkt_elem_t *bkt_elem;
    int src_format, dst_format;
    rbkt_idx_t src_rbkt_idx, dst_rbkt_idx;
    int src_bank, dst_bank;
    int src_ent, dst_ent;
    int i = 0, hw_idx;
    key_tuple_t sub_tp = {.len = dst_sub}; /* Used for transfering sub_bkt */
    SHR_FUNC_ENTER(u);

    /* sal_memset(dbbkts->rbuf, 0, sizeof(dbbkts->rbuf)); */

    SHR_DQ_BACK_TRAVERSE(list, elem) /* Longest pick format first */
        bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        src_idx = bkt_elem->index;
        src_idx_arr[i] = src_idx;
        SHR_IF_ERR_MSG_EXIT(
            ver0_base_entry_alloc(u, m, db, ln, dst_group, dst_sub,
                                  bkt_elem->key.t.len,
                                  DATA_REDUCED,
                                  &dst_idx,
                                  NULL, /* Will write anyway */
                                  &dst_format),
            (BSL_META("ver0_base_entry_alloc failed on group %d sub %d\n"),
             dst_group, dst_sub));
        dst_idx_arr[i++] = dst_idx;
        dst_rbkt_idx = RBKT_FROM_INDEX(dst_idx);
        dst_bank = dst_rbkt_idx - dst_group;
        dst_ent = ENT_OFS_FROM_IDX(dst_idx);

        if (dst_rbkts[dst_bank] == 0) {
            hw_idx = ver0_hw_idx_from_rbkt_idx(u, m, db, ln, dst_rbkt_idx);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx,
                                           &dbbkts->rbuf[dst_bank][0], NULL));
            sal_memcpy(dbbkts_copy->rbuf[dst_bank], dbbkts->rbuf[dst_bank],
                       sizeof(dbbkts_copy->rbuf[dst_bank]));
            dst_rbkts[dst_bank] = dst_rbkt_idx;
        }
        if (src_idx != INVALID_INDEX) {
            src_rbkt_idx = RBKT_FROM_INDEX(src_idx);
            src_ent = ENT_OFS_FROM_IDX(src_idx);
            src_bank = src_rbkt_idx - src_group;
            ALPM_ASSERT(src_bank >= 0 && src_bank < ALPM_MAX_BANKS);

            if (src_rbkts[src_bank] == 0) {
                hw_idx = ver0_hw_idx_from_rbkt_idx(u, m, db, ln, src_rbkt_idx);
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx,
                                               &sbbkts->rbuf[src_bank][0], NULL));
                src_rbkts[src_bank] = src_rbkt_idx;
            }

            src_format = RBKT_FORMAT(u, m, db, ln, src_rbkt_idx);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_move(u, m, format_type,
                                                  src_format,
                                                  src_ent,
                                                  NULL,
                                                  &sbbkts->rbuf[src_bank][0],
                                                  0,

                                                  dst_format,
                                                  dst_ent,
                                                  &sub_tp,
                                                  &dbbkts->rbuf[dst_bank][0]
                                                  ));
        } else {
            ln_base_entry_t base_entry;
#ifdef ALPM_DEBUG
            /*
             * Some reserved fileds won't be encoded, thus can be random.
             * Clear it for sanity check.
             */
            sal_memset(base_entry, 0, sizeof(base_entry));
#endif
            assert(i == 1); /* New entry, if any, must be the first one */
            arg->a1.sub_bkt = dst_sub; /* Set sub bucket for bucket entry */
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_encode(u, m, format_type,
                                                    dst_format,
                                                    0,
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
        }
        bkt_elem->index = dst_idx;
    SHR_DQ_TRAVERSE_END(list, elem);

    ALPM_ASSERT(i == count);
exit:
    if (SHR_FUNC_ERR() && i) {
        ALPM_ASSERT(i < count); /* Assume just entry alloc would fail */
        count = i;
        i = 0;
        SHR_DQ_BACK_TRAVERSE(list, elem)
            bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
            (void)ver0_base_entry_free(u, m, db, ln, dst_group,
                                       bkt_elem->index, NULL);
            bkt_elem->index = src_idx_arr[i++];
            if (i == count) {
                break;
            }
        SHR_DQ_TRAVERSE_END(list, elem);
    }

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
 * \param [in] src_sub Source sub bucket
 * \param [in] dst_group Dest group
 * \param [in] dst_sub Destination sub bucket
 * \param [in] list Move list
 * \param [in] count Count of bak_idx array
 * \param [in] arg New entry arg, in case new entry in split bucket.
 * \param [in] split_arg Arg for split pivot.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_move(int u, int m, int db, int ln,
                 rbkt_group_t src_group, uint8_t src_sub,
                 rbkt_group_t dst_group, uint8_t dst_sub,
                 shr_dq_t *list,
                 int count,
                 alpm_arg_t *arg,
                 alpm_arg_t *split_arg,
                 bkt_split_aux_t *split_aux)
{
    alpm_pivot_t *dst_pivot;
    ln_index_t *src_idx = NULL;
    ln_index_t *dst_idx = NULL;
    alpm_pivot_t *split_pivot;
    uint8_t i;
    int hw_idx;
    uint8_t decr_count;
    base_buckets_ver0_t *sbbkts = NULL;
    base_buckets_ver0_t *dbbkts = NULL;
    base_buckets_ver0_t *dbbkts_copy = NULL;
    int src_rbkts[ALPM_MAX_BANKS] = {0}; /* assuming RESERVED_GROUP = 0. */
    int dst_rbkts[ALPM_MAX_BANKS] = {0}; /* assuming RESERVED_GROUP = 0. */
    enum {BUCKET_MOVE_NONE, BUCKET_MOVE_COPY} done = BUCKET_MOVE_NONE;

    SHR_FUNC_ENTER(u);

    sbbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (sbbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    dbbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (dbbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    dbbkts_copy = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (dbbkts_copy == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* $ Save old ent_idx of prefixes */
    ALPM_ALLOC(src_idx, count * sizeof(ln_index_t) * 2, "bcmptmRmalpmV1idx");
    dst_idx = src_idx + count;
    /* sal_memset(src_idx, -1, count * sizeof(ln_index_t)); */

    /* $ (HW) Copy split prefixes from old bucket to new bucket */
    SHR_IF_ERR_VERBOSE_EXIT(
        ver0_bucket_copy(u, m, db, ln,
                         src_group,
                         src_sub,
                         sbbkts,
                         src_rbkts,
                         src_idx,

                         dst_group,
                         dst_sub,
                         dbbkts,
                         dst_rbkts,
                         dst_idx,
                         dbbkts_copy,

                         list,
                         count,
                         arg
                         ));
    done = BUCKET_MOVE_COPY;
    /* Write dest rbkts */
    for (i = 0; i < ALPM_MAX_BANKS; i++) {
        if (dst_rbkts[i]) {
            hw_idx = ver0_hw_idx_from_rbkt_idx(u, m, db, ln, dst_rbkts[i]);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx,
                                            &dbbkts->rbuf[i][0],
                                            HIT_OP_AUTO, 0, 0));
        }
    }

    /* $ Activate the pivot */
    if (split_arg) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_split_pivot_add(u, m, db, PREV_LEVEL(ln), split_arg,
                                           &split_pivot, split_aux));
    } else {
        dst_pivot = ver0_bucket_pivot_get(u, m, db, ln, dst_group, dst_sub);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln), dst_pivot,
                                        NULL,
                                        UPDATE_ALL));
    }

    /* $ Invalidate moved elems from old bucket in HW */
    /* Write source rbkts */
    for (i = 0; i < ALPM_MAX_BANKS; i++) {
        if (src_rbkts[i]) {
            hw_idx = ver0_hw_idx_from_rbkt_idx(u, m, db, ln, src_rbkts[i]);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx,
                                            &sbbkts->rbuf[i][0],
                                            HIT_OP_AUTO, 0, 0));

        }
    }

    /* $ Free those moved elems from parent buckets */
    SHR_IF_ERR_EXIT(
        ver0_base_entries_free(u, m, db, ln, src_group, src_idx, count));

    if (split_arg) {
        SHR_DQ_SWAP_HEAD(&split_pivot->bkt->list, list);
    }
    if (src_idx[0] == INVALID_INDEX) { /* Exists new entry */
        decr_count = count - 1;
    } else {
        decr_count = count;
    }
    ver0_rbkt_group_free_count_update(u, m, db, ln, src_group, -decr_count); /* decr */
    ver0_rbkt_group_free_count_update(u, m, db, ln, dst_group, count);
exit:
    /* Assume bucket copy and split_pivot_add are subject to fail */
    if (SHR_FUNC_ERR()) {
        if (done == BUCKET_MOVE_COPY) {
            shr_dq_t *elem;
            alpm_bkt_elem_t *bkt_elem;
            /* Restore dest rbkts */
            for (i = 0; i < ALPM_MAX_BANKS; i++) {
                if (dst_rbkts[i]) {
                    hw_idx = ver0_hw_idx_from_rbkt_idx(u, m, db, ln, dst_rbkts[i]);
                    if (SHR_FAILURE(
                        bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx,
                                                    &dbbkts_copy->rbuf[i][0],
                                                    HIT_OP_NONE, 0, 0))) {
                        break;
                    }
                }
            }
            i = 0;
            SHR_DQ_BACK_TRAVERSE(list, elem)
                bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
                (void)ver0_base_entry_free(u, m, db, ln, dst_group,
                                           bkt_elem->index, NULL);
                bkt_elem->index = src_idx[i++];
                if (i == count) {
                    break;
                }
            SHR_DQ_TRAVERSE_END(list, elem);
        }
    }
    if (src_idx != NULL) {
        SHR_FREE(src_idx);
    }
    if (sbbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], sbbkts);
    }
    if (dbbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], dbbkts);
    }
    if (dbbkts_copy) {
        shr_lmm_free(base_buckets_lmem[u][m], dbbkts_copy);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Merge entries from one bucket to another.
 *
 * The original bucket will be freed, and elems in new bucket are likely
 * reorganized to be more efficient.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Source group
 * \param [in] src_sub Source sub bucket
 * \param [in] dst_group Dest group
 * \param [in] dst_sub Destination sub bucket
 * \param [in] merge_type Merge type
 * \param [out] shuffled Buckets shuffled
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_merge(int u, int m, int db, int ln,
                  rbkt_group_t src_group, uint8_t src_sub,
                  rbkt_group_t dst_group, uint8_t dst_sub,
                  bucket_merge_type_t merge_type,
                  bool *shuffled)
{
    uint8_t count;
    alpm_pivot_t *src_pivot, *dst_pivot;
    alpm_pivot_t *parent_pivot, *child_pivot;
    enum {
        BUCKET_MERGE_NONE,
        BUCKET_MERGE_COPY,
        BUCKET_MERGE_CHILD_TRIE_MERGE,
        BUCKET_MERGE_CHILD_PIVOT_DELETE,
    } done = BUCKET_MERGE_NONE;
    alpm_arg_t *arg = NULL;
    SHR_FUNC_ENTER(u);

    src_pivot = ver0_bucket_pivot_get(u, m, db, ln, src_group, src_sub);
    dst_pivot = ver0_bucket_pivot_get(u, m, db, ln, dst_group, dst_sub);
    ALPM_ASSERT(src_pivot);
    ALPM_ASSERT((merge_type != BUCKET_SHARE && dst_pivot) ||
                (merge_type == BUCKET_SHARE && !dst_pivot));

    count = bcmptm_rm_alpm_trie_count(src_pivot->bkt->trie);
    if (merge_type != BUCKET_MERGE_CHILD_TO_PARENT) {
        ver0_bucket_pivot_set(u, m, db, ln, dst_group, dst_sub, src_pivot);
    }
    /* Set new logical bucket. Do this ahead for tcam entry link.
     * If merge child to parent, then the src_pivot is child pivot, in which
     * case, it will be freed.
     */
    src_pivot->bkt->log_bkt = VER0_LOG_BKT(dst_group, dst_sub);

    SHR_IF_ERR_EXIT(
        ver0_bucket_move(u, m, db, ln,
                         src_group,
                         src_sub,
                         dst_group,
                         dst_sub,
                         &src_pivot->bkt->list,
                         count,
                         NULL, /* no new entry */
                         NULL,
                         NULL));
    done = BUCKET_MERGE_COPY;
    if (merge_type != BUCKET_SHARE) { /* For true merge */

        if (merge_type == BUCKET_MERGE_PARENT_TO_CHILD) {
            parent_pivot = src_pivot;
            child_pivot  = dst_pivot;
        } else if (merge_type == BUCKET_MERGE_CHILD_TO_PARENT) {
            child_pivot  = src_pivot;
            parent_pivot = dst_pivot;
        }

        /* SW bucket merge: not expect to fail */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_merge(u, m, parent_pivot->bkt->trie,
                                      child_pivot->bkt->trie->trie,
                                      &child_pivot->key.t.trie_ip.key[0],
                                      child_pivot->key.t.len));
        done = BUCKET_MERGE_CHILD_TRIE_MERGE;
        arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
        if (arg == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        bcmptm_rm_alpm_pivot_arg_init(u, m, db, PREV_LEVEL(ln), child_pivot,
                                      UPDATE_KEY, arg);
        /* Child pivot will be deleted and freed */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_delete(u, m, PREV_LEVEL(ln), arg));
        done = BUCKET_MERGE_CHILD_PIVOT_DELETE;
        if (shuffled) {
            *shuffled = true;
        }
    }

    ver0_bucket_pivot_clr(u, m, db, ln, src_group, src_sub);
    /* do not use ver0_bucket_free, because we still want this group.
    SHR_IF_ERR_EXIT(
        ver0_bucket_free(u, m, db, ln, src_group, src_sub));
    */

exit:
    if (SHR_FUNC_ERR()) {
        /*  Not expecting any failure after pivot deleted */
        assert(done != BUCKET_MERGE_CHILD_PIVOT_DELETE);
        switch (done)  {
        case BUCKET_MERGE_CHILD_TRIE_MERGE:
            if (merge_type != BUCKET_SHARE) {
                uint32_t tmp_pivot_len;
                trie_ip_t tmp_pivot_key;
                bcmptm_rm_alpm_trie_split2(u, m, parent_pivot->bkt->trie,
                                           &child_pivot->key.t.trie_ip.key[0],
                                           child_pivot->key.t.len,
                                           &tmp_pivot_key.key[0],
                                           &tmp_pivot_len,
                                           &child_pivot->bkt->trie->trie);
            }
            /* Fall through */
        case BUCKET_MERGE_COPY:
            if (merge_type != BUCKET_MERGE_CHILD_TO_PARENT) {
                ver0_bucket_pivot_clr(u, m, db, ln, dst_group, dst_sub);
            }
            /* Fall through */
        case BUCKET_MERGE_NONE:
        default:
            break;
        }
    }
    if (arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find two qualified group and merge, such that one become free.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [out] free_group Free group
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_share(int u, int m, int db, int ln, rbkt_group_t *free_group)
{
    int cnt, cnt2;
    uint8_t bkt_cnt;
    rbkt_group_t src_group, dst_group;
    uint8_t fmt, first_round, src_sbkts;
    shr_dq_t *group_list, *group_list2;
    shr_dq_t *elem;
    static const uint8_t fast_popcount4[] = {
         0,  /* 0000 */
         1,  /* 0001 */
         1,  /* 0010 */
         2,  /* 0011 */
         1,  /* 0100 */
         2,  /* 0101 */
         2,  /* 0110 */
         3,  /* 0111 */
         1,  /* 1000 */
         2,  /* 1001 */
         2,  /* 1010 */
         3,  /* 1011 */
         2,  /* 1100 */
         3,  /* 1101 */
         3,  /* 1110 */
         4   /* 1111 */
    };
    SHR_FUNC_ENTER(u);

    dst_group = 0;

    for (fmt = VER0_FORMAT_1; fmt <= VER0_MAX_FORMAT(u, m, FORMAT_ROUTE); fmt++) {

    bkt_cnt= ver0_base_entry_count(u, m, db, ln, fmt);
    /* Assure total count of two buckets won't exceed max */
    first_round = 1;
    src_group = 0;
    src_sbkts = 0;
    for (cnt = 1; cnt <= (bkt_cnt >> 1); cnt++) {
        rbkt_t *curr;
        group_list = &FMT_GROUP_USAGE_LIST(u, m, db, ln, fmt, cnt);

        SHR_DQ_TRAVERSE(group_list, elem) /* Group 0 won't be traversed */
            curr = SHR_DQ_ELEMENT_GET(rbkt_t *, elem, fmt_usgnode);
            ALPM_ASSERT(curr->sub_bkts && curr->sub_bkts != 0xF);
            ALPM_ASSERT(curr->count == cnt);
            if (first_round) {
                if (src_group == 0) {
                    src_group = curr->index;
                    src_sbkts = curr->sub_bkts;
                } else if (
                    VER0_CAN_BUCKET_SHARE(u, m, db, ln, src_group, curr->index)) {
                    dst_group = curr->index;
                    goto done;
                } else if (fast_popcount4[curr->sub_bkts] <
                           fast_popcount4[src_sbkts]) {
                    src_group = curr->index;
                    src_sbkts = curr->sub_bkts;
                }
            } else {
                if (src_group &&
                    VER0_CAN_BUCKET_SHARE(u, m, db, ln, src_group, curr->index)) {
                    dst_group = curr->index;
                    goto done;
                } else if (fast_popcount4[curr->sub_bkts] <
                           fast_popcount4[src_sbkts]) {
                    /*
                     * We have found a src_group with least entries in the
                     * first round, but there is no dst_group can share with
                     * it. In the following rounds, the src_group will be with
                     * more entries, therefore the only chance to share is to
                     * look for a src_group that is with less sub_buckets.
                     */
                    src_group = curr->index;
                    src_sbkts = curr->sub_bkts;
                }
            }
        SHR_DQ_TRAVERSE_END(group_list, elem);

        if (src_group == 0) {
            continue;
        }

        first_round = 0;
        /* Reverse order to make group compact */
        for (cnt2 = (bkt_cnt - cnt); cnt2 >= cnt + 1; cnt2--) {
/*        for (cnt2 = cnt +1 ; cnt2 <= (bkt_cnt - cnt); cnt2++) { */
            rbkt_t *curr2;
            group_list2 = &FMT_GROUP_USAGE_LIST(u, m, db, ln, fmt, cnt2);
            SHR_DQ_TRAVERSE(group_list2, elem) /* Group 0 won't be traversed */
                curr2 = SHR_DQ_ELEMENT_GET(rbkt_t *, elem, fmt_usgnode);
                ALPM_ASSERT(curr2->sub_bkts && curr2->sub_bkts != 0xF);
                if (VER0_CAN_BUCKET_SHARE(u, m, db, ln, src_group, curr2->index)) {
                    dst_group = curr2->index;
                    goto done;
                }
            SHR_DQ_TRAVERSE_END(group_list, elem);
        }
        src_group = 0;
    }
    }
done:
    if (dst_group) {
        uint8_t src_sub, dst_sub;
        for (src_sub = 0; src_sub < MAX_SUB_BKTS; src_sub++) {
            if ((RBKT_SUB_BKT_IS_SET(u, m, db, ln, src_group, src_sub))) {
                SHR_IF_ERR_EXIT(
                    ver0_sub_bucket_alloc(u, m, db, ln, dst_group, &dst_sub));
                SHR_IF_ERR_EXIT(
                   ver0_bucket_merge(u, m, db, ln, src_group, src_sub,
                                     dst_group, dst_sub, BUCKET_SHARE, NULL));
            }
        }

        *free_group = src_group;
    } else {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    bcmptm_rm_alpm_internals[u][m][db][ln].ver0_bucket_share++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find the most free rbkt group and carve out a sub bucket from it.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] fmt Format value
 * \param [out] group Group with most free entries
 * \param [out] sub_bkt Sub bucket carved from group
 *
 * \return SHR_E_NONE only.
 */
static int
ver0_bucket_carve(int u, int m, int db, int ln, uint8_t fmt,
                  rbkt_group_t *group, uint8_t *sub_bkt)
{
    uint8_t cnt, max_cnt;
    shr_dq_t *group_list, *elem;
    rbkt_t *curr;
    SHR_FUNC_ENTER(u);

    /*
     * Try to alloc a sub bucket from least occupied bucket of
     * same memory fmt.
     */
    ALPM_ASSERT(fmt != DISABLED_FORMAT);
    max_cnt = ver0_base_entry_count(u, m, db, ln, fmt);
    for (cnt = 1; cnt < max_cnt - 1; cnt++) { /* at least two free entry */
        group_list = &FMT_GROUP_USAGE_LIST(u, m, db, ln, fmt, cnt);
        SHR_DQ_TRAVERSE(group_list, elem)
            curr = SHR_DQ_ELEMENT_GET(rbkt_t *, elem, fmt_usgnode);
            ALPM_ASSERT(curr->sub_bkts != 0xF);
            *group = curr->index;
            return ver0_sub_bucket_alloc(u, m, db, ln, *group, sub_bkt);
        SHR_DQ_TRAVERSE_END(group_list, elem);
    }
    SHR_ERR_EXIT(SHR_E_RESOURCE);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a logical bucket.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [out] group Group
 * \param [out] sub_bkt Sub bucket
 *
 * \return SHR_E_NONE only.
 */
static inline int
ver0_bucket_free(int u, int m, int db, int ln, rbkt_group_t group, uint8_t sub_bkt)
{

    if (group != INVALID_GROUP) { /* Invalid group does not exist */
        ver0_bucket_pivot_clr(u, m, db, ln, group, sub_bkt);

        if (RBKT_ECOUNT(u, m, db, ln, group) == 0) {
            return ver0_rbkt_group_free(u, m, db, ln, group);
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Allocate a new logical bucket with various sizes.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] fmt Format value
 * \param [out] group New group allocated
 * \param [out] sub_bkt New sub bucket allocated
 *
 * \return SHR_E_NONE only.
 */
static int
ver0_bucket_alloc(int u, int m, int db, int ln, uint8_t fmt,
                  rbkt_group_t *group, uint8_t *sub_bkt)
{
    int rv;
    uint8_t size;
    SHR_FUNC_ENTER(u);

    size = bucket_scale[u][m];
    rv = ver0_rbkt_group_alloc(u, m, db, ln, fmt, size, false, group);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_EXIT(
            ver0_sub_bucket_alloc(u, m, db, ln, *group, sub_bkt));
    } else if (rv == SHR_E_RESOURCE) {
        rv = ver0_bucket_share(u, m, db, ln, group);
        if (SHR_SUCCESS(rv)) {
            uint8_t i;
            ALPM_ASSERT(RBKT_VBANKS(u, m, db, ln, *group) ==
                        size * BKT_HDL_MAX_BANKS(u, m, db, ln));
            for (i = 0; i < RBKT_VBANKS(u, m, db, ln, *group); i++) {
                RBKT_FORMAT(u, m, db, ln, i + *group) = fmt;
            }
            SHR_IF_ERR_EXIT(
                ver0_sub_bucket_alloc(u, m, db, ln, *group, sub_bkt));
            ALPM_ASSERT(*sub_bkt == 0);
            ALPM_ASSERT(RBKT_ECOUNT(u, m, db, ln, *group) == 0);
        } else if (rv == SHR_E_RESOURCE) {
            SHR_IF_ERR_EXIT(
                ver0_bucket_carve(u, m, db, ln, fmt, group, sub_bkt));
            ALPM_ASSERT(RBKT_ECOUNT(u, m, db, ln, *group));
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
        /* Reclaim the physical bucket in case it has been freed */
        ALPM_ASSERT(RBKT_IS_HEAD(u, m, db, ln, *group));
    }
    /* Succeed */
    ALPM_ASSERT(RBKT_FORMAT(u, m, db, ln, *group) == fmt);
exit:
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
 * \param [in] src_sub Source sub bucket
 * \param [in] arg New entry arg, in case new entry in split bucket.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_alloc_and_move(int u, int m, int db, int ln,
                           rbkt_group_t src_group,
                           uint8_t src_sub,
                           alpm_arg_t *arg)
{
    int count;
    rbkt_group_t new_group;
    uint8_t new_sub, fmt;
    alpm_pivot_t *src_pivot;
    enum {
        ALLOC_AND_MOVE_NONE,
        ALLOC_AND_MOVE_ALLOC,
        ALLOC_AND_MOVE_MOVE
    } done = ALLOC_AND_MOVE_NONE;
    SHR_FUNC_ENTER(u);

    src_pivot = ver0_bucket_pivot_get(u, m, db, ln, src_group, src_sub);
    fmt = RBKT_FORMAT(u, m, db, ln, src_group);
    SHR_IF_ERR_EXIT(
        ver0_bucket_alloc(u, m, db, ln, fmt, &new_group, &new_sub));
    ver0_bucket_pivot_set(u, m, db, ln, new_group, new_sub, src_pivot);

    count = bcmptm_rm_alpm_trie_count(src_pivot->bkt->trie);
    src_pivot->bkt->log_bkt = VER0_LOG_BKT(new_group, new_sub);
    done = ALLOC_AND_MOVE_ALLOC;

    SHR_IF_ERR_VERBOSE_EXIT(
        ver0_bucket_move(u, m, db, ln,
                         src_group,
                         src_sub,
                         new_group,
                         new_sub,
                         &src_pivot->bkt->list,
                         count,
                         arg, /* New entry */
                         NULL,
                         NULL));

    done = ALLOC_AND_MOVE_MOVE;
    SHR_IF_ERR_EXIT(
        ver0_bucket_free(u, m, db, ln, src_group, src_sub));
/*    src_pivot->bkt->log_bkt = VER0_LOG_BKT(new_group, new_sub); */
exit:
    if (SHR_FUNC_ERR()) {
        switch (done) {
        case ALLOC_AND_MOVE_MOVE:
            ALPM_ASSERT(0); /* not supposed to fail */
        case ALLOC_AND_MOVE_ALLOC:
            src_pivot->bkt->log_bkt = VER0_LOG_BKT(src_group, src_sub);
            ver0_bucket_free(u, m, db, ln, new_group, new_sub);
        case ALLOC_AND_MOVE_NONE:
        default:
            break;
        }
    }
    SHR_FUNC_EXIT();
}

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
ver0_base_entry_encode_write(int u, int m, int ln, alpm_arg_t *arg)
{
    int rbkt_idx;
    ln_base_entry_t base_entry = {0};
    rbkt_t *rbkt;
    ln_index_t ent_idx;
    int ent_ofs, hw_idx;
    int pivot_len, db;
    alpm_pivot_t *pivot;
    base_buckets_ver0_t *bbkts = NULL;
    uint8_t hit_support;
    SHR_FUNC_ENTER(u);

    ent_idx = arg->index[ln];

    db = arg->db;
    pivot = arg->pivot[PREV_LEVEL(ln)];
    pivot_len = pivot->key.t.len;
    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver0_hw_idx_from_rbkt_idx(u, m, db, ln, rbkt_idx);
    bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (bbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0], NULL));

    rbkt = &RBKT(u, m, db, ln, rbkt_idx);

    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    arg->a1.sub_bkt = VER0_SUB_BKT(u, m, pivot->bkt->log_bkt);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_encode(u, m, format_type,
                                            rbkt->format,
                                            pivot_len,
                                            arg,
                                            &base_entry[0]));
#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        alpm_arg_t arg2;
        bcmptm_rm_alpm_arg_init(u, m, &arg2);
        /* Decode needs non-info except the rbkt info */
        arg2.key.t.max_bits = arg->key.t.max_bits;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_decode(u, m, format_type,
                                                rbkt->format,
                                                pivot_len,
                                                &arg2,
                                                &base_entry[0]));
        /* Ver0 stores full ip in bucket, thus don't need api key create*/
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_len_to_mask(u, m, arg2.key.t.max_bits, arg2.key.t.len,
                                       &arg2.key.ip_mask[0]));

        ALPM_ASSERT(!sal_memcmp(arg2.key.ip_addr, arg->key.ip_addr,
                                sizeof(alpm_ip_t)));
        ALPM_ASSERT(!sal_memcmp(arg2.key.ip_mask, arg->key.ip_mask,
                                sizeof(alpm_ip_t)));
        ALPM_ASSERT(arg2.key.t.len == arg->key.t.len);
        ALPM_ASSERT(arg2.key.t.max_bits == arg->key.t.max_bits);
    }
#endif

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_set(u, m, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &bbkts->rbuf[0][0]));

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0],
                                    HIT_OP_NONE, 0, 0));

    hit_support = bcmptm_rm_alpm_hit_support(u, m);
    if (hit_support != HIT_MODE_DISABLE) { /* Write hit */
        uint32_t eidx, hit_entry[1];
        if (hit_support == HIT_MODE_FORCE_CLEAR) {
            hit_entry[0] = 0;
        } else {
            hit_entry[0] = 0xFFFF;
        }
        eidx = ver0_hw_eidx_from_ent_idx(u, m, db, ln, ent_idx);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_hit_set(u, m, ln, (eidx & 3), hit_entry, arg->hit));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_write_ln_hit(u, m, ln, eidx >> 2, hit_entry));
        bcmptm_rm_alpm_internals[u][m][db][ln].write++;
    }

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));
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
ver0_base_entry_decode_read(int u, int m, int ln, alpm_arg_t *arg)
{
    int rbkt_idx;
    ln_base_entry_t base_entry = {0};
    rbkt_t *rbkt;
    int ent_ofs, hw_idx;
    int db = arg->db;
    ln_index_t ent_idx;
    base_buckets_ver0_t *bbkts = NULL;
    SHR_FUNC_ENTER(u);

    ent_idx = arg->index[ln];
    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver0_hw_idx_from_rbkt_idx(u, m, db, ln, rbkt_idx);
    bbkts = shr_lmm_alloc(base_buckets_lmem[u][m]);
    if (bbkts == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, m, db, ln, hw_idx, &bbkts->rbuf[0][0], NULL));

    rbkt = &RBKT(u, m, db, ln, rbkt_idx);

    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_get(u, m, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &bbkts->rbuf[0][0]));
    /* Decode needs non-info except the rbkt info */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_decode(u, m, format_type,
                                            rbkt->format,
                                            0, /* Not used */
                                            arg,
                                            &base_entry[0]));

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));

    /* Ver0 stores full ip in bucket, thus don't need api key create. */
    /*
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_api_key_create(u, m, arg->key.t.max_bits,
                                      &arg->key.t.trie_ip,
                                      arg->key.t.len,
                                      &arg->key.ip_addr[0]));
                                      */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_len_to_mask(u, m, arg->key.t.max_bits,
                                   arg->key.t.len,
                                   &arg->key.ip_mask[0]));

    if (bcmptm_rm_alpm_hit_support(u, m) != HIT_MODE_DISABLE) { /* Read hit */
        uint32_t eidx;
        uint8_t hit;
        uint32_t hit_entry;
        eidx = ver0_hw_eidx_from_ent_idx(u, m, db, ln, ent_idx);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_hit(u, m, ln, eidx >> 2, &hit_entry));
        /* Get hit bit */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_hit_get(u, m, ln, (eidx & 3), &hit_entry, &hit));
        arg->hit = hit;
    }
exit:
    if (bbkts) {
        shr_lmm_free(base_buckets_lmem[u][m], bbkts);
    }
    SHR_FUNC_EXIT();
}


static int
ver0_bucket_resource_check(int u, int m, alpm_arg_t *arg, int extra)
{
    int8_t ln, db = arg->db;
    int free_rbkts;
    alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, m, db);

    if (bcmptm_rm_alpm_l1_can_insert(u, m, arg) == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "Level 1 resource full\n")));
        return SHR_E_RESOURCE;
    }

    for (ln = LEVEL2; ln < database_ptr->max_levels; ln++) {
        free_rbkts = BKT_HDL_RBKTS_TOTAL(u, m, db, ln) -
                     BKT_HDL_RBKTS_USED(u, m, db, ln);
        if (free_rbkts <= (BKT_HDL_MAX_BANKS(u, m, db, ln) + extra)) {
            return SHR_E_RESOURCE;
        }
    }
    return SHR_E_NONE;
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
ver0_bucket_insert(int u, int m, int ln, alpm_arg_t *arg,
                   alpm_pivot_t *pivot)
{
    int format_value;
    int bucket_len = arg->key.t.len - pivot->key.t.len;
    rbkt_group_t group;
    ln_index_t ent_idx = INVALID_INDEX;
    int rv;
    uint8_t sub_bkt;
    SHR_FUNC_ENTER(u);

    if (pivot->bkt->log_bkt == INVALID_LOG_BKT) {
        /*
         * If pivot exists, but bucket group is invalid,
         * this implies the pivot is just itself without a bucket.
         * So we should alloc a bucket group for it first.
         */
        ALPM_ASSERT(pivot->bkt->trie->trie == NULL);
        rv = ver0_bucket_resource_check(u, m, arg,
                                        arg->dt_update ?
                                        EXTRA_UPDATE_DT:
                                        EXTRA_NEW_INSERT);
        if (rv == SHR_E_RESOURCE) {
            arg->resouce_full = 1;
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }

        SHR_IF_ERR_EXIT(
            ver0_bucket_alloc(u, m, arg->db, ln, arg->fmt, &group, &sub_bkt));
        ver0_bucket_pivot_set(u, m, arg->db, ln, group, sub_bkt, pivot);
        pivot->bkt->log_bkt = VER0_LOG_BKT(group, sub_bkt);
    } else {
        group = VER0_GROUP(u, m, pivot->bkt->log_bkt);
        sub_bkt = VER0_SUB_BKT(u, m, pivot->bkt->log_bkt);
    }
    rv = ver0_base_entry_alloc(u, m, arg->db, ln, group, sub_bkt,
                               bucket_len,
                               DATA_REDUCED,
                               &ent_idx,
                               NULL,
                               &format_value);
    if (rv == SHR_E_RESOURCE) {
        rv = ver0_bucket_resource_check(u, m, arg,
                                        arg->dt_update ?
                                        EXTRA_UPDATE_DT:
                                        EXTRA_NEW_INSERT);
        if (rv == SHR_E_RESOURCE) {
            arg->resouce_full = 1;
        }
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    ALPM_ASSERT(ent_idx != INVALID_INDEX);
    arg->pivot[ln]->index = ent_idx;
    arg->index[ln] = ent_idx;
    arg->pivot[PREV_LEVEL(ln)] = pivot; /* Set pivot for pivot_len */
    SHR_IF_ERR_EXIT(
        ver0_base_entry_encode_write(u, m, ln, arg));
    ver0_rbkt_group_free_count_update(u, m, arg->db, ln, group, 1);
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
ver0_bucket_delete(int u, int m, int db, int ln,
                   alpm_pivot_t *pivot,
                   ln_index_t ent_idx)
{
    rbkt_group_t group = VER0_GROUP(u, m, pivot->bkt->log_bkt);
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        ver0_base_entry_clear(u, m, db, ln, ent_idx));

    SHR_IF_ERR_EXIT(
        ver0_base_entry_free(u, m, db, ln, group, ent_idx, NULL));

    ver0_rbkt_group_free_count_update(u, m, db, ln, group, -1);
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
ver0_bucket_split_insert(int u, int m, int ln, alpm_arg_t *arg,
                         alpm_pivot_t *parent_pivot,
                         bkt_split_aux_t *split_aux)
{
    int rv;
    rbkt_group_t new_group, parent_group;
    uint8_t new_sub, parent_sub;
    alpm_pivot_t *split_pivot;
    uint32_t msl, msc;
    rm_alpm_trie_node_t *split_root;
    uint32_t pivot_len;
    uint8_t db = arg->db;
    trie_ip_t pivot_key;
    alpm_arg_t *split_arg = NULL;
    int prev_ln = ln - 1;
    enum {
        SPLIT_NONE,
        SPLIT_ALLOC_GROUP,
        SPLIT_TRIE_SPLIT,
        SPLIT_BUCKET_CONNECT,
        SPLIT_BUCKET_MOVE_AND_ACTIVATE,
        SPLIT_OLD_BUCKET_CLEANUP,
    } done = SPLIT_NONE;
    bool new_in_parent = false, new_in_split = false;
    shr_dq_t split_list;

    SHR_FUNC_ENTER(u);

    /*
     * $ Allocate a new group.
     * First try with simple alloc (no shuffle) for several sizes.
     * The shuffle alloc kicks in right before trying with least size.
     */
    SHR_IF_ERR_EXIT(
        ver0_bucket_alloc(u, m, db, ln, arg->fmt, &new_group, &new_sub));
    arg->log_bkt[prev_ln] = VER0_LOG_BKT(new_group, new_sub);
    done = SPLIT_ALLOC_GROUP;

    /* $ Trie split */
    parent_group = VER0_GROUP(u, m, parent_pivot->bkt->log_bkt);
    parent_sub = VER0_SUB_BKT(u, m, parent_pivot->bkt->log_bkt);
    msl = ver0_trie_msl(u, m, arg->key.t.ipv);
    msc = ver0_trie_msc(u, m, db, ln, arg->fmt, new_group);

    /* For non-last level, payload split can save us a lot of trouble */
    rv = bcmptm_rm_alpm_trie_split(u, m, parent_pivot->bkt->trie,
                                   msl,
                                   &pivot_key.key[0],
                                   &pivot_len,
                                   &split_root,
                                   NULL,
                                   NULL,
                                   false,
                                   msc);
    if (SHR_SUCCESS(rv)) {
        if (pivot_len > msl) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        done = SPLIT_TRIE_SPLIT;
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    /* $ Connect bkt elems in new split bucket */
    SHR_IF_ERR_EXIT(
        ver0_bkt_elems_connect(u, m, &split_list, split_root,
                               &new_in_split));

    /* $ Connect bkt elems in parent bucket */
    SHR_IF_ERR_EXIT(
        ver0_bkt_elems_connect(u, m, &parent_pivot->bkt->list,
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

    SHR_IF_ERR_EXIT(
        ver0_bucket_move(u, m, db, ln,
                         parent_group,
                         parent_sub,
                         new_group,
                         new_sub,
                         &split_list,
                         split_root->count,
                         arg, /* May have new entry */
                         split_arg,
                         split_aux));

    SHR_IF_ERR_EXIT(rv);

    split_pivot = split_arg->pivot[prev_ln];
    if (new_in_split) {
        arg->pivot[prev_ln] = split_pivot;
    }

    ALPM_ASSERT(split_pivot->bkt->trie);
    split_pivot->bkt->trie->trie = split_root;
    ver0_bucket_pivot_set(u, m, db, ln, new_group, new_sub, split_pivot);
    done = SPLIT_BUCKET_MOVE_AND_ACTIVATE;

    if (RBKT_ECOUNT(u, m, db, ln, parent_group) == 0 && !new_in_parent) {
        /*
         * If it's in the middle of split, there is a possiblity
         * the parent_group has all it's entries moved to another group,
         * while the new entry is about to insert into the parent group.
         * IN this case, the parent_pivot has one payload, yet
         * the ECOUNT is not updated, i.e, 0.
         */
        ALPM_ASSERT(parent_pivot->bkt->trie->trie == NULL ||
                    parent_pivot->bkt->trie->trie->count == 1);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_clean(u, m, db, ln, &parent_pivot));
    }
    done = SPLIT_OLD_BUCKET_CLEANUP;
    if (new_in_parent) {
        rv = ver0_bucket_insert(u, m, ln, arg, parent_pivot);
        if (rv == SHR_E_RESOURCE) {
            /* Take the chance to reorganize (also try to insert again) */
            rv = ver0_bucket_alloc_and_move(u, m, db, ln,
                                            parent_group, parent_sub, arg);
        }
        SHR_IF_ERR_EXIT(rv);
    }
    VER0_RBKT_GROUP_SANITY(u, m, db, ln, new_group);
    VER0_RBKT_GROUP_SANITY(u, m, db, ln, parent_group);
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
             * ver0_bucket_pivot_clr(u, m, db, ln, new_group) = NULL;  will be done in
             * ver0_bucket_free.
             */
            /*
             * If E_RESOURCE (not expecting)
             * we might break immediatelly to trigger abort early.
             */
            ALPM_ASSERT(SHR_FUNC_VAL_IS(SHR_E_RESOURCE));
        case SPLIT_BUCKET_CONNECT:
            /* Elems will be reconnected in following op */
        case SPLIT_TRIE_SPLIT:
            bcmptm_rm_alpm_trie_merge(u, m, parent_pivot->bkt->trie,
                                      split_root,
                                      &pivot_key.key[0],
                                      pivot_len);
            ver0_bkt_elems_connect(u, m, &parent_pivot->bkt->list,
                                   parent_pivot->bkt->trie->trie,
                                   &new_in_parent);
            /* Fall through */
        case SPLIT_ALLOC_GROUP:
            ver0_bucket_free(u, m, db, ln, new_group, new_sub);
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
    SHR_FUNC_EXIT();
}


static inline int
ver0_log_bucket_free(int u, int m, int db, int ln, log_bkt_t log_bkt)
{
    return ver0_bucket_free(u, m, db, ln, VER0_GROUP(u, m, log_bkt),
                            VER0_SUB_BKT(u, m, log_bkt));
}

static int
ver0_log_bucket_alloc(int u, int m, int ln, alpm_arg_t *arg, log_bkt_t *log_bkt)
{
    rbkt_group_t group;
    uint8_t sub_bkt;
    int rv;
    rv = ver0_bucket_alloc(u, m, arg->db, ln, arg->fmt, &group, &sub_bkt);
    if (SHR_SUCCESS(rv)) {
        *log_bkt = VER0_LOG_BKT(group, sub_bkt);
    }
    return rv;
}

static inline void
ver0_log_bucket_pivot_set(int u, int m, int db, int ln, log_bkt_t log_bkt,
                          alpm_pivot_t *pivot)
{
    ver0_bucket_pivot_set(u, m, db, ln, VER0_GROUP(u, m, log_bkt),
                          VER0_SUB_BKT(u, m, log_bkt), pivot);
}

static alpm_pivot_t *
ver0_log_bucket_pivot_get(int u, int m, int db, int ln, ln_index_t index)
{
    rbkt_idx_t rbkt_idx;
    uint8_t sub_bkt;

    rbkt_idx = RBKT_FROM_INDEX(index);
    ALPM_ASSERT(rbkt_idx >= 0 && rbkt_idx < BKT_HDL_RBKTS_TOTAL(u, m, db, ln));
    while (!RBKT_IS_HEAD(u, m, db, ln, rbkt_idx) && rbkt_idx >= 0) {
        rbkt_idx--;
    }
    ALPM_ASSERT(rbkt_idx >= 0);

    sub_bkt = SUB_BKT_FROM_IDX(index);
    return ver0_bucket_pivot_get(u, m, db, ln, rbkt_idx, sub_bkt);
}

static void
ver0_alpm_data_encode(int u, int m, int ln, alpm_arg_t *arg, void *adata)
{
    alpm1_data_t *a1;
    rbkt_group_t group;

    if (arg->log_bkt[LEVEL1] == INVALID_LOG_BKT) {
        group = RESERVED_GROUP;
    } else {
        group = VER0_GROUP(u, m, arg->log_bkt[LEVEL1]);
    }

    a1 = (alpm1_data_t *)adata;
    a1->direct_route = 0; /* to skip Level-1 pivot mgmt */
    a1->default_miss = arg->default_miss;
    a1->phy_bkt = (group + BKT_HDL_BASE_RBKT(u, m, arg->db, NEXT_LEVEL(ln))) /
                  BKT_HDL_MAX_BANKS(u, m, arg->db, NEXT_LEVEL(ln));
    a1->sub_bkt = VER0_SUB_BKT(u, m, arg->log_bkt[LEVEL1]);
}

static int
ver0_bucket_recover(int u, int m, int ln, alpm_arg_t *arg)
{
    uint8_t sub_bkt;
    rbkt_group_t group;
    int db = arg->db;

    SHR_FUNC_ENTER(u);
    group = arg->a1.phy_bkt * BKT_HDL_MAX_BANKS(u, m, arg->db, ln) -
            BKT_HDL_BASE_RBKT(u, m, arg->db, ln);
    sub_bkt = arg->a1.sub_bkt;

    if (group == RESERVED_GROUP) {
        arg->log_bkt[PREV_LEVEL(ln)] = INVALID_LOG_BKT;
    } else {
        SHR_IF_ERR_EXIT(
            ver0_log_bucket_recover(u, m, db, ln, group, sub_bkt, arg));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
ver0_bucket_recover_done(int u, int m, int db, int ln)
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
        ver0_rbkt_group_free_bank_set(u, m, db, ln, prev_rbkt->index, free_banks);
        prev_group = group;
    }
    prev_rbkt = &RBKT(u, m, db, ln, prev_group);
    free_banks = BKT_HDL_RBKTS_TOTAL(u, m, db, ln) -
                 prev_rbkt->index - prev_rbkt->valid_banks;
    ALPM_ASSERT((int)free_banks >= 0);
    ver0_rbkt_group_free_bank_set(u, m, db, ln, prev_rbkt->index, free_banks);

    return SHR_E_NONE;
}

static int
ver0_bkt_hdl_init(int u, int m, bool recover)
{
    alpm_dev_info_t *dev_info;
    int db_idx, level;
    int db_count;
    int db_max_levels[DBS] = {0};
    uint8_t max_count, fmt;
    int rv;
    int is_urpf = 0;
    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);
    VER0_MAX_FORMAT(u, m, FORMAT_PIVOT) = dev_info->pivot_max_format_1;
    VER0_MAX_FORMAT(u, m, FORMAT_ROUTE) = dev_info->route_max_format_1;

    if (!bcmptm_rm_alpm_is_parallel(u, m) && !is_urpf) {
        bucket_scale[u][m] = 2;
    } else {
        bucket_scale[u][m] = 1;
    }

    db_count = bcmptm_rm_alpm_database_count(u, m);
    for (db_idx = 0; db_idx < db_count; db_idx++) {
        alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, m, db_idx);
        db_max_levels[db_idx] = database_ptr->max_levels;
        if (db_max_levels[db_idx] > 2) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }
    for (db_idx = DB0; db_idx < db_count; db_idx++) {
        /* Start from Level2 */
        for (level = LEVEL2; level < db_max_levels[db_idx]; level++) {
            int i;
            bkt_hdl_t *bkt_hdl = NULL;
            int alloc_sz;
            uint8_t max_formats;
            rbkt_t *rbkt;

            ALPM_ALLOC(bkt_hdl, sizeof(bkt_hdl_t), "bcmptmRmalpmV0bkthdl");
            sal_memset(bkt_hdl, 0, sizeof(bkt_hdl_t));
            BKT_HDL(u, m, db_idx, level) = bkt_hdl;

            /* Bucket info */
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_uft_banks(u, m, db_idx, level,
                                         &bkt_hdl->max_banks,
                                         NULL,
                                         &bkt_hdl->bucket_bits,
                                         &bkt_hdl->bank_bitmap));
            bkt_hdl->rbkts_total = bkt_hdl->max_banks * (1 << bkt_hdl->bucket_bits);
            bkt_hdl->groups_used = 1;
            bkt_hdl->rbkts_used = 1; /* Reserved group */
            bkt_hdl->format_type = (level == (db_max_levels[db_idx] - 1)) ?
                                    FORMAT_ROUTE : FORMAT_PIVOT;

            SHR_DQ_INIT(&bkt_hdl->global_group_list);
            alloc_sz = (bucket_scale[u][m] + 1) * sizeof(usage_list_t);
            ALPM_ASSERT(bkt_hdl->group_usage == NULL);
            ALPM_ALLOC(bkt_hdl->group_usage, alloc_sz, "bcmptmRmalpmV0grpusg");
            sal_memset(bkt_hdl->group_usage, 0, alloc_sz);
            for (i = 0; i <= bucket_scale[u][m]; i++) {
                SHR_DQ_INIT(&bkt_hdl->group_usage[i].groups);
            }
            bkt_hdl->group_usage_count = (bucket_scale[u][m] + 1);

            max_formats = VER0_MAX_FORMAT(u, m, FORMAT_ROUTE);
            ALPM_ASSERT(bkt_hdl->fmt_group_usage == NULL);
            alloc_sz = (max_formats + 1) * sizeof(usage_list_t *);
            ALPM_ALLOC(bkt_hdl->fmt_group_usage, alloc_sz, "bcmptmRmalpmV0fmtusg");
            sal_memset(bkt_hdl->fmt_group_usage, 0, alloc_sz);

            for (fmt = VER0_FORMAT_1; fmt <= max_formats; fmt++) {
                max_count = ver0_base_entry_count(u, m, db_idx, level, fmt);
                alloc_sz = (max_count + 1) * sizeof(usage_list_t);
                ALPM_ASSERT(bkt_hdl->fmt_group_usage[fmt] == NULL);
                ALPM_ALLOC(bkt_hdl->fmt_group_usage[fmt], alloc_sz, "bcmptmRmalpmV0fmtusg1");
                sal_memset(bkt_hdl->fmt_group_usage[fmt], 0, alloc_sz);
                for (i = 0; i <= max_count; i++) {
                    SHR_DQ_INIT(&bkt_hdl->fmt_group_usage[fmt][i].groups);
                }
            }

            alloc_sz = bkt_hdl->rbkts_total * sizeof(rbkt_t);
            ALPM_ASSERT(bkt_hdl->rbkt_array == NULL);
            ALPM_ALLOC(bkt_hdl->rbkt_array, alloc_sz,
                       "bcmptmRmalpmV0bktarray");
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
                SHR_DQ_INSERT_HEAD(&bkt_hdl->group_usage[bucket_scale[u][m]].groups,
                                   &rbkt->usgnode);
                for (fmt = VER0_FORMAT_1; fmt <= max_formats; fmt++) {
                    max_count = ver0_base_entry_count(u, m, db_idx, level, fmt);
                    SHR_DQ_INSERT_HEAD(&bkt_hdl->fmt_group_usage[fmt][max_count].groups,
                                       &rbkt->fmt_usgnode);
                }
            }

            for (i = 0; i < bkt_hdl->rbkts_total; i++) {
                rbkt_t *bbt = &bkt_hdl->rbkt_array[i];
                bbt->index = i;
            }
        }
    }

    ALPM_LMM_INIT(base_entries_ver0_t,
                  base_entries_lmem[u][m],
                  BASE_ENTRIES_ALLOC_CHUNK_SIZE,
                  false,
                  rv);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    ALPM_LMM_INIT(base_buckets_ver0_t,
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
ver0_bkt_hdl_cleanup(int u, int m)
{
    int i, j;
    int db_count;
    bkt_hdl_t *bkt_hdl;
    uint8_t fmt;

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
                if (bkt_hdl->fmt_group_usage) {
                    for (fmt = VER0_FORMAT_1; fmt <= VER0_MAX_FORMAT(u, m, FORMAT_ROUTE);
                         fmt++) {
                        SHR_FREE(bkt_hdl->fmt_group_usage[fmt]);
                    }
                    SHR_FREE(bkt_hdl->fmt_group_usage);
                }
                SHR_FREE(bkt_hdl->rbkt_array);
                SHR_FREE(bkt_hdl);
                BKT_HDL(u, m, i, j) = NULL;
            }
        }
    }
}


static void
ver0_bucket_sanity(int u, int m, int db, int ln, log_bkt_t log_bkt)
{
    VER0_RBKT_GROUP_SANITY(u, m, db, ln, VER0_GROUP(u, m, log_bkt));
}

static int
ver0_bucket_table_dump(int u, int m, int ln, int lvl_cnt, alpm_arg_t *arg, int *count)
{
    
    return SHR_E_NONE;
}
/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_alpm_bucket_ver0_register(int u, bucket_mgmt_t *bmgmt)
{
    uint8_t banks;
    int m = ALPM_0;
    SHR_FUNC_ENTER(u);

    if (bmgmt) {
        bmgmt->bkt_hdl_cleanup = ver0_bkt_hdl_cleanup;
        bmgmt->bkt_hdl_init = ver0_bkt_hdl_init;
        bmgmt->bkt_hdl_get = ver0_bkt_hdl_get;
        bmgmt->alpm_data_encode = ver0_alpm_data_encode;
        bmgmt->bucket_pivot_get = ver0_log_bucket_pivot_get;
        bmgmt->bucket_pivot_set = ver0_log_bucket_pivot_set;
        bmgmt->bucket_free = ver0_log_bucket_free;
        bmgmt->bucket_alloc = ver0_log_bucket_alloc;
        bmgmt->bucket_delete = ver0_bucket_delete;
        bmgmt->bucket_insert = ver0_bucket_insert;
        bmgmt->bucket_split_insert = ver0_bucket_split_insert;
        bmgmt->entry_write = ver0_base_entry_encode_write;
        bmgmt->entry_read = ver0_base_entry_decode_read;
        bmgmt->entry_hw_idx_get = ver0_hw_idx_from_ent_idx;
        bmgmt->bucket_sanity = ver0_bucket_sanity;
        bmgmt->bucket_recover = ver0_bucket_recover;
        bmgmt->bucket_recover_done = ver0_bucket_recover_done;
        bmgmt->bucket_resource_check = ver0_bucket_resource_check;
        bmgmt->bucket_table_dump = ver0_bucket_table_dump;

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_uft_banks(u, m, DBS, LEVEL2, &banks, NULL, NULL, NULL));
        switch (banks) {
        case 2: ver0_bank_bits[u][m] = 1; break;
        case 4: ver0_bank_bits[u][m] = 2; break;
        case 8: ver0_bank_bits[u][m] = 3; break;
        default: SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}
