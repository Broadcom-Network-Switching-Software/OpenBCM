/*! \file field_int.h
 *
 * Field internal management header file.
 * This file contains the management for FIELD module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_FIELD_INT_H
#define BCMINT_LTSW_FIELD_INT_H

#include <bcm/field.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/field_ha_int.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Defines
 */

/* ! Group Info Max value for all array fields */
#define BCMINT_INFO_MAX (16)

/* ! Invalid field ID */
#define BCMINT_FIELD_ID_INVALID  (-1)

/* ! MAX EFP KEY combinations */
#define BCMINT_FIELD_EFP_KEYS  (14)

/* ! GROUP PRIO ANY Value */
#define BCMINT_FIELD_GROUP_PRIO_ANY  (0x7FFFFFF)

/* ! Max pipe */
#define BCMINT_FIELD_MAX_PIPE (4)

/*!
 * \brief return max uint32 value based on width in bits
 */
#define BCMINT_FIELD_MAX_WIDTH_VALUE(width)  \
    ((width < 32) ?                          \
    ((1 << width) - 1) :                     \
    (0xFFFFFFFFUL))

/*! BCMINT_FP_ALLOC UTILITY */
#define BCMINT_FIELD_MEM_ALLOC(_ptr, _sz, _str)   \
    do {                                          \
        SHR_ALLOC(_ptr, _sz, _str);               \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY);       \
        sal_memset(_ptr, 0, _sz);                 \
    } while(0)

#define BCMINT_FIELD_MEM_FREE(_ptr)            \
    do {                                       \
        SHR_FREE(_ptr);                        \
    } while(0)

#define BCMINT_FIELD_UINT16_BIT_SET(_arr, _bit) \
    do {                                        \
        _arr[(_bit / 16)] |= (1 << (_bit % 16));\
    } while (0)

#define BCMINT_FIELD_UINT16_BIT_GET(_arr, _bit) \
    ((_arr[(_bit / 16)] >> (_bit % 16)) & 1)

#define BCMINT_FIELD_ACTION_ARR_TO_ASET(_a_cnt, _a_arr, _aset)     \
     {                                                             \
         bcm_field_action_t _a;                                    \
         for (_a = 0; _a < _a_cnt; _a++) {                         \
             BCM_FIELD_ASET_ADD(_aset, _a_arr[_a]);                \
         }                                                         \
     }

#define BCMINT_FIELD_ASET_TO_ACTION_ARR(_aset, _a_cnt, _a_arr)     \
     {                                                             \
         uint16_t _ct = 0;                                         \
         bcm_field_action_t _a;                                    \
         for (_a = 0; _a < bcmFieldActionCount; _a++) {            \
             if (BCM_FIELD_ASET_TEST(_aset, _a)) {                 \
                 _a_arr[_ct++] = _a;                               \
             }                                                     \
         }                                                         \
         _a_cnt = _ct;                                             \
     }

#define BCMINT_FIELD_QUAL_ARR_TO_QSET(_q_cnt, _q_arr, _qset)       \
     {                                                             \
         bcm_field_qualify_t _q;                                 \
         for (_q = 0; _q < _q_cnt; _q++) {                         \
             BCM_FIELD_QSET_ADD(_qset, _q_arr[_q]);                \
         }                                                         \
     }


/*! Stage Flags */
#define BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL            (1 << 1)
#define BCMINT_FIELD_STAGE_KEY_TYPE_SELCODE           (1 << 2)
#define BCMINT_FIELD_STAGE_KEY_TYPE_FIXED             (1 << 3)
#define BCMINT_FIELD_STAGE_POLICY_TYPE_PDD            (1 << 4)
#define BCMINT_FIELD_STAGE_KEYGEN_PROFILE             (1 << 5)
#define BCMINT_FIELD_STAGE_ENTRY_TYPE_HASH            (1 << 6)
#define BCMINT_FIELD_STAGE_DEFAULT_POLICY             (1 << 6)
#define BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR      (1 << 7)
#define BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC         (1 << 8)
#define BCMINT_FIELD_STAGE_NO_METER_SUPPORT           (1 << 9)
#define BCMINT_FIELD_STAGE_LEGACY_CTR_SUPPORT         (1 << 10)
#define BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT           (1 << 11)
#define BCMINT_FIELD_STAGE_SUPPORTS_COLORED_ACTIONS   (1 << 12)
#define BCMINT_FIELD_STAGE_COMPRESSION_SUPPORTED      (1 << 13)
#define BCMINT_FIELD_STAGE_POLICER_SUPPORT            (1 << 14)
#define BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT     (1 << 15)
#define BCMINT_FIELD_STAGE_DEFAULT_ENTRY_SUPPORT      (1 << 16)
#define BCMINT_FIELD_STAGE_LEGACY_POLICER_SUPPORT     (1 << 17)
#define BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY   (1 << 18)
#define BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY   (1 << 19)
#define BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY   (1 << 20)
#define BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY   (1 << 21)
#define BCMINT_FIELD_STAGE_PIPE_MODE_NOT_SUPPORTED    (1 << 22)
/* stage supports only a single slice (like stage EM) */
#define BCMINT_FIELD_STAGE_SINGLE_SLICE_ONLY          (1 << 23)
#define BCMINT_FIELD_STAGE_PROFILE_SUPPORT            (1 << 24)
#define BCMINT_FIELD_STAGE_POLICER_POOL_HINT_SUPPORT  (1 << 25)
#define BCMINT_FIELD_STAGE_FLEXCTR_CONT_OBJ_SUPPORT   (1 << 26)
/* Flex stats needs atleast one entry installed in group. */
#define BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO        (1 << 27)

/*! Group Flags */
#define BCMINT_FIELD_GROUP_INSTALLED                  (1 << 1)
#define BCMINT_FIELD_GROUP_DISABLED                   (1 << 2)
#define BCMINT_FIELD_GROUP_CREATED_WITH_PRESEL        (1 << 3)
#define BCMINT_FIELD_GROUP_COMPRESSION_ENABLED        (1 << 4)
#define BCMINT_FIELD_GROUP_CREATE_WITH_RESERVE_ID     (1 << 5)
#define BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE     (1 << 6)
#define BCMINT_FIELD_GROUP_COMPR_SRC_IP_ENABLED       (1 << 7)
#define BCMINT_FIELD_GROUP_COMPR_SRC_IP6_ENABLED      (1 << 8)
#define BCMINT_FIELD_GROUP_COMPR_DST_IP_ENABLED       (1 << 9)
#define BCMINT_FIELD_GROUP_COMPR_DST_IP6_ENABLED      (1 << 10)

/* Total no of reserved Group Ids.
 * The reserved group IDs are allocated as the higher group IDs
 * for each stage.
 */
#define BCMINT_FIELD_GROUP_RESERVE_GID_COUNT          1

/*! Entry Flags */
#define BCMINT_FIELD_ENTRY_INSTALLED                   (1 << 1)
#define BCMINT_FIELD_ENTRY_DISABLED                    (1 << 2)
#define BCMINT_FIELD_ENTRY_SRC_PORT_TYPE_GPORT         (1 << 3)
#define BCMINT_FIELD_ENTRY_DST_PORT_TYPE_GPORT         (1 << 4)
#define BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT     (1 << 5)
#define BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT    (1 << 6)
#define BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT     (1 << 7)
#define BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT    (1 << 8)
#define BCMINT_FIELD_ENTRY_DEFAULT_ONLY                (1 << 9)
#define BCMINT_FIELD_ENTRY_EGR_OBJ_OVERLAY             (1 << 10)
#define BCMINT_FIELD_ENTRY_EGR_OBJ_UNDERLAY            (1 << 11)
#define BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_OVERLAY  (1 << 12)
#define BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_UNDERLAY (1 << 13)
#define BCMINT_FIELD_ENTRY_EGR_INTF_OVERLAY            (1 << 14)
#define BCMINT_FIELD_ENTRY_GPORT_MPLS                  (1 << 15)
#define BCMINT_FIELD_ENTRY_GPORT_WLAN                  (1 << 16)
#define BCMINT_FIELD_ENTRY_GPORT_VXLAN                 (1 << 17)
#define BCMINT_FIELD_ENTRY_GPORT_VLAN                  (1 << 18)
#define BCMINT_FIELD_ENTRY_GPORT_NIV                   (1 << 19)
#define BCMINT_FIELD_ENTRY_GPORT_MIM                   (1 << 20)
#define BCMINT_FIELD_ENTRY_GPORT_FLOWPORT              (1 << 21)
#define BCMINT_FIELD_ENTRY_GPORT_MODPORT               (1 << 22)
#define BCMINT_FIELD_ENTRY_L2_MCAST                    (1 << 23)
#define BCMINT_FIELD_ENTRY_L3_MCAST                    (1 << 24)
#define BCMINT_FIELD_ENTRY_HW_INVALIDATED              (1 << 25)
#define BCMINT_FIELD_ENTRY_TUNNEL_TYPE_MPLS            (1 << 26)
#define BCMINT_FIELD_ENTRY_TUNNEL_TYPE_IPL3            (1 << 27)
#define BCMINT_FIELD_ENTRY_TUNNEL_TYPE_FLEXFLOW        (1 << 28)
#define BCMINT_FIELD_ENTRY_STAT_ATTACHED               (1 << 29)
#define BCMINT_FIELD_ENTRY_TEMP_FOR_SLICE_INFO         (1 << 30)

