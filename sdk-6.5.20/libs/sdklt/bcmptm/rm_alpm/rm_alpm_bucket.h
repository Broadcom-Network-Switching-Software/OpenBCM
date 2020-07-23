/*! \file rm_alpm_bucket.h
 *
 * Main routines for RM ALPM Level-N bucket
 *
 * This file contains routines manage ALPM Level-N (SRAM) resouces,
 * which are internal to ALPM resource manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_BUCKET_H
#define RM_ALPM_BUCKET_H


/*******************************************************************************
   Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include "rm_alpm.h"

/*******************************************************************************
 * Defines
 */
#define HEAD 1
#define BODY 0
#define MAX_INT_VALUE           0x7FFFFFFF
#define DISABLED_FORMAT         0
#define DEFAULT_FORMAT          1
#define INVALID_FORMAT          0xFF
#define VER0_FORMAT_5           5
#define VER0_FORMAT_4           4
#define VER0_FORMAT_3           3
#define VER0_FORMAT_2           2
#define VER0_FORMAT_1           1

#define MAX_SUB_BKTS            4

#define ALPM_BIT_GET(_a, _b)          ((_a) & (1 << (_b)))
#define ALPM_BIT_SET(_a, _b)          ((_a) |= (1 << (_b)))
#define ALPM_BIT_CLR(_a, _b)          ((_a) &= ~(1 << (_b)))

#define BKT_HDL(u, m, _d, _l)            (sbkt_hdl[u][m][_d][_l])
#define RBKTS(u, m, _d, _l)              (BKT_HDL(u, m, _d, _l)->rbkt_array)
#define RBKT(u, m, _d, _l, _i)           (BKT_HDL(u, m, _d, _l)->rbkt_array[_i])
#define GROUP_USAGES(u, m, _d, _l)       (BKT_HDL(u, m, _d, _l)->group_usage)
#define GROUP_USAGE(u, m, _d, _l, _c)    (BKT_HDL(u, m, _d, _l)->group_usage[_c])
#define GROUP_USAGE_CNT(u, m, _d, _l)    (BKT_HDL(u, m, _d, _l)->group_usage_count)
#define FMT_GROUP_USAGES(u, m, _d, _l, _f)     \
    (BKT_HDL(u, m, _d, _l)->fmt_group_usage[_f])
#define FMT_GROUP_USAGE(u, m, _d, _l, _f, _c)  \
    (BKT_HDL(u, m, _d, _l)->fmt_group_usage[_f][_c])

#define BKT_HDL_GROUPS_USED(u, m, _d, _l)  (BKT_HDL(u, m, _d, _l)->groups_used)
#define BKT_HDL_RBKTS_TOTAL(u, m, _d, _l)  (BKT_HDL(u, m, _d, _l)->rbkts_total)
#define BKT_HDL_RBKTS_USED(u, m, _d, _l)   (BKT_HDL(u, m, _d, _l)->rbkts_used)
#define BKT_HDL_BASE_RBKT(u, m, _d, _l)    (BKT_HDL(u, m, _d, _l)->base_rbkt)
#define BKT_HDL_MAX_BANKS(u, m, _d, _l)    (BKT_HDL(u, m, _d, _l)->max_banks)
#define BKT_HDL_START_BANK(u, m, _d, _l)   (BKT_HDL(u, m, _d, _l)->start_bank)
#define BKT_HDL_LAST_BANK(u, m, _d, _l)    (BKT_HDL(u, m, _d, _l)->last_bank)
#define BKT_HDL_BUCKET_BITS(u, m, _d, _l)  (BKT_HDL(u, m, _d, _l)->bucket_bits)
#define BKT_HDL_FORMAT_TYPE(u, m, _d, _l)  (BKT_HDL(u, m, _d, _l)->format_type)
#define BKT_HDL_GROUP_LIST(u, m, _d, _l)   (BKT_HDL(u, m, _d, _l)->global_group_list)
#define BKT_HDL_THRD_BANKS(u, m, _d, _l)   (BKT_HDL(u, m, _d, _l)->thrd_banks)
#define BKT_HDL_BANK_BITMAP(u, m, _d, _l)  (BKT_HDL(u, m, _d, _l)->bank_bitmap)

