/*! \file bcmptm_rm_alpm_internal.h
 *
 * APIs, defines for PTM to interface with ALPM Resource Manager
 * Mainly for V4 and V6 Route LTs
 * This file contains APIs, defines for PTRM-ALPM interfaces
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_ALPM_INTERNAL_H
#define BCMPTM_RM_ALPM_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <shr/shr_bitop.h>
#include <shr/shr_pb.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_cth_alpm_be_internal.h>
#include <bcmptm/bcmlrd_ptrm.h>

/*******************************************************************************
  Defines
 */
/*! Level 1 narrow view first half */
#define L1V_NARROW_0        0
/*! Level 1 narrow view second half */
#define L1V_NARROW_1        1
/*! Level 1 narrow view full */
#define L1V_NARROW_FULL     2
/*! Level 1 wide view */
#define L1V_WIDE            3

#define PKM_S        0 /*!< Single */
#define PKM_D        1 /*!< Double */
#define PKM_Q        2 /*!< Quad */
#define PKM_WS       3 /*!< Wide Single */
#define PKM_WD       4 /*!< Wide double */
#define PKM_WQ       5 /*!< Wide quad */
#define PKM_SS       6 /*!< Source Single */
#define PKM_SD       7 /*!< Source Double */
#define PKM_SQ       8 /*!< Source Quad */
#define PKM_FP       9 /*!< FP */
#define PKM_SFP      10 /*!< Source FP */
#define PKM_NUM      11 /*!< PKM count */
#define DECODE_PKM_NORMAL PKM_S  /*!< Normal PKM used for decode */
#define DECODE_PKM_WIDE   PKM_WS /*!< Wide PKM used for decode */
#define DECODE_PKM_FP     PKM_FP /*!< FP PKM used for decode */

#define LAYOUT_SH    0  /*!< Layout single and half */
#define LAYOUT_D     1  /*!< Layout double */
#define LAYOUT_NUM   2  /*!< Layout types */

/*!
 * \brief ALPM application types
 */
typedef enum {
    APP_LPM,     /*!< LPM application */
    APP_L3MC,    /*!< L3MC application */
    APP_COMP0,   /*!< FP compression application v0 */
    APP_COMP1,   /*!< FP compression application v1 */
    APP_NUM      /*!< Number of APP types */
} alpm_app_type_t;

/*! To check if a given pkm belongs to wide block */
#define WIDE_PKM(_pkm)  ((_pkm) == PKM_WS || (_pkm) == PKM_WD || (_pkm) == PKM_WQ)

/*! To check if a given pkm belongs to FP compression */
#define FP_PKM(_pkm)  ((_pkm) == PKM_FP || (_pkm) == PKM_SFP)

/*! Maximum TCAM/ALPM memory entry size in words  */
#define ALPM_L1_ENT_MAX      28   /* BCMPTM_MAX_PT_FIELD_WORDS (40) */
/*! Maximum ALPM memory entry size in words */
#define ALPM_LN_ENT_MAX      15   /* BCMPTM_MAX_PT_FIELD_WORDS (40) */

/*! Number of formats for ALPM2_DATA */
#define ALPM2_DATA_FORMATS    8
/*! Number of formats for ALPM1_DATA */
#define ALPM1_DATA_FORMATS    6
/*! Maximum Level 1 max field size in words */
#define DEFIP_MAX_PT_FIELD_WORDS  5


/*! ALPM level 0. */
#define LEVEL1 0
/*! ALPM level 1. */
#define LEVEL2 1
/*! ALPM level 1. */
#define LEVEL3 2
/*! ALPM levels. */
#define LEVELS 3
/*! ALPM 1 levels. */
#define ONE_LEVELS  1
/*! ALPM 2 levels. */
#define TWO_LEVELS  2

/*! ALPM instances. */
#define ALPM_INSTANCES 2

/*! Half wide. */
#define WIDE_HALF  0
/*! Single wide. */
#define WIDE_SINGLE 1
/*! Double wide. */
#define WIDE_DOUBLE 2

/*! ALPM DB 0. */
#define DB0  0
/*! ALPM DB 1. */
#define DB1  1
/*! ALPM DB 2. */
#define DB2  2
/*! ALPM DB 3. */
#define DB3  3
/*! ALPM DB number. */
#define DBS  4

/*! ALPM unsupported yet. */
#define ALPM_VERSION_NONE       0
/*! ALPM version 0. */
#define ALPM_VERSION_0          1
/*! ALPM version 1. */
#define ALPM_VERSION_1          2

/*! ALPM hit support version 0. */
#define ALPM_HIT_VERSION_0      0
/*! ALPM hit support version 1. */
#define ALPM_HIT_VERSION_1      1

/*! ALPM max key size in words. */
#define ALPM_MAX_KEY_SIZE_WORDS  10

#define IP_VER_4        0 /*!< IPv4 */
#define IP_VER_6        1 /*!< IPv6 */
#define IP_VER_COUNT    2 /*!< Number of IP versions. */


/* #define ALPM_DEBUG_FILE */
/* #define ALPM_DEBUG */

#ifdef ALPM_DEBUG

#define ALPM_ASSERT(_expr)          assert(_expr)
#define ALPM_ASSERT_ERRMSG(_expr)   if (!(_expr)) \
       {LOG_WARN(BSL_LOG_MODULE, (BSL_META("ALPM Check Fail\n")));}
#define LOG_V(C)    LOG_VERBOSE(BSL_LOG_MODULE, C)
#define LOG_D(C)    LOG_DEBUG(BSL_LOG_MODULE, C)
#else
/*! ALPM assert. */
#define ALPM_ASSERT(_expr)
/*! ALPM assert with error message. */
#define ALPM_ASSERT_ERRMSG(_expr)
/*! Verbose level log. */
#define LOG_V(C)
/*! Debug level log. */
#define LOG_D(C)