/*! Entry Action Flags */
#define BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_OVERLAY            (1 << 1)
#define BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY (1 << 2)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_MPLS                 (1 << 3)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_WLAN                 (1 << 4)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_VXLAN                (1 << 5)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_VLAN                 (1 << 6)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_NIV                  (1 << 7)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_MIM                  (1 << 8)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_FLOW                 (1 << 9)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_MODPORT              (1 << 10)
#define BCMINT_FIELD_ENTRY_ACTION_COSQNEW_WITH_INT           (1 << 11)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_L2GRE                (1 << 12)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_TRILL                (1 << 13)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_PORT                 (1 << 14)
#define BCMINT_FIELD_ENTRY_ACTION_GPORT_TRUNK                (1 << 15)
#define BCMINT_FIELD_ENTRY_ACTION_POLICER_CANCEL_OVERLAY_0   (1 << 16)
#define BCMINT_FIELD_ENTRY_ACTION_POLICER_CANCEL_OVERLAY_1   (1 << 17)

#define BCMINT_FIELD_ENTRY_ACTION_GPORT_FLAGS         \
        (BCMINT_FIELD_ENTRY_ACTION_GPORT_MPLS  |      \
         BCMINT_FIELD_ENTRY_ACTION_GPORT_WLAN  |      \
         BCMINT_FIELD_ENTRY_ACTION_GPORT_VXLAN |      \
         BCMINT_FIELD_ENTRY_ACTION_GPORT_VLAN  |      \
         BCMINT_FIELD_ENTRY_ACTION_GPORT_NIV   |      \
         BCMINT_FIELD_ENTRY_ACTION_GPORT_MIM   |      \
         BCMINT_FIELD_ENTRY_ACTION_GPORT_FLOW  |      \
         BCMINT_FIELD_ENTRY_ACTION_GPORT_L2GRE |      \
         BCMINT_FIELD_ENTRY_ACTION_GPORT_TRUNK |      \
         BCMINT_FIELD_ENTRY_ACTION_GPORT_TRILL)

/* Validate the Presel ID */
#define BCMINT_FIELD_IS_PRESEL_ENTRY(_e)           \
    (((_e & 0xf0000000) == BCM_FIELD_QUALIFY_PRESEL) ? TRUE:FALSE)

/*! Entry Flags */
#define BCMINT_FIELD_ENTRY_TYPE_PRESEL                (1 << 1)

/* Initialise field group id bitmap */
#define BCMINT_FIELD_GRPID_BMP_INIT(group_bmp)  \
    sal_memset(&(group_bmp), 0, sizeof(bcmi_field_grpid_bmp_t))

/* Initialise field entry id bitmap */
#define BCMINT_FIELD_ENTRYID_BMP_INIT(entryid_bmp)  \
    sal_memset(&(entryid_bmp), 0, sizeof(bcmint_field_entryid_bmp_t))

/* Initialise field entry id bitmap */
#define BCMINT_FIELD_PRESELID_BMP_INIT(preselid_bmp)  \
    sal_memset(&(preselid_bmp), 0, sizeof(bcmint_field_preselid_bmp_t))

/* Internal stage ID get from the stage qualifier */
#define BCMINT_FIELD_STAGE_ID_GET(_stage_qual_, _stage_id_)                   \
     {                                                                        \
         if (_stage_qual_ == bcmFieldQualifyStageIngress) {                   \
             _stage_id_ = bcmiFieldStageIngress;                              \
         } else if (_stage_qual_ == bcmFieldQualifyStageLookup) {             \
             _stage_id_ = bcmiFieldStageVlan;                                 \
         } else if (_stage_qual_ == bcmFieldQualifyStageEgress) {             \
             _stage_id_ = bcmiFieldStageEgress;                               \
         } else if (_stage_qual_ == bcmFieldQualifyStageIngressExactMatch) {  \
             _stage_id_ = bcmiFieldStageExactMatch;                           \
         }  else if (_stage_qual_ == bcmFieldQualifyStageIngressFlowtracker) {\
             _stage_id_ = bcmiFieldStageFlowTracker;                          \
         }                                                                    \
     }

/* Pseudo qualifiers */
#define BCMINT_FIELD_IS_PSEUDO_QUAL(_q)                         \
            ((bcmFieldQualifyStage                   == _q) ||  \
             (bcmFieldQualifyStageIngress            == _q) ||  \
             (bcmFieldQualifyStageLookup             == _q) ||  \
             (bcmFieldQualifyStageEgress             == _q) ||  \
             (bcmFieldQualifyStageIngressExactMatch  == _q) ||  \
             (bcmFieldQualifyStageIngressFlowtracker == _q) ||  \
             (bcmiFieldQualifyUdfExtOuter            == _q) ||  \
             (bcmiFieldQualifyUdfExtInner            == _q))

/* Compression Qualifiers */
#define BCMINT_FIELD_IS_COMPRESSION_QUAL(_q) \
            ((bcmFieldQualifySrcIp     == _q) || \
             (bcmFieldQualifyDstIp     == _q) || \
             (bcmFieldQualifySrcIp6    == _q) || \
             (bcmFieldQualifyDstIp6    == _q) || \
             (bcmFieldQualifyL4SrcPort == _q) || \
             (bcmFieldQualifyL4DstPort == _q))

#define BCMINT_FIELD_IS_IP_QUAL(_q) \
            ((bcmFieldQualifySrcIp     == _q) || \
             (bcmFieldQualifyDstIp     == _q) || \
             (bcmFieldQualifySrcIp6    == _q) || \
             (bcmFieldQualifyDstIp6    == _q))

