/*! \file field.h
 *
 * Field header file.
 * This file contains the management for Field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_FIELD_H
#define BCMI_LTSW_FIELD_H

#include <bcm/field.h>
#include <bcm_int/ltsw/flexctr.h>

/******************************************************************************
 * Defines
 */
/*! Invalid field action. */
#define BCMI_LTSW_FIELD_ACTION_INVALID  (0)

/*! Divison factor to caliculate the depth of hash table for
 * entries.
 */
#define BCMI_LTSW_FIELD_HASH_DIV_FACTOR (4)

/* Flags for field qualifier type. */
#define BCM_FIELD_F_ENUM    0x1
#define BCM_FIELD_F_STRUCT  0x2
#define BCM_FIELD_F_ARRAY   0x4
#define BCM_FIELD_F_BMP     0x8

/* Maximum LT field offset/width map */
#define BCM_FIELD_QUAL_LT_FIELD_MAP_MAX     4

/*! Maximum number of color action table pools. */
#define BCM_FIELD_COLOR_ACTION_POOL_MAX     (8)

/*! Macros to map the .em field of qualifier map */
#define BCMI_FIELD_STAGE_LTMAP_ALL           0x00
#define BCMI_FIELD_STAGE_LTMAP_IFP_ONLY      0x01
#define BCMI_FIELD_STAGE_LTMAP_EM_ONLY       0x02
#define BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD 0x04

/*
 * Internal version of qset add.
 */
#define _BCMI_FIELD_BITS_OP(_a, _b, _max, _op)                        \
        {                                                        \
           int _ct;                                              \
           int _max_w = (_max + SHR_BITWID - 1)/SHR_BITWID;      \
           for (_ct = 0; _ct < (_max_w); _ct++) {                \
               (_a)[_ct] _op (_b)[_ct];                          \
           }                                                     \
        }
#define BCMI_FIELD_QSET_ADD(qset, q)     SHR_BITSET(((qset).w), (q))
#define BCMI_FIELD_QSET_REMOVE(qset, q)  SHR_BITCLR(((qset).w), (q))
#define BCMI_FIELD_QSET_TEST(qset, q)    SHR_BITGET(((qset).w), (q))
#define BCMI_FIELD_QSET_OR(_qset, _qset_new, _max_count)  \
        _BCMI_FIELD_BITS_OP(_qset.w, _qset_new.w, _max_count, |=)
#define BCMI_FIELD_ASET_OR(_aset, _aset_new, _max_count)  \
        _BCMI_FIELD_BITS_OP(_aset.w, _aset_new.w, _max_count, |=)

#define BCMI_FIELD_STAGE_STRINGS \
    {"Ingress",            \
     "Egress",             \
     "Lookup",             \
     "ExactMatch",         \
     "Flowtracker"}
/******************************************************************************
 * LTSW field enumerations
 */

/*!
 * \brief Field stages.
 */
typedef enum bcmi_ltsw_field_stage_s {

    /*! IFP. */
    bcmiFieldStageIngress = 0,

    /*! EFP. */
    bcmiFieldStageEgress = 1,

    /*! VFP. */
    bcmiFieldStageVlan = 2,

    /*! EM. */
    bcmiFieldStageExactMatch = 3,

    /*! Flow Tracker. */
    bcmiFieldStageFlowTracker = 4,

    /*! Always last one. */
    bcmiFieldStageCount = 5

} bcmi_ltsw_field_stage_t;

/*!
 * \brief Field operational modes.
 */
typedef enum bcmi_ltsw_field_oper_mode_s {

    /* Operation mode - Global. */
    bcmiFieldOperModeGlobal = 0,

    /* Operation mode - Pipe Unique. */
    bcmiFieldOperModePipeUnique = 1,

    /* Operation mode - Global Pipe Aware. */
    bcmiFieldOperModeGlobalPipeAware = 2,

    /*! Always last one. */
    bcmiFieldOperModeCount = 3

} bcmi_ltsw_field_oper_mode_t;

