/*! \file bcm56880_a0_dna_4_6_6_qos.c
 *
 * BCM56880_A0 DNA_4_6_6 QoS driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/control.h>

#include <bcm_int/ltsw/mbcm/qos.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/qos_int.h>
#include <bcm_int/ltsw/dev.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_QOS

/*! QoS remarking table number. */
#define QOS_REMARK_TABLE_NUM 12

/*! Strength profile table index for INT_PRI. */
#define QOS_ING_STR_INT_PRI_PRF_IDX 0x7

/*! Strength profile table index for CNG. */
#define QOS_ING_STR_CNG_PRF_IDX 0x6

/* QoS ingress policy table key id. */
typedef enum qos_ing_policy_id_e {
    FLEX_QOS_QOS_DISABLE_ECN_DISABLE_3_INDEX            = 0,
    FLEX_QOS_QOS_DISABLE_ECN_DISABLE_2_INDEX            = 1,
    FLEX_QOS_QOS_MPLS_EXP_MPLS_ECN_INDEX                = 2,
    FLEX_QOS_QOS_MPLS_EXP_L2_TUNNEL_ECN_INDEX           = 3,
    FLEX_QOS_QOS_MPLS_EXP_L3_TUNNEL_ECN_INDEX           = 4,
    FLEX_QOS_QOS_DISABLE_ECN_DISABLE_1_INDEX            = 5,
    FLEX_QOS_QOS_L3_IIF_DSCP_MPLS_ECN_INDEX             = 6,
    FLEX_QOS_QOS_L3_IIF_DSCP_L3_TUNNEL_ECN_INDEX        = 7,
    FLEX_QOS_QOS_L3_IIF_DSCP_L2_TUNNEL_ECN_INDEX        = 8,
    FLEX_QOS_QOS_L3_IIF_DSCP_NON_TUNNEL_ECN_INDEX       = 9,
    FLEX_QOS_QOS_SVP_DSCP_MPLS_ECN_INDEX                = 10,
    FLEX_QOS_QOS_SVP_DSCP_L3_TUNNEL_ECN_INDEX           = 11,
    FLEX_QOS_QOS_SVP_DSCP_L2_TUNNEL_ECN_INDEX           = 12,
    FLEX_QOS_QOS_SVP_DSCP_NON_TUNNEL_ECN_INDEX          = 13,
    FLEX_QOS_QOS_SVP_IDOT1P_CFI_MPLS_ECN_INDEX          = 14,
    FLEX_QOS_QOS_SVP_IDOT1P_CFI_L3_TUNNEL_ECN_INDEX     = 15,
    FLEX_QOS_QOS_SVP_IDOT1P_CFI_L2_TUNNEL_ECN_INDEX     = 16,
    FLEX_QOS_QOS_SVP_IDOT1P_CFI_NON_TUNNEL_ECN_INDEX    = 17,
    FLEX_QOS_QOS_SVP_IDOT1P_CFI_ECN_DISABLE_INDEX       = 18,
    FLEX_QOS_QOS_SVP_ODOT1P_CFI_MPLS_ECN_INDEX          = 19,
    FLEX_QOS_QOS_SVP_ODOT1P_CFI_L3_TUNNEL_ECN_INDEX     = 20,
    FLEX_QOS_QOS_SVP_ODOT1P_CFI_L2_TUNNEL_ECN_INDEX     = 21,
    FLEX_QOS_QOS_SVP_ODOT1P_CFI_NON_TUNNEL_ECN_INDEX    = 22,
    FLEX_QOS_QOS_SVP_ODOT1P_CFI_ECN_DISABLE_INDEX       = 23,
    FLEX_QOS_QOS_L2_IIF_DSCP_MPLS_ECN_INDEX             = 24,
    FLEX_QOS_QOS_L2_IIF_DSCP_L3_TUNNEL_ECN_INDEX        = 25,
    FLEX_QOS_QOS_L2_IIF_DSCP_L2_TUNNEL_ECN_INDEX        = 26,
    FLEX_QOS_QOS_L2_IIF_DSCP_NON_TUNNEL_ECN_INDEX       = 27,
    FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_MPLS_ECN_INDEX       = 28,
    FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_L3_TUNNEL_ECN_INDEX  = 29,
    FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_L2_TUNNEL_ECN_INDEX  = 30,
    FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_NON_TUNNEL_ECN_INDEX = 31,
    FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_ECN_DISABLE_INDEX    = 32,
    FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_MPLS_ECN_INDEX       = 33,
    FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_L3_TUNNEL_ECN_INDEX  = 34,
    FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_L2_TUNNEL_ECN_INDEX  = 35,
    FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_NON_TUNNEL_ECN_INDEX = 36,
    FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_ECN_DISABLE_INDEX    = 37,
    FLEX_QOS_QOS_DISABLE_ECN_DISABLE_0_INDEX            = 38,
    FLEX_QOS_ING_POLICY_ID_CNT                          = 39
} qos_ing_policy_id_t;