/* Colored action. */
#define BCMINT_FIELD_IS_COLORED_ACTION(action) \
    ((action == bcmFieldActionGpDrop) || \
     (action == bcmFieldActionYpDrop) || \
     (action == bcmFieldActionRpDrop) || \
     (action == bcmFieldActionGpPrioIntNew) || \
     (action == bcmFieldActionYpPrioIntNew) || \
     (action == bcmFieldActionRpPrioIntNew) || \
     (action == bcmFieldActionGpIntCongestionNotificationNew) || \
     (action == bcmFieldActionYpIntCongestionNotificationNew) || \
     (action == bcmFieldActionRpIntCongestionNotificationNew) || \
     (action == bcmFieldActionGpDropPrecedence) || \
     (action == bcmFieldActionYpDropPrecedence) || \
     (action == bcmFieldActionRpDropPrecedence) || \
     (action == bcmFieldActionGpTimeStampToCpu) || \
     (action == bcmFieldActionYpTimeStampToCpu) || \
     (action == bcmFieldActionRpTimeStampToCpu) || \
     (action == bcmFieldActionGpSwitchToCpuReinstate) || \
     (action == bcmFieldActionYpSwitchToCpuReinstate) || \
     (action == bcmFieldActionRpSwitchToCpuReinstate) || \
     (action == bcmFieldActionGpCopyToCpu) || \
     (action == bcmFieldActionYpCopyToCpu) || \
     (action == bcmFieldActionRpCopyToCpu) || \
     (action == bcmFieldActionGpCopyToCpuCancel) || \
     (action == bcmFieldActionYpCopyToCpuCancel) || \
     (action == bcmFieldActionRpCopyToCpuCancel) || \
     (action == bcmFieldActionGpDropCancel) || \
     (action == bcmFieldActionYpDropCancel) || \
     (action == bcmFieldActionRpDropCancel) || \
     (action == bcmFieldActionGpOpaqueCtrlId) || \
     (action == bcmFieldActionYpOpaqueCtrlId) || \
     (action == bcmFieldActionRpOpaqueCtrlId) || \
     (action == bcmFieldActionGpStatGroup) || \
     (action == bcmFieldActionYpStatGroup) || \
     (action == bcmFieldActionRpStatGroup) || \
     (action == bcmFieldActionGpDscpNew) || \
     (action == bcmFieldActionYpDscpNew) || \
     (action == bcmFieldActionRpDscpNew) || \
     (action == bcmFieldActionGpPrioPktNew) || \
     (action == bcmFieldActionYpPrioPktNew) || \
     (action == bcmFieldActionRpPrioPktNew) || \
     (action == bcmFieldActionGpOuterVlanCfiNew) || \
     (action == bcmFieldActionYpOuterVlanCfiNew) || \
     (action == bcmFieldActionRpOuterVlanCfiNew) || \
     (action == bcmFieldActionGpCosMapNew) || \
     (action == bcmFieldActionYpCosMapNew) || \
     (action == bcmFieldActionRpCosMapNew))

/* Egress Mask Bitmaps */
#define BCMINT_FIELD_EGRESS_MASK_L2_L3         0
#define BCMINT_FIELD_EGRESS_MASK_L2            1
#define BCMINT_FIELD_EGRESS_MASK_L3            2

/* Redirection Action */
#define BCMINT_FIELD_EGRESS_MASK_OP_AND        0
#define BCMINT_FIELD_EGRESS_MASK_OP_OR         1
#define BCMINT_FIELD_DELAYED_DROP              2
#define BCMINT_FIELD_DELAYED_REDIRECT          3

/* Get stage entry id from entry id and stage. */
#define BCMINT_FIELD_STAGE_ENTRY_ID_GET(_stg_, _eid_, _stg_eid_)      \
        {                                                             \
           if (_stg_ == bcmiFieldStageIngress) {                      \
               _stg_eid_ = _eid_ | BCM_FIELD_IFP_ID_BASE;             \
           } else if (_stg_ == bcmiFieldStageVlan) {                  \
               _stg_eid_ = _eid_ | BCM_FIELD_VFP_ID_BASE;             \
           } else if (_stg_ == bcmiFieldStageEgress) {                \
               _stg_eid_ = _eid_ | BCM_FIELD_EFP_ID_BASE;             \
           } else if (_stg_ == bcmiFieldStageExactMatch) {            \
               _stg_eid_ = _eid_ | BCM_FIELD_EM_ID_BASE;              \
           } else if (_stg_ == bcmiFieldStageFlowTracker) {           \
               _stg_eid_ = _eid_ | BCM_FIELD_FT_ID_BASE;              \
           } else {                                                   \
                  SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);            \
           }                                                          \
        }                                                             \

/* Hint Hash size & mask */
#define BCMINT_FP_HASH_INDEX_MASK   (BCMI_FIELD_HA_HINT_ID_MAX - 1)

#define BCMINT_FIELD_ENTRY_ACTION_VALID_LT_FIELDS_SET           (1 << 1)
#define BCMINT_FIELD_ENTRY_ACTION_INVALID_LT_FIELDS_SET         (1 << 2)

/******************************************************************************
 * Internal Field Module Structures
 */

typedef int (*ltsw_field_profile_lt_get_f)(int unit, bcmi_ltsw_field_stage_t stage_id, int index, void *profile);
typedef int (*ltsw_field_profile_lt_insert_f)(int unit, bcmi_ltsw_field_stage_t stage_id, int index, void *profile);
typedef int (*ltsw_field_profile_lt_delete_f)(int unit, bcmi_ltsw_field_stage_t stage_id, int index);

typedef struct ltsw_field_profile_info_s {
    bcmi_ltsw_profile_hdl_t phd;
    int entry_idx_min;
    int entry_idx_max;
    int entries_per_set;
    int entry_size;
    ltsw_field_profile_lt_get_f lt_get;
    ltsw_field_profile_lt_insert_f lt_ins;
    ltsw_field_profile_lt_delete_f lt_del;
} ltsw_field_profile_info_t;

/* EFP Key & Qual mapping */
typedef struct bcmint_field_efp_key_qual_map_s {
    bcm_field_qset_t key_qset;
    char *grp_mode;
    char *port_pkt_type;
    bcm_field_group_mode_type_t mode_type;
    bcm_field_group_packet_type_t packet_type;
} bcmint_field_efp_key_qual_map_t;

