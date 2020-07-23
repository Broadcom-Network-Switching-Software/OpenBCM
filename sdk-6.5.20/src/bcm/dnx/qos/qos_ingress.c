/** \file qos_ingress.c
 *
 *  Qos PHB procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_QOS
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/bslenum.h>
#include <bcm/types.h>
#include <bcm/qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include "qos_internal.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>

/*
 * }
 */

/**
 *  qos map types values for qos context selection
 */
typedef enum
{
    INGRESS_QOS_MAP_TYPE_DEFAULT_PORT,  /* Qos mapping to default PORT */
    INGRESS_QOS_MAP_TYPE_ETH_USE_IPV4,  /* QOS mapping according IPV4 even if packet is ethernet */
    INGRESS_QOS_MAP_TYPE_ETH_USE_IPV6,  /* QOS mapping according IPv6 even if packet is ethernet */
    INGRESS_QOS_MAP_TYPE_ETH_USE_MPLS,  /* QOS mapping according MPLS even if packet is ethernet */
    INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_TWO,        /* qos mapping according to Ethernet both outer and inner tag */
    INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_INNER,      /* qos mapping according to Ethernet inner tag */
    INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_OUTER,      /* qos mapping according to Ethernet outer tag */
    INGRESS_QOS_MAP_TYPE_ETH_ONE_TAG,   /* qos mapping according to Ethernet tag if packet is single tag */
    INGRESS_QOS_MAP_TYPE_ETH_UNTAGGED,  /* QOS mapping according to Default port since packet is Ethernet untagged (no
                                         * recognized VLANs) */
    INGRESS_QOS_MAP_TYPE_RCH,   /* qos mapping accordinig to resycle */
    INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS,        /* qos mapping for two tags packet with PWE tagged double tags */
    INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG,  /* qos mapping for single tag packet with PWE tagged outer tag */
    INGRESS_QOS_MAP_TYPE_MPLS,  /* qos mapping according to MPLS */
    INGRESS_QOS_MAP_TYPE_UNTERM_MPLS,   /* qos mapping according to untermMPLS */
    INGRESS_QOS_MAP_TYPE_IPV4,  /* QOS mapping according to IPv4 TOS */
    INGRESS_QOS_MAP_TYPE_IPV6,  /* QOS mapping according to IPv6 */
    INGRESS_QOS_MAP_TYPE_16,
    INGRESS_QOS_MAP_TYPE_17,
    INGRESS_QOS_MAP_TYPE_18,
    INGRESS_QOS_MAP_TYPE_19,
    INGRESS_QOS_MAP_TYPE_20,
    INGRESS_QOS_MAP_TYPE_21,
    INGRESS_QOS_MAP_TYPE_22,
    INGRESS_QOS_MAP_TYPE_23,
    INGRESS_QOS_MAP_TYPE_24,
    INGRESS_QOS_MAP_TYPE_25,
    INGRESS_QOS_MAP_TYPE_26,
    INGRESS_QOS_MAP_TYPE_27,
    INGRESS_QOS_MAP_TYPE_28,
    INGRESS_QOS_MAP_TYPE_29,
    INGRESS_QOS_MAP_TYPE_30,
    INGRESS_QOS_MAP_TYPE_31,
    INGRESS_QOS_MAP_TYPE_VALUE_NOF
} ingress_qos_map_type_value_e;

/**
 *  we have 32 availiable map types values and 16 ingress map
 *  index. This table is used to convert 32 map types values
 *  to the appropriate map index.
 */
const dbal_enum_value_field_ingress_qos_map_index_e dnx_map_type_to_index[INGRESS_QOS_MAP_TYPE_VALUE_NOF] = {
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_DEFAULT_PORT        */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4,         /**INGRESS_QOS_MAP_TYPE_ETH_USE_IPV4        */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6,         /**INGRESS_QOS_MAP_TYPE_ETH_USE_IPV6        */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS,         /**INGRESS_QOS_MAP_TYPE_ETH_USE_MPLS        */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_TWO_TAGS,   /**INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_TWO            */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_ONE_TAG,   /**INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_INNER          */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_ONE_TAG,   /**INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_OUTER          */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_ONE_TAG,   /**INGRESS_QOS_MAP_TYPE_ETH_ONE_TAG         */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_ETH_UNTAGGED        */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_RCH,          /**INGRESS_QOS_MAP_TYPE_RCH             */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_TWO_TAGS,  /**INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS    */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_ONE_TAG,   /**INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG      */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS,         /**INGRESS_QOS_MAP_TYPE_MPLS            */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS,         /**INGRESS_QOS_MAP_TYPE_UNTERM_MPLS            */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4,         /**INGRESS_QOS_MAP_TYPE_IPV4            */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6,         /**INGRESS_QOS_MAP_TYPE_IPV6            */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_16              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_17              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_18              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_19              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_20              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_21              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_22              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_23              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_24              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_25              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_26              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_27              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_28              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_29              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_30              */
    DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT,      /**INGRESS_QOS_MAP_TYPE_31              */
};

/**
 * TC,DP,nwk_qos, ECN offset in explicit map index
 */
#define DNX_QOS_EXPLICIT_DP_OFFSET  6
#define DNX_QOS_EXPLICIT_TC_OFFSET  3
#define DNX_QOS_EXPLICIT_VAR_OFFSET 0
#define DNX_QOS_EXPLICIT_ECN_OFFSET 2

/**
 *\brief
 * ECN mapping opcode,
 * Only support two ecn mapping profile, default 0 with opcode 0 that ecn is not eligible and
 * profile 1 with opcode 1 that ecn is eligible
 */
#define DNX_QOS_ECN_MAPPING_OPCODE_DEFAULT  0
#define DNX_QOS_ECN_MAPPING_OPCODE_ELIGIBLE 1
/**
 * ECN type
 */
#define DNX_QOS_ECN_NOT_ECT  0  /* ECN is not eligible */
#define DNX_QOS_ECN_ECT_1    1  /* previous node ECN ecligible and works in ECT 1 mode */
#define DNX_QOS_ECN_ECT_0    2  /* previous node ECN ecligible and works in ECT 0 mode */
#define DNX_QOS_ECN_CE       3  /* previous node ECN ecligible and congestion */

/**
 * \brief
 * Internal map key size value
 * use all available bits
 * The map_size indicates the nof bits to stamp from the "map index" on the key to the result memory.
 * it maps values [1:8]
 */
#define QOS_INT_MAP_KEY_SIZE 7

/**
 * \brief
 * Internal map key size value
 * use all available bits
 * The map_size indicates the nof bits to stamp from the "map index" on the key to the result memory.
 * it maps values [1:4]
 */
#define QOS_ECN_INT_MAP_KEY_SIZE 3

/**
 * \brief
 * max number FFC enries in QOS HW
 */
#define QOS_MAX_NUM_FFC 2

/*QOS FFC structure*/
/**opcode: 0-get field from relative header (accoridng to index offset)
           1-get field from header (according to index of layer records)
           2-get field straight from the pbus
           3-take field from layer records according to index"
  index:   either an index of a layer in the packet header or
           an index offset to a layer in the header
*/
typedef struct qos_ffc_s
{
    uint8 opcode;
    uint8 index;
    uint16 field_offset;
    uint8 field_size;
    uint8 key_offset;
} qos_ffc_t;

typedef enum
{
    QOS_UNUSED_FIELD = 0,
    QOS_FFC_0_ETH_PCP_0_VAL,    /* single (outer ) tag pcp */
    QOS_FFC_1_ETH_DEI_0_VAL,    /* single (outer ) tag dei */
    QOS_FFC_0_ETH_PCP_1_VAL,    /* double (inner ) tag pcp */
    QOS_FFC_1_ETH_DEI_1_VAL,    /* double (inner ) tag dei */
    QOS_FFC_0_ETH_PCP_DEI_VAL,  /* double (outer ) tag pcp&dei */
    QOS_FFC_1_ETH_PCP_DEI_VAL,  /* double (inner ) tag pcp&dei */
    QOS_FFC_0_IPV4_TOS_VAL,
    QOS_FFC_1_IPV4_TOS_VAL,
    QOS_FFC_0_IPV6_TOS_VAL,
    QOS_FFC_1_IPV6_TOS_VAL,
    QOS_FFC_0_MPLS_EXP_VAL,
    QOS_FFC_1_MPLS_EXP_VAL,
    QOS_FFC_0_IPV4_TTL_VAL,
    QOS_FFC_1_IPV4_TTL_VAL,
    QOS_FFC_0_IPV6_TTL_VAL,
    QOS_FFC_1_IPV6_TTL_VAL,
    QOS_FFC_0_MPLS_TTL_VAL,
    QOS_FFC_1_MPLS_TTL_VAL,
    QOS_FFC_0_ETH_IPV4_VAL,
    QOS_FFC_1_ETH_IPV4_VAL,
    QOS_FFC_0_ETH_IPV6_VAL,
    QOS_FFC_1_ETH_IPV6_VAL,
    QOS_FFC_0_ETH_MPLS_VAL,
    QOS_FFC_1_ETH_MPLS_VAL,
    QOS_FFC_0_PHB_RCH_VAL,
    QOS_FFC_1_PHB_RCH_VAL,
    QOS_FFC_0_REM_RCH_VAL,
    QOS_FFC_1_REM_RCH_VAL,
    QOS_FFC_0_TTL_RCH_VAL,
    QOS_FFC_1_TTL_RCH_VAL,
    QOS_FFC_0_MPLS_ETH_PCP_0_VAL,
    QOS_FFC_1_MPLS_ETH_DEI_0_VAL,
    QOS_FFC_0_MPLS_ETH_PCP_DEI_VAL,
    QOS_FFC_1_MPLS_ETH_PCP_DEI_VAL,
    QOS_FFC_TYPE_VALUE_NOF
} qos_ffc_type_value_e;

CONST qos_ffc_t qos_ffc_table[QOS_FFC_TYPE_VALUE_NOF] = {
    /**********************************************************************/
    /**  opcode  *  index  *  field_offset  *  field_size  *  key_offset  */
    /**********************************************************************/
        /** QOS_UNUSED_FIELD*/
    {0, 0, 0, 0, 0},
        /** QOS_FFC_0_ETH_PCP_0_VAL*/
    {0, 0, (144 * 8 - 96 - 20), 2, 0},
        /** QOS_FFC_1_ETH_DEI_0_VAL*/
    {0, 0, (144 * 8 - 96 - 20 + 3), 0, 3},
        /** QOS_FFC_0_ETH_PCP_1_VAL*/
    {0, 0, (144 * 8 - 96 - 52), 2, 0},
        /** QOS_FFC_1_ETH_DEI_1_VAL*/
    {0, 0, (144 * 8 - 96 - 52 + 3), 0, 3},
        /** QOS_FFC_0_ETH_PCP_DEI_VAL*/
    {0, 0, (144 * 8 - 96 - 20), 3, 0},
        /** QOS_FFC_1_ETH_PCP_DEI_VAL*/
    {0, 0, (144 * 8 - 96 - 52), 3, 4},
        /** QOS_FFC_0_IPV4_TOS_VAL*/
    {0, 0, (144 * 8 - 16), 3, 0},
        /** QOS_FFC_1_IPV4_TOS_VAL*/
    {0, 0, (144 * 8 - 16 + 4), 3, 4},
        /** QOS_FFC_0_IPV6_TOS_VAL*/
    {0, 0, (144 * 8 - 12), 3, 0},
        /** QOS_FFC_1_IPV6_TOS_VAL*/
    {0, 0, (144 * 8 - 12 + 4), 3, 4},
        /** QOS_FFC_0_MPLS_EXP_VAL*/
    {0, 0, (144 * 8 - 23), 1, 0},
        /** QOS_FFC_1_MPLS_EXP_VAL*/
    {0, 0, (144 * 8 - 23 + 2), 0, 2},
        /** QOS_FFC_0_IPV4_TTL_VAL*/
    {0, 0, (144 * 8 - 72), 3, 0},
        /** QOS_FFC_1_IPV4_TTL_VAL*/
    {0, 0, (144 * 8 - 72 + 4), 3, 4},
        /** QOS_FFC_0_IPV6_TTL_VAL*/
    {0, 0, (144 * 8 - 64), 3, 0},
        /** QOS_FFC_1_IPV6_TTL_VAL*/
    {0, 0, (144 * 8 - 64 + 4), 3, 4},
        /** QOS_FFC_0_MPLS_TTL_VAL*/
    {0, 0, (144 * 8 - 32), 3, 0},
        /** QOS_FFC_1_MPLS_TTL_VAL*/
    {0, 0, (144 * 8 - 32 + 4), 3, 4},
        /** QOS_FFC_0_ETH_IPV4_VAL*/
    {0, 1, (144 * 8 - 16), 3, 0},
        /** QOS_FFC_1_ETH_IPV4_VAL*/
    {0, 1, (144 * 8 - 16 + 4), 3, 4},
        /** QOS_FFC_0_ETH_IPV6_VAL*/
    {0, 1, (144 * 8 - 12), 3, 0},
        /** QOS_FFC_1_ETH_IPV6_VAL*/
    {0, 1, (144 * 8 - 12 + 4), 3, 4},
        /** QOS_FFC_0_ETH_MPLS_VAL*/
    {0, 1, (144 * 8 - 23), 1, 0},
        /** QOS_FFC_1_ETH_MPLS_VAL*/
    {0, 1, (144 * 8 - 23 + 2), 0, 2},
        /** QOS_FFC_0_PHB_RCH_VAL   */
    {0, 0, (144 * 8 - 35), 1, 0},
        /** QOS_FFC_1_PHB_RCH_VAL   */
    {0, 0, (144 * 8 - 40), 2, 2},
        /** QOS_FFC_0_REM_RCH_VAL   */
    {0, 0, (144 * 8 - 104), 3, 0},
        /** QOS_FFC_1_REM_RCH_VAL   */
    {0, 0, (144 * 8 - 104 + 4), 3, 4},
        /** QOS_FFC_0_TTL_RCH_VAL   */
    {0, 0, (144 * 8 - 112), 3, 0},
        /** QOS_FFC_1_TTL_RCH_VAL   */
    {0, 0, (144 * 8 - 112 + 4), 3, 4},
        /** QOS_FFC_0_MPLS_ETH_PCP_0_VAL*/
    {0, 1, (144 * 8 - 96 - 20), 2, 0},
        /** QOS_FFC_1_MPLS_ETH_DEI_0_VAL*/
    {0, 1, (144 * 8 - 96 - 20 + 3), 0, 3},
        /** QOS_FFC_0_MPLS_ETH_PCP_DEI_VAL*/
    {0, 1, (144 * 8 - 96 - 20), 3, 0},
        /** QOS_FFC_1_MPLS_ETH_PCP_DEI_VAL*/
    {0, 1, (144 * 8 - 96 - 52), 3, 4}
};

#define QOS_FFC_VALUE(ffc_entry) \
    ((ffc_entry.opcode << 19) | (ffc_entry.index << 16) | (ffc_entry.field_offset << 5) \
    | (ffc_entry.field_size << 3) | (ffc_entry.key_offset))

#define QOS_RM_PHB_FFC_VALUE(ffc_entry) \
    ((ffc_entry.opcode << 21) | (ffc_entry.index << 18) | (ffc_entry.field_offset << 7) \
    | (ffc_entry.field_size << 4) | (ffc_entry.key_offset))

/**
 * \brief
 * mask value for qos context selection profile
 */
#define QOS_NO_CS_PROFILE_MASK 0
#define QOS_CS_PROFILE_FULL_MASK 0xf

/**
 * \brief ETH QUALUFIER
 * For ETH layer types Qualifier we look at first tpid exist(bit 2)and second tpid exist(bit6)
 * we have 3 ETH Layer types:
 * QOS_ETH_UNTAGGED - for ETH UNTAGGED first tpid exist(bit2)=0 and second tpid exist(bit6)=0
 * QOS_ETH_ONE_TAG  - for ETH ONE TAG  first tpid exist(bit2)=1 and second tpid exist(bit6)=0
 * QOS_ETH_TWO_TAG  - for ETH TWO TAG  first tpid exist(bit2)=1 and second tpid exist(bit6)=1
 * QOS_ETH QUALIFIER MASK
 * for all ETH layer types the QUALIFIER MASK is 0x44 (bit2 & bit 6) if two TAGs are expected and 0x4 if one TAG is expected.
 */