/* Enum for qualifier datatypes. */
typedef enum bcm_qual_data_type_e {
    BCM_QUAL_DATA_TYPE_INT,
    BCM_QUAL_DATA_TYPE_UINT64,
    BCM_QUAL_DATA_TYPE_UINT32,
    BCM_QUAL_DATA_TYPE_UINT16,
    BCM_QUAL_DATA_TYPE_UINT8,
    BCM_QUAL_DATA_TYPE_MAC,
    BCM_QUAL_DATA_TYPE_IP,
    BCM_QUAL_DATA_TYPE_IP6,
    BCM_QUAL_DATA_TYPE_PBMP,
    BCM_QUAL_DATA_TYPE_PORT,
    BCM_QUAL_DATA_TYPE_GPORT,
    BCM_QUAL_DATA_TYPE_L4_PORT,
    BCM_QUAL_DATA_TYPE_TRUNK,
    BCM_QUAL_DATA_TYPE_VLAN,
    BCM_QUAL_DATA_TYPE_VPN,
    BCM_QUAL_DATA_TYPE_IF,
    BCM_QUAL_DATA_TYPE_IF_GROUP,
    BCM_QUAL_DATA_TYPE_VRF,
    BCM_QUAL_DATA_TYPE_COS,
    BCM_QUAL_DATA_TYPE_MULTICAST,
    BCM_QUAL_DATA_TYPE_MPLS_LABEL,
    BCM_QUAL_DATA_TYPE_NAT_ID,
    BCM_QUAL_DATA_TYPE_CLASS,
    BCM_QUAL_DATA_TYPE_TRILL_NAME,
    BCM_QUAL_DATA_TYPE_CUSTOM,
    BCM_QUAL_DATA_TYPE_STG,
    BCM_QUAL_DATA_TYPE_INVALID,
    BCM_QUAL_DATA_TYPE_LLC,
    BCM_QUAL_DATA_TYPE_SNAP,
} bcm_qual_data_type_t;