static const int qos_ing_policy_field[] = {QOS_ING_STR_INT_PRI_PRF_IDX,
                                           QOS_ING_STR_CNG_PRF_IDX};
/*
 * Ingress policy table fields configuration.
 */
static const bcmint_qos_tbl_ent_t qos_ing_policy_tbl_ent[] = {
    {
        .key = {FLEX_QOS_QOS_DISABLE_ECN_DISABLE_3_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_DISABLE_ECN_DISABLE_2_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_MPLS_EXP_MPLS_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_MPLS_EXP_L2_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_MPLS_EXP_L3_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_DISABLE_ECN_DISABLE_1_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L3_IIF_DSCP_MPLS_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L3_IIF_DSCP_L3_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L3_IIF_DSCP_L2_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L3_IIF_DSCP_NON_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_DSCP_MPLS_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_DSCP_L3_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_DSCP_L2_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_DSCP_NON_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_IDOT1P_CFI_MPLS_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_IDOT1P_CFI_L3_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_IDOT1P_CFI_L2_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_IDOT1P_CFI_NON_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_IDOT1P_CFI_ECN_DISABLE_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_ODOT1P_CFI_MPLS_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_ODOT1P_CFI_L3_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_ODOT1P_CFI_L2_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_ODOT1P_CFI_NON_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_SVP_ODOT1P_CFI_ECN_DISABLE_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_DSCP_MPLS_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_DSCP_L3_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_DSCP_L2_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_DSCP_NON_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_MPLS_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_L3_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_L2_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_NON_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_IDOT1P_CFI_ECN_DISABLE_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_MPLS_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_L3_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_L2_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_NON_TUNNEL_ECN_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_L2_IIF_ODOT1P_CFI_ECN_DISABLE_INDEXs},
        .field = qos_ing_policy_field
    },
    {
        .key = {FLEX_QOS_QOS_DISABLE_ECN_DISABLE_0_INDEXs},
        .field = qos_ing_policy_field
    },
};

/*
 * Ingress policy table configuration.
 */
static const bcmint_qos_tbl_cfg_t qos_ing_policy_tbl_cfg[] = {
    {
        .key_name = {FLEX_QOS_ING_POLICY_IDs},
        .field_num = 2,
        .field_array = {FLEX_QOS_ING_PHB_INT_PRI_STRENGTH_PROFILE_IDs,
                        FLEX_QOS_ING_PHB_INT_CNG_STRENGTH_PROFILE_IDs},
        .ent_num = FLEX_QOS_ING_POLICY_ID_CNT,
        .ent = qos_ing_policy_tbl_ent
    }
};

/* QoS egress policy table key id. */
typedef enum qos_egr_policy_id_e {
    FLEX_QOS_QOS_REMARK_L3_MPLS_RESPONSIVE_1_INDEX     = 0,
    FLEX_QOS_QOS_REMARK_L3_MPLS_NON_RESPONSIVE_1_INDEX = 1,
    FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_1_INDEX     = 2,
    FLEX_QOS_QOS_REMARK_L2_MPLS_RESPONSIVE_INDEX       = 3,
    FLEX_QOS_QOS_REMARK_L2_MPLS_NON_RESPONSIVE_INDEX   = 4,
    FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_0_INDEX     = 5,
    FLEX_QOS_QOS_REMARK_L3_MPLS_RESPONSIVE_0_INDEX     = 6,
    FLEX_QOS_QOS_REMARK_L3_MPLS_NON_RESPONSIVE_0_INDEX = 7,
    FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_D1P_COPY_INDEX = 8,
    FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_INDEX         = 9,
    FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_TOS_COPY_INDEX = 10,
    FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_D1P_TOS_COPY_INDEX = 11,
    FLEX_QOS_QOS_REMARK_L2_TUNNEL_ENCAP_D1P_TOS_COPY_INDEX = 12,
    FLEX_QOS_QOS_REMARK_L2_TUNNEL_ENCAP_D1P_COPY_INDEX = 13,
    FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_TOS_COPY_0_INDEX = 14,
    FLEX_QOS_QOS_REMARK_L3_TUNNEL_ENCAP_D1P_TOS_COPY_INDEX = 15,
    FLEX_QOS_QOS_REMARK_L3_TUNNEL_ENCAP_D1P_COPY_INDEX = 16,
    FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_TOS_COPY_1_INDEX = 17,
    FLEX_QOS_EGR_POLICY_ID_CNT                         = 18
} qos_egr_policy_id_t;