#define QOS_ETH_QUAL_MASK            0x44 /** mask for first tpid exist(bit 2)and second tpid exist(bit6)*/
#define QOS_ETH_QUAL_ONE_TAG_MASK    0x4  /** mask for first tpid exist(bit 2)*/
#define QOS_ETH_UNTAGED_QUAL         0x0  /** look at first tpid exist(bit 2=0) and second tpid exist(bit6=0) both need to be zero */
#define QOS_ETH_ONE_TAG_QUAL         0x4  /** look at first tpid exist(bit 2=1)and second tpid exist(bit6 = 0) */
#define QOS_ETH_TWO_TAG_QUAL         0x44 /** look at first tpid exist(bit 2=1) and second tpid exist(bit 6=1) */

/**
 * \brief
 * default value and mask for layers not using the layer qualifier/next layer qualifier fields
 */
#define QOS_NO_QUAL 0
#define QOS_NO_QUAL_MASK 0x0

/**
 * \brief
 * default value and mask for default port layer
 */
#define QOS_DEF_PORT_VAL  0x0
#define QOS_DEF_PORT_MASK 0x0

/**
 * \brief
 * Layer type mask
 */
#define QOS_LAYER_TYPE_MASK 0x1F
#define QOS_NO_LAYER_TYPE_MASK 0x0

/**
 * \brief
 * qos field empty - used for initialization of unused fields in table to 0
 */
#define QOS_UNUSED_FIELD 0

#define QOS_DEF_PORT_PRIORITY 31
#define QOS_UNUSED_ENTRY_PRIORITY 32

/**
 * \brief
 * Entry value for unused entry in
 * DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION table
 */
#define UNUSED_CS_ENTRY QOS_UNUSED_ENTRY_PRIORITY,                                                                 \
                        QOS_UNUSED_FIELD, QOS_UNUSED_FIELD, QOS_UNUSED_FIELD, QOS_UNUSED_FIELD, QOS_UNUSED_FIELD,  \
                        QOS_UNUSED_FIELD, QOS_UNUSED_FIELD, QOS_UNUSED_FIELD, QOS_UNUSED_FIELD, QOS_UNUSED_FIELD,  \
                       {QOS_UNUSED_FIELD, QOS_UNUSED_FIELD},QOS_UNUSED_FIELD, QOS_UNUSED_FIELD, FALSE

/**
 * \brief  structures for PHB init table managment
 * used to initiate
 * DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION table
 */
typedef struct
{
    uint32 priority;
    uint32 layer_type;
    uint32 next_layer_type;
    uint32 layer_qualifier;
    uint32 next_layer_qualifier;
    uint32 phb_cs_profile;
    uint32 layer_type_mask;
    uint32 next_layer_type_mask;
    uint32 layer_qualifier_mask;
    uint32 next_layer_qualifier_mask;
    uint32 phb_cs_profile_mask;
    uint32 qos_ffc[QOS_MAX_NUM_FFC];
    uint32 qos_result_type;
    uint32 qos_int_map_key_size;
    uint8 valid;
} qos_context_selection_table_t;

/* *INDENT-OFF* */
/**
 * \brief QOS Context Selection Init DB
 * This table cosist the initialization parameters of DBAL
 * Table: DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION
 * in this init table DB for each PACKET_TYPE we have an
 * entry with all needed parameters.
 * This init table is used by the QOS PHB tables init
 * function.
 * Each entry has a priprity field to indicae the order f configuration to the HW,
 * No Holes in priority are allowed, and shall result with ERROR in initialization process
 * The table is of size: INGRESS_QOS_MAP_TYPE_VALUE_NOF+1, the last enty holds default
 * value which will be written to unused HW table entries.
 * the table structure is described below.
 */
CONST qos_context_selection_table_t qos_phb_cs_table_init[INGRESS_QOS_MAP_TYPE_VALUE_NOF+1]=
{/********************************************************************************************************************************
  *      LAYER TYPE              *      NEXT LAYER TYPE             *  LAYER QUALIFIER    *  NEXT LAYER         * CS PROFILE
  *                              *                                  *                     *  QUALIFIER          *
  ********************************************************************************************************************************
  *    LAYER TYPE MASK           *      NEXT LAYER TYPE             * LAYER QUALIFIER     *  NEXT LAYER         * CS PROFILE MASK
  *                              *            MASK                  *       MASK          *  QUALIFIER MASK     *
  ********************************************************************************************************************************
  *         FFC_0                *            FFC_1                 *   QOS_RESULT_TYPE   * QOS_INT_MAP_KEY_SIZE* VALID
 */
  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_DEF_PORT */
   31,
   QOS_DEF_PORT_VAL              , QOS_DEF_PORT_VAL                 , QOS_DEF_PORT_VAL    , QOS_DEF_PORT_VAL    , QOS_DEF_PORT_VAL       ,
   QOS_DEF_PORT_MASK             , QOS_DEF_PORT_MASK                , QOS_DEF_PORT_MASK   , QOS_DEF_PORT_MASK   , QOS_DEF_PORT_MASK      ,
  {QOS_UNUSED_FIELD/**FFC_0*/    , QOS_UNUSED_FIELD/**FFC_1*/}      , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_EXPLICIT , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_IPV4 */
    0,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_IPV4   , QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2,
    QOS_LAYER_TYPE_MASK           , QOS_LAYER_TYPE_MASK                     , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_ETH_IPV4_VAL/**FFC_0*/  , QOS_FFC_1_ETH_IPV4_VAL/**FFC_1*/}  , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_IPV6 */
    1,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_IPV6   , QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2,
    QOS_LAYER_TYPE_MASK           , QOS_LAYER_TYPE_MASK                     , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_ETH_IPV6_VAL/**FFC_0*/, QOS_FFC_1_ETH_IPV6_VAL/**FFC_1*/}    , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_MPLS */
    2,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_MPLS   , QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2,
    QOS_LAYER_TYPE_MASK           , QOS_LAYER_TYPE_MASK                     , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_ETH_MPLS_VAL/**FFC_0*/ , QOS_FFC_1_ETH_MPLS_VAL/**FFC_1*/}   , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_TWO */
    3,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_TWO_TAG_QUAL, QOS_NO_QUAL  , DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK                  , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
   {QOS_FFC_0_ETH_PCP_DEI_VAL     , QOS_FFC_1_ETH_PCP_DEI_VAL}              , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_INNER */
   4,
   DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_TWO_TAG_QUAL, QOS_NO_QUAL   , DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_INNER,
   QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
   {QOS_FFC_0_ETH_PCP_1_VAL     , QOS_FFC_1_ETH_DEI_1_VAL}       , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USER_OUTER */
    5,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_TWO_TAG_QUAL, QOS_NO_QUAL  ,  DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
   {QOS_FFC_0_ETH_PCP_0_VAL     , QOS_FFC_1_ETH_DEI_0_VAL}       , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_ONE_TAG */
    6,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_ONE_TAG_QUAL, QOS_NO_QUAL  , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_FFC_0_ETH_PCP_0_VAL      , QOS_FFC_1_ETH_DEI_0_VAL}         , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_UNTAGGED */
   7,
   DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_UNTAGED_QUAL, QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT,
   QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
   {QOS_UNUSED_FIELD              , QOS_UNUSED_FIELD}                , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_EXPLICIT  , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_RCH */
    8,
    DBAL_ENUM_FVAL_LAYER_TYPES_RCH , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL, QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT ,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_FFC_0_PHB_RCH_VAL        , QOS_FFC_1_PHB_RCH_VAL}       , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS */
   9,
   DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, QOS_NO_QUAL         , QOS_NO_QUAL , DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_TYPICAL,
   QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
   {QOS_FFC_0_MPLS_ETH_PCP_DEI_VAL , QOS_FFC_1_MPLS_ETH_PCP_DEI_VAL} , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG */
   10,
   DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, QOS_NO_QUAL         , QOS_NO_QUAL  , DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER,
   QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
   {QOS_FFC_0_MPLS_ETH_PCP_0_VAL  , QOS_FFC_1_MPLS_ETH_DEI_0_VAL}    , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_MPLS */
   11,
   DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT,
   QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
   {QOS_FFC_0_MPLS_EXP_VAL        , QOS_FFC_1_MPLS_EXP_VAL}          , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_UNTERM_MPLS */
   12,
   DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT,
   QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
   {QOS_FFC_0_MPLS_EXP_VAL        , QOS_FFC_1_MPLS_EXP_VAL}          , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_IPV4 */
   13,
   DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT,
   QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
  {QOS_FFC_0_IPV4_TOS_VAL       , QOS_FFC_1_IPV4_TOS_VAL}         , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_IPV6 */
   14,
   DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT,
   QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
  {QOS_FFC_0_IPV6_TOS_VAL        , QOS_FFC_1_IPV6_TOS_VAL}          , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_16 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_17 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_18 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_19 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_20 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_21 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_22 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_23 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_24 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_25 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_26 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_27 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_28 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_29 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_30 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_31 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE = INGRESS_QOS_MAP_TYPE_VALUE_NOF, This  index is not part of the ENUM and must be set to UNUSED_CS_ENTRY*/
   UNUSED_CS_ENTRY},
};

/**
 * \brief QOS Context Selection Init DB
 * This table cosist the initialization parameters of DBAL
 * Table: DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION
 * in this init table DB for each PACKET_TYPE we have an
 * entry with all needed parameters.
 * This init table is used by the QOS PHB tables init
 * function.
 * Each entry has a priprity field to indicae the order f configuration to the HW,
 * No Holes in priority are allowed, and shall result with ERROR in initialization process
 * The table is of size: INGRESS_QOS_MAP_TYPE_VALUE_NOF+1, the last enty holds default
 * value which will be written to unused HW table entries.
 * the table structure is described below.
 */
CONST qos_context_selection_table_t qos_remark_cs_table_init[INGRESS_QOS_MAP_TYPE_VALUE_NOF+1]=
{/********************************************************************************************************************************
  *      LAYER TYPE              *      NEXT LAYER TYPE             *  LAYER QUALIFIER    *  NEXT LAYER         * CS PROFILE
  *                              *                                  *                     *  QUALIFIER          *
  ********************************************************************************************************************************
  *    LAYER TYPE MASK           *      NEXT LAYER TYPE             * LAYER QUALIFIER     *  NEXT LAYER         * CS PROFILE MASK
  *                              *            MASK                  *       MASK          *  QUALIFIER MASK     *
  ********************************************************************************************************************************
  *         FFC_0                *            FFC_1                 *  QOS_RESULT_TYPE    * QOS_INT_MAP_KEY_SIZE* VALID
 */

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_DEF_PORT */
    31,
    QOS_DEF_PORT_VAL              , QOS_DEF_PORT_VAL                 , QOS_DEF_PORT_VAL    , QOS_DEF_PORT_VAL    , QOS_DEF_PORT_VAL       ,
    QOS_DEF_PORT_MASK             , QOS_DEF_PORT_MASK                , QOS_DEF_PORT_MASK   , QOS_DEF_PORT_MASK   , QOS_DEF_PORT_MASK      ,
    {QOS_UNUSED_FIELD/**FFC_0*/    , QOS_UNUSED_FIELD/**FFC_1*/}      , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_EXPLICIT , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_IPV4 */
    0,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_IPV4   , QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2    ,
    QOS_LAYER_TYPE_MASK           , QOS_LAYER_TYPE_MASK              , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_ETH_IPV4_VAL/**FFC_0*/    , QOS_FFC_1_ETH_IPV4_VAL/**FFC_1*/}      , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_IPV6 */
    1,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_IPV6   , QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2    ,
    QOS_LAYER_TYPE_MASK           , QOS_LAYER_TYPE_MASK              , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_ETH_IPV6_VAL/**FFC_0*/, QOS_FFC_1_ETH_IPV6_VAL/**FFC_1*/}   , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_MPLS */
    2,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_MPLS   , QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2    ,
    QOS_LAYER_TYPE_MASK           , QOS_LAYER_TYPE_MASK              , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_ETH_MPLS_VAL/**FFC_0*/    , QOS_FFC_1_ETH_MPLS_VAL/**FFC_1*/}      , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_TWO */
    3,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_TWO_TAG_QUAL, QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_ETH_PCP_DEI_VAL     , QOS_FFC_1_ETH_PCP_DEI_VAL}       , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_INNER */
    4,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_TWO_TAG_QUAL, QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_INNER,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_ETH_PCP_1_VAL     , QOS_FFC_1_ETH_DEI_1_VAL}       , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USER_OUTER */
    5,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_TWO_TAG_QUAL, QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_FFC_0_ETH_PCP_0_VAL     , QOS_FFC_1_ETH_DEI_0_VAL}       , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_ONE_TAG */
    6,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_ONE_TAG_QUAL, QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT    ,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_FFC_0_ETH_PCP_0_VAL        , QOS_FFC_1_ETH_DEI_0_VAL}           , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_UNTAGGED */
    7,
    DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_ETH_UNTAGED_QUAL, QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT    ,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_ETH_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_UNUSED_FIELD              , QOS_UNUSED_FIELD}                , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_EXPLICIT  , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_RCH */
    8,
    DBAL_ENUM_FVAL_LAYER_TYPES_RCH , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL, QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT    ,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK   , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_FFC_0_REM_RCH_VAL        , QOS_FFC_1_REM_RCH_VAL}       , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS */
    9,
    DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, QOS_NO_QUAL         , QOS_NO_QUAL  , DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_TYPICAL,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_MPLS_ETH_PCP_DEI_VAL , QOS_FFC_1_MPLS_ETH_PCP_DEI_VAL} , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG */
    10,
    DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, QOS_NO_QUAL         , QOS_NO_QUAL  , DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_CS_PROFILE_FULL_MASK   ,
    {QOS_FFC_0_MPLS_ETH_PCP_0_VAL  , QOS_FFC_1_MPLS_ETH_DEI_0_VAL}    , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_MPLS */
    11,
    DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT    ,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_FFC_0_MPLS_EXP_VAL        , QOS_FFC_1_MPLS_EXP_VAL}          , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_UNTERM_MPLS */
    12,
    DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT    ,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_FFC_0_MPLS_EXP_VAL        , QOS_FFC_1_MPLS_EXP_VAL}          , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_IPV4 */
    13,
    DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT    ,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_FFC_0_IPV4_TOS_VAL       , QOS_FFC_1_IPV4_TOS_VAL}         , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_IPV6 */
    14,
    DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL         , QOS_NO_QUAL         , DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT    ,
    QOS_LAYER_TYPE_MASK           , QOS_NO_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_NO_CS_PROFILE_MASK   ,
    {QOS_FFC_0_IPV6_TOS_VAL        , QOS_FFC_1_IPV6_TOS_VAL}          , DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED    , QOS_INT_MAP_KEY_SIZE, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_16 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_17 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_18 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_19 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_20 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_21 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_22 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_23 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_24 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_25 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_26 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_27 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_28 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_29 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_30 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_31 */
   UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE = INGRESS_QOS_MAP_TYPE_VALUE_NOF, This  index is not part of the ENUM and must be set to UNUSED_CS_ENTRY*/
   UNUSED_CS_ENTRY},
};
/* *INDENT-ON* */

/**
 * \brief QOS ECN Context Selection Init DB
 * This table cosist the initialization parameters of DBAL
 * Table: DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION in this
 * init table DB for each PACKET_TYPE we have an entry with all
 * needed parameters. This init table is used by the QOS PHB
 * tables init function. Each entry has a priprity field to
 * indicae the order f configuration to the HW, No Holes in
 * priority are allowed, and shall result with ERROR in initialization process
 * The table is of size: INGRESS_QOS_MAP_TYPE_VALUE_NOF+1, the last enty holds default
 * value which will be written to unused HW table entries.
 * the table structure is described below. */
