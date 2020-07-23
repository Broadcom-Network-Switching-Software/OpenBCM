/*! \file rm_alpm_common.h
 *
 * Databases for RM ALPM all levels
 *
 * This file contains main routines which are internal to
 * ALPM resource manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_COMMON_H
#define RM_ALPM_COMMON_H

/*******************************************************************************
  Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_dq.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include <bcmptm/bcmptm_cth_alpm_be_internal.h>

#include "rm_alpm_trie.h"

/*******************************************************************************
  Defines
 */

#define MAX_LEVELS       (LEVELS)
#define MAX_PIVOT_LEVELS (LEVELS - 1)
#define MAX_ALPM_LTS            40

#define ALPM_NONE_BOOT  0
#define ALPM_COLD_BOOT  1
#define ALPM_WARM_BOOT  2

#define INVALID_INDEX           (-1)
#define INVALID_GROUP           (-1)
#define INVALID_LOG_BKT         (-1)
#define INVALID_SID             0

#define RESERVED_GROUP          0
#define FIRST_GROUP             1

#define UNPAIR_PKMS  2

#define ALPM_ALLOC(_ptr, _sz, _str) \
        SHR_ALLOC(_ptr, _sz, _str); \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY)

/*!< Max entries in a raw bucket */

#define PREV_LEVEL(n)  ((n) - 1)
#define NEXT_LEVEL(n)  ((n) + 1)

#define ALPM_MIN(a, b)              ((a) < (b) ? (a) : (b))
#define ALPM_MAX(a, b)              ((a) > (b) ? (a) : (b))

/*! ALPM database */
#define ALPM_DB_CNT(u, m)              (bcmptm_rm_alpm_database_cnt[u][m])
#define ALPM_DBS(u, m)                 (bcmptm_rm_alpm_database[u][m])
#define ALPM_DB(u, m, d)               (bcmptm_rm_alpm_database[u][m][d])
#define VRF_HDLS(u, m, d)              (bcmptm_rm_alpm_database[u][m][d].vrf_hdl)
#define VRF_HDL(u, m, d, v, w)         (bcmptm_rm_alpm_database[u][m][d].vrf_hdl[v][w])

#define DB_LMM_ELEMS(u, m, d)          (ALPM_DB(u, m, d).elems)
#define DB_LMM_BKTS(u, m, d)           (ALPM_DB(u, m, d).buckets)
#define DB_LMM_ADATAS(u, m, d)         (ALPM_DB(u, m, d).adatas)
#define DB_LEVELS(u, m, d)             (ALPM_DB(u, m, d).max_levels)
#define DB_LAST_LEVEL(u, m, d)         (ALPM_DB(u, m, d).max_levels - 1)
#define DB_VRF_HDL_CNT(u, m, d)        (ALPM_DB(u, m, d).vrf_hdl_cnt)
#define DB_VRF_HDL_LIST(u, m, d, v)    (ALPM_DB(u, m, d).vrf_hdl_list[v])

#define VRF_HDL_DQNODE(u, m, d, v, w)   (VRF_HDL(u, m, d, v, w).dqnode)
#define VRF_HDL_PTRIES(u, m, d, v, w)   (VRF_HDL(u, m, d, v, w).pivot_trie)
#define VRF_HDL_PTRIE(u, m, d, v, w, l) (VRF_HDL(u, m, d, v, w).pivot_trie[l])
#define VRF_HDL_RCNT(u, m, d, v, w)     (VRF_HDL(u, m, d, v, w).route_cnt)
#define VRF_HDL_IN_USE(u, m, d, v, w)   (VRF_HDL(u, m, d, v, w).in_use)
#define VRF_HDL_IN_DQ(u, m, d, v, w)    (VRF_HDL(u, m, d, v, w).in_dq)
#define VRF_HDL_DT(u, m, d, v, w)       (VRF_HDL(u, m, d, v, w).vrf_data_type)
#define VRF_HDL_VT(u, m, d, v, w)       (VRF_HDL(u, m, d, v, w).vt)
#define VRF_HDL_APP(u, m, d, v, w)      (VRF_HDL(u, m, d, v, w).app)
#define VRF_HDL_DFT(u, m, d, v, w)      (VRF_HDL(u, m, d, v, w).virtual_default)