/* Different FP template IDs and key fields. */
typedef struct bcmint_field_tbls_info_s {
    /* Group template SID */
    char *group_sid;
    /* Group template key FID */
    char *group_key_fid;
    /* Rule template SID */
    char *rule_sid;
    /* Rule template key FID */
    char *rule_key_fid;
    /* Entry Rule template key FID */
    char *entry_rule_key_fid;
    /* Policy template SID */
    char *policy_sid;
    /* Policy template key FID */
    char *policy_key_fid;
    /* Entry Policy template key FID */
    char *entry_policy_key_fid;
    /* Entry SID */
    char *entry_sid;
    /* Entry key FID */
    char *entry_key_fid;
    /* PDD template SID */
    char *pdd_sid;
    /* PDD template key FID */
    char *pdd_key_fid;
    /* SBR template SID */
    char *sbr_sid;
    /* SBR template key FID */
    char *sbr_key_fid;
    /* Presel entry template SID */
    char *presel_entry_sid;
    /* Presel entry template key FID */
    char *presel_entry_key_fid;
    /* Group Presel entry template key FID */
    char *grp_presel_entry_key_fid;
    /* Presel entry count */
    char *presel_entry_count_fid;
    /* Presel entry priority */
    char *presel_entry_prio_fid;
    /* Presel Group template SID */
    char *presel_group_sid;
    /* Presel Group template key FID */
    char *presel_group_key_fid;
    /* Counter template SID */
    char *ctr_sid;
    /* Counter template key FID */
    char *ctr_key_fid;
    /* Meter template SID */
    char *meter_sid;
    /* Meter template key FID */
    char *meter_key_fid;
    /* Operation Mode SID */
    char *oper_mode_sid;
    /* Operation Mode FID */
    char *oper_mode_fid;
    /* Flex counter index 0 */
    char *flex_ctr_idx0_fid;
    /* Flex counter index 1 */
    char *flex_ctr_idx1_fid;
    /* Flex counter index 2 */
    char *flex_ctr_idx2_fid;
    /* Flex counter index 3 */
    char *flex_ctr_idx3_fid;
    /* Flex counter index 4 */
    char *flex_ctr_idx4_fid;
    /* Flex counter index 5 */
    char *flex_ctr_idx5_fid;
    /* Flex counter index 6 */
    char *flex_ctr_idx6_fid;
    /* Flex counter index 7 */
    char *flex_ctr_idx7_fid;
    /* Flex counter index 8 */
    char *flex_ctr_idx8_fid;
    /* Flex counter index 9 */
    char *flex_ctr_idx9_fid;
    /* Field Compression type */
    char *compression_type_fid;
    /* Field Compression count */
    char *compression_type_cnt_fid;
    /* group template oper Mode */
    char *grp_oper_mode_fid;
    /* Source class mode SID. */
    char *src_class_mode_sid;
    /* Source class mode key FID. */
    char *src_class_mode_key_fid;
    /* Default SBR template key FID */
    char *def_sbr_key_fid;
    /* Default PDD template key FID */
    char *def_pdd_key_fid;
    /* Default action template key FID */
    char *def_policy_key_fid;
    /* Entry Enable FID */
    char *entry_enable_fid;
    /* Entry Priority FID */
    char *entry_priority_fid;
    /* Group info LT SID */
    char *group_info_sid;
    /* Group info LT key FID */
    char *group_info_key_fid;
    /* Group info LT num_parts FID */
    char *group_info_num_parts_fid;
    /* Group info LT entries created FID */
    char *group_info_entries_created_fid;
    /* Group info LT entries tentative FID */
    char *group_info_entries_tentative_fid;
    /* Group info LT action res FID */
    char *group_info_act_res_fid;
    /* Group info qual LT SID */
    char *group_info_partition_sid;
    /* Group info qual LT key FID */
    char *group_info_partition_key_fid;
    /* Group info actions LT SID */
    char *group_info_action_sid;
    /* Group info action LT key FID */
    char *group_info_action_key_fid;
    /* Group info presel LT SID */
    char *group_info_presel_sid;
    /* Group info presel LT key FID */
    char *group_info_presel_key_fid;
    /* Group info qual/action/presel LT Key Part FID */
    char *group_info_common_partition_key_fid;
    /* Group info LT slice count FID */
    char *group_info_num_slice_fid;
    /* Group info LT slice FID */
    char *group_info_slice_fid;
    /* compress config FID */
    char *compress_sid;
    /* compress config FID */
    char *compress_src_ip4_only_sid;
    /* compress config FID */
    char *compress_src_ip6_only_sid;
    /* compress config FID */
    char *compress_dst_ip4_only_sid;
    /* compress config FID */
    char *compress_dst_ip6_only_sid;
    /* Slice info LT SID */
    char *slice_info_sid;
    /* Slice info LT key FID */
    char *slice_info_key_fid;
    /* Slice info LT pipe FID */
    char *slice_info_pipe_fid;
    /* Slice info LT entry in use FID */
    char *slice_info_entry_in_use_fid;
    /* Slice info LT entry maximum FID */
    char *slice_info_entry_max_fid;
    /* Action container info LT SID */
    char *action_cont_info_sid;
    /* Action container info LT KEY field */
    char *action_cont_info_key_fid;
    /* Stage info LT SID */
    char *stage_info_sid;
    /* Stage info slice count FID */
    char *stage_info_num_slices_fid;
    /* Stage info entry count FID */
    char *stage_info_num_entries_fid;
    /* Meter info LT SID */
    char *meter_info_sid;
    /* Meter info num meters FID */
    char *meter_info_num_meters_fid;
    /* Meter info entry in use FID */
    char *meter_info_num_meters_per_pipe_fid;
    /* Action info CPU Match ID FID */
    char *action_info_cpu_match_id_fid;
    /* Action l3 switch nhop fid */
    char *action_l3_switch_nhop_fid;
    /* Action l3 switch ecmp fid */
    char *action_l3_switch_ecmp_fid;
    /* Action redirect egr nexthop fid */
    char *action_redirect_egr_nhop_fid;
    /* Action redirect egr nexthop ecmp fid */
    char *action_redirect_egr_ecmp_fid;
    /* Action info Mirror container ID FID */
    char *action_mirror_cont_id_fid;
    /* Action info Mirror container enable FID */
    char *action_mirror_cont_enable_fid;
    /* Redirect profile FID */
    char *redirect_profile_sid;
    /* Redirect profile key SID */
    char *redirect_profile_key_fid;
    /* Redirect profile ports SID */
    char *redirect_profile_ports_fid;
    /* Egress Mask SID */
    char *egress_mask_profile_sid;
    /* Egress Mask key FID */
    char *egress_mask_profile_key_fid;
    /* Egress Mask section FID */
    char *egress_mask_profile_section_fid;
    /* Egress Mask ports FID */
    char *egress_mask_profile_ports_fid;
    /* Egress Mask target FID */
    char *egress_mask_profile_target_fid;
    /* Egress Mask action FID */
    char *egress_mask_profile_action_fid;
    /* delayed_redirect profile FID */
    char *delayed_redirect_profile_sid;
    /* delayed_redirect profile key SID */
    char *delayed_redirect_profile_key_fid;
    /* delayed_redirect profile ports SID */
    char *delayed_redirect_profile_ports_fid;
    /* delayed_redirect profile elephant pkts only SID */
    char *delayed_redirect_profile_elephant_pkts_fid;
    /* delayed_drop profile FID */
    char *delayed_drop_profile_sid;
    /* delayed_drop profile key SID */
    char *delayed_drop_profile_key_fid;
    /* delayed_drop profile ports SID */
    char *delayed_drop_profile_ports_fid;
    /* delayed_drop profile elephant pkts only SID */
    char *delayed_drop_profile_elephant_pkts_fid;
    /* Action unmodified redirect to port FID */
    char *action_unmod_redirect_to_port_fid;
    /* Action unmodified redirect to trunk FID */
    char *action_unmod_redirect_to_trunk_fid;
    /* Action to enable counter FID */
    char *action_ctr_enable;
    /* Flex Ctr Action Container FID */
    char *flex_ctr_cont_fid;
} bcmint_field_tbls_info_t;

/* Information about the color action tables. */
typedef struct bcmint_field_color_action_tbls_info_s {
    /* Number of color action table pools. */
    int num_pools;
    /* Pipe LT name. */
    char *pipe_cfg_sid;
    /* Pipe field name. */
    char *pipe_cfg_fid;
    /* PIPE field name. */
    char *pipe_fid;
    /* Color action PDD table name. */
    char *pdd_tbl_sid;
    /* Color action PDD table key field name. */
    char *pdd_tbl_key_fid;
    /* Color action SBR table name. */
    char *sbr_sid;
    /* Color action SBR table key field name. */
    char *sbr_key_fid;
    /* Array of color action table names of each pool . */
    char *tbl_sid[BCM_FIELD_COLOR_ACTION_POOL_MAX];
    /* Array of color action table key field names of each pool . */
    char *tbl_key_fid[BCM_FIELD_COLOR_ACTION_POOL_MAX];
} bcmint_field_color_action_tbls_info_t;

/* field hintid bitmap */
typedef struct bcmint_field_hintid_bmp_s {
    SHR_BITDCL  *w;
} bcmint_field_hintid_bmp_t;

/* Field Presel Group database Structure */
typedef struct bcmint_field_presel_group_s {

    /*! Presel Group ID */
    int presel_group;

    /*! Presel QSET */
    bcm_field_qset_t presel_qset;

    /*! next presel group */
    struct bcmint_field_presel_group_s *next;

} bcmint_field_presel_group_t;


/* Field Action Strength Profile Structure */
typedef struct bcmint_field_action_strength_prof_s {
    bool color;
    uint32 group_priority;
    bcmi_field_aset_t aset;
    uint32 *action_strength;
    uint32 action_strength_cnt;
    uint32 strength_prof_idx;
    bool update;
    uint16_t ref_cnt;
    struct bcmint_field_action_strength_prof_s *next;
    struct bcmint_field_action_strength_prof_s *prev;
} bcmint_field_action_strength_prof_t;

typedef struct bcmint_field_action_strength_prof_base_s {
    bcmint_field_action_strength_prof_t *strength_prof;
} bcmint_field_action_strength_prof_base_t;

/*!
 * \brief Field Stage informaiton.
 */