CONST qos_context_selection_table_t qos_ecn_cs_table_init[INGRESS_QOS_MAP_TYPE_VALUE_NOF + 1] = {
 /********************************************************************************************************************************
  *      LAYER TYPE              *      NEXT LAYER TYPE             *  LAYER QUALIFIER    *  NEXT LAYER         * CS PROFILE
  *                              *                                  *                     *  QUALIFIER          *
  ********************************************************************************************************************************
  *    LAYER TYPE MASK           *      NEXT LAYER TYPE             * LAYER QUALIFIER     *  NEXT LAYER         * CS PROFILE MASK
  *                              *            MASK                  *       MASK          *  QUALIFIER MASK     *
  ********************************************************************************************************************************
  *         FFC_0                *            FFC_1                 *  QOS_RESULT_TYPE    * QOS_INT_MAP_KEY_SIZE* VALID
 */
    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_DEF_PORT */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_IPV4 */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_IPV6 */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_MPLS */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_TWO */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_INNER */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_OUTER */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_ONE_TAG */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_UNTAGGED */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_RCH */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_MPLS */
     0,
     DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL, QOS_NO_QUAL,
     QOS_UNUSED_FIELD,
     QOS_LAYER_TYPE_MASK, QOS_NO_QUAL_MASK, QOS_NO_QUAL_MASK, QOS_NO_QUAL_MASK, QOS_UNUSED_FIELD,
     {QOS_UNUSED_FIELD /**FFC_0*/ , QOS_UNUSED_FIELD /**FFC_1*/ }, DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED,
     QOS_ECN_INT_MAP_KEY_SIZE, TRUE},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_UNTERM_MPLS */
     1,
     DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION,
     QOS_NO_QUAL, QOS_NO_QUAL,
     QOS_UNUSED_FIELD,
     QOS_LAYER_TYPE_MASK, QOS_NO_QUAL_MASK, QOS_NO_QUAL_MASK, QOS_NO_QUAL_MASK, QOS_UNUSED_FIELD,
     {QOS_UNUSED_FIELD /**FFC_0*/ , QOS_UNUSED_FIELD /**FFC_1*/ }, DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED,
     QOS_ECN_INT_MAP_KEY_SIZE, TRUE},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_IPV4 */
     2,
     DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL, QOS_NO_QUAL,
     QOS_UNUSED_FIELD,
     QOS_LAYER_TYPE_MASK, QOS_NO_QUAL_MASK, QOS_NO_QUAL_MASK, QOS_NO_QUAL_MASK, QOS_UNUSED_FIELD,
     {QOS_FFC_0_IPV4_TOS_VAL, QOS_FFC_1_IPV4_TOS_VAL}, DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED,
     QOS_ECN_INT_MAP_KEY_SIZE, TRUE},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_IPV6 */
     3,
     DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL, QOS_NO_QUAL,
     QOS_UNUSED_FIELD,
     QOS_LAYER_TYPE_MASK, QOS_NO_QUAL_MASK, QOS_NO_QUAL_MASK, QOS_NO_QUAL_MASK, QOS_UNUSED_FIELD,
     {QOS_FFC_0_IPV6_TOS_VAL, QOS_FFC_1_IPV6_TOS_VAL}, DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED,
     QOS_ECN_INT_MAP_KEY_SIZE, TRUE},

    {
   /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_16 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_17 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_18 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_19 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_20 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_21 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_22 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_23 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_24 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_25 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_26 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_27 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_28 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_29 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_30 */
     UNUSED_CS_ENTRY},

    {
  /** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_31 */
     UNUSED_CS_ENTRY},

    {
   /** INGRESS_QOS_MAP_TYPE = INGRESS_QOS_MAP_TYPE_VALUE_NOF, This  index is not part of the ENUM and must be set to UNUSED_CS_ENTRY*/
     UNUSED_CS_ENTRY},
};
/* *INDENT-ON* */


/* *INDENT-OFF* */
/**
 * \brief QOS TTL Context Selection Init DB
 * This table cosist the initialization parameters of DBAL
 * Table: DBAL_TABLE_QOS_INGRESS_INIT_TTL_CONTEXT_SELECTION
 * in this init table DB for each PACKET_TYPE we have an
 * entry with all needed parameters.
 * This init table is used by the QOS PHB tables init
 * function.
 * Each entry has a priority field to indicae the order of configuration to the HW,
 * No Holes in priority are allowed, and shall result with ERROR in initialization process
 * The table is of size: INGRESS_QOS_MAP_TYPE_VALUE_NOF+1, the last enty holds default
 * value which will be written to unused HW table entries.
 * the table structure is described below.
 *
 * Please Note:  some of the fields are not used, this is done to reuse the Init code for
 * DBAL_TABLE_QOS_INIT_INGRESS_CONTEXT_SELECTION table
 */
CONST qos_context_selection_table_t qos_ttl_cs_table_init[INGRESS_QOS_MAP_TYPE_VALUE_NOF+1]=
{/********************************************************************************************************************************
  *      LAYER TYPE              *      NEXT LAYER TYPE             *  LAYER QUALIFIER    *  NEXT LAYER         * QOS_UNUSED_FIELD
  *                              *                                  *                     *  QUALIFIER          *
  ********************************************************************************************************************************
  *    LAYER TYPE MASK           *      NEXT LAYER TYPE             * LAYER QUALIFIER     *  NEXT LAYER         * QOS_UNUSED_FIELD
  *                              *            MASK                  *       MASK          *  QUALIFIER MASK     *
  ********************************************************************************************************************************
  *         FFC_0                *            FFC_1                 * QOS_UNUSED_FIELD    * QOS_UNUSED_FIELD    * VALID
 */

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_DEF_PORT */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_IPV4 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_IPV6 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_USE_MPLS */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_TWO */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_INNER */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_DOUBLE_TAG_USE_OUTER */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_ONE_TAG */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_ETH_UNTAGGED */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_RCH */
    0,
    DBAL_ENUM_FVAL_LAYER_TYPES_RCH , DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL, QOS_NO_QUAL         , QOS_UNUSED_FIELD   ,
    QOS_LAYER_TYPE_MASK            , QOS_NO_LAYER_TYPE_MASK          , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK    , QOS_UNUSED_FIELD   ,
    {QOS_FFC_0_TTL_RCH_VAL         , QOS_FFC_1_TTL_RCH_VAL}          , QOS_UNUSED_FIELD    , QOS_UNUSED_FIELD, TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_MPLS */
   1,
   DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL  , QOS_NO_QUAL       , QOS_UNUSED_FIELD,
   QOS_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK                 , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK  , QOS_UNUSED_FIELD,
  {QOS_FFC_0_MPLS_TTL_VAL        , QOS_FFC_1_MPLS_TTL_VAL }         , QOS_UNUSED_FIELD    , QOS_UNUSED_FIELD  , TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_UNTERM_MPLS */
   2,
   DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL  , QOS_NO_QUAL       , QOS_UNUSED_FIELD,
   QOS_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK                 , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK  , QOS_UNUSED_FIELD,
  {QOS_FFC_0_MPLS_TTL_VAL        , QOS_FFC_1_MPLS_TTL_VAL }         , QOS_UNUSED_FIELD    , QOS_UNUSED_FIELD  , TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_IPV4 */
   3,
   DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL  , QOS_NO_QUAL       , QOS_UNUSED_FIELD,
   QOS_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK                 , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK  , QOS_UNUSED_FIELD,
  {QOS_FFC_0_IPV4_TTL_VAL        , QOS_FFC_1_IPV4_TTL_VAL}          , QOS_UNUSED_FIELD    , QOS_UNUSED_FIELD  , TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_IPV6 */
   4,
   DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, QOS_NO_QUAL  , QOS_NO_QUAL       , QOS_UNUSED_FIELD,
   QOS_LAYER_TYPE_MASK           , QOS_NO_QUAL_MASK                 , QOS_NO_QUAL_MASK    , QOS_NO_QUAL_MASK  , QOS_UNUSED_FIELD,
  {QOS_FFC_0_IPV6_TTL_VAL        , QOS_FFC_1_IPV6_TTL_VAL}          , QOS_UNUSED_FIELD    , QOS_UNUSED_FIELD  , TRUE},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_18 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_17 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_18 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_19 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_20 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_21 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_22 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_23 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_24 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_25 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_26 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_27 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_28 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_29 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_30 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE =  INGRESS_QOS_MAP_TYPE_31 */
     UNUSED_CS_ENTRY},

  {/** INGRESS_QOS_MAP_TYPE = INGRESS_QOS_MAP_TYPE_VALUE_NOF, This  index is not part of the ENUM and must be set to UNUSED_CS_ENTRY*/
   UNUSED_CS_ENTRY},
};
/* *INDENT-ON* */

/*
 * profile name space
 */
CONST char *profile_namespace_strings[DBAL_NOF_ENUM_QOS_PROFILE_NAME_SPACE_VALUES] = {
    "Default",  /* DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT, */
    "L3_l2",    /* DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_L3_L2 */
    "Ethernet Two tags",        /* DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_ETHERNET */
    "PWE tagged mode"   /* DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_TAGGED_PWE */
};

/*
 * define the numbers of bits to shift for TTL_MODEL
 * TTL_MODEL = {TTL_DECREMENT_DISABLE(1), TTL_MODE(1)}
 */
#define DNX_QOS_BIT_SHIFT_PREFIX_TTL_MODE                 (6)
#define DNX_QOS_BIT_SHIFT_PREFIX_TTL_DECREMENT_DISABLE    (7)

#define DNX_QOS_BIT_MASK_PREFIX_TTL_MODE                  (1)

/**
 * \brief
 * build map key.
 * for each profile & app & map type we have a unique key
 * structure for example for ETH it is PCP/DEI, for IPV6 it is
 * TOS for IPV4 it is DSCP
 *    \param [in] unit - Relevant unit.
 *    \param [in] flags - Relevant flags.
 *    \param [in] map_entry - Relevant map entry.
 *    \param [in] map_key - Relevant map key.
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 */
shr_error_e
dnx_qos_build_map_key(
    int unit,
    uint32 flags,
    bcm_qos_map_t * map_entry,
    uint8 *map_key)
{
    uint8 outer_pcp_dei;
    uint8 inner_pcp_dei;
    dbal_enum_value_field_ingress_qos_map_index_e map_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_ingress_map_index_get(unit, flags, &map_index));

    switch (map_index)
    {
        case DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT:
            *map_key =
                ((map_entry->pkt_pri & DNX_QOS_TC_MASK) << DNX_QOS_TC_OFFSET) | (map_entry->pkt_cfi & DNX_QOS_DP_MASK);
            break;
        case DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_ONE_TAG:
            if (flags & BCM_QOS_MAP_L2_INNER_TAG)
            {
                *map_key =
                    ((map_entry->inner_pkt_pri & DNX_QOS_PRI_MASK) << DNX_QOS_PRI_OFFSET) | (map_entry->inner_pkt_cfi &
                                                                                             DNX_QOS_CFI_MASK);
            }
            else
            {
                *map_key =
                    ((map_entry->
                      pkt_pri & DNX_QOS_PRI_MASK) << DNX_QOS_PRI_OFFSET) | (map_entry->pkt_cfi & DNX_QOS_CFI_MASK);
            }
            break;

        case DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_TWO_TAGS:
            outer_pcp_dei =
                ((map_entry->
                  pkt_pri & DNX_QOS_PRI_MASK) << DNX_QOS_PRI_OFFSET) | (map_entry->pkt_cfi & DNX_QOS_CFI_MASK);
            inner_pcp_dei =
                ((map_entry->
                  inner_pkt_pri & DNX_QOS_PRI_MASK) << DNX_QOS_PRI_OFFSET) | (map_entry->inner_pkt_cfi &
                                                                              DNX_QOS_CFI_MASK);
            *map_key = (inner_pcp_dei << DNX_QOS_PCP_DEI_OFFSET) | outer_pcp_dei;
            break;

        case DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4:
            *map_key = map_entry->dscp;
            break;

        case DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6:
            /** In IPV6 we use the dscp field to update the tos value */
            *map_key = map_entry->dscp;
            break;

        case DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS:
            *map_key = map_entry->exp;
            break;

        case DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_RCH:
            if (flags & BCM_QOS_MAP_PHB)
            {
                *map_key =
                    ((map_entry->remark_int_pri & DNX_QOS_TC_MASK) << DNX_QOS_TC_OFFSET) | (map_entry->remark_color &
                                                                                            DNX_QOS_DP_MASK);
            }
            else if (flags & BCM_QOS_MAP_REMARK)
            {
                *map_key = map_entry->int_pri;
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for map_index [%x]\n", map_index);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize qos hw table
 * DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION currently it is
 * used for both PHB & REMARING single entry to it's initial
 * values.
 *    \param [in] unit - Relevant unit.
 *    \param [in] entry_handle_id - table hadle id
 *    \param [in] priority - relevant entry priority used as key
 *                           to the table
 *    \param [in] map_type - qos map type
 *    \param [in] qos_cs_table_init_ptr - ptr to cs table init
 *           data.
 *    \param [in] qos_init_type - qos init type PHB/REMARK.
 * \return
 *   Negative in case of an error.
 *   Zero in case of NO ERROR
 * \remark
 *   Write to QOS Hardware.
 *   DBAL table: DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION
 *   DBAL table:
 *   DBAL_TABLE_QOS_INGRESS_INIT_TTL_CONTEXT_SELECTION
 */
static shr_error_e
dnx_qos_ingress_tables_init_write_entry(
    int unit,
    uint32 entry_handle_id,
    uint32 priority,
    ingress_qos_map_type_value_e map_type,
    CONST qos_context_selection_table_t * qos_cs_table_init_ptr,
    uint32 qos_init_type)
{
    uint8 ffc_index;

    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_CAM_PRIORITY, priority);

    /*
     * The following key field & result fields don't exist in TTL table
     */
    if (qos_init_type != DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_TTL)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INGRESS_APP_TYPE, qos_init_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_QOS_MAP_INDEX, INST_SINGLE,
                                     dnx_map_type_to_index[map_type]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_CS_PROFILE, INST_SINGLE,
                                     qos_cs_table_init_ptr->phb_cs_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_CS_PROFILE_MASK, INST_SINGLE,
                                     qos_cs_table_init_ptr->phb_cs_profile_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_RESOLUTION_MODE, INST_SINGLE,
                                     qos_cs_table_init_ptr->qos_result_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_KEY_SIZE, INST_SINGLE,
                                     qos_cs_table_init_ptr->qos_int_map_key_size);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_ACTION, INST_SINGLE, priority);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, INST_SINGLE,
                                 qos_cs_table_init_ptr->layer_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_TYPE, INST_SINGLE,
                                 qos_cs_table_init_ptr->next_layer_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER, INST_SINGLE,
                                 qos_cs_table_init_ptr->layer_qualifier);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_QUALIFIER, INST_SINGLE,
                                 qos_cs_table_init_ptr->next_layer_qualifier);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_MASK, INST_SINGLE,
                                 qos_cs_table_init_ptr->layer_type_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_TYPE_MASK, INST_SINGLE,
                                 qos_cs_table_init_ptr->next_layer_type_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_MASK, INST_SINGLE,
                                 qos_cs_table_init_ptr->layer_qualifier_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_QUALIFIER_MASK, INST_SINGLE,
                                 qos_cs_table_init_ptr->next_layer_qualifier_mask);

    for (ffc_index = 0; ffc_index < QOS_MAX_NUM_FFC; ffc_index++)
    {
        qos_ffc_type_value_e qos_ffc_type;
        uint32 ffc_value;

        qos_ffc_type = qos_cs_table_init_ptr->qos_ffc[ffc_index];
        if ((qos_init_type == DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB
             || qos_init_type == DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK)
            && (dnx_data_qos.feature.feature_get(unit, dnx_data_qos_feature_phb_rm_ffc_extend)))
        {
            ffc_value = QOS_RM_PHB_FFC_VALUE(qos_ffc_table[qos_ffc_type]);
        }
        else
        {
            ffc_value = QOS_FFC_VALUE(qos_ffc_table[qos_ffc_type]);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, ffc_index, ffc_value);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, qos_cs_table_init_ptr->valid);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Update the QoS context selection table data for PWE tagged mode cases.
 *    \param [in] unit - Relevant unit.
 *    \param [in,out] qos_cs_table_data - ptr to cs table init data
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
static void
dnx_qos_ingress_cs_data_hdr_qual_after_mpls_update(
    int unit,
    qos_context_selection_table_t * qos_cs_table_data)
{
    /*
     * Update the default data with layer information after mpls termination
     */
    qos_cs_table_data[INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS].next_layer_type =
        DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET;
    qos_cs_table_data[INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS].next_layer_type_mask = QOS_LAYER_TYPE_MASK;
    qos_cs_table_data[INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS].next_layer_qualifier = QOS_ETH_TWO_TAG_QUAL;
    qos_cs_table_data[INGRESS_QOS_MAP_TYPE_PWE_TAGGED_DOUBLE_TAGS].next_layer_qualifier_mask = QOS_ETH_QUAL_MASK;

    qos_cs_table_data[INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG].next_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET;
    qos_cs_table_data[INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG].next_layer_type_mask = QOS_LAYER_TYPE_MASK;
    qos_cs_table_data[INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG].next_layer_qualifier = QOS_ETH_QUAL_ONE_TAG_MASK;
    qos_cs_table_data[INGRESS_QOS_MAP_TYPE_PWE_TAGGED_OUTER_TAG].next_layer_qualifier_mask = QOS_ETH_ONE_TAG_QUAL;
}

