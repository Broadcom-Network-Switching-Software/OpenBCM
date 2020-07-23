/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains Service meters(global meters) definitions internal
 * to the BCM library.
 */
#ifndef _BCM_INT_SVC_METER_H
#define _BCM_INT_SVC_METER_H


#include <shared/bsl.h>

#include <bcm/types.h>
#include <sal/core/libc.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <bcm/error.h>
#include <bcm/policer.h>
#include <soc/mcm/memregs.h>

#define BCM_POLICER_TYPE_REGEX       0x1
#define BCM_POLICER_TYPE_SHIFT  24
#define BCM_POLICER_TYPE_MASK   0x0f000000
#define BCM_POLICER_IS_REGEX_METER(policer_id) \
  ((((policer_id) & BCM_POLICER_TYPE_MASK) >> BCM_POLICER_TYPE_SHIFT)== BCM_POLICER_TYPE_REGEX)

#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_CNG_ATTR_BITS 0x00000001
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_INT_PRI_ATTR_BITS 0x00000002
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_VLAN_FORMAT_ATTR_BITS 0x00000004
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_OUTER_DOT1P_ATTR_BITS 0x00000008
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_INNER_DOT1P_ATTR_BITS 0x00000010
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_INGRESS_PORT_ATTR_BITS 0x00000020
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_TOS_ATTR_BITS 0x00000040
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_PKT_RESOLUTION_ATTR_BITS 0x00080
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_SVP_TYPE_ATTR_BITS 0x00000100
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_DROP_ATTR_BITS 0x00000200
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_IP_PKT_ATTR_BITS 0x00000400
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_SHORT_INT_PRI_ATTR_BITS 0x00000800
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_TOS_ECN_ATTR_BITS 0x00001000
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_PREEMPTABLE_ATTR_BITS 0x00002000
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_ETHERTYPE_ATTR_BITS 0x00004000
/* UDF to be the highest bit */
#define BCM_POLICER_SVC_METER_UNCOMPRESSED_USE_UDF_ATTR_BITS 0x80000000

#define BCM_SVC_METER_PORT_MAP_SIZE         172
#define BCM_SVC_METER_MAP_SIZE_64           64
#define BCM_SVC_METER_MAP_SIZE_256          256
#define BCM_SVC_METER_MAP_SIZE_1024         1024

#define    BCM_POLICER_SVC_METER_MAX_MODE                4
#define    BCM_POLICER_SVC_METER_OFFSET_TABLE_KEY_SIZE   8
#define    BCM_POLICER_SVC_METER_OFFSET_TABLE_MAX_INDEX  4
#define    BCM_POLICER_SVC_METER_MAX_OFFSET              256
#define    BCM_POLICER_SVC_METER_MAX_ATTR_SELECTORS      256
#define    SVC_METER_UDF_MAX_BIT_POSITION                16
#define    SVC_METER_UDF1_MAX_BIT_POSITION               32
#define    SVC_METER_UDF1_VALID                          0x1
#define    SVC_METER_UDF2_VALID                          0x2
#define    SVC_METER_UDF_MAX_SUB_DIVISIONS               \
                                BCM_POLICER_SVC_METER_OFFSET_TABLE_KEY_SIZE
#define SVC_METER_MAX_SELECTOR_BITS                    \
                                BCM_POLICER_SVC_METER_OFFSET_TABLE_KEY_SIZE

#define BCM_POLICER_SVC_METER_CNG_ATTR_SIZE                             2
#define BCM_POLICER_SVC_METER_IFP_ATTR_SIZE                             2
#define BCM_POLICER_SVC_METER_INT_PRI_ATTR_SIZE                         4
#define BCM_POLICER_SVC_METER_SHORT_INT_PRI_ATTR_SIZE                   3
#define BCM_POLICER_SVC_METER_VLAN_FORMAT_ATTR_SIZE                     2
#define BCM_POLICER_SVC_METER_OUTER_DOT1P_ATTR_SIZE                     3
#define BCM_POLICER_SVC_METER_INNER_DOT1P_ATTR_SIZE                     3
#define BCM_POLICER_SVC_METER_ING_PORT_ATTR_SIZE                        6
#define BCM_POLICER_SVC_METER_TOS_ATTR_SIZE                             6
#define BCM_POLICER_SVC_METER_TOS_ECN_ATTR_SIZE                         2
#define BCM_POLICER_SVC_METER_PKT_REOLUTION_ATTR_SIZE                   6
#define BCM_POLICER_SVC_METER_SVP_TYPE_ATTR_SIZE                        1
#define BCM_POLICER_SVC_METER_SVP_NETWORK_GROUP_ATTR_SIZE               3
#define BCM_POLICER_SVC_METER_DROP_ATTR_SIZE                            1
#define BCM_POLICER_SVC_METER_IP_PKT_ATTR_SIZE                          1
#define BCM_POLICER_SVC_METER_KT2_ING_PORT_ATTR_SIZE                    8
#define BCM_POLICER_SVC_METER_SB2_ING_PORT_ATTR_SIZE                    7

#define BCM_POLICER_GLOBAL_METER_INDEX_MASK  0x1FFFFFFF
#define BCM_POLICER_GLOBAL_METER_MODE_MASK   0xE0000000
#define BCM_POLICER_GLOBAL_METER_MODE_SHIFT  29
#define BCM_POLICER_GLOBAL_METER_NO_OFFSET_MODE 0x20000000
#define BCM_POLICER_GLOBAL_METER_DISABLE  0x1
#define BCM_POLICER_GLOBAL_METER_MAX_POOL 8
#define BCM_POLICER_GLOBAL_METER_MAX_BANKS_PER_POOL 2
#define BCM_POLICER_GLOBAL_METER_GROUP_MODE_TYPE_MASK 0xFF000000
#define BCM_POLICER_GLOBAL_METER_GROUP_MODE_TYPE_SHIFT 24