/* Internal qualifiers. */
typedef enum bcmi_field_qualify_e {
    bcmiFieldQualifyB4Chunk0 = bcmFieldQualifyCount,
                                       /* [00] 1-byte chunk qualifier 0.  */
    bcmiFieldQualifyB4Chunk1,          /* [01] 1-byte chunk qualifier 1.  */
    bcmiFieldQualifyB4Chunk2,          /* [02] 1-byte chunk qualifier 2.  */
    bcmiFieldQualifyB4Chunk3,          /* [03] 1-byte chunk qualifier 3.  */
    bcmiFieldQualifyB4Chunk4,          /* [04] 1-byte chunk qualifier 4.  */
    bcmiFieldQualifyB4Chunk5,          /* [05] 1-byte chunk qualifier 5.  */
    bcmiFieldQualifyB4Chunk6,          /* [06] 1-byte chunk qualifier 6.  */
    bcmiFieldQualifyB4Chunk7,          /* [07] 1-byte chunk qualifier 7.  */
    bcmiFieldQualifyB4Chunk8,          /* [08] 1-byte chunk qualifier 8.  */
    bcmiFieldQualifyB4Chunk9,          /* [09] 1-byte chunk qualifier 9.  */
    bcmiFieldQualifyB4Chunk10,         /* [10] 1-byte chunk qualifier 10. */
    bcmiFieldQualifyB4Chunk11,         /* [11] 1-byte chunk qualifier 11. */
    bcmiFieldQualifyB4Chunk12,         /* [12] 1-byte chunk qualifier 12. */
    bcmiFieldQualifyB4Chunk13,         /* [13] 1-byte chunk qualifier 13. */
    bcmiFieldQualifyB4Chunk14,         /* [14] 1-byte chunk qualifier 14. */
    bcmiFieldQualifyB4Chunk15,         /* [15] 1-byte chunk qualifier 15. */
    bcmiFieldQualifyB4Chunk16,         /* [16] 1-byte chunk qualifier 16. */
    bcmiFieldQualifyB2Chunk0,          /* [17] 2-byte chunk qualifier 0.  */
    bcmiFieldQualifyB2Chunk1,          /* [18] 2-byte chunk qualifier 1.  */
    bcmiFieldQualifyB2Chunk2,          /* [19] 2-byte chunk qualifier 2.  */
    bcmiFieldQualifyB2Chunk3,          /* [20] 2-byte chunk qualifier 3.  */
    bcmiFieldQualifyB2Chunk4,          /* [21] 2-byte chunk qualifier 4.  */
    bcmiFieldQualifyB2Chunk5,          /* [22] 2-byte chunk qualifier 5.  */
    bcmiFieldQualifyB2Chunk6,          /* [23] 2-byte chunk qualifier 6.  */
    bcmiFieldQualifyB2Chunk7,          /* [24] 2-byte chunk qualifier 7.  */
    bcmiFieldQualifyB2Chunk8,          /* [25] 2-byte chunk qualifier 8.  */
    bcmiFieldQualifyB2Chunk9,          /* [26] 2-byte chunk qualifier 9.  */
    bcmiFieldQualifyB2Chunk10,         /* [27] 2-byte chunk qualifier 10. */
    bcmiFieldQualifyB2Chunk11,         /* [28] 2-byte chunk qualifier 11. */
    bcmiFieldQualifyB2Chunk12,         /* [29] 2-byte chunk qualifier 12. */
    bcmiFieldQualifyB2Chunk13,         /* [30] 2-byte chunk qualifier 13. */
    bcmiFieldQualifyB2Chunk14,         /* [31] 2-byte chunk qualifier 14. */
    bcmiFieldQualifyB2Chunk15,         /* [32] 2-byte chunk qualifier 15. */
    bcmiFieldQualifyB2Chunk16,         /* [33] 2-byte chunk qualifier 16. */
    bcmiFieldQualifyB1Chunk0,          /* [34] 4-byte chunk qualifier 0.  */
    bcmiFieldQualifyB1Chunk1,          /* [35] 4-byte chunk qualifier 1.  */
    bcmiFieldQualifyB1Chunk2,          /* [36] 4-byte chunk qualifier 2.  */
    bcmiFieldQualifyB1Chunk3,          /* [37] 4-byte chunk qualifier 3.  */
    bcmiFieldQualifyB1Chunk4,          /* [38] 4-byte chunk qualifier 4.  */
    bcmiFieldQualifyB1Chunk5,          /* [39] 4-byte chunk qualifier 5.  */
    bcmiFieldQualifyB1Chunk6,          /* [40] 4-byte chunk qualifier 6.  */
    bcmiFieldQualifyB1Chunk7,          /* [41] 4-byte chunk qualifier 7.  */
    bcmiFieldQualifyB1Chunk8,          /* [42] 4-byte chunk qualifier 8.  */
    bcmiFieldQualifyB1Chunk9,          /* [43] 4-byte chunk qualifier 9.  */
    bcmiFieldQualifyB1Chunk10,         /* [44] 4-byte chunk qualifier 10. */
    bcmiFieldQualifyB1Chunk11,         /* [45] 4-byte chunk qualifier 11. */
    bcmiFieldQualifyB1Chunk12,         /* [46] 4-byte chunk qualifier 12. */
    bcmiFieldQualifyB1Chunk13,         /* [47] 4-byte chunk qualifier 13. */
    bcmiFieldQualifyB1Chunk14,         /* [48] 4-byte chunk qualifier 14. */
    bcmiFieldQualifyB1Chunk15,         /* [49] 4-byte chunk qualifier 15. */
    bcmiFieldQualifyB1Chunk16,         /* [50] 4-byte chunk qualifier 16. */
    bcmiFieldQualifyUdfExtOuter,       /* [51] UDF Extraction Outer Payload. */
    bcmiFieldQualifyUdfExtInner,       /* [52] UDF Extraction Inner Payload. */
    bcmiFieldQualifyLastCount,         /* [53] Last Count                 */
} bcmi_field_qualify_t;

/* Internal qualifiers. */
#define _BCMI_FIELD_QUALIFY_STRINGS \
{ \
    "B4Chunk0", \
    "B4Chunk1", \
    "B4Chunk2", \
    "B4Chunk3", \
    "B4Chunk4", \
    "B4Chunk5", \
    "B4Chunk6", \
    "B4Chunk7", \
    "B4Chunk8", \
    "B4Chunk9", \
    "B4Chunk10", \
    "B4Chunk11", \
    "B4Chunk12", \
    "B4Chunk13", \
    "B4Chunk14", \
    "B4Chunk15", \
    "B4Chunk16", \
    "B2Chunk0", \
    "B2Chunk1", \
    "B2Chunk2", \
    "B2Chunk3", \
    "B2Chunk4", \
    "B2Chunk5", \
    "B2Chunk6", \
    "B2Chunk7", \
    "B2Chunk8", \
    "B2Chunk9", \
    "B2Chunk10", \
    "B2Chunk11", \
    "B2Chunk12", \
    "B2Chunk13", \
    "B2Chunk14", \
    "B2Chunk15", \
    "B2Chunk16", \
    "B1Chunk0", \
    "B1Chunk1", \
    "B1Chunk2", \
    "B1Chunk3", \
    "B1Chunk4", \
    "B1Chunk5", \
    "B1Chunk6", \
    "B1Chunk7", \
    "B1Chunk8", \
    "B1Chunk9", \
    "B1Chunk10", \
    "B1Chunk11", \
    "B1Chunk12", \
    "B1Chunk13", \
    "B1Chunk14", \
    "B1Chunk15", \
    "B1Chunk16", \
    "UdfExtOuter", \
    "UdfExtInner" \
}