/**
 * \brief
 * Initialize qos hw table for a specific ingress type
 * PHB/REMARK to it's initial values.
 *    \param [in] unit - Relevant unit.
 *    \param [in] entry_handle_id - Relevant entry_handle_id.
 *    \param [in] qos_init_type - qos init type PHB/REMARK.
 *    \param [in] qos_cs_table_init_ptr - ptr to cs table init
 *           data.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e
dnx_qos_tables_ingress_type_init(
    int unit,
    uint32 entry_handle_id,
    uint32 qos_init_type,
    CONST qos_context_selection_table_t * qos_cs_table_init_ptr)
{
    ingress_qos_map_type_value_e map_type;
    uint8 qos_init_status[INGRESS_QOS_MAP_TYPE_VALUE_NOF];
    uint32 valid_entries_cntr = 0;
    uint32 priority;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(qos_init_status, 0, INGRESS_QOS_MAP_TYPE_VALUE_NOF);

    for (map_type = 0; map_type < INGRESS_QOS_MAP_TYPE_VALUE_NOF; map_type++)
    {
        priority = qos_cs_table_init_ptr[map_type].priority;
        if (priority < QOS_DEF_PORT_PRIORITY)
        {
            valid_entries_cntr++;

            if (qos_init_status[priority] == 0)
            {
                dnx_qos_ingress_tables_init_write_entry(unit, entry_handle_id, priority,
                                                        map_type, &qos_cs_table_init_ptr[map_type], qos_init_type);
                qos_init_status[priority]++;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "priority [%d] allready configured", qos_init_status[map_type]);
            }
        }
    }

    /*
     * Check init DB has no Holes in it
     */
    for (priority = 0; priority < valid_entries_cntr; priority++)
    {
        if (qos_init_status[priority] == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "init DB has a HOLE, priority [%d] is not configured", priority);
        }
    }

    /*
     * Write DEFAULT PORT entry as last valid entry
     */
    dnx_qos_ingress_tables_init_write_entry(unit, entry_handle_id, valid_entries_cntr,
                                            INGRESS_QOS_MAP_TYPE_DEFAULT_PORT,
                                            &qos_cs_table_init_ptr[INGRESS_QOS_MAP_TYPE_DEFAULT_PORT], qos_init_type);
    valid_entries_cntr++;

    /*
     * write all other entries as unused
     */
    for (map_type = valid_entries_cntr; map_type < INGRESS_QOS_MAP_TYPE_VALUE_NOF; map_type++)
    {
        dnx_qos_ingress_tables_init_write_entry(unit, entry_handle_id, valid_entries_cntr,
                                                INGRESS_QOS_MAP_TYPE_DEFAULT_PORT,
                                                &qos_cs_table_init_ptr[INGRESS_QOS_MAP_TYPE_VALUE_NOF], qos_init_type);
        valid_entries_cntr++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize qos hw table DBAL_TABLE_QOS_INHERITANCE_RESOLUTION_MPLS_CFG
 * to default mpls layer type.
 *    \param [in] unit - Relevant unit.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_tables_ingress_mpls_layer_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_INHERITANCE_RESOLUTION_MPLS_TYPE, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_INHERITANCE_RESOLUTION_MPLS, INST_SINGLE,
                                 DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PHB_INHERITANCE_RESOLUTION_MPLS, INST_SINGLE,
                                 DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_INHERITANCE_RESOLUTION_MPLS, INST_SINGLE,
                                 DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_INHERITANCE_RESOLUTION_MPLS, INST_SINGLE,
                                 DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize qos hw table DBAL_TABLE_QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE
 * to default inlif profile.
 */

shr_error_e
dnx_qos_tables_ingress_policer_map_init(
    int unit)
{
    uint32 entry_handle_id;
    uint8 pcp = 0, dei = 0;
    int ingress_dp = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ingress policer is for IVE and processed at egress, so table name is egress
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE, &entry_handle_id));

    for (ingress_dp = 0; ingress_dp <= dnx_data_qos.qos.packet_max_dp_get(unit); ingress_dp++)
    {
        for (pcp = 0; pcp <= dnx_data_qos.qos.packet_max_priority_get(unit); pcp++)
        {
            for (dei = 0; dei <= dnx_data_qos.qos.packet_max_cfi_get(unit); dei++)
            {
                /**
                 * Set dbal key : dp profie + dp + map_key.
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_PROFILE, 0);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_DP, ingress_dp);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PCP, pcp);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_DEI, dei);
                /*
                 * set dbal map value
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_PCP, INST_SINGLE, pcp);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_DEI, INST_SINGLE, dei);

                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set qos init pcp dei map type for IVE.
 *    \param [in] unit - Relevant unit.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_tables_ingress_ive_init_pcp_dei_type_set(
    int unit)
{
    uint32 entry_handle_id;
    uint8 llvp_pcp_dei_type;
    uint8 pcp_dei_extend;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_IVE_INITIAL_PCP_DEI_MAP_TYPE, &entry_handle_id));

    for (llvp_pcp_dei_type = 0; llvp_pcp_dei_type < DBAL_NOF_ENUM_PCP_DEI_MAP_TYPE_VALUES; llvp_pcp_dei_type++)
    {
        for (pcp_dei_extend = 0; pcp_dei_extend < DBAL_NOF_ENUM_PCP_DEI_MAP_TYPE_VALUES; pcp_dei_extend++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_PCP_DEI_MAP_TYPE, llvp_pcp_dei_type);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_MAP_EXTEND, pcp_dei_extend);
            if ((llvp_pcp_dei_type == DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_EXPLICIT) &&
                (pcp_dei_extend == DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_EXPLICIT))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_PCP_DEI_MAP_TYPE, INST_SINGLE,
                                             DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_EXPLICIT);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_PCP_DEI_MAP_TYPE, INST_SINGLE,
                                             DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_MAPPING);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  init hardware table for reserved propatation profile
 *
 * \param [in] unit - The relevant unit.
 * \param [in] propag_profile - The relevant propagation profile.
 *
 * \return
 * \retval Negative in case of an error.
 * \retval Zero in case of NO ERROR.
 */

static shr_error_e
dnx_qos_tables_ingress_propagation_init(
    int unit,
    int propag_profile)
{
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    dnx_qos_propagation_type_e ttl_propagation_type;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the attributes of default profile from SW */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get(unit, propag_profile,
                                                                 &phb_propagation_type, &remark_propagation_type,
                                                                 &ecn_propagation_type, &ttl_propagation_type));
    /** Set the attributes of default profile to HW */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set(unit, propag_profile,
                                                               phb_propagation_type, remark_propagation_type,
                                                               ecn_propagation_type, ttl_propagation_type));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     This function initializes ingress ecn mapping
 *     profile 1 mapping to opcode 1,
 *     opcode 1 full mapping if ecn eligible
 */
static shr_error_e
dnx_qos_tables_ingress_ecn_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 map_key;
    uint32 ecn_value = 0;
    uint32 resource_flags;
    int map_opcode;
    int map_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ECN profile/opcode 0 is reserved, and only profile/opcode 1 is used internally for ECN eligible
     */
    resource_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    map_opcode = DNX_QOS_ECN_MAPPING_OPCODE_ELIGIBLE;
    SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn_opcode.allocate_single(unit, resource_flags, NULL, &map_opcode));
    map_id = DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE;
    SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn.allocate_single(unit, resource_flags, NULL, &map_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_ECN_PROFILE_TO_INTERNAL_MAP_ID, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_QOS_PROFILE, DNX_QOS_MAP_PROFILE_GET(map_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_QOS_MAP_INDEX,
                                     DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4,
                                     DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, INST_SINGLE,
                                 DNX_QOS_MAP_PROFILE_GET(map_opcode));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** set ECN mapping */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_INGRESS_ECN_INTERNAL_MAP_ENTRIES, entry_handle_id));

    for (map_key = 0; map_key <= dnx_data_qos.qos.packet_max_dscp_get(unit); map_key++)
    {
        ecn_value = map_key & DNX_QOS_ECN_MASK;
        if (dnx_data_qos.qos.feature_get(unit, dnx_data_qos_qos_ecn_L4S))
        {
            uint32 ecn_L4S = 0;

            /** IP-ECN == X1, classifies to the L4S queue */
            ecn_L4S = ecn_value & DNX_QOS_ECN_ECT_1;
            if (ecn_L4S)
            {
                ecn_value = ecn_value & (~DNX_QOS_ECN_ECT_1);
            }
            ecn_value |= (ecn_L4S << DNX_QOS_ECN_OFFSET);
        }

        /*
         * 2b'10 means ECN eligible which classifies to the Classic queue
         */
        if (ecn_value == DNX_QOS_ECN_ECT_0)
        {
            ecn_value = DNX_QOS_ECN_ECT_1;
        }

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_KEY, map_key);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID,
                                   DNX_QOS_ECN_MAPPING_OPCODE_ELIGIBLE);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_VALUE, INST_SINGLE, ecn_value);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize qos hw table DBAL_TABLE_QOS_PHB_CONTEXT_SELECTION
 * to it's initial values.
 */
shr_error_e
dnx_qos_ingress_tables_init(
    int unit)
{
    uint32 entry_handle_id;
    qos_context_selection_table_t qos_phb_cs_table_data[INGRESS_QOS_MAP_TYPE_VALUE_NOF + 1];
    qos_context_selection_table_t qos_remark_cs_table_data[INGRESS_QOS_MAP_TYPE_VALUE_NOF + 1];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memcpy(qos_phb_cs_table_data, qos_phb_cs_table_init, sizeof(qos_phb_cs_table_init));
    sal_memcpy(qos_remark_cs_table_data, qos_remark_cs_table_init, sizeof(qos_remark_cs_table_init));

    SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_mpls_layer_init(unit));

    SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_policer_map_init(unit));

    SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_ive_init_pcp_dei_type_set(unit));

    if (dnx_data_qos.feature.feature_get(unit, dnx_data_qos_feature_hdr_qual_after_mpls_available))
    {
        dnx_qos_ingress_cs_data_hdr_qual_after_mpls_update(unit, qos_phb_cs_table_data);
        dnx_qos_ingress_cs_data_hdr_qual_after_mpls_update(unit, qos_remark_cs_table_data);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_type_init
                    (unit, entry_handle_id, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB, &qos_phb_cs_table_data[0]));

    SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_type_init
                    (unit, entry_handle_id, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK, &qos_remark_cs_table_data[0]));

    SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_type_init
                    (unit, entry_handle_id, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN, &qos_ecn_cs_table_init[0]));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_INGRESS_INIT_TTL_CONTEXT_SELECTION, entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_type_init
                    (unit, entry_handle_id, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_TTL, &qos_ttl_cs_table_init[0]));

    SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_ecn_init(unit));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  allocate reseved propagation profile,
 *  the PIPE propagation profile is allocated as default.
 *  here allocate uniform and short pipe profile.
 *
 * \param [in] unit - The relevant unit.
 *
 * \return
 * \retval Negative in case of an error.
 * \retval Zero in case of NO ERROR.
 */
static shr_error_e
dnx_qos_ingress_propagation_profiles_init(
    int unit)
{
    dnx_qos_propagation_type_e propagation_type;
    int propag_profile;
    uint8 is_first;
    uint8 is_last;

    SHR_FUNC_INIT_VARS(unit);

    /** PIPE is allocated as default, only need to init hardware table*/
    SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_propagation_init(unit, DNX_QOS_INGRESS_PROPAGATION_PROFILE_PIPE));

    /**UNIFORM, used for native ac, and mpls dumy native lif (dnx_mpls_init_add_dummy_native_virtual_lif)*/
    propagation_type = DNX_QOS_PROPAGATION_TYPE_UNIFORM;
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc
                    (unit, propagation_type, propagation_type, DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE, propagation_type,
                     DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE, &propag_profile, &is_first, &is_last));
    if (propag_profile != DNX_QOS_INGRESS_PROPAGATION_PROFILE_UNIFORM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "propataion profile UNIFORM should be 1, but geallocate %x\n", propag_profile);
    }

    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_propagation_init(unit, propag_profile));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "init must be first\n");
    }

    /** SHORT PIPE*/
    propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc
                    (unit, propagation_type, propagation_type, propagation_type, propagation_type,
                     DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE, &propag_profile, &is_first, &is_last));

    if (propag_profile != DNX_QOS_INGRESS_PROPAGATION_PROFILE_SHORT_PIPE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "propataion profile SHORT-PIPE should be 2, but allocate %x\n", propag_profile);
    }

    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_qos_tables_ingress_propagation_init(unit, propag_profile));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "init must be first\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * allocate profile 0 as default profile
 */
shr_error_e
dnx_qos_ingress_profiles_init(
    int unit)
{
    uint32 flags;
    int map_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create map_id as default used internally.
     */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(dnx_qos_ingress_map_create(unit, flags, &map_id));

    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2_VLAN_PCP;
    SHR_IF_ERR_EXIT(dnx_qos_ingress_map_create(unit, flags, &map_id));

    /*
     * Init default propagation profile by setting HW
     */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profiles_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * convert bcm flags to map_index (DBAL/HW) table identifiers
 */
shr_error_e
dnx_qos_ingress_map_index_get(
    int unit,
    uint32 flags,
    dbal_enum_value_field_ingress_qos_map_index_e * map_index)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_L2)
    {
        if (flags & BCM_QOS_MAP_L2_UNTAGGED)
        {
            *map_index = DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT;
        }
        else if (flags & BCM_QOS_MAP_L2_TWO_TAGS)
        {
            *map_index = DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_TWO_TAGS;
        }
        else
        /**BCM_QOS_MAP_L2_INNER_TAG or BCM_QOS_MAP_L2_OUTER_TAG*/
        {
            *map_index = DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_ONE_TAG;
        }
    }
    else if (flags & BCM_QOS_MAP_L3)
    {
        if (flags & BCM_QOS_MAP_IPV6)
        {
            *map_index = DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6;
        }
        else
        /** BCM_QOS_MAP_IPV4*/
        {
            *map_index = DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4;
        }
    }
    else if (flags & BCM_QOS_MAP_MPLS)
    {
        *map_index = DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS;
    }
    else if (flags & BCM_QOS_MAP_PORT)
    {
        *map_index = DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT;
    }
    else if (flags & BCM_QOS_MAP_RCH)
    {
        *map_index = DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_RCH;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for flags %x\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  create the ingress sw qos profile according to the flags
 */
shr_error_e
dnx_qos_ingress_map_create(
    int unit,
    uint32 flags,
    int *map_id)
{
    uint32 resource_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_WITH_ID)
    {
        resource_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        /** clear map type and direction*/
        *map_id = DNX_QOS_MAP_PROFILE_GET(*map_id);
    }

    if ((flags & BCM_QOS_MAP_PHB) && (flags & BCM_QOS_MAP_REMARK))
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.allocate_single(unit, resource_flags, NULL, map_id));
        /** set map PHB */
        DNX_QOS_PHB_MAP_SET(*map_id);
        /** set map REMARK */
        DNX_QOS_REMARK_MAP_SET(*map_id);
    }
    else if (flags & BCM_QOS_MAP_POLICER)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_policer.allocate_single(unit, resource_flags, NULL, map_id));
        /** set map policer */
        DNX_QOS_POLICER_MAP_SET(*map_id);
    }
    else if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_vlan_pcp.allocate_single(unit, resource_flags, NULL, map_id));
        /** set map pcp */
        DNX_QOS_VLAN_PCP_MAP_SET(*map_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags combination not supported for INGRESS QOS profile [%x]\n", flags);
    }
    /** set map direction  */
    DNX_QOS_INGRESS_MAP_SET(*map_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  create the ingress sw qos opcode according to the flags
 */
shr_error_e
dnx_qos_ingress_opcode_create(
    int unit,
    uint32 flags,
    int *map_opcode)
{
    uint32 resource_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_WITH_ID)
    {
        resource_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        /** clear map type and direction*/
        *map_opcode = DNX_QOS_MAP_PROFILE_GET(*map_opcode);
    }

    if (flags & BCM_QOS_MAP_REMARK)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_remark_opcode.allocate_single(unit, resource_flags, NULL, map_opcode));
        /** set map REMARK */
        DNX_QOS_REMARK_MAP_SET(*map_opcode);
    }
    else if (flags & BCM_QOS_MAP_PHB)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_phb_opcode.allocate_single(unit, resource_flags, NULL, map_opcode));
        /** set map PHB */
        DNX_QOS_PHB_MAP_SET(*map_opcode);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags combination not supported for INGRESS QOS opcode [%x]\n", flags);
    }
    /** set map opcode */
    DNX_QOS_OPCODE_MAP_SET(*map_opcode);
    /** set map direction  */
    DNX_QOS_INGRESS_MAP_SET(*map_opcode);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * dnx_qos_ingress_map_opcode_set
 * set app map opcode according to ingress qos_profile and map type
 *    \param [in] unit - Relevant unit.
 *    \param [in] map_id - ID of the INGRESS MAP.
 *    \param [in] flags - related map id.
 *         BCM_QOS_MAP_PHB
 *         BCM_QOS_MAP_REMARK
 *    \param [in] opcode - encoded ID for internal map.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_ingress_map_opcode_set(
    int unit,
    uint32 flags,
    int map_id,
    uint32 opcode)
{
    uint32 entry_handle_id;
    int table_id;
    int profile_index = DNX_QOS_MAP_PROFILE_GET(map_id);
    int opcode_index = DNX_QOS_MAP_PROFILE_GET(opcode);
    dbal_fields_e profile_field;
    dbal_enum_value_field_ingress_qos_map_index_e map_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_ingress_map_index_get(unit, flags, &map_index));

    if (flags & BCM_QOS_MAP_PHB)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID;
        profile_field = DBAL_FIELD_QOS_PROFILE;
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID;
        profile_field = DBAL_FIELD_QOS_PROFILE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, profile_field, profile_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_QOS_MAP_INDEX, map_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, INST_SINGLE, opcode_index);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * dnx_qos_ingress_map_opcode_clear
 * clear app map opcode id that connected to qos_profile
 *     and map type.
 *    \param [in] unit - Relevant unit.
 *    \param [in] map_id - ID of the INGRESS MAP.
 *    \param [in] flags - related flags.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_ingress_map_opcode_clear(
    int unit,
    uint32 flags,
    int map_id)
{
    uint32 entry_handle_id;
    int table_id;
    int profile_index = DNX_QOS_MAP_PROFILE_GET(map_id);
    dbal_fields_e profile_field;
    dbal_enum_value_field_ingress_qos_map_index_e map_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_ingress_map_index_get(unit, flags, &map_index));

    if (flags & BCM_QOS_MAP_PHB)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID;
        profile_field = DBAL_FIELD_QOS_PROFILE;
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID;
        profile_field = DBAL_FIELD_QOS_PROFILE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, profile_field, profile_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_QOS_MAP_INDEX, map_index);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API returns the opcode id connected to ingress qos_profile and
 * map type.
 *    \param [in] unit - Relevant unit.