#define BCM_POLICER_GLOBAL_METER_ACTION_CHANGE_INT_PRI_BIT_POS 16
#define BCM_POLICER_GLOBAL_METER_ACTION_CHANGE_ECN_BIT_POS 17
#define BCM_POLICER_GLOBAL_METER_ACTION_CHANGE_DOT1P_BIT_POS 18
#define BCM_POLICER_GLOBAL_METER_ACTION_CHANGE_CNG_BIT_POS 19
#define BCM_POLICER_GLOBAL_METER_ACTION_CHANGE_DSCP_BIT_POS 21

#define BCM_POLICER_GLOBAL_METER_ACTION_ECN_BIT_POS 0
#define BCM_POLICER_GLOBAL_METER_ACTION_DSCP_BIT_POS 2
#define BCM_POLICER_GLOBAL_METER_ACTION_DOT1P_BIT_POS 8
#define BCM_POLICER_GLOBAL_METER_ACTION_DROP_BIT_POS 11
#define BCM_POLICER_GLOBAL_METER_ACTION_INT_PRI_BIT_POS 12


#define BCM_POLICER_GLOBAL_METER_ACTION_DSCP_MASK 0xfc
#define BCM_POLICER_GLOBAL_METER_ACTION_DOT1P_MASK 0x700
#define BCM_POLICER_GLOBAL_METER_ACTION_INT_PRI_MASK 0xf000
#define BCM_POLICER_GLOBAL_METER_ACTION_SHORT_INT_PRI_MASK 0x7000
#define BCM_POLICER_GLOBAL_METER_ACTION_ECN_MASK 0x3
#define BCM_POLICER_GLOBAL_METER_ACTION_CNG_MASK 0x180000

#define BCM_POLICER_GLOBAL_METER_MAX_ACTIONS  8192

#define GLOBAL_METER_ALLOC_VERTICAL  0x0
#define GLOBAL_METER_ALLOC_HORIZONTAL  0x1


#define GLOBAL_METER_REFRESH_MAX_DISABLE_LIMIT 0x1f
#define GLOBAL_METER_MODE_DEFAULT 0x0
#define GLOBAL_METER_MODE_CASCADE 0x1
#define GLOBAL_METER_MODE_TR_TCM 0x2
#define GLOBAL_METER_MODE_TR_TCM_MODIFIED 0x3
#define GLOBAL_METER_MODE_SR_TCM 0x4
#define GLOBAL_METER_MODE_SR_TCM_MODIFIED 0x5

#define BCM_GLOBAL_METER_MAX_SCACHE_ENTRIES 16
/* Mode Id = 0 is used only to create policer group with 1 policer
 * member. So, it basically do not store any information which
 * require storing to scache. Hence, not allocating size to it.
 */
#define BCM_GLOBAL_METER_MAX_SCACHEABLE_GROUP_MODE (BCM_POLICER_SVC_METER_MAX_MODE - 1)

/* Generic memory allocation routine. */
#define _GLOBAL_METER_XGS3_ALLOC(_ptr_,_size_,_descr_)                 \
    do {                                             \
         if (NULL == (_ptr_)) {                       \
             (_ptr_) = sal_alloc((_size_), (_descr_)); \
         }                                            \
         if((_ptr_) != NULL) {                        \
              sal_memset((_ptr_), 0, (_size_));        \
         }  else {                                    \
             LOG_ERROR(BSL_LS_BCM_POLICER,                              \
                       (BSL_META("Error:Alloc failure %s\n"), (_descr_))); \
         }                                                              \
    } while (0)

#define _GLOBAL_METER_HASH_SIZE 0x100
#define _GLOBAL_METER_HASH_MASK 0xff



#define _GLOBAL_METER_HASH_INSERT(_hash_ptr_, _inserted_ptr_, _index_)    \
    do {                                                 \
        (_inserted_ptr_)->next = (_hash_ptr_)[(_index_)]; \
        (_hash_ptr_)[(_index_)] = (_inserted_ptr_);       \
    } while (0)

#define _GLOBAL_METER_HASH_REMOVE(_hash_ptr_, _entry_type_, _removed_ptr_, _index_)  \
    do {                                                    \
         _entry_type_ *_prev_ent_ = NULL;                    \
         _entry_type_ *_lkup_ent_ = (_hash_ptr_)[(_index_)]; \
         while (NULL != _lkup_ent_) {                        \
             if (_lkup_ent_ != (_removed_ptr_))  {           \
                 _prev_ent_ = _lkup_ent_;                    \
                 _lkup_ent_ = _lkup_ent_->next;              \
                 continue;                                   \
             }                                               \
             if (_prev_ent_!= NULL) {                        \
                 _prev_ent_->next = (_removed_ptr_)->next;   \
             } else {                                        \
                (_hash_ptr_)[(_index_)] = (_removed_ptr_)->next; \
             }                                               \
             break;                                          \
        }                                                   \
    } while (0)

#define BCM_SVM_DMA_CHUNK_SIZE    4096

/* Policer service meter Modes. */
typedef enum bcm_policer_svc_meter_mode_type_e {
    uncompressed_mode,/*service meter uncompressed mode*/
    compressed_mode, /* service meter compressed mode */
    udf_mode,      /* service meter in udf mode */
    cascade_mode,
    cascade_with_coupling_mode,
    udf_cascade_mode,
    udf_cascade_with_coupling_mode
} bcm_policer_svc_meter_mode_type_t;

/* bcm_policer_svc_meter_mode_t */
typedef uint32 bcm_policer_svc_meter_mode_t;

/* compressed_pri_cnf_attr_map_t */
typedef uint8 compressed_pri_cnf_attr_map_t;

/* compressed_pkt_pri_attr_map_t */
typedef uint8 compressed_pkt_pri_attr_map_t;

/* compressed_port_attr_map_t */
typedef uint8 compressed_port_attr_map_t;

/* compressed_tos_attr_map_t */
typedef uint8 compressed_tos_attr_map_t;

/* compressed_pkt_res_attr_map_t */
typedef uint8 compressed_pkt_res_attr_map_t;

/* offset_table_map_t */
typedef struct offset_table_entry_s {
    uint8 offset;
    uint8 meter_enable;
    uint8 pool;
} offset_table_entry_t;

typedef struct bcm_policer_svc_meter_uncompressed_attr_selectors_s {
    uint32 uncompressed_attr_bits_selector;
    offset_table_entry_t offset_map[BCM_SVC_METER_MAP_SIZE_256];
} uncompressed_attr_selectors_t;