/* Internal Actions. */
typedef enum bcmi_field_action_e {
    bcmiFieldActionFlexctrOpaque0 = bcmFieldActionCount,
    bcmiFieldActionFlexctrOpaque1,
    bcmiFieldActionFlexctrOpaque2,
    bcmiFieldActionFlexctrOpaque3,
    bcmiFieldActionFlexctrOpaque4,
    bcmiFieldActionFlexctrOpaque5,
    bcmiFieldActionFlexctrOpaque6,
    bcmiFieldActionFlexctrOpaque7,
    bcmiFieldActionFlexctrOpaque8,
    bcmiFieldActionFlexctrOpaque9,
    bcmiFieldActionFlexctrOpaque10,
    bcmiFieldActionCount
} bcmi_field_action_t;

#define BCMI_FIELD_MAX_ACTIONS_PER_DB 16
/*
 * Internal Action ASET
 */
typedef struct bcmi_field_aset_s {
    uint32_t w[_SHR_BITDCLSIZE(bcmiFieldActionCount)];
    bcm_field_action_width_t actions_width[BCMI_FIELD_MAX_ACTIONS_PER_DB];
} bcmi_field_aset_t;

#define BCMI_FIELD_ASET_INIT(aset)  \
    sal_memset(&(aset), 0, sizeof(bcmi_field_aset_t)) 

/******************************************************************************
 * LTSW field structures
 */

/*!
 * \brief Qual field information.
 *
 * This data structure is used to identify the qualifier, its type and value and mask.
 */
typedef struct bcm_qual_field_s {
    /*! Field name. */
    bcm_field_qualify_t     sdk6_qual_enum;

    /*! Field attribute (ref BCM_FIELD_F_xxx). */
    uint16_t        flags;

    /*! To indicate max index in an array. */
    uint16_t        depth;

    /*! Qualifier data type. To represent different
     * enum types supported in FP like mac_t, IPType etc
     */
    bcm_qual_data_type_t data_type;

    /*! Qualifier data portion */
    void *qual_data;

    /*! Qualifier mask portion */
    void *qual_mask;

    /*! Enum value in string format. */
    const char *enum_name;

    /*! Enum value in number format. */
    uint32_t enum_value;

    /*! Mask for enum value in number format. */
    uint32_t enum_mask;
} bcm_qual_field_t;

/*!
 * \brief Qual field information values.
 *
 * This data structure is used to retreive supported values for a particular qualifier.
 */
typedef struct bcm_qual_field_values_s {

    /* Macro Flags supported if qualifier is of macro type. */
    uint32 macro_flags[20];

    /* Macro Flag values supported if qualifier is of macro type. */
    uint8 macro_flags_value[20];

    /* Macro Flags mask values supported if qualifier is of macro type. */
    uint8 macro_flags_mask_value[20];

    /* Macro Flags mask width supported if qualifier is of macro type. */
    uint8 macro_flags_mask_width[20];

    /* Number of values filled. */
    uint8 num;
} bcm_qual_field_values_t;

/*!
 * \brief Field action internal logical field information.
 *
 * This data structure is used to map the SDK6 action with
 * the corresponding SDKLT internal/additional logical field.
 */
typedef struct bcm_field_qual_internal_map_info_s {
    /*! Additional Logical field name corresponding to the qualifier. */
    const char      *lt_field_name;

    /*! Number of offsets within the logical field
     *  that additional info is mapped to. */
    uint8_t        num_offsets;

    /*! Offset of the additional info in the corresponding logical field. */
    uint16_t        offset[BCM_FIELD_QUAL_LT_FIELD_MAP_MAX];

    /*! Width occupied by the additional info in the
     *  corresponding logical field. */
    uint16_t        width[BCM_FIELD_QUAL_LT_FIELD_MAP_MAX];

    /*! Additional info Value */
    const void       *value;
} bcm_field_qual_internal_map_info_t;