#define SPLIT_EPARTIAL_REASON_UNKNOWN      0
#define SPLIT_EPARTIAL_REASON_RESPLIT      1
#define SPLIT_EPARTIAL_REASON_REINSERT     2
#define SPLIT_EFULL_REASON_UNKNOWN         0
#define SPLIT_EFULL_REASON_PIVOT_FULL      1


#define UPDATE_KEY          0x1
#define UPDATE_CTRL         0x2
#define UPDATE_DATA         0x4
#define UPDATE_ALL          0x7

#define HIT_OP_NONE         0x0
#define HIT_OP_AUTO         0x1

#define HIT_MODE_DISABLE            0x0
#define HIT_MODE_FORCE_CLEAR        0x1
#define HIT_MODE_FORCE_SET          0x2


#define DATA_TYPE_STRICT_FIT(req, act) (req == act)
#define DATA_TYPE_LOOSE_FIT(req, act)  (req <= act)

#define VIRTUAL_DEFAULT_LEN     -1

#define EPARTIAL_REASON_NONE            0
#define EPARTIAL_REASON_L3BKTAC_SPLIT   1
#define EPARTIAL_REASON_L3BKT_DETACHED  2

#define SPLIT_RETRY_NONE        0
#define SPLIT_RETRY_ONE         1
#define SPLIT_RETRY_TWO         2
#define SPLIT_RETRY_DELAYED     3

#define SHR_IF_ERR_EXIT_EXCEPT_IF2(_expr, _rv_except)           \
        SHR_IF_ERR_EXIT_EXCEPT_IF(_expr, _rv_except);           \
        if (_tmp_rv == _rv_except)

#define SHR_IF_ERR_VERBOSE_EXIT2(_expr, ver_rv)     \
        _tmp_rv = _expr;                            \
        if (_tmp_rv == ver_rv) {                    \
            SHR_IF_ERR_VERBOSE_EXIT(_tmp_rv);       \
        } else {                                    \
            SHR_IF_ERR_EXIT(_tmp_rv);               \
        }

/*******************************************************************************
  Typedefs
 */
/*!
 * \brief Raw bucket index
 */
typedef int rbkt_idx_t;

/*!
 * \brief Raw bucket group
 */
typedef rbkt_idx_t rbkt_group_t;


/*!
 * \brief ALPM VRF
 */
typedef uint32_t alpm_vrf_t;


/*!
 * \brief ALPM Level-1 pt entry
 */
typedef uint32_t L1_ENTRY_t[ALPM_L1_ENT_MAX];

/*!
 * \brief ALPM Level-N pt entry
 */
typedef uint32_t LN_ENTRY_t[ALPM_LN_ENT_MAX];


/*!
 * \brief Route.
 */
typedef struct alpm_elem_s alpm_route_t;
typedef struct bkt_split_aux_s {
    uint32_t aux_pivot_len;
    trie_ip_t aux_pivot_key;
    alpm_pivot_t *l3_pivot; /* Pivot of last level bucket */
    rm_alpm_trie_node_t *l3_split_root;
    uint8_t epartial_reason;
    uint8_t retry;
    uint8_t delayed_payload_spilt;
} bkt_split_aux_t;



/*!< Key part of elem */
typedef struct elem_key_s {
    /*!< Key tuple */
    key_tuple_t t;

     /*!< VRF type, for Level-1 only */
    alpm_vrf_type_t vt;
} elem_key_t;

/*!< Bucket part of elem, only for pivot levels. i.e., non-last level */
typedef struct elem_bkt_s {
    /*! To link all bkt_elem in same bucket */
    shr_dq_t list;

    /*!< Bucket trie */
    rm_alpm_trie_t *trie;

    /*!< Bucket default, i.e., BPM  */
    alpm_route_t *bpm;

    /*!< Raw bucket group */
    /*
     * For Ver0: Group | Sub Bkt
     * For Ver1: Group
     */
    log_bkt_t log_bkt;

} elem_bkt_t;

/*!
 * \brief ALPM pivot node structure
 */