typedef struct bcm_policer_svc_meter_pkt_attr_bits_s {
    uint8 cng;              /* CNG bits - corresponds to 2 bits in uncompressed
                               mode */
    uint8 cng_pos;
    uint8 cng_mask;
    uint8 int_pri;          /* internal priority bits -corresponds to 4 bits in
                               uncompressed mode */
    uint8 int_pri_pos;
    uint8 int_pri_mask;
    uint8 short_int_pri;    /* internal priority bits -corresponds to 3 bits in
                               uncompressed mode */
    uint8 short_int_pri_pos;
    uint8 short_int_pri_mask;
    uint8 vlan_format;      /* vlan format - corresponds to 2 bits in
                               uncompressed mode */
    uint8 vlan_format_pos;
    uint8 vlan_format_mask;
    uint8 outer_dot1p;      /* outer 802.1p - corresponds to 3 bits in
                               uncompressed mode */
    uint8 outer_dot1p_pos;
    uint8 outer_dot1p_mask;
    uint8 inner_dot1p;      /* inner 802.1p - corresponds to 3 bits in
                               uncompressed mode */
    uint8 inner_dot1p_pos;
    uint8 inner_dot1p_mask;
    uint8 ing_port;         /* ingress port - corresponds to 6 bits in
                               uncompressed mode */
    uint8 ing_port_pos;
    uint8 ing_port_mask;
    uint8 tos;              /* tos bits DSCP - corresponds to 6 bits in uncompressed
                               mode */
    uint8 tos_pos;
    uint8 tos_mask;
    uint8 tos_ecn;          /* tos ecn bits - corresponds to 2 bits in uncompressed
                               mode */
    uint8 tos_ecn_pos;
    uint8 tos_ecn_mask;
    uint8 pkt_resolution;   /* packet resolution - corresponds to 6 bits in
                               uncompressed mode */
    uint8 pkt_res_pos;
    uint8 pkt_res_mask;
    uint8 svp_type;         /* svp type - corresponds to 1 bit in uncompressed
                               mode */
    uint8 svp_type_pos;
    uint8 svp_type_mask;
    uint8 drop;             /* use drop bit -corresponds to 1 bit in
                               uncompressed mode */
    uint8 drop_pos;
    uint8 drop_mask;
    uint8 ip_pkt;           /* corresponds to 1 bit in uncompressed mode */
    uint8 ip_pkt_pos;
    uint8 ip_pkt_mask;
    uint8 preemptable_pkt;  /* preemptable_pkt */
    uint32 ethernet_type;   /* ethernet_type */
} pkt_attr_bits_t;

typedef struct bcm_policer_svc_meter_udf_pkt_attr_bits_s {
    uint8 udf0; /* Represents bits to be used from UDF0 in udf mode */
    uint8 udf1; /* Represents bits to be used from UDF1 in udf mode */
} udf_pkt_attr_bits_t;

#if defined(BCM_GLOBAL_METER_V2_SUPPORT)
#define BCMI_POLICER_GLOBAL_METER_ATTR_MASK_ALL     (0xFF)
#define BCMI_POLICER_GLOBAL_METER_INVALID_ACTION_ID (0)

typedef struct pkt_tcam_attr_bits_s {
    /* TCAM SW information */
    uint32 ref_count;
    uint32 mode_id;
    uint32 attr_bits_selector;
    /* TCAM HW information */
    uint32 tcam_idx; /* TCAM index */
    uint8 final_offset; /* final_offset */
    uint8 offset_mode; /* offset_mode */
    udf_pkt_attr_bits_t udf, udf_mask;
    uint32 value[bcmPolicerGroupModeAttrCount];
    uint32 value_mask[bcmPolicerGroupModeAttrCount];
} pkt_tcam_attr_bits_t;

void pkt_tcam_attr_bits_t_init(pkt_tcam_attr_bits_t *attr);

/*
 * Typedef:
 *     bcmi_global_meter_policer_t
 * Purpose:
 *     This is the policer description structure.
 *     Indexed by bcm_policer_t handle.
 */
typedef struct bcmi_global_meter_policer_control_s {
    bcm_policer_t        pid; /* Unique policer identifier */
    uint16               ref_count; /* SW object use reference count */
    bcm_policer_config_t *pol_cfg; /* policer config of each policer */
    uint32               no_of_policers;
    bcm_policer_group_mode_t grp_mode;
    struct bcmi_global_meter_policer_control_s *next; /* lookup linked list */
} bcmi_global_meter_policer_control_t;

/* Structure to store which attributes are used by user (for complete list) */
typedef struct bcmi_policer_tcam_attr_selectors_s {
    uint32 total_policers;
    uint32 pkt_attr_v_cnt; /* Number of multiple elements specified
                                           in all of the pkt_attr_v[] array */
    pkt_tcam_attr_bits_t *pkt_attr_v; /* TCAM information */
} bcmi_policer_tcam_attr_selectors_t;

typedef struct bcmi_policer_svm_offset_mode_bk_s {
    uint8 is_custom_group; /* TRUE : created by policer_group_create
                              FALSE : created by custom_group_mode_id_create */
    uint32 ref_cnt;
    uint32 used;
    bcm_policer_group_mode_t group_mode;
    bcm_policer_group_mode_type_t group_mode_type;
    uint32 no_of_policers;
    uint32 no_of_attr_selectors;
    bcm_policer_group_mode_attr_selector_t *attr_selectors;
    uint32 pkt_attr_v_cnt; /* Number of multiple elements specified
                                           in all of the pkt_attr_v[] array */
    pkt_tcam_attr_bits_t *pkt_attr_v; /* TCAM information */
} bcmi_policer_svm_offset_mode_bk_t;