/*!
 * \brief Field qualifier logical field information.
 *
 * This data structure is used to map the SDK6 qualifier with
 * the corresponding SDKLT logical field.
 */
typedef struct bcm_field_qual_map_info_s {
    /*! Logical field name corresponding to the qualifier. */
    const char      *lt_field_name;

    /*! Additional logical field name corresponding to the qualifier
     *  in rule template table. If set to NULL, use name above. */
    const char   *lt_field_mask_name;

    /*! Additional logical field name corresponding to the qualifier
     *  in group template table. If set to NULL, use name above. */
    const char      *lt_group_field_name;

    /*! Number of offsets within the logical field
     * that qualifier is mapped to. */
    uint8_t        num_offsets;

    /*! Offset of the qualifier in the corresponding logical field. */
    uint16_t        offset[BCM_FIELD_QUAL_LT_FIELD_MAP_MAX];

    /*! Width occubpied by the qualifier in the corresponding logical field. */
    uint16_t        width[BCM_FIELD_QUAL_LT_FIELD_MAP_MAX];

    /*! Qualifier supported in Exact Match
     *  Possible values:
     *   BCMI_FIELD_STAGE_LTMAP_ALL      => Support all stages.
     *   BCMI_FIELD_STAGE_LTMAP_IFP_ONLY => Support IFP only.
     *   BCMI_FIELD_STAGE_LTMAP_EM_ONLY  => Support EM only.
     */
    uint8_t        em;

    /*! Additional info enum/Macro value needed for
     *  ENUM/MACRO fo SDK6 mapped to SDKLT qualifier.
     */
    uint32_t        value;
    /*! Number of internal additional maps */
    uint8_t         num_internal_maps;

    /*! Additional Field information */
    const bcm_field_qual_internal_map_info_t *internal_map;

} bcm_field_qual_map_info_t;

typedef enum bcmi_field_data_type_e {
    bcmiFieldDataTypeNone = 0,
    bcmFieldDataTypeEnumtoLtField = 1,
    bcmFieldDataTypeMacrotoLtField = 2
} bcmi_field_data_type_t;

/*!
 * \brief Field qualifier map information.
 *
 * This data structure is used to map the SDK6 qualifier with
 * the corresponding SDKLT logical field.
 */
typedef struct bcm_field_qual_map_s {
    /*! Field name. */
    bcmi_field_qualify_t     qual;

    /*! Number of logical fields that the qualifier is mapped to. */
    uint32_t        num_maps;

    /*! Flag indicating if more than one qualifier share the LT field */
    uint8_t        flags;

    bcmi_field_data_type_t data_type;
    /*! List of logical fields that the qualifier is mapped to. */
    bcm_field_qual_map_info_t     *map;
} bcm_field_qual_map_t;

/*!
 * \brief Field action internal logical field information.
 *
 * This data structure is used to map the SDK6 action with
 * the corresponding SDKLT internal/additional logical field.
 */
typedef struct bcm_field_action_internal_map_info_s {
    /*! Additional Logical field name corresponding to the action. */
    const char      *lt_field_name;

    /*! Additional logical field name corresponding to the action
     *  in PDD template table. If set to NULL, use name above. */
    const char      *lt_pdd_field_name;

    /*! Additional logical field name corresponding to the action
     *  in SBR template table. If set to NULL, use name above. */
    const char      *lt_sbr_field_name;

    /*! SBR configuration */
    bool            sbr_enabled;

    /*! Number of offsets within the logical field
     *  that additional info is mapped to. */
    uint8_t        num_offsets;

    /*! Offset of the additional info in the corresponding logical field. */
    uint16_t        offset[BCM_FIELD_QUAL_LT_FIELD_MAP_MAX];

    /*! Width occupied by the additional info in the
     *  corresponding logical field. */
    uint16_t        width[BCM_FIELD_QUAL_LT_FIELD_MAP_MAX];

    /*! Additional info Value */
    uint32_t        value;
} bcm_field_action_internal_map_info_t;