#endif

/*! Convenience macro for initialization. */
#define ALPM_LMM_INIT(_T, _hdl, _chunk_size, _multi_thread, _rv)  \
do {                                     \
    _rv = shr_lmm_init(_chunk_size,      \
            sizeof(_T),                  \
            0,                           \
            _multi_thread,               \
            #_T,                         \
            &_hdl);                      \
} while(0)
/*******************************************************************************
  Typedefs
 */

/*! \brief PT-specific info for ALPM type NPL LT
 */
typedef struct bcmptm_rm_alpm_hw_entry_info_s {

/*    bool non_aggr; */

    uint8_t num_insts;  /*!< Number of instances. */

    const uint8_t *ip_length;       /*!< IP length. */

    const uint16_t *num_ip_segs;    /*!< Number of IP segments. */
    const uint16_t **ip_start_bit;  /*!< IP start bit of physical entry. */
    const uint16_t **ip_end_bit;    /*!< IP end bit of physical entry. */
    const uint16_t **ip_start_bit_l; /*!< IP start bit of logical entry. */
    const uint16_t **ip_end_bit_l;  /*!< IP end bit of logical entry. */


    const uint16_t *num_ip_mask_segs;    /*!< Number of IP mask segments. */
    const uint16_t **ip_mask_start_bit;  /*!< IP mask start bit of physical entry. */
    const uint16_t **ip_mask_end_bit;    /*!< IP mask end bit of physical entry. */
    const uint16_t **ip_mask_start_bit_l; /*!< IP mask start bit of logical entry. */
    const uint16_t **ip_mask_end_bit_l;  /*!< IP mask end bit of logical entry. */

    const uint16_t *num_vrf_segs;       /*!< Number of VRF segments. */
    const uint16_t **vrf_start_bit;     /*!< VRF start bit of physical entry. */
    const uint16_t **vrf_end_bit;       /*!< VRF end bit of physical entry. */
    const uint16_t **vrf_start_bit_l;   /*!< VRF start bit of logical entry. */
    const uint16_t **vrf_end_bit_l;     /*!< VRF end bit of logical entry. */

    const uint16_t *num_vrf_mask_segs;       /*!< Number of VRF segments. */
    const uint16_t **vrf_mask_start_bit;     /*!< VRF start bit of physical entry. */
    const uint16_t **vrf_mask_end_bit;       /*!< VRF end bit of physical entry. */
    const uint16_t **vrf_mask_start_bit_l;   /*!< VRF start bit of logical entry. */
    const uint16_t **vrf_mask_end_bit_l;     /*!< VRF end bit of logical entry. */

    const uint8_t num_keys;             /*!< Number of key rows. */
    const bcmdrd_sid_t **sid;           /*!< TCAM sid. */

    const uint8_t num_datas;                /*!< Number of data rows. */
    const bcmdrd_sid_t **sid_data_only;     /*!< DATA only sid. */

    const uint16_t **key_type_val;      /*!< Key type value. */
    const uint16_t *data_start_bit;     /*!< Start bit of data portion. */
    const uint16_t *data_end_bit;       /*!< End bit of data portion. */
} bcmptm_rm_alpm_hw_entry_info_t;

/*!
 * \brief POLICY_DATA format information
 */
typedef struct pdd_field_info_s {
    /*! Field name in POLICY_FORMAT */
    bcmltd_fid_t fld;

    /*! Start bit of the field in POLICY_FORMAT */
    uint16_t start_bit;

    /*! End bit of the field in POLICY_FORMAT */
    uint16_t end_bit;
} pdd_field_info_t;


/*!
 * \brief LPM generic key field information
 */
typedef struct lpm_generic_key_field_s {

    /*! Start bit of all fields  */
    uint16_t start_bit;

    /*! End bit of all fields */
    uint16_t end_bit;

    /*! Start bit of the ip_addr field  */
    uint16_t ip_addr_start_bit;

    /*! End bit of the ip_addr field */
    uint16_t ip_addr_end_bit;

    /*! Start bit of the misc exact match field  */
    uint16_t misc_start_bit;

    /*! End bit of the misc exact match field  */
    uint16_t misc_end_bit;

} lpm_generic_key_field_t;

/*!
 * \brief ALPM Key type information.
 */
typedef struct bcmptm_alpm_key_type_info_s {
    /*! Key type for alpm ipv4 */
    int lpm_ipv4;

    /*! Key type for alpm ipv6 */
    int lpm_ipv6;

    /*! Key type for compression ipv4 */
    int comp_ipv4;

    /*! Key type for compression ipv6  */
    int comp_ipv6;
} bcmptm_alpm_key_type_info_t;


/*! \brief ALPM variant dependent information.
 */
typedef struct bcmptm_alpm_variant_info_s {
    /*! APP_LPM full mode POLICY_FORMAT */
    const pdd_field_info_t *l3uc_pdd_info;

    /*! Number of fields in APP_LPM full mode POLICY_FORMAT */
    size_t l3uc_pdd_info_count;

    /*! Array of POLICY_FORMAT data types */
    const uint8_t *pdd_data_types_array;

    /*! Number of POLICY_FORMAT data types */
    size_t pdd_data_types_array_count;

    /*! ALPM generic key field info for LPM_IPv4 */
    const lpm_generic_key_field_t *lpm_ipv4;

    /*! ALPM generic key field info for LPM_IPv6 */
    const lpm_generic_key_field_t *lpm_ipv6;

    /*! ALPM generic key field info for COMP_IPv4 */
    const lpm_generic_key_field_t *comp_ipv4;

    /*! ALPM generic key field info for COMP_IPv6 */
    const lpm_generic_key_field_t *comp_ipv6;

    /*! Key type for main pipe ALPM. */
    const bcmptm_alpm_key_type_info_t *main_key_type;

    /*! Key type for mtop ALPM. */
    const bcmptm_alpm_key_type_info_t *mtop_key_type;

} bcmptm_alpm_variant_info_t;