*    \param [in] flags - related flags.
 *         BCM_QOS_MAP_PHB
 *         BCM_QOS_MAP_REMARK
 *    \param [in] map_id - ID of the INGRESS MAP.
 *    \param [in] opcode - ID of internal map.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_ingress_map_opcode_get(
    int unit,
    uint32 flags,
    int map_id,
    uint32 *opcode)
{
    uint32 entry_handle_id;
    int profile_index = DNX_QOS_MAP_PROFILE_GET(map_id);
    uint32 table_id;
    dbal_fields_e profile_field;
    dbal_enum_value_field_ingress_qos_map_index_e map_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_ingress_map_index_get(unit, flags, &map_index));

    if (flags & BCM_QOS_MAP_PHB)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID;
        profile_field = DBAL_FIELD_QOS_PROFILE;
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID;
        profile_field = DBAL_FIELD_QOS_PROFILE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, profile_field, profile_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_QOS_MAP_INDEX, map_index);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, INST_SINGLE, opcode);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (flags & BCM_QOS_MAP_PHB)
    {
        DNX_QOS_PHB_MAP_SET(*opcode);
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        DNX_QOS_REMARK_MAP_SET(*opcode);
    }

    DNX_QOS_INGRESS_MAP_SET(*opcode);
    DNX_QOS_OPCODE_MAP_SET(*opcode);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos INGRESS PHB map
 * ID & key.
 * for each profile & app & map type we have a unique key
 * structure for example for ETH it is PCP/DEI, for IPV6 it is
 * TOS for IPV4 it is DSCP
 */
shr_error_e
dnx_qos_ingress_phb_map_add(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint8 map_key;
    int dp;
    uint32 entry_handle_id;
    int opcode_index = DNX_QOS_MAP_PROFILE_GET(map_opcode);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_build_map_key(unit, flags, map_entry, &map_key));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_KEY, map_key);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, opcode_index);

    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, INST_SINGLE, map_entry->int_pri);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, INST_SINGLE, dp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos INGRESS PHB map ID & key.
 * for each profile & app & map type we have a unique key
 * structure for example for ETH it is PCP/DEI, for IPV6 it is
 * TOS for IPV4 it is DSCP
 *
 * Details: see qos_internal.h
 */
shr_error_e
dnx_qos_ingress_phb_map_delete(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint8 map_key;
    uint32 entry_handle_id;
    int opcode_ndx = DNX_QOS_MAP_PROFILE_GET(map_opcode);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_build_map_key(unit, flags, map_entry, &map_key));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES, &entry_handle_id));
    /**
     * Set dbal key : internal map_id + map_key.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_KEY, map_key);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, opcode_ndx);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos INGRESS REMARK map
 */
shr_error_e
dnx_qos_ingress_remark_map_add(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint8 map_key;
    uint32 entry_handle_id;
    int opcode_ndx = DNX_QOS_MAP_PROFILE_GET(map_opcode);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_build_map_key(unit, flags, map_entry, &map_key));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_REMARK_INTERNAL_MAP_ENTRIES, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_KEY, map_key);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, opcode_ndx);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, INST_SINGLE, map_entry->remark_int_pri);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos INGRESS REMARK map
 *
 * Details: see qos_internal.h
 */
shr_error_e
dnx_qos_ingress_remark_map_delete(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint8 map_key;
    uint32 entry_handle_id;
    int opcode_ndx = DNX_QOS_MAP_PROFILE_GET(map_opcode);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_build_map_key(unit, flags, map_entry, &map_key));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_REMARK_INTERNAL_MAP_ENTRIES, &entry_handle_id));
    /**
     * Set dbal key : internal map_id + map_key.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_KEY, map_key);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, opcode_ndx);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos INGRESS pcp explicit map for IVE
 */
static shr_error_e
dnx_qos_ingress_ive_pcp_explicit_map_add(
    int unit,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int profile_id = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_IVE_PCP_DEI_EXPLICIT_MAP, &entry_handle_id));

    /**  set key*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_DEI, map_entry->color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PCP, map_entry->int_pri);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_PCP_DEI_QOS_PROFILE, profile_id);

    /**  set new pcp dei*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_PCP, INST_SINGLE, map_entry->pkt_pri);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_DEI, INST_SINGLE, map_entry->pkt_cfi);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos INGRESS pcp explicit map for IVE
 *
 * Details: see qos_internal.h
 */
static shr_error_e
dnx_qos_ingress_ive_pcp_explicit_map_delete(
    int unit,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int profile_id = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_IVE_PCP_DEI_EXPLICIT_MAP, &entry_handle_id));

     /**  set key*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_DEI, map_entry->color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PCP, map_entry->int_pri);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_PCP_DEI_QOS_PROFILE, profile_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos INGRESS pcp map from tc/dp for IVE
 */
static shr_error_e
dnx_qos_ingress_ive_pcp_tc_dp_map_add(
    int unit,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int dp;
    int profile_id = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_IVE_UNTAG_MAP, &entry_handle_id));

    /**  set key*/
    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, map_entry->int_pri);
    /** padding bit must be set 0*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PADDING, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_PCP_DEI_QOS_PROFILE, profile_id);

    /**  set new pcp dei*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_PCP, INST_SINGLE, map_entry->pkt_pri);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_DEI, INST_SINGLE, map_entry->pkt_cfi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_PCP, INST_SINGLE, map_entry->pkt_pri);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_DEI, INST_SINGLE, map_entry->pkt_cfi);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos INGRESS pcp map from tc/dp for IVE
 */
static shr_error_e
dnx_qos_ingress_ive_pcp_tc_dp_map_delete(
    int unit,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int dp;
    int profile_id = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_IVE_UNTAG_MAP, &entry_handle_id));

     /**  set key*/
    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, map_entry->int_pri);
    /*
     * padding bit must be set 0
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PADDING, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_PCP_DEI_QOS_PROFILE, profile_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos INGRESS DP map to PCP/DEI for IVE
 */
shr_error_e
dnx_qos_ingress_policer_map_add(
    int unit,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int ingress_dp;
    int dp_profile = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ingress policer is for IVE and processed at egress, so table name is egress
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE, &entry_handle_id));

    /**
     * Set dbal key : dp profie + dp + map_key.
     */
    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &ingress_dp));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_PROFILE, dp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_DP, ingress_dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PCP,
                               (map_entry->remark_int_pri >> DNX_QOS_PRI_OFFSET));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_DEI,
                               (map_entry->remark_int_pri & DNX_QOS_CFI_MASK));
    /*
     * set dbal map value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_DEI, INST_SINGLE, map_entry->pkt_cfi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_PCP, INST_SINGLE, map_entry->pkt_pri);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos INGRESS DP map map to PCP/DEI for IVE
 */
shr_error_e
dnx_qos_ingress_policer_map_delete(
    int unit,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int ingress_dp;
    int dp_profile = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE, &entry_handle_id));
    /**
     * Set dbal key : dp profie + dp + map_key.
     */
    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &ingress_dp));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_PROFILE, dp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_DP, ingress_dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PCP,
                               (map_entry->remark_int_pri >> DNX_QOS_PRI_OFFSET));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_DEI,
                               (map_entry->remark_int_pri & DNX_QOS_CFI_MASK));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos INGRESS explicit map
 * for each profile & map type we have a unique key
 * explicit qos is in opcode index
 */
static shr_error_e
dnx_qos_ingress_explicit_map_add(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int table_id;
    int profile_index = DNX_QOS_MAP_PROFILE_GET(map_id);
    int explicit_opcode = 0;
    dbal_fields_e profile_field;
    dbal_enum_value_field_ingress_qos_map_index_e map_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_ingress_map_index_get(unit, flags, &map_index));

    if (flags & BCM_QOS_MAP_PHB)
    {
        int tc, dp;

        tc = map_entry->int_pri;
        SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));

        table_id = DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID;
        profile_field = DBAL_FIELD_QOS_PROFILE;
        /** explicit map index format
         *--------------------------------
         *| dp (2bits)| tc (3bits) | xxx |
         *--------------------------------
         */
        explicit_opcode = (tc << DNX_QOS_EXPLICIT_TC_OFFSET) | (dp << DNX_QOS_EXPLICIT_DP_OFFSET);
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID;
        profile_field = DBAL_FIELD_QOS_PROFILE;
        /** explicit map index is qos
         *----------------------
         *|    qos var (8bit)  |
         *----------------------
         */
        explicit_opcode = map_entry->remark_int_pri;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, profile_field, profile_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_QOS_MAP_INDEX, map_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, INST_SINGLE, explicit_opcode);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos INGRESS explicit map
 * for each profile & map type we have a unique key
 * explicit qos is in opcode index
 */
static shr_error_e
dnx_qos_ingress_explicit_map_delete(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    return dnx_qos_ingress_map_opcode_clear(unit, flags, map_id);
}

/**
 * \brief
 *     add ingress map entries to an allocated QOS profile
 */
shr_error_e
dnx_qos_ingress_map_add(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
    {
        if (flags & BCM_QOS_MAP_L2_UNTAGGED)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_ive_pcp_tc_dp_map_add(unit, map_id, map_entry));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_ive_pcp_explicit_map_add(unit, map_id, map_entry));
        }
    }
    else if (flags & BCM_QOS_MAP_POLICER)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_policer_map_add(unit, map_id, map_entry));
    }
    else if (((flags & BCM_QOS_MAP_L2) && (flags & BCM_QOS_MAP_L2_UNTAGGED)) || (flags & BCM_QOS_MAP_PORT))
    {
        /** use explicit mapping */
        SHR_IF_ERR_EXIT(dnx_qos_ingress_explicit_map_add(unit, flags, map_id, map_entry));
    }
    else if (flags & BCM_QOS_MAP_OPCODE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_opcode_set(unit, flags, map_id, map_entry->opcode));
    }
    else if (flags & BCM_QOS_MAP_PHB)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_phb_map_add(unit, flags, map_id, map_entry));
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_remark_map_add(unit, flags, map_id, map_entry));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "map_id[%x] not supported yet\n", map_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     delete ingress entries from an allocated QOS profile
 * \ref
 *     qos_internal.h
 */
shr_error_e
dnx_qos_ingress_map_delete(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
    {
        if (flags & BCM_QOS_MAP_L2_UNTAGGED)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_ive_pcp_tc_dp_map_delete(unit, map_id, map_entry));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_ive_pcp_explicit_map_delete(unit, map_id, map_entry));
        }
    }
    else if (flags & BCM_QOS_MAP_POLICER)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_policer_map_delete(unit, map_id, map_entry));
    }
    else if (((flags & BCM_QOS_MAP_L2) && (flags & BCM_QOS_MAP_L2_UNTAGGED)) || (flags & BCM_QOS_MAP_PORT))
    {
        /** use explicit mapping */
        SHR_IF_ERR_EXIT(dnx_qos_ingress_explicit_map_delete(unit, flags, map_id, map_entry));
    }
    else if (flags & BCM_QOS_MAP_OPCODE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_opcode_clear(unit, flags, map_id));
    }
    else if (flags & BCM_QOS_MAP_PHB)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_phb_map_delete(unit, flags, map_id, map_entry));
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_remark_map_delete(unit, flags, map_id, map_entry));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     set qos map structure with table map key
 */
static int
dnx_qos_ingress_map_key_set_map(
    int unit,
    uint32 flags,
    int key,
    bcm_qos_map_t * map)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_L2_INNER_TAG)
    {
        map->inner_pkt_pri = key >> DNX_QOS_PRI_OFFSET;
        map->inner_pkt_cfi = key & DNX_QOS_CFI_MASK;
    }
    else if (flags & BCM_QOS_MAP_L2_OUTER_TAG)
    {
        map->pkt_pri = key >> DNX_QOS_PRI_OFFSET;
        map->pkt_cfi = key & DNX_QOS_CFI_MASK;
    }
    else if (flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6))
    {
        map->dscp = key;
    }
    else if (flags & BCM_QOS_MAP_MPLS)
    {
        map->exp = key;
    }
    else if (flags & BCM_QOS_MAP_RCH)
    {
        if (flags & BCM_QOS_MAP_PHB)
        {
            map->remark_int_pri = key >> DNX_QOS_TC_OFFSET;
            map->remark_color = key & DNX_QOS_DP_MASK;
        }
        else if (flags & BCM_QOS_MAP_REMARK)
        {
            map->int_pri = key;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " [%x]: flags was not supported\n", flags);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " [%x]: layer flags was not set yet\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    to get the ingress mapping entries number for each type
 *    such as L2 untagged, L2 outer/inner tagged, L3 IPv4/IPv6, MPLS
 */
static int
dnx_qos_ingress_map_entries_num_get(
    int unit,
    uint32 flags,
    int *num_entries)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_OPCODE)
    {
        *num_entries = 1;
    }
    else if (((flags & BCM_QOS_MAP_L2) && (flags & BCM_QOS_MAP_L2_UNTAGGED)) || (flags & BCM_QOS_MAP_PORT))
    {
        /** explicit mapping*/
        *num_entries = 1;
    }
    else if (flags & BCM_QOS_MAP_POLICER)
    {
        int nof_pri = dnx_data_qos.qos.packet_max_priority_get(unit) + 1;
        int nof_cfi = dnx_data_qos.qos.packet_max_cfi_get(unit) + 1;
        int nof_dp = dnx_data_qos.qos.packet_max_dp_get(unit) + 1;
        *num_entries = nof_pri * nof_cfi * nof_dp;
        SHR_EXIT();
    }
    else if (flags & BCM_QOS_MAP_L2)
    {
        if (flags & BCM_QOS_MAP_L2_UNTAGGED)
        {
            int nof_dp = dnx_data_qos.qos.packet_max_dp_get(unit) + 1;
            int nof_tc = dnx_data_qos.qos.packet_max_tc_get(unit) + 1;
            *num_entries = nof_tc * nof_dp;
        }
        else if (flags & (BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_L2_INNER_TAG))
        {
            int nof_pri = dnx_data_qos.qos.packet_max_priority_get(unit) + 1;
            int nof_cfi = dnx_data_qos.qos.packet_max_cfi_get(unit) + 1;
            *num_entries = nof_pri * nof_cfi;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for flags %x\n", flags);
        }
    }
    else if ((flags & BCM_QOS_MAP_L3) && (flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6)))
    {
        *num_entries = dnx_data_qos.qos.packet_max_dscp_get(unit) + 1;
    }
    else if (flags & BCM_QOS_MAP_MPLS)
    {
        *num_entries = dnx_data_qos.qos.packet_max_exp_get(unit) + 1;
    }
    else if (flags & BCM_QOS_MAP_RCH)
    {
        if (flags & BCM_QOS_MAP_PHB)
        {
            *num_entries = dnx_data_qos.qos.packet_max_priority_get(unit) + 1;
        }
        else if (flags & BCM_QOS_MAP_REMARK)
        {
            *num_entries = dnx_data_qos.qos.packet_max_dscp_get(unit) + 1;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " flags %x is not supported\n", flags);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " [%x]: layer flags was not set yet\n", flags);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    to get the ingress pcp dei explicit mapping entries
 *    return mapping entry array, and number of mapping.
 */