#if 0
/* Internal 1-byte UDF chunk qualifiers. */
typedef enum bcmi_field_qual_b1_chunk_e {
    bcmiFieldQualB4Chunk0 = 0,      /* [00] 1-byte chunk qualifier 0.  */
    bcmiFieldQualB4Chunk1,          /* [01] 1-byte chunk qualifier 1.  */
    bcmiFieldQualB4Chunk2,          /* [02] 1-byte chunk qualifier 2.  */
    bcmiFieldQualB4Chunk3,          /* [03] 1-byte chunk qualifier 3.  */
    bcmiFieldQualB4Chunk4,          /* [04] 1-byte chunk qualifier 4.  */
    bcmiFieldQualB4Chunk5,          /* [05] 1-byte chunk qualifier 5.  */
    bcmiFieldQualB4Chunk6,          /* [06] 1-byte chunk qualifier 6.  */
    bcmiFieldQualB4Chunk7,          /* [07] 1-byte chunk qualifier 7.  */
    bcmiFieldQualB4Chunk8,          /* [08] 1-byte chunk qualifier 8.  */
    bcmiFieldQualB4Chunk9,          /* [09] 1-byte chunk qualifier 9.  */
    bcmiFieldQualB4Chunk10,         /* [10] 1-byte chunk qualifier 10. */
    bcmiFieldQualB4Chunk11,         /* [11] 1-byte chunk qualifier 11. */
    bcmiFieldQualB4Chunk12,         /* [12] 1-byte chunk qualifier 12. */
    bcmiFieldQualB4Chunk13,         /* [13] 1-byte chunk qualifier 13. */
    bcmiFieldQualB4Chunk14,         /* [14] 1-byte chunk qualifier 14. */
    bcmiFieldQualB4Chunk15,         /* [15] 1-byte chunk qualifier 15. */
    bcmiFieldQualB4Chunk16,         /* [16] 1-byte chunk qualifier 16. */
    bcmiFieldQualB4ChunkLastCount,  /* [17] Last Count                 */
} bcmi_field_qual_b1_chunk_t;

/* Internal 2-byte UDF chunk qualifiers. */
typedef enum bcmi_field_qual_b2_chunk_e {
    bcmiFieldQualB2Chunk0 = 0,      /* [00] 2-byte chunk qualifier 0.  */
    bcmiFieldQualB2Chunk1,          /* [01] 2-byte chunk qualifier 1.  */
    bcmiFieldQualB2Chunk2,          /* [02] 2-byte chunk qualifier 2.  */
    bcmiFieldQualB2Chunk3,          /* [03] 2-byte chunk qualifier 3.  */
    bcmiFieldQualB2Chunk4,          /* [04] 2-byte chunk qualifier 4.  */
    bcmiFieldQualB2Chunk5,          /* [05] 2-byte chunk qualifier 5.  */
    bcmiFieldQualB2Chunk6,          /* [06] 2-byte chunk qualifier 6.  */
    bcmiFieldQualB2Chunk7,          /* [07] 2-byte chunk qualifier 7.  */
    bcmiFieldQualB2Chunk8,          /* [08] 2-byte chunk qualifier 8.  */
    bcmiFieldQualB2Chunk9,          /* [09] 2-byte chunk qualifier 9.  */
    bcmiFieldQualB2Chunk10,         /* [10] 2-byte chunk qualifier 10. */
    bcmiFieldQualB2Chunk11,         /* [11] 2-byte chunk qualifier 11. */
    bcmiFieldQualB2Chunk12,         /* [12] 2-byte chunk qualifier 12. */
    bcmiFieldQualB2Chunk13,         /* [13] 2-byte chunk qualifier 13. */
    bcmiFieldQualB2Chunk14,         /* [14] 2-byte chunk qualifier 14. */
    bcmiFieldQualB2Chunk15,         /* [15] 2-byte chunk qualifier 15. */
    bcmiFieldQualB2Chunk16,         /* [16] 2-byte chunk qualifier 16. */
    bcmiFieldQualB2ChunkLastCount,  /* [17] Last Count                 */
} bcmi_field_qual_b2_chunk_t;