typedef struct bcmint_field_stage_info_s {

    /*! Stage Flags BCM_FIELD_STAGE_FLAGS_XXX */
    uint32_t flags;

    /*! Stage ID */
    bcmi_ltsw_field_stage_t stage_id;

    /*! Base value for stage Group/Entry/Presel ID */
    int stage_base_id;

    /*! entry qualifier db count */
    uint16_t qual_db_count;

    /*! group qualifier db count */
    uint16_t group_qual_db_count;

    /*! entry udf qualifier db count */
    uint16_t udf_qual_db_count;

    /*! presel entry qualifier db count */
    uint16_t presel_qual_db_count;

    /*! action db count */
    uint16_t action_db_count;

    /*! colored action profile bitmap. */
    bcmi_field_ha_profile_info_t *color_tbl_profile;

    /*! colored action pdd profile bitmap. */
    bcmi_field_ha_profile_info_t *color_pdd_tbl_profile;

    /*! colored action sbr profile bitmap. */
    bcmi_field_ha_profile_info_t *color_sbr_tbl_profile;

    /*! entry qualifier lt map db */
    bcm_field_qual_map_t *qual_lt_map_db;

    /*! group qualifier lt map db */
    bcm_field_qual_map_t *group_qual_lt_map_db;

    /*! udf qualifier lt map db */
    bcm_field_qual_map_t *udf_qual_lt_map_db;

    /*! presel qualifier lt map db */
    bcm_field_qual_map_t *presel_qual_lt_map_db;

    /*! action lt map db */
    const bcm_field_action_map_t *action_lt_map_db;

    /*! LT table and key field names */
    bcmint_field_tbls_info_t *tbls_info;

    /*! Colored action table and key field names. */
    bcmint_field_color_action_tbls_info_t *color_action_tbls_info;

    /*! Stage Group info */
    bcmi_field_ha_group_info_t *group_info;

    /*! Stage Presel info */
    bcmi_field_ha_presel_info_t *presel_info;

    /*! Stage Entry info */
    bcmi_field_ha_entry_info_t *entry_info;

    /*! Strength Profile db */
    bcmint_field_action_strength_prof_base_t *strength_base_prof;

    /*! SBR enabled actions supported in a stage */
    bcmi_field_aset_t sbr_aset;

    /*! Count of SBR enabled actions supported in a stage */
    uint16_t sbr_aset_cnt;

    /*! SBR Max limit */
    uint16_t sbr_max_limit;

    /*! Maximum possible strength ID */
    uint16_t strength_id_max;

    /*! Default Presel Group ID */
    int presel_group_def;

    /*! EFP Key & Qual mapping */
    bcmint_field_efp_key_qual_map_t efp_key_qual_map[BCMINT_FIELD_EFP_KEYS];

} bcmint_field_stage_info_t;

/* Different compression FP template IDs and key fields. */
typedef struct bcmint_field_compress_tbls_info_s {

    /* Index Mapped Ethertype LT SID */
    char *index_mapped_ethertype_sid;

    /* Index Mapped Ethertype LT Key */
    char *index_key_ethertype_fid;

    /* Index Mapped Ethertype LT Value */
    char *index_val_ethertype_fid;

    /* Index Mapped Ethertype LT Mapped Val */
    char *index_map_val_ethertype_fid;

    /* Index Mapped IP Protocol LT SID */
    char *index_mapped_ipprotocol_sid;

    /* Index Mapped IP Protocol LT Key */
    char *index_key_ipprotocol_fid;

    /* Index Mapped IP Protocol LT Value */
    char *index_val_ipprotocol_fid;

    /* Index Mapped IP Protocol LT Mapped Val */
    char *index_map_val_ipprotocol_fid;

    /* Direct Mapped TTL LT SID */
    char *direct_mapped_ttl_sid;

    /* Direct Mapped TTL LT Val */
    char *direct_val_ttl_fid;

    /* Direct Mapped TTL LT Mapped Val */
    char *direct_map_val_ttl_fid;

    /* Direct Mapped TCP flags LT SID */
    char *direct_mapped_tcp_sid;

    /* Direct Mapped TCP flags LT Val */
    char *direct_val_tcp_fid;

    /* Direct Mapped TCP flags LT Mapped Val */
    char *direct_map_val_tcp_fid;

    /* Direct Mapped TOS LT SID */
    char *direct_mapped_tos_sid;

    /* Direct Mapped TOS LT Val */
    char *direct_val_tos_fid;

    /* Direct Mapped TOS LT Mapped Val */
    char *direct_map_val_tos_fid;

    /* Direct Mapped TTL zero LT SID */
    char *direct_mapped_ttl_zero_sid;

    /* Direct Mapped TTL zero LT Val */
    char *direct_val_ttl_zero_fid;

    /* Direct Mapped TTL LT zero Mapped Val */
    char *direct_map_val_ttl_zero_fid;

    /* Direct Mapped TCP flags zero LT SID */
    char *direct_mapped_tcp_zero_sid;

    /* Direct Mapped TCP flags zero LT Val */
    char *direct_val_tcp_zero_fid;

    /* Direct Mapped TCP flags zero LT Mapped Val */
    char *direct_map_val_tcp_zero_fid;

    /* Direct Mapped TOS zero LT SID */
    char *direct_mapped_tos_zero_sid;

    /* Direct Mapped TOS zero LT Val */
    char *direct_val_tos_zero_fid;

    /* Direct Mapped TOS LT Mapped Val */
    char *direct_map_val_tos_zero_fid;

    /* Direct Mapped TTL one LT SID */
    char *direct_mapped_ttl_one_sid;

    /* Direct Mapped TTL one LT Val */
    char *direct_val_ttl_one_fid;

    /* Direct Mapped TTL LT one Mapped Val */
    char *direct_map_val_ttl_one_fid;

    /* Direct Mapped TCP flags one LT SID */
    char *direct_mapped_tcp_one_sid;

    /* Direct Mapped TCP flags one LT Val */
    char *direct_val_tcp_one_fid;

    /* Direct Mapped TCP flags one LT Mapped Val */
    char *direct_map_val_tcp_one_fid;

    /* Direct Mapped TOS one LT SID */
    char *direct_mapped_tos_one_sid;

    /* Direct Mapped TOS one LT Val */
    char *direct_val_tos_one_fid;

    /* Direct Mapped TOS LT Mapped Val */
    char *direct_map_val_tos_one_fid;

    /* Direct Mapped IP Tunnel LT SID */
    char *direct_mapped_tunnelttl_sid;

    /* Direct Mapped IP Tunnel LT Val */
    char *direct_val_tunnelttl_fid;

    /* Direct Mapped IP Tunnel LT Mapped Val */
    char *direct_map_val_tunnelttl_fid;

    /* Direct Mapped ipprotocol LT SID */
    char *direct_mapped_ipprotocol_sid;

    /* Direct Mapped ipprotocol LT Val */
    char *direct_val_ipprotocol_fid;

    /* Direct Mapped ipprotocol LT Mapped Val */
    char *direct_map_val_ipprotocol_fid;

    /* PIPE */
    char *pipe_fid;

    /* Operation Mode SID */
    char *oper_mode_sid;

    /* Operation Mode FID */
    char *oper_mode_fid;

} bcmint_field_compress_tbls_info_t;

/*!
 * \brief Field Control informaiton.
 */
typedef struct bcmint_field_control_info_s {

    bcmint_field_stage_info_t stage_info[bcmiFieldStageCount];

    /*! Bitmap of created hint ids */
    bcmint_field_hintid_bmp_t hintid_bmp;

    /*! Hints lookup hash */
    bcmi_field_ha_hints_t *hints_hash;

    /*! Field Control LT Map Info DB */
    const bcm_field_control_map_t *control_lt_map_db;

    /*! Field control db count */
    uint16_t control_db_count;

    /*! Compression LT table and field names */
    bcmint_field_compress_tbls_info_t *comp_tbls_info;

} bcmint_field_control_info_t;


/*!
 * \brief Field database.
 */
typedef struct bcmi_ltsw_field_db_s {

    /*! Field module lock. */
    sal_mutex_t lock;

    /*! Field control database. */
    bcmint_field_control_info_t control_db;

} bcmi_ltsw_field_db_t;

extern bcmi_ltsw_field_db_t bcmi_ltsw_field_db[BCM_MAX_NUM_UNITS];

/*! Field mutex lock. */
#define FP_LOCK(unit) \
    do { \
        if (bcmi_ltsw_field_db[unit].lock) { \
            sal_mutex_take(bcmi_ltsw_field_db[unit].lock, SAL_MUTEX_FOREVER); \
        } \
    } while(0)

/*! Field mutex unlock. */
#define FP_UNLOCK(unit) \
    do { \
        if (bcmi_ltsw_field_db[unit].lock) { \
            sal_mutex_give(bcmi_ltsw_field_db[unit].lock); \
        } \
    } while(0)

/******************************************************************************
 * Internal Field Module Specific Purpose Structures
 */
typedef struct bcmi_field_udf_qset_bitmap_s {

    /*! 1b Chunk qualifiers bitmaps */
    uint32_t *chunk_b1_bitmap;

    /*! 2b Chunk qualifiers bitmaps */
    uint32_t *chunk_b2_bitmap;

    /*! 4b Chunk qualifiers bitmap */
    uint32_t *chunk_b4_bitmap;

} bcmi_field_udf_qset_bitmap_t;