static shr_error_e
dnx_qos_ingress_ive_pcp_explicit_map_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    uint32 entry_handle_id;
    uint32 pcp_profile;
    uint32 profile_mask[1];
    int idx = 0;
    bcm_qos_map_t *map;
    uint32 in_pcp;
    uint32 in_dei;
    uint32 out_pcp;
    uint32 out_dei;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_IVE_PCP_DEI_EXPLICIT_MAP, &entry_handle_id));
    pcp_profile = DNX_QOS_MAP_PROFILE_GET(map_id);
    profile_mask[0] = DNX_QOS_MAP_PROFILE_MASK;

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /** Add KEY rule */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_VLAN_PCP_DEI_QOS_PROFILE,
                                                           DBAL_CONDITION_EQUAL_TO, &pcp_profile, profile_mask));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        map = &array[idx++];
        bcm_qos_map_t_init(map);

        /** process the entry */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PCP, &in_pcp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DEI, &in_dei));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_PCP, INST_SINGLE, &out_pcp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_DEI, INST_SINGLE, &out_dei));

        map->int_pri = in_pcp;
        map->color = in_dei;
        map->pkt_pri = out_pcp;
        map->pkt_cfi = out_dei;

        if (idx >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    *array_count = idx;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    to get the ingress pcp dei tc/dp mapping entries
 *    return mapping entry array, and number of mapping.
 */
static shr_error_e
dnx_qos_ingress_ive_pcp_tc_dp_map_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    uint32 entry_handle_id;
    uint32 pcp_profile;
    uint32 profile_mask[1];
    int idx = 0;
    bcm_qos_map_t *map;
    uint32 tc;
    uint32 dp;
    uint32 out_pcp;
    uint32 out_dei;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_IVE_UNTAG_MAP, &entry_handle_id));
    pcp_profile = DNX_QOS_MAP_PROFILE_GET(map_id);
    profile_mask[0] = DNX_QOS_MAP_PROFILE_MASK;

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /** Add KEY rule */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_VLAN_PCP_DEI_QOS_PROFILE,
                                                           DBAL_CONDITION_EQUAL_TO, &pcp_profile, profile_mask));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        map = &array[idx++];
        bcm_qos_map_t_init(map);

        /** process the entry */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_TC, &tc));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DP, &dp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUTER_PCP, INST_SINGLE, &out_pcp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUTER_DEI, INST_SINGLE, &out_dei));

        map->int_pri = tc;
        SHR_IF_ERR_EXIT(dnx_qos_color_decode(unit, dp, &map->color));
        map->pkt_pri = out_pcp;
        map->pkt_cfi = out_dei;

        if (idx >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    *array_count = idx;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    to get the egress dp & pcp_dei mapping entries for IVE
 *    return mapping entry array, and number of mapping.
 */
static shr_error_e
dnx_qos_ingress_policer_map_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    uint32 entry_handle_id;
    int idx = 0, is_end;
    uint32 dp_profile;
    uint32 profile_mask[1];
    bcm_qos_map_t *map;
    uint32 in_pcp;
    uint32 in_dei;
    uint32 ingress_dp;
    uint32 out_pcp = 0;
    uint32 out_dei = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE, &entry_handle_id));
    dp_profile = DNX_QOS_MAP_PROFILE_GET(map_id);
    profile_mask[0] = DNX_QOS_MAP_PROFILE_MASK;

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /** Add KEY rule */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_DP_PROFILE,
                                                           DBAL_CONDITION_EQUAL_TO, &dp_profile, profile_mask));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        map = &array[idx++];
        bcm_qos_map_t_init(map);

        /** process the entry */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_PCP, &in_pcp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_DEI, &in_dei));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_INGRESS_DP, &ingress_dp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_DEI, INST_SINGLE, &out_dei));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_PCP, INST_SINGLE, &out_pcp));

        SHR_IF_ERR_EXIT(dnx_qos_color_decode(unit, ingress_dp, &map->color));
        map->remark_int_pri = (in_pcp << DNX_QOS_PRI_OFFSET) | in_dei;
        map->pkt_pri = out_pcp;
        map->pkt_cfi = out_dei;

        if (idx >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    *array_count = idx;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *    to get the ingress phb/remark/ecn mapping entries
 *    return mapping entry array, and number of mapping.
 */
static shr_error_e
dnx_qos_ingress_phb_remark_ecn_map_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    uint32 entry_handle_id;
    dbal_fields_e field_id1 = 0, field_id2;
    int idx = 0;
    uint32 data1, data2;
    uint32 opcode, map_opcode;
    uint32 opcode_mask[1];
    bcm_qos_map_t *map;
    uint32 map_key = 0;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** explicit mapping for untagged or port*/
    if (((flags & BCM_QOS_MAP_L2) && (flags & BCM_QOS_MAP_L2_UNTAGGED)) || (flags & BCM_QOS_MAP_PORT))
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_opcode_get(unit, flags, map_id, &map_opcode));
        opcode = DNX_QOS_MAP_PROFILE_GET(map_opcode);
        map = &array[idx++];
        bcm_qos_map_t_init(map);
        if (flags & BCM_QOS_MAP_PHB)
        {
            int dp;

            map->int_pri = (opcode >> DNX_QOS_EXPLICIT_TC_OFFSET) & DNX_QOS_TC_MASK;
            dp = (opcode >> DNX_QOS_EXPLICIT_DP_OFFSET) & DNX_QOS_DP_MASK;
            SHR_IF_ERR_EXIT(dnx_qos_color_decode(unit, dp, &map->color));
        }
        else if (flags & BCM_QOS_MAP_REMARK)
        {
            map->remark_int_pri = opcode;
        }
        else
            /*ECN*/
        {
            map->int_pri = (opcode >> DNX_QOS_EXPLICIT_ECN_OFFSET) & DNX_QOS_ECN_MASK;
        }
        *array_count = idx;
        SHR_EXIT();
    }

    if (DNX_QOS_MAP_IS_OPCODE(map_id))
    {
        opcode = DNX_QOS_MAP_PROFILE_GET(map_id);
        map_opcode = map_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "map id [%x] must be opcode\n", map_id);
    }

    if (flags & BCM_QOS_MAP_PHB)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES, &entry_handle_id));
        field_id1 = DBAL_FIELD_TC;
        field_id2 = DBAL_FIELD_DP;
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_REMARK_INTERNAL_MAP_ENTRIES, &entry_handle_id));
        field_id1 = DBAL_FIELD_NETWORK_QOS;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }
    opcode_mask[0] = DNX_QOS_MAP_PROFILE_MASK;

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /** Add KEY rule */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID,
                                                           DBAL_CONDITION_EQUAL_TO, &opcode, opcode_mask));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        map = &array[idx++];
        bcm_qos_map_t_init(map);

        /** process the entry */

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_KEY, &map_key));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id1, INST_SINGLE, &data1));
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_key_set_map(unit, flags, map_key, map));
        if (flags & BCM_QOS_MAP_REMARK)
        {
            map->remark_int_pri = data1;
        }
        else
        {
            map->int_pri = data1;
        }
        if (flags & BCM_QOS_MAP_PHB)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, field_id2, INST_SINGLE, &data2));
            SHR_IF_ERR_EXIT(dnx_qos_color_decode(unit, data2, &map->color));
        }

        if (idx >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    *array_count = idx;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *    to get the ingress mapping entries
 *    return mapping entry array, and number of mapping.
 *    if array_size is zero, means only to get number of entries.
 */
shr_error_e
dnx_qos_ingress_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    int num_entries = 0;
    bcm_qos_map_t *map;
    uint32 map_opcode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(array_count, _SHR_E_PARAM, "array_count Is NULL\n");
    SHR_IF_ERR_EXIT(dnx_qos_ingress_map_entries_num_get(unit, flags, &num_entries));
    if (array_size == 0)
    {
        /*
         * querying the size of map
         */
        *array_count = num_entries;
        SHR_EXIT();
    }

    SHR_NULL_CHECK(array, _SHR_E_PARAM, "array Is NULL\n");

    if (flags & BCM_QOS_MAP_OPCODE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_opcode_get(unit, flags, map_id, &map_opcode));

        map = &array[0];
        bcm_qos_map_t_init(map);

        map->opcode = map_opcode;
        *array_count = 1;

        SHR_EXIT();
    }
    if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
    {
        if (flags & BCM_QOS_MAP_L2_UNTAGGED)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_ive_pcp_tc_dp_map_get(unit, flags, map_id, array_size, array, array_count));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_ive_pcp_explicit_map_get
                            (unit, flags, map_id, array_size, array, array_count));
        }
    }
    else if (flags & BCM_QOS_MAP_POLICER)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_policer_map_get(unit, flags, map_id, array_size, array, array_count));
    }
    else if (flags & (BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK))
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_phb_remark_ecn_map_get(unit, flags, map_id, array_size, array, array_count));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " [%x]: layer flags was not set yet\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  ref  qos_internal.h
 */
shr_error_e
dnx_qos_ingress_port_map_set(
    int unit,
    bcm_gport_t port,
    int ing_map)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dbal_tables_e dbal_table_id;
    int local_in_lif;
    uint8 is_physical_port;
    uint32 entry_handle_id;
    uint32 res_type;
    uint32 profile_ndx;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Extract profile index from INGRESS map
     */
    profile_ndx = DNX_QOS_MAP_PROFILE_GET(ing_map);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_physical_port));

    if (is_physical_port)
    {
        dnx_algo_gpm_gport_phy_info_t gport_info;
        /*
         * Get Port + Core
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);

            /** write map value  according map type*/
            if (DNX_QOS_MAP_IS_PHB(ing_map) && DNX_QOS_MAP_IS_REMARK(ing_map))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PHB_QOS_PROFILE, INST_SINGLE,
                                             profile_ndx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_QOS_PROFILE, INST_SINGLE,
                                             profile_ndx);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid qos map id [0x%x]\n", profile_ndx);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    else
    {
        dbal_table_field_info_t table_field_info;

        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        local_in_lif = gport_hw_resources.local_in_lif;
        dbal_table_id = gport_hw_resources.inlif_dbal_table_id;
        res_type = gport_hw_resources.inlif_dbal_result_type;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

        /** write map value  */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);
        if (DNX_QOS_MAP_IS_PHB(ing_map) && DNX_QOS_MAP_IS_REMARK(ing_map))
        {
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, dbal_table_id, DBAL_FIELD_QOS_PROFILE, FALSE, res_type, 0,
                                                       &table_field_info));

            if (profile_ndx < (1 << table_field_info.field_nof_bits))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, profile_ndx);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "trying to set too big QOS profile ID [%d], the limit is %d bits\n",
                             profile_ndx, table_field_info.field_nof_bits);
            }
        }
        else if (DNX_QOS_MAP_IS_VLAN_PCP(ing_map))
        {
            uint32 vlan_extend_pcp_dei_profile = 0;
            uint32 pcp_dei_map_extend = DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_EXPLICIT;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, DBAL_FIELD_PCP_DEI_QOS_PROFILE,
                             &profile_ndx, &vlan_extend_pcp_dei_profile));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, DBAL_FIELD_PCP_DEI_MAP_EXTEND,
                             &pcp_dei_map_extend, &vlan_extend_pcp_dei_profile));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE,
                                         INST_SINGLE, vlan_extend_pcp_dei_profile);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid qos map id [0x%x]\n", profile_ndx);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
  *  ref  qos_internal.h
  */

shr_error_e
dnx_qos_ingress_port_map_get(
    int unit,
    bcm_gport_t port,
    int *ing_map,
    uint32 flags)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dbal_tables_e dbal_table_id;
    int local_in_lif;
    uint8 is_physical_port;
    uint32 entry_handle_id;
    uint32 res_type;
    uint32 profile_ndx = BCM_DNX_QOS_MAP_INVALID_ID;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *ing_map = BCM_DNX_QOS_MAP_INVALID_ID;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_physical_port));

    if (is_physical_port)
    {
        dnx_algo_gpm_gport_phy_info_t gport_info;

        /*
         * Get Port + Core
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[0]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[0]);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        /** phb and remark profile is same*/
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PHB_QOS_PROFILE, INST_SINGLE, &profile_ndx));
    }
    else
    {
        /** Get local in-lif */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        local_in_lif = gport_hw_resources.local_in_lif;
        dbal_table_id = gport_hw_resources.inlif_dbal_table_id;
        res_type = gport_hw_resources.inlif_dbal_result_type;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        /** read entry's profile */
        if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
        {
            uint32 extend_pcp_dei_profile;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, INST_SINGLE,
                             &extend_pcp_dei_profile));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE,
                                                            DBAL_FIELD_PCP_DEI_QOS_PROFILE, &profile_ndx,
                                                            &extend_pcp_dei_profile));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, &profile_ndx));
        }
    }

    /*
     * Check if a valid qos map id is got
     */
    if (profile_ndx != BCM_DNX_QOS_MAP_INVALID_ID)
    {
        if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
        {
            *ing_map = profile_ndx;
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_vlan_pcp.is_allocated(unit, profile_ndx, &is_allocated));
            if (is_allocated)
            {
                /** set map vlan pcp */
                DNX_QOS_VLAN_PCP_MAP_SET(*ing_map);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Valid qos map id is not found in port 0x%08X", port);
            }
        }
        else
        {
            *ing_map = profile_ndx;
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.is_allocated(unit, profile_ndx, &is_allocated));
            if (is_allocated)
            {
                /** set map PHB */
                DNX_QOS_PHB_MAP_SET(*ing_map);
                /** set map Remark */
                DNX_QOS_REMARK_MAP_SET(*ing_map);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Valid qos map id is not found in port 0x%08X", port);
            }
        }
        /** set map direction*/
        DNX_QOS_INGRESS_MAP_SET(*ing_map);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Valid qos map id is not found in port 0x%08X", port);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * convert ingress qos model to propagation type.
 * see qos.h
 */