/*! \brief Place holder for more info as needed by ALPM rsrc_mgr from LRD
 * This contains data which is needed for ALPM rsrc_mgr apart from generic data.
 */
typedef struct bcmptm_rm_alpm_more_info_s {
    /*! Unpair TCAM packing mode, for encoding Level-1 unpair key */
    uint8_t pkm;

    /*! ALPM application type */
    uint8_t app;

    /*! Is source application LT */
    uint8_t is_src;

    /*! Level 1 entry view */
    uint8_t l1v;

    /*! Is ALPM IP version 6 */
    int ipv6;

    /*! VRF type */
    uint8_t vt;

    /*! Mtop */
    uint8_t mtop;

    /*! ALPM variant dependent information */
    const bcmptm_alpm_variant_info_t *variant_info;

    /*! \brief Length of array variant_info */
    uint32_t variant_info_count;
} bcmptm_rm_alpm_more_info_t;


/*! \brief Information requested by ALPM Resource Manager
 */
typedef struct bcmptm_rm_alpm_req_info_s {

/*! \brief Physical entry word size.
 */
    uint8_t pt_words;

/*! \brief Array of words holding key, mask fields of an entry.
 */

    uint32_t *ekw;

/*! \brief Array of words holding data fields of an entry.
 */
    uint32_t *edw;

/*! \brief Logical ID for dfield_format
 */
    uint16_t dfield_format_id;

/*! \brief Tells PTRM that this is same key as previous LOOKUP op.
 * - For more information Refer to the definition in bcmptm.h
 */
    bool same_key;

/*! \brief Entry priority, for NPL DEFIP LT. */
    uint32_t entry_pri;

/*! \brief Num of words in rsp_ekw_buf array
 * - Mainly for sanity check
 */
    size_t rsp_ekw_buf_wsize;

/*! \brief Num of words in rsp_edw_buf array
 * - Mainly for sanity check
 */
    size_t rsp_edw_buf_wsize;

/*! \brief Count of array rsp_entry_index & array rsp_entry_sid
 * - Suggest to set to 3, as this is the max levels.
 */
    uint8_t rsp_entry_max_count;

/*! \brief Hardware entry info. */
    bcmptm_rm_alpm_hw_entry_info_t *hw_info;

/*! \brief More info as needed by ALPM rsrc_mgr from LRD */
    const bcmptm_rm_alpm_more_info_t *rm_more_info;

/*! \brief Length of array rm_more_info */
    uint32_t rm_more_info_count;
} bcmptm_rm_alpm_req_info_t;

/*! \brief Information responded by ALPM Resource Manager */
typedef struct bcmptm_rm_alpm_rsp_info_s {

/*! \brief  Array of words holding key, mask fields of found entry.
 * - Match formats of L3_DEFIP or L3_DEFIP_PAIR_128 view.
 */
    uint32_t *rsp_ekw;

/*! \brief  Array of words holding data fields of found entry.
 * - Match formats of L3_DEFIP or L3_DEFIP_PAIR_128 view.
 */
    uint32_t *rsp_edw;

/*! \brief Logical ID for dfield_format of found entry (all ops).
 * - If entry was found, PTRM will return logical dfield_format.
 * - If entry was not found, PTRM will set this var to 0.
 */
    uint16_t rsp_dfield_format_id;

/*! \brief Pointer to info needed for TABLE_PT_OP_INFO */
    bcmptm_pt_op_info_t *rsp_pt_op_info;

/*! \brief Pointer to misc_info (eg: bcmptm_table_info_t). */
    void *rsp_misc;
} bcmptm_rm_alpm_rsp_info_t;


/*!
 * \brief Logical bucket index
 */
typedef int log_bkt_t;

/*!
 * \brief ALPM level-n index
 */
typedef int ln_index_t;

/*!
 * \brief ALPM IP address
 */
typedef uint32_t alpm_ip_t[10];


/*!
 * \brief ALPM IP address
 */
typedef uint32_t alpm_format_ip_t[20];


/*!
 * \brief ALPM IP versions
 */
typedef uint8_t alpm_ip_ver_t;


/*!
 * \brief ALPM trie format prefix
 */
typedef struct trie_ip_s {
    uint32_t key[10];       /*!< Common Key ALPM_MAX_KEY_SIZE_WORDS */
} trie_ip_t;

/*!
 * \brief ALPM data types
 */
typedef enum alpm_data_type_e {
     DATA_REDUCED        = 0,    /*!< Reduced data*/
     DATA_FULL           = 1,    /*!< Full data */
     DATA_FULL_2         = 2,    /*!< Full data 2 */
     DATA_FULL_3         = 3,    /*!< Full data 3 */
     DATA_TYPES                  /*!< Number of data types */
} alpm_data_type_t;

/*!
 * \brief ALPM KEY types
 */