typedef struct alpm_elem_s {
    /*!< trie node */
    rm_alpm_trie_node_t pvtnode;

    /* Bucket trie node in same bucket */
    rm_alpm_trie_node_t bktnode;

    /* Dq node in same bucket, for faster traverse */
    shr_dq_t dqnode;

    /*!< Element key */
    elem_key_t key;

    /*!< Entry index */
    ln_index_t index;

    /*!< Temporay Entry index */
    ln_index_t tmp_index;

    /*!< Bucket info for pivot */
    elem_bkt_t *bkt;

    /*!< Assoc data */
    assoc_data_t *ad;
} alpm_elem_t;


/*!
 * \brief VRF data type enumerations.
 */
typedef enum vrf_data_type_s {
    /*!< Last level dynamic, non-last level FULL. */
    VRF_DATA_DYNAMIC
} vrf_data_type_t;


/*!
 * \brief Global ALPM vrf handles
 *
 * Most ALPM resources are VRF based, this structure describes the VRF
 * related info.
 */
typedef struct alpm_vrf_hdl_s {
    /*!< DQ node to link all in-use VRF hdls */
    shr_dq_t dqnode;

    /*!< VRF hdls in-dq */
    bool in_dq;

    /*!< Pivot trie for all levels */
    rm_alpm_trie_t *pivot_trie[MAX_LEVELS];

    /*!< Route count */
    uint32_t route_cnt;

    /*!< VRF hdls in-use */
    bool in_use;

    /*!< IP version */
    alpm_ip_ver_t ipv;

    /*!< Application */
    uint8_t app;

    /*!< Weighted VRF */
    int w_vrf;

    /*!< VRF Type */
    int vt;

    /*!< Virtual default route. */
    alpm_route_t *virtual_default;

    /*!< VRF data type */
    vrf_data_type_t vrf_data_type;
} alpm_vrf_hdl_t;


/*!
 * \brief ALPM database
 *
 * ALPM provides the ability to allow different database co-exist.
 * Each database are compeletely separated and completely independant
 * to have different levels.
 */
typedef struct alpm_db_s {
    /*!< Elems LMM hdls. */
    shr_lmm_hdl_t elems;

    /*!< Buckets LMM hdls. */
    shr_lmm_hdl_t buckets;

    /*!< Assoc data LMM hdls. */
    shr_lmm_hdl_t adatas;

    /*!< Max level per database. */
    int max_levels;

    /*!< VRF handles per database. */
    alpm_vrf_hdl_t *vrf_hdl[IP_VER_COUNT];

    /*!< VRF handle count per database. */
    int vrf_hdl_cnt;

    /*!< List for all in-use VRF hdls, for faster traverse. */
    shr_dq_t vrf_hdl_list[IP_VER_COUNT];
} alpm_db_t;


/*!
 * \brief ALPM core configs
 */