typedef struct bcmi_policer_svm_bk_s {
    /* Store TCAM information */
    SHR_BITDCL  *tcam_intf_bitmap;
    uint32      valid_tcam_idx_cnt;
    uint32      valid_tcam_idx_start;
    uint32      valid_tcam_idx_end;
    uint32      empty_tcam_idx_cnt;
    uint32      empty_tcam_idx_start;
    uint32      empty_tcam_idx_end;
    /* Store Offset Mode information */
    bcmi_policer_svm_offset_mode_bk_t *offset_mode_v;
    SHR_BITDCL  *offset_mode_intf_bitmap;
    uint32      valid_offset_mode_cnt;
    uint32      valid_offset_mode_start;
    uint32      valid_offset_mode_end;
    uint32      empty_offset_mode_cnt;
    uint32      empty_offset_mode_start;
    uint32      empty_offset_mode_end;
    sal_mutex_t mutex;
} bcmi_policer_svm_bk_t;

typedef struct bcmi_policer_global_meter_action_bk_s {
    uint32                          used;
    uint32                          reference_count;
    bcm_policer_action_t            actions[bcmPolicerActionCount];
    uint32                          param[bcmPolicerActionCount];
    uint32                          actions_count;
} bcmi_policer_global_meter_action_bk_t;

#endif /* BCM_GLOBAL_METER_V2_SUPPORT */

typedef enum bcm_policer_attr_with_value_e {
    _bcmPolicerAttrPort = 0,
    _bcmPolicerAttrTosDscp = 1,
    _bcmPolicerAttrTosEcn = 2,
    _bcmPolicerAttrSvpType = 3,
    _bcmPolicerAttrMax = 4
} bcm_policer_attr_with_value_t;

/* Out of above attr_with_value, ing_port can have highest value
 * Taking Maximum bits required as 180 even though port is 169 (in Kt2) */
#define _BCM_POLICER_MAX_ATTR_VALUE_BITS 180
#define _BCM_POLICER_BIT_ARRAY_SIZE _SHR_BITDCLSIZE(_BCM_POLICER_MAX_ATTR_VALUE_BITS)
#define _BCM_POLICER_VALUE_SET(_array, _value) SHR_BITSET(_array, _value)
#define _BCM_POLICER_VALUE_GET(_array, _value) SHR_BITGET(_array, _value)

typedef struct bcm_policer_combine_attr_s {
#define _BCM_POLICER_COLOR_GREEN                     0x00000001
#define _BCM_POLICER_COLOR_YELLOW                    0x00000002
#define _BCM_POLICER_COLOR_RED                       0x00000004
    uint32 cng_flags;

    /* set corresponding bit-position for Prio */
#define _BCM_POLICER_PKT_PRI0                        0
#define _BCM_POLICER_PKT_PRI7                        7
#define _BCM_POLICER_PKT_PRI15                      15
    uint32 int_pri_flags;

#define _BCM_POLICER_VLAN_FORMAT_UNTAGGED            0x00000001
#define _BCM_POLICER_VLAN_FORMAT_INNER               0x00000002
#define _BCM_POLICER_VLAN_FORMAT_OUTER               0x00000004
#define _BCM_POLICER_VLAN_FORMAT_BOTH                0x00000008
    uint32 vlan_format_flags;

    /* set corresponding bit-position for Prio */
    uint32 outer_dot1p_flags;
    uint32 inner_dot1p_flags;

#define _BCM_POLICER_PKT_TYPE_UNKNOWN_PKT                    0x00000001
#define _BCM_POLICER_PKT_TYPE_CONTROL_PKT                    0x00000002
#define _BCM_POLICER_PKT_TYPE_OAM_PKT                        0x00000004
#define _BCM_POLICER_PKT_TYPE_BFD_PKT                        0x00000008
#define _BCM_POLICER_PKT_TYPE_BPDU_PKT                       0x00000010
#define _BCM_POLICER_PKT_TYPE_ICNM_PKT                       0x00000020
#define _BCM_POLICER_PKT_TYPE_PKT_IS_1588                    0x00000040
#define _BCM_POLICER_PKT_TYPE_KNOWN_L2UC_PKT                 0x00000080
#define _BCM_POLICER_PKT_TYPE_UNKNOWN_L2UC_PKT               0x00000100
#define _BCM_POLICER_PKT_TYPE_L2BC_PKT                       0x00000200
#define _BCM_POLICER_PKT_TYPE_KNOWN_L2MC_PKT                 0x00000400
#define _BCM_POLICER_PKT_TYPE_UNKNOWN_L2MC_PKT               0x00000800
#define _BCM_POLICER_PKT_TYPE_KNOWN_L3UC_PKT                 0x00001000
#define _BCM_POLICER_PKT_TYPE_UNKNOWN_L3UC_PKT               0x00002000
#define _BCM_POLICER_PKT_TYPE_KNOWN_IPMC_PKT                 0x00004000
#define _BCM_POLICER_PKT_TYPE_UNKNOWN_IPMC_PKT               0x00008000
#define _BCM_POLICER_PKT_TYPE_KNOWN_MPLS_L2_PKT              0x00010000
#define _BCM_POLICER_PKT_TYPE_KNOWN_MPLS_L3_PKT              0x00020000
#define _BCM_POLICER_PKT_TYPE_KNOWN_MPLS_PKT                 0x00040000
#define _BCM_POLICER_PKT_TYPE_UNKNOWN_MPLS_PKT               0x00080000
#define _BCM_POLICER_PKT_TYPE_KNOWN_MPLS_MULTICAST_PKT       0x00100000
#define _BCM_POLICER_PKT_TYPE_KNOWN_MIM_PKT                  0x00200000
#define _BCM_POLICER_PKT_TYPE_UNKNOWN_MIM_PKT                0x00400000
#define _BCM_POLICER_PKT_TYPE_KNOWN_TRILL_PKT                0x00800000
#define _BCM_POLICER_PKT_TYPE_UNKNOWN_TRILL_PKT              0x01000000
#define _BCM_POLICER_PKT_TYPE_KNOWN_NIV_PKT                  0x02000000
#define _BCM_POLICER_PKT_TYPE_UNKNOWN_NIV_PKT                0x04000000
    uint32 pkt_resolution_flags;

#define _BCM_POLICER_DROP_ENABLE                             0x00000001
#define _BCM_POLICER_DROP_DISABLE                            0x00000002
    uint32 drop_flags;

#define _BCM_POLICER_IP_PKT_ENABLE                           0x00000001
#define _BCM_POLICER_IP_PKT_DISABLE                          0x00000002
    uint32 ip_pkt_flags ;

#define _BCM_POLICER_PREMMPTABLE_PKT_ENABLE                  0x00000001
#define _BCM_POLICER_PREMMPTABLE_PKT_DISABLE                 0x00000002
    uint32 preemptable_pkt_flags;

    uint32 ethertype_flags;
    /* Array to store values for type which have values associated with */
    SHR_BITDCL value_array[_bcmPolicerAttrMax][_BCM_POLICER_BIT_ARRAY_SIZE];
} bcm_policer_combine_attr_t;