/* Internal 4-byte UDF chunk qualifiers. */
typedef enum bcmi_field_qual_b4_chunk_e {
    bcmiFieldQualB4Chunk0 = 0,      /* [00] 4-byte chunk qualifier 0.  */
    bcmiFieldQualB4Chunk1,          /* [01] 4-byte chunk qualifier 1.  */
    bcmiFieldQualB4Chunk2,          /* [02] 4-byte chunk qualifier 2.  */
    bcmiFieldQualB4Chunk3,          /* [03] 4-byte chunk qualifier 3.  */
    bcmiFieldQualB4Chunk4,          /* [04] 4-byte chunk qualifier 4.  */
    bcmiFieldQualB4Chunk5,          /* [05] 4-byte chunk qualifier 5.  */
    bcmiFieldQualB4Chunk6,          /* [06] 4-byte chunk qualifier 6.  */
    bcmiFieldQualB4Chunk7,          /* [07] 4-byte chunk qualifier 7.  */
    bcmiFieldQualB4Chunk8,          /* [08] 4-byte chunk qualifier 8.  */
    bcmiFieldQualB4Chunk9,          /* [09] 4-byte chunk qualifier 9.  */
    bcmiFieldQualB4Chunk10,         /* [10] 4-byte chunk qualifier 10. */
    bcmiFieldQualB4Chunk11,         /* [11] 4-byte chunk qualifier 11. */
    bcmiFieldQualB4Chunk12,         /* [12] 4-byte chunk qualifier 12. */
    bcmiFieldQualB4Chunk13,         /* [13] 4-byte chunk qualifier 13. */
    bcmiFieldQualB4Chunk14,         /* [14] 4-byte chunk qualifier 14. */
    bcmiFieldQualB4Chunk15,         /* [15] 4-byte chunk qualifier 15. */
    bcmiFieldQualB4Chunk16,         /* [16] 4-byte chunk qualifier 16. */
    bcmiFieldQualB4ChunkLastCount,  /* [17] Last Count                 */
} bcmi_field_qual_b4_chunk_t;
#endif

/* Enum to indicate param argument of field API */
typedef enum bcmi_field_action_param_e {
    bcmiFieldActionParamNone = 0,
    bcmiFieldActionParam1 = 1,
} bcmi_field_action_param_t;

/*!
 * \brief Field action logical field information.
 *
 * This data structure is used to map the SDK6 action with
 * the corresponding SDKLT logical field.
 */
typedef struct bcm_field_action_map_info_s {
    /*! Logical field name corresponding to the action. */
    const char      *lt_field_name;

    /*! Additional logical field name corresponding to the action
     *  in PDD template table. If set to NULL, use name above. */
    const char      *lt_pdd_field_name;

    /*! Additional logical field name corresponding to the action
     *  in SBR template table. If set to NULL, use name above. */
    const char      *lt_sbr_field_name;

    /*! Number of offsets within the logical field
     * that action is mapped to. */
    uint8_t         num_offsets;

    /*! Offset of the action in the corresponding logical field. */
    uint16_t        offset[BCM_FIELD_QUAL_LT_FIELD_MAP_MAX];

    /*! Width occubpied by the action in the corresponding logical field. */
    uint16_t        width[BCM_FIELD_QUAL_LT_FIELD_MAP_MAX];

    /*! SBR configuration */
    bool            sbr_enabled;

    /*! Action supported in Exact Match
     *  Possible values:
     *   BCMI_FIELD_STAGE_LTMAP_ALL      => Support all stages.
     *   BCMI_FIELD_STAGE_LTMAP_IFP_ONLY => support IFP only.
     *   BCMI_FIELD_STAGE_LTMAP_EM_ONLY  => Support EM only.
     */
    uint8_t         em;

    /*! Action is Policer action. */
    bool            policer;

    /*! Action is Policer action and this is a Policer PDD field. */
    bool            policer_pdd;

    /*! Pool number of color table index. */
    int             color_index;

    /*! PDD SBR index. */
    int             pdd_sbr_index;

    /*! Number of internal additional maps */
    uint8_t         num_internal_maps;

    /*! Additional Field information */
    const bcm_field_action_internal_map_info_t *internal_map;

    /*! Param argument of bcm API */
    bcmi_field_action_param_t param;
} bcm_field_action_map_info_t;

/*!
 * \brief Field action map information.
 *
 * This data structure is used to map the SDK6 action with
 * the corresponding SDKLT logical field.
 */