#define GROUP_USAGE_LIST(u, m, _d, _l, _c)  (GROUP_USAGE(u, m, _d, _l, _c).groups)
#define GROUP_USAGE_COUNT(u, m, _d, _l, _c) (GROUP_USAGE(u, m, _d, _l, _c).count)
#define FMT_GROUP_USAGE_LIST(u, m, _d, _l, _f, _c)  \
    (FMT_GROUP_USAGE(u, m, _d, _l, _f, _c).groups)
#define FMT_GROUP_USAGE_COUNT(u, m, _d, _l, _f, _c) \
    (FMT_GROUP_USAGE(u, m, _d, _l, _f, _c).count)

#define RBKT_IDX(u, m, _d, _l, _i)          (RBKT(u, m, _d, _l, _i).index)
#define RBKT_USG_NODE(u, m, _d, _l, _i)     (RBKT(u, m, _d, _l, _i).usgnode)
#define RBKT_FMT_USG_NODE(u, m, _d, _l, _i) (RBKT(u, m, _d, _l, _i).fmt_usgnode)
#define RBKT_LIST_NODE(u, m, _d, _l, _i)    (RBKT(u, m, _d, _l, _i).listnode)

#define EXTRA_UPDATE_DT     1
#define EXTRA_NEW_INSERT    2
#define EXTRA_NEW_VRF       3

typedef enum {
    BUCKET_SHARE = 0,
    BUCKET_MERGE_CHILD_TO_PARENT,
    BUCKET_MERGE_PARENT_TO_CHILD
} bucket_merge_type_t;

/*******************************************************************************
  Typedefs
 */
typedef struct bucket_traverse_data_s {
    shr_dq_t *dq[DATA_TYPES];
    uint32_t count;
    bool found_new;
} bucket_traverse_data_t;

/*!
 * \brief ALPM raw bucket
 */
typedef struct rbkt_s {
    /*!< Rbkt index for this rbkt, only inited once */
    int index;

    /*!< Entry bitmap */
    uint32_t entry_bitmap;

    /*!< Format value */
    uint8_t format;

    /*!< Is current rbkt head of group */
    uint8_t is_head;

    /*!< Valid banks for this group, valid for HEAD only */
    uint8_t valid_banks;

    /*!< Free banks for this group, valid for HEAD only */
    uint32_t free_banks;

    /*!< DQ node to link all group heads in the Global List */
    shr_dq_t listnode;

    /*!< DQ node to link all group heads in the Usage List */
    shr_dq_t usgnode;

    /*!< DQ node to link all group heads in the Usage List */
    shr_dq_t fmt_usgnode;

    /*!< Pivot pointer for this group, valid for HEAD only */
    alpm_pivot_t *pivot;

    /*!< No. of entries in group */
    uint8_t count;
     /*!< Bitmap of logical buckets */
    uint8_t sub_bkts;
     /*!< Pivot indices corresponding to logical buckets */
    alpm_pivot_t *pivots[MAX_SUB_BKTS];
} rbkt_t;

/*!
 * \brief ALPM group usage list
 */
typedef struct usage_list_s {
    /*!< Free count of all these groups in the dq list.
     * The biggest count is the max_banks, if a group's
     * free buckets is more than the max_banks, that group will also
     * add to the same list with count equals to max_banks.
     * Therefore, the count is not accurate for these groups, which
     * may have larger count.
     */
    int count;

    /*!< Group DQ list to link all groups with same free count */
    shr_dq_t groups;
} usage_list_t;

/*!
 * \brief ALPM Bucket Handle
 *
 * ALPM bucket handle contains the info describing bucket info in a level
 */