shr_error_e
dnx_qos_ingress_qos_model_to_propag_type(
    int unit,
    bcm_qos_ingress_model_type_t qos_propag_model,
    dnx_qos_propagation_type_e * qos_propag_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (qos_propag_model)
    {
        case bcmQosIngressModelUniform:
            *qos_propag_type = DNX_QOS_PROPAGATION_TYPE_UNIFORM;
            break;
        case bcmQosIngressModelShortpipe:
            *qos_propag_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
            break;
        case bcmQosIngressModelStuck:
            *qos_propag_type = DNX_QOS_PROPAGATION_TYPE_STUCK;
            break;
        case bcmQosIngressModelPipe:
            *qos_propag_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "qos model %d is not expected", qos_propag_model);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * convert propagation type to ingress qos model.
 * see qos.h
 */
shr_error_e
dnx_qos_ingress_propag_type_to_qos_model(
    int unit,
    dnx_qos_propagation_type_e qos_propag_type,
    bcm_qos_ingress_model_type_t * qos_propag_model)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (qos_propag_type)
    {
        case DNX_QOS_PROPAGATION_TYPE_UNIFORM:
            *qos_propag_model = bcmQosIngressModelUniform;
            break;
        case DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE:
            *qos_propag_model = bcmQosIngressModelShortpipe;
            break;
        case DNX_QOS_PROPAGATION_TYPE_STUCK:
            *qos_propag_model = bcmQosIngressModelStuck;
            break;
        case DNX_QOS_PROPAGATION_TYPE_PIPE:
            *qos_propag_model = bcmQosIngressModelPipe;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "qos propagation type %d is not expected", qos_propag_type);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set propagation profile attributes for a specific ingress
 * application type PHB/REMARK/ECN/TTL.
 */
CONST qos_ingress_propagation_model_t qos_ingress_propagation_table[DNX_QOS_PROPAGATION_TYPE_NOF] = {
    /**********************************************
     *  COMPARE STRENGHT *  PROPAGATION STRENGTH  *
 */
    /** pipe */
    {DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE, DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE}
    ,
    /** short pipe */
    {DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE, DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW}
    ,
    /** uniform */
    {DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW, DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE}
    ,
     /** stuck */
    {DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH, DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH}
};

/**
 * \brief
 * set propagation profile attributes for a specific ingress
 * application type PHB/REMARK/ECN/TTL
 *
 * \param [in] unit - The relevant unit.
 * \param [in] propag_profile - The relevant propagation profile
 *             (PIPE/UNIFORM/SHORTPIPE/STUCK)
 * \param [in] ing_app_type - The relevant ingress application type
 *             (PHB/REMARK/ECN/TTL)
 * \param [in] propag_strength - The relevant propagation strength value
 * \param [in] comp_strength - The relevant propagation compare value
 *
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_ingress_propagation_profile_app_set(
    int unit,
    int propag_profile,
    dbal_enum_value_field_qos_ingress_app_type_e ing_app_type,
    uint32 propag_strength,
    uint32 comp_strength)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_PROPAG_PROFILE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, propag_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INGRESS_APP_TYPE, ing_app_type);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROPAG_STRENGTH, INST_SINGLE, propag_strength);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_COMP_STRENGTH, INST_SINGLE, comp_strength);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set the propagation profile attributes to Hardware.
 */
shr_error_e
dnx_qos_ingress_propagation_profile_hw_set(
    int unit,
    int propagation_profile,
    dnx_qos_propagation_type_e phb_propagation_type,
    dnx_qos_propagation_type_e remark_propagation_type,
    dnx_qos_propagation_type_e ecn_propagation_type,
    dnx_qos_propagation_type_e ttl_propagation_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check the validity of all propagation types .*/
    if ((phb_propagation_type >= DNX_QOS_PROPAGATION_TYPE_NOF) ||
        (remark_propagation_type >= DNX_QOS_PROPAGATION_TYPE_NOF) ||
        (ecn_propagation_type >= DNX_QOS_PROPAGATION_TYPE_NOF)
        || (ttl_propagation_type >= DNX_QOS_PROPAGATION_TYPE_NOF))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid propagation type. Available range is [0, %d)\n",
                     DNX_QOS_PROPAGATION_TYPE_NOF);
    }

    /** set the profile data to h/w. */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_app_set
                    (unit, propagation_profile, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB,
                     qos_ingress_propagation_table[phb_propagation_type].propagation_strength,
                     qos_ingress_propagation_table[phb_propagation_type].compare_stength));

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_app_set
                    (unit, propagation_profile, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK,
                     qos_ingress_propagation_table[remark_propagation_type].propagation_strength,
                     qos_ingress_propagation_table[remark_propagation_type].compare_stength));

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_app_set
                    (unit, propagation_profile, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN,
                     qos_ingress_propagation_table[ecn_propagation_type].propagation_strength,
                     qos_ingress_propagation_table[ecn_propagation_type].compare_stength));

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_app_set
                    (unit, propagation_profile, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_TTL,
                     qos_ingress_propagation_table[ttl_propagation_type].propagation_strength,
                     qos_ingress_propagation_table[ttl_propagation_type].compare_stength));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete propagation profile attributes for a specific ingress
 * application type PHB/REMARK/ECN/TTL
 * \param [in] unit -
 *     The relevant unit..
 * \param [in] propag_profile -
 *     The relevant propagation profile
 *     PIPE/UNIFORM/SHORTPIPE/STUCK)
 * \param [in] ing_app_type -
 *     The relevant ingress application type
 *     (PHB/REMARK/ECN/TTL)
 *
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_ingress_propagation_profile_app_delete(
    int unit,
    int propag_profile,
    dbal_enum_value_field_qos_ingress_app_type_e ing_app_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_PROPAG_PROFILE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, propag_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INGRESS_APP_TYPE, ing_app_type);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  clear propagation profile attributes for a specific ingress
 *  propagation profile.
 */
shr_error_e
dnx_qos_ingress_propagation_profile_hw_clear(
    int unit,
    int propagation_profile)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_app_delete
                    (unit, propagation_profile, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB));

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_app_delete
                    (unit, propagation_profile, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK));

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_app_delete
                    (unit, propagation_profile, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN));

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_app_delete
                    (unit, propagation_profile, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_TTL));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See qos.h
 */
shr_error_e
dnx_qos_ingress_propagation_profile_attr_get(
    int unit,
    int propagation_profile,
    dnx_qos_propagation_type_e * phb_propagation_type,
    dnx_qos_propagation_type_e * remark_propagation_type,
    dnx_qos_propagation_type_e * ecn_propagation_type,
    dnx_qos_propagation_type_e * ttl_propagation_type)
{
    qos_propagation_profile_template_t propagation_profile_type;
    int reference_counter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                    (unit, propagation_profile, &reference_counter, &propagation_profile_type));

    *phb_propagation_type = propagation_profile_type.phb;
    *remark_propagation_type = propagation_profile_type.remark;
    *ecn_propagation_type = propagation_profile_type.ecn;
    *ttl_propagation_type = propagation_profile_type.ttl;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Allocate a propagation profile with the given PHB/REMARK/ECN/TTL propagation
 *  parameters by exchanging.
 */
shr_error_e
dnx_qos_ingress_propagation_profile_alloc(
    int unit,
    dnx_qos_propagation_type_e phb_propagation_type,
    dnx_qos_propagation_type_e remark_propagation_type,
    dnx_qos_propagation_type_e ecn_propagation_type,
    dnx_qos_propagation_type_e ttl_propagation_type,
    int old_propagation_profile,
    int *new_propagation_profile,
    uint8 *is_first,
    uint8 *is_last)
{
    int reference_counter;
    qos_propagation_profile_template_t propagation_profile_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(new_propagation_profile, _SHR_E_INTERNAL, "A valid pointer for propagation profile is required!");
    SHR_NULL_CHECK(is_first, _SHR_E_INTERNAL, "A valid pointer for is_first is required!");
    SHR_NULL_CHECK(is_last, _SHR_E_INTERNAL, "A valid pointer for is_first is required!");

    *is_first = FALSE;
    *is_last = FALSE;

    /*
     * Get the original propagation profile data.
     */
    SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                    (unit, old_propagation_profile, &reference_counter, &propagation_profile_type));

    /*
     * update profile data
     */
    if (phb_propagation_type != DNX_QOS_PROPAGATION_TYPE_NOF)
    {
        propagation_profile_type.phb = phb_propagation_type;
    }

    if (remark_propagation_type != DNX_QOS_PROPAGATION_TYPE_NOF)
    {
        propagation_profile_type.remark = remark_propagation_type;
    }

    if (ecn_propagation_type != DNX_QOS_PROPAGATION_TYPE_NOF)
    {
        propagation_profile_type.ecn = ecn_propagation_type;
    }

    if (ttl_propagation_type != DNX_QOS_PROPAGATION_TYPE_NOF)
    {
        propagation_profile_type.ttl = ttl_propagation_type;
    }

    /*
     * Exchange a new propagation profile.
     */
    SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.exchange
                    (unit, 0, &propagation_profile_type, old_propagation_profile,
                     NULL, new_propagation_profile, is_first, is_last));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Free qos propagation profile reference according to the
 * requested propagation type of PHB/REMARK/ECN/TTL combination.
 * the reference counter decrement (this is done using the
 * template manager).
 */
static shr_error_e
dnx_qos_ingress_propagation_profile_free(
    int unit,
    int propagation_profile,
    uint8 *last_reference)
{
    int new_propag_profile;
    int ref_count;
    uint8 is_first;
    qos_propagation_profile_template_t propagation_profile_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(last_reference, _SHR_E_INTERNAL, "A valid pointer for last_reference is required!");

    *last_reference = FALSE;

    /*
     * Free the propagation profile by exchanging with default data.
     */
    new_propag_profile = DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE;
    SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                    (unit, new_propag_profile, &ref_count, &propagation_profile_type));

    SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.exchange
                    (unit, 0, &propagation_profile_type, propagation_profile,
                     NULL, &new_propag_profile, &is_first, last_reference));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Free qos propagation profile reference according to the
 * requested propagation type of PHB/REMARK/ECN/TTL combination.
 * the reference counter decrement (this is done using the
 * template manager).
 * If it is the last reference, clear hardware table.
 */
shr_error_e
dnx_qos_ingress_propagation_profile_free_and_hw_clear(
    int unit,
    int propagation_profile,
    uint8 *last_reference)
{
    uint8 is_last;

    SHR_FUNC_INIT_VARS(unit);

    is_last = FALSE;

    /*
     * Free the propagation profile by exchanging with default data.
     */

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_free(unit, propagation_profile, &is_last));

    if (last_reference != NULL)
    {
        *last_reference = is_last;
    }

    if (is_last == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_clear(unit, propagation_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See qos.h. for reference
 */
shr_error_e
dnx_qos_ingress_port_model_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value)
{
    uint8 profile_strength;
    uint32 entry_handle_id;
    dbal_fields_e field;
    uint32 pp_port_index;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (value)
    {
        case bcmQosIngressModelShortpipe:
        {
            profile_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW;
            break;
        }
        case bcmQosIngressModelPipe:
        {
            profile_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE;
            break;
        }
        case bcmQosIngressModelStuck:
        {
            profile_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "qos model %d is not supported", value);
    }

    switch (type)
    {
        case bcmPortControlIngressQosModelPhb:
        {
            field = DBAL_FIELD_PHB_STRENGTH;
            break;
        }
        case bcmPortControlIngressQosModelRemark:
        {
            field = DBAL_FIELD_REMARK_STRENGTH;
            break;
        }
        case bcmPortControlIngressQosModelTtl:
        {
            field = DBAL_FIELD_TTL_STRENGTH;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qos port control type (%d) is not expected!", type);
            break;
        }
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Set profile strength to HW */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);

        dbal_entry_value_field8_set(unit, entry_handle_id, field, INST_SINGLE, profile_strength);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_qos_ingress_port_model_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value)
{
    uint8 profile_strength;
    uint32 entry_handle_id;
    dbal_fields_e field;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    *value = bcmQosIngressModelInvalid;
    switch (type)
    {
        case bcmPortControlIngressQosModelPhb:
        {
            field = DBAL_FIELD_PHB_STRENGTH;
            break;
        }
        case bcmPortControlIngressQosModelRemark:
        {
            field = DBAL_FIELD_REMARK_STRENGTH;
            break;
        }
        case bcmPortControlIngressQosModelTtl:
        {
            field = DBAL_FIELD_TTL_STRENGTH;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qos port control type (%d) is not expected!", type);
            break;
        }
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Set profile strength to HW */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);

    dbal_value_field8_request(unit, entry_handle_id, field, INST_SINGLE, &profile_strength);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    switch (profile_strength)
    {
        case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW:
        {
            *value = bcmQosIngressModelShortpipe;
            break;
        }
        case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE:
        {
            *value = bcmQosIngressModelPipe;
            break;
        }
        case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH:
        {
            *value = bcmQosIngressModelStuck;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "qos model %d is not expected", profile_strength);
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    set qos profile mapping to cs profile.
 */
static shr_error_e
dnx_qos_ingress_cs_profile_map_set(
    int unit,
    int table_id,
    int profile,
    uint32 cs_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, profile);

    /** set context selection profile*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_CS_PROFILE, INST_SINGLE, cs_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    get CS profile according to qos profile.
 */
shr_error_e
dnx_qos_ingress_cs_profile_map_get(
    int unit,
    int table_id,
    int profile,
    uint32 *cs_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, profile);

    /** get context selection profile*/
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QOS_CS_PROFILE, INST_SINGLE, cs_profile);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    set qos profile name space.
 */
static shr_error_e
dnx_qos_ingress_profile_sw_set(
    int unit,
    int profile,
    int app_type,
    uint32 name_space)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_PROFIILE_SW_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INGRESS_APP_TYPE, app_type);

    /** set profile name space*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE_NAME_SPACE, INST_SINGLE, name_space);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    get qos profile name space.
 */
static shr_error_e
dnx_qos_ingress_profile_sw_get(
    int unit,
    int profile,
    int app_type,
    uint32 *name_space)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_PROFIILE_SW_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INGRESS_APP_TYPE, app_type);

    /** set profile name space*/
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE_NAME_SPACE, INST_SINGLE, name_space);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    remove qos profile name space.
 */
shr_error_e
dnx_qos_ingress_profile_sw_remove(
    int unit,
    int profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_PROFIILE_SW_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, profile);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_QOS_INGRESS_APP_TYPE,
                                     DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB,
                                     DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    ingress qos map control set, see qos_internal.h .
 */
shr_error_e
dnx_qos_ingress_map_control_clear(
    int unit,
    int profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * clear SW state information
     */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_profile_sw_remove(unit, profile));

    /*
     * clear CS profile mapping
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    ingress qos map control set, see qos_internal.h .
 */
