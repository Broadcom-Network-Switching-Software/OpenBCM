/*! \file l3_fib_int.h
 *
 * L3 FIB internal header file.
 * This file contains L3 FIB definitions internal to the L3 module itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_L3_FIB_INT_H
#define BCMI_LTSW_L3_FIB_INT_H

#include <bcm_int/ltsw/types.h>
#include <bcm_int/ltsw/uft.h>

/* HA Sub component id for ALPM data mode. */
#define BCMINT_LTSW_L3_FIB_HA_ALPM_DMODE 0

/* Unicast forwarding fields. */
typedef enum bcmint_ltsw_l3_fib_fld_uc_e {
    BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0,
    BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1,
    BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK,
    BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK,
    BCMINT_LTSW_L3_FIB_TBL_UC_VRF,
    BCMINT_LTSW_L3_FIB_TBL_UC_VRF_MASK,
    BCMINT_LTSW_L3_FIB_TBL_UC_DEST,
    BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE,
    BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID,
    BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1,
    BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX,
    BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0,
    BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1,
    BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE,
    BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX,
    BCMINT_LTSW_L3_FIB_TBL_UC_NAT_CTRL,
    BCMINT_LTSW_L3_FIB_TBL_UC_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT
} bcmint_ltsw_l3_fib_fld_uc_fld_t;

/* Multicast forwarding fields. */
typedef enum bcmint_ltsw_l3_fib_fld_mc_e {
    BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR,
    BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK = BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR + 2,
    BCMINT_LTSW_L3_FIB_TBL_MC_VRF = BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK + 2,
    BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK,
    BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR,
    BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF = BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR + 2,
    BCMINT_LTSW_L3_FIB_TBL_MC_DEST,
    BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE,
    BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID,
    BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX,
    BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID,
    BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0,
    BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1,
    BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF,
    BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION,
    BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY,
    BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT
} bcmint_ltsw_l3_fib_fld_mc_fld_t;

/* ALPM usage key type. */
typedef enum bcmint_ltsw_l3_alpm_usage_key_type_e {
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV4_UC_GL,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV4_UC_VRF,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV4_UC_GH,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_SINGLE_GL,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_SINGLE_VRF,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_SINGLE_GH,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_DOUBLE_GL,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_DOUBLE_VRF,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_DOUBLE_GH,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_QUAD_GL,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_QUAD_VRF,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_QUAD_GH,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV4_COMP,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_COMP,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV4_MC,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_MC,
    BCMINT_LTSW_L3_ALPM_USAGE_KT_CNT
} bcmint_ltsw_l3_alpm_usage_key_type_t;

/* ALPM Control key input type. */
typedef enum bcmint_ltsw_alpm_key_input_e {
    BCMINT_LTSW_L3_ALPM_KI_LPM_DST_QUAD,
    BCMINT_LTSW_L3_ALPM_KI_LPM_DST_DOUBLE,
    BCMINT_LTSW_L3_ALPM_KI_LPM_DST_SINGLE,
    BCMINT_LTSW_L3_ALPM_KI_LPM_SRC_QUAD,
    BCMINT_LTSW_L3_ALPM_KI_LPM_SRC_DOUBLE,
    BCMINT_LTSW_L3_ALPM_KI_LPM_SRC_SINGLE,
    BCMINT_LTSW_L3_ALPM_KI_FP_COMP_DST,
    BCMINT_LTSW_L3_ALPM_KI_FP_COMP_SRC,
    BCMINT_LTSW_L3_ALPM_KI_LPM_L3MC_QUAD,
    BCMINT_LTSW_L3_ALPM_KI_LPM_L3MC_DOUBLE,
    BCMINT_LTSW_L3_ALPM_KI_LPM_L3MC_SINGLE,
    BCMINT_LTSW_L3_ALPM_KI_CNT
} bcmint_ltsw_alpm_key_input_t;

/* ALPM datebase. */
typedef enum bcmint_ltsw_l3_alpm_db_e {
    BCMINT_LTSW_L3_ALPM_DB_0,
    BCMINT_LTSW_L3_ALPM_DB_1,
    BCMINT_LTSW_L3_ALPM_DB_2,
    BCMINT_LTSW_L3_ALPM_DB_3,
    BCMINT_LTSW_L3_ALPM_DB_CNT
} bcmint_ltsw_l2_alpm_db_t;

/* ALPM usage fields. */
typedef enum bcmint_ltsw_l3_alpm_usage_fld_e {
    BCMINT_LTSW_L3_ALPM_USAGE_TBL_KEY_TYPE,
    BCMINT_LTSW_L3_ALPM_USAGE_TBL_DB,
    BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_ENT,
    BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_ENT,
    BCMINT_LTSW_L3_ALPM_USAGE_TBL_ENT_UTILIZATION,
    BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_RBKT,
    BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_RBKT,
    BCMINT_LTSW_L3_ALPM_USAGE_TBL_RBKT_UTILIZATION,
    BCMINT_LTSW_L3_ALPM_USAGE_TBL_FLD_CNT
} bcmint_ltsw_l3_alpm_usage_fld_t;

/* ALPM Control state. */
typedef enum bcmint_ltsw_l3_alpm_op_state_e {
    BCMINT_LTSW_L3_ALPM_OP_STATE_VALID,
    BCMINT_LTSW_L3_ALPM_OP_STATE_CONFILICT,
    BCMINT_LTSW_L3_ALPM_OP_STATE_INUSE,
    BCMINT_LTSW_L3_ALPM_OP_STATE_CNT
} bcmint_ltsw_l3_alpm_op_state_t;