typedef enum {
     KEY_TYPE_INVALID   = -1,   /*!< INVALID KEY type, force type int */
     KEY_IPV4           = 0,    /*!< IPv4 key type */
     KEY_IPV6_SINGLE        = 1,    /*!< Ipv6 len [0, 32] */
     KEY_IPV6_DOUBLE        = 2,    /*!< Ipv6 len [33, 64] */
     KEY_IPV6_QUAD       = 3,    /*!< Ipv6 len [65, 128] */
     KEY_L3MC_V4        = 4,    /*!< L3 IPv4 multicast */
     KEY_L3MC_V6        = 5,    /*!< L3 IPv6 multicast */
     KEY_COMP0_V4       = 6,    /*!< L3 IPv4 compression v0 */
     KEY_COMP0_V6       = 7,    /*!< L3 IPv6 compression v0 */
     KEY_COMP1_V4       = 8,    /*!< L3 IPv4 compression v1 */
     KEY_COMP1_V6       = 9,    /*!< L3 IPv6 compression v1 */
     KEY_TYPES                  /*!< Number of KEY types */
} alpm_key_type_t;


/*!
 * \brief ALPM sid types
 */
typedef enum {
     SID_TYPE_TCAM      = 0,        /*!< TCAM sid.*/
     SID_TYPE_TCAM2     = 1,        /*!< TCAM sid.*/
     SID_TYPE_DATA      = 2,        /*!< Data only sid. */
     SID_TYPE_COMBINED  = 3,        /*!< Combined sid. */
     SID_TYPES                      /*!< Number of sid types. */
} alpm_sid_type_t;

/*!
 * \brief ALPM sid index map info.
 */
typedef struct alpm_sid_index_map_s {
    uint32_t        index;          /*!< Logical index. */
    uint8_t         wide;           /*!< Entry wide if level-1, otherwise leveln. */
    alpm_sid_type_t sid_type;       /*!< SID type. */
    bcmdrd_sid_t    sid;            /*!< SID. */
    uint32_t        sid_index;      /*!< Index for this SID. */
} alpm_sid_index_map_t;

/*!
 * \brief ALPM sid index reverse map (hw_sid & index to base_sid & index + other view) output info.
 */
typedef struct alpm_sid_index_out_s {
    bcmdrd_sid_t    base_sid;       /*!< Base SID. */
    int             base_index;     /*!< Base index. */
    int             bank_depth;     /*!< Bank depth */
} alpm_sid_index_out_t;

/*!
 * \brief ALPM VRF types
 */
typedef enum {
    VRF_TYPE_INVALID    = -1,   /*!< INVALID VRF type, force type int */
    VRF_DEFAULT         = 0,    /*!< DEFAULT VRF, for other apps */
    VRF_PRIVATE         = 0,    /*!< PRIVATE VRF */
    VRF_GLOBAL          = 1,    /*!< GLOBAL VRF */
    VRF_OVERRIDE        = 2,    /*!< OVERRIDE VRF */
    VRF_TYPES                   /*!< Number of VRF types */
} alpm_vrf_type_t;

/*!
 * \brief ALPM key tuple
 */
typedef struct key_tuple_s {
    int           w_vrf;       /*!< Weighted VRF (hold L4_PORT in APP_COMP0) */
    trie_ip_t     trie_ip;     /*!< Trie format key */
    int           len;         /*!< Prefix len */
    uint8_t       app;         /*!< Application */
    alpm_ip_ver_t ipv;         /*!< IP version */
    uint16_t      max_bits;    /*!< Max bits of key */
} key_tuple_t;

/*!
 * \brief ALPM format type
 */
typedef enum format_type_e {
    FORMAT_PIVOT,
    FORMAT_ROUTE,
    FROMAT_TYPES
} format_type_t;

/*!
 * \brief ALPM Format buffer
 * Max base entry 15 words.
 */
typedef uint32_t ln_base_entry_t[20];

/*!
 * \brief Bucket elem.
 */
typedef struct alpm_elem_s alpm_bkt_elem_t;

/*!
 * \brief Pivot
 */
typedef struct alpm_elem_s alpm_pivot_t;

/*!
 * \brief ALPM assoc data
 */
typedef struct assoc_data_s {
    uint32_t src_discard:1;      /*!< Source discard */
    uint32_t flex_stat_valid:1;  /*!< Flex counter valid */
    uint32_t default_miss:1;     /*!< For Ver0 */
    uint32_t global_route:1;     /*!< For Ver0 */
    uint32_t fmt_ad3:20;         /*!< For Ver0 */
    uint32_t user_data_type:2;   /*!< User data type */
    uint32_t actual_data_type:2; /*!< Actual data type */
    uint32_t rsv0:4;             /*!< Reserved */
    uint16_t fmt_ad1;            /*!< For Ver0 */
    uint16_t fmt_ad2;            /*!< For Ver0 */
    uint32_t fmt_ad[DEFIP_MAX_PT_FIELD_WORDS]; /*!< Assoc data buf */
} assoc_data_t;

/*!
 * \brief ALPM data for level-2 pivot
 */
typedef struct alpm2_data_s {
    uint32_t phy_bkt:16;        /*!< Physical bucket */
    uint32_t kshift:8;          /*!< Kshift */
    uint32_t rofs:3;            /*!< Rofs */
    uint32_t rsv0:4;            /*!< Reserved */
    uint32_t default_miss:1;    /*!< Default miss */
    uint8_t fmt[ALPM2_DATA_FORMATS];    /*!< Formats */
} alpm2_data_t;

/*!
 * \brief ALPM data for level-1 pivot
 */
typedef struct alpm1_data_s {
    uint32_t phy_bkt:16;    /*!< Physical bucket */
    uint32_t kshift:8;      /*!< Kshift */
    uint32_t rofs:3;        /*!< Rofs */
    uint32_t sub_bkt:2;     /*!< Sub bucket */
    uint32_t is_route:1;        /*!< Is route */
    uint32_t default_miss:1;    /*!< Default miss */
    uint32_t direct_route:1;    /*!< Direct route */

    uint32_t hit_idx:24;    /*!< Hit index, for Ver0 */
    uint32_t rsv0:8;        /*!< Reserved */
    uint8_t fmt[ALPM1_DATA_FORMATS];    /*!< Formats */
} alpm1_data_t;