/* Structure to store which attributes are used by user (for complete list) */
typedef struct bcm_policer_attr_selectors_s {
    uint32 cng;
    uint32 combine_cng_flags;
    uint32 int_pri;
    uint32 combine_int_pri_flags;
    uint32 vlan_format;
    uint32 combine_vlan_format_flags;
    uint32 outer_dot1p;
    uint32 combine_outer_dot1p_flags;
    uint32 inner_dot1p;
    uint32 combine_inner_dot1p_flags;
    uint32 port;
    uint32 tos_dscp;
    uint32 tos_ecn;
    uint32 pkt_resolution;
    uint32 combine_pkt_res_flags;
    uint32 svp_type;
    uint32 drop;
    uint32 combine_drop_flags;
    uint32 ip_pkt;
    uint32 preemptable_pkt;
    uint32 ethertype;
    uint32 combine_ip_pkt_flags;
    uint32 total_policers;
    uint32 total_selectors;
    SHR_BITDCL combine_value_array[_bcmPolicerAttrMax][_BCM_POLICER_BIT_ARRAY_SIZE];
    bcm_policer_combine_attr_t  *combine_attr_data;
} bcm_policer_attr_selectors_t;

/* Number of compression attribute maps. */
#define _BCM_POLICER_PKT_COMPRESSED_ATTR_MAPS     5

/* Structure for global compression attribute table status. */
typedef struct _bcm_policer_pkt_compressed_attr_maps_s {
    uint8 ref_counts[_BCM_POLICER_PKT_COMPRESSED_ATTR_MAPS];
    uint32 cng;
    uint32 combine_cng_flags;
    uint32 int_pri;
    uint32 combine_int_pri_flags;
    uint32 vlan_format;
    uint32 combine_vlan_format_flags;
    uint32 outer_dot1p;
    uint32 combine_outer_dot1p_flags;
    uint32 inner_dot1p;
    uint32 combine_inner_dot1p_flags;
    uint32 port;
    uint32 tos_dscp;
    uint32 tos_ecn;
    uint32 pkt_resolution;
    uint32 combine_pkt_res_flags;
    uint32 svp_type;
    uint32 drop;
    uint32 combine_drop_flags;
    uint32 ip_pkt;
    uint32 combine_ip_pkt_flags;
    SHR_BITDCL combine_value_array[_bcmPolicerAttrMax][_BCM_POLICER_BIT_ARRAY_SIZE];
} _bcm_policer_pkt_compressed_attr_maps_t;

typedef struct bcm_policer_svc_meter_udf_sub_div_s {
    int udf_id;
    uint32 offset;
    uint32 width;
} bcm_policer_udf_sub_div_t;

typedef struct bcm_policer_svc_meter_udf_pkt_combine_attr_t {
    uint8 drop_flags;
    bcm_policer_udf_sub_div_t udf_subdiv[BCM_POLICER_SVC_METER_MAX_OFFSET];
    uint32 attr_value[BCM_POLICER_SVC_METER_MAX_OFFSET];
    uint8 selectors;
} bcm_policer_udf_pkt_combine_attr_t;

typedef struct bcm_policer_svc_meter_udf_attr_selectors_s {
    uint8 drop;
    bcm_policer_udf_sub_div_t udf_subdiv[SVC_METER_UDF_MAX_SUB_DIVISIONS];
    int total_subdiv;
    uint8 udf_bits[SVC_METER_UDF1_MAX_BIT_POSITION];
    bcm_policer_udf_pkt_combine_attr_t *combine_attr_data;
} bcm_policer_udf_attr_selectors_t;

typedef struct bcm_policer_svc_meter_compressed_attr_selectors_s {
    pkt_attr_bits_t pkt_attr_bits_v;
    compressed_pri_cnf_attr_map_t
         compressed_pri_cnf_attr_map_v[BCM_SVC_METER_MAP_SIZE_64];
    compressed_pkt_pri_attr_map_t
         compressed_pkt_pri_attr_map_v[BCM_SVC_METER_MAP_SIZE_256];
    compressed_port_attr_map_t
         compressed_port_attr_map_v[BCM_SVC_METER_PORT_MAP_SIZE];
    compressed_tos_attr_map_t
         compressed_tos_attr_map_v[BCM_SVC_METER_MAP_SIZE_256];
    compressed_pkt_res_attr_map_t
         compressed_pkt_res_attr_map_v[BCM_SVC_METER_MAP_SIZE_1024];
    offset_table_entry_t offset_map[BCM_SVC_METER_MAP_SIZE_256];
} compressed_attr_selectors_t;

typedef struct bcm_policer_svc_meter_udf_pkt_attr_selectors_s {
    udf_pkt_attr_bits_t udf_pkt_attr_bits_v;
    uint32 drop;
    bcm_policer_udf_sub_div_t udf_subdiv[SVC_METER_UDF_MAX_SUB_DIVISIONS];
    uint32 total_subdiv;
    uint32 num_selectors;
    int udf_id;
    offset_table_entry_t offset_map[BCM_SVC_METER_MAP_SIZE_256];
} udf_pkt_attr_selectors_t;

typedef struct bcm_policer_svc_meter_attr_s {
    bcm_policer_svc_meter_mode_type_t mode_type_v;
    /* Valid only for uncompressed */
    uncompressed_attr_selectors_t uncompressed_attr_selectors_v;
    /* valid only for compressed */
    compressed_attr_selectors_t compressed_attr_selectors_v;
    /* valid only for udf */
    udf_pkt_attr_selectors_t udf_pkt_attr_selectors_v;
} bcm_policer_svc_meter_attr_t;