/* Define the default egress policy table base index. */
#define SRC_DEFAULT FLEX_QOS_EGR_BASE_INDEX_DEFAULTs

/* Define egress policy table base index source enum. */
#define SRC_ENUM(n) (FLEX_QOS_EGR_BASE_INDEX_##n##s)

/*
 * Egress policy table fields configuration.
 */
static const bcmint_qos_tbl_ent_t qos_egr_policy_tbl_ent[] = {
    [FLEX_QOS_QOS_REMARK_L3_MPLS_RESPONSIVE_1_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L3_MPLS_RESPONSIVE_1_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(2)}
    },
    [FLEX_QOS_QOS_REMARK_L3_MPLS_NON_RESPONSIVE_1_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L3_MPLS_NON_RESPONSIVE_1_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(2)}
    },
    [FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_1_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_1_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(2), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(4), \
                      SRC_ENUM(4), SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT}
    },
    [FLEX_QOS_QOS_REMARK_L2_MPLS_RESPONSIVE_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_MPLS_RESPONSIVE_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_DEFAULT, SRC_ENUM(3), \
                      SRC_ENUM(1), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(2)}
    },
    [FLEX_QOS_QOS_REMARK_L2_MPLS_NON_RESPONSIVE_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_MPLS_NON_RESPONSIVE_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_DEFAULT, SRC_ENUM(3), \
                      SRC_ENUM(1), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(2)}
    },
    [FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_0_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_0_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(2), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(4), \
                      SRC_ENUM(4), SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT}
    },
    [FLEX_QOS_QOS_REMARK_L3_MPLS_RESPONSIVE_0_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L3_MPLS_RESPONSIVE_0_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(2)}
    },
    [FLEX_QOS_QOS_REMARK_L3_MPLS_NON_RESPONSIVE_0_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L3_MPLS_NON_RESPONSIVE_0_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), \
                      SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(2)}
    },
    [FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_D1P_COPY_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_D1P_COPY_INDEXs},
        .sym_field = {SRC_DEFAULT, SRC_DEFAULT, SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, \
                      SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY,
                        0,0,0,0,0,0,0,0,0,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_INT_CONGESTION_COPY}
    },
    [FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, \
                      SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {0,0,0,0,0,0,0,0,0,0,0,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_INT_CONGESTION_COPY}
    },
    [FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_TOS_COPY_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_TOS_COPY_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_DEFAULT, SRC_DEFAULT, \
                      SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, \
                      SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {0,0,BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY,
                        0,0,0,0,0,0,0,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_INT_CONGESTION_COPY}
    },
    [FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_D1P_TOS_COPY_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_L3_FORWARDING_D1P_TOS_COPY_INDEXs},
        .sym_field = {SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, \
                      SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, \
                      SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY,
                        0,0,0,0,0,0,0,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_INT_CONGESTION_COPY}
    },
    [FLEX_QOS_QOS_REMARK_L2_TUNNEL_ENCAP_D1P_TOS_COPY_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_TUNNEL_ENCAP_D1P_TOS_COPY_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT, SRC_ENUM(4), \
                      SRC_ENUM(4), SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {0,0,0,0,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY}
    },
    [FLEX_QOS_QOS_REMARK_L2_TUNNEL_ENCAP_D1P_COPY_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_TUNNEL_ENCAP_D1P_COPY_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_DEFAULT, SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(4), \
                      SRC_ENUM(4), SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {0,0,0,0,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY}
    },
    [FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_TOS_COPY_0_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_TOS_COPY_0_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(2), SRC_DEFAULT, SRC_DEFAULT, SRC_ENUM(4), \
                      SRC_ENUM(4), SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {0,0,0,0,0,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY}
    },
    [FLEX_QOS_QOS_REMARK_L3_TUNNEL_ENCAP_D1P_TOS_COPY_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L3_TUNNEL_ENCAP_D1P_TOS_COPY_INDEXs},
        .sym_field = {SRC_DEFAULT, SRC_DEFAULT, SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(2), SRC_DEFAULT, SRC_DEFAULT, SRC_ENUM(4), \
                      SRC_ENUM(4), SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY,
                        0,0,0,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY}
    },
    [FLEX_QOS_QOS_REMARK_L3_TUNNEL_ENCAP_D1P_COPY_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L3_TUNNEL_ENCAP_D1P_COPY_INDEXs},
        .sym_field = {SRC_DEFAULT, SRC_DEFAULT, SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(2), SRC_ENUM(3), SRC_ENUM(3), SRC_ENUM(4), \
                      SRC_ENUM(4), SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L2_DOT1P_COPY}
    },
    [FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_TOS_COPY_1_INDEX] = {
        .key = {FLEX_QOS_QOS_REMARK_L2_L3_TUNNEL_ENCAP_TOS_COPY_1_INDEXs},
        .sym_field = {SRC_ENUM(0), SRC_ENUM(0), SRC_ENUM(1), SRC_ENUM(1), \
                      SRC_ENUM(2), SRC_DEFAULT, SRC_DEFAULT, SRC_ENUM(4), \
                      SRC_ENUM(4), SRC_DEFAULT, SRC_DEFAULT, SRC_DEFAULT},
        .field_flags = {0,0,0,0,0,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY,
                        BCMINT_QOS_EGR_POLICY_TABLE_FLAGS_L3_TOS_COPY}
    },
};