/*!
 * \brief ALPM arg key
 */
typedef struct arg_key_s {
    alpm_vrf_type_t     vt;          /*!< VRF type */
    alpm_ip_t           ip_addr;     /*!< IP address */
    alpm_ip_t           ip_mask;     /*!< IP address mask */
    alpm_key_type_t     kt;          /*!< Key type, for Level-1 only */
    key_tuple_t         t;           /*!< Key tuple */
    int                 pfx;         /*!< Weighted prefix len, for Level-1 */
} arg_key_t;

/*!
 * \brief ALPM arguments containing all info describing a route
 */
typedef struct alpm_arg_s {
    /*! Associate Data */
    assoc_data_t   ad;
    /*! ALPM1 data */
    alpm1_data_t   a1;
    /*! ALPM2 data */
    alpm2_data_t   a2;
    /*! Fixed Data Part */
    uint32_t profile_index:4;
    /*! Route Hit */
    uint32_t hit:1;
    /*! Default route (len == 0) */
    uint32_t default_route:1;
    /*! Valid Field */
    uint32_t valid:2;
    /*! In move between level-1 blocks */
    uint32_t in_move:1;
    /*! Unpair TCAM packing mode, for encoding Level-1 unpair key */
    uint32_t pkm:4;
    /*! Database index */
    uint32_t db:2;
    /*! Encode replace data */
    uint32_t repl_data_encode:1;
    /*! Decode replace data */
    uint32_t repl_data_decode:1;
    /*! Format */
    uint32_t fmt:3;
    /*! Default miss */
    uint32_t default_miss:1;
    /*! Encode data only */
    uint32_t encode_data_only:1;
    /*! Fill data only if set, otherwise fill key as well */
    uint32_t decode_data_only:1;
    /*! Update with data type changed */
    uint32_t dt_update:1;
    /*! A new insert */
    uint32_t new_insert:1;
    /*! SW state changed */
    uint32_t state_changed:1;
    /*! Resource full */
    uint32_t resouce_full:1;
    /*! Encode data-only view */
    uint32_t encode_data_view:1;
    /*! Encode key-only view */
    uint32_t encode_key_view:1;
    /*! Encode lt view */
    uint32_t encode_lt_view:1;
    /*! Decode lt view */
    uint32_t decode_lt_view:1;
    /*! VRF init */
    uint32_t vrf_init:1;
    /*! LPM large vrf  */
    uint32_t lpm_large_vrf:1;
    /*! COMP large port  */
    uint32_t comp_large_port:1;
    /*! Match ready */
    uint32_t match_ready:1;
    /*! Request update (excluding data type change) */
    uint32_t req_update:1;
    /*! User data type */
    uint32_t data_type:2;
    /*! Is mtop */
    uint32_t mtop:1;
    /*! Reserved */
    uint32_t reserved:25;
    /*! Key part */
    arg_key_t key;
    /*! Group for bucket, attached with pivot */
    log_bkt_t log_bkt[LEVELS];
    /*! Pt index of levels */
    ln_index_t index[LEVELS];
    /*! Pivot */
    alpm_pivot_t *pivot[LEVELS];
    /*! Original index. */
    ln_index_t orig_index;
    /*! COMP key type */
    bcmptm_cth_alpm_comp_key_type_t comp_key_type;

    /*! Pointer to the next element in the list (or NULL). */
    struct alpm_arg_s *next;
}
 alpm_arg_t;

/*!
 * \brief ALPM device specific info
 */