typedef struct bkt_hdl_s {
    /*!< Sase rbkt, first rbkt of start bank */
    int base_rbkt;

    /*!< Number of used rbkt groups */
    int groups_used;

    /*!< Total number of rbkts */
    int rbkts_total;

    /*!< Used number of rbkts */
    int rbkts_used;

    /*!< Big array of rbkts */
    rbkt_t *rbkt_array;

    /*!< Max banks  */
    uint8_t max_banks;

    /*!< Threshold banks  */
    uint8_t thrd_banks;

    /*!< Start bank  */
    uint8_t start_bank;

    /*!< Last bank  */
    uint8_t last_bank;

    /*!< Bank bitmap */
    uint32_t bank_bitmap;

    /*!< Bucket bits */
    uint8_t bucket_bits;

    /*!< Logical to physical bank array */
    uint8_t l2p_bank_array[ALPM_LN_BANKS_MAX];

    /*!< Physical to logical bank array */
    uint8_t p2l_bank_array[ALPM_LN_BANKS_MAX];

    /*!< Format type */
    format_type_t format_type;

    /*!< Global group list to link all groups in a sequential way */
    shr_dq_t global_group_list;

    /*!< Array of usage list, each with different free count */
    usage_list_t *group_usage;

    /*!< Count of array group_usage */
    uint8_t group_usage_count;

    /*!< Array of usage list, each with different format & free count */
    usage_list_t **fmt_group_usage;
} bkt_hdl_t;


typedef void (*bkt_hdl_cleanup_f)(int u, int m);
typedef int (*bkt_hdl_init_f)(int u, int m, bool recover);
typedef int (*bkt_hdl_get_f)(int u, int m, int db, int ln, bkt_hdl_t **bkt_hdl);
typedef void (*alpm_data_encode_f)(int u, int m, int ln, alpm_arg_t *arg, void *adata);
typedef alpm_pivot_t *(*bucket_pivot_get_f)(int u, int m, int db, int ln,
                                            ln_index_t ln_index);
typedef void (*bucket_pivot_set_f)(int u, int m, int db, int ln, log_bkt_t group,
                                   alpm_pivot_t *pivot);
typedef int (*bucket_free_f)(int u, int m, int db, int ln, log_bkt_t group);
typedef int (*bucket_alloc_f)(int u, int m, int ln, alpm_arg_t *arg, log_bkt_t *group);
typedef int (*bucket_delete_f)(int u, int m, int db, int ln, alpm_pivot_t *pivot,
                               ln_index_t ent_idx);
typedef int (*bucket_insert_f)(int u, int m, int ln, alpm_arg_t *arg,
                               alpm_pivot_t *pivot);
typedef int (*bucket_split_insert_f)(int u, int m, int ln, alpm_arg_t *arg,
                                     alpm_pivot_t *parent_pivot,
                                     bkt_split_aux_t *split_aux);
typedef int (*entry_write_f)(int u, int m, int ln, alpm_arg_t *arg);
typedef int (*entry_read_f)(int u, int m, int ln, alpm_arg_t *arg);
typedef uint32_t (*entry_hw_idx_get_f)(int u, int m, int db, int ln,
                                       ln_index_t ent_idx);
typedef void (*bucket_sanity_f)(int u, int m, int db, int ln, log_bkt_t log_bkt);
typedef int (*bucket_recover_f)(int u, int m, int ln, alpm_arg_t *arg);
typedef int (*bucket_recover_done_f)(int u, int m, int db, int ln);
typedef int (*bucket_resource_check_f)(int u, int m, alpm_arg_t *arg, int extra);
typedef int (*bucket_table_dump_f)(int u, int m, int ln, int lvl_cnt,
                                   alpm_arg_t *arg, int *count);

typedef struct bucket_mgmt_s {
    bkt_hdl_cleanup_f bkt_hdl_cleanup;
    bkt_hdl_init_f bkt_hdl_init;
    bkt_hdl_get_f  bkt_hdl_get;
    alpm_data_encode_f alpm_data_encode;
    bucket_pivot_get_f bucket_pivot_get;
    bucket_pivot_set_f bucket_pivot_set;
    bucket_free_f bucket_free;
    bucket_alloc_f bucket_alloc;
    bucket_delete_f bucket_delete;
    bucket_insert_f bucket_insert;
    bucket_split_insert_f bucket_split_insert;
    entry_write_f entry_write;
    entry_read_f entry_read;
    entry_hw_idx_get_f entry_hw_idx_get;
    bucket_sanity_f bucket_sanity;
    bucket_recover_f bucket_recover;
    bucket_recover_done_f bucket_recover_done;
    bucket_resource_check_f bucket_resource_check;
    bucket_table_dump_f bucket_table_dump;
} bucket_mgmt_t;
/*******************************************************************************
   Function prototypes
 */