/******************************************************************************
 * Internal Field Module Profile Management Functions
 */

/*!
 * \brief Get profile entries from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] stage_id Stage Id.
 * \param [in] index First profile entry index.
 * \param [in] pinfo Profile information.
 * \param [out] profile Profile entries.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_field_profile_get(int unit,
        bcmi_ltsw_field_stage_t stage_id, int index,
        ltsw_field_profile_info_t *pinfo, void *profile);

/*!
 * \brief Add profile.
 *
 * If same entries existed, just increment reference count.
 *
 * \param [in] unit Unit number.
 * \param [in] stage_id Stage Id.
 * \param [in] profile Profile to be added.
 * \param [in] ref_count Add how many reference counts for this profile.
 * \param [in] pinfo Profile information.
 * \param [out] index Index of the added or existed profile enrty.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_field_profile_add(int unit,
        bcmi_ltsw_field_stage_t stage_id,
        void *profile, int ref_count,
        ltsw_field_profile_info_t *pinfo, int *index);

/*!
 * \brief Add profile reference.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the added profile entry.
 * \param [in] ref_count Add how many reference counts for this profile.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_field_profile_add_ref(int unit, int index, int ref_count,
        ltsw_field_profile_info_t *pinfo);

/*!
 * \brief Delete profile.
 *
 * If the entry is referred by more than one soure, just decreament ref count.
 *
 * \param [in] unit Unit number.
 * \param [in] stage_id Stage Id.
 * \param [in] index Index of the first entry to be deleted.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_field_profile_delete(int unit,
        bcmi_ltsw_field_stage_t stage_id, int index,
        ltsw_field_profile_info_t *pinfo);

/*!
 * \brief Recover hash and reference count for profile.
 *
 * Hash is recovered from HW. Reference count is always increamented.
 *
 * \param [in] unit Unit number.
 * \param [in] stage_id Stage Id.
 * \param [in] index Profile index to be recovered.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_field_profile_recover(int unit,
        bcmi_ltsw_field_stage_t stage_id,
        int index,
        ltsw_field_profile_info_t *pinfo);

/*!
 * \brief Compare profile set.
 *
 * Compare if given profile entries equals to the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] index Profile table index to be compared.
 * \param [in] pinfo Profile information.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_field_ingress_profile_cmp(int unit,
        void *entries, int index,
        ltsw_field_profile_info_t *pinfo, int *cmp);

/******************************************************************************
 * Internal Field Module Functions
 */
extern int
bcmint_field_control_get(int unit, bcmint_field_control_info_t **fc);

extern int
bcmint_field_stage_info_get(int unit,
                            bcmi_ltsw_field_stage_t stage_id,
                            bcmint_field_stage_info_t **stage_info);

extern bool
bcmint_field_map_supported(int unit,
                           bcmint_field_stage_info_t *stage_info,
                           bcm_field_qual_map_info_t map);
extern int
bcmint_field_stage_compress_type_init(int unit,
                           bcmint_field_stage_info_t *stage_info);
extern int
bcmint_field_qual_map_find(int unit,
                           bcmi_field_qualify_t qual,
                           bcmint_field_stage_info_t *stage_info,
                           bool presel,
                           bool group,
                           int *map_count,
                           bcm_field_qual_map_t **lt_map);

extern int
bcmint_field_group_oper_lookup(
    int unit,
    int group_id,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t **group_oper);
extern int
bcmint_field_entry_oper_lookup(
    int unit,
    int group_id,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_entry_oper_t **entry_oper);
extern int
bcmint_field_stage_from_entry_get(int unit,
                                  bcm_field_entry_t entry,
                                  bcmint_field_stage_info_t **stage_info);
extern int
bcmint_field_stage_from_group_get(int unit, bcm_field_group_t group,
                                  bcmint_field_stage_info_t **stage_info);
extern int
bcmint_field_group_stage_based_id_get(int unit,
                                      bcmi_ltsw_field_stage_t stage_id,
                                      bcm_field_group_t group_id,
                                      bcm_field_group_t *stage_base_group_id);
extern int
bcmint_field_entry_stage_based_id_get(int unit,
                                      bcmi_ltsw_field_stage_t stage_id,
                                      bcm_field_entry_t entry_id,
                                      bcm_field_entry_t *stage_base_entry_id);
extern int
bcmint_field_hint_init(int unit);

extern int
bcmint_field_hint_cleanup(int unit);

extern int
bcmint_field_hints_create(int unit, bcm_field_hintid_t *hint_id);

extern int
bcmint_field_hints_destroy(int unit, bcm_field_hintid_t hint_id);

extern int
bcmint_field_hints_add(int unit,
                       bcm_field_hintid_t hint_id,
                       bcm_field_hint_t *hint);

extern int
bcmint_field_hints_get(int unit,
                       bcm_field_hintid_t hint_id,
                       bcm_field_hint_t *hint);

extern int
bcmint_field_hints_delete(int unit,
                          bcm_field_hintid_t hint_id,
                          bcm_field_hint_t *hint);

extern int
bcmint_field_hints_delete_all(int unit,
                              bcm_field_hintid_t hint_id);

extern int
bcmint_field_hint_group_count_update(int unit,
                                     bcm_field_hintid_t hint_id,
                                     uint8 action);
extern int
bcmint_field_hints_group_qual_value_update(int unit,
                                           bcm_field_hintid_t hint_id,
                                           uint8_t *qual_hint_extracted,
                                           int qual_idx,
                                           int width,
                                           int map_offset,
                                           uint64_t *max_value);

extern int
bcmint_field_group_udf_qset_bitmap_alloc(int unit,
    bcmi_field_udf_qset_bitmap_t **udf_qset_bitmap);

extern int
bcmint_field_group_udf_qset_bitmap_free(int unit,
    bcmi_field_udf_qset_bitmap_t **udf_qset_bitmap);

extern int
bcmint_field_group_udf_qset_bitmap_update(int unit,
    bcm_field_hintid_t hint_id,
    bcm_udf_id_t udf_id,
    bcm_field_qset_t *udf_qset,
    bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap);

extern int
bcmint_field_hint_dump(int unit, bcm_field_hintid_t hint_id);

extern int
bcmint_field_hint_dump_all(int unit);

extern int
bcmint_field_group_udf_qset_update_with_hints(
    int unit,
    bcm_field_hintid_t hintid,
    bcm_field_qset_t *qset,
    bcm_field_qset_t *udf_qset,
    bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap);

extern int
bcmint_field_process_group_qset(int unit,
                                bcm_field_qset_t *qset,
                                bcm_field_hintid_t hintid,
                                bcmint_field_stage_info_t *stage_info,
                                uint8 set_qual_bitmap,
                                bool is_presel_grp,
                                bcmlt_entry_handle_t *group_template);
extern int
bcmint_field_process_group_aset(int unit,
                                bcmint_field_stage_info_t *stage_info,
                                uint8 set_action_bitmap,
                                int act_pipe,
                                const bcm_field_aset_t *aset,
                                bcm_field_hintid_t hintid,
                                bcmlt_entry_handle_t *pdd_template,
                                int policer_pool_id,
                                int *color_pdd_idx);
extern int
bcmint_field_group_config_create(int unit,
                                 bcm_field_group_config_t *group_config,
                                 uint32 flags);
extern int
bcmint_field_group_entries_remove(int unit,
                          bcm_field_group_t group);
extern int
bcmint_field_group_destroy(int unit,
                           bcm_field_group_t group);
extern int
bcmint_field_group_destroy_all(int unit,
                               bcmint_field_stage_info_t *stage_info);
extern int
bcmint_field_strength_destroy_all(int unit,
                                  bcmint_field_stage_info_t *stage_info);
extern int
bcmint_field_group_presel_get(int unit,
                              bcm_field_group_t group,
                              bcm_field_presel_set_t *presel);
extern int
bcmint_field_group_presel_set(int unit,
                              bcm_field_group_t group,
                              bcm_field_presel_set_t *presel);
extern int
bcmint_field_group_qset_get(int unit,
                            bcm_field_group_t group,
                            bcm_field_qset_t *qset);