typedef struct alpm_dev_info_s {
    int max_vrf;                /*!< MAX VRFs */
    int max_fp_op;              /*!< MAX FP Opaque (e.g. L4_PORT) */
    int tcam_depth;             /*!< TCAM depth */
    int tcam_blocks;            /*!< TCAM blocks */
    int l1_blocks;              /*!< L1 blocks */

    bcmdrd_sid_t unpair_sid;    /*!< Level-1 Single-wide SID */
    bcmdrd_sid_t pair_sid;      /*!< Level-1 Double-wide SID */
    bcmdrd_sid_t wide_unpair_sid;    /*!< Level-1 Single-wide SID */
    bcmdrd_sid_t wide_pair_sid;      /*!< Level-1 Double-wide SID */
    bcmdrd_sid_t tcam_only_sid; /*!< Level-1 Tcam only SID */
    bcmdrd_sid_t data_only_sid; /*!< Level-1 Data only SID */
    bcmdrd_sid_t wide_tcam_only_sid; /*!< Wide level-1 Tcam only SID */
    bcmdrd_sid_t wide_data_only_sid; /*!< Wide level-1 Data only SID */
    bcmdrd_sid_t l1_hit_sid;    /*!< Level-1 HIT SID */
    bcmdrd_sid_t l2_sid;        /*!< Level-2 SID */
    bcmdrd_sid_t l2_hit_sid;    /*!< Level-2 HIT SID */
    bcmdrd_sid_t l3_sid;        /*!< Level-3 SID */
    bcmdrd_sid_t l3_hit_sid;    /*!< Level-3 HIT SID */
    int feature_urpf;           /*!< uRPF feature supported */
    int feature_pkms;           /*!< Number of packing modes supported */
    int feature_max_levels;     /*!< Max levels supported */
    int feature_cache_dt;       /*!< Cache data type */
    int pivot_max_format_3;       /*!< Max format value for pivot */
    int route_max_format_3;       /*!< Max format value for route */
    int pivot_max_format_2;       /*!< Max format value for pivot */
    int route_max_format_2;       /*!< Max format value for route */
    int pivot_max_format_1;       /*!< Max format value for pivot */
    int route_max_format_1;       /*!< Max format value for route */
    int pivot_max_format_0;       /*!< Max format value for pivot */
    int route_max_format_0;       /*!< Max format value for route */
    int pivot_min_format_3;     /*!< Start format value for pivot */
    int route_min_format_3;     /*!< Start format value for route */
    int pivot_min_format_2;     /*!< Start format value for pivot */
    int route_min_format_2;     /*!< Start format value for route */
    int pivot_min_format_1;     /*!< Start format value for pivot */
    int route_min_format_1;     /*!< Start format value for route */
    int pivot_min_format_0;     /*!< Start format value for pivot */
    int route_min_format_0;     /*!< Start format value for route */
    int *l2p_route_fmts;        /*!< Logical to physical route format map (NULL for one to one map) */
    void *wo_template;          /*!< Template wide entry for VRF override */
    void *wp_template;          /*!< Template wide entry for VRF private */
    void *wg_template;          /*!< Template wide entry for VRF global */
    void *no_template;          /*!< Template narrow entry for VRF override */
    void *np_template;          /*!< Template narrow entry for VRF private */
    void *ng_template;          /*!< Template narrow entry for VRF global */
    uint8_t n_size;             /*!< Template narrow entry size */
    uint8_t w_size;             /*!< Template wide entry size */
    uint8_t wn_size;            /*!< Template wide block narrow entry size */
    uint8_t ww_size;            /*!< Template wide block wide entry size */
    uint8_t alpm_ver;           /*!< ALPM version */
    uint8_t hit_ver;            /*!< ALPM hit support version */
    uint8_t max_base_entries;   /*!< Max base entries. */
    uint8_t key_data_separate[2];  /*!< Key data separated. */
    uint8_t l1_data_only_x2;    /*!< If set, data only index is always x2 TCAM_ONLY index */
    uint8_t l1_tcams_shared;    /*!< L1 TCAM resources are shared between RM_TCAM
                                 * and RM_ALPM. */
    uint16_t single_tcam_start_bit;     /*!< Start bit of tcam portion in single wide view. */
    uint16_t single_tcam_end_bit;       /*!< End bit of tcam portion in single wide view. */
    uint16_t single_data_0_start_bit;   /*!< Start bit of data_0 portion in single wide view. */
    uint16_t single_data_0_end_bit;     /*!< End bit of data_0 portion in single wide view. */
    uint16_t single_data_1_start_bit;   /*!< Start bit of data_1 portion in single wide view. */
    uint16_t single_data_1_end_bit;     /*!< End bit of data_1 portion in single wide view. */
    uint16_t double_data_start_bit;     /*!< Start bit of data portion in double wide view. */
    uint16_t double_data_end_bit;       /*!< End bit of data portion in double wide view. */
    uint16_t double_tcam_0_start_bit;   /*!< Start bit of tcam_0 portion in double wide view. */
    uint16_t double_tcam_0_end_bit;     /*!< End bit of tcam_0 portion in double wide view. */
    uint16_t double_tcam_1_start_bit;   /*!< Start bit of tcam_1 portion in double wide view. */
    uint16_t double_tcam_1_end_bit;     /*!< End bit of tcam_1 portion in double wide view. */
    uint8_t l1_ipv6_single_len;     /*!< IPv6 length in single wide */
    uint8_t l1_ipv6_double_len;     /*!< IPv6 length in double wide */
    uint8_t l1_ipv6_quad_len;       /*!< IPv6 length in quad wide */
    uint8_t feature_l1_ipv6_len_nonstandard;  /*!< If set IPv6 length is nonstandard */
    uint8_t feature_has_4_data_types;     /*!< Has 4 data type */
    uint8_t feature_large_data_type[DBS];  /*!< Is large data type */

} alpm_dev_info_t;

/*! Function type for Level-1 entry encode */
typedef int (*l1_encode_f) (int u, int m, void *arg, void *de, void *se, uint8_t l1v);

/*! Function type for Level-1 entry data part decode */
typedef int (*l1_decode_f) (int u, int m, void *arg, void *de, void *se, uint8_t l1v);

/*! Function type for Level-1 half entry move */
typedef int (*l1_half_entry_move_f) (int u, int m, void *src, void *dst,
                                     int x, int y, int pkm);


/*! Function type for Level-1 half entry clear. */
typedef int (*l1_half_entry_clear_f) (int u, int m, void *de, int x, int pkm);

/*! Function type for Level-1 multidepth info */
typedef int (*l1_multidepth_info_get_f) (int u, int m, bcmdrd_sid_t *sid, int *index, int *count);

/*! Function type for Level-N multidepth info */
typedef int (*ln_multidepth_info_get_f) (int u, int m, bcmdrd_sid_t *sid, int *index);

/*! Function type for Level-1 sid info map. */
typedef int(*l1_sid_index_map_f) (int u, int m, alpm_sid_index_map_t *map_info);

/*! Function type for Level-1 sid reverse map (hw sid to base sid). */
typedef int(*l1_sid_index_reverse_map_f) (int u, int m,
                                          bcmdrd_sid_t hw_sid,
                                          alpm_sid_index_out_t *out_info);

/*! Function type for device info initialization */
typedef int (*device_info_init_f) (int u, int m, alpm_dev_info_t *dev);

/*! Function type for retrieveing base entry info */
typedef int (*ln_base_entry_info_f) (int u, int m,
                                     format_type_t format_type,
                                     uint8_t format_value,
                                     int *prefix_len,
                                     int *full,
                                     int *number);

/*! Function type for base entry enocding */
typedef int (*ln_base_entry_encode_f) (int u, int m, format_type_t format_type,
                                       uint8_t format_value,
                                       uint8_t kshift,
                                       alpm_arg_t *arg,
                                       uint32_t *entry);