/*!
 * \brief Cleanup ALPM bucket handle.
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX.
 */
extern void
bcmptm_rm_alpm_bkt_hdl_cleanup(int u, int m);

/*!
 * \brief Initiliaze ALPM bucket handle.
 *
 * \param [in] u Device u.
 * \param [in] recover True if recover process.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_bkt_hdl_init(int u, int m, bool recover);

/*!
 * \brief Get alpm bkt handle for level-n
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [out] bkt_hdl Bucket handle
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND.
 */
extern int
bcmptm_rm_alpm_bkt_hdl_get(int u, int m, int db, int ln,
                           bkt_hdl_t **bkt_hdl);


extern void
bcmptm_rm_alpm_adata_encode(int u, int m, int ln, alpm_arg_t *arg, void *adata);

extern alpm_pivot_t *
bcmptm_rm_alpm_bucket_pivot_get(int u, int m, int db, int ln, ln_index_t ln_index);
extern void
bcmptm_rm_alpm_bucket_pivot_set(int u, int m, int db, int ln, log_bkt_t group,
                                alpm_pivot_t *pivot);
extern int
bcmptm_rm_alpm_bucket_free(int u, int m, int db, int ln, log_bkt_t group);
extern int
bcmptm_rm_alpm_bucket_alloc(int u, int m, int ln, alpm_arg_t *arg, log_bkt_t *group);

extern int
bcmptm_rm_alpm_bucket_delete(int u, int m, int db, int ln, alpm_pivot_t *pivot,
                             ln_index_t ent_idx);

extern int
bcmptm_rm_alpm_bucket_insert(int u, int m, int ln, alpm_arg_t *arg,
                             alpm_pivot_t *pivot);

extern int
bcmptm_rm_alpm_bucket_split_insert(int u, int m, int ln, alpm_arg_t *arg,
                            alpm_pivot_t *parent_pivot,
                            bkt_split_aux_t *split_aux);

extern int
bcmptm_rm_alpm_bucket_entry_write(int u, int m, int ln, alpm_arg_t *arg);

extern int
bcmptm_rm_alpm_bucket_entry_read(int u, int m, int ln, alpm_arg_t *arg);

extern int
bcmptm_rm_alpm_bucket_entry_hw_idx_get(int u, int m, int db, int ln, ln_index_t ent_idx,
                                uint32_t *hw_idx);

extern int
bcmptm_rm_alpm_bucket_recover(int u, int m, int ln, alpm_arg_t *arg);

extern int
bcmptm_rm_alpm_bucket_recover_done(int u, int m, int db, int ln);

extern int
bcmptm_rm_alpm_bucket_resource_check(int u, int m, alpm_arg_t *arg, int extra);

extern int
bcmptm_rm_alpm_bucket_table_dump(int u, int m, int ln, int lvl_cnt, alpm_arg_t *arg, int *count);

/*!
 * \brief Sanity check for a given rbkt group
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Rbkt group
 * \param [in] with_holes If true, there exists holes in the group, else false.
 * \param [in] vbanks Valid banks
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_bucket_sanity(int u, int m, int db, int ln, log_bkt_t log_bkt);

extern void
bcmptm_rm_alpm_buckets_sanity(int u, int m, int db, int ln, int w_vrf, bool merge);

extern int
bcmptm_rm_alpm_bucket_ver0_register(int u, bucket_mgmt_t *bmgmt);
extern int
bcmptm_rm_alpm_bucket_ver1_register(int u, bucket_mgmt_t *bmgmt);
extern int
bcmptm_rm_alpm_bucket_init(int u, int m);
extern void
bcmptm_rm_alpm_bucket_cleanup(int u, int m);

#endif /* RM_ALPM_BUCKET_H */