/* Policer Flow types
 * A category is needed to know that the policers allocated
 * in the group are micro/macro/other policers */
typedef enum _bcm_policer_flow_type_e {
    bcmPolicerFlowTypeNormal = 0,       /* meter is other than MACRO or MICRO */
    bcmPolicerFlowTypeMicro = 1,        /* meter is MICRO */
    bcmPolicerFlowTypeMacro = 2         /* meter is MACRO */
} _bcm_policer_flow_type_t;

typedef struct _bcm_policer_flow_info_s {
    int mode_id;
    _bcm_policer_flow_type_t flow_type;
    int req_alloc;
    int skip_pool;
    int skip_bank;
    int req_pool;
    int req_base_offset;
} _bcm_policer_flow_info_t;

/* Below data structure is used for KT2/TR3/FB4 */
typedef struct bcm_policer_group_mode_attr_selector_wb_s {
    uint32 flags;                       /* packet attribute selector flags */
    uint32 policer_offset;              /* Policer Offset */
    bcm_policer_group_mode_attr_t attr; /* Attribute Selector */
    uint32 attr_value;                  /* Attribute Values */
    int udf_id;                         /* UDF ID created using bcm_udf_create.
                                           Applicable only if Group mode
                                           attribute is
                                           bcmPolicerGroupModeAttrUdf. */
    uint32 offset;                      /* Offset in bits from the beginning of
                                           the packet attribute */
    uint32 width;                       /* Number of bits in packet attribute
                                           from offset to be used for policer
                                           offset mapping */
} bcm_policer_group_mode_attr_selector_wb_t;

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Below data structure is used to sync/recover data to/from scache. So,
 * no new fields can be added to this structure directly.
 * bcm_policer_group_mode_attr_selector_wb_t is used for KT2/TR3/FB4.
 */
typedef struct bcm_policer_svc_meter_group_mode_attr_selectors_info_s {
    uint32 flags;
    uint32 num_selectors;
    bcm_policer_group_mode_attr_selector_wb_t
                attr_selectors[BCM_POLICER_SVC_METER_MAX_ATTR_SELECTORS];
} bcm_policer_group_mode_attr_selectors_info_t;
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Create a svc meter mode entry. */
extern int _bcm_policer_svc_meter_create_mode(
    int unit,
    bcm_policer_svc_meter_attr_t meter_attr,
    bcm_policer_svc_meter_mode_t *bcm_policer_svc_meter_mode_v);

/* Delete a svc meter mode entry. */
extern int _bcm_policer_svc_meter_delete_mode(
    int unit,
    bcm_policer_svc_meter_mode_t bcm_policer_svc_meter_mode_v);


typedef struct  bcm_policer_svc_meter_bookkeep_mode_s {
    uint32                          used;
    uint32                          reference_count;
    bcm_policer_svc_meter_attr_t    meter_attr;
    uint32                          no_of_policers;
    bcm_policer_group_mode_t        group_mode;
    int8                            type;
    uint32                          no_of_selectors;
    bcm_policer_group_mode_attr_selector_wb_t *attr_selectors;
} bcm_policer_svc_meter_bookkeep_mode_t;

typedef struct _bcm_policer_pkt_attr_bit_pos_s {
    int start_bit;
    int end_bit;
} _bcm_policer_pkt_attr_bit_pos_t;

typedef enum _bcm_policer_svm_source_type_s {
    _BCM_SVM_MEM_PORT = 0,
    _BCM_SVM_MEM_VLAN = 1,
    _BCM_SVM_MEM_VFI = 2,
    _BCM_SVM_MEM_SOURCE_VP = 3,
    _BCM_SVM_MEM_VLAN_XLATE = 4,
    _BCM_SVM_MEM_VFP_POLICY_TABLE = 5,
    _BCM_SVM_MEM_COUNT
} _bcm_policer_svm_source_type_t;

typedef struct {
    soc_mem_t table;
    soc_field_t offset_mode;
    soc_field_t meter_index;
} _bcm_policer_svm_source_t;

/* Structure contains parameters which varies among different devices */
typedef struct _bcm_policer_svm_dev_attr_s {
    uint8               banks_per_pool;
    uint8               uncompressed_ing_port_size;
    uint8               uncompressed_svp_type_size;
    uint8               compressed_int_pri_bit_pos;
    uint8              *pkt_resolution;
    _bcm_policer_pkt_attr_bit_pos_t *uncompressed_pkt_attr_bit_pos;
    _bcm_policer_pkt_attr_bit_pos_t *compressed_pkt_attr_bit_pos;
    _bcm_policer_pkt_attr_bit_pos_t *compressed_attr_map_bit_pos;
} _bcm_policer_svm_dev_attr_t;

typedef struct _bcm_policer_global_meter_control_s {
    _bcm_policer_svm_dev_attr_t svm_dev_attr_info;
    _bcm_policer_svm_source_t   svm_source[_BCM_SVM_MEM_COUNT];
} _bcm_policer_global_meter_control_t;

bcm_error_t _bcm_policer_svm_source_index_get(
    int unit,
    soc_mem_t table,
    _bcm_policer_svm_source_type_t *mem_index
);

bcm_error_t _bcm_policer_svc_meter_get_available_mode( uint32 unit,
    bcm_policer_svc_meter_mode_t        *svc_meter_mode
);

bcm_error_t _bcm_policer_svc_meter_update_selector_keys_enable_fields(
    int         unit,
    soc_reg_t   bcm_policer_svc_meter_pkt_attr_selector_key_reg,
    uint64      *bcm_policer_svc_meter_pkt_attr_selector_key_reg_value,
    uint32      bcm_policer_svc_meter_pkt_attr_bit_position,
    uint32      bcm_policer_svc_meter_pkt_attr_total_bits,
    uint8       bcm_policer_svc_meter_pkt_attr_bit_mask,
    uint8       *bcm_policer_svc_meter_current_bit_selector_position
);