/*! Function type for base entry decoding */
typedef int (*ln_base_entry_decode_f) (int u, int m, format_type_t format_type,
                                       uint8_t format_value,
                                       uint8_t kshift,
                                       alpm_arg_t *arg,
                                       uint32_t *entry);

/*! Function type for raw bucket encoding */
typedef int (*ln_raw_bucket_encode_f) (int u, int m, format_type_t format_type,
                                       uint8_t format_value,
                                       ln_base_entry_t *base_entry,
                                       int entry_count,
                                       void *buf);

/*! Function type for raw bucket decoding */
typedef int (*ln_raw_bucket_decode_f) (int u, int m, format_type_t format_type,
                                       uint8_t format_value,
                                       ln_base_entry_t *base_entry,
                                       int entry_count,
                                       void *buf);

/*! Function type for raw bucket entry get */
typedef int (*ln_raw_bucket_entry_get_f) (int u, int m, format_type_t format_type,
                                          uint8_t format_value,
                                          uint32_t *base_entry,
                                          int ent_ofs,
                                          void *buf);

/*! Function type for raw bucket entry set */
typedef int (*ln_raw_bucket_entry_set_f) (int u, int m, format_type_t format_type,
                                          uint8_t format_value,
                                          uint32_t *base_entry,
                                          int ent_ofs,
                                          void *buf);

/*! Function type for base entry move */
typedef int (*ln_base_entry_move_f) (int u, int m, format_type_t format_type,
                                     uint8_t src_format_value,
                                     int src_ent_ofs,
                                     key_tuple_t *src_t,
                                     void *src_bucket,
                                     alpm_data_type_t src_dt,

                                     uint8_t dst_format_value,
                                     int dst_ent_ofs,
                                     key_tuple_t *dst_t,
                                     void *dst_bucket);

/*! Function type for hit bit get */
typedef int (*ln_hit_get_f) (int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                             uint8_t *hit);

/*! Function type for hit bit set */
typedef int (*ln_hit_set_f) (int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                             uint8_t hit);

/*!
 * \brief ALPM device specific routines
 */
typedef struct alpm_driver_s {
    /*! Callback for level-1 entry encode */
    l1_encode_f l1_encode;

    /*! Callback for level-1 entry decode */
    l1_decode_f l1_decode;

    /*! Callback for level-n sid info */
    l1_sid_index_map_f ln_sid_index_map;

    /*! Callback for level-1 sid info */
    l1_sid_index_map_f l1_sid_index_map;

    /*! Callback for level-1 sid index reverse map info */
    l1_sid_index_reverse_map_f l1_sid_index_reverse_map;

    /*! Callback for level-1 half entry move */
    l1_half_entry_move_f l1_half_entry_move;

    /*! Callback for level-1 half key move */
    l1_half_entry_move_f l1_half_key_move;

    /*! Callback for level-1 half entry clear */
    l1_half_entry_clear_f l1_half_key_clear;

    /*! Callback for level-n base entry encode */
    ln_base_entry_encode_f ln_base_entry_encode;

    /*! Callback for level-n base entry decode */
    ln_base_entry_decode_f ln_base_entry_decode;

    /*! Callback for level-n raw bucket enocde */
    ln_raw_bucket_encode_f ln_raw_bucket_encode;

    /*! Callback for level-n raw bucket deocde */
    ln_raw_bucket_decode_f ln_raw_bucket_decode;

    /*! Callback for level-n raw bucket entry get */
    ln_raw_bucket_entry_get_f ln_raw_bucket_entry_get;

    /*! Callback for level-n raw bucket entry set */
    ln_raw_bucket_entry_set_f ln_raw_bucket_entry_set;

    /*! Callback for level-n base entry info get */
    ln_base_entry_info_f ln_base_entry_info;

    /*! Callback for level-n base entry move */
    ln_base_entry_move_f ln_base_entry_move;

    /*! Callback for level-n hit bit get */
    ln_hit_get_f ln_hit_get;

    /*! Callback for level-n hit bit set */
    ln_hit_set_f ln_hit_set;

    /*! Callback for device info initialization */
    device_info_init_f device_info_init;
}
 bcmptm_rm_alpm_driver_t;

/*******************************************************************************
  Function prototypes
 */
/*!
 * \brief Perform Lookup, Insert, Delete for ROUTE LTs
 *
 * \param [in] u Logical device id
 *
 * \param [in] req_flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 *
 * \param [in] cseq_id Transaction id. Must be passed with every cmdproc_write, read
 *
 * \param [in] req_ltid Logical Table enum for current req_op
 *
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 *
 * \param [in] req_op Only LOOKUP, DELETE, INSERT ops are valid for tables
 *             with key
 *
 * \param [in] req_info See the definition for bcmptm_rm_alpm_req_info_t
 *
 * \param [out] rsp_info See the definition for bcmptm_rm_alpm_rsp_info_t
 *
 * \param [out] rsp_ltid LTID of found entry.
 * \n 1. If entry was found, for all ops, PTRM will return ltid of found
 *       entry from PTcache.
 * \n 2. If entry was not found, PTRM will return ALL_ZEROs for LOOKUP,
 *       DELETE ops,
 * \n 3. If entry was not found, PTRM will return req_ltid for INSERT op.
 *
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND Entry not found for Lookup, Delete ops
 * \retval SHR_E_FULL Could not perform requested Insert
 * \retval SHR_E_UNAVAIL Requested op is not supported
 */