extern int
bcmint_field_group_qset_set(int unit,
                            bcm_field_group_t group,
                            bcm_field_qset_t qset);
extern int
bcmint_field_group_aset_get(int unit,
                            bcm_field_group_t group,
                            bcm_field_aset_t *aset);
extern int
bcmint_field_group_aset_set(int unit,
                            bcm_field_group_t group,
                            bcm_field_aset_t aset);
extern int
bcmint_field_group_mode_auto_get(int unit,
                                bcm_field_group_t group,
                                bool *mode_auto);
extern int
bcmint_field_group_priority_get(int unit,
                                bcm_field_group_t group,
                                int *prio);
extern int
bcmint_field_group_priority_set(int unit,
                                bcm_field_group_t group,
                                int prio);
extern int
bcmint_field_group_enable_get(int unit,
                              bcm_field_group_t group,
                              int *enable);
extern int
bcmint_field_group_enable_set(int unit,
                              bcm_field_group_t group,
                              int enable);
extern int
bcmint_field_group_mode_get(int unit,
                            bcm_field_group_t group,
                            bcm_field_group_mode_t *mode);
extern int
bcmint_field_entry_config_create(int unit,
                                 bcm_field_entry_config_t *entry_config);
extern int
bcmint_field_entry_config_get(int unit,
                                 bcm_field_entry_config_t *entry_config);
extern int
bcmint_field_entry_destroy(int unit,
                           bcm_field_entry_t entry,
                           bcmi_field_ha_entry_oper_t *entry_oper);
extern int
bcmint_field_entry_destroy_all(int unit, bcm_field_group_t group);

extern int
bcmint_field_entry_install(int unit,
                           bcm_field_entry_t entry);
extern int
bcmint_field_entry_prio_get(int unit,
                            bcm_field_entry_t entry,
                            int *prio);
extern int
bcmint_field_entry_prio_set(int unit,
                              bcm_field_entry_t entry,
                              int prio);
extern int
bcmint_field_entry_enable_get(int unit,
                              bcm_field_entry_t entry,
                              int *enable_flag);
extern int
bcmint_field_entry_enable_set(int unit,
                              bcm_field_entry_t entry,
                              int enable_flag);
extern int
bcmint_field_stage_from_presel_get(int unit,
                                  bcm_field_presel_t presel_id,
                                  bcmint_field_stage_info_t **stage_info);
extern int
bcmint_field_presel_create_stage_id(int unit,
                                    bcm_field_stage_t stage,
                                    bcm_field_presel_t presel_id);
extern int
bcmint_field_presel_config_create(int unit,
                                   bcm_field_presel_config_t *presel_config);
extern int
bcmint_field_presel_entry_destroy(int unit,
                            bcm_field_presel_t presel_entry);
extern int
bcmint_field_group_ports_get(int unit,
                             bcm_field_group_t group,
                             bcm_pbmp_t *pbmp);
extern int
bcmint_field_group_traverse(int unit,
                            bcm_field_group_traverse_cb callback,
                            void *user_data);
extern int
bcmint_field_group_oper_mode_get(
    int unit,
    bcm_field_qualify_t stage,
    bcm_field_group_oper_mode_t *mode);
extern int
bcmint_field_entry_flexctr_attach(int unit,
                                  bcm_field_entry_t entry,
                                  bcm_field_flexctr_config_t *flexctr_cfg);
extern int
bcmint_field_entry_flexctr_detach(int unit,
                                  bcm_field_entry_t entry,
                                  bcm_field_flexctr_config_t *flexctr_cfg);

extern int
bcmint_field_entry_policer_attach(int unit,
                                  bcm_field_entry_t entry,
                                  int level,
                                  bcm_policer_t policer_id);
extern int
bcmint_field_entry_policer_detach(int unit,
                                  bcm_field_entry_t entry,
                                  int level);
extern int
bcmint_field_entry_policer_detach_all(int unit,
                                      bcm_field_entry_t entry);
extern int
bcmint_field_entry_policer_get(int unit,
                               bcm_field_entry_t entry,
                               int level,
                               bcm_policer_t *policer_id);
extern int
bcmint_field_entry_policer_update_all(int unit,
                                      bcm_policer_t policer_id,
                                      uint8_t in_use);
extern int
bcmint_field_entry_policer_update(int unit,
                                  bcmint_field_stage_info_t *stage_info,
                                  bcmi_field_ha_entry_oper_t *entry_oper,
                                  bcm_policer_t policer_id);
extern int
bcmint_field_entry_multi_get(
    int unit,
    bcm_field_group_t group,
    int entry_size,
    bcm_field_entry_t *entry_array,
    int *entry_count);
extern int
bcmint_field_entry_remove(int unit,
                          bcmint_field_stage_info_t *stage_info,
                          int entry_id);

extern int
bcmint_field_action_entry_update(int unit,
                                 bcmi_field_ha_entry_oper_t *entry_oper,
                                 bcm_field_action_t action,
                                 bool add);
/* Field Action Internal API - Start */
extern int
bcmint_field_action_map_find(int unit,
                             bcm_field_action_t action,
                             bcmint_field_stage_info_t *stage_info,
                             const bcm_field_action_map_t **lt_map);
extern int
bcmint_field_action_mirror_gport_get(int unit,
                                     bcm_field_entry_t entry,
                                     bcmint_field_stage_info_t *stage_info,
                                     bcm_field_action_t action,
                                     uint32 *mirror_gport,
                                     int *num_gports);
extern int
bcmint_field_action_mirror_set(int unit,
                               bcm_field_entry_t entry,
                               bcmint_field_stage_info_t *stage_info,
                               bcm_field_action_t action,
                               bcm_field_action_params_t *params);
extern int
bcmint_ltsw_field_entry_action_present(
        int unit,
        bcm_field_action_t action,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bool *is_action_found);

extern int
bcmint_field_action_config_info_add(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_action_t action,
                                    bcm_field_action_params_t *params,
                                    bcm_field_action_match_config_t *match_config);
extern int
bcmint_field_action_config_info_get(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_action_t action,
                                    uint64_t *action_lt_val,
                                    bcm_field_action_params_t *params,
                                    bcm_field_action_match_config_t *match_config);
extern int
bcmint_field_action_delete(int unit,
                           bcm_field_entry_t entry,
                           bcm_field_action_t action,
                           uint32 param0, uint32 param1,
                           int no_param);
extern int
bcmint_field_action_delete_all(int unit,
                               bcm_field_entry_t entry);
/* Field Action Internal API - End */

extern int
bcmint_field_lt_entry_commit(int unit,
                             bcmlt_entry_handle_t *template,
                             bcmlt_opcode_t operation,
                             bcmlt_priority_level_t priority,
                             char *table_name,
                             char *key_name,
                             bool key_symbol,
                             uint64_t key_val,
                             char *key_str,
                             char *field_name,
                             uint64_t field_val,
                             uint32_t start_idx,
                             uint32_t num_element,
                             int free_template);
extern int
bcmint_field_entry_copy(int unit,
                        bcm_field_entry_t src_entry,
                        bcm_field_entry_t *dst_entry,
                        uint32 flags);
extern int
bcmint_field_presel_default_group_create(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int *presel_group);
extern int
bcmint_field_presel_default_group_destroy(
    int unit,
    bcmint_field_stage_info_t *stage_info);
extern int
bcmint_field_presel_group_config_create(
    int unit,
    bcm_field_presel_group_config_t *group_config);
extern int
bcmint_field_presel_group_destroy(
    int unit,
    bcm_field_group_t presel_group);
extern int
bcmint_field_group_stage_get(int unit,
                             bcm_field_qset_t *qset_p,
                             bcmi_ltsw_field_stage_t *stage);
extern int
bcmint_field_qset_is_subset(int unit,
                            const bcm_field_qset_t *qset_1,
                            bcmint_field_stage_info_t *stage_info,
                            bool presel,
                            uint16_t *qual_count);
extern int
bcmint_field_qset_id_multi_set(
    int unit,
    bcm_field_qualify_t qual,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset);

extern int
bcmint_field_qset_id_multi_get(
    int unit,
    bcm_field_qset_t qset,
    bcm_field_qualify_t qual,
    int max,
    int *objects_list,
    int *actual);

