/*! \file shr_itbm.c
 *
 * Indexed table block management library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_itbm.h>
#include <shr/shr_pb.h>
#include <shr/shr_bitop.h>

/*******************************************************************************
 * Local definitions
 */
/*! Combined ITBM layer/source for BSL logging. */
#define BSL_LOG_MODULE BSL_LS_SHR_ITBM

/*! Constant to indicate unknown unit number. */
#define SHR_ITBM_UNIT_UNKNOWN (-1)

/*! Maximum number of switch chips supported. */
#define SHR_ITBM_MAX_UNITS (8)

/*! Indexed buckets list default bucket base index value. */
#define SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX (0)

/*!
 * \brief List element property constant definitions.
 */
typedef enum shr_itbm_list_elem_prop_e {
    /*! First element of a block allocated from a \c block_stat enabled list. */
    SHR_ITBM_LIST_BLK_FIRST_ELEM    = 0,
    /*! Must be last one. */
    SHR_ITBM_LIST_ELEM_PROP_COUNT
} shr_itbm_list_elem_prop_t;

/*!
 * \brief List property constant definitions.
 */
typedef enum shr_itbm_list_prop_e {
    /*! Regular indexed table list type. */
    SHR_ITBM_LIST_REGULAR       = 0,
    /*! Shared indexed table list type. */
    SHR_ITBM_LIST_SHARED        = 1,
    /*! Index partitioned list type. */
    SHR_ITBM_LIST_PART          = 2,
    /*!
     * The list has non-zero mininum element index value. An example is a
     * list mapped to the lower half of an index partitioned hardware table,
     * the \c first element of this list will have a non-zero index value.
     */
    SHR_ITBM_LIST_MIN_NONZERO   = 3,
    /*! Indexed buckets style list. */
    SHR_ITBM_LIST_BUCKETS       = 4,
    /*! Must be last one. */
    SHR_ITBM_LIST_PROP_COUNT
} shr_itbm_list_prop_t;

/*!
 * \brief List property types.
 *
 * These property types must match the corresponding codes in
 * \ref shr_itbm_list_prop_t
 */
static const char *lprop_msg[] = {
    "Regular",          /* SHR_ITBM_LIST_REGULAR */
    "Shared",           /* SHR_ITBM_LIST_SHARED */
    "Partitioned",      /* SHR_ITBM_LIST_PART */
    "Min non-zero",     /* SHR_ITBM_LIST_MIN_NONZERO */
    "Indexed buckets",  /* SHR_ITBM_LIST_BUCKETS */
    "Count"             /* SHR_ITBM_LIST_PROP_COUNT */
};

/*!
 * \brief Constat definitions for the results of two values comparison.
 */
typedef enum shr_itbm_blk_base_cmp_e {
    /*! First value is lesser than the second value. */
    SHR_ITBM_BLK_BASE_LOWER     = -1,
    /*! Compared values are equal. */
    SHR_ITBM_BLK_BASE_EQUAL     = 0,
    /*! First value is greater than the second value. */
    SHR_ITBM_BLK_BASE_HIGHER    = 1,
    /*! Must be last one. */
    SHR_ITBM_CMP_COUNT = 3
} shr_itbm_blk_base_cmp_t;

/*!
 * \brief Number of entries per bucket constant definitions.
 */
typedef enum shr_itbm_list_bkt_entry_count_e {
    /*! Default value for regular (non-buckets) indexed lists. */
    SHR_ITBM_LIST_BKT_NONE          = 0,
    /*! One entry per bucket. */
    SHR_ITBM_LIST_BKT_ONE_ENTRY     = 1,
    /*! Two entries per bucket. */
    SHR_ITBM_LIST_BKT_TWO_ENTRY     = 2,
    /*! Four entries per bucket. */
    SHR_ITBM_LIST_BKT_FOUR_ENTRY    = 4
} shr_itbm_list_bkt_entry_count_t;

/*!
 * \brief List identifier key fields structure type definition.
 */
typedef struct shr_itbm_list_key_s {
    /*! Unit number. */
    int unit;
    /*! Minimum entry number in this indexed table. */
    uint32_t min;
    /*! Maximum entry number in this indexed table. */
    uint32_t max;
    /*! Device resource database symbol ID. */
    uint32_t table_id;
} shr_itbm_list_key_t;

/*!
 * \brief List element data fields structure type definition.
 */
typedef struct shr_itbm_list_edata_s {
    /* Maximum number of entries supported by this list array element. */
    uint8_t max_entries;
    /*!
     * Element or bucket reference count, non-zero value indicates it is
     * in-use.
     */
    uint8_t eref_cnt;
    /*! List element property flags bitmap. */
    SHR_BITDCL *eprop;
    /*!
     * Applicable to indexed buckets type lists, bitmap of the bucket local
     * entries.
     */
    SHR_BITDCL *ebmp;
    /*! Component/s using this element/bucket. */
    uint32_t *comp_id;
    /*!
     * If this element is the first element of an allocated block or if this
     * bucket is the base bucket that contains the first element of an allocated
     * block, then the size of block (contiguours entries) is stored with the
     * first element of the block, to speed up list block management
     * operations.
     */
    uint32_t *blk_sz;
} shr_itbm_list_edata_t;

/*!
 * \brief List attributes structure type definition.
 */
typedef struct shr_itbm_lattr_s {
    /*! Attribute ID. */
    uint32_t aid;
    /*! Total elements in this list. */
    uint32_t tot_elems;
    /*! Indexed table parameters. */
    shr_itbm_list_params_t params;
    /*! List statistics. */
    shr_itbm_list_stat_t stat;
    /*! List statistics backup. */
    shr_itbm_list_stat_t stat_bk;
    /*! Total allocations for this list count. */
    uint32_t num_allocs;
    /*! Pointer to the next list attributes node. */
    struct shr_itbm_lattr_s *next;
} shr_itbm_lattr_t;

/*!
 * \brief Block move source and destination location details.
 */
typedef struct shr_itbm_blk_move_info_s {
    /*! Move source block bucket number. */
    uint32_t sbucket;
    /*! Move source block first element number. */
    uint32_t sfirst_elem;
    /*! Move destination block bucket number. */
    uint32_t dbucket;
    /*! Move destination block first element number. */
    uint32_t dfirst_elem;
    /*!
     * Pointer to next move location of this block, if multiple moves are
     * required for this block during defragmentation.
     */
    struct shr_itbm_blk_move_info_s *next;
} shr_itbm_blk_move_info_t;

/*!
 * \brief Block defragmentation sequence.
 */
typedef struct shr_itbm_defrag_seq_s {
    /*! Total moves required for this block. */
    uint32_t mcount;
    /*! Count of elements in the block being moved. */
    uint32_t ecount;
    /*! Pointer to the block move sequence linked-list. */
    shr_itbm_blk_move_info_t *minfo;
} shr_itbm_defrag_seq_t;

/*!
 * \brief List information structure type definition.
 */
typedef struct shr_itbm_list_info_s {
    /*! Lock for the control structure. */
    sal_mutex_t llock;
    /*! List property flags bitmap. */
    SHR_BITDCLNAME(lprop, SHR_ITBM_LIST_PROP_COUNT);
    /*! List ID. */
    uint16_t list_id;
    /*! List reference count. */
    uint8_t lref_cnt;
    /*! The key field combination makes a list unique. */
    shr_itbm_list_key_t key;
    /*!
     * Maximum entries per-bucket supported by this list for indexed buckets
     * type lists.
     */
    uint32_t max_ent_per_bkt;
    /*!
     * Total count of entries or buckets in the list (for indexed buckets type
     * lists).
     */
    uint32_t tot_ecount;
    /*!
     * Pointer to the first element or bucket of the list element/bucket
     * data array.
     */
    shr_itbm_list_edata_t *edata;
    /*! Count of attributes linked to this list. */
    uint32_t acount;
    /*! Pointer to the head of the list attributes linked list. */
    shr_itbm_lattr_t *alist;
    /*! Number of elements/buckets in use. */
    uint32_t inuse_ecount;
    /*! Number of free elements/buckets. */
    uint32_t free_ecount;
    /*! Total number of blocks alloced from this list. */
    uint32_t tot_blks_alloced;
    /*! List defragmentation in-progress status.  */
    bool defrag_inprog;
    /*!
     * Pointer to the list handle of the component that has initiated
     * the defragmentation operation on the list.
     */
    shr_itbm_list_hdl_t defrag_hdl;
    /*! Pointer to list blocks defragmentation sequence. */
    shr_itbm_defrag_seq_t **dseq;
    /*!
     * Pointer to backup copy of the list elements data array created
     * during defrag.
     */
    shr_itbm_list_edata_t *edata_bk;
    /*!
     * Total number of blocks moved during list defragmentation operation.
     * This value defines the size of \c dblk_cur_loc and \c dblk_mseq
     * arrays, members of this structure.
     */
    uint32_t dblks_moved;
    /*!
     * Pointer to the block current location array for the list of blocks
     * moved during defragmentation.
     */
    shr_itbm_blk_t *dblk_cur_loc;
    /*!
     * Pointer to the list blocks moved during defragmentation and the move
     * sequence.
     */
    shr_itbm_defrag_blk_mseq_t *dblk_mseq;
    /*! Defragmented list free blocks array pointer. */
    shr_itbm_blk_t *dfgl_fblk_arr;
    /*! Pointer to the next list. */
    struct shr_itbm_list_info_s *next;
} shr_itbm_list_info_t;

/*!
 * \brief ITBM main list control structure type definition.
 */
typedef struct shr_itbm_ctrl_s {
    /*! Global lock for this control structure instance. */
    sal_mutex_t glock;
    /*!
     * Number of active lists count, this is also the index of next
     * available free \c list index.
     */
    uint32_t lcount;
    /*! Pointer to the head node of the indexed lists linked-list. */
    shr_itbm_list_info_t *ilist;
} shr_itbm_ctrl_t;

/*! Pointer to the ITBM list control structure.  */
static shr_itbm_ctrl_t *ctrl[SHR_ITBM_MAX_UNITS] = {0};

/*!
 * \brief Indexed table list structure type definition.
 */
typedef struct shr_itbm_list_s {
    /*! Pointer to the list information structure. */
    shr_itbm_list_info_t *linfo;
    /*! Indexed table parameter details. */
    shr_itbm_list_params_t params;
    /*! List attribute ID. */
    uint32_t aid;
} shr_itbm_list_t;

/*!
 * \brief List supported operation type constant definitions.
 */
typedef enum shr_itbm_list_oper_e {
    /*! List element/elements alloc. */
    SHR_ITBM_LIST_OPER_ALLOC    = 0,
    /*! List element/elements alloc with ID. */
    SHR_ITBM_LIST_OPER_ALLOC_ID = 1,
    /*! List element/elements free. */
    SHR_ITBM_LIST_OPER_FREE     = 2,
    /*! List block resize operation. */
    SHR_ITBM_LIST_OPER_RESIZE   = 3,
    /*! Must be last one. */
    SHR_ITBM_LIST_OPER_COUNT
} shr_itbm_list_oper_t;

/*!
 * \brief List operation structure type definition.
 */
typedef struct shr_itbm_list_op_s {
    /*! Pointer to list information structure. */
    shr_itbm_list_info_t *linfo;
    /*! List attributes.  */
    shr_itbm_lattr_t *lattr;
    /*!
     * For partitioned indexed tables, this offset value must be subtracted
     * from input index value to determine the zero based index value for
     * indexing into \c edata array.
     */
    uint32_t offset;
    /*! List operation type. */
    SHR_BITDCLNAME(loper, SHR_ITBM_LIST_OPER_COUNT);
    /*! Number of elements to be allocated. */
    uint32_t num_elem;
    /*! List parameters. */
    shr_itbm_list_params_t params;
    /*! Bucket that contains the first element of the allocated block. */
    uint32_t base_bkt;
    /*! Block first element index value. */
    uint32_t base_idx;
    /*! Bucket that contains the last element of the allocated block. */
    uint32_t end_bkt;
    /*! Bucket local index value of the last element of the block. */
    uint32_t end_idx;
    /*! Number of buckets occupied by this block. */
    uint32_t num_bkt;
} shr_itbm_list_op_t;

/*******************************************************************************
 * Private functions
 */
/*! Get number of elements in the bucket. */
static inline uint32_t
shr_itbm_list_blk_num_bkts_get(uint32_t blk_sz,
                               uint32_t bkt_offset,
                               uint32_t ent_per_bkt,
                               bool reverse)
{
    uint32_t adj_elem_cnt = 0;

    if (blk_sz == 0 || bkt_offset == SHR_ITBM_INVALID
        || ent_per_bkt == 0) {
        return 0;
    }
    /*
     * Direction of alloc is top to bottom:
     * If the input base bucket free element offset index is non-zero, then
     * it's possible that the base bucket is used by some other block/s. So,
     * from the blk_sz elements, some elements can be accomodated in this
     * base bucket.
     *
     * Determine number of element/s from blk_sz that can be accomodated in
     * the base bucket and adjust these elements count while computing the
     * total number of buckets required to allocate this block.
     */
    if (reverse == 0 && blk_sz > (ent_per_bkt - bkt_offset)) {
        adj_elem_cnt = bkt_offset ? (ent_per_bkt - bkt_offset) : 0;
    }

    if (reverse == 1 && blk_sz > (bkt_offset + 1)) {
        adj_elem_cnt = (bkt_offset == ent_per_bkt - 1) ? 0 : (bkt_offset + 1);
    }
    /*
     * Compute the number of buckets required to fit the input block size
     * elements using buckets with ent_per_bkt elements capacity.
     */
    return (((blk_sz - adj_elem_cnt) / ent_per_bkt) +
                (((blk_sz - adj_elem_cnt) % ent_per_bkt) ? 1 : 0) +
                ((adj_elem_cnt) ? 1 : 0));
}

static inline bool
shr_itbm_list_key_match(shr_itbm_list_key_t key,
                        shr_itbm_list_params_t params)
{
    return ((key.unit == params.unit
             && key.table_id == params.table_id
             && key.min == params.min
             && key.max == params.max) ? 1 : 0);
}

static inline bool
shr_itbm_list_min_max_overlap(shr_itbm_list_key_t key,
                              shr_itbm_list_params_t params)
{
    return ((key.unit == params.unit
             && key.table_id == params.table_id
             && ((key.min >= params.min && key.min <= params.max)
                  || (key.max >= params.min && key.max <= params.max)
                  || (params.min >= key.min && params.min <= key.max)
                  || (params.max >= key.min && params.max <= key.max)))
            ? 1 : 0);
}

static inline bool
itbm_ibkt_list_first_last_match(shr_itbm_list_params_t lparams,
                                shr_itbm_list_params_t iparams)
{
    return ((lparams.first == iparams.first && lparams.last == iparams.last)
             ? 1 : 0);
}

static inline bool
itbm_ibkt_list_first_last_overlap(shr_itbm_list_params_t lparams,
                                  shr_itbm_list_params_t iparams)
{
    return ((iparams.first >= lparams.first && iparams.first <= lparams.last)
             || (iparams.last >= lparams.first && iparams.last <= lparams.last)
             || (lparams.first >= iparams.first && lparams.first
                  <= iparams.last)
             || (lparams.last >= iparams.first && lparams.last <= iparams.last)
             ? 1 : 0);
}

static inline bool
shr_itbm_list_params_match(shr_itbm_list_params_t lparams,
                           shr_itbm_list_params_t iparams)
{
    return ((lparams.unit == iparams.unit
             && lparams.table_id == iparams.table_id
             && lparams.min == iparams.min
             && lparams.max == iparams.max
             && lparams.first == iparams.first
             && lparams.last == iparams.last
             && lparams.comp_id == iparams.comp_id
             && lparams.buckets == iparams.buckets
             && lparams.entries_per_bucket == iparams.entries_per_bucket)
            ? 1 : 0);
}

static inline void
shritbm_bmp_msg(shr_pb_t *pb,
                SHR_BITDCL *bmp,
                uint32_t size,
                const char *msg[])
{
    uint32_t b = 0;
    if (pb != NULL && bmp != NULL && msg != NULL) {
        SHR_BIT_ITER(bmp, size, b) {
            shr_pb_printf(pb, "\"%s\":", msg[b]);
        }
    }
    return;
}

static inline uint8_t
shr_itbm_list_elem_ref_cnt_get(shr_itbm_list_edata_t *edata,
                               uint32_t idx)
{
    return (edata[idx].eref_cnt);
}

static bool
shr_itbm_list_range_elem_in_use_check(shr_itbm_list_hdl_t hdl,
                                      uint32_t start,
                                      uint32_t end)
{
    uint32_t idx = SHR_ITBM_INVALID;
    uint32_t f = start, l = end;

    if (SHR_BITGET(hdl->linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        f = start - hdl->params.min;
        l = end - hdl->params.min;
    }
    for (idx = f; idx <= l; idx++) {
        if (shr_itbm_list_elem_ref_cnt_get(hdl->linfo->edata, idx)) {
            return 1;
        }
    }
    return 0;
}

static int
shr_itbm_rev_bkt_list_blk_base_get(int unit,
                                   uint32_t end_bkt,
                                   uint32_t end_idx,
                                   uint32_t ecount,
                                   uint8_t epbkt,
                                   uint32_t *base_bkt,
                                   uint32_t *base_idx)
{
    uint32_t ebkt = end_bkt;
    uint8_t eidx = end_idx;
    uint32_t c = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(base_bkt, SHR_E_INTERNAL);
    SHR_NULL_CHECK(base_idx, SHR_E_INTERNAL);

    for (c = 1; c < ecount; c++) {
        if (eidx > SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX) {
            eidx--;
        }
        if (eidx == SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX) {
            ebkt -= 1;
            eidx = epbkt - 1;
        }
    }
    *base_bkt = ebkt;
    *base_idx = eidx;
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "bb=%-5d bbi=%-1u eb=%-5d ef=%-5u ecnt=%u "
                              "epbkt=%-u\n"),
                   (int)*base_bkt,
                   *base_idx,
                   (int)end_bkt,
                   end_idx,
                   ecount,
                   epbkt));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_blk_end_get(int unit,
                              uint32_t bucket,
                              uint32_t first,
                              uint32_t ecount,
                              uint8_t epbkt,
                              uint32_t *end_bkt,
                              uint8_t *end_idx)
{
    uint32_t ebkt = bucket;
    uint8_t eidx;
    uint32_t c = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(end_bkt, SHR_E_INTERNAL);
    SHR_NULL_CHECK(end_idx, SHR_E_INTERNAL);

    for (c = 0, eidx = first; c < ecount; c++, eidx++) {
        if (ecount > epbkt && eidx == epbkt) {
            ebkt++;
            /*
             * Bucket capacity met, set element index to base element index in
             * the next bucket.
             */
            eidx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
        }
    }
    *end_bkt = ebkt;
    *end_idx = (eidx - 1);
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_invt_bkt_list_blks_gap_get(int unit,
                                    const shr_itbm_blk_t *ublk,
                                    shr_itbm_blk_t *gblk)
{
    uint32_t b = 0, f = 0, e = 0;
    uint32_t ecount = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ublk, SHR_E_INTERNAL);
    SHR_NULL_CHECK(gblk, SHR_E_INTERNAL);

    /*
     * As this function is computing the upper block size, ublk
     * entries_per_bucket value is used for determining total entries count.
     */
    if (gblk && ublk) {
        f = ublk->first_elem;
        if (ublk->entries_per_bucket == gblk->entries_per_bucket) {
            /*
             * When ublk and gblk entries_per_bucket count values are same,
             * account of any free elements in the gblk bucket located before
             * the first_elem index.
             */
            for (b = ublk->bucket; b <= gblk->bucket; b++) {
                for (e = f; e < ublk->entries_per_bucket; e++) {
                    if (b == gblk->bucket && e == gblk->first_elem) {
                        ecount++;
                        break;
                    }
                    ecount++;
                }
                f = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
            }
        }
        /*
         * When ublk and gblk entries_per_bucket count values do not
         * match, skip any free entries in gblk bucket located prior
         * to the first_elem index.
         */
        if (ublk->entries_per_bucket != gblk->entries_per_bucket) {
            for (b = ublk->bucket; b <= gblk->bucket; b++) {
                for (e = f; e < ublk->entries_per_bucket; e++) {
                    ecount++;
                }
                f = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
            }
        }
        gblk->ecount = (ecount - ublk->ecount);

        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && ecount != 0) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "ublk(epbkt=%-1u):b=%-5u f=%-5u ecnt=%-5u"
                                  "gblk(epbkt=%-1u):eb=%-5u eidx=%-5u - "
                                  "gblk.ecnt=%-5u.\n"),
                       ublk->entries_per_bucket,
                       ublk->bucket,
                       ublk->first_elem,
                       ublk->ecount,
                       gblk->entries_per_bucket,
                       gblk->bucket,
                       gblk->first_elem,
                       gblk->ecount));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_ublk_size_get(int unit,
                                shr_itbm_blk_t *ublk,
                                const shr_itbm_blk_t *lblk)
{
    uint32_t b = 0, f = 0, e = 0;
    uint32_t ecount = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ublk, SHR_E_INTERNAL);
    SHR_NULL_CHECK(lblk, SHR_E_INTERNAL);
    /*
     * As this function is computing the upper block size, ublk
     * entries_per_bucket value is used for determining total entries count.
     */
    if (ublk && lblk) {
        f = ublk->first_elem;
        if (ublk->entries_per_bucket == lblk->entries_per_bucket) {
            /*
             * When ublk and lblk entries_per_bucket count values are same,
             * account of any free elements in the lblk bucket located before
             * the first_elem index.
             */
            for (b = ublk->bucket; b <= lblk->bucket; b++) {
                for (e = f; e < ublk->entries_per_bucket; e++) {
                    if (b == lblk->bucket && e == lblk->first_elem) {
                        break;
                    }
                    ecount++;
                }
                f = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
            }
        }
        /*
         * When ublk and lblk entries_per_bucket count values do not
         * match, skip any free entries in lblk bucket located prior
         * to the first_elem index.
         */
        if (ublk->entries_per_bucket != lblk->entries_per_bucket) {
            for (b = ublk->bucket; b < lblk->bucket; b++) {
                for (e = f; e < ublk->entries_per_bucket; e++) {
                    ecount++;
                }
                f = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
            }
        }
        ublk->ecount = ecount;
    }
exit:
    SHR_FUNC_EXIT();
}

static void
shr_itbm_list_info_t_init(shr_itbm_list_info_t *linfo)
{
    if (linfo) {
        sal_memset(linfo, 0, sizeof(*linfo));
    }
    return;
}

static void
shr_itbm_lattr_t_init(shr_itbm_lattr_t *attr)
{
    if (attr) {
        sal_memset(attr, 0, sizeof(*attr));
    }
    return;
}

static void
shr_itbm_list_op_t_init(shr_itbm_list_op_t *op)
{
    if (op) {
        sal_memset(op, 0, sizeof(*op));
        op->lattr = NULL;
        op->base_bkt = SHR_ITBM_INVALID;
        op->base_idx = SHR_ITBM_INVALID;
        op->end_bkt = SHR_ITBM_INVALID;
        op->end_idx = SHR_ITBM_INVALID;
    }
    return;
}

static int
shr_itbm_ctrl_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /*
     * If the control structure is already initialized for this unit, then
     * there is nothing to do, so exit this function.
     */
    if (ctrl[unit]) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Ctrl[u=%-2u] init already done - rv=%s.\n"),
                     unit,
                     shr_errmsg(SHR_E_NONE)));
        SHR_EXIT();
    }

    /* Allocate the control structure memory. */
    SHR_ALLOC(ctrl[unit], sizeof(*ctrl[unit]), "shritbmUnitCtrlInit");
    SHR_NULL_CHECK(ctrl[unit], SHR_E_MEMORY);
    sal_memset(ctrl[unit], 0, sizeof(*ctrl[unit]));

    /* Create the per-unit ITBM control structure synchronisation mutex lock. */
    ctrl[unit]->glock = sal_mutex_create("shritbmUnitCtrlMutex");
    if (!(ctrl[unit]->glock)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "UnitCtrlMutexLock create failed.\n")));
        SHR_FREE(ctrl[unit]);
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: [Ctrl init done - (lcount=%u)].\n"),
                 ctrl[unit]->lcount));
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_blk_bkt_cmp_descend(const void *a,
                             const void *b)
{
    shr_itbm_blk_t *f; /* First block. */
    shr_itbm_blk_t *s; /* Second value. */

    f = (shr_itbm_blk_t *)a;
    s = (shr_itbm_blk_t *)b;

    if (f != NULL && s != NULL) {
        if (f->bucket > s->bucket) {
            return (SHR_ITBM_BLK_BASE_LOWER);
        } else if (f->bucket < s->bucket) {
            return (SHR_ITBM_BLK_BASE_HIGHER);
        } else if (f->bucket == s->bucket
                   && f->first_elem > s->first_elem) {
            return (SHR_ITBM_BLK_BASE_LOWER);
        } else if (f->bucket == s->bucket
                   && f->first_elem < s->first_elem) {
            return (SHR_ITBM_BLK_BASE_HIGHER);
        }
    }
    return (SHR_ITBM_BLK_BASE_EQUAL);
}

static int
shr_itbm_blk_bkt_cmp(const void *a,
                     const void *b)
{
    shr_itbm_blk_t *f; /* First block. */
    shr_itbm_blk_t *s; /* Second value. */

    f = (shr_itbm_blk_t *)a;
    s = (shr_itbm_blk_t *)b;

    if (f != NULL && s != NULL) {
        if (f->bucket < s->bucket) {
            return (SHR_ITBM_BLK_BASE_LOWER);
        } else if (f->bucket > s->bucket) {
            return (SHR_ITBM_BLK_BASE_HIGHER);
        } else if (f->bucket == s->bucket
                   && f->first_elem < s->first_elem) {
            return (SHR_ITBM_BLK_BASE_LOWER);
        } else if (f->bucket == s->bucket
                   && f->first_elem > s->first_elem) {
            return (SHR_ITBM_BLK_BASE_HIGHER);
        }
    }
    return (SHR_ITBM_BLK_BASE_EQUAL);
}

static int
shr_itbm_blk_base_cmp(const void *a,
                      const void *b)
{
    shr_itbm_blk_t *fblk; /* First block. */
    shr_itbm_blk_t *sblk; /* Second value. */

    fblk = (shr_itbm_blk_t *)a;
    sblk = (shr_itbm_blk_t *)b;

    if (fblk != NULL && sblk != NULL) {
        if (fblk->first_elem < sblk->first_elem) {
            return (SHR_ITBM_BLK_BASE_LOWER);
        } else if (fblk->first_elem > sblk->first_elem) {
            return (SHR_ITBM_BLK_BASE_HIGHER);
        }
    }
    return (SHR_ITBM_BLK_BASE_EQUAL);
}

static int
shr_itbm_ctrl_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /*
     * If the unit ITBM control structure is already null, then there is
     * nothing to be done, exit the function.
     */
    if (!ctrl[unit]) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ctrl[u=%-2u] cleanup already done - rv=%s.\n"),
                     unit,
                     shr_errmsg(SHR_E_NONE)));
        SHR_EXIT();
    }

    /*
     * If there are still valid lists, then return error.
     * All the lists must be destroyed before performing the cleanup.
     */
    if (ctrl[unit]->lcount || ctrl[unit]->ilist) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "ctrl[u=%u]: (lcnt=%u) list/s in "
                                        "use - rv=%s.\n"),
                             unit,
                             ctrl[unit]->lcount,
                             shr_errmsg(SHR_E_BUSY)));
    }

    /* Destroy the lock. */
    if (ctrl[unit]->glock) {
        sal_mutex_destroy(ctrl[unit]->glock);
        ctrl[unit]->glock = NULL;
    }
    sal_free(ctrl[unit]);
    ctrl[unit] = NULL;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: [unit=%d ctrl cleanup].\n"),
                 unit));
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_attr_stat_get(int unit,
                            shr_itbm_list_hdl_t hdl,
                            shr_itbm_list_stat_t *stat)
{
    shr_itbm_lattr_t *alist = NULL; /* Pointer to list attributes node. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stat, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->alist, SHR_E_PARAM);

    alist = hdl->linfo->alist;
    while (alist && alist->aid != hdl->aid) {
        alist = alist->next;
    }
    if (alist == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        /* Clear the stats prior to current stats assignment. */
        shr_itbm_list_stat_t_init(stat);
        stat->in_use_count = alist->stat.in_use_count;
        stat->free_count = alist->stat.free_count;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_edata_elements_realloc(int unit,
                                     shr_itbm_list_params_t params,
                                     char *name,
                                     shr_itbm_list_info_t *linfo)
{
    uint32_t i = SHR_ITBM_INVALID; /* Element index. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(linfo->edata, SHR_E_PARAM);

    for (i = 0; i < linfo->tot_ecount; i++) {
        /*
         * Reallocate entries bitmap based on the new list max_ent_per_bkt
         * value.
         */
        SHR_FREE(linfo->edata[i].ebmp);
        SHR_ALLOC(linfo->edata[i].ebmp,
                  SHR_BITALLOCSIZE(params.entries_per_bucket), name);
        SHR_NULL_CHECK(linfo->edata[i].ebmp, SHR_E_MEMORY);
        sal_memset(linfo->edata[i].ebmp, 0,
                   SHR_BITALLOCSIZE(params.entries_per_bucket));
    }
    linfo->max_ent_per_bkt = params.entries_per_bucket;

exit:
    if (SHR_FUNC_ERR() && linfo != NULL && linfo->edata != NULL) {
        for (i = 0; i < linfo->tot_ecount; i++) {
            SHR_FREE(linfo->edata[i].ebmp);
        }
    }
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_alloc(int unit,
                    shr_itbm_list_params_t params,
                    char *name,
                    shr_itbm_list_info_t **linfo)
{
    uint32_t alloc_sz = 0; /* Memory alloc size in bytes. */
    uint32_t i = SHR_ITBM_INVALID; /* Element index. */
    shr_itbm_list_info_t *nlist = NULL; /* Pointer to list node. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    if  (!name || !linfo) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "No \"name\" or \"linfo\" - rv=%s.\n"),
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Initialize the out parameter value to NULL. */
    *linfo = NULL;
    /* Allocate list memory. */
    SHR_ALLOC(nlist, sizeof(*nlist), name);
    SHR_NULL_CHECK(nlist, SHR_E_MEMORY);
    sal_memset(nlist, 0, sizeof(*nlist));

    shr_itbm_list_info_t_init(nlist);
    /* Create the list synchronisation mutex lock. */
    nlist->llock = sal_mutex_create("shritbmListLock");
    if (!(nlist->llock)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "shritbmListLock create failed.\n")));
        SHR_FREE(nlist);
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /*
     * Initialize total elements/buckets and free elements/buckets count of
     * the list. For indexed buckets style list, each list element becomes a
     * bucket and supports entries_per_bucket count of local elements/entries
     * in it.
     */
    nlist->tot_ecount = (params.max - params.min) + 1;
    nlist->free_ecount = (params.max - params.min) + 1;

    /* Allocate memory to store list elements data. */
    alloc_sz = sizeof(*(nlist->edata)) * nlist->tot_ecount;
    SHR_ALLOC(nlist->edata, alloc_sz, name);
    SHR_NULL_CHECK(nlist->edata, SHR_E_MEMORY);
    sal_memset(nlist->edata, 0, alloc_sz);

    /* Save the list key fields values. */
    nlist->key.unit = params.unit;
    nlist->key.table_id = params.table_id;
    nlist->key.min = params.min;
    nlist->key.max = params.max;

    /*
     * If the minimum index of the list is starting at an offset and
     * not zero, then set the SHR_ITBM_LIST_MIN_NONZERO list type flag
     * bit. The list type is checked to compute the zero based index
     * for accessing edata list array elements.
     */
    if (params.min != 0) {
        SHR_BITSET(nlist->lprop, SHR_ITBM_LIST_MIN_NONZERO);
    }

    /*
     * If the list is an indexed buckets style list, then set the buckets
     * property and allocate per bucket bitarray to store per bucket entries,
     * for all the buckets of the list.
     */
    if (params.buckets) {
        /* Set indexed buckets type list property flag bit. */
        SHR_BITSET(nlist->lprop, SHR_ITBM_LIST_BUCKETS);
        /* Initialize number of list elements supported per physical entry. */
        nlist->max_ent_per_bkt = params.entries_per_bucket;
        /* Allocate memory for the list indexed buckets local elements. */
        for (i = 0; i < nlist->tot_ecount; i++) {
            /* Allocate bucket entries bitmap memory. */
            nlist->edata[i].ebmp = NULL;
            SHR_ALLOC(nlist->edata[i].ebmp,
                      SHR_BITALLOCSIZE(nlist->max_ent_per_bkt),
                      name);
            SHR_NULL_CHECK(nlist->edata[i].ebmp, SHR_E_MEMORY);
            sal_memset(nlist->edata[i].ebmp, 0,
                       SHR_BITALLOCSIZE(nlist->max_ent_per_bkt));

            nlist->edata[i].eprop = NULL;
            nlist->edata[i].blk_sz = NULL;
            nlist->edata[i].comp_id = NULL;
        }
    } else {
        /*
         * Initialize as regular list, with one list element supported per
         * physical entry.
         */
        SHR_BITSET(nlist->lprop, SHR_ITBM_LIST_REGULAR);
        nlist->max_ent_per_bkt = SHR_ITBM_LIST_BKT_ONE_ENTRY;
        for (i = 0; i < nlist->tot_ecount; i++) {
            nlist->edata[i].ebmp = NULL;
            nlist->edata[i].eprop = NULL;
            nlist->edata[i].blk_sz = NULL;
            nlist->edata[i].comp_id = NULL;
        }
    }
    /* Assign a list ID for tracking and debugging. */
    nlist->list_id = ctrl[unit]->lcount;
    nlist->lref_cnt++;
    *linfo = nlist;
    ctrl[unit]->lcount++;
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[List=%s LID=%-4u lcount=%-2u]: (u=%-2u "
                              "tid=%-5u min=%-5u max=%-5u tot_e=%-5u free=%-5u "
                              "inuse_e=%-1u max_epbkt=%-5u).\n"),
                   name,
                   nlist->list_id,
                   ctrl[unit]->lcount,
                   nlist->key.unit,
                   nlist->key.table_id,
                   nlist->key.min,
                   nlist->key.max,
                   nlist->tot_ecount,
                   nlist->free_ecount,
                   nlist->inuse_ecount,
                   nlist->max_ent_per_bkt));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (nlist) {
            for (i = 0; i < nlist->tot_ecount; i++) {
                SHR_FREE(nlist->edata[i].ebmp);
            }
            if (nlist->edata) {
                SHR_FREE(nlist->edata);
            }
            if (!(nlist->llock)) {
                sal_mutex_destroy(nlist->llock);
                nlist->llock = NULL;
            }
        }
        SHR_FREE(nlist);
    }
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_free(int unit,
                   shr_itbm_list_info_t *linfo)
{
    uint32_t i = SHR_ITBM_INVALID; /* Element index. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(linfo, SHR_E_INTERNAL);
    SHR_NULL_CHECK(linfo->edata, SHR_E_INTERNAL);

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Free list(LID=%-4u) edata(=%p) memory.\n"),
                   linfo->list_id,
                   (void *)linfo->edata));
    }

    /* Free memory allocated for storing bucket local elements bitmap. */
    for (i = 0; i < linfo->tot_ecount; i++) {
        SHR_FREE(linfo->edata[i].ebmp);
        SHR_FREE(linfo->edata[i].eprop);
        SHR_FREE(linfo->edata[i].blk_sz);
        SHR_FREE(linfo->edata[i].comp_id);
    }
    /* Free memory allocated to store list elements data. */
    SHR_FREE(linfo->edata);
    if (linfo->llock) {
        sal_mutex_destroy(linfo->llock);
        linfo->llock = NULL;
    }
    /* Free the list memory. */
    SHR_FREE(linfo);

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "List resource free - done.\n")));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_ilist_node_delete(int unit,
                           shr_itbm_list_info_t *linfo)
{
    shr_itbm_list_info_t *list = NULL; /* Ptr to the list current node. */
    shr_itbm_list_info_t *del = NULL; /* Ptr to the list current node. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(linfo, SHR_E_PARAM);

    /* Get the unit list head node pointer. */
    list = ctrl[unit]->ilist;
    SHR_NULL_CHECK(list, SHR_E_PARAM);

    /* Check if the head node must be deleted. */
    if (list == linfo) {
        del = list;
        ctrl[unit]->ilist = list->next;
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "List node del(LID=%-4u).\n"),
                       del->list_id));
        }
        SHR_VERBOSE_EXIT
            (shr_itbm_list_free(unit, del));
    }

    /* Traverse the list and find the node to be deleted. */
    while (list->next) {
        if (list->next == linfo) {
            del = list->next;
            list->next = del->next;
            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "List node del(LID=%-4u).\n"),
                           del->list_id));
            }
            SHR_VERBOSE_EXIT
                (shr_itbm_list_free(unit, del));
        }
        list = list->next;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_attr_get(int unit,
                       shr_itbm_list_info_t *linfo,
                       uint32_t aid,
                       shr_itbm_lattr_t **lattr)
{
    shr_itbm_lattr_t *attr = NULL; /* Ptr to a list attribute node. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(linfo, SHR_E_INTERNAL);
    SHR_NULL_CHECK(linfo->alist, SHR_E_INTERNAL);
    SHR_NULL_CHECK(lattr, SHR_E_INTERNAL);

    *lattr = NULL;
    attr = linfo->alist;
    while (attr && attr->aid != aid) {
        attr = attr->next;
    }
    if (attr == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        *lattr = attr;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_attr_add(int unit,
                       shr_itbm_list_params_t params,
                       char *name,
                       shr_itbm_list_info_t *linfo,
                       uint32_t *aid)
{
    shr_itbm_lattr_t *alist = NULL; /* Ptr to params linked list node. */
    shr_itbm_lattr_t *nalist = NULL; /* New list params. */
    shr_itbm_list_stat_t cur_stat;
    uint32_t tot_elems = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(name, SHR_E_INTERNAL);
    SHR_NULL_CHECK(linfo, SHR_E_INTERNAL);
    SHR_NULL_CHECK(aid, SHR_E_INTERNAL);

    alist = linfo->alist;
    if (!alist) {
        /* Very first params-attribute to be added to this list. */
        SHR_ALLOC(nalist, sizeof(*nalist), name);
        SHR_NULL_CHECK(nalist, SHR_E_MEMORY);
        sal_memset(nalist, 0, sizeof(*nalist));
        shr_itbm_lattr_t_init(nalist);
        nalist->params = params;
        nalist->aid = linfo->acount;

        /*
         * Initialize list free elements count based on the list type set
         * in input params.
         */
        if (params.buckets) {
            nalist->stat.free_count = (((params.last - params.first) + 1)
                                        * params.entries_per_bucket);
        } else {
            nalist->stat.free_count = ((params.last - params.first) + 1);
        }
        /* Initialize total elements supported by the list. */
        nalist->tot_elems = nalist->stat.free_count;

        *aid = nalist->aid;
        linfo->alist = nalist;
        linfo->acount++;
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LAttrAdd - aid=%-3u name=%s acount=%-2u "
                                  "lref_cnt=%-2u in_use=%u free=%u "
                                  "tot_elems=%-5u f=%-5u l=%-5u "
                                  "min=%-5u max=%-5u bs=%s bkts=%s "
                                  "epbkt=%-2u rev=%s.\n"),
                       nalist->aid,
                       name,
                       linfo->acount,
                       linfo->lref_cnt,
                       nalist->stat.in_use_count,
                       nalist->stat.free_count,
                       nalist->tot_elems,
                       nalist->params.first,
                       nalist->params.last,
                       nalist->params.min,
                       nalist->params.max,
                       nalist->params.block_stat ? "T" : "F",
                       nalist->params.buckets ? "T" : "F",
                       nalist->params.entries_per_bucket,
                       nalist->params.reverse ? "T" : "F"));
        }
        SHR_EXIT();
    }
    /* Append the new params-attribute to the params linked-list. */
    while (alist) {
        /*
         * Get the list current stat if the new list is a shared list with
         * same list config params.
         */
        if (alist->params.first == params.first
            && alist->params.last == params.last
            && alist->params.block_stat == params.block_stat
            && alist->params.buckets == params.buckets
            && alist->params.entries_per_bucket == params.entries_per_bucket
            && alist->params.reverse == params.reverse) {
            shr_itbm_list_stat_t_init(&cur_stat);
            cur_stat = alist->stat;
            tot_elems = alist->tot_elems;
        }
        if (!alist->next) {
            /*
             * Allocate memory to store the new input params for an
             * existing list.
             */
            SHR_ALLOC(nalist, sizeof(*nalist), name);
            SHR_NULL_CHECK(nalist, SHR_E_MEMORY);
            sal_memset(nalist, 0, sizeof(*nalist));

            shr_itbm_lattr_t_init(nalist);
            nalist->params = params;
            nalist->aid = linfo->acount;

            /* Initialize list free elements count. */
            if (params.buckets) {
                if (tot_elems) {
                    nalist->stat = cur_stat;
                    nalist->tot_elems = tot_elems;
                } else {
                    /*
                     * Shared but index partitioned case i.e. different
                     * first - last range.
                     */
                    nalist->stat.free_count = (((params.last - params.first)
                                                 + 1)
                                                 * params.entries_per_bucket);
                    /* Initialize total elements supported by the list. */
                    nalist->tot_elems = nalist->stat.free_count;
                }
            } else {
                /* Initialize the list stats attributes. */
                if (tot_elems) {
                    nalist->stat = cur_stat;
                    nalist->tot_elems = tot_elems;
                } else {
                    /*
                     * Shared but index partitioned case i.e. different
                     * first - last range.
                     */
                    nalist->stat.free_count = ((params.last - params.first)
                                                + 1);
                    nalist->tot_elems = nalist->stat.free_count;
                }
            }

            *aid = nalist->aid;
            alist->next = nalist;
            /* Increment list attributes count. */
            linfo->acount++;
            /*
             * Increment the list reference count here, as this list is
             * being shared by another component/caller now.
             */
            linfo->lref_cnt++;
            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LAttrAdd - aid=%-3u name=%s acount=%-2u "
                                      "lref_cnt=%-2u in_use=%u free=%u "
                                      "tot_elems=%-5u f=%-5u "
                                      "l=%-5u min=%-5u max=%-5u bs=%s bkts=%s "
                                      "epbkt=%-2u rev=%s.\n"),
                           nalist->aid,
                           name,
                           linfo->acount,
                           linfo->lref_cnt,
                           nalist->stat.in_use_count,
                           nalist->stat.free_count,
                           nalist->tot_elems,
                           nalist->params.first,
                           nalist->params.last,
                           nalist->params.min,
                           nalist->params.max,
                           nalist->params.block_stat ? "T" : "F",
                           nalist->params.buckets ? "T" : "F",
                           nalist->params.entries_per_bucket,
                           nalist->params.reverse ? "T" : "F"));
            }
            break;
        } else {
            alist = alist->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_ilist_node_append(int unit,
                           shr_itbm_list_info_t *linfo)
{
    shr_itbm_list_info_t *list = NULL; /* Ptr to the list current node. */

    SHR_FUNC_ENTER(unit);

    list = ctrl[unit]->ilist;
    SHR_NULL_CHECK(list, SHR_E_INTERNAL);

    while (list) {
        if (!list->next) {
            list->next = linfo;
            break;
        }
        list = list->next;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_hdl_alloc(int unit,
                   shr_itbm_list_params_t params,
                   shr_itbm_list_hdl_t *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(*hdl, sizeof(**hdl), "shritbmNewHdl");
    SHR_NULL_CHECK(*hdl, SHR_E_MEMORY);
    sal_memset(*hdl, 0, sizeof(**hdl));

    (*hdl)->params = params;
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "(U=%-1u T=%-5u Mi=%-5u Ma=%-5u F=%-5u L=%-5u "
                              "C=%-2u BS=%-1u Bkts=%-1u EntPerBkt=%-2u).\n"),
                   (*hdl)->params.unit,
                   (*hdl)->params.table_id,
                   (*hdl)->params.min,
                   (*hdl)->params.max,
                   (*hdl)->params.first,
                   (*hdl)->params.last,
                   (*hdl)->params.comp_id,
                   (*hdl)->params.block_stat,
                   (*hdl)->params.buckets,
                   (*hdl)->params.entries_per_bucket));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_match_find(int unit,
                         shr_itbm_list_params_t params,
                         bool *found,
                         shr_itbm_list_info_t **mlist)
{
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to the list. */
    shr_itbm_lattr_t *alist = NULL; /* Pointer to list params. */
    shr_itbm_list_params_t lparams; /* Current list params. */

    SHR_FUNC_ENTER(unit);

    /* Validate function input parameters. */
    SHR_NULL_CHECK(mlist, SHR_E_PARAM);
    SHR_NULL_CHECK(found, SHR_E_PARAM);

    /* Get the head node pointer of the indexed linked list. */
    linfo = ctrl[unit]->ilist;
    SHR_NULL_CHECK(linfo, SHR_E_INTERNAL);

    /*
     * Initialize the matched list pointer to null and matched params
     * index to invalid.
     */
    *mlist = NULL;

    /*
     * Traverse the existing lists and check if a list already exists
     * with the same key field values as the input list params.
     */
    while (linfo) {
        /*
         * Check if the new list params match with an existing list key
         * params.
         */
        if (shr_itbm_list_key_match(linfo->key, params)) {
            *mlist = linfo;
            *found = TRUE;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "ListMatchFound: (List key params matched "
                                  "existing LID=%u).\n"),
                       linfo->list_id));
            break;
        }
        /*
         * If the key params did not match, check if the new input params
         * min/max overlaps with existing list's min-max index range of values.
         * A partial overlap of min/max index value with an existing list is
         * invalid, check this condition and return resource error.
         */
        if (shr_itbm_list_min_max_overlap(linfo->key, params)) {
            *found = FALSE;
            SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                                (BSL_META_U(unit,
                                            "List index range overlaps with "
                                            "\"LID=%u\" - Key:[Min=%u "
                                            "Max=%u], Param:[Min=%u "
                                            "Max=%u].\n"),
                                 linfo->list_id,
                                 linfo->key.min,
                                 linfo->key.max,
                                 params.min,
                                 params.max));
        }
        linfo = linfo->next;
    }

    /*
     * If no match was found, return the search status as not found and
     * exit.
     */
    if (!(*mlist)) {
        *found = FALSE;
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "ListMatch-NotFound.\n")));
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    } else {
        /* List key matched, check for rest of the params match. */
        alist = (*mlist)->alist;
        SHR_NULL_CHECK(alist, SHR_E_INTERNAL);

        while (alist) {
            shr_itbm_list_params_t_init(&lparams);
            lparams = alist->params;
            /* Check and return error for duplicate list. */
            if (shr_itbm_list_params_match(lparams, params)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "List(LID)=%u lattr_id(AID)=%u "
                                        "table_id=%u comp_id=%u min=%u "
                                        "max=%u first=%u last=%u bs=%u bkts=%u "
                                        "epbkt=%u rev=%u - rv=%s.\n"),
                            (*mlist)->list_id, alist->aid, lparams.table_id,
                            lparams.comp_id, lparams.min, lparams.max,
                            lparams.first, lparams.last, lparams.block_stat,
                            lparams.buckets, lparams.entries_per_bucket,
                            lparams.reverse, shr_errmsg(SHR_E_EXISTS)));
                SHR_VERBOSE_EXIT(SHR_E_EXISTS);
            }
            alist = alist->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_attr_delete(int unit,
                          shr_itbm_list_params_t params,
                          shr_itbm_list_info_t *linfo)
{
    shr_itbm_lattr_t *alist = NULL; /* Ptr to params linked list node. */
    shr_itbm_lattr_t *del = NULL; /* New list params. */
    shr_itbm_list_params_t lparams; /* Local params variable. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(linfo, SHR_E_PARAM);

    /*
     * The list must have atleast one params node to perform delete
     * operation and the list must be in use.
     */
    if (linfo->acount < 1 || linfo->lref_cnt < 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    alist = linfo->alist;
    SHR_NULL_CHECK(alist, SHR_E_PARAM);

    /* Check if the head node params matches the params to be deleted. */
    shr_itbm_list_params_t_init(&lparams);
    lparams = alist->params;
    if (shr_itbm_list_params_match(lparams, params)) {
        /*
         * Check if there are entries still in-use for this list handle.
         * If they are in-use, return SHR_E_BUSY error.
         */
        if (alist->num_allocs != 0) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Head-aid=%u: list elements alloced and "
                                    "are in-use, num_allocs=%u.\n"),
                         alist->aid, alist->num_allocs));
            SHR_VERBOSE_EXIT(SHR_E_BUSY);
        }
        /*
         * If in-use count is zero, then goahead and delete this list
         * attribute.
         */
        del = alist;
        linfo->alist = alist->next;
        SHR_FREE(del);

        /* Decrement list attributes count and the list reference count. */
        linfo->acount--;
        linfo->lref_cnt--;

        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Done (AttrDel): (LID=%-2u acount=%-2u "
                                  "lref_cnt=%-2u U=%-1u T=%-5u Mi=%-5u Ma=%-5u "
                                  "F=%-5u L=%-5u C=%-2u).\n"),
                       linfo->list_id,
                       linfo->acount,
                       linfo->lref_cnt,
                       lparams.unit,
                       lparams.table_id,
                       lparams.min,
                       lparams.max,
                       lparams.first,
                       lparams.last,
                       lparams.comp_id));
        }

       SHR_EXIT();
    }

    /* Traverse the list and find the node and delete it. */
    while (alist->next) {
        shr_itbm_list_params_t_init(&lparams);
        lparams = (alist->next)->params;
        if (shr_itbm_list_params_match(lparams, params)) {
            /*
             * Check if there are entries still in-use for this list
             * handle. If they are in-use, return SHR_E_BUSY error.
             */
            if ((alist->next)->num_allocs !=0) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "aid=%u: list elements alloced and are "
                                        "in-use, num_allocs=%u.\n"),
                             alist->aid, alist->num_allocs));
                SHR_VERBOSE_EXIT(SHR_E_BUSY);
            }
            /*
             * If in-use count is zero, then goahead and delete this list
             * attribute.
             */
            del = alist->next;
            alist->next = del->next;
            SHR_FREE(del);

            /*
             * Decrement list attributes count and the list reference
             * count.
             */
            linfo->acount--;
            linfo->lref_cnt--;

            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Done (AttrDel): (LID=%-2u acount=%-2u "
                                      "lref_cnt=%-2u U=%-1u T=%-5u Mi=%-5u "
                                      "Ma=%-5u F=%-5u L=%-5u C=%-2u).\n"),
                           linfo->list_id,
                           linfo->acount,
                           linfo->lref_cnt,
                           lparams.unit,
                           lparams.table_id,
                           lparams.min,
                           lparams.max,
                           lparams.first,
                           lparams.last,
                           lparams.comp_id));
            }
            SHR_EXIT();
        }
        alist = alist->next;
    }

    SHR_ERR_EXIT(SHR_E_INTERNAL);
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_add(int unit,
                  shr_itbm_list_params_t params,
                  char *name,
                  shr_itbm_list_hdl_t *hdl)
{
    shr_itbm_list_info_t *nlinfo = NULL; /* Ptr to a new list node. */
    shr_itbm_list_info_t *mlinfo = NULL; /* Ptr to matched list node. */
    shr_itbm_lattr_t *alist = NULL; /* Pointer to list attributes params. */
    shr_itbm_list_params_t lparams; /* Current list params. */
    int rv = SHR_E_INTERNAL; /* Function return value. */
    bool found = FALSE; /* A list match found status. */
    uint32_t aid = SHR_ITBM_INVALID; /* List attributes ID. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);

    /*
     * Check if this is the first list on this unit. If yes, then go ahead
     * and allocate a new list on this unit, allocate a new handle, update
     * the list information and return the handle to the caller.
     */
    if (!ctrl[unit]->lcount) {
        /* Allocate the first list on this unit. */
        rv = shr_itbm_list_alloc(unit, params, name, &nlinfo);
        SHR_IF_ERR_EXIT(rv);

        /* Allocate a new handle for this list. */
        rv = shr_itbm_hdl_alloc(unit, params, hdl);
        if (rv != SHR_E_NONE) {
            /* Free the list resources. */
            shr_itbm_list_free(unit, nlinfo);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "shr_itbm_hdl_alloc() - rv=%s.\n"),
                                 shr_errmsg(rv)));
        }

        /*
         * Add the new params to the matched list's attributes
         * linked-list.
         */
        rv = shr_itbm_list_attr_add(unit, params, name, nlinfo, &aid);
        if (rv != SHR_E_NONE) {
            /* Free the handle. */
            SHR_FREE(*hdl);
            /* Free the new list resources. */
            shr_itbm_list_free(unit, nlinfo);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "shr_itbm_list_attr_add() - "
                                            "rv=%s.\n"),
                                 shr_errmsg(rv)));
        }

        (*hdl)->linfo = nlinfo;
        (*hdl)->aid = aid;
        ctrl[unit]->ilist = nlinfo;

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "First list on this unit - rv=%s.\n"),
                   shr_errmsg(rv)));
    } else {
        /*
         * If there are existing lists on this unit (lcount > 0), check if
         * the new list params match any of the existing list key field
         * params i.e input list params {.unit, .table_id, .min, .max} ==
         * existing list key field params {.unit, .table_id, .min, .max}.
         *
         * Duplicate list: If there is a key fields match, then determine if
         * the caller is attempting to create a duplicate list i.e. rest of the
         * params {.comp_id, .first, .last, .buckets and .entries_per_bucket}
         * also match existing list {.comp_id, .first, .last, .buckets and
         * .entries_per_bucket} params. If they match, then it is a config
         * error, return SHR_E_EXISTS error as a list already exists with this
         * parameters.
         *
         * Indexed list shared between components:
         *  - If the new list input params {.first, .last, .buckets and
         *    .entries_per_bucket} are same as the existing list, but the
         *    {.comp_id} param value is different, then the list is being
         *    shared between two components. Append new list params to existing
         *    list attributes linked-list.
         *
         * List partitioned and shared between components:
         *  - If the {.first, .last} params of the new list are different
         *    from an existing key matched list, then the list is being
         *    partitioned by list index range and shared between components
         *    given the {.comp_id}s are also different. If the {.comp_id}s are
         *    same, then the list is partitioned by index range and used by
         *    the same application/component.
         *
         * For shared and index partitioned list cases:
         *  - A new handle is allocated for the existing list.
         *  - The list "params" are appended to list attributes linked-list and
         *    the list attributes count is incremented.
         *  - The list reference count is also incremented.
         *  - Newly allocated list handle pointer is updated to point to the
         *    existing list elements and the input list params is also copied
         *    to the list handle structure.
         *  - The list handle is returned to the caller via out parameter.
         */
        rv = shr_itbm_list_match_find(unit,
                                      params,
                                      &found,
                                      &mlinfo);
        if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT(rv);
        }

        /*
         * If a match was found, then matched list info (mlinfo) pointer
         * must be valid. So, validate the mlinfo pointer based on the
         * return status, before accessing it.
         */
        if (found && mlinfo == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Matched list \"mlinfo\" ptr is NULL.\n")));
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        /*
         * Matching list found, but supplied input params set is new. So,
         * add the new params to existing params linked-list maintained
         * for this indexed list.
         */
        if (found) {
            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                pb = shr_pb_create();
                shritbm_bmp_msg(pb,
                                mlinfo->lprop,
                                SHR_ITBM_LIST_PROP_COUNT,
                                lprop_msg);
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Match Found(LID=%-5u lprop=%s): Add New "
                                      "Attr - rv=%s.\n"),
                           mlinfo->list_id,
                           shr_pb_str(pb),
                           shr_errmsg(rv)));
                shr_pb_destroy(pb);
                pb = NULL;
            }
            if ((SHR_BITGET(mlinfo->lprop, SHR_ITBM_LIST_BUCKETS)
                && params.buckets == FALSE)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "params.buckets=%u, but must be"
                                        "enabled(1) for shared indexed "
                                        "bucket lists.\n"),
                             params.buckets));
                SHR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            if ((params.entries_per_bucket > mlinfo->max_ent_per_bkt
                 && mlinfo->tot_blks_alloced > 0)) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                                    (BSL_META_U(unit,
                                                "Matched indexed buckets list "
                                                "(LID=%u) blocks in-use, so "
                                                "cannot modify bucket params "
                                                "to \"buckets=%s + epbkt=%u\""
                                                "- rv=%s.\n"),
                                     mlinfo->list_id,
                                     params.buckets ? "T" : "F",
                                     params.entries_per_bucket,
                                     shr_errmsg(SHR_E_BUSY)));
            }
            /* Additional criteria to be met for indexed bucket type lists. */
            if (SHR_BITGET(mlinfo->lprop, SHR_ITBM_LIST_BUCKETS)) {
                alist = mlinfo->alist;
                SHR_NULL_CHECK(alist, SHR_E_INTERNAL);
                while (alist) {
                    shr_itbm_list_params_t_init(&lparams);
                    lparams = alist->params;
                    if (itbm_ibkt_list_first_last_match(lparams, params) == 0
                        && itbm_ibkt_list_first_last_overlap(lparams, params)) {
                        LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(unit,
                                                "Given first=%u, last=%u list "
                                                "range is part of parent list"
                                                ": (comp_id=%u table_id=%u "
                                                "epbkt=%u reverse=%u min=%u "
                                                "max=%u first=%u last=%u).\n"),
                                     params.first, params.last,
                                     lparams.comp_id, lparams.table_id,
                                     lparams.entries_per_bucket,
                                     lparams.reverse, lparams.min, lparams.max,
                                     lparams.first, lparams.last));
                        SHR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                    if (itbm_ibkt_list_first_last_match(lparams, params)
                        && lparams.comp_id == params.comp_id
                        && (lparams.entries_per_bucket
                            == params.entries_per_bucket
                            || lparams.reverse == params.reverse)) {
                        LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(unit,
                                                "A list already exists with "
                                                "given epbkt=%u, reverse=%u "
                                                "config, parent list lparams: "
                                                "(comp_id=%u table_id=%u "
                                                "epbkt=%u reverse=%u min=%u "
                                                "max=%u first=%u last=%u).\n"),
                                     params.entries_per_bucket,
                                     params.reverse,
                                     lparams.comp_id, lparams.table_id,
                                     lparams.entries_per_bucket,
                                     lparams.reverse, lparams.min, lparams.max,
                                     lparams.first, lparams.last));
                        SHR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                    if (itbm_ibkt_list_first_last_match(lparams, params)
                        && lparams.comp_id != params.comp_id
                        && ((lparams.reverse == params.reverse
                             && lparams.entries_per_bucket
                                != params.entries_per_bucket)
                            || (lparams.entries_per_bucket
                                == params.entries_per_bucket
                                && lparams.reverse != params.reverse))) {
                        LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(unit,
                                                "New list epbkt=%u reverse=%u, "
                                                "param values must be "
                                                "identical to the parent list: "
                                                "(comp_id=%u table_id=%u "
                                                "epbkt=%u reverse=%u min=%u "
                                                "max=%u first=%u last=%u).\n"),
                                     params.entries_per_bucket, params.reverse,
                                     lparams.comp_id, lparams.table_id,
                                     lparams.entries_per_bucket,
                                     lparams.reverse, lparams.min, lparams.max,
                                     lparams.first, lparams.last));
                        SHR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                    alist = alist->next;
                }
            }
            /*
             * If the new list supports more entries per-bucket and if no
             * elements from the list are in use, then free the current edata
             * list elements (allocated based on old entries_per_bucket count)
             * and realloc them using the new entries_per_bucket count. Upon
             * successful alloc, the list elements can support new
             * entries_per_bucket members per list element.
             */
            if (params.entries_per_bucket > mlinfo->max_ent_per_bkt
                && mlinfo->tot_blks_alloced == 0) {
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_edata_elements_realloc(unit,
                                                          params,
                                                          name,
                                                          mlinfo));
            }

            /* Update the matched list in the list handle. */
            SHR_IF_ERR_EXIT
                (shr_itbm_hdl_alloc(unit, params, hdl));
            (*hdl)->linfo = mlinfo;

            /*
             * Add the new params to the matched list's attributes
             * linked-list.
             */
            rv = shr_itbm_list_attr_add(unit, params, name, mlinfo, &aid);
            if (rv != SHR_E_NONE) {
                /* Free the handle. */
                SHR_FREE(*hdl);
                SHR_IF_ERR_MSG_EXIT(rv,
                                    (BSL_META_U(unit,
                                                "shr_itbm_list_attr_add() - "
                                                "rv=%s.\n"),
                                     shr_errmsg(rv)));
            }
            (*hdl)->aid = aid;

            /* Clear regular and set as shared list. */
            SHR_BITCLR(mlinfo->lprop, SHR_ITBM_LIST_REGULAR);
            SHR_BITSET(mlinfo->lprop, SHR_ITBM_LIST_SHARED);
            /*
             * If the new list first and last values do not match the matched
             * list  key min-max values, then the new list is part/subset of
             * the matched superset list,
             */
            if (params.first != mlinfo->key.min
                || params.last != mlinfo->key.max) {
                SHR_BITSET(mlinfo->lprop, SHR_ITBM_LIST_PART);
            }
        } else {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "No Match: Create New List - rv=%s.\n"),
                       shr_errmsg(rv)));

            rv = shr_itbm_list_alloc(unit, params, name, &nlinfo);
            SHR_IF_ERR_EXIT(rv);

            rv = shr_itbm_hdl_alloc(unit, params, hdl);
            if (rv != SHR_E_NONE) {
                /* Free the new list resources. */
                shr_itbm_list_free(unit, nlinfo);
                SHR_IF_ERR_MSG_EXIT(rv,
                                    (BSL_META_U(unit,
                                                "shr_itbm_hdl_alloc() - "
                                                "rv=%s.\n"),
                                     shr_errmsg(rv)));
            }
            (*hdl)->linfo = nlinfo;

            /*
             * Add the new params to the matched list's attributes
             * linked-list.
             */
            rv = shr_itbm_list_attr_add(unit, params, name, nlinfo, &aid);
            if (rv != SHR_E_NONE) {
                /* Free the handle. */
                SHR_FREE(*hdl);
                /* Free the new list resources. */
                shr_itbm_list_free(unit, nlinfo);
                SHR_IF_ERR_MSG_EXIT(rv,
                                    (BSL_META_U(unit,
                                                "shr_itbm_list_attr_add() - "
                                                "rv=%s.\n"),
                                     shr_errmsg(rv)));
            }
            (*hdl)->aid = aid;

            rv = shr_itbm_ilist_node_append(unit, nlinfo);
            if (rv != SHR_E_NONE) {
                /* Delete the new list attribute. */
                shr_itbm_list_attr_delete(unit, params, nlinfo);
                /* Free the handle. */
                SHR_FREE(*hdl);
                /* Free the new list resources. */
                shr_itbm_list_free(unit, nlinfo);
                SHR_IF_ERR_MSG_EXIT(rv,
                                    (BSL_META_U(unit,
                                                "shr_itbm_ilist_node_append() "
                                                "- rv=%s.\n"),
                                     shr_errmsg(rv)));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_delete(int unit,
                     shr_itbm_list_hdl_t hdl)
{
    shr_itbm_list_info_t *linfo = NULL; /* New list node. */

    SHR_FUNC_ENTER(unit);

    /* Validate the list pointer. */
    linfo = hdl->linfo;
    SHR_NULL_CHECK(linfo, SHR_E_PARAM);

    /* Delete the attributes of this list. */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_attr_delete(unit,
                                   hdl->params,
                                   hdl->linfo));
    /*
     * If the list reference count is '0', then free the list resources as
     * part of the list delete operation.
     */
    if (linfo->lref_cnt == 0) {
        SHR_IF_ERR_EXIT
            (shr_itbm_ilist_node_delete(unit, hdl->linfo));
        hdl->linfo = NULL;
        /* Decrement list count for this unit. */
        ctrl[unit]->lcount--;
    }

    /* Free list handle memory. */
    SHR_FREE(hdl);

exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_aparam_elem_count_get(int unit,
                                       shr_itbm_list_op_t *op,
                                       bool alloc,
                                       shr_itbm_list_params_t aparams,
                                       uint32_t *count)
{
    int becnt = 0;
    uint32_t b = 0, num_bkt = 0;
    uint32_t bb = SHR_ITBM_INVALID, eb = SHR_ITBM_INVALID;
    uint32_t min_idx = SHR_ITBM_INVALID;
    uint32_t num_bits = SHR_ITBM_INVALID;
    shr_itbm_list_edata_t *edata = NULL; /* Pointer to list elements array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    edata = op->linfo->edata;
    bb = op->base_bkt - op->offset;
    eb = op->end_bkt - op->offset;
    if (alloc) {
        for (b = bb; b <= eb; b++) {
            /*
             * For list forward direction management, min_idx will be the base
             * bucket block first element bucket local index or the base index
             * i.e. 0 of the bucket.
             */
            min_idx = (b == bb) ? op->base_idx : 0;
            num_bits = (b == eb) ? (op->end_idx + 1)
                                    : op->params.entries_per_bucket;
            /* Initialize entries in the bucket count value to zero. */
            becnt = 0;
            SHR_NULL_CHECK(edata[b].ebmp, SHR_E_INTERNAL);
            SHR_BITCOUNT_RANGE(edata[b].ebmp, becnt, min_idx, num_bits);
            if (becnt == edata[b].eref_cnt) {
                num_bkt++;
            }
        }
    } else {
        for (b = bb; b <= eb; b++) {
            if (edata[b].eref_cnt == 0) {
                num_bkt++;
            }
        }
    }
    *count = (num_bkt * aparams.entries_per_bucket);
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_stat_bkup_clear(int unit,
                              shr_itbm_list_hdl_t hdl)
{
    shr_itbm_list_info_t *linfo = NULL; /* New list node. */
    shr_itbm_lattr_t *alist = NULL; /* Ptr to list attributes linked list. */
    shr_itbm_list_params_t lparams; /* Pointer to list params. */
    shr_itbm_list_params_t aparams; /* Pointer to list attributes params. */
    bool found = FALSE; /* Attribute ID match found. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    linfo = hdl->linfo;
    SHR_NULL_CHECK(linfo->alist, SHR_E_PARAM);
    alist = linfo->alist;

    while (alist) {
        if (alist->aid == hdl->aid) {
            found = TRUE;
            alist->stat_bk.in_use_count = 0;
            alist->stat_bk.free_count = 0;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LID=%u AID=%u: Bk - (in_use=%u free_cnt=%u)."
                                  "\n"),
                       linfo->list_id, alist->aid,
                       alist->stat_bk.in_use_count,
                       alist->stat_bk.free_count));
            break;
        }
        alist = alist->next;
    }

    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Check and update stats for the shared indexed bucket lists. */
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_SHARED)) {
        shr_itbm_list_params_t_init(&lparams);
        lparams = hdl->params;
        alist = linfo->alist;
        while (alist) {
            shr_itbm_list_params_t_init(&aparams);
            aparams = alist->params;
            if (alist->aid != hdl->aid && aparams.first >= lparams.first
                && aparams.last <= lparams.last) {
                alist->stat_bk.in_use_count = 0;
                alist->stat_bk.free_count = 0;;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LID=%u AID=%u: Bk - (in_use=%u "
                                      "free_cnt=%u).\n"),
                           linfo->list_id, alist->aid,
                           alist->stat_bk.in_use_count,
                           alist->stat_bk.free_count));
            }
            alist = alist->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_stat_bkup(int unit,
                        shr_itbm_list_hdl_t hdl)
{
    shr_itbm_list_info_t *linfo = NULL; /* New list node. */
    shr_itbm_lattr_t *alist = NULL; /* Ptr to list attributes linked list. */
    shr_itbm_list_params_t lparams; /* Pointer to list params. */
    shr_itbm_list_params_t aparams; /* Pointer to list attributes params. */
    bool found = FALSE; /* Attribute ID match found. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    linfo = hdl->linfo;
    SHR_NULL_CHECK(linfo->alist, SHR_E_PARAM);
    alist = linfo->alist;

    while (alist) {
        if (alist->aid == hdl->aid) {
            found = TRUE;
            alist->stat_bk.in_use_count = alist->stat.in_use_count;
            alist->stat_bk.free_count = alist->stat.free_count;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LID=%u AID=%u: Bk - (in_use=%u free_cnt=%u)"
                                  " Cur - (in_use=%u free_cnt=%u).\n"),
                       linfo->list_id, alist->aid,
                       alist->stat_bk.in_use_count,
                       alist->stat_bk.free_count,
                       alist->stat.in_use_count, alist->stat.free_count));
            break;
        }
        alist = alist->next;
    }

    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Check and update stats for the shared indexed bucket lists. */
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_SHARED)) {
        shr_itbm_list_params_t_init(&lparams);
        lparams = hdl->params;
        alist = linfo->alist;
        while (alist) {
            shr_itbm_list_params_t_init(&aparams);
            aparams = alist->params;
            if (alist->aid != hdl->aid && aparams.first >= lparams.first
                && aparams.last <= lparams.last) {
                alist->stat_bk.in_use_count = alist->stat.in_use_count;
                alist->stat_bk.free_count = alist->stat.free_count;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LID=%u AID=%u: Bk - (in_use=%u "
                                      "free_cnt=%u) Cur - (in_use=%u "
                                      "free_cnt=%u).\n"),
                           linfo->list_id, alist->aid,
                           alist->stat_bk.in_use_count,
                           alist->stat_bk.free_count,
                           alist->stat.in_use_count, alist->stat.free_count));
            }
            alist = alist->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_stat_abort(int unit,
                         shr_itbm_list_hdl_t hdl)
{
    shr_itbm_list_info_t *linfo = NULL; /* New list node. */
    shr_itbm_lattr_t *alist = NULL; /* Ptr to list attributes linked list. */
    shr_itbm_list_params_t lparams; /* Pointer to list params. */
    shr_itbm_list_params_t aparams; /* Pointer to list attributes params. */
    bool found = FALSE; /* Attribute ID match found. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    linfo = hdl->linfo;
    SHR_NULL_CHECK(linfo->alist, SHR_E_PARAM);
    alist = linfo->alist;

    while (alist) {
        if (alist->aid == hdl->aid) {
            found = TRUE;
            alist->stat.in_use_count = alist->stat_bk.in_use_count;
            alist->stat.free_count = alist->stat_bk.free_count;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LID=%u AID=%u: Bk - (in_use=%u free_cnt=%u)"
                                  " Cur - (in_use=%u free_cnt=%u).\n"),
                       linfo->list_id, alist->aid,
                       alist->stat_bk.in_use_count,
                       alist->stat_bk.free_count,
                       alist->stat.in_use_count, alist->stat.free_count));
            break;
        }
        alist = alist->next;
    }

    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Check and update stats for the shared indexed bucket lists. */
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_SHARED)) {
        shr_itbm_list_params_t_init(&lparams);
        lparams = hdl->params;
        alist = linfo->alist;
        while (alist) {
            shr_itbm_list_params_t_init(&aparams);
            aparams = alist->params;
            if (alist->aid != hdl->aid && aparams.first >= lparams.first
                && aparams.last <= lparams.last) {
                alist->stat.in_use_count = alist->stat_bk.in_use_count;
                alist->stat.free_count = alist->stat_bk.free_count;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LID=%u AID=%u: Bk - (in_use=%u "
                                      "free_cnt=%u) Cur - (in_use=%u "
                                      "free_cnt=%u).\n"),
                           linfo->list_id, alist->aid,
                           alist->stat_bk.in_use_count,
                           alist->stat_bk.free_count,
                           alist->stat.in_use_count, alist->stat.free_count));
            }
            alist = alist->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_stat_update(int unit,
                              shr_itbm_list_hdl_t hdl,
                              shr_itbm_list_op_t *op)
{
    shr_itbm_list_info_t *linfo = NULL; /* New list node. */
    shr_itbm_lattr_t *alist = NULL; /* Ptr to list attributes linked list. */
    shr_itbm_list_params_t lparams; /* Pointer to list params. */
    shr_itbm_list_params_t aparams; /* Pointer to list attributes params. */
    bool alloc = FALSE; /* Alloc operation. */
    bool found = FALSE; /* Attribute ID match found. */
    uint32_t count = 0; /* List elements update count. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    linfo = hdl->linfo;
    SHR_NULL_CHECK(linfo->alist, SHR_E_PARAM);
    alist = linfo->alist;

    if (SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_ALLOC)
        || SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_ALLOC_ID)) {
        alloc = TRUE;
    }

    while (alist) {
        if (alist->aid == hdl->aid) {
            found = TRUE;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "%s - LID=%u AID=%u num_elem=%u: cur - "
                                  "in_use=%u free_cnt=%u new - in_use=%u "
                                  "free_cnt=%u.\n"),
                       alloc ? "alloc" : "free", linfo->list_id, alist->aid,
                       op->num_elem, alist->stat.in_use_count,
                       alist->stat.free_count,
                       alloc ? (alist->stat.in_use_count + op->num_elem)
                                : (alist->stat.in_use_count - op->num_elem),
                       alloc ? (alist->stat.free_count - op->num_elem)
                                : (alist->stat.free_count + op->num_elem)));
            if (alloc) {
                alist->stat.in_use_count += op->num_elem;
                alist->stat.free_count -= op->num_elem;
            } else {
                alist->stat.in_use_count -= op->num_elem;
                alist->stat.free_count += op->num_elem;
            }
            if (SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_RESIZE) == 0) {
                /*
                 * A block resize is already accounted during the
                 * original block alloc, so it's not recounted in
                 * num_allocs.
                 */
                alloc ? alist->num_allocs++ : alist->num_allocs--;
            }
            break;
        }
        alist = alist->next;
    }

    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Check and update stats for the shared indexed bucket lists. */
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_SHARED)) {
        shr_itbm_list_params_t_init(&lparams);
        lparams = hdl->params;
        alist = linfo->alist;
        while (alist) {
            shr_itbm_list_params_t_init(&aparams);
            aparams = alist->params;
            if (alist->aid != hdl->aid && aparams.first >= lparams.first
                && aparams.last <= lparams.last) {
                if (aparams.entries_per_bucket
                    != lparams.entries_per_bucket) {
                    /*
                     * Compute the elements count in terms of the shared
                     * list entries_per_bucket value, as it is different
                     * from the current list's entries_per_bucket value.
                     */
                    SHR_IF_ERR_EXIT
                        (shr_itbm_bkt_list_aparam_elem_count_get(unit, op,
                                                                 alloc, aparams,
                                                                 &count));
                } else {
                    count = op->num_elem;
                }

                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "%s - LID=%u AID=%u num_elem=%u/count=%u:"
                                      " cur - in_use=%u free_cnt=%u new - "
                                      "in_use=%u free_cnt=%u.\n"),
                           alloc ? "alloc" : "free", linfo->list_id, alist->aid,
                           op->num_elem, count, alist->stat.in_use_count,
                           alist->stat.free_count,
                           alloc ? (alist->stat.in_use_count + count)
                                    : (alist->stat.in_use_count - count),
                           alloc ? (alist->stat.free_count - count)
                                    : (alist->stat.free_count + count)));

                if (alloc) {
                    alist->stat.in_use_count += count;
                    alist->stat.free_count -= count;
                } else {
                    alist->stat.in_use_count -= count;
                    alist->stat.free_count += count;
                }
            }
            alist = alist->next;
        }
    }

    if (alloc) {
        linfo->inuse_ecount += op->num_elem;
        linfo->free_ecount -= op->num_elem;
    } else {
        linfo->inuse_ecount -= op->num_elem;
        linfo->free_ecount += op->num_elem;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_stat_update(int unit,
                          shr_itbm_list_hdl_t hdl,
                          shr_itbm_list_op_t *op)
{
    shr_itbm_list_info_t *linfo = NULL; /* New list node. */
    shr_itbm_lattr_t *alist = NULL; /* Ptr to list attributes linked list. */
    shr_itbm_list_params_t lparams; /* Pointer to list params. */
    shr_itbm_list_params_t aparams; /* Pointer to list attributes params. */
    bool alloc = FALSE; /* Alloc operation. */
    bool found = FALSE; /* Attribute ID match found. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    linfo = hdl->linfo;
    SHR_NULL_CHECK(linfo->alist, SHR_E_PARAM);
    alist = linfo->alist;

    if (SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_ALLOC)
        || SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_ALLOC_ID)) {
        alloc = TRUE;
    }
    while (alist) {
        if (alist->aid == hdl->aid) {
            found = TRUE;
            if (alloc) {
                alist->stat.in_use_count += op->num_elem;
                alist->stat.free_count -= op->num_elem;
            } else {
                alist->stat.in_use_count -= op->num_elem;
                alist->stat.free_count += op->num_elem;
            }
            if (SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_RESIZE) == 0) {
                alloc ? alist->num_allocs++ : alist->num_allocs--;
            }
            break;
        }
        alist = alist->next;
    }

    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Update stats for the shared lists. */
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_SHARED)) {
        shr_itbm_list_params_t_init(&lparams);
        lparams = hdl->params;
        alist = linfo->alist;
        while (alist) {
            shr_itbm_list_params_t_init(&aparams);
            aparams = alist->params;
            if (alist->aid != hdl->aid && aparams.first >= lparams.first
                && aparams.last <= lparams.last) {
                if (alloc) {
                    alist->stat.in_use_count += op->num_elem;
                    alist->stat.free_count -= op->num_elem;
                } else {
                    alist->stat.in_use_count -= op->num_elem;
                    alist->stat.free_count += op->num_elem;
                }
            }
            alist = alist->next;
        }
    }

    if (alloc) {
        linfo->inuse_ecount += op->num_elem;
        linfo->free_ecount -= op->num_elem;
    } else {
        linfo->inuse_ecount -= op->num_elem;
        linfo->free_ecount += op->num_elem;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_edata_update(int unit,
                           shr_itbm_list_edata_t *elem_arr,
                           const shr_itbm_list_op_t *op)
{
    uint32_t i; /* Element offset index in the block. */
    uint32_t f = SHR_ITBM_INVALID; /* First element in the block. */
    bool alloc = FALSE; /* Block alloc operation. */
    bool rsz = FALSE; /* Block resize operation. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(elem_arr, SHR_E_INTERNAL);

    /* Get block base index accounting for non-zero offset index value. */
    f = op->base_idx - op->offset;
    /*
     * Get the type of block operation to update edata elements info
     * accordingly.
     */
    if (SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_ALLOC)
        || SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_ALLOC_ID)) {
        alloc = TRUE;
    }
    if (SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_RESIZE)) {
        rsz = TRUE;
    }

    for (i = 0; i < op->num_elem; i++) {
        if (alloc) {
            if (elem_arr[(f + i)].eref_cnt == 0) {
                /*
                 * Allocate memory to store the component which is using
                 * this entry.
                 */
                elem_arr[(f + i)].comp_id = NULL;
                SHR_ALLOC(elem_arr[(f + i)].comp_id,
                          (sizeof(*(elem_arr[f + i].comp_id))
                           * op->linfo->max_ent_per_bkt),
                          "shritbmBlkCompId");
                SHR_NULL_CHECK(elem_arr[(f + i)].comp_id, SHR_E_MEMORY);
                sal_memset(elem_arr[(f + i)].comp_id, 0,
                           (sizeof(*(elem_arr[f + i].comp_id))
                            * op->linfo->max_ent_per_bkt));
                elem_arr[(f + i)].comp_id[SHR_ITBM_LIST_BKT_NONE] =
                                                            op->params.comp_id;
            }
            elem_arr[(f + i)].eref_cnt++;
        } else {
            SHR_NULL_CHECK(elem_arr[(f + i)].comp_id, SHR_E_INTERNAL);
            if (elem_arr[(f + i)].comp_id[SHR_ITBM_LIST_BKT_NONE]
                != op->params.comp_id) {
                /*
                 * Element reference count must be non-zero to free
                 * and the component that allocated must free the element.
                 * So, element comp_id must match input comp_id value.
                 */
                SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                    (BSL_META_U(unit,
                                                "block_free() called with "
                                                "\"comp_id=%u\" hdl but "
                                                "blk in-use by "
                                                "\"comp_id=%d\" - rv=%s.\n"),
                              (int)op->params.comp_id,
                              elem_arr[(f + i)].comp_id[SHR_ITBM_LIST_BKT_NONE],
                              shr_errmsg(SHR_E_PARAM)));
            }
            elem_arr[(f + i)].eref_cnt--;
            if (elem_arr[(f + i)].eref_cnt == 0) {
                SHR_FREE(elem_arr[(f + i)].comp_id);
            }
        }
    }
    /*
     * Check if block statatistics must be updated for this list. If needed,
     * then update the details.
     */
    if (op->params.block_stat && rsz == FALSE) {
        if (alloc) {
            /* Entry property. */
            elem_arr[f].eprop = NULL;
            SHR_ALLOC(elem_arr[f].eprop,
                      SHR_BITALLOCSIZE(op->linfo->max_ent_per_bkt),
                      "shritbmBlkEprop");
            SHR_NULL_CHECK(elem_arr[f].eprop, SHR_E_MEMORY);
            sal_memset(elem_arr[f].eprop, 0,
                       SHR_BITALLOCSIZE(op->linfo->max_ent_per_bkt));
            SHR_BITSET(elem_arr[f].eprop, SHR_ITBM_LIST_BLK_FIRST_ELEM);

            /* Block size, can range from 1 to device supported maximum. */
            elem_arr[f].blk_sz = NULL;
            SHR_ALLOC(elem_arr[f].blk_sz,
                      (sizeof(*(elem_arr[f].blk_sz))
                       * op->linfo->max_ent_per_bkt), "shritbmBlkSz");
            SHR_NULL_CHECK(elem_arr[f].blk_sz, SHR_E_MEMORY);
            sal_memset(elem_arr[f].blk_sz, 0,
                       (sizeof(*(elem_arr[f].blk_sz))
                        * op->linfo->max_ent_per_bkt));
            elem_arr[f].blk_sz[SHR_ITBM_LIST_BKT_NONE] = op->num_elem;

            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "blk-alloc: [u=%-1u eprop=0x%x f=%-5u "
                                      "sz=%-5u].\n"),
                           unit,
                           SHR_BITGET(elem_arr[f].eprop,
                                      SHR_ITBM_LIST_BLK_FIRST_ELEM),
                           f,
                           elem_arr[f].blk_sz[SHR_ITBM_LIST_BKT_NONE]));
            }
        } else {
            SHR_NULL_CHECK(elem_arr[f].eprop, SHR_E_INTERNAL);
            SHR_NULL_CHECK(elem_arr[f].blk_sz, SHR_E_INTERNAL);
            SHR_FREE(elem_arr[f].eprop);
            SHR_FREE(elem_arr[f].blk_sz);
            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "blk-freed: [u=%-1u f=%-5u sz=%-5u].\n"),
                           unit,
                           f,
                           op->num_elem));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_edata_update(int unit,
                               shr_itbm_list_edata_t *edata,
                               const shr_itbm_list_op_t *op)
{
    bool alloc = FALSE, rsz = FALSE; /* Alloc and resize oper flags. */
    uint32_t bb = SHR_ITBM_INVALID; /* Base bucket number. */
    uint32_t bbi = SHR_ITBM_INVALID; /* Base bucket offset index. */
    uint32_t b = SHR_ITBM_INVALID; /* Bucket iterator. */
    uint32_t e = SHR_ITBM_INVALID; /* Bucket element index. */
    uint32_t epbkt = SHR_ITBM_INVALID; /* Total entries in the bucket. */
    uint32_t n = 0; /* Number of elements to set. */
    uint32_t nb = 0; /* Number of buckets. */
    uint32_t ne = 0; /* Number of elements in the block. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(edata, SHR_E_PARAM);

    if (op->base_bkt < op->params.first || op->base_bkt > op->params.last
        || op->end_bkt < op->params.first || op->end_bkt> op->params.last) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List index range: \"f=%u l=%u \""
                                        "(zoff=%u), input bbkt=%d - "
                                        "ebkt=%d (num_bkt=%u) not within"
                                        " range - rv=%s.\n"),
                             op->params.first,
                             op->params.last,
                             op->offset,
                             (int)op->base_bkt,
                             (int)op->end_bkt,
                             op->num_bkt,
                             shr_errmsg(SHR_E_PARAM)));
    }
    if (op->base_idx > op->params.entries_per_bucket) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "base_idx=%d > becnt=%u (total "
                                        "entries in the bucket) - rv=%s.\n"),
                             (int)op->base_idx,
                             op->params.entries_per_bucket,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Get the block first entry index. */
    epbkt = op->params.entries_per_bucket;
    bb = op->base_bkt - op->offset;
    bbi = op->base_idx;
    nb = op->num_bkt;
    ne = op->num_elem;

    if (SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_ALLOC)
        || SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_ALLOC_ID)) {
        alloc = TRUE;
    }

    if (SHR_BITGET(op->loper, SHR_ITBM_LIST_OPER_RESIZE)) {
        rsz = TRUE;
    }

    /* Block element/s alloc operation. */
    if (alloc) {
        for (b = bb, n = 0; b < (bb + nb); b++) {
            SHR_NULL_CHECK(edata[b].ebmp, SHR_E_INTERNAL);
            if (edata[b].eref_cnt == 0) {
                /*
                 * If this is a new bucket that is being used for elements
                 * allocation, then update the bucket maximum entries supported
                 * by this bucket count. This value is used for future
                 * elements allocation from this list and also for resize
                 * and free operations.
                 */
                edata[b].max_entries = epbkt;
                /*
                 * Allocate memory to store the component associated with this
                 * element.
                 */
                edata[b].comp_id = NULL;
                SHR_ALLOC(edata[b].comp_id,
                          (sizeof(*(edata[b].comp_id))
                           * op->linfo->max_ent_per_bkt),
                          "shritbmBktBlkCompId");
                SHR_NULL_CHECK(edata[b].comp_id, SHR_E_MEMORY);
                sal_memset(edata[b].comp_id, 0,
                           (sizeof(*(edata[b].comp_id))
                            * op->linfo->max_ent_per_bkt));
            }
            SHR_NULL_CHECK(edata[b].comp_id, SHR_E_INTERNAL);
            for (e = bbi; e < epbkt && n < ne; e++) {
                SHR_BITSET(edata[b].ebmp, e);
                edata[b].comp_id[e] = op->params.comp_id;
                edata[b].eref_cnt++;
                n++;
            }
            bbi = 0;
        }
    } else {
        /* Block element/s free operation. */
        for (b = bb, n = ne; b < (bb + nb); b++) {
            if (!edata[b].ebmp || !edata[b].comp_id) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "edata[%u]: null - epbm=%u "
                                        "comp_id=%u.\n"),
                             b,
                             edata[b].ebmp == NULL ? 1 : 0,
                             edata[b].comp_id == NULL ? 1 : 0));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            for (e = bbi; e < epbkt && n > 0; e++) {
                if (edata[b].comp_id[e] != op->params.comp_id) {
                    /*
                     * Element reference count must be non-zero to free
                     * and the component that allocated must free the element.
                     * So, element comp_id must match input comp_id value.
                     */
                    SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                        (BSL_META_U(unit,
                                                    "Block: (b=0x%x e=0x%x)"
                                                    " in-use by \"comp_id=%u"
                                                    "\", but block_free() call "
                                                    "from \"comp_id=%u\" - "
                                                    "rv=%s.\n"),
                                         b,
                                         e,
                                         edata[b].comp_id[e],
                                         op->params.comp_id,
                                         shr_errmsg(SHR_E_PARAM)));
                }
                SHR_BITCLR(edata[b].ebmp, e);
                edata[b].comp_id[e] = SHR_ITBM_INVALID;
                edata[b].eref_cnt--;
                n--;
            }
            bbi = 0;
            if (edata[b].eref_cnt == 0) {
                SHR_FREE(edata[b].comp_id);
                edata[b].max_entries = 0;
            }
        }
    }
    /*
     * Check if block statatistics must be maintained for this list and update
     * the details.
     */
    if (op->params.block_stat && rsz == FALSE) {
        /* Get the block base bucket offset index value. */
        bbi = op->base_idx;
        if (alloc) {
            if (edata[bb].blk_sz == NULL) {
                SHR_ALLOC(edata[bb].blk_sz,
                          (sizeof(*(edata[bb].blk_sz))
                           * op->linfo->max_ent_per_bkt),
                          "shritbmBktBlkSz");
                SHR_NULL_CHECK(edata[bb].blk_sz, SHR_E_MEMORY);
                sal_memset(edata[bb].blk_sz, 0,
                           (sizeof(*(edata[bb].blk_sz))
                            * op->linfo->max_ent_per_bkt));
            }
            if (edata[bb].eprop == NULL) {
                SHR_ALLOC(edata[bb].eprop,
                          SHR_BITALLOCSIZE(op->linfo->max_ent_per_bkt),
                          "shritbmBktBlkEprop");
                SHR_NULL_CHECK(edata[bb].eprop, SHR_E_MEMORY);
                sal_memset(edata[bb].eprop, 0,
                           SHR_BITALLOCSIZE(op->linfo->max_ent_per_bkt));
            }
            /*
             * Store the block size value in the base bucket at the first
             * element offset index for new alloc and alloc_id operations.
             */
            edata[bb].blk_sz[bbi] = ne;
            SHR_BITSET_RANGE(edata[bb].eprop, bbi, 1);
        } else {
            SHR_NULL_CHECK(edata[bb].eprop, SHR_E_INTERNAL);
            SHR_NULL_CHECK(edata[bb].blk_sz, SHR_E_INTERNAL);
            /*
             * Clear the stored block size value from the base bucket at first
             * element offset index.
             */
            edata[bb].blk_sz[bbi] = 0;
            SHR_BITCLR_RANGE(edata[bb].eprop, bbi, 1);
            if (edata[bb].eref_cnt == 0) {
                SHR_FREE(edata[bb].blk_sz);
                SHR_FREE(edata[bb].eprop);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_elems_alloc_rev(int unit,
                                  shr_itbm_list_op_t *op)
{
    uint32_t f = SHR_ITBM_INVALID; /* First element number of the list. */
    uint32_t l = SHR_ITBM_INVALID; /* Last element numbers of the list. */
    uint32_t epbkt = 0; /* Entries per-bucket count. */
    uint32_t eb = SHR_ITBM_INVALID; /* end bucket number. */
    uint32_t ebi = SHR_ITBM_INVALID; /* End bucket local entry offset index. */
    uint32_t ebl = SHR_ITBM_INVALID; /* End bucket blk last element. */
    uint32_t eb_off = SHR_ITBM_INVALID; /* End bucket blk last element. */
    uint32_t e = SHR_ITBM_INVALID; /* Entry offset in base bucket. */
    uint32_t cbkt = SHR_ITBM_INVALID; /* Current bucket iterator variable. */
    uint32_t num_elem = 0; /* Number of elements to alloc. */
    uint32_t rbkt_cnt = 0; /* Required number of buckets. */
    int count = 0; /* Number of bits set in the specified range. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->lattr, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    /* Initialize the nubmer of buckets used return value. */
    edata = op->linfo->edata;
    f = op->params.first - op->offset;
    l = op->params.last - op->offset;
    epbkt = op->params.entries_per_bucket;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[loper=0x%x]: (U=%-2u T=%-5u comp_id=%-2u "
                              "f=%-5u l=%-5u epbkt=%-2u num_elem=%-5u "
                              "zoff=%-5u).\n"),
                   op->loper[0],
                   op->params.unit,
                   op->params.table_id,
                   op->params.comp_id,
                   f + op->offset,
                   l + op->offset,
                   epbkt,
                   op->num_elem,
                   op->offset));
    }

    for (eb = l; eb >= f && eb <= l; eb--) {
        SHR_NULL_CHECK(edata[eb].ebmp, SHR_E_INTERNAL);
        /*
         * Initialize base bucket, entry offset index in bucket and bucket bits
         * set count variables.
         */
        ebi = count = rbkt_cnt = 0;
        /* Initialize the end bucket block last element bucket local index. */
        eb_off = ebl = epbkt - 1;
        /*
         * "num_elem" value is used to determine if the alloc has been completed
         * successfully.
         */
        num_elem = op->num_elem;
        /*
         * Skip in-use bucket with mismatch in bucket capacity or if the
         * in-use bucket is full.
         */
        if ((edata[eb].eref_cnt > 0 && edata[eb].max_entries != epbkt)
            || (edata[eb].eref_cnt > 0
                && edata[eb].eref_cnt == edata[eb].max_entries)) {
            continue;
        }
        /*
         * Following statements determine if an in-use block's free elements can
         * be used for the new block alloc.
         */
        if (edata[eb].eref_cnt > 0 && edata[eb].max_entries == epbkt
            && edata[eb].eref_cnt < edata[eb].max_entries) {
            /*
             * Determine how many free entries are available in the end
             * bucket.
             */
            ebi = (epbkt - edata[eb].eref_cnt);
            /*
             * For max index to min index allocation scheme, bucket local
             * entries at the lower indices can be used if they are free. So,
             * if the bucket reference count is lesser than max entries of the
             * bucket, check free entries are available at the lower indices.
             */
            SHR_BITCOUNT_RANGE(edata[eb].ebmp, count, 0, ebi);
            if (count != 0) {
                /* Not all free elements are at the lower index. */
                continue;
            }
            /*
             * Free entries are available at the LSB position, compute the end
             * bucket last element bucket local index.
             */
            ebl = eb_off = (epbkt - edata[eb].eref_cnt) - 1;
        }

        /*
         * Determine number of buckets required to complete this alloc
         * request, based on the free elements available in the end bucket (eb)
         * and additional buckets required to complete the block alloc.
         */
        rbkt_cnt = shr_itbm_list_blk_num_bkts_get(num_elem,
                                                  ebl,
                                                  epbkt,
                                                  op->params.reverse);
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Req bkts(lrev=%s): rbkt_cnt=%u - (epbkt=%u "
                                  "eb=%-5d ebl=%u eb_off=%u num_elem=%u).\n"),
                       op->params.reverse ? "T" : "F" ,
                       rbkt_cnt,
                       epbkt,
                       (int)eb,
                       ebl,
                       eb_off,
                       num_elem));
        }

        if (eb < (rbkt_cnt - 1)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "eb less than rbkt_cnt - 1 %d.\n"),
                       (eb - (rbkt_cnt - 1))));
            break;
        }

        for (cbkt = eb;
             cbkt >= (eb - (rbkt_cnt - 1)) && cbkt >= f
             && cbkt != SHR_ITBM_INVALID;
             cbkt--) {
            SHR_NULL_CHECK(edata[cbkt].ebmp, SHR_E_INTERNAL);
            for (e = eb_off;
                 e < epbkt && e != SHR_ITBM_INVALID && num_elem > 0;
                 e--) {
                if (SHR_BITGET(edata[cbkt].ebmp, e)) {
                    break;
                }
                num_elem--;
                if (num_elem == 0) {
                    break;
                }
            }
            if (e < epbkt && num_elem > 0) {
                /* Encountered an in-use element in this bucket. */
                break;
            }
            if (num_elem == 0) {
                break;
            }
            /*
             * Initialize bucket entry offset index to the MSB element in the
             * bucket.
             */
            eb_off = (epbkt - 1);
        }

        if (num_elem == 0) {
            /* Populate function output values. */
            op->base_bkt = (eb + op->offset - rbkt_cnt + 1);
            op->base_idx = e;
            op->end_bkt = (eb + op->offset);
            op->end_idx = ebl;
            op->num_bkt = rbkt_cnt;
            SHR_IF_ERR_EXIT
                (shr_itbm_bkt_list_edata_update(unit, edata, op));
            SHR_EXIT();
        }
    }
    /*
     * If there are free elements in the list but a block of contiguous free
     * elements was not found of the requested block size, then return
     * resource error. If there are no free elements in the list, then return
     * table full error.
     */
    if (op->lattr->stat.free_count) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else {
        SHR_ERR_EXIT(SHR_E_FULL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_elems_alloc_fwd(int unit,
                                  shr_itbm_list_op_t *op)
{
    uint32_t f = SHR_ITBM_INVALID; /* First element number of the list. */
    uint32_t l = SHR_ITBM_INVALID; /* Last element numbers of the list. */
    uint32_t epbkt = 0; /* Entries per-bucket count. */
    uint32_t bb = SHR_ITBM_INVALID; /* Base bucket number. */
    uint32_t be_off = SHR_ITBM_INVALID; /* Bucket entry offset index. */
    uint32_t bbf = SHR_ITBM_INVALID; /* Base bucket blk first element. */
    uint32_t e = SHR_ITBM_INVALID; /* Entry offset in base bucket. */
    uint32_t cbkt = SHR_ITBM_INVALID; /* Current bucket iterator variable. */
    uint32_t num_elem = 0; /* Number of elements to alloc. */
    uint32_t rbkt_cnt = 0; /* Required number of buckets. */
    int count = 0; /* Number of bits set in the specified range. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->lattr, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    /* Initialize the nubmer of buckets used return value. */
    edata = op->linfo->edata;
    f = op->params.first - op->offset;
    l = op->params.last - op->offset;
    epbkt = op->params.entries_per_bucket;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[LID=%u loper=0x%x]: (U=%-2u T=%-5u "
                              "comp_id=%-2u f=%-5u l=%-5u epbkt=%-2u "
                              "num_elem=%-5u zoff=%-5u).\n"),
                   op->linfo->list_id, op->loper[0], op->params.unit,
                   op->params.table_id, op->params.comp_id, f + op->offset,
                   l + op->offset, epbkt, op->num_elem, op->offset));
    }

    for (bb = f; bb <= l; bb++) {
        /*
         * Initialize base bucket, entry offset index in bucket and bucket bits
         * set count variables.
         */
        bbf = be_off = count = 0;
        rbkt_cnt = 0;
        /*
         * "num_elem" value is used to determine if the alloc has been completed
         * successfully.
         */
        num_elem = op->num_elem;
        /*
         * Skip in-use bucket with mismatch in bucket capacity or if the
         * in-use bucket is full.
         */
        if ((edata[bb].eref_cnt > 0 && edata[bb].max_entries != epbkt)
            || (edata[bb].eref_cnt > 0 && edata[bb].eref_cnt == epbkt)) {
            continue;
        }
        /*
         * Following statements determine if an in-use block's free elements can
         * be used for the new block alloc.
         */
        if (edata[bb].eref_cnt > 0 && edata[bb].max_entries == epbkt
            && edata[bb].eref_cnt < epbkt) {
            /*
             * For top to bottom blocks alloc direction, confirm that the in-use
             * elements are at lower indices of the bucket by checking the ebmp
             * bit positions from bit 0 to eref_cnt. If contiguous free elements
             * are not found within this range, then skip this bucket.
             *
             * "count" - is output parameter value in below macro and indicates
             * how many bits are set.
             */
            SHR_NULL_CHECK(edata[bb].ebmp, SHR_E_INTERNAL);
            SHR_BITCOUNT_RANGE(edata[bb].ebmp, count, 0, edata[bb].eref_cnt);
            if (count != edata[bb].eref_cnt) {
                /*
                 * Free elements not at the MSB index/indices of this bucket,
                 * so skip this bucket.
                 */
                continue;
            }
            /*
             * Use the first free element in the bucket as the first element
             * for the new block allocation.
             */
            bbf = be_off = edata[bb].eref_cnt;
        }

        /*
         * Determine number of buckets required to complete this alloc
         * request, based on the free elements available in the base bucket (bb)
         * and additional buckets required to complete the block alloc.
         */
        rbkt_cnt = shr_itbm_list_blk_num_bkts_get(num_elem,
                                                  edata[bb].eref_cnt,
                                                  epbkt,
                                                  op->params.reverse);
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Reqbkts-(rev=%s): rbkt_cnt=%u - (epbkt=%u "
                                  "bb=0x%x bbf=%u ne=%u).\n"),
                       op->params.reverse ? "T" : "F", rbkt_cnt, epbkt, bb, bbf,
                       num_elem));
        }

        for (cbkt = bb; cbkt < (bb + rbkt_cnt) && cbkt <= l; cbkt++) {
            for (e = be_off; e < epbkt && num_elem > 0; e++) {
                SHR_NULL_CHECK(edata[cbkt].ebmp, SHR_E_INTERNAL);
                if (SHR_BITGET(edata[cbkt].ebmp, e)) {
                    break;
                }
                num_elem--;
            }
            if (e != epbkt && num_elem > 0) {
                /* Encountered an in-use element in this bucket. */
                break;
            }
            be_off = 0;
        }

        if (num_elem == 0) {
            /* Populate function output values. */
            op->base_bkt = (bb + op->offset);
            op->base_idx = bbf;
            op->end_bkt = (bb + op->offset + rbkt_cnt - 1);
            op->end_idx = (e == epbkt ? (e - 1) : e);
            op->num_bkt = rbkt_cnt;
            SHR_IF_ERR_EXIT
                (shr_itbm_bkt_list_edata_update(unit, edata, op));
            SHR_EXIT();
        }
    }
    /*
     * If there are free elements in the list but a block of contiguous free
     * elements was not found of the requested block size, then return
     * resource error. If there are no free elements in the list, then return
     * table full error.
     */
    if (op->lattr->stat.free_count) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else {
        SHR_ERR_EXIT(SHR_E_FULL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_elems_alloc(int unit,
                              shr_itbm_list_op_t *op)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->lattr, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    /* This function must be called only for indexed bkts style lists. */
    if (op->params.buckets == FALSE
        || SHR_BITGET(op->linfo->lprop, SHR_ITBM_LIST_BUCKETS) == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate list operation input parameters. */
    if (SHR_BITGET(op->linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)
        && !op->offset) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->offset=%u\" value -"
                                        " rv=%s.\n"),
                             op->offset,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Validate "num_elem" value, it must be non-zero for alloc calls. */
    if (op->num_elem == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->num_elem=%u\" value -"
                                        " rv=%s.\n"),
                             op->num_elem,
                             shr_errmsg(SHR_E_INTERNAL)));
    }
    /* Check if the alloc must be done in forward or reverse direction. */
    if (op->params.reverse) {
        SHR_IF_ERR_EXIT
            (shr_itbm_bkt_list_elems_alloc_rev(unit, op));
    } else {
        SHR_IF_ERR_EXIT
            (shr_itbm_bkt_list_elems_alloc_fwd(unit, op));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_elems_alloc_rev(int unit,
                              shr_itbm_list_op_t *op)
{
    uint32_t f, l; /* First and last element numbers of the list. */
    uint32_t e; /* Blk first element number. */
    uint32_t idx; /* Index iterator variable. */
    uint32_t num_elem; /* Number of elements to alloc. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->lattr, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    edata = op->linfo->edata;
    num_elem = op->num_elem;
    f = op->params.first - op->offset;
    l = op->params.last - op->offset;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[loper=0x%x]: (U=%-2u T=%-5u comp_id=%-2u "
                              "f=%-5u l=%-5u num_elem=%-5u zoff=%-5u).\n"),
                   op->loper[0],
                   op->params.unit,
                   op->params.table_id,
                   op->params.comp_id,
                   f + op->offset,
                   l + op->offset,
                   num_elem,
                   op->offset));
    }

    for (e = l; e <= l && e >= f; e--) {
        if (!shr_itbm_list_elem_ref_cnt_get(edata, e)) {
            /*
             * Starting from the current free element (e), check if
             * "num_elem" number of elements are free to satisfy this block
             * alloc request.
             */
            for (idx = e;
                 idx > (e - num_elem) && idx <= l && idx >= f;
                 idx--) {
                if (shr_itbm_list_elem_ref_cnt_get(edata, idx)) {
                    break;
                }
            }
            if (idx == (e - num_elem)) {
                op->base_idx = (e - num_elem) + 1 + op->offset;
                op->num_elem = num_elem;
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_edata_update(unit, edata, op));
                SHR_EXIT();
            }
        }
    }

    /*
     * If there are free elements in the list but a block of contiguous free
     * elements was not found of the requested block size, then return
     * resource error. If there are no free elements in the list, then return
     * table full error.
     */
    /* Get the list statistics. */
    if (op->lattr->stat.free_count) {
        SHR_IF_ERR_EXIT(SHR_E_RESOURCE);
    } else {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_elems_alloc_fwd(int unit,
                              shr_itbm_list_op_t *op)
{
    uint32_t f, l; /* First and last element numbers of the list. */
    uint32_t e; /* Blk first element number. */
    uint32_t idx; /* Index iterator variable. */
    uint32_t num_elem; /* Number of elements to alloc. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->lattr, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    edata = op->linfo->edata;
    num_elem = op->num_elem;
    f = op->params.first - op->offset;
    l = op->params.last - op->offset;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[loper=0x%x]: (U=%-2u T=%-5u comp_id=%-2u "
                              "f=%-5u l=%-5u num_elem=%-5u zoff=%-5u).\n"),
                   op->loper[0],
                   op->params.unit,
                   op->params.table_id,
                   op->params.comp_id,
                   f + op->offset,
                   l + op->offset,
                   num_elem,
                   op->offset));
    }

    for (e = f; e <= l;) {
        if (!shr_itbm_list_elem_ref_cnt_get(edata, e)) {
            /*
             * Starting from the current free element (e), check if
             * "num_elem" number of elements are free to satisfy this block
             * alloc request.
             */
            for (idx = e;
                 idx < (e + num_elem) && idx <= l;
                 idx++) {
                if (shr_itbm_list_elem_ref_cnt_get(edata, idx)) {
                    break;
                }
            }
            if (idx == (e + num_elem)) {
                op->base_idx = e + op->offset;
                op->num_elem = num_elem;
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_edata_update(unit, edata, op));
                SHR_EXIT();
            }
        }
        /*
         * Increment "e" value based on list "block_stat" status and
         * element's block flag settings.
         */
        if (op->params.block_stat
            && shr_itbm_list_elem_ref_cnt_get(edata, e)
            && edata[e].eprop != NULL
            && SHR_BITGET(edata[e].eprop, SHR_ITBM_LIST_BLK_FIRST_ELEM)) {
            e += edata[e].blk_sz[SHR_ITBM_LIST_BKT_NONE];
        } else {
            e++;
        }
    }

    /*
     * If there are free elements in the list but a block of contiguous free
     * elements was not found of the requested block size, then return
     * resource error. If there are no free elements in the list, then return
     * table full error.
     */
    if (op->lattr->stat.free_count) {
        SHR_IF_ERR_EXIT(SHR_E_RESOURCE);
    } else {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_elems_alloc(int unit,
                          shr_itbm_list_op_t *op)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->lattr, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    /* Validate list operation input parameters. */
    if (SHR_BITGET(op->linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)
        && !op->offset) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->offset=%u\" value - "
                                        "rv=%s.\n"),
                             op->offset,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Validate "num_elem" value, it must be non-zero for alloc calls. */
    if (op->num_elem == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->num_elem=%u\" value -"
                                        " rv=%s.\n"),
                             op->num_elem,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Check list block alloc direction. */
    if (op->params.reverse) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_elems_alloc_rev(unit, op));
    } else {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_elems_alloc_fwd(unit, op));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_elems_free(int unit,
                             shr_itbm_list_op_t *op)
{
    uint32_t f = SHR_ITBM_INVALID; /* First element number of the list. */
    uint32_t l = SHR_ITBM_INVALID; /* Last element numbers of the list. */
    uint32_t epbkt = 0; /* Entries per-bucket count. */
    uint32_t bb = SHR_ITBM_INVALID; /* Base bucket number. */
    uint32_t bbf = SHR_ITBM_INVALID; /* Base bucket blk first element. */
    uint32_t e = SHR_ITBM_INVALID; /* Entry offset in the bucket. */
    uint32_t b = SHR_ITBM_INVALID; /* Current bucket iterator variable. */
    uint32_t num_elem; /* Number of elements to alloc. */
    uint32_t bkt_cnt = 0; /* Number of buckets occupied by the block. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    /* This function must be called only for indexed bkts style lists. */
    if (op->params.buckets == FALSE
        || SHR_BITGET(op->linfo->lprop, SHR_ITBM_LIST_BUCKETS) == 0
        || op->base_bkt == SHR_ITBM_INVALID) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate list operation input parameters. */
    if (SHR_BITGET(op->linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)
        && !op->offset) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->offset=%u\" value - "
                                        "rv=%s.\n"),
                             op->offset,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Validate "num_elem" value, it must be non-zero for alloc calls. */
    if (op->num_elem == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->num_elem=%u\" "
                                        "value - rv=%s.\n"),
                             op->num_elem,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    edata = op->linfo->edata;
    f = op->params.first - op->offset;
    l = op->params.last - op->offset;
    bb = op->base_bkt - op->offset;
    bbf = op->base_idx;
    epbkt = op->params.entries_per_bucket;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[loper=0x%x]: (U=%-2u T=%-5u comp_id=%-2u "
                              "f=%-5u l=%-5u zoff=%-5u epbkt=%-2u bb=0x%-5u "
                              "bbf=0x%-2u num_elem=%-5u).\n"),
                   op->loper[0],
                   op->params.unit,
                   op->params.table_id,
                   op->params.comp_id,
                   f + op->offset,
                   l + op->offset,
                   op->offset,
                   epbkt,
                   bb,
                   bbf,
                   op->num_elem));
    }
    /*
     * Validate bucket number value is this within the first and last elements
     * range of the list.
     */
    if (bb < f || bb > l) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid \"bucket=%u\" [Valid range: "
                                        "%u To %u] - rv=%s.\n"),
                             bb,
                             f,
                             l,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /*
     * "num_elem" value is used to determine if the alloc has been completed
     * successfully.
     */
    num_elem = op->num_elem;
    bkt_cnt = shr_itbm_list_blk_num_bkts_get(num_elem,
                                             bbf,
                                             epbkt,
                                             FALSE);
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "BktsToFree: bkt_cnt=%u - (epbkt=%u bb=0x%x "
                              "bbf=%u num_elem=%u).\n"),
                   bkt_cnt,
                   epbkt,
                   bb,
                   bbf,
                   num_elem));
    }

    for (b = bb; b < (bb + bkt_cnt); b++) {
        if (edata[b].eref_cnt > 0 && edata[b].max_entries != epbkt) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Input bucket(=%u) capacity "
                                            "mismatch exp/act=\"%u\"/\"%u\""
                                            " - rv=%s.\n"),
                                 b,
                                 epbkt,
                                 edata[b].max_entries,
                                 shr_errmsg(SHR_E_PARAM)));
        }
        for (e = bbf; e < epbkt && num_elem > 0; e++) {
            if (SHR_BITGET(edata[b].ebmp, e) == 0) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                                    (BSL_META_U(unit,
                                                "List \"bkt=%u elem=%u\" "
                                                "not in-use by comp_id=%u - "
                                                "rv=%s.\n"),
                                     b,
                                     e,
                                     op->params.comp_id,
                                     shr_errmsg(SHR_E_NOT_FOUND)));
            }
            num_elem--;
        }
        bbf = 0;
    }
    if (num_elem == 0) {
        /* Populate function output parameter values. */
        op->end_bkt = (op->base_bkt + bkt_cnt - 1);
        op->end_idx = (e == epbkt ? e - 1 : e );
        op->num_bkt = bkt_cnt;
        SHR_IF_ERR_EXIT
            (shr_itbm_bkt_list_edata_update(unit, edata, op));
        SHR_EXIT();
    }
    /* Should never hit this condition. */
    SHR_ERR_EXIT(SHR_E_INTERNAL);
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_elems_free(int unit,
                         shr_itbm_list_op_t *op)
{
    uint32_t f, l; /* First and last element numbers of the list. */
    uint32_t blk_fidx; /* Blk first element number. */
    uint32_t idx; /* Index iterator variable. */
    uint32_t num_elem; /* Number of elements to alloc. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    /* Validate list operation input parameters. */
    if (SHR_BITGET(op->linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)
        && !op->offset) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->offset=%u\" value.\n"),
                             op->offset));
    }

    /* Validate "num_elem" value, it must be non-zero for free calls. */
    if (op->num_elem == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->num_elem=%u\" value.\n"),
                             op->num_elem));
    }

    edata = op->linfo->edata;
    num_elem = op->num_elem;
    blk_fidx = op->base_idx - op->offset;
    f = op->params.first - op->offset;
    l = op->params.last - op->offset;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[loper=0x%x base_idx=%-5u]: (U=%-2u T=%-5u "
                              "comp_id=%-2u f=%-5u l=%-5u num_elem=%-5u "
                              "zoff=%-5u).\n"),
                   op->loper[0],
                   op->base_idx,
                   op->params.unit,
                   op->params.table_id,
                   op->params.comp_id,
                   f + op->offset,
                   l + op->offset,
                   num_elem,
                   op->offset));
    }

    /*
     * Validate blk_fidx element index value is this within the first and
     * last elements range of the list.
     */
    if (blk_fidx < f || blk_fidx > l) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"blk_fidx=%u\" [valid "
                                        "range: %u To %u] - rv=%s.\n"),
                             blk_fidx,
                             f,
                             l,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    for (idx = blk_fidx;
         idx < (blk_fidx + num_elem) && idx <= l;
         idx++) {
        if (!shr_itbm_list_elem_ref_cnt_get(edata, idx)) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                                (BSL_META_U(unit,
                                            "List \"elem=%u\" not in-use - "
                                            "rv=%s.\n"),
                                 idx,
                                 shr_errmsg(SHR_E_NOT_FOUND)));
        }
    }
    if (idx == (blk_fidx + num_elem)) {
        /* Set the block base index value. */
        op->base_idx = (blk_fidx + op->offset);
        SHR_IF_ERR_EXIT
            (shr_itbm_list_edata_update(unit, edata, op));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_block_move(int unit,
                             shr_itbm_list_hdl_t hdl,
                             uint32_t sblk_bkt,
                             uint32_t sblk_first,
                             uint32_t dblk_bkt,
                             uint32_t dblk_first,
                             uint32_t ecount)
{
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_list_op_t op; /* List operation structure. */
    uint32_t end_bkt = 0; /* Block end bucket number. */
    uint8_t end_idx = 0; /* Block end bucket element local index. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);

    /*
     * Initialize the list info and list element data local pointer
     * variables.
     */
    linfo = hdl->linfo;
    edata = linfo->edata;

    /*
     * Increment the dblk elements reference count, with an alloc call to edata
     * update function.
     */
    SHR_IF_ERR_EXIT
        (shr_itbm_bkt_list_blk_end_get(unit,
                                       dblk_bkt,
                                       dblk_first,
                                       ecount,
                                       hdl->params.entries_per_bucket,
                                       &end_bkt,
                                       &end_idx));
    /* Setup list operation input params. */
    shr_itbm_list_op_t_init(&op);
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        op.offset = hdl->params.min;
    }
    SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_ALLOC_ID);
    op.linfo = hdl->linfo;
    op.params = hdl->params;
    op.base_bkt = dblk_bkt;
    op.base_idx = dblk_first;
    op.end_bkt = end_bkt;
    op.end_idx = end_idx;
    op.num_elem = ecount;
    op.num_bkt = (end_bkt - dblk_bkt + 1);
    SHR_IF_ERR_EXIT
        (shr_itbm_bkt_list_edata_update(unit, edata, &op));
    /* Update the list STAT. */
    SHR_IF_ERR_EXIT
        (shr_itbm_bkt_list_stat_update(unit, hdl, &op));
    /*
     * Decrement the sblk elements reference count, with a free call to edata
     * update function.
     */
    SHR_IF_ERR_EXIT
        (shr_itbm_bkt_list_blk_end_get(unit,
                                       sblk_bkt,
                                       sblk_first,
                                       ecount,
                                       hdl->params.entries_per_bucket,
                                       &end_bkt,
                                       &end_idx));
    shr_itbm_list_op_t_init(&op);
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        op.offset = hdl->params.min;
    }
    SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_FREE);
    op.linfo = hdl->linfo;
    op.params = hdl->params;
    op.base_bkt = sblk_bkt;
    op.base_idx = sblk_first;
    op.end_bkt = end_bkt;
    op.end_idx = end_idx;
    op.num_elem = ecount;
    op.num_bkt = (end_bkt - sblk_bkt + 1);
    SHR_IF_ERR_EXIT
        (shr_itbm_bkt_list_edata_update(unit, edata, &op));
    /* Update the list STAT. */
    SHR_IF_ERR_EXIT
        (shr_itbm_bkt_list_stat_update(unit, hdl, &op));
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_block_move(int unit,
                         shr_itbm_list_hdl_t hdl,
                         uint32_t sblk_first,
                         uint32_t dblk_first,
                         uint32_t ecount)
{
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_list_op_t op; /* List operation structure. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);

    /*
     * Initialize the list info and list element data local pointer
     * variables.
     */
    linfo = hdl->linfo;
    edata = linfo->edata;

    /*
     * Increment the dblk elements reference count, with an alloc call to edata
     * update function.
     */

    /* Setup list operation input params. */
    shr_itbm_list_op_t_init(&op);
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        op.offset = hdl->params.min;
    }
    SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_ALLOC_ID);
    op.linfo = hdl->linfo;
    op.params = hdl->params;
    op.base_idx = dblk_first;
    op.num_elem = ecount;
    SHR_IF_ERR_EXIT
        (shr_itbm_list_edata_update(unit, edata, &op));
    /*
     * Decrement the sblk elements reference count, with a free call to edata
     * update function.
     */
    shr_itbm_list_op_t_init(&op);
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        op.offset = hdl->params.min;
    }
    SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_FREE);
    op.linfo = hdl->linfo;
    op.params = hdl->params;
    op.base_idx = sblk_first;
    op.num_elem = ecount;
    SHR_IF_ERR_EXIT
        (shr_itbm_list_edata_update(unit, edata, &op));
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_blk_dseq_update(int unit,
                                  shr_itbm_list_hdl_t hdl,
                                  uint32_t tot_inuse_cnt,
                                  uint32_t index,
                                  uint32_t sblk_bkt,
                                  uint32_t sblk_first,
                                  uint32_t dblk_bkt,
                                  uint32_t dblk_first,
                                  uint32_t ecount)
{
    shr_itbm_defrag_seq_t *dseq = NULL; /* Block defragmentation sequence. */
    shr_itbm_blk_move_info_t *minfo = NULL; /* Pointer to block move info. */
    shr_itbm_blk_move_info_t *new = NULL; /* Pointer to block move info. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->dseq, SHR_E_PARAM);
    if (index > tot_inuse_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "\"index=%u > tot_inuse_cnt=%u\""
                                        " - rv=%s.\n"),
                             index,
                             tot_inuse_cnt,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Get the block defragementation sequence pointer. */
    dseq = hdl->linfo->dseq[index];
    /*
     * If this is the first move sequence for this block, then allocate the
     * head node memory and set the move details.
     */
    if (dseq == NULL) {
        /* Allocate memory to store block defragmentation sequence. */
        SHR_ALLOC(hdl->linfo->dseq[index], sizeof(*dseq), "shritbmBlkDfgSeq");
        SHR_NULL_CHECK(hdl->linfo->dseq[index], SHR_E_MEMORY);
        sal_memset(hdl->linfo->dseq[index], 0, sizeof(*dseq));

        dseq = hdl->linfo->dseq[index];
        dseq->ecount = ecount;
        dseq->mcount++;

        /* Allocate head node memory. */
        SHR_ALLOC(new, sizeof(*new), "shritbmBlkMovInfo");
        SHR_NULL_CHECK(new, SHR_E_MEMORY);
        sal_memset(new, 0, sizeof(*new));
        /* Update the move details. */
        new->sbucket = sblk_bkt;
        new->sfirst_elem = sblk_first;
        new->dbucket = dblk_bkt;
        new->dfirst_elem = dblk_first;
        dseq->minfo = new;
        SHR_EXIT();
    }

    /* Get the list head node pointer. */
    minfo = dseq->minfo;;
    /* Append the new block move sequence. */
    while (minfo) {
        if (minfo->next == NULL) {
            /*
             * Allocate memory to store the new input params for an
             * existing list.
             */
            SHR_ALLOC(new, sizeof(*new), "shritbmBlkMovInfo");
            SHR_NULL_CHECK(new, SHR_E_MEMORY);
            sal_memset(new, 0, sizeof(*new));

            new->sbucket = sblk_bkt;
            new->sfirst_elem = sblk_first;
            new->dbucket = dblk_bkt;
            new->dfirst_elem = dblk_first;
            minfo->next = new;
            /* Increment the block move count. */
            dseq->mcount++;
            break;
        } else {
            minfo = minfo->next;
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        /* Free allocated memory for error return case. */
        SHR_FREE(dseq);
    }
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_blk_dseq_update(int unit,
                              shr_itbm_list_hdl_t hdl,
                              uint32_t tot_inuse_cnt,
                              uint32_t index,
                              uint32_t sblk_first,
                              uint32_t dblk_first,
                              uint32_t ecount)
{
    shr_itbm_defrag_seq_t *dseq = NULL; /* Block defragmentation sequence. */
    shr_itbm_blk_move_info_t *minfo = NULL; /* Pointer to block move info. */
    shr_itbm_blk_move_info_t *new = NULL; /* Pointer to block move info. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->dseq, SHR_E_PARAM);
    if (index > tot_inuse_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "\"index=%u > tot_inuse_cnt=%u\""
                                        " - rv=%s.\n"),
                             index,
                             tot_inuse_cnt,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Get the block defragementation sequence pointer. */
    dseq = hdl->linfo->dseq[index];
    /*
     * If this is the first move sequence for this block, then allocate the
     * head node memory and set the move details.
     */
    if (dseq == NULL) {
        /* Allocate memory to store block defragmentation sequence. */
        SHR_ALLOC(hdl->linfo->dseq[index], sizeof(*dseq), "shritbmBlkDfgSeq");
        SHR_NULL_CHECK(hdl->linfo->dseq[index], SHR_E_MEMORY);
        sal_memset(hdl->linfo->dseq[index], 0, sizeof(*dseq));
        dseq = hdl->linfo->dseq[index];
        dseq->ecount = ecount;
        dseq->mcount++;

        /* Allocate head node memory. */
        SHR_ALLOC(new, sizeof(*new), "shritbmBlkMovInfo");
        SHR_NULL_CHECK(new, SHR_E_MEMORY);
        sal_memset(new, 0, sizeof(*new));

        /* Update the move details. */
        new->sbucket = SHR_ITBM_INVALID;
        new->sfirst_elem = sblk_first;
        new->dbucket = SHR_ITBM_INVALID;
        new->dfirst_elem = dblk_first;
        dseq->minfo = new;
        SHR_EXIT();
    }

    /* Get the list head node pointer. */
    minfo = dseq->minfo;;
    /* Append the new block move sequence. */
    while (minfo) {
        if (minfo->next == NULL) {
            /*
             * Allocate memory to store the new input params for an
             * existing list.
             */
            SHR_ALLOC(new, sizeof(*new), "shritbmBlkMovInfo");
            SHR_NULL_CHECK(new, SHR_E_MEMORY);
            sal_memset(new, 0, sizeof(*new));

            new->sbucket = SHR_ITBM_INVALID;
            new->sfirst_elem = sblk_first;
            new->dbucket = SHR_ITBM_INVALID;
            new->dfirst_elem = dblk_first;
            minfo->next = new;
            /* Increment the block move count. */
            dseq->mcount++;
            break;
        } else {
            minfo = minfo->next;
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        /* Free allocated memory for error return case. */
        SHR_FREE(dseq);
    }

    if (!SHR_FUNC_ERR() && dseq && BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[idx=%-5u mcnt=%-4u ecnt=%-5u s:b=%-5u "
                              "f=%-5u d:b=%-5u f=%-5u].\n"),
                   index,
                   dseq->mcount,
                   dseq->ecount,
                   new->sbucket,
                   new->sfirst_elem,
                   new->dbucket,
                   new->dfirst_elem));
    }
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_dseq_cleanup(int unit,
                           shr_itbm_list_hdl_t hdl,
                           uint32_t tot_inuse_cnt)
{
    uint32_t bidx; /* Block index iterator variable. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_blk_move_info_t *minfo = NULL; /* Block move info. */
    shr_itbm_blk_move_info_t *del = NULL; /* Block move info. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);

    /* Initialize the list info pointer. */
    linfo = hdl->linfo;
    SHR_NULL_CHECK(linfo->dseq, SHR_E_PARAM);

    for (bidx = 0; bidx < tot_inuse_cnt; bidx++) {
        if (linfo->dseq[bidx] != NULL && linfo->dseq[bidx]->mcount > 0) {
            minfo = linfo->dseq[bidx]->minfo;
            SHR_NULL_CHECK(minfo, SHR_E_INTERNAL);
            while (minfo->next) {
                del = minfo->next;
                minfo->next = del->next;
                SHR_FREE(del);
                linfo->dseq[bidx]->mcount--;
            }
            /* Free the head node memory. */
            if (linfo->dseq[bidx]->mcount != 1) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_FREE(minfo);
        }
        /* Free the dseq pointer memory. */
        SHR_FREE(linfo->dseq[bidx]);
    }
    SHR_FREE(linfo->dseq);
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Done: [\"LID=%-4u\" and \"comp_id=%-4u\"].\n"),
                     hdl->linfo->list_id,
                     hdl->params.comp_id));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_elems_alloc_id(int unit,
                                 shr_itbm_list_op_t *op)
{
    uint32_t f = SHR_ITBM_INVALID; /* First element number of the list. */
    uint32_t l = SHR_ITBM_INVALID; /* Last element numbers of the list. */
    uint32_t epbkt = 0; /* Entries per-bucket count. */
    uint32_t bb = SHR_ITBM_INVALID; /* Base bucket number. */
    uint32_t bbf = SHR_ITBM_INVALID; /* Base bucket blk first element. */
    uint32_t e = SHR_ITBM_INVALID; /* Entry offset in the bucket. */
    uint32_t b = SHR_ITBM_INVALID; /* Current bucket iterator variable. */
    uint32_t num_elem; /* Number of elements to alloc. */
    uint32_t rbkt_cnt = 0; /* Required number of buckets. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    /* This function must be called only for indexed bkts style lists. */
    if (op->params.buckets == FALSE
        || SHR_BITGET(op->linfo->lprop, SHR_ITBM_LIST_BUCKETS) == 0
        || op->base_bkt == SHR_ITBM_INVALID) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "buckets=%u list param is disabled -"
                                        " rv=%s.\n"),
                             op->params.buckets,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Validate list operation input parameters. */
    if (SHR_BITGET(op->linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)
        && !op->offset) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->offset=%u\" value -"
                                        " rv=%s.\n"),
                             op->offset,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Validate "num_elem" value, it must be non-zero for alloc calls. */
    if (op->num_elem == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->num_elem=%u\" "
                                        "value - rv=%s.\n"),
                             op->num_elem,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    edata = op->linfo->edata;
    f = op->params.first - op->offset;
    l = op->params.last - op->offset;
    bb = op->base_bkt - op->offset;
    bbf = op->base_idx;
    epbkt = op->params.entries_per_bucket;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[loper=0x%x]: (U=%-2u T=%-5u comp_id=%-2u "
                              "f=%-5u l=%-5u num_elem=%-5u zoff=%-5u).\n"),
                   op->loper[0],
                   op->params.unit,
                   op->params.table_id,
                   op->params.comp_id,
                   f + op->offset,
                   l + op->offset,
                   op->num_elem,
                   op->offset));
    }
    /*
     * Validate bucket number value is this within the first and last elements
     * range of the list.
     */
    if (bb < f || bb > l) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid \"bucket=%u\", valid "
                                        "range is \"%u to %u\" - rv=%s.\n"),
                             bb,
                             f,
                             l,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /*
     * "num_elem" value is used to determine if the alloc has been completed
     * successfully.
     */
    num_elem = op->num_elem;
    rbkt_cnt = shr_itbm_list_blk_num_bkts_get(num_elem,
                                              bbf,
                                              epbkt,
                                              FALSE);
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "No. of bkts req: rbkt_cnt=%u - (epbkt=%u bb=%u "
                              "bbf=%u num_elem=%u).\n"),
                  rbkt_cnt,
                  epbkt,
                  bb,
                  bbf,
                  num_elem));
    }

    for (b = bb; b < (bb + rbkt_cnt); b++) {
        if (edata[b].eref_cnt > 0 && edata[b].max_entries != epbkt) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Input bkt(=%u) ecount "
                                            "mismatch exp/act=\"%u\"/\"%u\""
                                            " - rv=%s.\n"),
                                 b,
                                 epbkt,
                                 edata[b].max_entries,
                                 shr_errmsg(SHR_E_PARAM)));
        }
        /*
         * If a bucket is unused and all the elements in the bucket are free
         * and available, then no need to check for individual elements in
         * the bucket. Use this bucket for servicing the new alloc request and
         * update (decrement) the num_elem value and also set element index
         * value (e) to epbkt as all elements from this bucket are now taken.
         */
        if (edata[b].eref_cnt == 0 && num_elem >= epbkt) {
            num_elem -= epbkt;
            e = epbkt;
            continue;
        }
        for (e = bbf; e < epbkt && num_elem > 0; e++) {
            if (SHR_BITGET(edata[b].ebmp, e)) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_EXISTS,
                                    (BSL_META_U(unit,
                                                "List \"bkt=%u elem=%u\" "
                                                "already in-use by "
                                                "\"comp_id=%u\" - rv=%s.\n"),
                                     b,
                                     e,
                                     edata[b].comp_id[e],
                                     shr_errmsg(SHR_E_EXISTS)));
            }
            num_elem--;
        }
        bbf = 0;
    }
    if (num_elem == 0) {
        /* Populate function output parameter values. */
        op->end_bkt = (op->base_bkt + rbkt_cnt - 1);
        op->end_idx = (e == epbkt ? e - 1 : e);
        op->num_bkt = rbkt_cnt;
        SHR_IF_ERR_EXIT
            (shr_itbm_bkt_list_edata_update(unit, edata, op));
        SHR_EXIT();
    }
    /* Should never hit this condition. */
    SHR_ERR_EXIT(SHR_E_INTERNAL);
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_elems_alloc_id(int unit,
                             shr_itbm_list_op_t *op)
{
    uint32_t f, l; /* First and last element numbers of the list. */
    uint32_t blk_fidx; /* Blk first element number. */
    uint32_t idx; /* Index iterator variable. */
    uint32_t num_elem; /* Number of elements to alloc. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(op->linfo->edata, SHR_E_PARAM);

    /* Validate list operation input parameters. */
    if (SHR_BITGET(op->linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)
        && !op->offset) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->offset=%u\" "
                                        "value - rv=%s.\n"),
                             op->offset,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Validate "num_elem" value, it must be non-zero for alloc calls. */
    if (op->num_elem == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"op->num_elem=%u\" "
                                        "value - rv=%s.\n"),
                             op->num_elem,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    edata = op->linfo->edata;
    num_elem = op->num_elem;
    blk_fidx = op->base_idx - op->offset;
    f = op->params.first - op->offset;
    l = op->params.last - op->offset;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[LID=%u loper=0x%x base_idx=%-5u]: (U=%-2u "
                              "T=%-5u comp_id=%-2u f=%-5u l=%-5u num_elem=%-5u "
                              "zoff=%-5u).\n"),
                   op->linfo->list_id, op->loper[0], op->base_idx,
                   op->params.unit, op->params.table_id, op->params.comp_id,
                   f + op->offset, l + op->offset, num_elem, op->offset));
    }
    /*
     * Validate blk_fidx element index value is this within the first and
     * last elements range of the list.
     */
    if (blk_fidx < f || blk_fidx > l) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid \"blk_fidx=%u\" "
                                        "[Valid range: %u To %u] - rv=%s.\n"),
                             blk_fidx,
                             f,
                             l,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    for (idx = blk_fidx;
         idx < (blk_fidx + num_elem) && idx <= l;
         idx++) {
        if (shr_itbm_list_elem_ref_cnt_get(edata, idx)) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_EXISTS,
                                (BSL_META_U(unit,
                                            "List \"elem=%u\" requested by "
                                            "\"comp_id=%u\" already in-use by "
                                            "\"comp_id=%u\" - rv=%s.\n"),
                                 idx,
                                 op->params.comp_id,
                                 edata[idx].comp_id[SHR_ITBM_LIST_BKT_NONE],
                                 shr_errmsg(SHR_E_EXISTS)));
        }
    }
    if (idx == (blk_fidx + num_elem)) {
        /* Set the block base index value to update the element details. */
        op->base_idx = (blk_fidx + op->offset);
        SHR_IF_ERR_EXIT
            (shr_itbm_list_edata_update(unit, edata, op));
        SHR_EXIT();
    }
    SHR_ERR_EXIT(SHR_E_INTERNAL);
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_inuse_blks_get(int unit,
                                 shr_itbm_list_hdl_t hdl,
                                 uint32_t tot_blks_alloced,
                                 shr_itbm_blk_t *ablk_arr,
                                 uint32_t *tot_inuse_cnt,
                                 uint32_t *comp_blks_cnt,
                                 uint32_t *comp_max_ecount)
{
    uint32_t f, l; /* First and last element numbers of the list. */
    uint32_t b = 0, e = 0; /* Index iterator variable. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */
    uint32_t zoff = 0;     /* Zero based index offset for edata array. */
    bool lshrd_part = FALSE; /* List is shared and index partitioned. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(ablk_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(tot_inuse_cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(comp_blks_cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(comp_max_ecount, SHR_E_PARAM);

    /* This function is supported only for block_stat enabled lists. */
    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "\"block_stat\" is disabled - rv=%s."
                                        "\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }
    /*
     * This function must be called with the total number of block in-use
     * count to retrieve the block details. If this value is zero, then return
     * input parameter error.
     */
    if (!tot_blks_alloced) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Determine the offset for computing zero based list bucket index value. */
    if (SHR_BITGET(hdl->linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        zoff = hdl->params.min;
    }

    edata = hdl->linfo->edata;
    f = hdl->params.first - zoff;
    l = hdl->params.last - zoff;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "(U=%-2u T=%-5u comp_id=%-2u f=%-5u l=%-5u "
                              "zoff=%-5u ablk_arr[arr_sz=%-5u]).\n"),
                   hdl->params.unit,
                   hdl->params.table_id,
                   hdl->params.comp_id,
                   f + zoff,
                   l + zoff,
                   zoff,
                   tot_blks_alloced));
    }

    /* Initialize the function output parameter values. */
    *tot_inuse_cnt = 0;
    *comp_blks_cnt = 0;
    *comp_max_ecount = 0;

    /*
     * Iterate from the first to last index of the table and check for
     * elements that are in-use and are also the first element of the
     * allocated block.
     */
    for (b = f; b <= l && (*tot_inuse_cnt < tot_blks_alloced); b++) {
        if (shr_itbm_list_elem_ref_cnt_get(edata, b)) {
            for (e = 0; e < edata[b].max_entries; e++) {
                /*
                 * Check if the first element property flag is set for this
                 * element.
                 */
                if (edata[b].eprop != NULL && SHR_BITGET(edata[b].eprop, e)) {
                    /*
                     * Adjust bucket value for zero based indexing, if min
                     * bucket index is non-zero.
                     */
                    ablk_arr[*tot_inuse_cnt].entries_per_bucket =
                                                        edata[b].max_entries;
                    ablk_arr[*tot_inuse_cnt].bucket = (b + zoff);
                    ablk_arr[*tot_inuse_cnt].first_elem = e;
                    ablk_arr[*tot_inuse_cnt].comp_id = edata[b].comp_id[e];
                    ablk_arr[*tot_inuse_cnt].ecount = edata[b].blk_sz[e];
                    if (edata[b].comp_id[e] == hdl->params.comp_id) {
                        /* Update block comp_id match count. */
                        *comp_blks_cnt += 1;
                        /*
                         * Check and update the maximum block size used by the
                         * component value.
                         */
                        if (ablk_arr[*tot_inuse_cnt].ecount >
                            *comp_max_ecount) {
                            *comp_max_ecount = ablk_arr[*tot_inuse_cnt].ecount;
                        }
                    }
                    /* Update total in-use blocks count. */
                    *tot_inuse_cnt += 1;
                }
            }
        }
    }

    if (SHR_BITGET(hdl->linfo->lprop, SHR_ITBM_LIST_SHARED)
        && SHR_BITGET(hdl->linfo->lprop, SHR_ITBM_LIST_PART)) {
        lshrd_part = TRUE;
    }
    /*
     * Total blocks allocated count must match the tot_inuse_cnt value for
     * lists that are not shared and paritioned.
     */
    if (*tot_inuse_cnt != tot_blks_alloced && lshrd_part == FALSE) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "[tot_inuse_cnt(%u) != "
                                        "tot_blks_alloced(%u)] - rv=%s.\n"),
                             *tot_inuse_cnt,
                             tot_blks_alloced,
                             shr_errmsg(SHR_E_INTERNAL)));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_inuse_blks_get(int unit,
                             shr_itbm_list_hdl_t hdl,
                             uint32_t tot_blks_alloced,
                             shr_itbm_blk_t *ablk_arr,
                             uint32_t *tot_inuse_cnt,
                             uint32_t *comp_blks_cnt,
                             uint32_t *comp_max_ecount)
{
    uint32_t f, l; /* First and last element numbers of the list. */
    uint32_t idx; /* Index iterator variable. */
    shr_itbm_list_edata_t *edata; /* Pointer to list elements array. */
    uint32_t zoff = 0;     /* Zero based index offset for edata array. */
    bool lshrd_part = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(ablk_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(tot_inuse_cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(comp_blks_cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(comp_max_ecount, SHR_E_PARAM);

    /* This function is supported only for block_stat enabled lists. */
    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "\"block_stat\" is disabled - rv=%s."
                                        "\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }
    /*
     * This function should not be called when there were no blocks
     * allocated in a list.
     */
    if (!tot_blks_alloced) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_BITGET(hdl->linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        zoff = hdl->params.min;
    }

    edata = hdl->linfo->edata;
    f = hdl->params.first - zoff;
    l = hdl->params.last - zoff;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "(U=%-2u T=%-5u comp_id=%-2u f=%-5u l=%-5u "
                              "zoff=%-5u ablk_arr[arr_sz=%-5u]).\n"),
                   hdl->params.unit,
                   hdl->params.table_id,
                   hdl->params.comp_id,
                   f + zoff,
                   l + zoff,
                   zoff,
                   tot_blks_alloced));
    }

    /* Initialize the function output parameter values. */
    *tot_inuse_cnt = 0;
    *comp_blks_cnt = 0;
    *comp_max_ecount = 0;

    /*
     * Iterate from the first to last index of the table and check for
     * elements that are in-use and are also the first element of the
     * allocated block.
     */
    for (idx = f; idx <= l && (*tot_inuse_cnt < tot_blks_alloced);) {
        if (shr_itbm_list_elem_ref_cnt_get(edata, idx)
            && edata[idx].eprop != NULL
            && SHR_BITGET(edata[idx].eprop, SHR_ITBM_LIST_BLK_FIRST_ELEM)) {
            /* Update in-use block details. */
            shr_itbm_blk_t_init(&ablk_arr[*tot_inuse_cnt]);
            ablk_arr[*tot_inuse_cnt].first_elem = (idx + zoff);
            ablk_arr[*tot_inuse_cnt].comp_id =
                                    edata[idx].comp_id[SHR_ITBM_LIST_BKT_NONE];
            ablk_arr[*tot_inuse_cnt].ecount =
                edata[idx].blk_sz[SHR_ITBM_LIST_BKT_NONE];
            if (edata[idx].comp_id[SHR_ITBM_LIST_BKT_NONE]
                == hdl->params.comp_id) {
                /* Update block comp_id match count. */
                *comp_blks_cnt += 1;
                /*
                 * Check and update the maximum block size used by the
                 * component value.
                 */
                if (ablk_arr[*tot_inuse_cnt].ecount > *comp_max_ecount) {
                    *comp_max_ecount = ablk_arr[*tot_inuse_cnt].ecount;
                }
            }
            /*
             * Move the list traversal index to the first entry present
             * after the current block.
             */
            idx += ablk_arr[*tot_inuse_cnt].ecount;
            /* Update total in-use blocks count. */
            *tot_inuse_cnt += 1;
         } else {
             idx++;
         }
    }

    if (SHR_BITGET(hdl->linfo->lprop, SHR_ITBM_LIST_SHARED)
        && SHR_BITGET(hdl->linfo->lprop, SHR_ITBM_LIST_PART)) {
        lshrd_part = TRUE;
    }

    /* Total blocks allocated count must match the tot_inuse_cnt value. */
    if (*tot_inuse_cnt != tot_blks_alloced && !lshrd_part) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "[tot_inuse_cnt(%u) != "
                                        "tot_blks_alloced(%u)] - rv=%s.\n"),
                             *tot_inuse_cnt,
                             tot_blks_alloced,
                             shr_errmsg(SHR_E_INTERNAL)));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_free_blks_get(int unit,
                                shr_itbm_list_hdl_t hdl,
                                uint32_t ablks_cnt,
                                shr_itbm_blk_t *ablk_arr,
                                uint32_t *free_blks_cnt,
                                shr_itbm_blk_t **free_blks,
                                shr_itbm_blk_t *largest_fblk)
{
    uint32_t b = 0; /* Bucket index iterator variables. */
    uint32_t free_idx; /* Free blocks index, iterator variable. */
    uint32_t fblks_cnt = 0; /* Total free blocks count. */
    uint32_t ebkt = SHR_ITBM_INVALID; /* Blk end bucket. */
    uint8_t eidx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX; /* Bkt local index. */
    shr_itbm_blk_t fblk; /* Free block. */
    shr_itbm_blk_t lblk; /* Last block. */
    shr_itbm_blk_t *fblk_arr = NULL; /* Free blocks array. */
    shr_itbm_list_stat_t stat; /* List statistics. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(free_blks_cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(free_blks, SHR_E_PARAM);
    SHR_NULL_CHECK(largest_fblk, SHR_E_PARAM);

    /* This function is supported only for block_stat enabled lists. */
    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List \"block_stat\" is disabled - "
                                        "rv=%s).\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (ablks_cnt) {
        /*
         * If ablks_cnt is non-zero, then "ablk_arr" input parameter must not
         * be null. "ablk_arr" will be NULL only when no blocks from this list
         * are in-use.
         */
        SHR_NULL_CHECK(ablk_arr, SHR_E_PARAM);
    }

    /*
     * If no blocks have been allocated from the list, then the entire table
     * is a one single free block.
     */
    if (ablks_cnt == 0) {
        /* Get the list statistics. */
        SHR_IF_ERR_EXIT
            (shr_itbm_list_attr_stat_get(unit, hdl, &stat));
        /* Setup list free block details. */
        SHR_ALLOC(fblk_arr, sizeof(*fblk_arr), "shritbmFreeBlkArr");
        SHR_NULL_CHECK(fblk_arr, SHR_E_MEMORY);
        sal_memset(fblk_arr, 0, sizeof(*fblk_arr));
        shr_itbm_blk_t_init(fblk_arr);
        fblk_arr->entries_per_bucket = hdl->params.entries_per_bucket;
        fblk_arr->bucket = hdl->params.first;
        fblk_arr->first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
        fblk_arr->ecount = stat.free_count;
        fblk_arr->comp_id = SHR_ITBM_INVALID;
        *free_blks = fblk_arr;
        *free_blks_cnt = 1;
        SHR_EXIT();
    }

    /*
     * Initialize the first free block of the list as starting at the first
     * bucket (b[0]) and at the first element offset (e[0]) in this bucket.
     *
     * Example an indexed buckets list with four entries_per_bucket support:
     *      ----------------------------
     *      | e[3] | e[2] | e[1] | e[0] | b[0] -> fblk.bucket + first_elem
     *      ----------------------------
     *      | e[3] | e[2] | e[1] | e[0] | b[1]
     *      ----------------------------
     *      | e[3] | e[2] | e[1] | e[0] | b[2] -> ablk_arr[0]
     *      ----------------------------
     *      .....
     *      ----------------------------
     *      | e[3] | e[2] | e[1] | e[0] | b[n]
     *      ----------------------------
     */
    shr_itbm_blk_t_init(&fblk);
    fblk.entries_per_bucket = hdl->params.entries_per_bucket;
    fblk.bucket = hdl->params.first;
    fblk.first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
    /*
     * Iterate through the in-use blocks and find the free blocks (gaps)
     * between the in-use blocks.
     */
    for (b = 0; b < ablks_cnt; b++) {
        SHR_IF_ERR_EXIT
            (shr_itbm_bkt_list_ublk_size_get(unit, &fblk, &ablk_arr[b]));
        if (fblk.ecount) {
            /* Increment the free blocks count. */
            fblks_cnt++;
        }
        /*
         * As the free block has been accounted now, as the next step,
         * get the current allocated in-use block end bucket index and
         * it's last element bucket local index value.
         */
        SHR_IF_ERR_EXIT
            (shr_itbm_bkt_list_blk_end_get(unit,
                                           ablk_arr[b].bucket,
                                           ablk_arr[b].first_elem,
                                           ablk_arr[b].ecount,
                                           ablk_arr[b].entries_per_bucket,
                                           &ebkt,
                                           &eidx));
        /*
         * To find out the next free block gap in between the in-use blocks,
         * move the current free block bucket and bucket local element index
         * past the current ablk_arr[] element using the ebkt and eidx values
         * returned by the above function call.
         */
        if (eidx == (ablk_arr[b].entries_per_bucket - 1)
            || (hdl->params.entries_per_bucket
                != ablk_arr[b].entries_per_bucket)) {
            fblk.bucket = ebkt + 1;
            fblk.first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
        } else {
            fblk.bucket = ebkt;
            fblk.first_elem = eidx + 1;
        }
        /* Clear the free block size value. */
        fblk.ecount = 0;
    }

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "B4-lbk-check: fblks_cnt=%u\n"),
                   fblks_cnt));
    }
    /*
     * After the above for-loop, the free block's first_elem's location will
     * be the first element after the all in-use blocks. Now check if there
     * are free elements between this location and the last element of this
     * list.
     */
    shr_itbm_blk_t_init(&lblk);
    lblk.entries_per_bucket = hdl->params.entries_per_bucket;
    lblk.bucket = (hdl->params.last + 1);
    lblk.first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
    SHR_IF_ERR_EXIT
        (shr_itbm_bkt_list_ublk_size_get(unit, &fblk, &lblk));
    if (fblk.ecount) {
        /* If free elements are found, then increment the free blocks count. */
        fblks_cnt++;
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Aft-lbk-check: fblks_cnt=%u\n"),
                       fblks_cnt));
        }
    }
    if (fblks_cnt) {
        SHR_ALLOC(fblk_arr, sizeof(*fblk_arr) * fblks_cnt, "shritbmFreeBlkArr");
        SHR_NULL_CHECK(fblk_arr, SHR_E_MEMORY);
        sal_memset(fblk_arr, 0, (sizeof(*fblk_arr) * fblks_cnt));
        free_idx = 0;
        shr_itbm_blk_t_init(&fblk);
        fblk.entries_per_bucket = hdl->params.entries_per_bucket;
        fblk.bucket = hdl->params.first;
        fblk.first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
        for (b = 0; b < ablks_cnt; b++) {
            SHR_IF_ERR_EXIT
                (shr_itbm_bkt_list_ublk_size_get(unit, &fblk, &ablk_arr[b]));
            if (fblk.ecount && (free_idx < fblks_cnt)) {
                /* Check and update the biggest free block available params. */
                if (fblk.ecount > largest_fblk->ecount) {
                    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                                  (BSL_META_U(unit,
                                              "largest_fblk: epbkt=%-2u "
                                              "b=%-5d f=%-5u ecnt=%-5u "
                                              "comp_id=%-2d.\n"),
                                   fblk.entries_per_bucket,
                                   (int)fblk.bucket,
                                   fblk.first_elem,
                                   fblk.ecount,
                                   (int)fblk.comp_id));
                    }
                    *largest_fblk = fblk;
                }
                fblk_arr[free_idx].entries_per_bucket = fblk.entries_per_bucket;
                fblk_arr[free_idx].bucket = fblk.bucket;
                fblk_arr[free_idx].first_elem = fblk.first_elem;
                fblk_arr[free_idx].comp_id = SHR_ITBM_INVALID;
                fblk_arr[free_idx++].ecount = fblk.ecount;
            }
            /*
             * Get the end bucket and element index of the current alloc
             * block.
             */
            SHR_IF_ERR_EXIT
                (shr_itbm_bkt_list_blk_end_get(unit,
                                               ablk_arr[b].bucket,
                                               ablk_arr[b].first_elem,
                                               ablk_arr[b].ecount,
                                               ablk_arr[b].entries_per_bucket,
                                               &ebkt,
                                               &eidx));
            /*
             * Move the free block's bucket and first_element number location
             * past the current in-use block.
             */
            if (eidx == (ablk_arr[b].entries_per_bucket - 1)
                || (hdl->params.entries_per_bucket
                    != ablk_arr[b].entries_per_bucket)) {
                fblk.bucket = ebkt + 1;
                fblk.first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
            } else {
                fblk.bucket = ebkt;
                fblk.first_elem = eidx + 1;
            }
            /* Clear the free block size value. */
            fblk.ecount = 0;
        }

        shr_itbm_blk_t_init(&lblk);
        lblk.entries_per_bucket = hdl->params.entries_per_bucket;
        lblk.bucket = (hdl->params.last + 1);
        lblk.first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
        /*
         * fblk is upper block and lblk is the lower block in below function
         * call.
         */
        SHR_IF_ERR_EXIT
            (shr_itbm_bkt_list_ublk_size_get(unit, &fblk, &lblk));
        if (fblk.ecount && (free_idx < fblks_cnt)) {
            /* Check and update the biggest free block available params. */
            if (fblk.ecount > largest_fblk->ecount) {
                if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "largest_fblk: epbkt=%-2u b=%-5d "
                                          "f=%-5u ecnt=%-5u comp_id=%-2d.\n"),
                               fblk.entries_per_bucket,
                               (int)fblk.bucket,
                               fblk.first_elem,
                               fblk.ecount,
                               (int)fblk.comp_id));

                }
                *largest_fblk = fblk;
            }
            fblk_arr[free_idx].entries_per_bucket = fblk.entries_per_bucket;
            fblk_arr[free_idx].bucket = fblk.bucket;
            fblk_arr[free_idx].first_elem = fblk.first_elem;
            fblk_arr[free_idx].comp_id = SHR_ITBM_INVALID;
            fblk_arr[free_idx++].ecount = fblk.ecount;
        }

        /*
         * Final "free_idx" value must be equal to the total "fblks_cnt",
         * that was previously determined.
         */
        if (free_idx != fblks_cnt) {
            SHR_FREE(fblk_arr);
            SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                                (BSL_META_U(unit,
                                            "(free_idx(%u) != "
                                            "fblks_cnt(%u)) - rv=%s.\n"),
                                 free_idx,
                                 fblks_cnt,
                                 shr_errmsg(SHR_E_INTERNAL)));
        }
    }
    *free_blks = fblk_arr;
    *free_blks_cnt = fblks_cnt;
exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(fblk_arr);
    }
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_free_blks_get(int unit,
                            shr_itbm_list_hdl_t hdl,
                            uint32_t ablks_cnt,
                            shr_itbm_blk_t *ablk_arr,
                            uint32_t *free_blks_cnt,
                            shr_itbm_blk_t **free_blks,
                            shr_itbm_blk_t *largest_fblk)
{
    uint32_t idx; /* Index iterator variable. */
    uint32_t free_idx; /* Free blocks index, iterator variable. */
    uint32_t fblks_cnt = 0; /* Total free blocks count. */
    shr_itbm_blk_t fblk; /* Blks allocated by the component. */
    shr_itbm_blk_t *fblk_arr = NULL; /* Free blocks array. */
    shr_itbm_list_stat_t stat; /* List statistics. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(free_blks_cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(free_blks, SHR_E_PARAM);
    SHR_NULL_CHECK(largest_fblk, SHR_E_PARAM);

    /* This function is supported only for block_stat enabled lists. */
    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List \"block_stat\" is disabled - "
                                        "rv=%s).\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (ablks_cnt) {
        /*
         * If ablks_cnt is non-zero, then "ablk_arr" input parameter must not
         * be null. "ablk_arr" will be NULL only when no blocks from this list
         * are in-use.
         */
        SHR_NULL_CHECK(ablk_arr, SHR_E_PARAM);
    }

    /*
     * If no blocks have been allocated from the list, then the entire table
     * is a one single free block.
     */
    if (!ablks_cnt) {
        /* Get the list statistics. */
        SHR_IF_ERR_EXIT
            (shr_itbm_list_attr_stat_get(unit, hdl, &stat));
        SHR_ALLOC(fblk_arr, sizeof(*fblk_arr), "shritbmFreeBlkArr");
        SHR_NULL_CHECK(fblk_arr, SHR_E_MEMORY);
        sal_memset(fblk_arr, 0, sizeof(*fblk_arr));

        fblk_arr->first_elem = hdl->params.first;
        fblk_arr->ecount = stat.free_count;
        fblk_arr->comp_id = SHR_ITBM_INVALID;
        *free_blks = fblk_arr;
        *free_blks_cnt = 1;
        SHR_EXIT();
    }

    /*
     * Initialize the first free block's first_elem number to the indexed
     * table's first entry number.
     */
    shr_itbm_blk_t_init(&fblk);
    fblk.first_elem = hdl->params.first;
    fblks_cnt = 0;

    /*
     * Iterate through the in-use blocks and find the free blocks (gaps)
     * between the in-use blocks.
     */
    for (idx = 0; idx < ablks_cnt; idx++) {
        /*
         * Determine if there are gaps between the current in-use block and
         * the free block's current first_elem location.
         */
        fblk.ecount = ablk_arr[idx].first_elem - fblk.first_elem;
        if (fblk.ecount) {
            /* Increment the free blocks count. */
            fblks_cnt++;
        }
        /*
         * Move the free block's first_element number location past the
         * current in-use block.
         */
        fblk.first_elem = ablk_arr[idx].first_elem + ablk_arr[idx].ecount;
    }
    /*
     * After the above for-loop, the free block's first_elem's location will
     * be the first element after the all in-use blocks. Now check if there
     * are free elements between this location and the last element of this
     * list.
     */
    fblk.ecount = (hdl->params.last + 1) - fblk.first_elem;
    if (fblk.ecount) {
        /* If free elements are found, then increment the free blocks count. */
        fblks_cnt++;
    }

    if (fblks_cnt) {
        SHR_ALLOC(fblk_arr, sizeof(*fblk_arr) * fblks_cnt, "shritbmFreeBlkArr");
        SHR_NULL_CHECK(fblk_arr, SHR_E_MEMORY);
        sal_memset(fblk_arr, 0, (sizeof(*fblk_arr) * fblks_cnt));
        free_idx = 0;
        shr_itbm_blk_t_init(&fblk);
        fblk.first_elem = hdl->params.first;
        for (idx = 0; idx < ablks_cnt; idx++) {
            fblk.ecount = ablk_arr[idx].first_elem - fblk.first_elem;
            if (fblk.ecount && (free_idx < fblks_cnt)) {
                /* Check and update the biggest free block available params. */
                if (fblk.ecount > largest_fblk->ecount) {
                    *largest_fblk = fblk;
                }
                shr_itbm_blk_t_init(&fblk_arr[free_idx]);
                fblk_arr[free_idx].first_elem = fblk.first_elem;
                fblk_arr[free_idx].comp_id = SHR_ITBM_INVALID;
                fblk_arr[free_idx++].ecount = fblk.ecount;
            }
            fblk.first_elem = ablk_arr[idx].first_elem +
                                        ablk_arr[idx].ecount;
        }
        fblk.ecount = (hdl->params.last + 1) - fblk.first_elem;
        if (fblk.ecount && (free_idx < fblks_cnt)) {
            /* Check and update the biggest free block available params. */
            if (fblk.ecount > largest_fblk->ecount) {
                *largest_fblk = fblk;
            }
            shr_itbm_blk_t_init(&fblk_arr[free_idx]);
            fblk_arr[free_idx].first_elem = fblk.first_elem;
            fblk_arr[free_idx].comp_id = SHR_ITBM_INVALID;
            fblk_arr[free_idx++].ecount = fblk.ecount;
        }

        /*
         * Final "free_idx" value must be equal to the total "fblks_cnt",
         * that was previously determined.
         */
        if (free_idx != fblks_cnt) {
            SHR_FREE(fblk_arr);
            SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                                (BSL_META_U(unit,
                                            "\"free_idx(%u) != "
                                            "fblks_cnt(%u)\" - rv=%s.\n"),
                                 free_idx,
                                 fblks_cnt,
                                 shr_errmsg(SHR_E_INTERNAL)));
        }
    }
    *free_blks = fblk_arr;
    *free_blks_cnt = fblks_cnt;
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_edata_backup_create(int unit,
                                  shr_itbm_list_hdl_t hdl)
{
    uint32_t idx = SHR_ITBM_INVALID; /* Index iterator variable. */
    uint32_t alloc_sz = 0; /* Memory alloc size in bytes. */
    uint32_t epbkt = SHR_ITBM_LIST_BKT_ONE_ENTRY; /* Entries per bucket. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);

    /* Get the list info pointer. */
    linfo = hdl->linfo;

    if (hdl->params.buckets == TRUE) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_stat_bkup(unit, hdl));
    }

    /* Allocate memory to store list elements data. */
    alloc_sz = sizeof(*(linfo->edata_bk)) * linfo->tot_ecount;
    SHR_ALLOC(linfo->edata_bk, alloc_sz, "shritbmEdataBkArr");
    SHR_NULL_CHECK(linfo->edata_bk, SHR_E_MEMORY);
    sal_memset(linfo->edata_bk, 0, alloc_sz);

    /* Backup the active list elements data. */
    for (idx = 0; idx < linfo->tot_ecount; idx++) {
        /* Initialize the edata_bk member pointers. */
        linfo->edata_bk[idx].eprop = NULL;
        linfo->edata_bk[idx].ebmp = NULL;
        linfo->edata_bk[idx].blk_sz = NULL;
        linfo->edata_bk[idx].comp_id = NULL;
        if (hdl->params.buckets) {
            epbkt = linfo->edata[idx].max_entries;
            /* For indexed buckets lists, ebmp must not be null. */
            SHR_NULL_CHECK(linfo->edata[idx].ebmp, SHR_E_INTERNAL);
            SHR_ALLOC(linfo->edata_bk[idx].ebmp,
                      SHR_BITALLOCSIZE(linfo->max_ent_per_bkt),
                      "shritbmEdataBkEbmp");
            SHR_NULL_CHECK(linfo->edata_bk[idx].ebmp, SHR_E_MEMORY);
            sal_memset(linfo->edata_bk[idx].ebmp, 0,
                       SHR_BITALLOCSIZE(linfo->max_ent_per_bkt));

            SHR_BITCOPY_RANGE(linfo->edata_bk[idx].ebmp, 0,
                              linfo->edata[idx].ebmp,
                              SHR_ITBM_LIST_BLK_FIRST_ELEM,
                              epbkt);
        }
        if (linfo->edata[idx].eref_cnt > 0) {
            if (linfo->edata[idx].eprop != NULL) {
                /*
                 * When eprop pointer is valid, corresponding blk_sz pointer
                 * must also be valid.
                 */
                SHR_NULL_CHECK(linfo->edata[idx].blk_sz, SHR_E_INTERNAL);

                SHR_ALLOC(linfo->edata_bk[idx].eprop, SHR_BITALLOCSIZE(epbkt),
                          "shritbmEdataBkEprop");
                SHR_NULL_CHECK(linfo->edata_bk[idx].eprop, SHR_E_MEMORY);
                sal_memset(linfo->edata_bk[idx].eprop, 0,
                           SHR_BITALLOCSIZE(epbkt));

                SHR_BITCOPY_RANGE(linfo->edata_bk[idx].eprop, 0,
                                  linfo->edata[idx].eprop,
                                  SHR_ITBM_LIST_BLK_FIRST_ELEM,
                                  epbkt);
                /* Copy block size data. */
                SHR_ALLOC(linfo->edata_bk[idx].blk_sz,
                          sizeof(*(linfo->edata_bk[idx].blk_sz)) * epbkt,
                          "shritbmEdataBkBlksz");
                SHR_NULL_CHECK(linfo->edata_bk[idx].blk_sz, SHR_E_MEMORY);
                sal_memset(linfo->edata_bk[idx].blk_sz, 0,
                           sizeof(*(linfo->edata_bk[idx].blk_sz)) * epbkt);
                sal_memcpy(linfo->edata_bk[idx].blk_sz,
                           linfo->edata[idx].blk_sz,
                           sizeof(*(linfo->edata[idx].blk_sz)) * epbkt);
            }
            if (linfo->edata[idx].comp_id != NULL) {
                /* Copy component ID data. */
                SHR_ALLOC(linfo->edata_bk[idx].comp_id,
                          sizeof(*(linfo->edata_bk[idx].comp_id)) * epbkt,
                          "shritbmEdataBkCompid");
                SHR_NULL_CHECK(linfo->edata_bk[idx].comp_id, SHR_E_MEMORY);
                sal_memset(linfo->edata_bk[idx].comp_id, 0,
                           sizeof(*(linfo->edata_bk[idx].comp_id)) * epbkt);
                sal_memcpy(linfo->edata_bk[idx].comp_id,
                           linfo->edata[idx].comp_id,
                           sizeof(*(linfo->edata[idx].comp_id)) * epbkt);
            }
        }
        linfo->edata_bk[idx].max_entries = linfo->edata[idx].max_entries;
        linfo->edata_bk[idx].eref_cnt = linfo->edata[idx].eref_cnt;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: [edata_bk=%p alloc_sz(in bytes)=%-5u "
                            "tot_ecnt=%-5u].\n"),
                 (void *)hdl->linfo->edata_bk, alloc_sz, linfo->tot_ecount));
exit:
    if (SHR_FUNC_ERR()) {
        if (hdl && hdl->linfo && hdl->linfo->edata) {
            for (idx = 0; idx < linfo->tot_ecount; idx++) {
                SHR_FREE(linfo->edata_bk[idx].eprop);
                SHR_FREE(linfo->edata_bk[idx].ebmp);
                SHR_FREE(linfo->edata_bk[idx].blk_sz);
                SHR_FREE(linfo->edata_bk[idx].comp_id);
            }
            SHR_FREE(linfo->edata_bk);
        }
    }
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_defrag_abort_error_cleanup(int unit,
                                         shr_itbm_list_hdl_t hdl)
{
    uint32_t idx;      /* Index iterator variable. */
    uint32_t midx = 0; /* Block move index. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_defrag_blk_mseq_t *bmov_seq = NULL; /* Blocks move sequence. */
    shr_itbm_lattr_t *alist = NULL; /* Ptr to list attributes linked list. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata_bk, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->alist, SHR_E_PARAM);

    /* Get the list info pointer. */
    linfo = hdl->linfo;
    alist = linfo->alist;
    while (alist) {
        if (alist->params.buckets) {
            alist->stat.free_count = (((alist->params.last
                                        - alist->params.first) + 1)
                                        * alist->params.entries_per_bucket);
        } else {
            alist->stat.free_count = ((alist->params.last - alist->params.first)
                                      + 1);
        }
        alist->stat.in_use_count = 0;
        alist->tot_elems = alist->stat.free_count;
        alist = alist->next;
    }
    for (idx = 0; idx < linfo->tot_ecount; idx++) {
        if (hdl->params.buckets) {
            SHR_BITCLR_RANGE(linfo->edata[idx].ebmp, 0, linfo->max_ent_per_bkt);
            SHR_FREE(linfo->edata_bk[idx].ebmp);
            linfo->edata[idx].max_entries = 0;
        }
        linfo->edata[idx].eref_cnt = 0;
        SHR_FREE(linfo->edata_bk[idx].eprop);
        SHR_FREE(linfo->edata_bk[idx].blk_sz);
        SHR_FREE(linfo->edata_bk[idx].comp_id);
        SHR_FREE(linfo->edata[idx].eprop);
        SHR_FREE(linfo->edata[idx].blk_sz);
        SHR_FREE(linfo->edata[idx].comp_id);
    }
    SHR_FREE(linfo->edata_bk);
    /*
     * Free the memory allocated to store and share the defragementation
     * sequence with the caller.
     */
    SHR_FREE(linfo->dblk_cur_loc);
    bmov_seq = linfo->dblk_mseq;
    if (bmov_seq) {
        for (midx = 0; midx < linfo->dblks_moved; midx++) {
            if (bmov_seq[midx].mcount) {
                SHR_FREE(bmov_seq[midx].bucket);
                SHR_FREE(bmov_seq[midx].first_elem);
            }
        }
        SHR_FREE(linfo->dblk_mseq);
    }
    /* Clear list defragmentation blocks moved counter. */
    linfo->dblks_moved = 0;
    /* Clear list defragmentation handle pointer value. */
    linfo->defrag_hdl = NULL;
    /* Clear list defragmentation enabled state. */
    linfo->defrag_inprog = FALSE;
    /* Initialize number of in-use elements count. */
    linfo->inuse_ecount = 0;
    /* Initialize number of free elements count. */
    linfo->free_ecount = linfo->tot_ecount;
    /*! Initialize total number of blocks alloced from this list count. */
    linfo->tot_blks_alloced = 0;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: Completed abort error cleanup.\n")));
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_defrag_abort(int unit,
                           shr_itbm_list_hdl_t hdl)
{
    uint32_t idx;      /* Index iterator variable. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    uint32_t epbkt = SHR_ITBM_LIST_BKT_ONE_ENTRY; /* Entries per bucket. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata_bk, SHR_E_PARAM);

    /* Get the list info pointer. */
    linfo = hdl->linfo;

    if (hdl->params.buckets == TRUE) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_stat_abort(unit, hdl));
    }

    /* Copy edata from backup. */
    for (idx = 0; idx < linfo->tot_ecount; idx++) {
        if (hdl->params.buckets) {
            SHR_NULL_CHECK(linfo->edata[idx].ebmp, SHR_E_INTERNAL);
            SHR_NULL_CHECK(linfo->edata_bk[idx].ebmp, SHR_E_INTERNAL);
            epbkt = linfo->edata_bk[idx].max_entries;
        }
        SHR_FREE(linfo->edata[idx].eprop);
        SHR_FREE(linfo->edata[idx].blk_sz);
        SHR_FREE(linfo->edata[idx].comp_id);

        if (linfo->edata_bk[idx].eref_cnt > 0) {
            if (hdl->params.block_stat == TRUE
                && linfo->edata_bk[idx].eprop != NULL) {
                SHR_ALLOC(linfo->edata[idx].eprop,
                          SHR_BITALLOCSIZE(linfo->max_ent_per_bkt),
                          "shritbmEdataBlkEprop");
                SHR_NULL_CHECK(linfo->edata[idx].eprop, SHR_E_MEMORY);
                sal_memset(linfo->edata[idx].eprop, 0,
                           SHR_BITALLOCSIZE(linfo->max_ent_per_bkt));
                SHR_ALLOC(linfo->edata[idx].blk_sz,
                          (sizeof(*(linfo->edata[idx].blk_sz))
                           * linfo->max_ent_per_bkt),
                          "shritbmEdataBlkSz");
                SHR_NULL_CHECK(linfo->edata[idx].blk_sz, SHR_E_MEMORY);
                sal_memset(linfo->edata[idx].blk_sz, 0,
                           (sizeof(*(linfo->edata[idx].blk_sz))
                            * linfo->max_ent_per_bkt));
            }
            if (linfo->edata_bk[idx].comp_id != NULL) {
                /*
                 * Alloc comp_id memory freed during list defragmentation
                 * block move sequence.
                 */
                SHR_ALLOC(linfo->edata[idx].comp_id,
                          (sizeof(*(linfo->edata[idx].comp_id))
                           * linfo->max_ent_per_bkt),
                          "shritbmEdataBlkCompId");
                SHR_NULL_CHECK(linfo->edata[idx].comp_id, SHR_E_MEMORY);
                sal_memset(linfo->edata[idx].comp_id, 0,
                           (sizeof(*(linfo->edata[idx].comp_id))
                            * linfo->max_ent_per_bkt));
            }
        }
        if (hdl->params.buckets) {
            SHR_BITCOPY_RANGE(linfo->edata[idx].ebmp, 0,
                              linfo->edata_bk[idx].ebmp,
                              SHR_ITBM_LIST_BLK_FIRST_ELEM,
                              epbkt);
        }
        if (linfo->edata_bk[idx].eprop != NULL) {
            /* Copy value from backup. */
            SHR_BITCOPY_RANGE(linfo->edata[idx].eprop, 0,
                              linfo->edata_bk[idx].eprop,
                              SHR_ITBM_LIST_BLK_FIRST_ELEM,
                              epbkt);
        }
        if (linfo->edata_bk[idx].blk_sz != NULL) {
            sal_memcpy(linfo->edata[idx].blk_sz,
                       linfo->edata_bk[idx].blk_sz,
                       sizeof(*(linfo->edata_bk[idx].blk_sz)) * epbkt);
        }
        if (linfo->edata_bk[idx].comp_id != NULL) {
            sal_memcpy(linfo->edata[idx].comp_id,
                       linfo->edata_bk[idx].comp_id,
                       sizeof(*(linfo->edata_bk[idx].comp_id)) * epbkt);
        }
        linfo->edata[idx].max_entries = linfo->edata_bk[idx].max_entries;
        linfo->edata[idx].eref_cnt = linfo->edata_bk[idx].eref_cnt;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: copied tot_ecnt=%-5u from bkup due to abort"
                            ".\n"),
                 linfo->tot_ecount));
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_edata_backup_destroy(int unit,
                                   shr_itbm_list_hdl_t hdl)
{
    uint32_t idx;      /* Index iterator variable. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */

    SHR_FUNC_ENTER(unit);
    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata_bk, SHR_E_PARAM);
    /* Get the list info pointer. */
    linfo = hdl->linfo;

    if (hdl->params.buckets == TRUE) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_stat_bkup_clear(unit, hdl));
    }

    for (idx = 0; idx < linfo->tot_ecount; idx++) {
        SHR_FREE(linfo->edata_bk[idx].eprop);
        SHR_FREE(linfo->edata_bk[idx].ebmp);
        SHR_FREE(linfo->edata_bk[idx].blk_sz);
        SHR_FREE(linfo->edata_bk[idx].comp_id);
    }
    SHR_FREE(linfo->edata_bk);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: Cleared tot_ecnt=%-5u from bkup.\n"),
                 linfo->tot_ecount));
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_defragment_rev(int unit,
                                 shr_itbm_list_hdl_t hdl,
                                 uint32_t comp_blks_cnt,
                                 uint32_t comp_max_ecount,
                                 uint32_t ablks_cnt,
                                 shr_itbm_blk_t *ablks,
                                 uint32_t fblks_cnt,
                                 shr_itbm_blk_t *fblks,
                                 shr_itbm_blk_t largest_fblk,
                                 uint32_t *dfg_bcount,
                                 shr_itbm_blk_t **dfg_bcur_loc,
                                 shr_itbm_defrag_blk_mseq_t **dfg_bmseq)
{
    uint32_t idx; /* Index iterator variable. */
    uint32_t midx = 0; /* Block move array index. */
    uint32_t bidx; /* Block index iterator variable. */
    bool rsvd_in_gap = FALSE; /* Reserved block part of the gap block. */
    uint32_t base_bkt = SHR_ITBM_INVALID; /* Block end bucket. */
    uint32_t base_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX; /* Blk end elem. */
    uint8_t lepbkt = 0; /* List entires per bucket count. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_defrag_seq_t *dseq = NULL; /* Pointer to list defrag sequence. */
    shr_itbm_defrag_blk_mseq_t *bmov_seq = NULL; /* Blocks move sequence. */
    shr_itbm_blk_move_info_t *minfo = NULL; /* Block move info. */
    shr_itbm_blk_t *mblk_cur_loc = NULL;  /* Defragmented component blocks. */
    shr_itbm_blk_t rsvd;   /* Reserved free block. */
    shr_itbm_blk_t gap;    /* Gap between the in-use blocks. */
    shr_itbm_blk_t cblk;   /* Current in-use block. */
    shr_pb_t *pb = NULL;   /* Debug print buffer. */
    shr_pb_t *mvpb = NULL; /* Block moves debug print buffer. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(dfg_bcount, SHR_E_PARAM);
    SHR_NULL_CHECK(dfg_bcur_loc, SHR_E_PARAM);
    SHR_NULL_CHECK(dfg_bmseq, SHR_E_PARAM);

    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List \"block_stat\" is disabled - "
                                        "rv=%s).\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (!ablks_cnt || ablks == NULL) {
        /*
         * If no blocks are in use then there is nothing to defragment, exit
         * the function.
         */
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "No blocks in-use to perform "
                                        "defragmentation - rv=%s.\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (!fblks_cnt || fblks == NULL) {
        /*
         * If there are no free blocks in the list, then defragmentation
         * operation cannot be performed. Set the error return status to
         * resource error and exit this function.
         */
        SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                            (BSL_META_U(unit,
                                        "No free blocks to support "
                                        "defragmentation - rv=%s.\n"),
                             shr_errmsg(SHR_E_RESOURCE)));
    }

    /* Initialize the list info pointer. */
    linfo = hdl->linfo;

    /*
     * Initialize total blocks moved during defragmentation operation count to
     * zero.
     */
    linfo->dblks_moved = 0;

    /* Allocate memory to store the blocks defragmentation move sequence. */
    SHR_ALLOC(linfo->dseq, sizeof(*(linfo->dseq)) * ablks_cnt,
              "shritbmDseqBktArr");
    SHR_NULL_CHECK(linfo->dseq, SHR_E_MEMORY);
    sal_memset(linfo->dseq, 0, sizeof(*(linfo->dseq)) * ablks_cnt);

    /* Initialize and update the reserved block parameters. */
    shr_itbm_blk_t_init(&rsvd);
    rsvd = largest_fblk;
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rsvd: fwd-[b=%-5d f=%-5u cnt=%-5u c=%-2d].\n"),
                     (int)rsvd.bucket,
                     rsvd.first_elem,
                     rsvd.ecount,
                     (int)rsvd.comp_id));
    }
    /*
     * Sort the in-use blocks array elements based on their first_elem
     * values in descending order as this is a reversed list defragmentation
     * operation. In-use blocks must be pushed/moved towards the bottom of the
     * table/list.
     */
    sal_qsort((void *)ablks, ablks_cnt, sizeof(*ablks),
              shr_itbm_blk_bkt_cmp_descend);
    /* Sorted allocated blocks array debug print. */
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "Sorted \"ablks[count=%-5u]\":\n",
                      ablks_cnt);
        shr_pb_printf(pb,
                      "----------------------------------------\n");
        for (idx = 0; idx < ablks_cnt; idx++) {
            shr_pb_printf(pb,
                          "%5u.[b=%-5u f=%-5u c=%-2d ecnt=%-5u]\n",
                          (idx + 1),
                          (int)ablks[idx].bucket,
                          ablks[idx].first_elem,
                          (int)ablks[idx].comp_id,
                          ablks[idx].ecount);
        }
        shr_pb_printf(pb, "\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }

    /* Sort the free blocks array elements based on their first_elem values. */
    sal_qsort((void *)fblks, fblks_cnt, sizeof(*fblks),
              shr_itbm_blk_bkt_cmp_descend);
    /* Sorted free blocks array debug print. */
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "Sorted \"fblks[count=%-5u]\":\n",
                      ablks_cnt);
        shr_pb_printf(pb,
                      "----------------------------------------\n");
        for (idx = 0; idx < fblks_cnt; idx++) {
            shr_pb_printf(pb,
                          "%5u.[b=%-5u f=%-5u c=%-2d ecnt=%-5u]\n",
                          (idx + 1),
                          (int)fblks[idx].bucket,
                          fblks[idx].first_elem,
                          (int)fblks[idx].comp_id,
                          fblks[idx].ecount);
        }
        shr_pb_printf(pb, "\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }

    /*
     * Initialize the gap block to point to the first bucket and element of the
     * list.
     */
    lepbkt = hdl->params.entries_per_bucket;
    shr_itbm_blk_t_init(&gap);
    gap.entries_per_bucket = lepbkt;
    gap.bucket = hdl->params.last;
    gap.first_elem = lepbkt - 1;
    gap.ecount = 0;
    gap.comp_id = hdl->params.comp_id;

    /* Create block moves print buffer to capture the moves. */
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        mvpb = shr_pb_create();
        shr_pb_printf(mvpb, "Capturing block move sequence:\n");
        shr_pb_printf(mvpb, "-------------------------------------------\n");
        shr_pb_printf(mvpb, "rsvd@start  : (epbkt=%-1u b=%-5u f=%-5u ecnt=%-5u "
                            "comp=%-2d)\n",
                            rsvd.entries_per_bucket,
                            rsvd.bucket,
                            rsvd.first_elem,
                            rsvd.ecount,
                            rsvd.comp_id);
        shr_pb_printf(mvpb, "gap@start   : (epbkt=%-1u b=%-5u f=%-5u ecnt=%-5u "
                            "comp=%-2d)\n",
                            gap.entries_per_bucket,
                            gap.bucket,
                            gap.first_elem,
                            gap.ecount,
                            gap.comp_id);
        shr_pb_printf(mvpb, "dblks_moved : (%-5d).\n",
                            linfo->dblks_moved);
        shr_pb_printf(mvpb, "-------------------------------------------\n");
    }

    /*
     * Iterate over the allocated in-use blocks of the component that has
     * initiated the list defragmentation operation and compress the gaps
     * between the blocks used by this component. Allocated list blocks that
     * belong to another component or have a different entries_per_bucket
     * count value are skipped during the list defragmentation operation.
     */
    for (bidx = 0; bidx < ablks_cnt; bidx++) {
        /* Initialize and get the current in-use block params. */
        shr_itbm_blk_t_init(&cblk);
        cblk = ablks[bidx];

        /*
         * Skip the in-use blocks with component ID mismatch or
         * entries_per_bucket count value mismatch.
         */
        if (cblk.comp_id != gap.comp_id
            || cblk.entries_per_bucket != gap.entries_per_bucket) {
            /*
             * Set the gap block bucket and first element index location
             * to the bucket and element before the current block.
             */
            gap.bucket = cblk.bucket - 1;
            gap.first_elem = gap.entries_per_bucket - 1;
            gap.ecount = 0;
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                shr_pb_printf(mvpb,
                              "SkipCblk@(bidx=%u): [epbkt(e/a)=%-1u/%-1u "
                              "c(e/a)=%-2u/%-2u - b=%-5u f=%-5u ecnt=%-5u].\n",
                              bidx,
                              gap.entries_per_bucket,
                              cblk.entries_per_bucket,
                              gap.comp_id,
                              cblk.comp_id,
                              (int)cblk.bucket,
                              cblk.first_elem,
                              cblk.ecount);
            }
            continue;
        }
        /*
         * Determine the gap between the gap block bucket + first_elem and
         * the current in-use (cblk) block's bucket + last element.
         *
         * Note: Current block (cblk) and gap block (gap) must support same
         * entries_per_bucket count value for this defragmentation function
         * to work correctly.
         */
        SHR_IF_ERR_EXIT
            (shr_itbm_invt_bkt_list_blks_gap_get(unit, &cblk, &gap));

        if (cblk.bucket < rsvd.bucket && gap.bucket >= rsvd.bucket) {
            if (gap.bucket >= rsvd.bucket && gap.ecount >= cblk.ecount
                && gap.ecount >= rsvd.ecount) {
                /*
                 * The current gap includes the reserved block. The reserved
                 * block is used as a hop destination for blocks require more
                 * than one move operation and as the current gap is larger
                 * than the previous reserved block size, after moving the
                 * cblk into the current gap, update the new location + size
                 * of the reserved block.
                 * e.g.
                 *   _______________
                 *  |_______________|
                 *  |______cblk.0___|b[x - 5]
                 *  |______cblk.1___|b[x - 4]
                 *  |______gap/rsvd_|b[x - 3]
                 *  |______gap/rsvd_|b[x - 2]
                 *  |______gap/rsvd_|b[x - 1]
                 *  |______gap______|b[x]
                 *  |_______________|
                 */
                rsvd_in_gap = TRUE;
            } else if (gap.ecount < comp_max_ecount) {
                /*
                 * Skip the reserved block and move the gap block's first_elem
                 * location to the first element in the reserved block.
                 * e.g.
                 *   _______________
                 *  |_______________|
                 *  |______cblk.0___|b[x - 4]
                 *  |______cblk.1___|b[x - 3]
                 *  |_______________|b[x - 2]
                 *  |______gap/rsvd_|b[x - 1]
                 *  |______gap/rsvd_|b[x]
                 *  |_______________|
                 */
                gap.bucket = rsvd.bucket;
                gap.first_elem = rsvd.first_elem;
                /* Recompute the gap size based on the new location. */
                SHR_IF_ERR_EXIT
                    (shr_itbm_invt_bkt_list_blks_gap_get(unit, &cblk, &gap));
            }
        }

        if (gap.ecount == 0) {
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                shr_pb_printf(mvpb,
                              "SkipCblk@GapZero(bidx=%-4u, gap=%-1u): "
                              "[b=%-5u f=%-5u c=%-4u ecnt=%-5u].\n",
                              bidx,
                              gap.ecount,
                              (int)cblk.bucket,
                              cblk.first_elem,
                              cblk.comp_id,
                              cblk.ecount);
            }
            /*
             * Set gap block base bucket and bucket local index to the location
             * prior to the current in-use block.
             */
            if (cblk.first_elem == SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX) {
                gap.bucket = cblk.bucket - 1;
                gap.first_elem = cblk.entries_per_bucket - 1;
            } else {
                gap.bucket = cblk.bucket;
                gap.first_elem = cblk.first_elem - 1;
            }
        } else if (gap.ecount > 0) {
            if (cblk.ecount <= gap.ecount) {
                /*
                 * Findout the gap's base_bucket and bucket local index based
                 * on it's current bucket, first_elem and cblk ecount value.
                 * This is the forward destination location for cblk to be
                 * passed to the calling application.
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_rev_bkt_list_blk_base_get(unit,
                                                        gap.bucket,
                                                        gap.first_elem,
                                                        cblk.ecount,
                                                        gap.entries_per_bucket,
                                                        &base_bkt,
                                                        &base_idx));
                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "Mv@Gap(bidx=%-4u gap=%-5u rsvd@gap=%-1d): "
                                  "[s:b=%-5u f=%-5u epbkt=%-1u d:bb=%-5u "
                                  "bbi=%-5u eb=%-5u ef=%-5u epbkt=%-1u "
                                  "ecnt=%-5u].\n",
                                  bidx,
                                  gap.ecount,
                                  rsvd_in_gap,
                                  (int)cblk.bucket,
                                  cblk.first_elem,
                                  cblk.entries_per_bucket,
                                  base_bkt,
                                  base_idx,
                                  (int)gap.bucket,
                                  gap.first_elem,
                                  gap.entries_per_bucket,
                                  cblk.ecount);
                }
                /*
                 * Perform the block move operation.
                 *  src     : cblk.first_elem
                 *  dst     : gap.first_elem
                 *  ecount  : cblk.ecount
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_block_move(unit,
                                                  hdl,
                                                  cblk.bucket,
                                                  cblk.first_elem,
                                                  base_bkt,
                                                  base_idx,
                                                  cblk.ecount));
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_dseq_update(unit,
                                                       hdl,
                                                       ablks_cnt,
                                                       bidx,
                                                       cblk.bucket,
                                                       cblk.first_elem,
                                                       base_bkt,
                                                       base_idx,
                                                       cblk.ecount));
                /* Update reserved block's new location. */
                if (rsvd_in_gap) {
                    /* Clear the flag settings. */
                    rsvd_in_gap = FALSE;

                    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                        shr_pb_printf(mvpb,
                                      "    Old@Rsvd: [b=%-5u f=%-5u ecnt=%-5u "
                                      "epbkt=%-1u].\n",
                                      (int)rsvd.bucket,
                                      rsvd.first_elem,
                                      rsvd.ecount,
                                      rsvd.entries_per_bucket);
                    }
                    /*
                     * Compute the reserved block's new base bucket and first
                     * element index, which will precede the cblk location.
                     */
                    SHR_IF_ERR_EXIT
                        (shr_itbm_rev_bkt_list_blk_base_get
                            (unit,
                             gap.bucket,
                             gap.first_elem,
                             (gap.ecount - cblk.ecount),
                             gap.entries_per_bucket,
                             &rsvd.bucket,
                             &rsvd.first_elem));
                    rsvd.ecount = gap.ecount - cblk.ecount;
                    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                        shr_pb_printf(mvpb,
                                      "    New@Rsvd: [b=%-5u f=%-5u ecnt=%-5u "
                                      "epbkt=%-1u].\n",
                                      rsvd.bucket,
                                      rsvd.first_elem,
                                      rsvd.ecount,
                                      rsvd.entries_per_bucket);
                    }
                }
                if (base_idx == SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX) {
                    gap.bucket = base_bkt - 1;
                    gap.first_elem = cblk.entries_per_bucket - 1;
                } else {
                    gap.bucket = base_bkt;
                    gap.first_elem = base_idx - 1;
                }
                /* Increment list blocks moved count. */
                linfo->dblks_moved++;
            } else if (cblk.ecount <= rsvd.ecount) {
                /*
                 * Perform the block move operation.
                 * Use reserved block as an hop, prior to moving the in-use
                 * block to it's final location.
                 *  Src     : cblk.first_elem
                 *  Dst     : rsvd.first_elem
                 *  ecount  : cblk.ecount
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_block_move(unit,
                                                  hdl,
                                                  cblk.bucket,
                                                  cblk.first_elem,
                                                  rsvd.bucket,
                                                  rsvd.first_elem,
                                                  cblk.ecount));
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_dseq_update(unit,
                                                       hdl,
                                                       ablks_cnt,
                                                       bidx,
                                                       cblk.bucket,
                                                       cblk.first_elem,
                                                       rsvd.bucket,
                                                       rsvd.first_elem,
                                                       cblk.ecount));

                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "*MvCblk@ToRsvd(bidx=%-4u): [s:b=%-5u f=%-5u"
                                  "d:b=%-5u f=%-5u rsvd_ecnt=%-5u].\n",
                                  bidx,
                                  cblk.bucket,
                                  cblk.first_elem,
                                  rsvd.bucket,
                                  rsvd.first_elem,
                                  rsvd.ecount);
                }
                /*
                 * Perform the block move operation.
                 * Move from the reserved block location to the gap region.
                 *  Src     : rsvd.first_elem
                 *  Dst     : gap.first_elem
                 *  ecount  : cblk.ecount
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_rev_bkt_list_blk_base_get(unit,
                                                        gap.bucket,
                                                        gap.first_elem,
                                                        cblk.ecount,
                                                        gap.entries_per_bucket,
                                                        &base_bkt,
                                                        &base_idx));
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_block_move(unit,
                                                  hdl,
                                                  rsvd.bucket,
                                                  rsvd.first_elem,
                                                  base_bkt,
                                                  base_idx,
                                                  cblk.ecount));
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_dseq_update(unit,
                                                       hdl,
                                                       ablks_cnt,
                                                       bidx,
                                                       rsvd.bucket,
                                                       rsvd.first_elem,
                                                       base_bkt,
                                                       base_idx,
                                                       cblk.ecount));
                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "MvToGap@FromRsvd(cblk=%-5u): [s:b=%-5u "
                                  "f=%-5u d:b=%-5u f=%-5u ecnt=%-5u].\n",
                                  bidx,
                                  rsvd.bucket,
                                  rsvd.first_elem,
                                  base_bkt,
                                  base_idx,
                                  cblk.ecount);
                }
                if (base_idx == SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX) {
                    gap.bucket = base_bkt - 1;
                    gap.first_elem = cblk.entries_per_bucket - 1;
                } else {
                    gap.bucket = base_bkt;
                    gap.first_elem = base_idx - 1;
                }
                /* Increment list blocks moved count. */
                linfo->dblks_moved++;
            } else {
                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "SkipCblk@NoFit(gap=%-5u, cblk=%-5u):[b=%-5u "
                                  "f=%-5u ecnt=%-5u].\n",
                                  gap.ecount,
                                  bidx,
                                  cblk.bucket,
                                  cblk.first_elem,
                                  cblk.ecount);
                }
                /*
                 * Move gap block bucket and offset index to point to the
                 * bucket and element before the current in-use block.
                 */
                if (cblk.first_elem == SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX) {
                    gap.bucket = cblk.bucket - 1;
                    gap.first_elem = cblk.entries_per_bucket - 1;
                } else {
                    gap.bucket = cblk.bucket;
                    gap.first_elem = cblk.first_elem - 1;
                }
            }
        } else {
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "%s"), shr_pb_str(mvpb)));
                shr_pb_destroy(mvpb);
                mvpb = NULL;
            }
            /* Gap should never be negative. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_printf(mvpb, "\n");
        shr_pb_printf(mvpb, "-------------------------------------------\n");
        shr_pb_printf(mvpb, "rsvd@end   : (b=%-5d f=%-5u ecnt=%-5u c=%-2d "
                            "epbkt=%-1u).\n",
                            (int)rsvd.bucket,
                            rsvd.first_elem,
                            rsvd.ecount,
                            rsvd.comp_id,
                            rsvd.entries_per_bucket);
        shr_pb_printf(mvpb, "gap@end   : (b=%-5d f=%-5u ecnt=%-5u c=%-2d "
                            "epbkt=%-1u).\n",
                            (int)gap.bucket,
                            gap.first_elem,
                            gap.ecount,
                            gap.comp_id,
                            gap.entries_per_bucket);
        shr_pb_printf(mvpb, "dblks_moved : (%-5d).\n", linfo->dblks_moved);
        shr_pb_printf(mvpb, "-------------------------------------------\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(mvpb)));
        shr_pb_destroy(mvpb);
        mvpb = NULL;
    }

    /*
     * Populate the moved blocks current location and the defragementation
     * sequence of the blocks in the function output parameters.
     */
    if (linfo->dblks_moved > 0 && ablks_cnt > 0) {
        SHR_ALLOC(mblk_cur_loc, sizeof(*mblk_cur_loc) * linfo->dblks_moved,
                  "shritbmCompMblkArr");
        SHR_NULL_CHECK(mblk_cur_loc, SHR_E_MEMORY);
        sal_memset(mblk_cur_loc, 0, sizeof(*mblk_cur_loc) * linfo->dblks_moved);

        SHR_ALLOC(bmov_seq, sizeof(*bmov_seq) * linfo->dblks_moved,
                  "shritbmBlkMseqArr");
        SHR_NULL_CHECK(bmov_seq, SHR_E_MEMORY);
        sal_memset(bmov_seq, 0, sizeof(*bmov_seq) * linfo->dblks_moved);

        /* Identify the in-use blocks that have been moved. */
        midx = 0;
        for (bidx = 0; bidx < ablks_cnt; bidx++) {
            dseq = linfo->dseq[bidx];
            if (dseq && dseq->mcount) {
                /*
                 * Update the block's current location in the component
                 * output parameters array.
                 */
                mblk_cur_loc[midx].comp_id = ablks[bidx].comp_id;
                mblk_cur_loc[midx].bucket = ablks[bidx].bucket;
                mblk_cur_loc[midx].first_elem = ablks[bidx].first_elem;
                mblk_cur_loc[midx].ecount = ablks[bidx].ecount;

                /* Update the blocks move sequence. */
                bmov_seq[midx].ecount = dseq->ecount;
                bmov_seq[midx].mcount = dseq->mcount;
                /*
                 * Allocate memory to store the move destination bucket and
                 * first_elem values for this moved block.
                 */
                SHR_ALLOC(bmov_seq[midx].bucket,
                          sizeof(uint32_t) * bmov_seq[midx].mcount,
                          "shritbmBlkMseqBkt");
                SHR_NULL_CHECK(bmov_seq[midx].bucket, SHR_E_MEMORY);
                sal_memset(bmov_seq[midx].bucket, 0,
                           sizeof(uint32_t) * bmov_seq[midx].mcount);

                SHR_ALLOC(bmov_seq[midx].first_elem,
                          sizeof(uint32_t) * bmov_seq[midx].mcount,
                          "shritbmBlkMseqFirst");
                SHR_NULL_CHECK(bmov_seq[midx].first_elem, SHR_E_MEMORY);
                sal_memset(bmov_seq[midx].first_elem, 0,
                           sizeof(uint32_t) * bmov_seq[midx].mcount);
                idx = 0;
                minfo = dseq->minfo;
                while (minfo) {
                    bmov_seq[midx].bucket[idx] = minfo->dbucket;
                    bmov_seq[midx].first_elem[idx++] = minfo->dfirst_elem;
                    minfo = minfo->next;
                }
                /* Increment mblk_cur_loc and bmov_seq array index. */
                midx++;
            }
        }

        if (midx != linfo->dblks_moved) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                                (BSL_META_U(unit,
                                            "midx=%u != dblks_moved=%u "
                                            "- rv=%s.\n"),
                                 midx,
                                 linfo->dblks_moved,
                                 shr_errmsg(SHR_E_INTERNAL)));
        } else {
            /* Return total defragemented blocks count. */
            *dfg_bcount = linfo->dblks_moved;

            /* Return the blocks current location. */
            linfo->dblk_cur_loc = mblk_cur_loc;
            *dfg_bcur_loc = linfo->dblk_cur_loc;

            /* Return the blocks move sequence. */
            linfo->dblk_mseq = bmov_seq;
            *dfg_bmseq = linfo->dblk_mseq;
        }
    } else {
        /* No blocks moved as part of the defragmentation operation. */
        *dfg_bcount = 0;
        *dfg_bcur_loc = NULL;
        *dfg_bmseq = NULL;
    }
exit:
    if (linfo && linfo->dseq) {
        shr_itbm_list_dseq_cleanup(unit,
                                   hdl,
                                   ablks_cnt);
    }
    if (mvpb) {
        if (SHR_FUNC_ERR() && BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            /*
             * Print the defrag sequence before the error to help with
             * error analysis.
             */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "%s"), shr_pb_str(mvpb)));
        }
        shr_pb_destroy(mvpb);
    }
    if (SHR_FUNC_ERR() && linfo) {
        if (bmov_seq) {
            for (midx = 0; midx < linfo->dblks_moved; midx++) {
                if (bmov_seq[midx].mcount) {
                    SHR_FREE(bmov_seq[midx].bucket);
                    SHR_FREE(bmov_seq[midx].first_elem);
                }
            }
            SHR_FREE(bmov_seq);
            linfo->dblk_mseq = NULL;
        }
        SHR_FREE(mblk_cur_loc);
        linfo->dblk_cur_loc = NULL;
        linfo->dblks_moved = 0;
    }
    SHR_FUNC_EXIT();
}

static int
shr_itbm_bkt_list_defragment(int unit,
                             shr_itbm_list_hdl_t hdl,
                             uint32_t comp_blks_cnt,
                             uint32_t comp_max_ecount,
                             uint32_t ablks_cnt,
                             shr_itbm_blk_t *ablks,
                             uint32_t fblks_cnt,
                             shr_itbm_blk_t *fblks,
                             shr_itbm_blk_t largest_fblk,
                             uint32_t *dfg_bcount,
                             shr_itbm_blk_t **dfg_bcur_loc,
                             shr_itbm_defrag_blk_mseq_t **dfg_bmseq)
{
    uint32_t idx; /* Index iterator variable. */
    uint32_t midx = 0; /* Block move array index. */
    uint32_t bidx; /* Block index iterator variable. */
    bool rsvd_in_gap = FALSE; /* Reserved block part of the gap block. */
    uint32_t end_bkt = SHR_ITBM_INVALID; /* Block end bucket. */
    uint8_t end_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX; /* Blk end elem. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_defrag_seq_t *dseq = NULL; /* Pointer to list defrag sequence. */
    shr_itbm_defrag_blk_mseq_t *bmov_seq = NULL; /* Blocks move sequence. */
    shr_itbm_blk_move_info_t *minfo = NULL; /* Block move info. */
    shr_itbm_blk_t *mblk_cur_loc = NULL;  /* Defragmented component blocks. */
    shr_itbm_blk_t rsvd;   /* Reserved free block. */
    shr_itbm_blk_t gap;    /* Gap between the in-use blocks. */
    shr_itbm_blk_t cblk;   /* Current in-use block. */
    shr_itbm_blk_t tblk;   /* Temporary block. */
    shr_pb_t *pb = NULL;   /* Debug print buffer. */
    shr_pb_t *mvpb = NULL; /* Block moves debug print buffer. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(dfg_bcount, SHR_E_PARAM);
    SHR_NULL_CHECK(dfg_bcur_loc, SHR_E_PARAM);
    SHR_NULL_CHECK(dfg_bmseq, SHR_E_PARAM);

    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List \"block_stat\" is disabled - "
                                        "rv=%s.\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (!ablks_cnt || ablks == NULL) {
        /*
         * If no blocks are in use then there is nothing to defragment, exit
         * the function.
         */
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "No blocks in-use to perform "
                                        "defragmentation - rv=%s.\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (!fblks_cnt || fblks == NULL) {
        /*
         * If there are no free blocks in the list, then defragmentation
         * operation cannot be performed. Set the error return status to
         * resource error and exit this function.
         */
        SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                            (BSL_META_U(unit,
                                        "No free blocks to support "
                                        "defragmentation - rv=%s.\n"),
                             shr_errmsg(SHR_E_RESOURCE)));
    }

    /* Initialize the list info pointer. */
    linfo = hdl->linfo;

    /*
     * Initialize total blocks moved during defragmentation operation count to
     * zero.
     */
    linfo->dblks_moved = 0;

    /* Allocate memory to store the blocks defragmentation move sequence. */
    SHR_ALLOC(linfo->dseq, sizeof(*(linfo->dseq)) * ablks_cnt,
              "shritbmDseqBktArr");
    SHR_NULL_CHECK(linfo->dseq, SHR_E_MEMORY);
    sal_memset(linfo->dseq, 0, sizeof(*(linfo->dseq)) * ablks_cnt);

    /* Initialize and update the reserved block parameters. */
    shr_itbm_blk_t_init(&rsvd);
    rsvd = largest_fblk;

    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rsvd: [b=%-5d f=%-5u cnt=%-5u c=%-2d].\n"),
                     (int)rsvd.bucket,
                     rsvd.first_elem,
                     rsvd.ecount,
                     (int)rsvd.comp_id));
    }

    /*
     * Sort the in-use blocks array elements based on their first_elem
     * values.
     */
    sal_qsort((void *)ablks, ablks_cnt, sizeof(*ablks), shr_itbm_blk_bkt_cmp);
    /* Sorted allocated blocks array debug print. */
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "Sorted \"ablks[count=%-5u]\":\n",
                      ablks_cnt);
        shr_pb_printf(pb,
                      "----------------------------------------\n");
        for (idx = 0; idx < ablks_cnt; idx++) {
            shr_pb_printf(pb,
                          "%5u.[b=%-5u f=%-5u c=%-2d ecnt=%-5u]\n",
                          (idx + 1),
                          (int)ablks[idx].bucket,
                          ablks[idx].first_elem,
                          (int)ablks[idx].comp_id,
                          ablks[idx].ecount);
        }
        shr_pb_printf(pb, "\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }

    /* Sort the free blocks array elements based on their first_elem values. */
    sal_qsort((void *)fblks, fblks_cnt, sizeof(*fblks), shr_itbm_blk_bkt_cmp);
    /* Sorted free blocks array debug print. */
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "Sorted \"fblks[count=%-5u]\":\n",
                      fblks_cnt);
        shr_pb_printf(pb,
                      "----------------------------------------\n");
        for (idx = 0; idx < fblks_cnt; idx++) {
            shr_pb_printf(pb,
                          "%5u.[b=%-5u f=%-5u c=%-2d ecnt=%-5u]\n",
                          (idx + 1),
                          (int)fblks[idx].bucket,
                          fblks[idx].first_elem,
                          (int)fblks[idx].comp_id,
                          fblks[idx].ecount);
        }
        shr_pb_printf(pb, "\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }

    /*
     * Initialize the gap block to point to the first bucket and element of the
     * list.
     */
    shr_itbm_blk_t_init(&gap);
    gap.entries_per_bucket = hdl->params.entries_per_bucket;
    gap.bucket = hdl->params.first;
    gap.first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
    gap.ecount = 0;
    gap.comp_id = hdl->params.comp_id;

    /* Create block moves print buffer to capture the moves. */
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        mvpb = shr_pb_create();
        shr_pb_printf(mvpb, "Capturing block move sequence:\n");
        shr_pb_printf(mvpb, "-------------------------------------------\n");
        shr_pb_printf(mvpb, "rsvd@start  : (epbkt=%-1u b=%-5u f=%-5u ecnt=%-5u "
                            "comp=%-2d)\n",
                            rsvd.entries_per_bucket,
                            rsvd.bucket,
                            rsvd.first_elem,
                            rsvd.ecount,
                            rsvd.comp_id);
        shr_pb_printf(mvpb, "gap@start   : (epbkt=%-1u b=%-5u f=%-5u ecnt=%-5u "
                            "comp=%-2d)\n",
                            gap.entries_per_bucket,
                            gap.bucket,
                            gap.first_elem,
                            gap.ecount,
                            gap.comp_id);
        shr_pb_printf(mvpb, "dblks_moved : (%-5d).\n",
                            linfo->dblks_moved);
        shr_pb_printf(mvpb, "-------------------------------------------\n");
    }

    /*
     * Iterate over the allocated in-use blocks of the component that has
     * initiated the list defragmentation operation and compress the gaps
     * between the blocks used by this component. Allocated list blocks that
     * belong to another component or have a different entries_per_bucket
     * count value are skipped during the list defragmentation operation.
     */
    for (bidx = 0; bidx < ablks_cnt; bidx++) {
        /* Initialize and get the current in-use block params. */
        shr_itbm_blk_t_init(&cblk);
        cblk = ablks[bidx];

        /*
         * Skip the in-use blocks with component ID mismatch or
         * entries_per_bucket count value mismatch.
         */
        if (cblk.comp_id != gap.comp_id
            || cblk.entries_per_bucket != gap.entries_per_bucket) {
            SHR_IF_ERR_EXIT
                (shr_itbm_bkt_list_blk_end_get(unit,
                                               cblk.bucket,
                                               cblk.first_elem,
                                               cblk.ecount,
                                               cblk.entries_per_bucket,
                                               &end_bkt,
                                               &end_idx));
            /*
             * Move the gap block bucket and first element index location
             * beyond the current block.
             */
            gap.bucket = end_bkt + 1;
            gap.first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
            gap.ecount = 0;
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                shr_pb_printf(mvpb,
                              "SkipCblk@(bidx=%u): [epbkt(e/a)=%-1u/%-1u "
                              "c(e/a)=%-2u/%-2u - b=%-5u f=%-5u ecnt=%-5u].\n",
                              bidx,
                              gap.entries_per_bucket,
                              cblk.entries_per_bucket,
                              gap.comp_id,
                              cblk.comp_id,
                              (int)cblk.bucket,
                              cblk.first_elem,
                              cblk.ecount);
            }
            continue;
        }

        /*
         * Determine the gap between the gap block bucket + first_element and
         * the current in-use (cblk) block's bucket + first_element.
         */
        SHR_IF_ERR_EXIT
            (shr_itbm_bkt_list_ublk_size_get(unit, &gap, &cblk));

        if (cblk.bucket > rsvd.bucket && gap.bucket <= rsvd.bucket) {
            if (gap.bucket <= rsvd.bucket && gap.ecount >= cblk.ecount
                && gap.ecount >= rsvd.ecount) {
                /*
                 * The current gap includes the reserved block. The reserved
                 * block is used as a hop destination for blocks require more
                 * than one move operation and as the current gap is larger
                 * than the previous reserved block size, after moving the
                 * cblk into the current gap, update the new location + size
                 * of the reserved block.
                 * e.g.
                 *   _______________
                 *  |_______________|
                 *  |______gap______|b[x]
                 *  |______gap/rsvd_|b[x + 1]
                 *  |______gap/rsvd_|b[x + 2]
                 *  |______gap/rsvd_|b[x + 3]
                 *  |______cblk.0___|b[x + 4]
                 *  |______cblk.1___|b[x + 5]
                 *  |_______________|
                 */
                rsvd_in_gap = TRUE;
            } else if (gap.ecount < comp_max_ecount) {
                /*
                 * Skip the reserved block and move the gap block's first_elem
                 * location to the first element after the reserved block.
                 * e.g.
                 *   _______________
                 *  |_______________|
                 *  |______gap/rsvd_|b[x]
                 *  |______gap/rsvd_|b[x + 1]
                 *  |_______________|b[x + 2]
                 *  |______cblk.0___|b[x + 3]
                 *  |______cblk.1___|b[x + 4]
                 *  |_______________|
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_end_get(unit,
                                                   rsvd.bucket,
                                                   rsvd.first_elem,
                                                   rsvd.ecount,
                                                   rsvd.entries_per_bucket,
                                                   &end_bkt,
                                                   &end_idx));
                if (end_idx == (rsvd.entries_per_bucket - 1)) {
                    end_bkt += 1;
                    end_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
                } else {
                    end_idx += 1;
                }
                gap.bucket = end_bkt;
                gap.first_elem = end_idx;
                /* Recompute the gap size based on the new location. */
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_ublk_size_get(unit, &gap, &cblk));
            }
        }

        if (gap.ecount == 0) {
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                shr_pb_printf(mvpb,
                              "SkipCblk@GapZero(bidx=%-4u, gap=%-1u): "
                              "[b=%-5u f=%-5u c=%-4u ecnt=%-5u].\n",
                              bidx,
                              gap.ecount,
                              (int)cblk.bucket,
                              cblk.first_elem,
                              cblk.comp_id,
                              cblk.ecount);
            }
            /* Move gap block index beyound the current in-use block. */
            SHR_IF_ERR_EXIT
                (shr_itbm_bkt_list_blk_end_get(unit,
                                               cblk.bucket,
                                               cblk.first_elem,
                                               cblk.ecount,
                                               cblk.entries_per_bucket,
                                               &end_bkt,
                                               &end_idx));
            if (end_idx == (cblk.entries_per_bucket - 1)) {
                end_bkt = end_bkt + 1;
                end_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
            } else {
                end_idx = end_idx + 1;
            }
            gap.bucket = end_bkt;
            gap.first_elem = end_idx;
        } else if (gap.ecount > 0) {
            if (cblk.ecount <= gap.ecount) {
                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "Mv@Gap(bidx=%-4u gap=%-5u rsvd@gap=%-1d): "
                                  "[s:b=%-5u f=%-5u epbkt=%-1u d:b=%-5u f=%-5u "
                                  "epbkt=%-1u ecnt=%-5u].\n",
                                  bidx,
                                  gap.ecount,
                                  rsvd_in_gap,
                                  (int)cblk.bucket,
                                  cblk.first_elem,
                                  cblk.entries_per_bucket,
                                  (int)gap.bucket,
                                  gap.first_elem,
                                  gap.entries_per_bucket,
                                  cblk.ecount);
                }

                /*
                 * Perform the block move operation.
                 *  src     : cblk.first_elem
                 *  dst     : gap.first_elem
                 *  ecount  : cblk.ecount
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_block_move(unit,
                                                  hdl,
                                                  cblk.bucket,
                                                  cblk.first_elem,
                                                  gap.bucket,
                                                  gap.first_elem,
                                                  cblk.ecount));
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_dseq_update(unit,
                                                       hdl,
                                                       ablks_cnt,
                                                       bidx,
                                                       cblk.bucket,
                                                       cblk.first_elem,
                                                       gap.bucket,
                                                       gap.first_elem,
                                                       cblk.ecount));
                /* Update reserved block's new location. */
                if (rsvd_in_gap) {
                    /* Clear the flag settings. */
                    rsvd_in_gap = FALSE;

                    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                        shr_pb_printf(mvpb,
                                      "    Old@Rsvd: [b=%-5u f=%-5u ecnt=%-5u "
                                      "epbkt=%-1u].\n",
                                      (int)rsvd.bucket,
                                      rsvd.first_elem,
                                      rsvd.ecount,
                                      rsvd.entries_per_bucket);
                    }
                    /*
                     * Compute the reserved block's new base bucket and first
                     * element index, which will be immediately after the cblk.
                     */
                    SHR_IF_ERR_EXIT
                        (shr_itbm_bkt_list_blk_end_get(unit,
                                                       gap.bucket,
                                                       gap.first_elem,
                                                       cblk.ecount,
                                                       gap.entries_per_bucket,
                                                       &end_bkt,
                                                       &end_idx));
                    if (end_idx == gap.entries_per_bucket - 1) {
                        end_bkt = end_bkt + 1;
                        end_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
                    } else {
                        end_idx = end_idx + 1;
                    }
                    rsvd.bucket = end_bkt;
                    rsvd.first_elem = end_idx;

                    rsvd.ecount = (cblk.first_elem + cblk.ecount) -
                                        rsvd.first_elem;

                    SHR_IF_ERR_EXIT
                        (shr_itbm_bkt_list_blk_end_get(unit,
                                                       cblk.bucket,
                                                       cblk.first_elem,
                                                       cblk.ecount,
                                                       cblk.entries_per_bucket,
                                                       &end_bkt,
                                                       &end_idx));
                    shr_itbm_blk_t_init(&tblk);
                    if (end_idx == cblk.entries_per_bucket - 1) {
                        tblk.bucket = end_bkt + 1;
                        tblk.first_elem = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
                    } else {
                        tblk.bucket = end_bkt;
                        tblk.first_elem = end_idx + 1;
                    }
                    tblk.entries_per_bucket = cblk.entries_per_bucket;
                    /* Determine the reserved block size. */
                    SHR_IF_ERR_EXIT
                        (shr_itbm_bkt_list_ublk_size_get(unit, &rsvd, &tblk));

                    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                        shr_pb_printf(mvpb,
                                      "    New@Rsvd: [b=%-5u f=%-5u ecnt=%-5u "
                                      "epbkt=%-1u].\n",
                                      rsvd.bucket,
                                      rsvd.first_elem,
                                      rsvd.ecount,
                                      rsvd.entries_per_bucket);
                    }
                }
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_end_get(unit,
                                                   gap.bucket,
                                                   gap.first_elem,
                                                   cblk.ecount,
                                                   gap.entries_per_bucket,
                                                   &end_bkt,
                                                   &end_idx));
                if (end_idx == gap.entries_per_bucket - 1) {
                    end_bkt = end_bkt + 1;
                    end_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
                } else {
                    end_idx = end_idx + 1;
                }
                /* Both reserved and the gap blocks are same now. */
                gap.bucket = end_bkt;
                gap.first_elem = end_idx;
                /* Increment list blocks moved count. */
                linfo->dblks_moved++;
            } else if (cblk.ecount <= rsvd.ecount) {
                /*
                 * Perform the block move operation.
                 * Use reserved block as an hop, prior to moving the in-use
                 * block to it's final location.
                 *  Src     : cblk.first_elem
                 *  Dst     : rsvd.first_elem
                 *  ecount  : cblk.ecount
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_block_move(unit,
                                                  hdl,
                                                  cblk.bucket,
                                                  cblk.first_elem,
                                                  rsvd.bucket,
                                                  rsvd.first_elem,
                                                  cblk.ecount));
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_dseq_update(unit,
                                                       hdl,
                                                       ablks_cnt,
                                                       bidx,
                                                       cblk.bucket,
                                                       cblk.first_elem,
                                                       rsvd.bucket,
                                                       rsvd.first_elem,
                                                       cblk.ecount));

                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "*MvCblk@ToRsvd(bidx=%-4u): [s:b=%-5u f=%-5u"
                                  "d:b=%-5u f=%-5u rsvd_ecnt=%-5u].\n",
                                  bidx,
                                  cblk.bucket,
                                  cblk.first_elem,
                                  rsvd.bucket,
                                  rsvd.first_elem,
                                  rsvd.ecount);
                }
                /*
                 * Perform the block move operation.
                 * Move from the reserved block location to the gap region.
                 *  Src     : rsvd.first_elem
                 *  Dst     : gap.first_elem
                 *  ecount  : cblk.ecount
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_block_move(unit,
                                                  hdl,
                                                  rsvd.bucket,
                                                  rsvd.first_elem,
                                                  gap.bucket,
                                                  gap.first_elem,
                                                  cblk.ecount));
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_dseq_update(unit,
                                                       hdl,
                                                       ablks_cnt,
                                                       bidx,
                                                       rsvd.bucket,
                                                       rsvd.first_elem,
                                                       gap.bucket,
                                                       gap.first_elem,
                                                       cblk.ecount));

                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "MvToGap@FromRsvd(cblk=%-5u): [s:b=%-5u "
                                  "f=%-5u d:b=%-5u f=%-5u ecnt=%-5u].\n",
                                  bidx,
                                  rsvd.bucket,
                                  rsvd.first_elem,
                                  gap.bucket,
                                  gap.first_elem,
                                  cblk.ecount);
                }
                /* Move gap block index beyound the current in-use block. */
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_end_get(unit,
                                                   gap.bucket,
                                                   gap.first_elem,
                                                   cblk.ecount,
                                                   gap.entries_per_bucket,
                                                   &end_bkt,
                                                   &end_idx));
                if (end_idx == gap.entries_per_bucket - 1) {
                    end_bkt = end_bkt + 1;
                    end_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
                } else {
                    end_idx = end_idx + 1;
                }
                /* Both reserved and the gap blocks are same now. */
                gap.bucket = end_bkt;
                gap.first_elem = end_idx;
                /* Increment list blocks moved count. */
                linfo->dblks_moved++;
            } else {
                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "SkipCblk@NoFit(gap=%-5u, cblk=%-5u):[b=%-5u "
                                  "f=%-5u ecnt=%-5u].\n",
                                  gap.ecount,
                                  bidx,
                                  cblk.bucket,
                                  cblk.first_elem,
                                  cblk.ecount);
                }
                /* Move gap block index beyound the current in-use block. */
                SHR_IF_ERR_EXIT
                    (shr_itbm_bkt_list_blk_end_get(unit,
                                                   cblk.bucket,
                                                   cblk.first_elem,
                                                   cblk.ecount,
                                                   cblk.entries_per_bucket,
                                                   &end_bkt,
                                                   &end_idx));
                if (end_idx == (cblk.entries_per_bucket - 1)) {
                    end_bkt = end_bkt + 1;
                    end_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
                } else {
                    end_idx = end_idx + 1;
                }
                gap.bucket = end_bkt;
                gap.first_elem = end_idx;
            }
        } else {
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "%s"), shr_pb_str(mvpb)));
                shr_pb_destroy(mvpb);
                mvpb = NULL;
            }
            /* Gap should never be negative. */
            SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                                (BSL_META_U(unit,
                                            "Gap should not be -ve - rv=%s.\n"),
                                 shr_errmsg(SHR_E_INTERNAL)));
        }
    }
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_printf(mvpb, "\n");
        shr_pb_printf(mvpb, "-------------------------------------------\n");
        shr_pb_printf(mvpb, "rsvd@end   : (b=%-5d f=%-5u ecnt=%-5u c=%-2d "
                            "epbkt=%-1u).\n",
                            (int)rsvd.bucket,
                            rsvd.first_elem,
                            rsvd.ecount,
                            rsvd.comp_id,
                            rsvd.entries_per_bucket);
        shr_pb_printf(mvpb, "gap@end   : (b=%-5d f=%-5u ecnt=%-5u c=%-2d "
                            "epbkt=%-1u).\n",
                            (int)gap.bucket,
                            gap.first_elem,
                            gap.ecount,
                            gap.comp_id,
                            gap.entries_per_bucket);
        shr_pb_printf(mvpb, "dblks_moved : (%-5d).\n", linfo->dblks_moved);
        shr_pb_printf(mvpb, "-------------------------------------------\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(mvpb)));
        shr_pb_destroy(mvpb);
        mvpb = NULL;
    }

    /*
     * Populate the moved blocks current location and the defragementation
     * sequence of the blocks in the function output parameters.
     */
    if (linfo->dblks_moved > 0 && ablks_cnt > 0) {
        SHR_ALLOC(mblk_cur_loc, sizeof(*mblk_cur_loc) * linfo->dblks_moved,
                  "shritbmCompMblkArr");
        SHR_NULL_CHECK(mblk_cur_loc, SHR_E_MEMORY);
        sal_memset(mblk_cur_loc, 0, sizeof(*mblk_cur_loc) * linfo->dblks_moved);

        SHR_ALLOC(bmov_seq, sizeof(*bmov_seq) * linfo->dblks_moved,
                  "shritbmBlkMseqArr");
        SHR_NULL_CHECK(bmov_seq, SHR_E_MEMORY);
        sal_memset(bmov_seq, 0, sizeof(*bmov_seq) * linfo->dblks_moved);

        /* Identify the in-use blocks that have been moved. */
        midx = 0;
        for (bidx = 0; bidx < ablks_cnt; bidx++) {
            dseq = linfo->dseq[bidx];
            if (dseq && dseq->mcount) {
                /*
                 * Update the block's current location in the component
                 * output parameters array.
                 */
                mblk_cur_loc[midx].comp_id = ablks[bidx].comp_id;
                mblk_cur_loc[midx].bucket = ablks[bidx].bucket;
                mblk_cur_loc[midx].first_elem = ablks[bidx].first_elem;
                mblk_cur_loc[midx].ecount = ablks[bidx].ecount;

                /* Update the blocks move sequence. */
                bmov_seq[midx].ecount = dseq->ecount;
                bmov_seq[midx].mcount = dseq->mcount;
                /*
                 * Allocate memory to store the move destination bucket and
                 * first_elem values for this moved block.
                 */
                SHR_ALLOC(bmov_seq[midx].bucket,
                          sizeof(uint32_t) * bmov_seq[midx].mcount,
                         "shritbmBlkMseqBkt");
                SHR_NULL_CHECK(bmov_seq[midx].bucket, SHR_E_MEMORY);
                sal_memset(bmov_seq[midx].bucket, 0,
                           sizeof(uint32_t) * bmov_seq[midx].mcount);

                SHR_ALLOC(bmov_seq[midx].first_elem,
                          sizeof(uint32_t) * bmov_seq[midx].mcount,
                          "shritbmBlkMseqFirst");
                SHR_NULL_CHECK(bmov_seq[midx].first_elem, SHR_E_MEMORY);
                sal_memset(bmov_seq[midx].first_elem, 0,
                           sizeof(uint32_t) * bmov_seq[midx].mcount);
                idx = 0;
                minfo = dseq->minfo;
                while (minfo) {
                    bmov_seq[midx].bucket[idx] = minfo->dbucket;
                    bmov_seq[midx].first_elem[idx++] = minfo->dfirst_elem;
                    minfo = minfo->next;
                }
                /* Increment mblk_cur_loc and bmov_seq array index. */
                midx++;
            }
        }

        if (midx != linfo->dblks_moved) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                                (BSL_META_U(unit,
                                            "midx=%u != dblks_moved=%u - "
                                            "rv=%s.\n"),
                                 midx,
                                 linfo->dblks_moved,
                                 shr_errmsg(SHR_E_INTERNAL)));
        } else {
            /* Return total defragemented blocks count. */
            *dfg_bcount = linfo->dblks_moved;

            /* Return the blocks current location. */
            linfo->dblk_cur_loc = mblk_cur_loc;
            *dfg_bcur_loc = linfo->dblk_cur_loc;

            /* Return the blocks move sequence. */
            linfo->dblk_mseq = bmov_seq;
            *dfg_bmseq = linfo->dblk_mseq;
        }
    } else {
        /* No blocks moved as part of the defragmentation operation. */
        *dfg_bcount = 0;
        *dfg_bcur_loc = NULL;
        *dfg_bmseq = NULL;
    }
exit:
    if (linfo && linfo->dseq) {
        shr_itbm_list_dseq_cleanup(unit,
                                   hdl,
                                   ablks_cnt);
    }
    if (mvpb) {
        if (SHR_FUNC_ERR() && BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            /*
             * Print the defrag sequence before the error to help with
             * error analysis.
             */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "%s"), shr_pb_str(mvpb)));
        }
        shr_pb_destroy(mvpb);
    }
    if (SHR_FUNC_ERR() && linfo) {
        if (bmov_seq) {
            for (midx = 0; midx < linfo->dblks_moved; midx++) {
                if (bmov_seq[midx].mcount) {
                    SHR_FREE(bmov_seq[midx].bucket);
                    SHR_FREE(bmov_seq[midx].first_elem);
                }
            }
            SHR_FREE(bmov_seq);
            linfo->dblk_mseq = NULL;
        }
        SHR_FREE(mblk_cur_loc);
        linfo->dblk_cur_loc = NULL;
        linfo->dblks_moved = 0;
    }
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_defragment(int unit,
                         shr_itbm_list_hdl_t hdl,
                         uint32_t comp_blks_cnt,
                         uint32_t comp_max_ecount,
                         uint32_t tot_inuse_cnt,
                         shr_itbm_blk_t *inuse_blks,
                         uint32_t free_blks_cnt,
                         shr_itbm_blk_t *free_blks,
                         shr_itbm_blk_t largest_fblk,
                         uint32_t *dfg_bcount,
                         shr_itbm_blk_t **dfg_bcur_loc,
                         shr_itbm_defrag_blk_mseq_t **dfg_bmseq)
{
    uint32_t idx; /* Index iterator variable. */
    uint32_t midx = 0; /* Block move array index. */
    uint32_t bidx; /* Block index iterator variable. */
    bool rsvd_in_gap = FALSE; /* Reserved block part of the gap block. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_defrag_seq_t *dseq = NULL; /* Pointer to list defrag sequence. */
    shr_itbm_defrag_blk_mseq_t *bmov_seq = NULL; /* Blocks move sequence. */
    shr_itbm_blk_move_info_t *minfo = NULL; /* Block move info. */
    shr_itbm_blk_t *mblk_cur_loc = NULL;  /* Defragmented component blocks. */
    shr_itbm_blk_t rsvd;   /* Reserved free block. */
    shr_itbm_blk_t gap;    /* Gap between the in-use blocks. */
    shr_itbm_blk_t cblk;   /* Current in-use block. */
    shr_pb_t *pb = NULL;   /* Debug print buffer. */
    shr_pb_t *mvpb = NULL; /* Block moves debug print buffer. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(dfg_bcount, SHR_E_PARAM);
    SHR_NULL_CHECK(dfg_bcur_loc, SHR_E_PARAM);
    SHR_NULL_CHECK(dfg_bmseq, SHR_E_PARAM);

    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List \"block_stat\" is disabled - "
                                        "rv=%s.\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (!tot_inuse_cnt || inuse_blks == NULL) {
        /*
         * If no blocks are in use then there is nothing to defragment, exit
         * the function.
         */
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "No blocks in-use to perform "
                                        "defragmentation - rv=%s.\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (!free_blks_cnt || free_blks == NULL) {
        /*
         * If there are no free blocks in the list, then defragmentation
         * operation cannot be performed. Set the error return status to
         * resource error and exit this function.
         */
        SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                            (BSL_META_U(unit,
                                        "No free blocks to support "
                                        "defragmentation - rv=%s.\n"),
                             shr_errmsg(SHR_E_RESOURCE)));
    }

    /* Initialize the list info pointer. */
    linfo = hdl->linfo;

    /*
     * Initialize total blocks moved during defragmentation operation count to
     * zero.
     */
    linfo->dblks_moved = 0;

    /* Allocate memory to store the blocks defragmentation move sequence. */
    SHR_ALLOC(linfo->dseq, sizeof(*(linfo->dseq)) * tot_inuse_cnt,
              "shritbmDseqPtrArr");
    SHR_NULL_CHECK(linfo->dseq, SHR_E_MEMORY);
    sal_memset(linfo->dseq, 0, sizeof(*(linfo->dseq)) * tot_inuse_cnt);

    /* Initialize and update the reserved block parameters. */
    shr_itbm_blk_t_init(&rsvd);
    rsvd = largest_fblk;
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rsvd: [f=%-5u cnt=%-5u c=%-2d].\n"),
                     rsvd.first_elem,
                     rsvd.ecount,
                     (int)rsvd.comp_id));
    }

    /*
     * Sort the in-use blocks array elements based on their first_elem
     * values.
     */
    sal_qsort((void *)inuse_blks,
              tot_inuse_cnt,
              sizeof(*inuse_blks),
              shr_itbm_blk_base_cmp);

    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "Sorted \"inuse_blks[count=%-5u]\":\n",
                      tot_inuse_cnt);
        shr_pb_printf(pb, "----------------------------------------\n");
        for (idx = 0; idx < tot_inuse_cnt; idx++) {
            shr_pb_printf(pb,
                          "%5u.[f=%-5u c=%-2u ecnt=%-5u]\n",
                          (idx + 1),
                          inuse_blks[idx].first_elem,
                          inuse_blks[idx].comp_id,
                          inuse_blks[idx].ecount);
        }
        shr_pb_printf(pb, "\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }

    /* Sort the free blocks array elements based on their first_elem values. */
    sal_qsort((void *)free_blks,
              free_blks_cnt,
              sizeof(*free_blks),
              shr_itbm_blk_base_cmp);

    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "Sorted \"free_blks[count=%-5u]\":\n",
                      tot_inuse_cnt);
        shr_pb_printf(pb,
                      "----------------------------------------\n");
        for (idx = 0; idx < free_blks_cnt; idx++) {
            shr_pb_printf(pb,
                          "%5u.[f=%-5u c=%-2d ecnt=%-5u]\n",
                          (idx + 1),
                          free_blks[idx].first_elem,
                          (int)free_blks[idx].comp_id,
                          free_blks[idx].ecount);
        }
        shr_pb_printf(pb, "\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }
    /*
     * Initialize the first free block's first_elem number to the index table's
     * first element/entry number.
     */
    shr_itbm_blk_t_init(&gap);
    gap.first_elem = hdl->params.first;
    gap.comp_id = hdl->params.comp_id;
    gap.ecount = 0;

    /* Create block moves print buffer to track the moves. */
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        mvpb = shr_pb_create();
        shr_pb_printf(mvpb, "Capturing block move sequence:\n");
        shr_pb_printf(mvpb, "-------------------------------------------\n");
        shr_pb_printf(mvpb, "rsvd@start  : (f=%-5u ecnt=%-5u c=%-2d)\n",
                            rsvd.first_elem,
                            rsvd.ecount,
                            rsvd.comp_id);
        shr_pb_printf(mvpb, "gap@start   : (f=%-5u ecnt=%-5u c=%-2d)\n",
                            gap.first_elem,
                            gap.ecount,
                            gap.comp_id);
        shr_pb_printf(mvpb, "dblks_moved : (%-5d).\n",
                            linfo->dblks_moved);
        shr_pb_printf(mvpb, "-------------------------------------------\n");
    }

    /*
     * Iterate over the in-use blocks and compress the gaps between the blocks
     * of a component. Blocks that do not belong to the list component that
     * has invoked this defragmentation call are skipped and their location
     * MUST not change.
     */
    for (bidx = 0; bidx < tot_inuse_cnt; bidx++) {
        /* Initialize and get the current in-use block params. */
        shr_itbm_blk_t_init(&cblk);
        cblk = inuse_blks[bidx];

        /* Skip the in-use block if there is a component ID mismatch. */
        if (cblk.comp_id != hdl->params.comp_id) {
            /* Move beyound this block to the next one. */
            gap.first_elem = cblk.first_elem + cblk.ecount;
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                shr_pb_printf(mvpb,
                              "Skip@CompMiss: [cblk=%-5u - c=%-4u f=%-5u "
                              "ecnt=%-5u].\n",
                              bidx,
                              cblk.comp_id,
                              cblk.first_elem,
                              cblk.ecount);
            }
            continue;
        }

        /*
         * Determine the gap between the gap's base and the current in-use
         * (cblk) block's base.
         */
        gap.ecount = cblk.first_elem - gap.first_elem;

        if (cblk.first_elem > rsvd.first_elem
            && gap.first_elem <= rsvd.first_elem) {
            if (gap.first_elem <= rsvd.first_elem && gap.ecount >= cblk.ecount
                && gap.ecount >= rsvd.ecount) {
                /* This gap includes the reserved block section. */
                rsvd_in_gap = TRUE;
            } else if (gap.ecount < comp_max_ecount) {
                /*
                 * Skip the reserved block and move the gap block's first_elem
                 * location to the first element after the reserved block.
                 */
                gap.first_elem = rsvd.first_elem + rsvd.ecount;
                /* Recalculate the gap based on the new location. */
                gap.ecount = cblk.first_elem - gap.first_elem;
            }
        }

        if (gap.ecount == 0) {
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                shr_pb_printf(mvpb,
                              "Skip@GapZero(gap=%-1u): [cblk=%-5u - c=%-4u "
                              "f=%-5u ecnt=%-5u].\n",
                              gap.ecount,
                              bidx,
                              cblk.comp_id,
                              cblk.first_elem,
                              cblk.ecount);
            }
            /* Move gap block index beyound the current in-use block. */
            gap.first_elem = cblk.first_elem + cblk.ecount;
        } else if (gap.ecount > 0) {
            if (cblk.ecount <= gap.ecount) {
                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "Mv@Gap(gap=%-5u rsvd_in_gap=%d): [cblk=%-5u "
                                  "src=%-5u dst=%-5u ecnt=%-5u].\n",
                                  gap.ecount,
                                  rsvd_in_gap,
                                  bidx,
                                  cblk.first_elem,
                                  gap.first_elem,
                                  cblk.ecount);
                }

                /*
                 * Perform the block move operation.
                 *  src     : cblk.first_elem
                 *  dst     : gap.first_elem
                 *  ecount  : cblk.ecount
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_block_move(unit,
                                              hdl,
                                              cblk.first_elem,
                                              gap.first_elem,
                                              cblk.ecount));
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_blk_dseq_update(unit,
                                                   hdl,
                                                   tot_inuse_cnt,
                                                   bidx,
                                                   cblk.first_elem,
                                                   gap.first_elem,
                                                   cblk.ecount));
                /* Update reserved block's new location. */
                if (rsvd_in_gap) {
                    /* Clear the flag settings. */
                    rsvd_in_gap = FALSE;

                    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                        shr_pb_printf(mvpb,
                                      "    Old@Rsvd: [f=%-5u ecnt=%-5u].\n",
                                      rsvd.first_elem,
                                      rsvd.ecount);
                    }
                    /*
                     * e.g.
                     *   _________________
                     *  |_________________|
                     *  |___gap.f_e_______|b[x]
                     *  |___gap/rsvd.f_e__|b[x + 1]
                     *  |___gap/rsvd______|b[x + 2]
                     *  |___gap/rsvd______|b[x + 3]
                     *  |___cblk_0.f_e____|b[x + 4]
                     *  |___cblk_1________|b[x + 5]
                     *  |_________________|
                     *
                     * - cblk_0 and cblk_1, ecount entries moved to gap.f_e
                     *   index location.
                     *   ____________________
                     *  |____________________|
                     *  |___mv:cblk_0________|b[x]
                     *  |___mv:cblk_1________|b[x + 1]
                     *  |___gap/rsvd.f_e_____|b[x + 2]<-|
                     *  |___gap/rsvd_________|b[x + 3]  |-->rsvd.ecount
                     *  |___cblk_0.f_e/rsvd__|b[x + 4]  |
                     *  |___cblk_1/rsvd______|b[x + 5]<-|
                     *  |____________________|
                     * - cblk structure members still have the old first_elem
                     *   and ecount value when computing rsvd.ecount value.
 */
                    rsvd.first_elem = gap.first_elem + cblk.ecount;
                    rsvd.ecount = (cblk.first_elem + cblk.ecount) -
                                        rsvd.first_elem;
                    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                        shr_pb_printf(mvpb,
                                      "    New@Rsvd: [f=%-5u ecnt=%-5u].\n",
                                      rsvd.first_elem,
                                      rsvd.ecount);
                    }
                }
                /* Both reserved and the gap blocks are same now. */
                gap.first_elem += cblk.ecount;
                /* Increment list blocks moved count. */
                linfo->dblks_moved++;
            } else if (cblk.ecount <= rsvd.ecount) {
                /*
                 * Perform the block move operation.
                 * Use reserved block as an hop, prior to moving the in-use
                 * block to it's final location.
                 *  Src     : cblk.first_elem
                 *  Dst     : rsvd.first_elem
                 *  ecount  : cblk.ecount
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_block_move(unit,
                                              hdl,
                                              cblk.first_elem,
                                              rsvd.first_elem,
                                              cblk.ecount));
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_blk_dseq_update(unit,
                                                   hdl,
                                                   tot_inuse_cnt,
                                                   bidx,
                                                   cblk.first_elem,
                                                   rsvd.first_elem,
                                                   cblk.ecount));

                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "*Mv@ToRsvd: [cblk=%-5u src=%-5u dst=%-5u "
                                  "rsvd_ecnt=%-5u].\n",
                                  bidx,
                                  cblk.first_elem,
                                  rsvd.first_elem,
                                  rsvd.ecount);
                }
                /*
                 * Perform the block move operation.
                 * Move from the reserved block location to the gap region.
                 *  Src     : rsvd.first_elem
                 *  Dst     : gap.first_elem
                 *  ecount  : cblk.ecount
                 */
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_block_move(unit,
                                              hdl,
                                              rsvd.first_elem,
                                              gap.first_elem,
                                              cblk.ecount));
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_blk_dseq_update(unit,
                                                   hdl,
                                                   tot_inuse_cnt,
                                                   bidx,
                                                   rsvd.first_elem,
                                                   gap.first_elem,
                                                   cblk.ecount));

                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "Mv@FromRsvd: [cblk=%-5u src=%-5u dst=%-5u "
                                  "ecnt=%-5u].\n",
                                  bidx,
                                  rsvd.first_elem,
                                  gap.first_elem,
                                  cblk.ecount);
                }
                /* Move gap block index beyound the current in-use block. */
                gap.first_elem += cblk.ecount;
                /* Increment list blocks moved count. */
                linfo->dblks_moved++;
            } else {
                if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    shr_pb_printf(mvpb,
                                  "Skip@NoFit(gap=%-5u): [cblk=%-5u f=%-5u "
                                  "ecnt=%-5u].\n",
                                  gap.ecount,
                                  bidx,
                                  cblk.first_elem,
                                  cblk.ecount);
                }
                /* Move gap block index beyound the current in-use block. */
                gap.first_elem = cblk.first_elem + cblk.ecount;
            }
        } else {
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "%s"), shr_pb_str(mvpb)));
                shr_pb_destroy(mvpb);
                mvpb = NULL;
            }
            /* Gap should never be negative. */
            SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                                (BSL_META_U(unit,
                                            "Gap should not be -ve - rv=%s.\n"),
                                 shr_errmsg(SHR_E_INTERNAL)));
        }
    }
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_printf(mvpb, "\n");
        shr_pb_printf(mvpb, "-------------------------------------------\n");
        shr_pb_printf(mvpb, "rsvd@end   : (f=%-5u ecnt=%-5u c=%-2d).\n",
                            rsvd.first_elem,
                            rsvd.ecount,
                            rsvd.comp_id);
        shr_pb_printf(mvpb, "gap@end   : (f=%-5u ecnt=%-5u c=%-2d).\n",
                            gap.first_elem,
                            (hdl->params.last - gap.first_elem) + 1,
                            gap.comp_id);
        shr_pb_printf(mvpb, "dblks_moved : (%-5d).\n",
                            linfo->dblks_moved);

        shr_pb_printf(mvpb, "-------------------------------------------\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(mvpb)));
        shr_pb_destroy(mvpb);
        mvpb = NULL;
    }

    /*
     * Populate the moved blocks current location and the defragementation
     * sequence of the blocks in the function output parameters.
     */
    if (linfo->dblks_moved > 0 && tot_inuse_cnt > 0) {
        SHR_ALLOC(mblk_cur_loc,
                  sizeof(*mblk_cur_loc) * linfo->dblks_moved,
                  "shritbmCompMblkArr");
        SHR_NULL_CHECK(mblk_cur_loc, SHR_E_MEMORY);
        sal_memset(mblk_cur_loc, 0, sizeof(*mblk_cur_loc) * linfo->dblks_moved);

        SHR_ALLOC(bmov_seq, sizeof(*bmov_seq) * linfo->dblks_moved,
                  "shritbmBlkMseqArr");
        SHR_NULL_CHECK(bmov_seq, SHR_E_MEMORY);
        sal_memset(bmov_seq, 0, sizeof(*bmov_seq) * linfo->dblks_moved);

        /* Identify the in-use blocks that have been moved. */
        midx = 0;
        for (bidx = 0; bidx < tot_inuse_cnt; bidx++) {
            dseq = linfo->dseq[bidx];
            if (dseq && dseq->mcount) {
                /*
                 * Update the block's current location in the component
                 * output parameters array.
                 */
                mblk_cur_loc[midx].comp_id = inuse_blks[bidx].comp_id;
                mblk_cur_loc[midx].bucket = inuse_blks[bidx].bucket;
                mblk_cur_loc[midx].first_elem = inuse_blks[bidx].first_elem;
                mblk_cur_loc[midx].ecount = inuse_blks[bidx].ecount;

                /* Update the blocks move sequence. */
                bmov_seq[midx].ecount = dseq->ecount;
                bmov_seq[midx].mcount = dseq->mcount;
                /*
                 * Initialize bucket pointer to null for non-indexed buckets
                 * type lists.
                 */
                bmov_seq[midx].bucket = NULL;
                /*
                 * Allocate memory to store the move destination first_elem
                 * values for this moved block.
                 */
                SHR_ALLOC(bmov_seq[midx].first_elem,
                          sizeof(uint32_t) * bmov_seq[midx].mcount,
                          "shritbmBlkMseq");
                SHR_NULL_CHECK(bmov_seq[midx].first_elem, SHR_E_MEMORY);
                sal_memset(bmov_seq[midx].first_elem, 0,
                           sizeof(uint32_t) * bmov_seq[midx].mcount);
                idx = 0;
                minfo = dseq->minfo;
                while (minfo) {
                    bmov_seq[midx].first_elem[idx++] = minfo->dfirst_elem;
                    minfo = minfo->next;
                }
                /* Increment mblk_cur_loc and bmov_seq array index. */
                midx++;
            }
        }

        if (midx != linfo->dblks_moved) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                                (BSL_META_U(unit,
                                            "midx=%u != dblks_moved=%u - "
                                            "rv=%s"),
                                 midx,
                                 linfo->dblks_moved,
                                 shr_errmsg(SHR_E_INTERNAL)));
        } else {
            /* Return total defragemented blocks count. */
            *dfg_bcount = linfo->dblks_moved;

            /* Return the blocks current location. */
            linfo->dblk_cur_loc = mblk_cur_loc;
            *dfg_bcur_loc = linfo->dblk_cur_loc;

            /* Return the blocks move sequence. */
            linfo->dblk_mseq = bmov_seq;
            *dfg_bmseq = linfo->dblk_mseq;
        }
    } else {
        /* No blocks moved as part of the defragmentation operation. */
        *dfg_bcount = 0;
        *dfg_bcur_loc = NULL;
        *dfg_bmseq = NULL;
    }
exit:
    if (linfo && linfo->dseq) {
        shr_itbm_list_dseq_cleanup(unit,
                                   hdl,
                                   tot_inuse_cnt);
    }
    if (mvpb) {
        if (SHR_FUNC_ERR() && BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            /*
             * Print the defrag sequence before the error to help with
             * error analysis.
             */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "%s"), shr_pb_str(mvpb)));
        }
        shr_pb_destroy(mvpb);
    }
    if (SHR_FUNC_ERR() && linfo) {
        if (bmov_seq) {
            for (midx = 0; midx < linfo->dblks_moved; midx++) {
                if (bmov_seq[midx].mcount) {
                    SHR_FREE(bmov_seq[midx].first_elem);
                }
            }
            SHR_FREE(bmov_seq);
            linfo->dblk_mseq = NULL;
        }
        SHR_FREE(mblk_cur_loc);
        linfo->dblk_cur_loc = NULL;
        linfo->dblks_moved = 0;
    }
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_in_use_blk_match_check(int unit,
                                     shr_itbm_list_params_t params,
                                     const shr_itbm_list_edata_t *edata,
                                     uint32_t bucket,
                                     uint32_t first,
                                     uint32_t count,
                                     uint32_t zoff)
{
    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(edata, SHR_E_PARAM);

    if (params.buckets == TRUE) {
        /* Verify input first is the first element of the allocated block. */
        if (edata[bucket - zoff].eprop == NULL
            || SHR_BITGET(edata[bucket - zoff].eprop, first) == 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                                (BSL_META_U(unit,
                                            "Input blk-(bkt=0x%x, first="
                                            "0x%x, count=%u) not in-use by "
                                            "comp_id=%u - rv=%s.\n"),
                                 bucket,
                                 first,
                                 count,
                                 params.comp_id,
                                 shr_errmsg(SHR_E_NOT_FOUND)));
        }

        /*
         * Verify the input count value matches the block size value stored
         * during block allocation.
         */
        if (edata[(bucket - zoff)].blk_sz == NULL
            || edata[(bucket - zoff)].blk_sz[first] != count) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Input \"count=%u\" does not match "
                                            "expected \"count=%u\" value for "
                                            "this block:[bkt=%u f=%u zoff=%u] "
                                            "- rv=%s.\n"),
                                 count, edata[bucket - zoff].blk_sz[first],
                                 bucket, first, zoff, shr_errmsg(SHR_E_PARAM)));
        }
    } else {
        /*
         * Verify input first parameter is actually the first element of the
         * allocated block.
         */
        if (edata[first - zoff].eprop == NULL
            || SHR_BITGET(edata[first - zoff].eprop,
                          SHR_ITBM_LIST_BLK_FIRST_ELEM) == 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                                (BSL_META_U(unit,
                                            "Block with the input \"first=%u "
                                            "and count=%u\" elements not "
                                            "found - rv=%s.\n"),
                                 first,
                                 count,
                                 shr_errmsg(SHR_E_NOT_FOUND)));
        }
        /* Verify input count value is valid for this block. */
        if (edata[(first - zoff)].blk_sz == NULL
            || edata[(first - zoff)].blk_sz[SHR_ITBM_LIST_BKT_NONE] != count) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Input \"count=%u\" value does "
                                            "not match expected \"count=%u\" "
                                            "value - rv=%s.\n"),
                             count,
                             edata[first - zoff].blk_sz[SHR_ITBM_LIST_BKT_NONE],
                             shr_errmsg(SHR_E_PARAM)));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
shr_itbm_list_blk_params_validate(int unit,
                                  shr_itbm_list_hdl_t hdl,
                                  uint32_t bucket,
                                  uint32_t first,
                                  uint32_t count)
{
    uint32_t rbkts = 0; /* Required buckets count. */
    shr_itbm_list_params_t params; /* List params. */
    shr_itbm_lattr_t *lattr = NULL; /* Pointer to list attribute. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);

    /* Get the list params. */
    shr_itbm_list_params_t_init(&params);
    params = hdl->params;

    /*
     * Get the list attribute details, for additional input parameter
     * validations.
     */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_attr_get(unit,
                                hdl->linfo,
                                hdl->aid,
                                &lattr));
    SHR_NULL_CHECK(lattr, SHR_E_INTERNAL);

    /*
     * 'block_stat' must be enabled for the list while creating it,
     * to allocate blocks with more than one element.
     */
    if (count > 1 && params.block_stat == FALSE) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List created with \"block_stat=0\", "
                                        "so \"count=%u\" is invalid, value "
                                        "must be \"1\" - rv=%s.\n"),
                             count,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /*
     * Verify elements count is non-zero and is not greater than the total
     * elements supported by the list.
     */
    if (count == 0 || count > lattr->tot_elems) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "\"count=%u\" is invalid - rv=%s.\n"),
                             count,
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (params.buckets == TRUE) {
        if (bucket < params.first || bucket > params.last) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Invalid bucket=0x%x value, "
                                            "valid range=\"0x%x to 0x%x\" "
                                            "- rv=%s.\n"),
                                 bucket,
                                 params.first,
                                 params.last,
                                 shr_errmsg(SHR_E_PARAM)));
        }
        if (first >= params.entries_per_bucket) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Invalid first=0x%x value, valid "
                                            "range=\"0 to %u\" - rv=%s.\n"),
                                 first,
                                 (params.entries_per_bucket - 1),
                                 shr_errmsg(SHR_E_PARAM)));
        }

        /*
         * Determine number of buckets required to complete this alloc request.
         * For resize, this condition is always expected to fail as the block
         * would have allready been allocated successfully with the input
         * first + count values. The check for "new_count" is done separately
         * in the resize function.
         */
        rbkts = shr_itbm_list_blk_num_bkts_get(count,
                                               first,
                                               params.entries_per_bucket,
                                               FALSE);
        if (((bucket + rbkts) - 1) > params.last) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Input block with count=%u "
                                            "elements, total no. of bkts "
                                            "req=%u, with block base@\""
                                            "bucket=0x%x & first=0x%x\", the "
                                            "end bucket of the block=0x%x "
                                            "exceeds the valid bkts "
                                            "range=\"0x%x to 0x%x\" - "
                                            "rv=%s.\n"),
                                 count,
                                 rbkts,
                                 bucket,
                                 first,
                                 (bucket + rbkts) - 1,
                                 params.first,
                                 params.last,
                                 shr_errmsg(SHR_E_PARAM)));
        }
    } else {
        /* Validate first element number value.  */
        if (first < params.first || first > params.last) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Invalid input: first=%u, valid "
                                            "range=\"%u To %u\" - rv=%s."
                                            "\n"),
                                 first,
                                 params.first,
                                 params.last,
                                 shr_errmsg(SHR_E_PARAM)));
        }
        /*
         * Verify input first and count number of elements are within the
         * valid range (last) of the list.
         */
        if (((first + count) - 1) > params.last) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Block (first=%u + count=%u) "
                                            "exceeds valid range=\"%u to "
                                            "%u\" - rv=%s.\n "),
                                 first,
                                 count,
                                 params.first,
                                 params.last,
                                 shr_errmsg(SHR_E_PARAM)));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
itbm_list_resize(int unit,
                 shr_itbm_list_hdl_t hdl,
                 shr_itbm_lattr_t *lattr,
                 uint32_t new_first,
                 uint32_t new_last)
{
    shr_itbm_list_params_t aparams; /* List attributes params. */
    shr_itbm_list_params_t nparams; /* new list params. */
    shr_itbm_lattr_t *alist = NULL; /* Pointer to list attributes params. */
    uint32_t pt_ent_added = 0, pt_ent_deleted = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(lattr, SHR_E_PARAM);

    alist = hdl->linfo->alist;
    SHR_NULL_CHECK(alist, SHR_E_INTERNAL);

    /* Additional validation to be done for indexed bucket type lists. */
    if (hdl->params.buckets) {
        while (alist) {
            shr_itbm_list_params_t_init(&aparams);
            aparams = alist->params;
            shr_itbm_list_params_t_init(&nparams);
            nparams = hdl->params;
            if (alist->aid == hdl->aid
                || (aparams.reverse != nparams.reverse
                    && aparams.entries_per_bucket != nparams.entries_per_bucket)
                ) {
                /*
                 * Skip list self attributes and lists that do not have same
                 * reverse and entries_per_bucket config.
                 */
                alist = alist->next;
                continue;
            }
            /* assign new index range and check for overlap. */
            nparams.first = new_first;
            nparams.last = new_last;
            if (itbm_ibkt_list_first_last_overlap(alist->params, nparams)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "The new_first=%u, new_last=%u index "
                                        "range overlaps with another list: "
                                        "(comp_id=%u table_id=%u epbkt=%u "
                                        "reverse=%u min=%u max=%u first=%u "
                                        "last=%u).\n"),
                             nparams.first, nparams.last, aparams.comp_id,
                             aparams.table_id,
                             aparams.entries_per_bucket,
                             aparams.reverse, aparams.min,
                             aparams.max, aparams.first,
                             aparams.last));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            alist = alist->next;
        }
    }

    if (hdl->params.first != new_first) {
        if (new_first < hdl->params.first) {
            pt_ent_added = (hdl->params.first - new_first);
        } else {
            pt_ent_deleted = (new_first - hdl->params.first);
        }
    }
    if (hdl->params.last != new_last) {
        if (new_last > hdl->params.last) {
            pt_ent_added += (new_last - hdl->params.last);
        } else {
            pt_ent_deleted += (hdl->params.last - new_last);
        }
    }
    /*
     * Update list managed elements range, total elements and free elements
     * count.
     */
    hdl->params.first = new_first;
    hdl->params.last = new_last;
    lattr->params.first = new_first;
    lattr->params.last = new_last;
    if (hdl->params.buckets) {
        lattr->stat.free_count += (pt_ent_added
                                    * lattr->params.entries_per_bucket);
        lattr->stat.free_count -= (pt_ent_deleted
                                    * lattr->params.entries_per_bucket);
        lattr->tot_elems += (pt_ent_added
                                    * lattr->params.entries_per_bucket);
        lattr->tot_elems -= (pt_ent_deleted
                                    * lattr->params.entries_per_bucket);
    } else {
        lattr->stat.free_count += pt_ent_added;
        lattr->stat.free_count -= pt_ent_deleted;
        lattr->tot_elems += pt_ent_added;
        lattr->tot_elems -= pt_ent_deleted;
    }

    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "LID=%u lattr(aid=%u): first=%u last=%u bkts=%u "
                               "epbkt=%u rev=%u stat:free_count=%u "
                               "tot_elems=%u pt_ent_added=%u pt_ent_deleted=%u"
                               ".\n"),
                   hdl->linfo->list_id, lattr->aid, lattr->params.first,
                   lattr->params.last, hdl->params.buckets,
                   lattr->params.entries_per_bucket, lattr->params.reverse,
                   lattr->stat.free_count, lattr->stat.free_count,
                   pt_ent_added, pt_ent_deleted));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
itbm_shrd_ibkt_list_defrag(int unit,
                           shr_itbm_list_hdl_t hdl)
{
    int rv = SHR_E_INTERNAL; /* Return value. */
    uint32_t idx = 0; /* Index iterator. */
    uint32_t ablks_cnt = 0; /* Total blocks in-use count. */
    uint32_t comp_blks_cnt = 0; /* Total blocks allocated by this component. */
    uint32_t fblks_cnt = 0; /* Total free blocks count. */
    uint32_t comp_max_ecount = 0; /* Max block size in-use by the component. */
    uint32_t count = 0;
    uint32_t midx = 0; /* Block move index. */
    shr_itbm_blk_t *ablk_arr = NULL; /* Allocated in-use blocks array. */
    shr_itbm_blk_t *fblk_arr = NULL; /* Free blocks array. */
    shr_itbm_blk_t largest_fblk; /* Biggest free block available. */
    shr_pb_t *pb = NULL; /* Debug print buffer. */
    shr_itbm_blk_t *dfgl_ablk_arr = NULL; /* Allocated in-use blocks array. */
    shr_itbm_blk_t *dfgl_fblk_arr = NULL; /* Free blocks array. */
    shr_itbm_blk_t *blk_cur_loc = NULL; /* Ptr to list blks current location. */
    shr_itbm_defrag_blk_mseq_t *blk_mov_seq = NULL; /* Blks move sequence. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);

    /* Get the allocated and in-use block parameters details. */
    SHR_ALLOC(ablk_arr,
              sizeof(*ablk_arr) * hdl->linfo->tot_blks_alloced,
              "shritbmInuseBlkArr");
    SHR_NULL_CHECK(ablk_arr, SHR_E_MEMORY);
    sal_memset(ablk_arr, 0,
               sizeof(*ablk_arr) * hdl->linfo->tot_blks_alloced);
    /* Get the total in-use blocks count and the block details. */
    rv = shr_itbm_bkt_list_inuse_blks_get(unit,
                                          hdl,
                                          hdl->linfo->tot_blks_alloced,
                                          ablk_arr,
                                          &ablks_cnt,
                                          &comp_blks_cnt,
                                          &comp_max_ecount);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "inuse_blks_get() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && ablks_cnt > 0) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "InUseBlks(l_epbkt=%-1u): [Alloced=%-5u Act_InUse=%-5u "
                      "CompIDMatched=%-5u].\n",
                      hdl->params.entries_per_bucket,
                      hdl->linfo->tot_blks_alloced,
                      ablks_cnt,
                      comp_blks_cnt);
        shr_pb_printf(pb,
                      "==============================================="
                      "===\n");
        for (idx = 0; idx < ablks_cnt; idx++) {
            shr_pb_printf(pb,
                          "%5u.[epbkt=%-1u b=%-5d f=%-5u c=%-2u sz=%-5u].\n",
                          idx,
                          ablk_arr[idx].entries_per_bucket,
                          (int)ablk_arr[idx].bucket,
                          ablk_arr[idx].first_elem,
                          ablk_arr[idx].comp_id,
                          ablk_arr[idx].ecount);
        }
        shr_pb_printf(pb, "\n");
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }

    /* Get the total free blocks count and the block details. */
    shr_itbm_blk_t_init(&largest_fblk);
    rv = shr_itbm_bkt_list_free_blks_get(unit,
                                         hdl,
                                         ablks_cnt,
                                         ablk_arr,
                                         &fblks_cnt,
                                         &fblk_arr,
                                         &largest_fblk);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "free_blks_get() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    /*
     * Check if free blocks are available to perform the list
     * defragmentation.
     */
    if (fblks_cnt && fblk_arr) {
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            pb = shr_pb_create();
            shr_pb_printf(pb,
                          "Free_Blks: [fblks_cnt=%-5u].\n",
                          fblks_cnt);
            shr_pb_printf(pb,
                          "==========================================\n");
            for (idx = 0; idx < fblks_cnt; idx++) {
                shr_pb_printf(pb,
                              "%5u.[epbkt=%-1u b=%-5d f=%-5u c=%-2d "
                              "ecnt=%-5u].\n",
                              idx,
                              fblk_arr[idx].entries_per_bucket,
                              (int)fblk_arr[idx].bucket,
                              fblk_arr[idx].first_elem,
                              (int)fblk_arr[idx].comp_id,
                              fblk_arr[idx].ecount);
            }
            shr_pb_printf(pb, "\n");
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
            pb = NULL;
        }
    } else {
        /* Return no free blocks were found. */
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    if (hdl->params.reverse == TRUE) {
        rv = shr_itbm_bkt_list_defragment_rev(unit,
                                              hdl,
                                              comp_blks_cnt,
                                              comp_max_ecount,
                                              ablks_cnt,
                                              ablk_arr,
                                              fblks_cnt,
                                              fblk_arr,
                                              largest_fblk,
                                              &count,
                                              &blk_cur_loc,
                                              &blk_mov_seq);

    } else {
        rv = shr_itbm_bkt_list_defragment(unit,
                                          hdl,
                                          comp_blks_cnt,
                                          comp_max_ecount,
                                          ablks_cnt,
                                          ablk_arr,
                                          fblks_cnt,
                                          fblk_arr,
                                          largest_fblk,
                                          &count,
                                          &blk_cur_loc,
                                          &blk_mov_seq);
    }
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_defragment() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    SHR_FREE(hdl->linfo->dblk_cur_loc);
    if (blk_mov_seq) {
        for (midx = 0; midx < hdl->linfo->dblks_moved; midx++) {
            if (blk_mov_seq[midx].mcount) {
                SHR_FREE(blk_mov_seq[midx].bucket);
                SHR_FREE(blk_mov_seq[midx].first_elem);
            }
        }
        SHR_FREE(hdl->linfo->dblk_mseq);
    }
exit:
    SHR_FREE(ablk_arr);
    SHR_FREE(fblk_arr);
    SHR_FREE(dfgl_ablk_arr);
    SHR_FREE(dfgl_fblk_arr);
    if (hdl && hdl->linfo) {
        hdl->linfo->dblks_moved = 0;
    }
    SHR_FUNC_EXIT();
}

static int
itbm_ibkt_list_free_blk_get(int unit,
                            shr_itbm_list_hdl_t hdl,
                            shr_itbm_blk_t *largest)
{
    int rv = SHR_E_INTERNAL; /* Return value. */
    uint32_t idx = 0; /* Index iterator. */
    uint32_t ablks_cnt = 0; /* Total blocks in-use count. */
    uint32_t comp_blks_cnt = 0; /* Total blocks allocated by this component. */
    uint32_t fblks_cnt = 0; /* Total free blocks count. */
    uint32_t comp_max_ecount = 0; /* Max block size in-use by the component. */
    shr_itbm_blk_t *ablk_arr = NULL; /* Allocated in-use blocks array. */
    shr_itbm_blk_t *fblk_arr = NULL; /* Free blocks array. */
    shr_itbm_blk_t largest_fblk; /* Biggest free block available. */
    shr_pb_t *pb = NULL; /* Debug print buffer. */
    shr_itbm_blk_t *dfgl_ablk_arr = NULL; /* Allocated in-use blocks array. */
    shr_itbm_blk_t *dfgl_fblk_arr = NULL; /* Free blocks array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(largest, SHR_E_PARAM);

    /*
     * From the defragmented list, get the allocated and in-use block
     * parameters details.
     */
    SHR_ALLOC(dfgl_ablk_arr,
              sizeof(*dfgl_ablk_arr) * hdl->linfo->tot_blks_alloced,
              "shritbmDfglInuseBlkArr");
    SHR_NULL_CHECK(dfgl_ablk_arr, SHR_E_MEMORY);
    sal_memset(dfgl_ablk_arr, 0,
               sizeof(*dfgl_ablk_arr) * hdl->linfo->tot_blks_alloced);
    /* Get the total in-use blocks count and the block details. */
    rv = shr_itbm_bkt_list_inuse_blks_get(unit, hdl,
                                          hdl->linfo->tot_blks_alloced,
                                          dfgl_ablk_arr, &ablks_cnt,
                                          &comp_blks_cnt,
                                          &comp_max_ecount);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "dfgl:inuse_blks_get() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE) && ablks_cnt > 0) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "dfgl:InUseBlks(l_epbkt=%-1u): [Alloced=%-5u "
                      "Act_InUse=%-5u CompIDMatched=%-5u].\n",
                      hdl->params.entries_per_bucket,
                      hdl->linfo->tot_blks_alloced, ablks_cnt,
                      comp_blks_cnt);
        shr_pb_printf(pb, "==============================================="
                          "=====\n");
        for (idx = 0; idx < ablks_cnt; idx++) {
            shr_pb_printf(pb, "%5u.[epbkt=%-1u b=%-5d f=%-5u c=%-2u "
                              "sz=%-5u].\n",
                          idx, dfgl_ablk_arr[idx].entries_per_bucket,
                          (int)dfgl_ablk_arr[idx].bucket,
                          dfgl_ablk_arr[idx].first_elem,
                          dfgl_ablk_arr[idx].comp_id,
                          dfgl_ablk_arr[idx].ecount);
        }
        shr_pb_printf(pb, "\n");
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }

    /*
     * From the defragmented list, get the total free blocks count and the
     * block details.
     */
    shr_itbm_blk_t_init(&largest_fblk);
    rv = shr_itbm_bkt_list_free_blks_get(unit, hdl, ablks_cnt,
                                         dfgl_ablk_arr, &fblks_cnt,
                                         &dfgl_fblk_arr, &largest_fblk);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "dfgl:free_blks_get() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    if (fblks_cnt && dfgl_fblk_arr) {
        /* Update the largest block details in the out parameter. */
        sal_memcpy(largest, &largest_fblk, sizeof(*largest));
        if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            pb = shr_pb_create();
            shr_pb_printf(pb,
                          "dfgl_free_blks: [fblks_cnt=%-5u large_fblk: "
                          "epbkt=%u b=%d f=%u c=%d ecnt=%u].\n",
                          fblks_cnt, largest_fblk.entries_per_bucket,
                          (int)largest_fblk.bucket,
                          largest_fblk.first_elem,
                          (int)largest_fblk.comp_id,
                          largest_fblk.ecount);
            shr_pb_printf(pb, "========================================="
                              "=====\n");
            for (idx = 0; idx < fblks_cnt; idx++) {
                shr_pb_printf(pb,
                              "%5u.[epbkt=%-1u b=%-5d f=%-5u c=%-2d "
                              "ecnt=%-5u].\n",
                              idx, dfgl_fblk_arr[idx].entries_per_bucket,
                              (int)dfgl_fblk_arr[idx].bucket,
                              dfgl_fblk_arr[idx].first_elem,
                              (int)dfgl_fblk_arr[idx].comp_id,
                              dfgl_fblk_arr[idx].ecount);
            }
            shr_pb_printf(pb, "\n");
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
            pb = NULL;
        }
    } else {
        /* No free blocks were found. */
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FREE(ablk_arr);
    SHR_FREE(fblk_arr);
    SHR_FREE(dfgl_ablk_arr);
    SHR_FREE(dfgl_fblk_arr);
    SHR_FUNC_EXIT();
}

static int
itbm_list_defrag_cleanup(int unit,
                         shr_itbm_list_hdl_t hdl)
{
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata_bk, SHR_E_PARAM);

    rv = shr_itbm_list_defrag_abort(unit, hdl);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_MEMORY) {
            /* Reset list to the default state on memory error. */
            shr_itbm_list_defrag_abort_error_cleanup(unit, hdl);
        }
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "shr_itbm_list_defrag_abort() -"
                                        " rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    rv = shr_itbm_list_edata_backup_destroy(unit, hdl);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "shr_itbm_list_edata_backup_destroy() -"
                                        "rv=%s.\n"),
                             shr_errmsg(rv)));
    }
exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public functions
 */
void
shr_itbm_list_params_t_init(shr_itbm_list_params_t *params)
{
    if (params) {
        sal_memset(params, 0, sizeof(*params));
        params->unit = SHR_ITBM_UNIT_UNKNOWN;
    }
    return;
}

void
shr_itbm_list_stat_t_init(shr_itbm_list_stat_t *stat)
{
    if (stat) {
        sal_memset(stat, 0, sizeof(*stat));
    }
    return;
}

void
shr_itbm_blk_t_init(shr_itbm_blk_t *blk)
{
    if (blk) {
        sal_memset(blk, 0, sizeof(*blk));
        blk->comp_id = SHR_ITBM_INVALID;
        blk->bucket = SHR_ITBM_INVALID;
        blk->first_elem = SHR_ITBM_INVALID;
    }
    return;
}

int
shr_itbm_list_create(shr_itbm_list_params_t params,
                     char *name,
                     shr_itbm_list_hdl_t *hdl)
{
    int unit = params.unit; /* Initialize unit number local variable. */
    int rv = SHR_E_INTERNAL; /* Return value. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);

    /* Validate unit input parameter. */
    if (unit < 0 || unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid (unit=%u) no. - rv=%s.\n"),
                             unit,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /* Validate index table elements/entries range. */
    if ((params.first == 0 && params.last == 0)
        || (params.min == 0 && params.max == 0)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid (first=%u, last=%u) || "
                                        "(min=%u, max=%u) value - rv=%s.\n"),
                             params.first,
                             params.last,
                             params.min,
                             params.max,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Validate first input parameter value. */
    if (params.first < params.min || params.first > params.max) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid (first=%u) value - rv=%s.\n"),
                             params.first,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /* Validate last input parameter value. */
    if (params.last < params.min || params.last > params.max) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid (last=%u) value - rv=%s.\n"),
                             params.last,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Validate buckets and entries_per_bucket input parameters. */
    if ((params.buckets == TRUE
         && (params.entries_per_bucket != SHR_ITBM_LIST_BKT_ONE_ENTRY
             && params.entries_per_bucket != SHR_ITBM_LIST_BKT_TWO_ENTRY
             && params.entries_per_bucket != SHR_ITBM_LIST_BKT_FOUR_ENTRY))
        || (params.buckets == FALSE
            && (params.entries_per_bucket != SHR_ITBM_LIST_BKT_NONE))) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid \"buckets=%u\" and "
                                        "\"entries_per_bucket=%u\" values "
                                        "supplied - rv=%s.\n"),
                             params.buckets,
                             params.entries_per_bucket,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Initialize address pointed by the handle to NULL. */
    *hdl = NULL;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[Lparams Name=%s]:"
                              "(u=%u tab_id=%u min=%u max=%u f=%u "
                              "l=%u comp_id=%u BS=%s Bkts=%s epbkt=%u "
                              "rev=%s.\n"),
                   name,
                   params.unit,
                   params.table_id,
                   params.min,
                   params.max,
                   params.first,
                   params.last,
                   params.comp_id,
                   params.block_stat ? "T" : "F",
                   params.buckets ? "T" : "F",
                   params.entries_per_bucket,
                   params.reverse ? "T" : "F"));
    }

    /*
     * Allocate and initialize the unit control structure if it does not
     * exist.
     */
    if (!ctrl[unit]) {
        SHR_IF_ERR_EXIT
            (shr_itbm_ctrl_init(unit));
    }
    /* Take the unit control structure lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(ctrl[unit]->glock, SAL_MUTEX_FOREVER));

    /* Add the new indexed list to the unit linked-list. */
    rv = shr_itbm_list_add(unit, params, name, hdl);
    if (rv != SHR_E_NONE) {
        sal_mutex_give(ctrl[unit]->glock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_add()  - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    rv = sal_mutex_give(ctrl[unit]->glock);
    if (SHR_FAILURE(rv)) {
        /* Free the created list and it's resources. */
        (void)shr_itbm_list_delete(unit, *hdl);
        SHR_ERR_EXIT(rv);
    }

    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shritbm_bmp_msg(pb,
                        (*hdl)->linfo->lprop,
                        SHR_ITBM_LIST_PROP_COUNT,
                        lprop_msg);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Done: [LID=%u Comp_ID=%u Attr_ID=%u "
                                "blk_stat=%s bkts=%s epbkt=%u rev=%s "
                                "lprop:%s] - rv=%s.\n"),
                     (*hdl)->linfo->list_id,
                     (*hdl)->params.comp_id,
                     (*hdl)->aid,
                     (*hdl)->params.block_stat ? "T" : "F" ,
                     (*hdl)->params.buckets ? "T" : "F",
                     (*hdl)->params.entries_per_bucket,
                     (*hdl)->params.reverse ? "T" : "F",
                     shr_pb_str(pb),
                     shr_errmsg(rv)));
        shr_pb_destroy(pb);
    }
exit:
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_destroy(shr_itbm_list_hdl_t hdl)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    int rv = SHR_E_INTERNAL; /* Return value. */
    uint16_t list_id = 0; /* List ID. */
    uint32_t comp_id = SHR_ITBM_INVALID; /* Component ID. */
    uint32_t aid = SHR_ITBM_INVALID; /* Attribute ID. */
    shr_itbm_list_stat_t stat; /* List statistics. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);

    /* Validate unit input parameter. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid (unit=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number in the local variable. */
    unit = hdl->params.unit;

    /*
     * If the caller has already destroyed all the lists on this unit, then
     * there is nothing to do exit the function.
     */
    if (!ctrl[unit]) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                            (BSL_META_U(unit,
                                        "No lists on (unit=%u) to destroy - "
                                        "rv=%s.\n"),
                             unit,
                             shr_errmsg(SHR_E_NOT_FOUND)));
    }

    /* If list defragmentation started and in-progress, then return busy. */
    if (hdl->linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }
    /* Get the list statistics. */
    shr_itbm_list_stat_t_init(&stat);
    rv = shr_itbm_list_attr_stat_get(unit, hdl, &stat);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "LID=%u shr_itbm_list_attr_stat_get() "
                                        " - rv=%s.\n"),
                             hdl->linfo->list_id,
                             shr_errmsg(rv)));
    }

    /*
     * List elements still in-use, elements must be freed before destroying the
     * list.
     */
    if (stat.in_use_count && hdl->linfo->lref_cnt == 1) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u: elements - inuse_ecount=%u "
                                        "- rv=%s.\n"),
                             hdl->linfo->list_id,
                             stat.in_use_count,
                             shr_errmsg(SHR_E_BUSY)));
    }

    /* Update list ID and comp_id values in local variables. */
    list_id = hdl->linfo->list_id;
    comp_id = hdl->params.comp_id;
    aid = hdl->aid;

    /* Take the unit global list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(ctrl[unit]->glock, SAL_MUTEX_FOREVER));
    /*
     * Delete the list from this unit's linked list and free resources
     * allocated for this list.
     */
    rv = shr_itbm_list_delete(unit, hdl);
    if (rv != SHR_E_NONE) {
        sal_mutex_give(ctrl[unit]->glock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_delete() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    SHR_IF_ERR_EXIT
        (sal_mutex_give(ctrl[unit]->glock));
    /*
     * If all the lists on this unit have been destroyed, then free the
     * list control resources allocated for this unit.
     */
    if (!ctrl[unit]->lcount) {
        rv = shr_itbm_ctrl_cleanup(unit);
        SHR_IF_ERR_EXIT(rv);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: [LID=%-4u comp_id=%-2u aid=%-4u].\n"),
                 list_id,
                 comp_id,
                 aid));
exit:
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_block_alloc(shr_itbm_list_hdl_t hdl,
                          uint32_t count,
                          uint32_t *bucket,
                          uint32_t *first)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    shr_itbm_list_op_t op; /* List operation structure. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    int rv = SHR_E_NONE; /* Return value. */
    shr_itbm_lattr_t *lattr = NULL; /* Pointer to list attribute. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(bucket, SHR_E_PARAM);
    SHR_NULL_CHECK(first, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number and list info in local variables. */
    unit = hdl->params.unit;
    linfo = hdl->linfo;

    /* Verify the list is valid before trying to allocate from the list. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid: (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /*
     * For using block allocation function, block_stat attribute must be
     * enabled for the list in list params, while creating the list.
      */
    if (hdl->params.block_stat == FALSE) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List block_stat(%u) is disabled - "
                                        "rv=%s.\n"),
                             hdl->params.block_stat,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /*
     * Get the list attribute details, for further input parameter
     * validations.
     */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_attr_get(unit,
                                hdl->linfo,
                                hdl->aid,
                                &lattr));
    SHR_NULL_CHECK(lattr, SHR_E_INTERNAL);

    /*
     * Verify the requested entries count is not greater than the total
     * elements supported by the list.
     */
    if (count == 0 || count > (lattr->tot_elems)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "\"count=%u\" is invalid - rv=%s.\n"),
                             count,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /*
     * If there are no free entries in the list, then return table full
     * error.
     */
    if (lattr->stat.free_count == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_FULL,
                            (BSL_META_U(unit,
                                        "(Requested count=%u, "
                                        "free_count=%u) - rv=%s.\n"),
                             count,
                             lattr->stat.free_count,
                             shr_errmsg(SHR_E_FULL)));
    }

    /* Check if requested number of elements are available in the free pool. */
    if (count > lattr->stat.free_count) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                            (BSL_META_U(unit,
                                        "Requested (count=%u > "
                                        "free_count=%u) - rv=%s.\n"),
                             count,
                             lattr->stat.free_count,
                             shr_errmsg(SHR_E_RESOURCE)));
    }

    /* If list defragmentation started and in-progress, then return busy. */
    if (linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }

    /* Setup list operation input params. */
    shr_itbm_list_op_t_init(&op);
    op.linfo = hdl->linfo;
    op.lattr = lattr;
    SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_ALLOC);
    op.params = hdl->params;
    op.num_elem = count;
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        op.offset = hdl->params.min;
    }

    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));
    if (hdl->params.buckets) {
        rv = shr_itbm_bkt_list_elems_alloc(unit, &op);
    } else {
        rv = shr_itbm_list_elems_alloc(unit, &op);
    }
    if (rv != SHR_E_NONE) {
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_elems_alloc() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    if (hdl->params.buckets) {
        /* Update indexed buckets list statistics. */
        rv = shr_itbm_bkt_list_stat_update(unit, hdl, &op);
    } else {
        /* Update list statistics. */
        rv = shr_itbm_list_stat_update(unit, hdl, &op);
    }
    if (rv != SHR_E_NONE) {
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_stat_update() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    /*
     * Update total blocks allocated from this list count for block_stat
     * enabled lists.
     */
    if (hdl->params.block_stat) {
        linfo->tot_blks_alloced++;
    }

    if (hdl->params.buckets) {
        /*
         * Return allocated base bucket number and the first elements offset
         * index in the base bucket.
         */
        *bucket = op.base_bkt;
        *first = op.base_idx;
    } else {
        /* Allocated element number or first element number if block alloc. */
        *first = op.base_idx;
    }
    SHR_IF_ERR_EXIT
        (sal_mutex_give(linfo->llock));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: [LID=%-4u comp_id=%-2u aid=%-4u count=%u "
                            "bucket=%d first=%d].\n"),
                 linfo->list_id, hdl->params.comp_id, hdl->aid, count,
                 (int)*bucket, (int)*first));
exit:
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_block_alloc_id(shr_itbm_list_hdl_t hdl,
                             uint32_t count,
                             uint32_t bucket,
                             uint32_t first)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    shr_itbm_list_op_t op; /* List operation structure. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    int rv = SHR_E_NONE; /* Return value. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number and list info in local variables. */
    unit = hdl->params.unit;
    linfo = hdl->linfo;

    /* Verify the list is valid before trying to allocate from the list. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid : (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /* Validate input bucket, first and count parameters. */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_blk_params_validate(unit,
                                           hdl,
                                           bucket,
                                           first,
                                           count));

    /* If list defragmentation started and in-progress, then return busy. */
    if (linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }

    /* Setup list operation input params. */
    shr_itbm_list_op_t_init(&op);
    op.linfo = hdl->linfo;
    SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_ALLOC_ID);
    op.params = hdl->params;
    op.base_bkt = bucket;
    op.base_idx = first;
    op.num_elem = count;

    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        op.offset = hdl->params.min;
    }

    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));

    if (hdl->params.buckets) {
        rv = shr_itbm_bkt_list_elems_alloc_id(unit, &op);
    } else {
        rv = shr_itbm_list_elems_alloc_id(unit, &op);
    }
    if (rv != SHR_E_NONE) {
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_elems_alloc_id() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    if (hdl->params.buckets) {
        /* Update indexed buckets list statistics. */
        rv = shr_itbm_bkt_list_stat_update(unit, hdl, &op);
    } else {
        /* Update list statistics. */
        rv = shr_itbm_list_stat_update(unit, hdl, &op);
    }
    if (rv != SHR_E_NONE) {
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_stat_update() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    /*
     * Update total blocks allocated from this list count for block_stat
     * enabled lists.
     */
    if (hdl->params.block_stat) {
        linfo->tot_blks_alloced++;
    }
    SHR_IF_ERR_EXIT
        (sal_mutex_give(linfo->llock));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: [LID=%-4u comp_id=%-2u aid=%-4u count=%u "
                            "bucket=%d first=%d].\n"),
                 linfo->list_id, hdl->params.comp_id, hdl->aid, count,
                 (int)bucket, (int)first));
exit:
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_block_free(shr_itbm_list_hdl_t hdl,
                         uint32_t count,
                         uint32_t bucket,
                         uint32_t first)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    shr_itbm_list_op_t op; /* List operation structure. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_list_edata_t *edata = NULL; /* Ptr to list element/bucket array. */
    int rv = SHR_E_NONE; /* Return value. */
    uint32_t zoff = 0; /* Zero based index offset for edata array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number and list info in local variables. */
    unit = hdl->params.unit;
    linfo = hdl->linfo;
    edata = hdl->linfo->edata;

    /* Calculate the offset to be adjusted for zero-based indexing. */
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        zoff = hdl->params.min;
    }

    /* Verify the list is valid before trying to allocate from the list. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid: (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Validate input bucket, first and count parameters. */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_blk_params_validate(unit,
                                           hdl,
                                           bucket,
                                           first,
                                           count));

    /* If list defragmentation started and in-progress, then return busy. */
    if (linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }
    /*
     * Verify the input block is currently in-use and the input block
     * parameters match with the existing stored values of the block.
     */
    if (hdl->params.block_stat == TRUE) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_in_use_blk_match_check(unit,
                                                  hdl->params,
                                                  edata,
                                                  bucket,
                                                  first,
                                                  count,
                                                  zoff));
    }

    /* Setup list operation input params. */
    shr_itbm_list_op_t_init(&op);
    op.linfo = hdl->linfo;
    SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_FREE);
    op.params = hdl->params;
    op.base_bkt = bucket;
    op.base_idx = first;
    op.num_elem = count;
    op.offset = zoff;

    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));
    if (hdl->params.buckets) {
        rv = shr_itbm_bkt_list_elems_free(unit, &op);
    } else {
        rv = shr_itbm_list_elems_free(unit, &op);
    }
    if (rv != SHR_E_NONE) {
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_elems_free() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    if (hdl->params.buckets) {
        /* Update indexed buckets list statistics. */
        rv = shr_itbm_bkt_list_stat_update(unit, hdl, &op);
    } else {
        /* Update list statistics. */
        rv = shr_itbm_list_stat_update(unit, hdl, &op);
    }
    if (rv != SHR_E_NONE) {
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_stat_update() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    /*
     * Update total blocks allocated from this list count for block_stat
     * enabled lists.
     */
    if (hdl->params.block_stat) {
        linfo->tot_blks_alloced--;
    }
    SHR_IF_ERR_EXIT
        (sal_mutex_give(linfo->llock));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done: [LID=%-4u comp_id=%-2u aid=%-4u count=%u "
                            "bucket=%d first=%d].\n"),
                 linfo->list_id, hdl->params.comp_id, hdl->aid, count,
                 (int)bucket, (int)first));
exit:
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_block_resize(shr_itbm_list_hdl_t hdl,
                           uint32_t count,
                           uint32_t bucket,
                           uint32_t first,
                           uint32_t new_count)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    shr_itbm_list_op_t op; /* List operation structure. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    int rv = SHR_E_NONE; /* Return value. */
    uint32_t zoff = 0; /* Zero based index offset for edata array. */
    uint32_t nbkts = 0; /* Number of buckets required for resized block. */
    shr_itbm_lattr_t *lattr = NULL; /* Pointer to list attribute. */
    uint32_t end_bkt = SHR_ITBM_INVALID;
    uint8_t end_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number and list info in local variables. */
    unit = hdl->params.unit;
    linfo = hdl->linfo;

    /* Calculate the offset to be adjusted for zero-based indexing. */
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        zoff = hdl->params.min;
    }

    /* Verify the list is valid before trying to allocate from the list. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid: (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Get the list attribute details. */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_attr_get(unit,
                                hdl->linfo,
                                hdl->aid,
                                &lattr));
    SHR_NULL_CHECK(lattr, SHR_E_INTERNAL);

    /*
     * Verify the requested entries count is not greater than the total
     * elements in this list count.
     */
    if (new_count > lattr->tot_elems) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "\"new_count=%u > "
                                        "tot_ecount=%u\" - rv=%s.\n"),
                             new_count,
                             lattr->tot_elems,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /*
     * If there are no free elements in the list, then return table full
     * error.
     */
    if ((new_count > count) && lattr->stat.free_count == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_FULL,
                            (BSL_META_U(unit,
                                        "\"(new_count=%u > count=%u) && "
                                        "list free_ecount=%u\" - rv=%s.\n"),
                             new_count,
                             count,
                             lattr->stat.free_count,
                             shr_errmsg(SHR_E_FULL)));
    }

    /* Validate input bucket, first and count parameters. */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_blk_params_validate(unit,
                                           hdl,
                                           bucket,
                                           first,
                                           count));

    /* Verify block is still valid with the new_count block size. */
    if (hdl->params.buckets == TRUE) {
        /*
         * Determine total number of buckets required to complete the block
         * resize request, if the required buckets count goes beyond the valid
         * range, return resource error.
         */
        nbkts = shr_itbm_list_blk_num_bkts_get(new_count,
                                               first,
                                               hdl->params.entries_per_bucket,
                                               FALSE);
        if (((bucket + nbkts) - 1) > hdl->params.last) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                                (BSL_META_U(unit,
                                            "nbkts=0x%x req for new_count=%u "
                                            "blk-elems@(bucket=0x%x, "
                                            "first=0x%x) exceeds the valid "
                                            "buckets \"0x%x-0x%x\" "
                                            "range - rv=%s.\n"),
                                 nbkts,
                                 new_count,
                                 bucket,
                                 first,
                                 hdl->params.first,
                                 hdl->params.last,
                                 shr_errmsg(SHR_E_RESOURCE)));
        }
    } else {
        /*
         * Verify requested first + new_count number of elements is within the
         * valid range (last) of the list.
         */
        if ((first + new_count) > (hdl->params.last + 1)) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                                (BSL_META_U(unit,
                                            "Resize blk size(=%u) exceeds "
                                            "valid range (last=%u) - rv=%s."
                                            "\n"),
                                 (first + new_count),
                                 hdl->params.last,
                                 shr_errmsg(SHR_E_RESOURCE)));
        }
    }

    /* If list defragmentation started and in-progress, then return busy. */
    if (linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }

    /*
     * Verify the input block is currently in-use and the input block
     * parameters match with the existing stored values of the block.
     */
    if (hdl->params.block_stat == TRUE) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_in_use_blk_match_check(unit,
                                                  hdl->params,
                                                  hdl->linfo->edata,
                                                  bucket,
                                                  first,
                                                  count,
                                                  zoff));
    }

    /*
     * If the free elements count in the list is lesser than the requested
     * elements count, then return resource error.
     */
    if ((new_count > count) && (lattr->stat.free_count < (new_count - count))) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                            (BSL_META_U(unit,
                                        "\"(new_count=%u > count=%u) && "
                                        "list (free_ecount=%u)\" - rv=%s.\n"),
                             new_count,
                             count,
                             lattr->stat.free_count,
                             shr_errmsg(SHR_E_RESOURCE)));
    }

    /*
     * If new_count is same as current count, then nothing to do, exit the
     * function.
     */
    if (count == new_count) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "\"new_count(=%-5u) == count(=%-5u)\" - "
                                "rv=%s.\n"),
                     new_count,
                     count,
                     shr_errmsg(SHR_E_NONE)));
        SHR_EXIT();
    }

    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));
    /* Initialize list operation structure members. */
    shr_itbm_list_op_t_init(&op);
    if (new_count > count) {
        /* Setup list operation input params. */
        op.linfo = hdl->linfo;
        SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_RESIZE);
        SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_ALLOC_ID);
        op.params = hdl->params;
        op.num_elem = (new_count - count);
        op.offset = zoff;
        if (hdl->params.buckets) {
            /*
             * Calculate the current block's end bucket number and the block's
             * end element offset index in this bucket, after which the
             * additional elements allocation must start due to the block resize
             * operation.
             */
            SHR_IF_ERR_EXIT
                (shr_itbm_bkt_list_blk_end_get(unit,
                                               bucket,
                                               first,
                                               count,
                                               op.params.entries_per_bucket,
                                               &end_bkt,
                                               &end_idx));
            if (end_idx == (op.params.entries_per_bucket - 1)) {
                op.base_bkt = end_bkt + 1;
                op.base_idx = SHR_ITBM_LIST_DEFAULT_BKT_BASE_IDX;
            } else {
                op.base_bkt = end_bkt;
                op.base_idx = end_idx + 1;
            }
            rv = shr_itbm_bkt_list_elems_alloc_id(unit, &op);
        } else {
            /*
             * Starting from this element, num_elem must be allocated from the
             * list.
             */
            op.base_idx = (first + count);
            rv = shr_itbm_list_elems_alloc_id(unit, &op);
        }
        if (rv != SHR_E_NONE) {
            sal_mutex_give(linfo->llock);
            if (rv == SHR_E_EXISTS) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                                    (BSL_META_U(unit,
                                                "resize: elems_alloc_id() "
                                                "already exists - rv=%s.\n"),
                                     shr_errmsg(SHR_E_RESOURCE)));
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        /*
         * Update additional elements that have been allocated from the
         * list.
         */
        if (hdl->params.buckets) {
            /* Update indexed buckets list statistics. */
            rv = shr_itbm_bkt_list_stat_update(unit, hdl, &op);
        } else {
            rv = shr_itbm_list_stat_update(unit, hdl, &op);
        }
        if (rv != SHR_E_NONE) {
            sal_mutex_give(linfo->llock);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "list_stat_update - rv=%s.\n"),
                                 shr_errmsg(rv)));
        }

    } else {
        /* Setup list operation input params. */
        op.linfo = hdl->linfo;
        SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_RESIZE);
        SHR_BITSET(op.loper, SHR_ITBM_LIST_OPER_FREE);
        op.params = hdl->params;
        op.num_elem = (count - new_count);
        op.offset = zoff;
        if (hdl->params.buckets) {
            SHR_IF_ERR_EXIT
                (shr_itbm_bkt_list_blk_end_get(unit,
                                               bucket,
                                               first,
                                               new_count,
                                               op.params.entries_per_bucket,
                                               &end_bkt,
                                               &end_idx));
            if (end_idx == (op.params.entries_per_bucket - 1)) {
                op.base_bkt = end_bkt + 1;
                op.base_idx = 0;
            } else {
                op.base_bkt = end_bkt;
                op.base_idx = end_idx + 1;
            }
            rv = shr_itbm_bkt_list_elems_free(unit, &op);
        } else {
            /*
             * Starting from this element, num_elem must be freed back to
             * list.
             */
            op.base_idx = (first + new_count);
            rv = shr_itbm_list_elems_free(unit, &op);
        }
        if (rv != SHR_E_NONE) {
            sal_mutex_give(linfo->llock);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "list_elems_free - rv=%s.\n"),
                                 shr_errmsg(rv)));
        }
        /*
         * Update additional elements that have been freed back to the
         * list.
         */
        if (hdl->params.buckets) {
            /* Update indexed buckets list statistics. */
            rv = shr_itbm_bkt_list_stat_update(unit, hdl, &op);
        } else {
            rv = shr_itbm_list_stat_update(unit, hdl, &op);
        }
        if (rv != SHR_E_NONE) {
            sal_mutex_give(linfo->llock);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "stat_update - rv=%s.\n"),
                                 shr_errmsg(rv)));
        }
    }
    /*
     * If the block is being resized to zero elements i.e. new_count = 0,
     * then this resize operation is equivalent to block free. Decrement
     * the total blocks allocated from this list count.
     */
    if (hdl->params.block_stat && new_count == 0) {
        linfo->tot_blks_alloced--;
    }
    /*
     * Also update the new blk_sz in the first element of the block in this
     * function, as the first element of the block is not passed to
     * shr_itbm_list_elems_alloc_id() or shr_itbm_list_elems_free() functions
     * for the block resize case.
     */
    if (hdl->params.buckets) {
        if (SHR_BITGET(linfo->edata[bucket - zoff].eprop, first)) {
            linfo->edata[bucket - zoff].blk_sz[first] = new_count;
        }
    } else {
        if (SHR_BITGET(linfo->edata[first - zoff].eprop,
                       SHR_ITBM_LIST_BLK_FIRST_ELEM)) {
            linfo->edata[first - zoff].blk_sz[SHR_ITBM_LIST_BKT_NONE] =
                                                                    new_count;
        }
    }
    SHR_IF_ERR_EXIT
        (sal_mutex_give(linfo->llock));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Done: [LID=%-4u rev=%s bkts=%s epbkt=%u zoff=%-5u "
                          "b=%-5d f=%-5d cnt=%-5u ncnt=%-5u/"
                          "bsz=%-5u].\n"),
               linfo->list_id,
               hdl->params.reverse ? "T" : "F",
               hdl->params.buckets ? "T" : "F",
               hdl->params.entries_per_bucket,
               zoff,
               (int)bucket,
               (int)first,
               count,
               new_count,
               hdl->params.buckets ?
               linfo->edata[(bucket - zoff)].blk_sz[first] :
               linfo->edata[(first - zoff)].blk_sz[SHR_ITBM_LIST_BKT_NONE]));
exit:
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_elem_state_get(shr_itbm_list_hdl_t hdl,
                             uint32_t bucket,
                             uint32_t element,
                             bool *in_use,
                             uint32_t *comp_id)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    uint32_t zoff = 0;    /* Zero based index offset for edata array. */
    uint32_t b = SHR_ITBM_INVALID; /* Bucket number. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(in_use, SHR_E_PARAM);
    SHR_NULL_CHECK(comp_id, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number and list info in local variables. */
    unit = hdl->params.unit;
    linfo = hdl->linfo;

    /* Check if list min index has a non-zero offset index value. */
    if (SHR_BITGET(linfo->lprop, SHR_ITBM_LIST_MIN_NONZERO)) {
        zoff = hdl->params.min;
    }

    *in_use = 0;
    *comp_id = SHR_ITBM_INVALID;

    /* Verify the list is valid before trying to allocate from the list. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid - (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Validate input bucket, first and count parameters. */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_blk_params_validate(unit,
                                           hdl,
                                           bucket,
                                           element,
                                           1));

    /* If list defragmentation started and in-progress, then return busy. */
    if (linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }

    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));

    if (hdl->params.buckets) {
        b = bucket - zoff;
        *in_use = SHR_BITGET(linfo->edata[b].ebmp, element);
        if (*in_use) {
            *comp_id = linfo->edata[b].comp_id[element];
        }
    } else {
        /* Get the element status. */
        *in_use = shr_itbm_list_elem_ref_cnt_get(linfo->edata,
                                                 (element - zoff));
        if (*in_use) {
            /* If it is in-use, update the component ID out parameter. */
            *comp_id =
                linfo->edata[element - zoff].comp_id[SHR_ITBM_LIST_BKT_NONE];
        }
    }
    SHR_IF_ERR_EXIT
        (sal_mutex_give(linfo->llock));
exit:
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_stat_get(shr_itbm_list_hdl_t hdl,
                       shr_itbm_list_stat_t *stat)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    int rv = SHR_E_INTERNAL; /* Return value. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(stat, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number and list info in local variables. */
    unit = hdl->params.unit;
    linfo = hdl->linfo;

    /* Verify the list is valid before trying to allocate from the list. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid - (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* If list defragmentation started and in-progress, then return busy. */
    if (linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }

    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));

    rv = shr_itbm_list_attr_stat_get(unit, hdl, stat);
    if (rv != SHR_E_NONE) {
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "attr_stat_get() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    SHR_IF_ERR_EXIT
        (sal_mutex_give(linfo->llock));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Done: [LID=%-4u comp_id=%-2u aid=%-2u rev=%s "
                          "bkts=%s epbkt=%-1u in_use_count=%-5u "
                          "free_count=%-5u].\n"),
               linfo->list_id,
               hdl->params.comp_id,
               hdl->aid,
               hdl->params.reverse ? "T" : "F",
               hdl->params.buckets ? "T" : "F",
               hdl->params.entries_per_bucket,
               stat->in_use_count,
               stat->free_count));
exit:
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_block_stat_get(shr_itbm_list_hdl_t hdl,
                             uint32_t *alloc_count,
                             shr_itbm_blk_t **alloc_blk,
                             uint32_t *free_count,
                             shr_itbm_blk_t **free_blk)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    int rv = SHR_E_INTERNAL; /* Return value. */
    uint32_t idx = 0; /* Index iterator. */
    uint32_t ablks_cnt = 0; /* Allocated in-use blocks count. */
    uint32_t comp_blks_cnt = 0; /* Total blocks allocated by this component. */
    uint32_t fblks_cnt = 0; /* Total free blocks count. */
    uint32_t comp_max_ecount = 0; /* Max block size in-use by the component. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_blk_t *ablk_arr = NULL; /* Inuse blocks array. */
    shr_itbm_blk_t *fblk_arr = NULL; /* Free blocks array. */
    shr_itbm_blk_t largest_fblk; /* Biggest free block available. */
    shr_pb_t *pb = NULL; /* Debug print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(alloc_count, SHR_E_PARAM);
    SHR_NULL_CHECK(alloc_blk, SHR_E_PARAM);
    SHR_NULL_CHECK(free_count, SHR_E_PARAM);
    SHR_NULL_CHECK(free_blk, SHR_E_PARAM);

    /* Initialize output parameters. */
    *alloc_count = 0;
    *alloc_blk = NULL;
    *free_count = 0;
    *free_blk = NULL;

    /* Validate unit number. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number and list info in local variables. */
    unit = hdl->params.unit;
    linfo = hdl->linfo;

    /* Verify the list is valid before trying to allocate from the list. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid - (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* This function is supported only for block_stat enabled lists. */
    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List[LID=%u] \"block_stat\" is "
                                        "disabled - rv=%s.\n"),
                             hdl->linfo->list_id,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* If list defragmentation started and in-progress, then return busy. */
    if (linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }

    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));

    if (hdl->linfo->tot_blks_alloced) {
        SHR_ALLOC(ablk_arr,
                  sizeof(*ablk_arr) * hdl->linfo->tot_blks_alloced,
                  "shritbmInuseBlkArr");
        SHR_NULL_CHECK(ablk_arr, SHR_E_MEMORY);
        sal_memset(ablk_arr, 0,
                   sizeof(*ablk_arr) * hdl->linfo->tot_blks_alloced);
        if (hdl->params.buckets == TRUE) {
            /* Get the total in-use blocks count and the block details. */
            rv = shr_itbm_bkt_list_inuse_blks_get(unit,
                                                  hdl,
                                                  hdl->linfo->tot_blks_alloced,
                                                  ablk_arr,
                                                  &ablks_cnt,
                                                  &comp_blks_cnt,
                                                  &comp_max_ecount);
        } else {
            /* Get the total in-use blocks count and the block details. */
            rv = shr_itbm_list_inuse_blks_get(unit,
                                              hdl,
                                              hdl->linfo->tot_blks_alloced,
                                              ablk_arr,
                                              &ablks_cnt,
                                              &comp_blks_cnt,
                                              &comp_max_ecount);
        }
        if (SHR_FAILURE(rv)) {
            SHR_FREE(ablk_arr);
            sal_mutex_give(linfo->llock);
            SHR_VERBOSE_EXIT(rv);
        }

        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            pb = shr_pb_create();
            shr_pb_printf(pb,
                          "AllocBlksList(LID=%-2u): [Alloced=%-5u "
                          "Act_InUse=%-5u CompIDMatched=%-5u].\n",
                          hdl->linfo->list_id, hdl->linfo->tot_blks_alloced,
                          ablks_cnt, comp_blks_cnt);
            shr_pb_printf(pb,
                          "==============================================="
                          "===\n");
            for (idx = 0; idx < ablks_cnt; idx++) {
                shr_pb_printf(pb,
                              "%5u.[epbkt=%-2u comp_id=%-2d b=%-5d f=%-5u "
                              "ecnt=%-5u].\n",
                              idx,
                              ablk_arr[idx].entries_per_bucket,
                              (int)ablk_arr[idx].comp_id,
                              (int)ablk_arr[idx].bucket,
                              ablk_arr[idx].first_elem,
                              ablk_arr[idx].ecount);
            }
            shr_pb_printf(pb, "\n");
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
            pb = NULL;
        }
        *alloc_count = ablks_cnt;
        *alloc_blk = ablk_arr;
    }

    /* Get the total free blocks count and the block details. */
    shr_itbm_blk_t_init(&largest_fblk);
    if (hdl->params.buckets == TRUE) {
        rv = shr_itbm_bkt_list_free_blks_get(unit,
                                             hdl,
                                             ablks_cnt,
                                             ablk_arr,
                                             &fblks_cnt,
                                             &fblk_arr,
                                             &largest_fblk);

    } else {
        rv = shr_itbm_list_free_blks_get(unit,
                                         hdl,
                                         ablks_cnt,
                                         ablk_arr,
                                         &fblks_cnt,
                                         &fblk_arr,
                                         &largest_fblk);
    }
    if (rv != SHR_E_NONE) {
        SHR_FREE(ablk_arr);
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "free_blks_get() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    SHR_IF_ERR_EXIT
        (sal_mutex_give(linfo->llock));
    if (fblks_cnt && fblk_arr) {
        *free_count = fblks_cnt;
        *free_blk = fblk_arr;

        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            pb = shr_pb_create();
            shr_pb_printf(pb,
                          "FreeBlksList(LID=%-2u): [fblks_cnt=%-5u].\n",
                          hdl->linfo->list_id, fblks_cnt);
            shr_pb_printf(pb,
                          "==========================================\n");
            for (idx = 0; idx < fblks_cnt; idx++) {
                shr_pb_printf(pb,
                              "%5u.[epbkt=%-2u comp_id=%-2d b=%-5d f=%-5u "
                              "ecnt=%-5u].\n",
                              idx,
                              fblk_arr[idx].entries_per_bucket,
                              (int)fblk_arr[idx].comp_id,
                              (int)fblk_arr[idx].bucket,
                              fblk_arr[idx].first_elem,
                              fblk_arr[idx].ecount);
            }
            shr_pb_printf(pb, "\n");
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
            pb = NULL;
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (ablk_arr) {
            SHR_FREE(ablk_arr);
        }
        *alloc_count = 0;
        if (fblk_arr) {
            SHR_FREE(fblk_arr);
        }
        *free_count = 0;
    }
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_defrag_start(shr_itbm_list_hdl_t hdl,
                           uint32_t *count,
                           shr_itbm_blk_t **blk_cur_loc,
                           shr_itbm_defrag_blk_mseq_t **blk_mov_seq,
                           uint32_t *free_count,
                           shr_itbm_blk_t **free_blk,
                           shr_itbm_blk_t *largest_free_blk)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    int rv = SHR_E_INTERNAL; /* Return value. */
    uint32_t idx = 0, m = 0; /* Index iterator. */
    uint32_t ablks_cnt = 0; /* Total blocks in-use count. */
    uint32_t comp_blks_cnt = 0; /* Total blocks allocated by this component. */
    uint32_t fblks_cnt = 0; /* Total free blocks count. */
    uint32_t comp_max_ecount = 0; /* Max block size in-use by the component. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_blk_t *ablk_arr = NULL; /* Allocated in-use blocks array. */
    shr_itbm_blk_t *fblk_arr = NULL; /* Free blocks array. */
    shr_itbm_blk_t largest_fblk; /* Biggest free block available. */
    shr_pb_t *pb = NULL; /* Debug print buffer. */
    shr_itbm_blk_t *dfgl_ablk_arr = NULL; /* Allocated in-use blocks array. */
    shr_itbm_blk_t *dfgl_fblk_arr = NULL; /* Free blocks array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);
    SHR_NULL_CHECK(blk_cur_loc, SHR_E_PARAM);
    SHR_NULL_CHECK(blk_mov_seq, SHR_E_PARAM);
    if (free_count != NULL) {
        SHR_NULL_CHECK(free_blk, SHR_E_PARAM);
        SHR_NULL_CHECK(largest_free_blk, SHR_E_PARAM);
    }
    if (free_blk != NULL) {
        SHR_NULL_CHECK(free_count, SHR_E_PARAM);
        SHR_NULL_CHECK(largest_free_blk, SHR_E_PARAM);
    }
    if (largest_free_blk != NULL) {
        SHR_NULL_CHECK(free_count, SHR_E_PARAM);
        SHR_NULL_CHECK(free_blk, SHR_E_PARAM);
    }

    SHR_NULL_CHECK(blk_cur_loc, SHR_E_PARAM);
    SHR_NULL_CHECK(blk_mov_seq, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number and list info in local variables. */
    unit = hdl->params.unit;
    linfo = hdl->linfo;
    /* Initialize output parameter values. */
    *count = 0;
    *blk_cur_loc = NULL;
    *blk_mov_seq = NULL;

    /* Verify the list is valid before trying to allocate from the list. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid - (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* This function is supported only for block_stat enabled lists. */
    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List[LID=%u] \"block_stat\" is "
                                        "disabled - rv=%s.\n"),
                             hdl->linfo->list_id,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* If list defragmentation started and in-progress, then return busy. */
    if (linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }

    /*
     * If no blocks have been allocated from the list and user component
     * attempts to defragment the list, then return invalid parameter error
     * as defragmentation is not required for this list.
     */
    if (hdl->linfo->tot_blks_alloced == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Defrag not required, as all list "
                                        "elements are free and available - "
                                        "rv=%s.\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "shr_itbm_list_defrag_start(LID=%u) - begin\n"),
                     hdl->linfo->list_id));
    }
    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));

    /* Create a back-up copy of the current list elements state. */
    rv = shr_itbm_list_edata_backup_create(unit, hdl);
    if (rv != SHR_E_NONE) {
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "edata_backup_create() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    /* Set the defragementation enabled status for this list. */
    linfo->defrag_inprog = TRUE;
    /* Initialize the defragmentation list handle pointer value to null. */
    linfo->defrag_hdl = NULL;

    /* Get the allocated and in-use block parameters details. */
    SHR_ALLOC(ablk_arr,
              sizeof(*ablk_arr) * hdl->linfo->tot_blks_alloced,
              "shritbmInuseBlkArr");
    SHR_NULL_CHECK(ablk_arr, SHR_E_MEMORY);
    sal_memset(ablk_arr, 0,
               sizeof(*ablk_arr) * hdl->linfo->tot_blks_alloced);
    if (hdl->params.buckets == TRUE) {
        /* Get the total in-use blocks count and the block details. */
        rv = shr_itbm_bkt_list_inuse_blks_get(unit,
                                              hdl,
                                              hdl->linfo->tot_blks_alloced,
                                              ablk_arr,
                                              &ablks_cnt,
                                              &comp_blks_cnt,
                                              &comp_max_ecount);
    } else {
        rv = shr_itbm_list_inuse_blks_get(unit,
                                          hdl,
                                          hdl->linfo->tot_blks_alloced,
                                          ablk_arr,
                                          &ablks_cnt,
                                          &comp_blks_cnt,
                                          &comp_max_ecount);
    }
    if (rv != SHR_E_NONE) {
        shr_itbm_list_edata_backup_destroy(unit, hdl);
        linfo->defrag_inprog = FALSE;
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "inuse_blks_get() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && ablks_cnt > 0) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "InUseBlks(l_epbkt=%-1u): [Alloced=%-5u Act_InUse=%-5u "
                      "CompIDMatched=%-5u].\n",
                      hdl->params.entries_per_bucket,
                      hdl->linfo->tot_blks_alloced,
                      ablks_cnt,
                      comp_blks_cnt);
        shr_pb_printf(pb,
                      "==============================================="
                      "===\n");
        for (idx = 0; idx < ablks_cnt; idx++) {
            shr_pb_printf(pb,
                          "%5u.[epbkt=%-1u b=%-5d f=%-5u c=%-2u sz=%-5u].\n",
                          idx,
                          ablk_arr[idx].entries_per_bucket,
                          (int)ablk_arr[idx].bucket,
                          ablk_arr[idx].first_elem,
                          ablk_arr[idx].comp_id,
                          ablk_arr[idx].ecount);
        }
        shr_pb_printf(pb, "\n");
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }

    /* Get the total free blocks count and the block details. */
    shr_itbm_blk_t_init(&largest_fblk);
    if (hdl->params.buckets == TRUE) {
        rv = shr_itbm_bkt_list_free_blks_get(unit,
                                             hdl,
                                             ablks_cnt,
                                             ablk_arr,
                                             &fblks_cnt,
                                             &fblk_arr,
                                             &largest_fblk);

    } else {
        rv = shr_itbm_list_free_blks_get(unit,
                                         hdl,
                                         ablks_cnt,
                                         ablk_arr,
                                         &fblks_cnt,
                                         &fblk_arr,
                                         &largest_fblk);
    }
    if (rv != SHR_E_NONE) {
        shr_itbm_list_edata_backup_destroy(unit, hdl);
        linfo->defrag_inprog = FALSE;
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "free_blks_get() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    if (fblks_cnt && fblk_arr) {
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            pb = shr_pb_create();
            shr_pb_printf(pb,
                          "Free_Blks: [fblks_cnt=%-5u].\n",
                          fblks_cnt);
            shr_pb_printf(pb,
                          "==========================================\n");
            for (idx = 0; idx < fblks_cnt; idx++) {
                shr_pb_printf(pb,
                              "%5u.[epbkt=%-1u b=%-5d f=%-5u c=%-2d "
                              "ecnt=%-5u].\n",
                              idx,
                              fblk_arr[idx].entries_per_bucket,
                              (int)fblk_arr[idx].bucket,
                              fblk_arr[idx].first_elem,
                              (int)fblk_arr[idx].comp_id,
                              fblk_arr[idx].ecount);
            }
            shr_pb_printf(pb, "\n");
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
            pb = NULL;
        }
    }

    if (hdl->params.buckets == TRUE) {
        if (hdl->params.reverse == TRUE) {
            rv = shr_itbm_bkt_list_defragment_rev(unit,
                                                  hdl,
                                                  comp_blks_cnt,
                                                  comp_max_ecount,
                                                  ablks_cnt,
                                                  ablk_arr,
                                                  fblks_cnt,
                                                  fblk_arr,
                                                  largest_fblk,
                                                  count,
                                                  blk_cur_loc,
                                                  blk_mov_seq);

        } else {
            rv = shr_itbm_bkt_list_defragment(unit,
                                              hdl,
                                              comp_blks_cnt,
                                              comp_max_ecount,
                                              ablks_cnt,
                                              ablk_arr,
                                              fblks_cnt,
                                              fblk_arr,
                                              largest_fblk,
                                              count,
                                              blk_cur_loc,
                                              blk_mov_seq);
        }
    } else {
        rv = shr_itbm_list_defragment(unit,
                                      hdl,
                                      comp_blks_cnt,
                                      comp_max_ecount,
                                      ablks_cnt,
                                      ablk_arr,
                                      fblks_cnt,
                                      fblk_arr,
                                      largest_fblk,
                                      count,
                                      blk_cur_loc,
                                      blk_mov_seq);
    }
    if (rv != SHR_E_NONE) {
        shr_itbm_list_edata_backup_destroy(unit, hdl);
        linfo->defrag_inprog = FALSE;
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "list_defragment() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    if (free_count) {
        /*
         * From the defragmented list, get the allocated and in-use block
         * parameters details.
         */
        SHR_ALLOC(dfgl_ablk_arr,
                  sizeof(*dfgl_ablk_arr) * hdl->linfo->tot_blks_alloced,
                  "shritbmDfglInuseBlkArr");
        SHR_NULL_CHECK(dfgl_ablk_arr, SHR_E_MEMORY);
        sal_memset(dfgl_ablk_arr, 0,
                   sizeof(*dfgl_ablk_arr) * hdl->linfo->tot_blks_alloced);
        if (hdl->params.buckets == TRUE) {
            /* Get the total in-use blocks count and the block details. */
            rv = shr_itbm_bkt_list_inuse_blks_get(unit, hdl,
                                                  hdl->linfo->tot_blks_alloced,
                                                  dfgl_ablk_arr, &ablks_cnt,
                                                  &comp_blks_cnt,
                                                  &comp_max_ecount);
        } else {
            rv = shr_itbm_list_inuse_blks_get(unit, hdl,
                                              hdl->linfo->tot_blks_alloced,
                                              dfgl_ablk_arr, &ablks_cnt,
                                              &comp_blks_cnt, &comp_max_ecount);
        }
        if (rv != SHR_E_NONE) {
            shr_itbm_list_edata_backup_destroy(unit, hdl);
            linfo->defrag_inprog = FALSE;
            sal_mutex_give(linfo->llock);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "dfgl:inuse_blks_get() - rv=%s.\n"),
                                 shr_errmsg(rv)));
        }

        if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE) && ablks_cnt > 0) {
            pb = shr_pb_create();
            shr_pb_printf(pb,
                          "dfgl:InUseBlks(l_epbkt=%-1u): [Alloced=%-5u "
                          "Act_InUse=%-5u CompIDMatched=%-5u].\n",
                          hdl->params.entries_per_bucket,
                          hdl->linfo->tot_blks_alloced, ablks_cnt,
                          comp_blks_cnt);
            shr_pb_printf(pb, "==============================================="
                              "=====\n");
            for (idx = 0; idx < ablks_cnt; idx++) {
                shr_pb_printf(pb, "%5u.[epbkt=%-1u b=%-5d f=%-5u c=%-2u "
                                  "sz=%-5u].\n",
                              idx, dfgl_ablk_arr[idx].entries_per_bucket,
                              (int)dfgl_ablk_arr[idx].bucket,
                              dfgl_ablk_arr[idx].first_elem,
                              dfgl_ablk_arr[idx].comp_id,
                              dfgl_ablk_arr[idx].ecount);
            }
            shr_pb_printf(pb, "\n");
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
            pb = NULL;
        }

        /*
         * From the defragmented list, get the total free blocks count and the
         * block details.
         */
        shr_itbm_blk_t_init(&largest_fblk);
        if (hdl->params.buckets == TRUE) {
            rv = shr_itbm_bkt_list_free_blks_get(unit, hdl, ablks_cnt,
                                                 dfgl_ablk_arr, &fblks_cnt,
                                                 &dfgl_fblk_arr, &largest_fblk);

        } else {
            rv = shr_itbm_list_free_blks_get(unit, hdl, ablks_cnt,
                                             dfgl_ablk_arr, &fblks_cnt,
                                             &dfgl_fblk_arr, &largest_fblk);
        }
        if (rv != SHR_E_NONE) {
            shr_itbm_list_edata_backup_destroy(unit, hdl);
            linfo->defrag_inprog = FALSE;
            sal_mutex_give(linfo->llock);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "dfgl:free_blks_get() - rv=%s.\n"),
                                 shr_errmsg(rv)));
        }
        if (fblks_cnt && dfgl_fblk_arr) {
            if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                pb = shr_pb_create();
                shr_pb_printf(pb,
                              "dfgl_free_blks: [fblks_cnt=%-5u large_fblk: "
                              "epbkt=%u b=%d f=%u c=%d ecnt=%u].\n",
                              fblks_cnt, largest_fblk.entries_per_bucket,
                              (int)largest_fblk.bucket,
                              largest_fblk.first_elem,
                              (int)largest_fblk.comp_id,
                              largest_fblk.ecount);
                shr_pb_printf(pb, "========================================="
                                  "=====\n");
                for (idx = 0; idx < fblks_cnt; idx++) {
                    shr_pb_printf(pb,
                                  "%5u.[epbkt=%-1u b=%-5d f=%-5u c=%-2d "
                                  "ecnt=%-5u].\n",
                                  idx, dfgl_fblk_arr[idx].entries_per_bucket,
                                  (int)dfgl_fblk_arr[idx].bucket,
                                  dfgl_fblk_arr[idx].first_elem,
                                  (int)dfgl_fblk_arr[idx].comp_id,
                                  dfgl_fblk_arr[idx].ecount);
                }
                shr_pb_printf(pb, "\n");
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
                shr_pb_destroy(pb);
                pb = NULL;
            }
        }
        linfo->dfgl_fblk_arr = dfgl_fblk_arr;
        *free_count = fblks_cnt;
        *free_blk = dfgl_fblk_arr;
        sal_memcpy(largest_free_blk, &largest_fblk, sizeof(*largest_free_blk));
    }
    /*
     * Store the list handle of the component that has initiated the
     * defragmentation operation for verifying the list handle passed to the
     * defrag_end function.
     */
    linfo->defrag_hdl = hdl;
    sal_mutex_give(linfo->llock);
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Done: [Defrag started (in-prog=%-1u) for LID=%-4u "
                          "hdl=%p blks_moved_cnt=%u].\n"),
               linfo->defrag_inprog,
               linfo->list_id,
               (void *)linfo->defrag_hdl,
               *count));
   if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE) && count && *count > 0) {
        pb = shr_pb_create();
        shr_pb_printf(pb, "Blks moved=%u\n", *count);
        for (idx = 0; idx < *count; idx++) {
            shr_pb_printf(pb, "cblk_loc[%-5u]: (mcnt=%-3u mecnt=%-4u b=%-5d "
                              "f=%-5d cnt=%-5u cid=%-2d), mseq:: ",
                          idx, ((*blk_mov_seq) + idx)->mcount,
                          ((*blk_mov_seq) + idx)->ecount,
                          hdl->params.buckets ?
                            (int)(((*blk_cur_loc) + idx)->bucket) :
                            (int)SHR_ITBM_INVALID,
                          (int)((*blk_cur_loc) + idx)->first_elem,
                          (int)((*blk_cur_loc) + idx)->ecount,
                          (int)((*blk_cur_loc) + idx)->comp_id);
            for (m = 0; m < ((*blk_mov_seq) + idx)->mcount; m++) {
                shr_pb_printf(pb, "(b=%-5d f=%-5d)->",
                              hdl->params.buckets ?
                                (int)(((*blk_mov_seq) + idx)->bucket[m]) :
                                (int)SHR_ITBM_INVALID,
                              (int)((*blk_mov_seq) + idx)->first_elem[m]);
            }
            shr_pb_printf(pb, "\n");
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
   }
exit:
    SHR_FREE(ablk_arr);
    SHR_FREE(fblk_arr);
    SHR_FREE(dfgl_ablk_arr);
    if (SHR_FUNC_ERR()) {
        SHR_FREE(dfgl_fblk_arr);
    }
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_defrag_end(shr_itbm_list_hdl_t hdl,
                         bool success)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    int rv = SHR_E_INTERNAL; /* Return value. */
    uint32_t midx = 0; /* Block move index. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_defrag_blk_mseq_t *bmov_seq = NULL; /* Blocks move sequence. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->defrag_hdl, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0
        || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Update the input unit number and list info in local variables. */
    unit = hdl->params.unit;
    linfo = hdl->linfo;

    /* Verify the list is valid before trying to allocate from the list. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid - (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* This function is supported only for block_stat enabled lists. */
    if (!hdl->params.block_stat) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List[LID=%u] \"block_stat\" is "
                                        "disabled - rv=%s.\n"),
                             hdl->linfo->list_id,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Validate if defragmentation is in-progress for this list to end it. */
    if (!linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Defrag end called for \"LID=%u\" "
                                        "without a defag start call - "
                                        "rv=%s.\n"),
                             hdl->linfo->list_id,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Validate input handle supplied by the caller. */
    if (linfo->defrag_hdl != hdl) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid handle \"hdl=%p\" passed to "
                                        "end defrag, expected \"hdl=%p\" - "
                                        "rv=%s.\n"),
                             (void *)hdl,
                             (void *)hdl->linfo->defrag_hdl,
                             shr_errmsg(SHR_E_PARAM)));
    }
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "shr_itbm_list_defrag_end(LID=%u) - begin\n"),
                     hdl->linfo->list_id));
    }
    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));

    if (!success) {
        /*
         * Abort defragmentation block moves and revert list to the state before
         * defragmentation.
         */
        rv = shr_itbm_list_defrag_abort(unit, hdl);
        if (rv != SHR_E_NONE) {
            if (rv == SHR_E_MEMORY) {
                /* Reset list to the default state on memory error. */
                shr_itbm_list_defrag_abort_error_cleanup(unit, hdl);
            }
            linfo->dblks_moved = 0;
            linfo->defrag_hdl = NULL;
            linfo->defrag_inprog = FALSE;
            sal_mutex_give(linfo->llock);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "shr_itbm_list_defrag_abort() -"
                                            " rv=%s.\n"),
                                 shr_errmsg(rv)));
        }
    }

    rv = shr_itbm_list_edata_backup_destroy(unit, hdl);
    if (rv != SHR_E_NONE) {
        linfo->dblks_moved = 0;
        linfo->defrag_hdl = NULL;
        linfo->defrag_inprog = FALSE;
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "shr_itbm_list_edata_backup_destroy() -"
                                        "rv=%s.\n"),
                             shr_errmsg(rv)));
    }

    /*
     * Free the memory allocated to store and share the defragementation
     * sequence with the caller.
     */
    SHR_FREE(linfo->dblk_cur_loc);
    bmov_seq = linfo->dblk_mseq;
    if (bmov_seq) {
        for (midx = 0; midx < linfo->dblks_moved; midx++) {
            if (bmov_seq[midx].mcount) {
                SHR_FREE(bmov_seq[midx].bucket);
                SHR_FREE(bmov_seq[midx].first_elem);
            }
        }
        SHR_FREE(linfo->dblk_mseq);
    }
    /*
     * Free the defragmented list free blocks array memory, if allocated in
     * the defragmentation start API call.
     */
    SHR_FREE(linfo->dfgl_fblk_arr);

    /* Clear list defragmentation blocks moved counter. */
    linfo->dblks_moved = 0;
    /* Clear list defragmentation handle pointer value. */
    linfo->defrag_hdl = NULL;
    /* Clear list defragmentation enabled state. */
    linfo->defrag_inprog = FALSE;
    SHR_IF_ERR_EXIT
        (sal_mutex_give(linfo->llock));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Done: [Defrag for LID=%-4u is %s].\n"),
              linfo->list_id,
              linfo->defrag_inprog ? "in-prog" : "complete"));
exit:
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_defrag_free_block_get(shr_itbm_list_hdl_t hdl,
                                    shr_itbm_list_hdl_t hdl_rev,
                                    shr_itbm_blk_t *largest,
                                    shr_itbm_blk_t *largest_rev)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    int rv = SHR_E_INTERNAL; /* Return value. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl_rev, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl_rev->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl_rev->linfo->edata, SHR_E_PARAM);
    SHR_NULL_CHECK(largest, SHR_E_PARAM);
    SHR_NULL_CHECK(largest_rev, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0 || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /* Get the valid unit number to log in the error messages. */
    unit = hdl->params.unit;
    /* Verify the list is valid. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid - (lref_cnt=%u) - "
                                        "rv=%s.\n"),
                             hdl->linfo->lref_cnt,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /* Verify the lists are indeed shared else return error. */
    if (!itbm_ibkt_list_first_last_match(hdl->params, hdl_rev->params)
        || !shr_itbm_list_key_match(hdl->linfo->key, hdl_rev->params)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "hdl(LID=%u) hdl_rev(LID=%u) are not a "
                                        "shared list pair - rv=%s.\n"),
                             hdl->linfo->list_id, hdl_rev->linfo->list_id,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /*
     * This function is supported only for block_stat enabled shared indexed
     * bucket type lists, check and return error if not valid.
     */
    if (hdl->params.block_stat == 0 || hdl->params.buckets == 0
        || hdl_rev->params.block_stat == 0 || hdl_rev->params.buckets == 0
        || hdl_rev->params.reverse == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid params: hdl(LID=%u)/hdl_rev"
                                        "(LID=%u) - block_stat=%u/%u buckets=%u"
                                        "/%u reverse=%u/%u - rv=%s.\n"),
                             hdl->linfo->list_id, hdl_rev->linfo->list_id,
                             hdl->params.block_stat, hdl_rev->params.block_stat,
                             hdl->params.buckets, hdl_rev->params.buckets,
                             hdl->params.reverse, hdl_rev->params.reverse,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /*
     * If no blocks have been allocated from the list and user component
     * attempts to defragment the list, then return invalid parameter error
     * as defragmentation is not required for this list.
     */
    if (hdl->linfo->tot_blks_alloced == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Defrag not required, as all list "
                                        "elements are free and available - "
                                        "rv=%s.\n"),
                             shr_errmsg(SHR_E_PARAM)));
    }
    /* Return busy error, if the list is already undergoing defragmentation. */
    if (hdl->linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "LID=%u defrag by \"comp_id=%u\" "
                                        "in-progress - rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "lparams - hdl/hdl_rev):lid=%u/%u aid=%u/%u "
                              "u=%u/%u tab_id=%u/%u min=%u/%u max=%u/%u "
                              "f=%u/%u l=%u/%u comp_id=%d/%d bs=%u/%u "
                              "bkts=%u/%u epbkt=%u/%u rev=%u/%u).\n"),
                   hdl->linfo->list_id, hdl_rev->linfo->list_id,
                   hdl->aid, hdl_rev->aid,
                   hdl->params.unit, hdl_rev->params.unit,
                   hdl->params.table_id, hdl_rev->params.table_id,
                   hdl->params.min, hdl_rev->params.min,
                   hdl->params.max, hdl_rev->params.max,
                   hdl->params.first, hdl_rev->params.first,
                   hdl->params.last, hdl_rev->params.last,
                   (int)hdl->params.comp_id, (int)hdl_rev->params.comp_id,
                   hdl->params.block_stat, hdl_rev->params.block_stat,
                   hdl->params.buckets, hdl_rev->params.buckets,
                   hdl->params.entries_per_bucket,
                   hdl_rev->params.entries_per_bucket,
                   hdl->params.reverse, hdl_rev->params.reverse));
    }
    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(hdl->linfo->llock, SAL_MUTEX_FOREVER));
    /* Create a back-up copy of the current list elements state. */
    rv = shr_itbm_list_edata_backup_create(unit, hdl);
    if (SHR_FAILURE(rv)) {
        sal_mutex_give(hdl->linfo->llock);
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "edata_backup_create() - rv=%s.\n"),
                             shr_errmsg(rv)));
    }
    /* Set the defragmentation in-progress status. */
    hdl->linfo->defrag_inprog = TRUE;
    /* Defragment the shared list using forward alloc list handle. */
    rv = itbm_shrd_ibkt_list_defrag(unit, hdl);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "fwd: list(LID=%u) defrag error - "
                                        "rv=%s.\n"),
                             hdl->linfo->list_id,
                             shr_errmsg(rv)));
    }
    /* Defragment the shared list now using reverse alloc list handle. */
    rv = itbm_shrd_ibkt_list_defrag(unit, hdl_rev);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "rev: list(LID=%u) defrag error - "
                                        "rv=%s.\n"),
                             hdl->linfo->list_id,
                             shr_errmsg(rv)));
    }
    /* Get forward alloc list free block details. */
    shr_itbm_blk_t_init(largest);
    SHR_IF_ERR_VERBOSE_EXIT
        (itbm_ibkt_list_free_blk_get(unit, hdl, largest));
    /* Get reverse alloc list free block details. */
    shr_itbm_blk_t_init(largest_rev);
    SHR_VERBOSE_EXIT
        (itbm_ibkt_list_free_blk_get(unit, hdl_rev, largest_rev));
exit:
    if (hdl && hdl->linfo && hdl->linfo->defrag_inprog) {
        itbm_list_defrag_cleanup(unit, hdl);
        hdl->linfo->defrag_inprog = FALSE;
        hdl->linfo->dblks_moved = 0;
        hdl->linfo->defrag_hdl = NULL;
        sal_mutex_give(hdl->linfo->llock);
    }
    SHR_FUNC_EXIT();
}

int
shr_itbm_list_resize(shr_itbm_list_hdl_t hdl,
                     uint32_t new_first,
                     uint32_t new_last)
{
    int unit = BSL_UNIT_UNKNOWN; /* Initialize unit to unknown. */
    int rv = SHR_E_INTERNAL; /* Return value. */
    shr_itbm_list_info_t *linfo = NULL; /* Pointer to list info. */
    shr_itbm_lattr_t *lattr = NULL; /* Pointer to list attribute. */
    bool found = FALSE; /* List match found. */
    uint32_t rstart = SHR_ITBM_INVALID, rend = SHR_ITBM_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo, SHR_E_PARAM);
    SHR_NULL_CHECK(hdl->linfo->edata, SHR_E_PARAM);

    /* Validate unit number. */
    if (hdl->params.unit < 0 || hdl->params.unit >= SHR_ITBM_MAX_UNITS) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid unit(=%u) no. - rv=%s.\n"),
                             hdl->params.unit, shr_errmsg(SHR_E_PARAM)));
    }

    /* Verify the list is valid before trying to resize it. */
    if (!hdl->linfo->llock || !hdl->linfo->lref_cnt) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "List invalid (lref_cnt=%u), cannot "
                                        "resize - rv=%s.\n"),
                             hdl->linfo->lref_cnt, shr_errmsg(SHR_E_PARAM)));
    }

    /* Get the list unit number and list info ptr. */
    unit = hdl->params.unit;
    rv = shr_itbm_list_match_find(unit, hdl->params, &found, &linfo);
    if (rv != SHR_E_EXISTS || (found && linfo == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "List with params(found=%u): table_id=%u "
                                "min=%u max=%u first=%u last=%u comp_id=%u "
                                "bs=%u bkts=%u epbkt=%u rev=%u - rv=%s.\n"),
                     found, hdl->params.table_id, hdl->params.min,
                     hdl->params.max, hdl->params.first, hdl->params.last,
                     hdl->params.comp_id, hdl->params.block_stat,
                     hdl->params.buckets, hdl->params.entries_per_bucket,
                     hdl->params.reverse, shr_errmsg(rv)));
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    if (hdl->linfo != linfo) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid list handle.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* List defragmentation in-progress, no other operations are allowed. */
    if (linfo->defrag_inprog) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                            (BSL_META_U(unit,
                                        "List(ID=%u) defrag by \"comp_id=%u\" "
                                        "in-progress, cannot resize now - "
                                        "rv=%s.\n"),
                             hdl->linfo->list_id,
                             hdl->params.comp_id,
                             shr_errmsg(SHR_E_BUSY)));
    }
    /* Validate the new_first and new_last input parameter values. */
    if ((new_first < hdl->params.min || new_first > hdl->params.max)
        || (new_last < hdl->params.min || new_last > hdl->params.max)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "new_first=%u and new_last=%u must be"
                                        "within the min=%u and max=%u elements "
                                        "range - rv=%s.\n"),
                             new_first, new_last, hdl->params.min,
                             hdl->params.max, shr_errmsg(SHR_E_PARAM)));
    }
    /* Get the list attribute details. */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_attr_get(unit, hdl->linfo, hdl->aid, &lattr));
    SHR_NULL_CHECK(lattr, SHR_E_INTERNAL);
    /*
     * Verify and return E_BUSY error if elements are in use first - new_first
     * range.
     */
    if (hdl->params.block_stat && hdl->params.first != new_first) {
        rstart = (hdl->params.first < new_first) ? hdl->params.first
                                                    : new_first;
        rend = (hdl->params.first < new_first) ? new_first : hdl->params.first;
        if (shr_itbm_list_range_elem_in_use_check(hdl, rstart, rend)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "List elements in this range still in "
                                    "use first=%u new_first=%u - rv=%s).\n"),
                         hdl->params.first, new_first, shr_errmsg(SHR_E_BUSY)));
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }
    /*
     * Verify and return E_BUSY error if elements are in use last - new_last
     * range.
     */
    if (hdl->params.block_stat && hdl->params.last != new_last) {
        rstart = (hdl->params.last < new_last) ? hdl->params.last : new_last;
        rend = (hdl->params.last < new_last) ? new_last : hdl->params.last;
        if (shr_itbm_list_range_elem_in_use_check(hdl, rstart, rend)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "List elements in this range still in "
                                    "use last=%u new_last=%u - rv=%s).\n"),
                         hdl->params.last, new_last, shr_errmsg(SHR_E_BUSY)));
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }
    /* Take the list lock. */
    SHR_IF_ERR_EXIT
        (sal_mutex_take(linfo->llock, SAL_MUTEX_FOREVER));
    rv = itbm_list_resize(unit, hdl, lattr, new_first, new_last);
    if (SHR_FAILURE(rv)) {
        sal_mutex_give(linfo->llock);
        SHR_IF_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT
        (sal_mutex_give(linfo->llock));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Done: [List resize LID=%u new_first=%u new_last=%u]."
                          "\n"),
               linfo->list_id, new_first, new_last));
exit:
    SHR_FUNC_EXIT();
}