typedef struct alpm_core_config_s {

    /*!< ALPM DBs */
    int alpm_dbs;

    /*!< ALPM mode */
    int alpm_mode;

    /*!< Num of levels  */
    int db_levels[DBS];

    /*!< Bank total info */
    bcmptm_cth_alpm_control_bank_t tot;

    /*!< Bank db0 info */
    bcmptm_cth_alpm_control_bank_t db0;

    /*!< Bank db1 info */
    bcmptm_cth_alpm_control_bank_t db1;

    /*!< Bank db2 info */
    bcmptm_cth_alpm_control_bank_t db2;

    /*!< Bank db3 info */
    bcmptm_cth_alpm_control_bank_t db3;

    /*!< Level1 key input on logical blocks */
    int l1_key_input[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 key input on physical blocks */
    int l1_phy_key_input[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 DB select on logical blocks */
    int l1_db[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 DB select on physical blocks */
    int l1_phy_db[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 logical blocks mapping to physical blocks */
    int l1_blocks_l2p[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 physical blocks mapping to logical blocks */
    int l1_blocks_p2l[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 blocks mapping is enabled. */
    int l1_blocks_map_en;

    /*!< Large vrf support */
    uint8_t large_vrf;

    /*!< Large vrf support */
    uint8_t large_l4_port;

    /*!< Compression key type */
    bcmptm_cth_alpm_comp_key_type_t comp_key_type;

} alpm_core_config_t;


/*!
 * \brief ALPM configs
 */
typedef struct alpm_config_s {
    /*!< Core settings. */
    alpm_core_config_t core;

    /*!< Type of hit support */
    uint8_t hit_support;

    /*!< Is PKM128 enabled for ALPM ver0 */
    bool ver0_128_enable;

    /*!< IPv4 unicast global low route strength profile_index */
    uint8_t ipv4_uc_sbr;

    /*!< IPv4 unicast private route strength profile_index */
    uint8_t ipv4_uc_vrf_sbr;

    /*!< IPv4 unicast override route strength profile_index */
    uint8_t ipv4_uc_override_sbr;

    /*!< IPv6 unicast global low route strength profile_index */
    uint8_t ipv6_uc_sbr;

    /*!< IPv6 unicast private route strength profile_index */
    uint8_t ipv6_uc_vrf_sbr;

    /*!< IPv6 unicast override route strength profile_index */
    uint8_t ipv6_uc_override_sbr;

    /*!< IPv4 compression strength profile_index */
    uint8_t ipv4_comp_sbr;

    /*!< IPv6 compression strength profile_index */
    uint8_t ipv6_comp_sbr;

    /*!< Destination. */
    uint16_t destination;

    /*!< Destination mask. */
    uint16_t destination_mask;

    /*!< Destination type if match. */
    uint8_t destination_type_match;

    /*!< Destination type if not match. */
    uint8_t destination_type_non_match;

} alpm_config_t;


/*!
 * \brief ALPM internal stats.
 */
typedef struct alpm_internals_s {
    uint32_t read;
    uint32_t write;
    uint32_t split;
    uint32_t split_write1;
    uint32_t split_write2;
    uint32_t rbkt_prepend;
    uint32_t rbkt_append;
    uint32_t shuffle_a;
    uint32_t ver0_bucket_share;
} alpm_internals_t;


/*******************************************************************************
  Function prototypes
 */

/*!
 * \brief Format type for a given database & level
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 *
 * \return Format type.
 */
extern format_type_t
bcmptm_rm_alpm_format_type_get(int u, int m, int db, int ln);

/*!
 * \brief Get ALPM config info
 *
 * \param [in] u Device u.
 *
 * \return ALPM config info ptr
 */
extern const alpm_config_t *
bcmptm_rm_alpm_config(int u, int m);

extern bool
bcmptm_rm_alpm_db_is_src(int u, int m, int db);

/*!
 * \brief Get database index for vt
 *
 * \param [in] u Device u.
 * \param [in] app Application
 * \param [in] is_src Is source application
 * \param [in] vt VRF type
 * \param [out] db Database.
 *
 * \return database index
 */
extern int
bcmptm_rm_alpm_db(int u, int m, uint8_t app, uint8_t is_src, alpm_vrf_type_t vt,
                  uint8_t *db);

/*!
 * \brief Insert bucket elem into trie & usage list.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] key Trie key of adding elem
 * \param [in] bkt Target bucket
 * \param [in] bkt_elem Payload to insert
 *
 * \return SHR_E_NONE, SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_bkt_insert(int u, int m, int ln, arg_key_t *key,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t *bkt_elem);

/*!
 * \brief Delete bucket elem from trie & usage list.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] key Trie key of deleting elem
 * \param [in] bkt Target bucket
 * \param [out] bkt_elem Payload deleted
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_bkt_delete(int u, int m, int ln, key_tuple_t *tp,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t **bucket);

/*!
 * \brief Lookup (exact match) bucket elem in trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot Pivot payload
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_bkt_lookup(int u, int m, key_tuple_t *tp,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t **bkt_elem);

/*!
 * \brief Allocate level elem based on a level, also init it.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] t Key tuple
 * \param [out] elem Elem payload
 *
 * \return SHR_E_NONE, SHR_E_MEMORY
 */
extern int
bcmptm_rm_alpm_elem_alloc(int u, int m, int db, int ln, bool direct_route,
                          arg_key_t *key,
                          alpm_elem_t **elem);

/*!
 * \brief Free pivot payload
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] elem Elem payload
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_elem_free(int u, int m, int db, int ln, bool direct_route, alpm_elem_t *elem);

/*!
 * \brief Insert pivot payload in pivot trie
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] tp ALPM key tuple
 * \param [out] pivot Inserting pivot payload
 *
 * \return SHR_E_NONE, SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_pvt_insert(int u, int m, int db, int ln,
                          bool direct_route,
                          arg_key_t *key, alpm_pivot_t *pivot);

/*!
 * \brief Delete pivot payload in pivot trie
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] tp ALPM key tuple
 * \param [out] pivot Deleted pivot payload
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_pvt_delete(int u, int m, int db, int ln,
                          bool direct_route,
                          key_tuple_t *tp, alpm_pivot_t **pivot);

/*!
 * \brief Lookup (exact match) pivot payload in pivot trie
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] tp ALPM key tuple
 * \param [out] pivot Pivot payload found
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_pvt_lookup(int u, int m, int db, int ln,
                          key_tuple_t *tp, alpm_pivot_t **pivot);

/*!
 * \brief Lookup (LPM match) pivot payload in pivot trie
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] tp ALPM key tuple
 * \param [out] pivot Pivot payload found
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_pvt_find(int u, int m, int db, int ln,
                        key_tuple_t *tp, alpm_pivot_t **pivot);

/*!
 * \brief Create pivot trie for a level
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] ipv IP version
 * \param [in] w_vrf Weighted VRF
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_pvt_trie_create(int u, int m, int db, int ln, int ipv,
                               int w_vrf, uint8_t app,
                               alpm_vrf_type_t vt);

/*!
 * \brief Destroy pivot trie for a level
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] ipv IP version
 * \param [in] w_vrf Weighted VRF
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_pvt_trie_destroy(int u, int m, int db, int ln, int ipv, int w_vrf);

/*!
 * \brief Get ALPM pivot trie
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] ln Level
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 * \param [out] pivot_trie Pivot trie
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_pvt_trie_get(int u, int m, int db, int ln, int w_vrf, int ipv,
                            rm_alpm_trie_t **pivot_trie);

/*!
 * \brief Set ALPM pivot trie
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] ln Level
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 * \param [in] pivot_trie Pivot trie
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_pvt_trie_set(int u, int m, int db, int ln, int w_vrf, int ipv,
                            rm_alpm_trie_t *pivot_trie);

/*!
 * \brief Increase route count
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 * \param [out] cnt Count after incresead
 *
 * \return nothing
 */
extern void
bcmptm_rm_alpm_vrf_route_inc(int u, int m, int db, int w_vrf, int ipv, int *cnt);

/*!
 * \brief Decrease route count
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 * \param [out] cnt Count after decreased
 *
 * \return nothing
 */
extern void
bcmptm_rm_alpm_vrf_route_dec(int u, int m, int db, int w_vrf, int ipv, int *cnt);

/*!
 * \brief Get route count
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 *
 * \return route count
 */
extern int
bcmptm_rm_alpm_vrf_route_cnt(int u, int m, int db, int w_vrf, int ipv);

/*!
 * \brief Get database
 *
 * \param [in] u Device u.
 * \param [in] db Database
 *
 * \return Pointer to database
 */
extern alpm_db_t*
bcmptm_rm_alpm_database_get(int u, int m, int db);

/*!
 * \brief Get database count
 *
 * \param [in] u Device u.
 *
 * \return Number of databases
 */
extern int
bcmptm_rm_alpm_database_count(int u, int m);


/*!
 * \brief Init level common database
 *
 * \param [in] u Device u.
 * \param [in] config ALPM config
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_database_init(int u, int m, alpm_config_t *config);

/*!
 * \brief Cleanup level common database
 *
 * \param [in] u Device u.
 *
 * \return nothing
 */
extern void
bcmptm_rm_alpm_database_cleanup(int u, int m);

/*!
 * \brief Reset internal counters
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_internals_reset(int u, int m);

/*!
 * \brief Set data type of middle level.
 *
 * \param [in] u Device u.
 * \param [in] db Database.
 * \param [in] full Full format if set.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_mid_dt_set(int u, int m, int db, alpm_data_type_t full);


/*!
 * \brief Get data type of middle level.
 *
 * \param [in] u Device u.
 * \param [in] db Database.
 * \param [in] app Applicaton.
 *
 * \return data type.
 */
extern alpm_data_type_t
bcmptm_rm_alpm_mid_dt_get(int u, int m, int db, int app);

#endif /* RM_ALPM_COMMON_H */