int
dnx_qos_ingress_map_control_set(
    int unit,
    uint32 map_id,
    uint32 flags,
    bcm_qos_map_control_type_t type,
    int arg)
{
    uint32 cs_profile;
    int table_id;
    int app_type;
    int profile = DNX_QOS_MAP_PROFILE_GET(map_id);
    uint32 name_space;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_PHB)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO;
        app_type = DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB;
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_INFO;
        app_type = DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }
    SHR_IF_ERR_EXIT(dnx_qos_ingress_profile_sw_get(unit, profile, app_type, &name_space));
    switch (type)
    {
        case bcmQosMapControlL3L2:
        {
            if ((name_space != DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT) &&
                (name_space != DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_L3_L2) && (arg != FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "This profile is used for %s, please set it to disable(default) first or create another profile! \n",
                             profile_namespace_strings[name_space]);
            }
            if (arg == FALSE)
            {
                cs_profile = DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT;
                name_space = DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT;
            }
            else
            {
                cs_profile = DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2;
                name_space = DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_L3_L2;;
            }
            break;
        }
        case bcmQosMapControlL2TwoTagsMode:
        {
            if ((name_space != DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT) &&
                (name_space != DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_ETHERNET) &&
                (arg != bcmQosMapControlL2TwoTagsModeOuterOnly))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "This profile is used for %s, please set it to disable(default) first or create another profile! \n",
                             profile_namespace_strings[name_space]);
            }
            switch (arg)
            {
                case bcmQosMapControlL2TwoTagsModeDoubleTag:
                {
                    cs_profile = DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE;
                    name_space = DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_ETHERNET;
                    break;
                }
                case bcmQosMapControlL2TwoTagsModeInnerOnly:
                {
                    cs_profile = DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_INNER;
                    name_space = DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_ETHERNET;
                    break;
                }
                case bcmQosMapControlL2TwoTagsModeOuterOnly:
                {
                    cs_profile = DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT;
                    name_space = DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "arg (%d) is invalid! \n", arg);
                }
            }
            break;
        }
        case bcmQosMapControlMplsPortModeServiceTag:
        {
            if ((name_space != DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT) &&
                (name_space != DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_TAGGED_PWE) &&
                (arg != bcmQosMapControlMplsPortModeServiceTagDisable))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "This profile is used for %s, please set it to disable(default) first or create another profile! \n",
                             profile_namespace_strings[name_space]);
            }
            switch (arg)
            {
                case bcmQosMapControlMplsPortModeServiceTagTypical:
                {
                    cs_profile = DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_TYPICAL;
                    name_space = DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_TAGGED_PWE;
                    break;
                }
                case bcmQosMapControlMplsPortModeServiceTagOuter:
                {
                    cs_profile = DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER;
                    name_space = DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_TAGGED_PWE;
                    break;
                }
                case bcmQosMapControlMplsPortModeServiceTagDisable:
                {
                    cs_profile = DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT;
                    name_space = DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "arg (%d) is invalid! \n", arg);
                }
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "type(%d) is invalid! \n", type);
        }
    }

    SHR_IF_ERR_EXIT(dnx_qos_ingress_profile_sw_set(unit, profile, app_type, name_space));
    SHR_IF_ERR_EXIT(dnx_qos_ingress_cs_profile_map_set(unit, table_id, profile, cs_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    ingress qos map control get, see qos_internal.h .
 */
int
dnx_qos_ingress_map_control_get(
    int unit,
    uint32 map_id,
    uint32 flags,
    bcm_qos_map_control_type_t type,
    int *arg)
{
    uint32 cs_profile;
    int table_id;
    int profile = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    if (flags & BCM_QOS_MAP_PHB)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO;
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        table_id = DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_INFO;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }

    SHR_IF_ERR_EXIT(dnx_qos_ingress_cs_profile_map_get(unit, table_id, profile, &cs_profile));
    switch (type)
    {
        case bcmQosMapControlL3L2:
        {
            if (cs_profile == DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2)
            {
                *arg = TRUE;
            }
            else
            {
                *arg = FALSE;
            }
            break;
        }
        case bcmQosMapControlL2TwoTagsMode:
        {
            switch (cs_profile)
            {
                case DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE:
                    *arg = bcmQosMapControlL2TwoTagsModeDoubleTag;
                    break;
                case DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_INNER:
                    *arg = bcmQosMapControlL2TwoTagsModeInnerOnly;
                    break;
                default:
                    *arg = bcmQosMapControlL2TwoTagsModeOuterOnly;
                    break;
            }
            break;
        }
        case bcmQosMapControlMplsPortModeServiceTag:
        {
            switch (cs_profile)
            {
                case DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_TYPICAL:
                    *arg = bcmQosMapControlMplsPortModeServiceTagTypical;
                    break;
                case DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER:
                    *arg = bcmQosMapControlMplsPortModeServiceTagOuter;
                    break;
                default:
                    *arg = bcmQosMapControlMplsPortModeServiceTagDisable;
                    break;
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "type is invalid! \n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set the QoS attributes(remark_profile, TTL_MODEL, ttl, etc.) used in egress
 *  encapsulation for the cases in which EEI is used for MPLS encapsulation.
 * \see
 *   qos_internal.h
 */
shr_error_e
dnx_qos_ingress_control_mpls_swap_qos_attributes_set(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int arg)
{
    uint32 entry_handle_id;
    uint32 profile_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (type)
    {
        /** remark_profile for MPLS ingress swap.*/
        case bcmQosControlMplsIngressSwapRemarkProfile:
        {
            profile_index = DNX_QOS_MAP_PROFILE_GET(arg);

            /** set egr_map to HW */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_MPLS_FHEI_TO_EES_RESOLUTION, &entry_handle_id));

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SWAP_REMARK,
                                         DNX_EEI_IDENTIFIER_SWAP, profile_index);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            break;
        }

        /** TTL model used by EEI.MPLS which is used for MPLS encap or swap.*/
        case bcmQosControlMplsIngressSwapTtlModel:
        {
            uint32 ttl_mode, ttl_decrement_disable, field_value;

            if (arg == bcmQosEgressModelUniform)
            {
                ttl_mode = DBAL_ENUM_FVAL_TTL_MODE_UNIFORM;
            }
            else
            {
                ttl_mode = DBAL_ENUM_FVAL_TTL_MODE_PIPE;
            }

            ttl_decrement_disable = 0;
            ttl_mode = ttl_mode & DNX_QOS_BIT_MASK_PREFIX_TTL_MODE;

            /** set the value on prefix[7:6] */
            field_value =
                ttl_mode << DNX_QOS_BIT_SHIFT_PREFIX_TTL_MODE | ttl_decrement_disable <<
                DNX_QOS_BIT_SHIFT_PREFIX_TTL_DECREMENT_DISABLE;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FHEI_ARR_PREFIX, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPS_FHEI_FIELD,
                                       DBAL_ENUM_FVAL_ETPS_FHEI_FIELD_TTL_MODEL);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PREFIX_VALUE, INST_SINGLE, field_value);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            break;
        }

        /** TTL value used by EEI.MPLS which is used for MPLS encap or swap.*/
        case bcmQosControlMplsIngressSwapTtl:
        {
            uint32 ttl_profile;

            ttl_profile = dnx_data_qos.qos.eei_mpls_ttl_profile_get(unit);
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set(unit, ttl_profile, (uint32) arg));

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Bcm QoS control type is not supported! \n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get the QoS attributes(remark_profile, TTL_MODEL, ttl, etc.) used in egress
 *  encapsulation for the cases in which EEI is used for MPLS encapsulation.
 *
 * \see
 *   qos_internal.h
 */
shr_error_e
dnx_qos_ingress_control_mpls_swap_qos_attributes_get(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int *arg)
{
    uint32 entry_handle_id;
    uint32 profile_index;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    switch (type)
    {
        /** remark_profile for MPLS ingress swap.*/
        case bcmQosControlMplsIngressSwapRemarkProfile:
        {
            *arg = BCM_DNX_QOS_MAP_INVALID_ID;
            profile_index = BCM_DNX_QOS_MAP_INVALID_ID;

            /*
             * Get qos map id from HW
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_MPLS_FHEI_TO_EES_RESOLUTION, &entry_handle_id));

            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FHEI_SWAP_REMARK,
                                       DNX_EEI_IDENTIFIER_SWAP, &profile_index);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            /*
             * Check if a valid qos map id is got
             */
            if (profile_index != BCM_DNX_QOS_MAP_INVALID_ID)
            {
                SHR_IF_ERR_EXIT(algo_qos_db.egress_qos.is_allocated(unit, profile_index, &is_allocated));
                if (!is_allocated)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Valid egress REMARK map ID is not found\n");
                }

                *arg = profile_index;

                /** set map remark  */
                DNX_QOS_REMARK_MAP_SET(*arg);

                /** set map direction */
                DNX_QOS_EGRESS_MAP_SET(*arg);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Valid egress REMARK map ID is not found\n");
            }
            break;
        }

        /** TTL model used by EEI.MPLS which is used for MPLS encap or swap.*/
        case bcmQosControlMplsIngressSwapTtlModel:
        {
            uint32 ttl_mode, field_value;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FHEI_ARR_PREFIX, &entry_handle_id));
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_ETPS_FHEI_FIELD,
                                      DBAL_ENUM_FVAL_ETPS_FHEI_FIELD_TTL_MODEL);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PREFIX_VALUE, INST_SINGLE, &field_value);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            /** ttl_mode is is prefix[6]*/
            ttl_mode = (field_value >> DNX_QOS_BIT_SHIFT_PREFIX_TTL_MODE) & DNX_QOS_BIT_MASK_PREFIX_TTL_MODE;

            if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_UNIFORM)
            {
                *arg = bcmQosEgressModelUniform;
            }
            else
            {
                *arg = bcmQosEgressModelPipeMyNameSpace;
            }
            break;
        }

        /** TTL value used by EEI.MPLS which is used for MPLS encap or swap.*/
        case bcmQosControlMplsIngressSwapTtl:
        {
            uint32 ttl_profile, ttl_value;
            ttl_profile = dnx_data_qos.qos.eei_mpls_ttl_profile_get(unit);

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_TTL_PIPE_MAPPING, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, ttl_profile);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TTL, INST_SINGLE, &ttl_value);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            *arg = (int) ttl_value;

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Bcm QoS control type is not supported! \n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * convert ingress qos model to compare/propagation strength.
 */
static shr_error_e
dnx_qos_ingress_qos_model_to_strength(
    int unit,
    bcm_qos_ingress_model_type_t qos_propag_model,
    int *comp_strength,
    int *prop_strength)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(comp_strength, _SHR_E_INTERNAL, "comp_strength");
    SHR_NULL_CHECK(prop_strength, _SHR_E_INTERNAL, "prop_strength");

    switch (qos_propag_model)
    {
        case bcmQosIngressModelShortpipe:
        {
            *comp_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE;
            *prop_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW;
            break;
        }
        case bcmQosIngressModelPipe:
        {
            *comp_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE;
            *prop_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE;
            break;
        }
        case bcmQosIngressModelUniform:
        {
            *comp_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW;
            *prop_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE;
            break;
        }
        case bcmQosIngressModelStuck:
        {
            *comp_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH;
            *prop_strength = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "qos model %d is not expected", qos_propag_model);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * convert compare/propagation strength to ingress qos model.
 */
static shr_error_e
dnx_qos_ingress_strength_to_qos_model(
    int unit,
    int comp_strength,
    int prop_strength,
    bcm_qos_ingress_model_type_t * qos_model)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qos_model, _SHR_E_INTERNAL, "qos_model");

    *qos_model = bcmQosIngressModelInvalid;

    switch (comp_strength)
    {
        case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW:
        {
            switch (prop_strength)
            {
                case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW:
                {
                    *qos_model = bcmQosIngressModelInvalid;
                    break;
                }
                case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE:
                {
                    *qos_model = bcmQosIngressModelUniform;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "compare strength %d, prop strength %d is not expected",
                                 comp_strength, prop_strength);
                }
            }
            break;
        }
        case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE:
        {
            switch (prop_strength)
            {
                case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW:
                {
                    *qos_model = bcmQosIngressModelShortpipe;
                    break;
                }
                case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE:
                {
                    *qos_model = bcmQosIngressModelPipe;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "compare strength %d, prop strength %d is not expected",
                                 comp_strength, prop_strength);
                }
            }
            break;
        }
        case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH:
        {
            switch (prop_strength)
            {
                case DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH:
                {
                    *qos_model = bcmQosIngressModelStuck;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "compare strength %d, prop strength %d is not expected",
                                 comp_strength, prop_strength);
                }
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "compare strength %d is not expected", comp_strength);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set explicit null label qos attribute.
 *
 * \see
 *   qos_internal.h
 */
shr_error_e
dnx_qos_ingress_control_explicit_null_label_qos_attributes_set(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int arg)
{
    int comp_strength = 0;
    int prop_strength = 0;
    int profile;
    uint32 entry_handle_id;
    bcm_qos_ingress_model_type_t qos_model;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES, &entry_handle_id));

    if (_SHR_IS_FLAG_SET(flags, BCM_QOS_MAP_IPV6))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VTT_MPLS_SPECIAL_LABEL_PROFILE,
                                   DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV6_EXPLICIT_NULL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VTT_MPLS_SPECIAL_LABEL_PROFILE,
                                   DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV4_EXPLICIT_NULL);
    }

    qos_model = arg;
    switch (type)
    {
        case bcmQosControlMplsExplicitNullIngressQosProfile:
        {
            profile = DNX_QOS_MAP_PROFILE_GET(arg);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_MAPPING_PROFILE, INST_SINGLE, profile);
            break;
        }
        case bcmQosControlMplsExplicitNullIngressPhbModel:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_strength(unit, qos_model, &comp_strength, &prop_strength));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PHB_COMPARE_STRENGTH, INST_SINGLE,
                                         comp_strength);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PHB_PROPAGATION_STRENGTH, INST_SINGLE,
                                         prop_strength);
            break;
        }
        case bcmQosControlMplsExplicitNullIngressRemarkModel:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_strength(unit, qos_model, &comp_strength, &prop_strength));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_COMPARE_STRENGTH, INST_SINGLE,
                                         comp_strength);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROPAGATION_STRENGTH, INST_SINGLE,
                                         prop_strength);
            break;
        }
        case bcmQosControlMplsExplicitNullIngressTtlModel:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_strength(unit, qos_model, &comp_strength, &prop_strength));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_COMPARE_STRENGTH, INST_SINGLE,
                                         comp_strength);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PROPAGATION_STRENGTH, INST_SINGLE,
                                         prop_strength);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid control type\n");
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  get explicit null label qos attribute.
 *
 * \see
 *    qos_internal.h
 */
shr_error_e
dnx_qos_ingress_control_explicit_null_label_qos_attributes_get(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int *arg)
{

    uint32 comp_strength = 0;
    uint32 prop_strength = 0;
    uint32 map_id;
    uint32 entry_handle_id;
    bcm_qos_ingress_model_type_t qos_model;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES, &entry_handle_id));

    if (_SHR_IS_FLAG_SET(flags, BCM_QOS_MAP_IPV6))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VTT_MPLS_SPECIAL_LABEL_PROFILE,
                                   DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV6_EXPLICIT_NULL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VTT_MPLS_SPECIAL_LABEL_PROFILE,
                                   DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV4_EXPLICIT_NULL);
    }

    switch (type)
    {
        case bcmQosControlMplsExplicitNullIngressQosProfile:
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QOS_MAPPING_PROFILE, INST_SINGLE, &map_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (map_id != 0)
            {
                DNX_QOS_PHB_MAP_SET(map_id);
                DNX_QOS_REMARK_MAP_SET(map_id);
                DNX_QOS_INGRESS_MAP_SET(map_id);
                *arg = (int) map_id;
            }
            break;
        }
        case bcmQosControlMplsExplicitNullIngressPhbModel:
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PHB_COMPARE_STRENGTH, INST_SINGLE,
                                       &comp_strength);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PHB_PROPAGATION_STRENGTH, INST_SINGLE,
                                       &prop_strength);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(dnx_qos_ingress_strength_to_qos_model(unit, comp_strength, prop_strength, &qos_model));
            *arg = qos_model;
            break;
        }
        case bcmQosControlMplsExplicitNullIngressRemarkModel:
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REMARK_COMPARE_STRENGTH, INST_SINGLE,
                                       &comp_strength);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REMARK_PROPAGATION_STRENGTH, INST_SINGLE,
                                       &prop_strength);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(dnx_qos_ingress_strength_to_qos_model(unit, comp_strength, prop_strength, &qos_model));
            *arg = qos_model;
            break;
        }
        case bcmQosControlMplsExplicitNullIngressTtlModel:
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TTL_COMPARE_STRENGTH, INST_SINGLE,
                                       &comp_strength);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TTL_PROPAGATION_STRENGTH, INST_SINGLE,
                                       &prop_strength);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(dnx_qos_ingress_strength_to_qos_model(unit, comp_strength, prop_strength, &qos_model));
            *arg = qos_model;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid control type\n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set the dscp preserve in MPLS PHP pop case .
 */
shr_error_e
dnx_qos_ingress_control_mpls_pop_dscp_preserve_set(
    int unit,
    uint32 flags,
    int arg)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_first_nibble_index_e nibble_index;
    uint32 forward_code;
    uint32 is_mc;
    uint32 preserve;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    preserve = arg ? 1 : 0;
    if (flags & BCM_QOS_MAP_IP_MC)
    {
        is_mc = TRUE;
    }
    else
    {
        is_mc = FALSE;
    }
    if (flags & BCM_QOS_MAP_IPV6)
    {
        nibble_index = DBAL_ENUM_FVAL_FIRST_NIBBLE_INDEX_IPV6;
        if (preserve & is_mc)
        {
            forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R1;
        }
        else if (preserve)
        {
            forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R1;
        }
        else if (is_mc)
        {
            forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R0;
        }
        else
        {
            forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R0;
        }
    }
    else
    {
        nibble_index = DBAL_ENUM_FVAL_FIRST_NIBBLE_INDEX_IPV4;
        if (preserve & is_mc)
        {
            forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R1;
        }
        else if (preserve)
        {
            forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R1;
        }
        else if (is_mc)
        {
            forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R0;
        }
        else
        {
            forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R0;
        }
    }

    /** set forwarding code for preserve or un-preserve*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_PHP_MAP_FWD_CODE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, nibble_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE, forward_code);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get if dscp preserve in MPLS PHP pop case
 */
shr_error_e
dnx_qos_ingress_control_mpls_pop_dscp_preserve_get(
    int unit,
    uint32 flags,
    int *arg)
{

    uint32 entry_handle_id;
    dbal_enum_value_field_first_nibble_index_e nibble_index;
    uint32 forward_code;
    uint32 is_mc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_IPV6)
    {
        nibble_index = DBAL_ENUM_FVAL_FIRST_NIBBLE_INDEX_IPV6;
    }
    else
    {
        nibble_index = DBAL_ENUM_FVAL_FIRST_NIBBLE_INDEX_IPV4;
    }

    if (flags & BCM_QOS_MAP_IP_MC)
    {
        is_mc = TRUE;
    }
    else
    {
        is_mc = FALSE;
    }

    /** get forwarding code*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_PHP_MAP_FWD_CODE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_NIBBLE_INDEX, nibble_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE, &forward_code);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *arg = FALSE;
    switch (forward_code)
    {
        case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R1:
        case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R1:
        case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R1:
        case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R1:
        {
            *arg = TRUE;
            break;
        }
        default:
        {
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