extern int
bcmint_field_qset_id_multi_delete(
    int unit,
    bcm_field_qualify_t qual,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset);
extern int
bcmint_field_group_mode_validate(int unit,
                                 bcmlt_entry_handle_t grp_template,
                                 bcm_field_group_mode_t mode,
                                 char **group_mode);
extern int
bcmint_field_find_is_subset_qset(
        const bcm_field_qset_t *qset_1,
        const bcm_field_qset_t *qset_2);

extern int
bcmint_field_oper_mode_status_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int *mode);
extern int
bcmint_field_group_oper_remove_all(
    int unit,
    bcmint_field_stage_info_t *stage_info);
extern int
bcmint_field_entry_oper_remove_all(
    int unit,
    bcmint_field_stage_info_t *stage_info);
extern int
bcmint_field_presel_oper_remove_all(
    int unit,
    bcmint_field_stage_info_t *stage_info);
extern int
bcmint_field_group_strength_reinit(
    int unit,
    bcmint_field_stage_info_t *stage_info);

extern bcmi_ltsw_field_db_t bcmi_ltsw_field_db[BCM_MAX_NUM_UNITS];

/*! Field mutex lock. */
#define FP_LOCK(unit) \
    do { \
        if (bcmi_ltsw_field_db[unit].lock) { \
            sal_mutex_take(bcmi_ltsw_field_db[unit].lock, SAL_MUTEX_FOREVER); \
        } \
    } while(0)

/*! Field mutex unlock. */
#define FP_UNLOCK(unit) \
    do { \
        if (bcmi_ltsw_field_db[unit].lock) { \
            sal_mutex_give(bcmi_ltsw_field_db[unit].lock); \
        } \
    } while(0)

extern int
bcmint_field_stage_enum_info_get(
        int unit,
        bcm_field_stage_t stage,
        bcmint_field_stage_info_t **stage_info);

extern int
bcmint_ltsw_field_source_class_mode_get(
        int unit,
        bcm_field_stage_t stage_id,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_mode_t *mode);

extern int
bcmint_ltsw_field_source_class_mode_set(
        int unit,
        bcm_field_stage_t stage_id,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_mode_t mode);

extern int
bcmint_ltsw_field_qualify_MixedSrcClassId(
        int unit,
        bcm_field_entry_t entry,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_t data,
        bcm_field_src_class_t mask);

extern int
bcmint_ltsw_field_qualify_MixedSrcClassId_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_t *data,
        bcm_field_src_class_t *mask);

extern int
bcmint_field_action_internal_map_set(int unit,
        uint8_t set_int_val,
        bcm_field_action_t action,
        bcmlt_entry_handle_t action_template_hdl,
        bcmlt_entry_handle_t action_lookup_template_hdl,
        bcmint_field_stage_info_t *stage_info,
        const bcm_field_action_map_info_t *map,
        uint32_t start_idx,
        uint32_t num_elem,
        bool map_val_valid,
        uint32_t *map_val);

extern int
bcmint_field_group_action_strength_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_prio,
    bcm_field_action_t action,
    uint32_t *strength);

extern int
bcmint_field_group_policer_pool_alloc(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int group_pipe,
    int group_priority,
    int *pool_id);

extern int
bcmint_field_group_policer_pool_free(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int pool_id);
extern int
bcmint_field_group_pipe_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    int *pipe);

extern int
bcmint_field_comp_oper_mode_status_get(
    int unit,
    bcmint_field_control_info_t *fp_control,
    int *mode);

extern int
bcmint_field_presel_action_get(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_action_t action,
                                    bcm_field_action_params_t *params);

extern int
bcmint_field_presel_oper_lookup(
    int unit,
    int presel_entry_id,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_presel_oper_t **presel_oper);

extern int
bcmint_field_group_primary_slice_id_get(
        int unit,
        bcm_field_group_t group,
        uint32_t *slice_id);

extern int
bcmint_field_flexctr_group_object_get(int unit,
        bcm_field_group_t group,
        uint32_t *flex_ctr_obj_id);
extern int
bcmint_field_group_entry_strength_update(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *grp_oper,
    int curr_grp_prio);
extern int
bcmint_field_group_strength_prof_create(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    int hintid,
    int policer_pool_id,
    bcm_field_aset_t *aset,
    uint64_t *strength_prof_id,
    int *color_sbr_idx);
extern int
bcmint_field_group_strength_prof_destroy(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    uint32 strength_prof_id,
    int color_sbr_idx);
extern int
bcmint_field_action_strength_aset_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_aset_t *aset,
    bcmi_field_aset_t *sbr_aset,
    uint8_t *a_cnt,
    bcmi_field_aset_t *sbr_color_aset,
    uint8_t *color_a_cnt);
extern int
bcmint_field_entry_strength_aset_create(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *grp_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_field_aset_t *new_aset,
    bool install);
extern int
bcmint_field_action_color_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action,
    bcm_field_action_params_t *params);
extern int
bcmint_field_action_color_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action,
    uint64_t *param);
extern int
bcmint_field_action_color_pdd_sbr_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action);
extern int
bcmint_field_color_profile_id_alloc(
    int unit,
    char *tbl_name,
    char *key_name,
    char *pipe_name,
    bcmlt_entry_handle_t entry_hdl,
    int pipe,
    int pool,
    int max_pools,
    bcmi_field_ha_profile_info_t *profile_ptr,
    int curr_id,
    int *alloc_id,
    bool *insert);
extern int
bcmint_field_color_profile_id_free(
    int unit,
    char *tbl_name,
    int pipe,
    bcmi_field_ha_profile_info_t *profile_ptr,
    int free_id,
    int pool,
    int max_pools,
    bool *delete);
extern int
bcmint_field_action_color_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action);
extern int
bcmint_field_action_color_pdd_sbr_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action);
extern int
bcmint_field_qual_internal_map_set(int unit,
                                bool group,
                                uint8_t set_int_val,
                                bcm_field_qualify_t qual,
                                bcmlt_entry_handle_t qual_template_hdl,
                                bcmlt_entry_handle_t qual_template_lkup_hdl,
                                bcmint_field_stage_info_t *stage_info,
                                const bcm_field_qual_map_info_t *map,
                                uint32_t start_idx,
                                uint32_t num_elem);

extern int
bcmi_field_flex_counter_info_get(
    int unit,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmint_field_stage_info_t *stage_info,
    uint64_t *counter_id,
    uint64_t *gp_counter_id,
    uint64_t *rp_counter_id,
    uint64_t *yp_counter_id);

extern int
bcmint_field_color_action_mode_get(int unit,
                                   bcmint_field_stage_info_t *stage_info,
                                   int *mode);
extern int
bcmint_field_color_action_pipe_get(int unit,
                                   bcmint_field_stage_info_t *stage_info,
                                   bcm_field_group_t group,
                                   int *pipe);
extern int
bcmint_field_stage_aset_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_aset_t *aset);
extern int
bcmint_field_action_cont_info_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_action_t action,
    uint8_t max_ct,
    uint16_t *cont_id_arr,
    uint16_t *cont_size_arr,
    uint8_t *actual_ct,
    uint8_t *action_size);
extern int
bcmint_field_presel_entry_prio_set(
    int unit,
    bcm_field_entry_t entry, int prio);
extern int
bcmint_field_hints_destroy_all(int unit);
extern int
bcmint_field_presel_entry_destroy_all(
    int unit,
    bcmint_field_stage_info_t *stage_info);
extern int
bcmint_field_flexctr_valid_action_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int hintid,
    bcmi_field_aset_t *aset,
    int max_action_cnt,
    bcmi_field_action_t *action_arr,
    int *action_cnt);
extern int
bcmint_field_entry_strength_action_update(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_field_action_t action,
    const bcm_field_action_map_t *lt_map_ref,
    bool add);
extern int
bcmint_field_presel_qset_get(
    int unit,
    bcmi_ltsw_field_stage_t stage_id,
    int presel_id,
    bool presel_grp_info,
    bcm_field_qset_t *pqset);

extern int
bcmint_presel_qual_validate(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int presel_grp_id,
    int presel_entry_id);
#endif /* BCMINT_LTSW_FIELD_INT_H */