extern int
bcmptm_rm_alpm_req(int u,
                   uint64_t req_flags,
                   uint32_t cseq_id,
                   bcmltd_sid_t req_ltid,
                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                   bcmptm_op_type_t req_op,
                   bcmptm_rm_alpm_req_info_t *req_info,

                   bcmptm_rm_alpm_rsp_info_t *rsp_info,
                   bcmltd_sid_t *rsp_ltid,
                   uint32_t *rsp_flags);


/*!
 * \brief Initialize the Resource manager for ALPMs.
 * \param [in] u Logical device id
 * \param [in] warm Warm boot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_alpm_init(int u, bool warm);

/*!
 * \brief Stop the Resource manager for ALPMs.
 * \param [in] u Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_alpm_stop(int u);

/*!
 * \brief De-initialize the Resource manager for ALPMs.
 * \param [in] u Logical device id
 * \param [in] graceful Graceful cleanup
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_alpm_cleanup(int u, bool graceful);

/*!
 * \brief RM ALPM Commit, Abort, Idle_Check
 *
 * \param [in] u Logical device id
 * \param [in] flags usage TBD
 * \param [in] cseq_id Transaction id
 * \param [in] trans_cmd Commit, Abort, Idle_Check (See bcmptm_trans_cmd_t)
 *
 * \retval SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_trans_cmd(int u,
                         uint64_t flags,
                         uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd);

/*!
 * \brief Initialize global resources for Resource manager for ALPMs.
 *
 * \retval SHR_E_NONE Success, SHR_E_MEMORY Fails.
 */
extern int
bcmptm_rm_alpm_global_init(void);

/*!
 * \brief Cleanup global resources for the Resource manager for ALPMs.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_alpm_global_cleanup(void);

/*!
 * \brief ALPM mode
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 *
 * \return Combined/Parallel
 */
extern int
bcmptm_rm_alpm_mode(int u, int m);
/*!
 * \brief Max levels for a given database
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 * \param [in] db Database
 *
 * \return Max levels.
 */
extern int
bcmptm_rm_alpm_max_levels(int u, int m, int db);


/*!
 * \brief Register device specfic driver for ALPM
 *
 * \param [in] u Device u.
 * \param [in] drv Device specific structure containing routine pointers.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_driver_register(int u, bcmptm_rm_alpm_driver_t *drv);

/*!
 * \brief Get device specfic variant infomation.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 *
 * \return SHR_E_XXX
 */
extern const bcmptm_alpm_variant_info_t *
bcmptm_rm_alpm_variant_get(int u, int m);

/*!
 * \brief Dump ALPM info
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 * \param [in] db Database.
 * \param [in] verbose Verbose.
 * \param [in] brief Brief message format.
 * \param [in] pb Print buffer.
 *
 * \return nothing.
 */
void
bcmptm_rm_alpm_info_dump(int u, int m, int db, int verbose, int brief, shr_pb_t *pb);

/*!
 * \brief Show ALPM L1/L2/L3 config info
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 * \param [in] pb Print buffer.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_config_show(int u, int m, shr_pb_t *pb);

/*!
 * \brief Abort current transaction for RM ALPM
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 * \param [in] full True if full abort, otherwise partial abort.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_abort(int u, int m, bool full);

/*!
 * \brief Commit current transaction for RM ALPM
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 * \param [in] force_sync Force sync.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_commit(int u, int m, bool force_sync);

/*!
 * \brief Do sanity check against LT.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 * \param [in] ltid which the sanity check done against.
 *                  INVALID_SID indicates all ALPM LTS.
 * \param [out] num_fails number of entries that fails sanity.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_sanity(int u, int m, bcmltd_sid_t ltid, int *num_fails);

/*!
 * \brief Dump ALPM HW table data from top to bottom
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 * \param [in] db Database.
 * \param [in] l1_index TCAM level1 index (-1 for all).
 * \param [in] levels ALPM levels to dump (from top to bottom).
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_table_dump(int u, int m, int db, int l1_index, int levels);

/*!
 * \brief Check whether in large vrf mode.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 *
 * \return True if in large vrf mode, otherwise not.
 */
extern int
bcmptm_rm_alpm_is_large_vrf(int u, int m);


/*!
 * \brief Check whether in large l4 port mode.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 *
 * \return True if in large l4 port mode, otherwise not.
 */
extern int
bcmptm_rm_alpm_is_large_l4_port(int u, int m);

/*!
 * \brief Check compression key type.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 *
 * \return alpm compression key type.
 */
extern int
bcmptm_rm_alpm_comp_key_type(int u, int m);

/*!
 * \brief Return VRF mask length.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 *
 * \return VRF mask length.
 */
extern int
bcmptm_rm_alpm_vrf_len(int u, int m);

/*!
 * \brief Check whether has 4 data types.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 *
 * \return True if has 4 data types, otherwise not.
 */

extern int
bcmptm_rm_alpm_has_4_data_types(int u, int m);

/*!
 * \brief Check whether is large data type.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 * \param [in] db Database.
 *
 * \return True if large data type, otherwise not.
 */

extern int
bcmptm_rm_alpm_is_large_data_type(int u, int m, int db);


/*!
 * \brief Check whether is data type cached.
 *
 * \param [in] u Device u.
 * \param [in] m Mtop
 *
 * \return True if cache data type, otherwise not.
 */

extern int
bcmptm_rm_alpm_cache_data_type(int u, int m);

/*!
 * \brief Return ALPM resource usage stats.
 * \param [in] u Device.
 * \param [in] m Mtop
 * \param [in] level ALPM levels
 * \param [in] info See the definition for bcmptm_cth_alpm_usage_info_t
 *
 * \return SHR_E_XXX
 */
extern void
bcmptm_rm_alpm_usage_get(int u, int m, int level, bcmptm_cth_alpm_usage_info_t *info);

#endif /* BCMPTM_RM_ALPM_INTERNAL_H */