bcm_error_t _bcm_policer_svc_meter_update_offset_table(
    int         unit,
    soc_mem_t   bcm_policer_svc_meter_offset_table_mem,
    bcm_policer_svc_meter_mode_t     bcm_policer_svc_meter_mode_v,
    offset_table_entry_t *offset_map
);

bcm_error_t _bcm_policer_svc_meter_reserve_mode(
    uint32                         unit,
    bcm_policer_svc_meter_mode_t   bcm_policer_svc_meter_mode_v,
    bcm_policer_group_mode_t       group_mode,
    bcm_policer_svc_meter_attr_t   *meter_attr
);

bcm_error_t _bcm_policer_svc_meter_unreserve_mode(
    uint32                            unit,
    bcm_policer_svc_meter_mode_t   bcm_policer_svc_meter_mode_v
);

bcm_error_t bcm_policer_svc_meter_inc_mode_reference_count(
    uint32                       unit,
    bcm_policer_svc_meter_mode_t bcm_policer_svc_meter_mode_v
);

bcm_error_t bcm_policer_svc_meter_dec_mode_reference_count(
    uint32                         unit,
    bcm_policer_svc_meter_mode_t   bcm_policer_svc_meter_mode_v
);

bcm_error_t _bcm_policer_svc_meter_update_udf_selector_keys(
    int                       unit,
    soc_reg_t                 bcm_policer_svc_meter_pkt_attr_selector_key_reg,
    udf_pkt_attr_selectors_t  *udf_pkt_attr_selectors_v,
    uint32                    *total_udf_bits
);


bcm_error_t _bcm_policer_svc_meter_get_mode_info(
                int                                     unit,
                bcm_policer_svc_meter_mode_t            meter_mode_v,
                bcm_policer_svc_meter_bookkeep_mode_t   *mode_info
                );

/* policer API's */

typedef struct  bcm_policer_global_meter_action_bookkeep_s {
    uint32                          used;
    uint32                          reference_count;
} bcm_policer_global_meter_action_bookkeep_t;

typedef struct  bcm_policer_global_meter_init_staus_s {
    uint32                          initialised;
} bcm_policer_global_meter_init_status_t;

int bcm_esw_global_meter_init(int unit);
int _bcm_esw_global_meter_reinit(int unit);

int _bcm_esw_global_meter_offset_mode_reinit(int unit);
int _bcm_esw_global_meter_compressed_offset_mode_reinit(
                                 int unit,
                                 int mode,
                                 uint32 selector_count,
                                 uint32 selector_for_bit_x_en_field_value[8],
                                 uint32 selector_for_bit_x_field_value[8]);
int _bcm_esw_global_meter_uncompressed_offset_mode_reinit(
                                 int unit,
                                 int mode,
                                 uint32 selector_count,
                                 uint32 selector_for_bit_x_en_field_value[8],
                                 uint32 selector_for_bit_x_field_value[8]);
int _bcm_esw_global_meter_udf_offset_mode_reinit(
                                 int unit,
                                 int mode,
                                 uint32 selector_count,
                                 uint32 selector_for_bit_x_en_field_value[8],
                                 uint32 selector_for_bit_x_field_value[8]);

int _bcm_esw_global_meter_action_reinit(int unit);
int _bcm_esw_is_global_policer(
    int unit,
    bcm_policer_t policer,
    int *is_global_policer);
int _bcm_esw_policer_validate(int unit, bcm_policer_t *policer);
int _bcm_esw_policer_decrement_ref_count(int unit, bcm_policer_t policer);
int _bcm_esw_policer_increment_ref_count(int unit, bcm_policer_t policer);
int _bcm_esw_global_meter_cleanup(int unit);
int _bcm_esw_add_policer_to_table(int unit,bcm_policer_t policer,
                                       int table, int index, void *data);
int _bcm_esw_delete_policer_from_table(int unit,bcm_policer_t policer,
                                 soc_mem_t table, int index, void *data);
int _bcm_esw_get_policer_from_table(int unit,soc_mem_t table,int index,
                                      void *data,
                                  bcm_policer_t *policer, int skip_read);
int _bcm_esw_policer_offset_mode_get(
    int unit,
    bcm_policer_t policer_id,
    int *offset_mode);


#define MAX_POLICER_OFFSETS 8
/*
 * Typedef:
 *     _global_meter_policer_t
 * Purpose:
 *     This is the policer description structure.
 *     Indexed by bcm_policer_t handle.
 */
typedef struct _global_meter_policer_s {
    bcm_policer_t        pid;         /* Unique policer identifier.       */
    uint16               ref_count;/* SW object use reference count.   */
    uint32               action_id; /* Index to action table */
    uint32               direction;
    uint32               no_of_policers;
    uint8                offset[MAX_POLICER_OFFSETS];
    bcm_policer_group_mode_t grp_mode;
    struct _global_meter_policer_s *next;    /* Policer lookup linked list */
}_global_meter_policer_control_t;

/*
 * Typedef:
 *     _global_meter_horizontal_alloc_t
 * Purpose:
 *     This is the policer description structure.
 *     Indexed by bcm_policer_t handle.
 */
typedef struct _global_meter_horizontal_alloc_s {
    uint8  alloc_bit_map;/*Meter allocation bit map across pools for an index */
    uint8  no_of_groups_allocated;/* SW object use reference count.   */
    uint8  first_bit_to_use;  /* first bit position from where new
                                             block can be allocated */
    uint8  last_bit_to_use;  /* last pool till which
                                             block can be allocated */
}_global_meter_horizontal_alloc_t;

#if defined(BCM_GLOBAL_METER_V2_SUPPORT)
/*
 * Global Meter's platform specific information per service
 * This structure maintains chip specific information
 */