/* ALPM Control fields. */
typedef enum bcmint_ltsw_l3_alpm_ctrl_fld_e {
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_OP_STATE,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_ALPM_MODE,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_0_LVLS,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_1_LVLS,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_2_LVLS,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_3_LVLS,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_0,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_1,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_2,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_3,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_4,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_5,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_6,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_7,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_8,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_9,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_0,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_1,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_2,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_3,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_4,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_5,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_6,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_7,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_8,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_9,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_HIT_MODE,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST_MASK,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST_TYPE_MATCH,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST_TYPE_NON_MATCH,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_UC_GL_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_UC_VRF_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_UC_GH_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_UC_GL_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_UC_VRF_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_UC_GH_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_COMP_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_COMP_STRENGTH_PROF_IDX,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_LARGE_VRF,
    BCMINT_LTSW_L3_ALPM_CTRL_TBL_FLD_CNT
} bcmint_ltsw_l3_alpm_ctrl_fld_t;

/* Field description. */
typedef struct bcmint_ltsw_l3_fib_fld_s {
    /* Field name. */
    const char *fld_name;

    /* Filed ID. */
    uint32_t fld_id;

    /* Key field. */
    bool key;

    /* Symbol field. */
    bool symbol;

    /* Number of elements needed for field. */
    uint32_t elements;

    /* Callback to get scalar value from symbol. */
    bcmi_ltsw_symbol_to_scalar_f symbol_to_scalar;

    /* Callback to get symbol from scalar value. */
    bcmi_ltsw_scalar_to_symbol_f scalar_to_symbol;

} bcmint_ltsw_l3_fib_fld_t;

/* L3 FIB tables. */
typedef enum bcmint_ltsw_l3_fib_tbl_id_e {
    BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC,
    BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC,
    BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_G,
    BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_SG,
    BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_G,
    BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_SG,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF,
    BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH,
    BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
    BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_64,
    BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
    BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_1_USAGE,
    BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_2_USAGE,
    BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_3_USAGE,
    BCMINT_LTSW_L3_FIB_TBL_ALPM_CONTROL,
    BCMINT_LTSW_L3_FIB_TBL_CNT
} bcmint_ltsw_l3_fib_tbl_id_t;

/* Attributes of FIB tables. */
/* Indicate IPv4 table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_V4      (1 << 0)
/* Indicate IPv6 table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_V6      (1 << 1)
/* Indicate unicast table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_UC      (1 << 2)
/* Indicate multicast table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_MC      (1 << 3)
/* Indicate host table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST    (1 << 4)
/* Indicate subnet route table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET  (1 << 5)
/* Indicate source route table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC     (1 << 6)
/* Indicate global low route table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_GL      (1 << 7)
/* Indicate VRF aware route table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF     (1 << 8)
/* Indicate global high route table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_GH      (1 << 9)
/* Indicate lpm128 route table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_LPM_128 (1 << 10)
/* Indicate (S, G) table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_SG      (1 << 11)
/* Indicate (*, G) table. */
#define BCMINT_LTSW_L3_FIB_TBL_ATTR_G       (1 << 12)

typedef enum bcmint_ltsw_l3_fib_tbl_rm_type_e {
    BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH = 1,
    BCMINT_LTSW_L3_FIB_TBL_RM_T_TCAM = 2,
    BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM = 3,
    BCMINT_LTSW_L3_FIB_TBL_RM_T_CNT
} bcmint_ltsw_l3_fib_tbl_rm_type_t;

/* The structure of L3 forwarding table. */
typedef struct bcmint_ltsw_l3_fib_tbl_s {
    /* Table name. */
    const char *name;

    /* Table ID. */
    uint32_t tbl_id;

    /* Table attributes: BCMINT_LTSW_L3_FIB_TBL_ATTR_xxx. */
    uint32_t attr;

    /* Multiple of base unit (IPv4 UC entry). */
    int multiple;

    /* Table resource management type. */
    bcmint_ltsw_l3_fib_tbl_rm_type_t rm_type;

    /* Field count. */
    uint32_t fld_cnt;

    /* Bitmap of valid fields. */
    uint64_t fld_bmp;

    /* The fields of table. */
    const bcmint_ltsw_l3_fib_fld_t *flds;

} bcmint_ltsw_l3_fib_tbl_t;

/* L3 forwarding table database. */
typedef struct bcmint_ltsw_l3_fib_tbl_db_s {
    /* The table bitmap. */
    uint32_t tbl_bmp;

    /* The table array. */
    const bcmint_ltsw_l3_fib_tbl_t *tbls;
} bcmint_ltsw_l3_fib_tbl_db_t;

/*!
 * \brief Get L3 FIB logical table info.
 *
 * \param [in] unit Unit Number.
 * \param [in] tbl_id FIB logical table description ID.
 * \param [out] tbl_info Pointer to table description info.
 *
 * \retval SHR_E_UNAVAIL Logical table is not supported.
 * \retval SHR_E_NONE No errer.
 */
extern int
bcmint_ltsw_l3_fib_tbl_get(int unit,
                           bcmint_ltsw_l3_fib_tbl_id_t tbl_id,
                           const bcmint_ltsw_l3_fib_tbl_t **tbl_info);

/*!
 * \brief Set L3_ALPM_CONTROL LT.
 *
 * \param [in] unit Unit number.
 * \param [in] alpm_temp ALPM template.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmint_ltsw_l3_alpm_control_set(int unit,
                                int alpm_temp,
                                bcmi_ltsw_uft_mode_t uft_mode);

#endif /* BCMI_LTSW_L3_FIB_INT_H */