typedef struct bcm_field_action_map_s {
    /*! Field name. */
    bcm_field_action_t     action;

    /*! Number of logical fields that the action is mapped to. */
    uint32_t        num_maps;

    /*! Flag indicating if more than one actions share the LT field */
    uint8_t        flags;

    /*! List of logical fields that the action is mapped to. */
    const bcm_field_action_map_info_t     *map;
} bcm_field_action_map_t;

/*! Set function for Field Control which has complex settings. */
typedef int (*bcm_field_control_set_f)(int unit, uint32 state);

/*! Get function for Field Control which has complex settings. */
typedef int (*bcm_field_control_get_f)(int unit, uint32 *state);

/*!
 * \brief Field control logical field information.
 *
 * This data structure is used to store the SDKLT logical field info.
 */
typedef struct bcm_field_control_map_info_s {
    /*! Logical table name corresponding to the Field control. */
    const char      *lt_name;

    /*! Logical field name corresponding to the Field Control. */
    const char      *lt_field_name;

    /*! Set function for Field Control which has complex settings. */
    const bcm_field_control_set_f   func_set;

    /*! Get function for Field Control which has complex settings. */
    const bcm_field_control_get_f   func_get;
} bcm_field_control_map_info_t;
/*!
 * \brief Field Control map information.
 *
 * This data structure is used to map the SDK6 control Enum with
 * the corresponding SDKLT logical field.
 */
typedef struct bcm_field_control_map_s {
    /*! Field name. */
    bcm_field_control_t     control;

    /*! Number of logical fields that the Field control is mapped to. */
    uint32_t        num_maps;

    /*! List of logical fields that the Field Control is mapped to. */
    const bcm_field_control_map_info_t     *map;
} bcm_field_control_map_t;


/*
 * This data structure is used by flexctr module, to retrive the group info
 * for a given group ID.
 */ 
typedef struct bcmi_field_flexctr_group_info_s {
    /*! Group's Priority */
    int priority;
    /*! Flex Counter Object ID */
    uint32_t flex_ctr_obj_id;
    /*! Group's Pipe Instance */
    int pipe;
} bcmi_field_flexctr_group_info_t;


/* LT Map action flags */
#define BCMI_FIELD_ACTION_INTERNAL 0x1

/******************************************************************************
 * LTSW Field Module Functions
 */
extern int
bcmi_ltsw_field_qualifier_set(int unit,
                              bcm_field_entry_t entry,
                              bcm_qual_field_t *qual_info);

extern int
bcmi_ltsw_field_qualifier_get(int unit,
                              bcm_field_entry_t entry,
                              bcm_qual_field_t *qual_info);

extern int
bcmi_ltsw_field_mirror_index_get(
    int unit,
    int entry,
    bcm_field_action_t action,
    int mirror_cont,
    bool *enable,
    int *mirror_index);

extern int
bcmi_ltsw_field_destination_mirror_index_get(
    int unit,
    bcm_field_destination_match_t *match,
    int *mirror_index);

extern int
bcmi_ltsw_field_entry_policer_update_all(int unit,
                                         bcm_policer_t policer_id,
                                         uint8_t in_use);

extern int
bcmi_ltsw_field_oper_mode_status_get(int unit,
                                     bcmi_ltsw_field_stage_t stage,
                                     int *mode);
extern int
bcmi_field_flexctr_group_info_get(
    int unit,
    bcm_flexctr_source_t source,
    int group,
    bcmi_field_flexctr_group_info_t *info);

extern int
bcmi_ltsw_field_flex_port_update(int unit,
                            bcm_pbmp_t detached_pbmp,
                            bcm_pbmp_t attached_pbmp);
extern int
bcmi_field_group_dump_brief(int unit, bcm_field_group_t group);

extern int
bcmi_field_presel_dump(int unit, bcm_field_presel_t presel_id);

extern int
bcmi_field_entry_hit_index_get(int unit,
                               bcm_field_entry_t entry,
                               uint32_t *hit_index);
extern int
bcmi_field_group_is_empty(int unit,
                          bcm_field_group_t group,
                          bool *entry_present);
extern int
bcmi_field_group_active_entries_get(int unit,
                                    bcm_field_group_t group,
                                    uint64_t *entries_created);
int
bcmi_field_group_delete_slice_reserve_entry(int unit,
                                            bcm_field_group_t group);
#endif /* BCMI_LTSW_FIELD_H */