typedef struct bcmi_global_meter_dev_info_s {
    char *table_name;
    int (*action_supported)(
        int unit,
        bcm_policer_action_t action);
    int (*group_mode_supported)(
        int unit,
        bcm_policer_group_mode_t group_mode);
    int (*group_mode_type_supported)(
        int unit,
        bcm_policer_group_mode_type_t group_mode_type);
    int (*group_mode_attr_supported)(
        int unit,
        bcm_policer_group_mode_attr_t group_mode_attr);
    int (*group_mode_attr_validate)(
        int unit,
        bcm_policer_group_mode_attr_t group_mode_attr,
        uint32 value);
    int (*mem_clear)(
        int unit);
    int (*mem_range_clear)(
        int unit,
        int base_index,
        uint32 numbers);
    int (*ifg_set)(
        int unit,
        uint8 ifg_enable);
    int (*ifg_get)(
        int unit,
        uint8 *ifg_enable);
    int (*source_order_set)(
        int unit,
        bcm_policer_global_meter_source_t *source_order,
        uint32 source_order_count);
    int (*source_order_get)(
        int unit,
        uint32 source_order_count,
        bcm_policer_global_meter_source_t *source_order);
    int (*svm_enable_set)(
        int unit,
        uint8 enable);
    int (*svm_enable_get)(
        int unit,
        uint8 *enabled);
    int (*seop_enable_set)(
        int unit,
        uint8 enable);
    int (*seop_enable_get)(
        int unit,
        uint8 *enabled);
    int (*tcam_keys_set)(
        int unit,
        pkt_tcam_attr_bits_t *pkt_attr_bits_v);
    int (*tcam_keys_get)(
        int unit,
        pkt_tcam_attr_bits_t *pkt_attr_bits_v);
    int (*tcam_keys_delete)(
        int unit,
        uint32 sw_idx);
    int (*policer_to_table_set)(
        int unit,
        soc_mem_t mem,
        uint32 mem_index,
        void *mem_data,
        int meter_index,
        int meter_offset_mode,
        int skip_read);
    int (*policer_to_table_get)(
        int unit,
        soc_mem_t mem,
        uint32 mem_index,
        void *mem_data,
        int *meter_index,
        int *meter_offset_mode,
        int skip_read);
    int (*meter_table_bitsize_get)(
        int unit,
        uint32 sw_idx,
        uint32 *refresh_bitsize,
        uint32 *bucket_max_bitsize,
        uint32 *bucket_cnt_bitsize);
    int (*meter_table_set)(
        int unit,
        uint32 sw_idx,
        bcm_policer_mode_t meter_mode,
        uint32 meter_flags,
        uint32 average_pkt_size,
        uint32 meter_gran_mode,
        uint32 bucket_size,
        uint32 bucket_count,
        uint32 refresh_rate,
        uint32 granularity);
    int (*meter_table_get)(
        int unit,
        uint32 sw_idx,
        uint32 *bucket_size,
        uint32 *bucket_count,
        uint32 *refresh_rate,
        uint32 *granularity,
        uint32 *pkt_bytes,
        uint32 *tsn_modified,
        uint32 *meter_gran_mode,
        uint32 *average_pkt_size);
    int (*policy_mode_set)(
        int unit,
        uint32 sw_idx,
        bcm_policer_mode_t meter_mode,
        uint32 meter_flags);
    int (*policy_mode_get)(
        int unit,
        uint32 sw_idx,
        bcm_policer_mode_t *meter_mode,
        uint32 *meter_flags);
    int (*policy_action_param_configure)(
        int unit,
        bcm_policer_action_t action,
        uint32 param0,
        uint64 *green_action,
        uint64 *yellow_action,
        uint64 *red_action);
    int (*policy_action_set)(
        int unit,
        uint32 sw_idx,
        uint64 green_action,
        uint64 yellow_action,
        uint64 red_action);
    int (*policy_action_get)(
       int unit,
        uint32 sw_idx,
        uint64 *green_action,
        uint64 *yellow_action,
        uint64 *red_action);
} bcmi_global_meter_dev_info_t;
#endif /* BCM_GLOBAL_METER_V2_SUPPORT */

int _bcm_global_meter_policer_get(int unit, bcm_policer_t pid,
                                 _global_meter_policer_control_t **policer_p);


int _bcm_gloabl_meter_reserve_bloc_horizontally(int unit, int pool_id,
                                                          bcm_policer_t pid);
int _bcm_gloabl_meter_unreserve_bloc_horizontally(int unit, int pool_id,
                                                          bcm_policer_t pid);
int _bcm_global_meter_reserve_bloc_horizontally(int unit, int pool_id,
                                                          bcm_policer_t pid);

int _bcm_esw_global_meter_policer_sync(int unit);
int _bcm_esw_global_meter_policer_get(int unit, bcm_policer_t policer_id,
                                                bcm_policer_config_t *pol_cfg);
int _bcm_esw_global_meter_policer_destroy(int unit, bcm_policer_t policer_id);
int _bcm_esw_global_meter_policer_destroy_all(int unit);
int _bcm_esw_global_meter_policer_destroy2(int unit,
               _global_meter_policer_control_t *policer_control);
int _bcm_esw_global_meter_policer_set(int unit, bcm_policer_t policer_id,
                   bcm_policer_config_t *pol_cfg);
int  _bcm_esw_global_meter_policer_traverse(int unit,
                                bcm_policer_traverse_cb cb,  void *user_data);

int  _bcm_esw_policer_svc_meter_delete_mode( int unit,
                    bcm_policer_svc_meter_mode_t bcm_policer_svc_meter_mode_v);
int _bcm_global_meter_get_coupled_cascade_policer_index(int unit,
                                          bcm_policer_t policer_id,
                  _global_meter_policer_control_t *policer_control,
                   int *new_index);
int _bcm_global_meter_base_policer_get(int unit, bcm_policer_t pid,
                         _global_meter_policer_control_t **policer_p);
int _bcm_esw_get_policer_id_from_index_offset(int unit, int index,
                                              int offset_mode,
                                              bcm_policer_t *policer_id);
int _bcm_esw_reset_policer_from_table(int unit, bcm_policer_t policer,
                                      int table, int index, void *data);
int _bcm_esw_policer_group_create(int unit, bcm_policer_group_mode_t mode,
                                   _bcm_policer_flow_info_t *flow_info,
                                   bcm_policer_map_t *offset_map,
                                   bcm_policer_t *policer_id, int *npolicers);
int _bcm_esw_get_policer_table_index(int unit, bcm_policer_t policer,
                                                         int *index);
int _check_global_meter_init(int unit);
#endif	/* !_BCM_INT_SVC_METER_H */