/* Define egress policy table base index field name. */
#define BASE_INDEX(n) (EGR_QOS_TABLE_##n##_BASE_INDEXs)

/* Define egress policy table base index source field name. */
#define BASE_INDEX_SRC(n) (EGR_QOS_TABLE_##n##_BASE_INDEX_SRCs)

/*
 * Egress policy table configuration.
 */
static const bcmint_qos_tbl_cfg_t qos_egr_policy_tbl_cfg[] = {
    {
        .key_name = {FLEX_QOS_EGR_POLICY_IDs},
        .field_num = QOS_REMARK_TABLE_NUM,
        .field_array = {BASE_INDEX(0), \
                        BASE_INDEX(1), \
                        BASE_INDEX(2), \
                        BASE_INDEX(3), \
                        BASE_INDEX(4), \
                        BASE_INDEX(5), \
                        BASE_INDEX(6), \
                        BASE_INDEX(7), \
                        BASE_INDEX(8), \
                        BASE_INDEX(9), \
                        BASE_INDEX(10), \
                        BASE_INDEX(11)},
        .sym_field_num = QOS_REMARK_TABLE_NUM,
        .sym_field_array = {BASE_INDEX_SRC(0), \
                            BASE_INDEX_SRC(1), \
                            BASE_INDEX_SRC(2), \
                            BASE_INDEX_SRC(3), \
                            BASE_INDEX_SRC(4), \
                            BASE_INDEX_SRC(5), \
                            BASE_INDEX_SRC(6), \
                            BASE_INDEX_SRC(7), \
                            BASE_INDEX_SRC(8), \
                            BASE_INDEX_SRC(9), \
                            BASE_INDEX_SRC(10), \
                            BASE_INDEX_SRC(11)},
        .ent_num = FLEX_QOS_EGR_POLICY_ID_CNT,
        .ent = qos_egr_policy_tbl_ent
    }
};

static const int qos_vxlan_tbl_val[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

/*
 * Egress PHB Vxlan G flag remarking table entry configuration.
 */
static const bcmint_qos_tbl_ent_t qos_egr_phb_vxlan_g_tbl_ent[] = {
    {
        .key_offset = &qos_vxlan_tbl_val[0],
        .field = &qos_vxlan_tbl_val[0] /* G bit not set. */
    },
    {
        .key_offset = &qos_vxlan_tbl_val[1],
        .field = &qos_vxlan_tbl_val[1] /* G bit set. */
    }
};

/*
 * Egress PHB Vxlan G flag remarking table configuration.
 */
static const bcmint_qos_tbl_cfg_t qos_egr_phb_vxlan_g_tbl_cfg[] = {
    {
        .key_name = {BASE_INDEXs},
        .field_num = 1,
        .field_array = {QOS_FIELDs},
        .ent_num = 2,
        .ent = qos_egr_phb_vxlan_g_tbl_ent
    }
};

/*
 * Egress PHB Vxlan BUM flag remarking table entry configuration.
 */
static const bcmint_qos_tbl_ent_t qos_egr_phb_vxlan_bum_tbl_ent[] = {
    {
        .key_offset = &qos_vxlan_tbl_val[9], /* L2MC. */
        .field = &qos_vxlan_tbl_val[1] /* BUM bit set. */
    },
    {
        .key_offset = &qos_vxlan_tbl_val[11], /* IPMC. */
        .field = &qos_vxlan_tbl_val[1] /* BUM bit set. */
    }
};

/*
 * Egress PHB Vxlan BUM flag remarking table configuration.
 */
static const bcmint_qos_tbl_cfg_t qos_egr_phb_vxlan_bum_tbl_cfg[] = {
    {
        .key_name = {BASE_INDEXs},
        .field_num = 1,
        .field_array = {QOS_FIELDs},
        .ent_num = 2,
        .ent = qos_egr_phb_vxlan_bum_tbl_ent
    }
};

/*
 * Egress PHB remarking table configuration for internal congestion copy.
 */
static const bcmint_qos_tbl_cfg_t qos_egr_phb_int_congestion_copy_tbl_cfg[] = {
    {
        .key_name = {BASE_INDEXs},
        .field_num = 1,
        .field_array = {QOS_FIELDs},
        .ent_num = 16,
        .one_to_one = true /* 1:1 mapping, so no need entry data. */
    }
};

/*
 * Table configuration.
 */
static const bcmint_qos_tbl_info_t qos_tbl[QOS_TBL_CNT] = {
    [QOS_TBL_ING_POLICY] = {
        .name = FLEX_QOS_ING_POLICYs,
        .id = FLEX_QOS_ING_POLICYt,
        .tbl_cfg = qos_ing_policy_tbl_cfg
    },
    [QOS_TBL_EGR_POLICY] = {
        .name = FLEX_QOS_EGR_POLICYs,
        .id = FLEX_QOS_EGR_POLICYt,
        .tbl_cfg = qos_egr_policy_tbl_cfg
    },
    [QOS_TBL_EGR_PHB_VXLAN_G] = {
        .name = FLEX_QOS_EGR_PHB_7s,
        .id = FLEX_QOS_EGR_PHB_7t,
        .tbl_cfg = qos_egr_phb_vxlan_g_tbl_cfg
    },
    [QOS_TBL_EGR_PHB_VXLAN_BUM] = {
        .name = FLEX_QOS_EGR_PHB_8s,
        .id = FLEX_QOS_EGR_PHB_8t,
        .tbl_cfg = qos_egr_phb_vxlan_bum_tbl_cfg
    },
    [QOS_TBL_EGR_PHB_INT_CONGESTION_COPY] = {
        .name = FLEX_QOS_EGR_PHB_11s,
        .id = FLEX_QOS_EGR_PHB_11t,
        .tbl_cfg = qos_egr_phb_int_congestion_copy_tbl_cfg
    }
};

/*
 * Table database.
 */
static const bcmint_qos_tbl_db_t qos_tbl_db[] = {
    {
        .tbl_num = 5,
        .tbls = qos_tbl
    }
};

/*
 * QoS attribute remark info.
 */
static const bcmint_qos_attribute_remark_info_t qos_remark_info[] = {
    {
        .copied_pri_cfi_ena = true,
        .copied_dscp_ecn_ena = true,
        .copy_int_congestion_ena = true
    }
};

/*
 * QoS control info.
 */
static const bcmint_qos_ctrl_info_t qos_ctrl_info[] = {
    {
        .tbl_db = qos_tbl_db,
        .remark_info = qos_remark_info
    }
};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Initialize QoS control info.
 *
 * \param [in] unit Unit Number.
 * \param [in|out] ctrl_info QoS control info pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_dna_4_6_6_ltsw_qos_ctrl_info_init(int unit,
    const bcmint_qos_ctrl_info_t **ctrl_info)
{
    *ctrl_info = qos_ctrl_info;
    return SHR_E_NONE;
}

/*!
 * \brief QoS driver function variable for bcm56880_a0 DNA_4_6_6 device.
 */
static mbcm_ltsw_qos_drv_t bcm56880_a0_dna_4_6_6_ltsw_qos_drv = {
    .qos_ctrl_info_init = bcm56880_a0_dna_4_6_6_ltsw_qos_ctrl_info_init
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_dna_4_6_6_ltsw_qos_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_qos_drv_set(unit, &bcm56880_a0_dna_4_6_6_ltsw_qos_drv));

exit:
    SHR_FUNC_EXIT();
}

