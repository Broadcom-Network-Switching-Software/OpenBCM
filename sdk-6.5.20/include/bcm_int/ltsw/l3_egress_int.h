/*! \file l3_egress_int.h
 *
 * L3 egress internal header file.
 * This file contains L3 egress definitions internal to the L3 egress module
 * itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMI_LTSW_L3_EGRESS_INT_H
#define BCMI_LTSW_L3_EGRESS_INT_H
#include <bcm_int/ltsw/l3_fib_int.h>

#include <bcm_int/ltsw/types.h>

/* HA Sub component id for ECMP flags. */
#define BCMINT_LTSW_L3_EGR_HA_ECMP_INFO     0
#define BCMINT_LTSW_L3_EGR_HA_OL_ECMP_FLAGS 1
#define BCMINT_LTSW_L3_EGR_HA_UL_ECMP_FLAGS 2
#define BCMINT_LTSW_L3_EGR_HA_ECMP_WEIGHTED_FLAGS 3
/* HA Sub component id for ECMP lb mode. */
#define BCMINT_LTSW_L3_EGR_HA_OL_ECMP_LB_MODE 4
#define BCMINT_LTSW_L3_EGR_HA_UL_ECMP_LB_MODE 5
#define BCMINT_LTSW_L3_EGR_HA_ECMP_MEMBER_DEST 6
#define BCMINT_LTSW_L3_EGR_HA_OL_RH_ECMP_MAX_PATHS 7
#define BCMINT_LTSW_L3_EGR_HA_UL_RH_ECMP_MAX_PATHS 8
#define BCMINT_LTSW_L3_EGR_HA_OL_NHOP_FLAGS 9

/* ECMP Group attributes. */
#define BCMI_LTSW_ECMP_ATTR_WGT         0x1 /* Weighted ECMP group. */
#define BCMI_LTSW_ECMP_ATTR_DLB         0x2 /* DLB ECMP group */
#define BCMI_LTSW_ECMP_ATTR_DGM         0x4 /* DGM ECMP group */
#define BCMI_LTSW_ECMP_ATTR_RH          0x8 /* Resilient Hash ECMP group */
#define BCMI_LTSW_ECMP_ATTR_OL          0x10 /* Overlay ECMP group */

/* Field description. */
typedef struct bcmint_ltsw_l3_egress_fld_s {
    /* Field name. */
    const char *fld_name;

    /* Key field. */
    bool key;

    /* Symbol field. */
    bool symbol;

    /* Callback to get scalar value from symbol. */
    bcmi_ltsw_symbol_to_scalar_f symbol_to_scalar;

    /* Callback to get symbol from scalar value. */
    bcmi_ltsw_scalar_to_symbol_f scalar_to_symbol;

} bcmint_ltsw_l3_egress_fld_t;

/* L3 egress tables. */
typedef enum bcmint_ltsw_l3_egress_tbl_id_e {
    BCMINT_LTSW_L3_EGRESS_TBL_UC,
    BCMINT_LTSW_L3_EGRESS_TBL_MC,
    BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT,
    BCMINT_LTSW_L3_EGRESS_TBL_MPLS_ENCAP,
    BCMINT_LTSW_L3_EGRESS_TBL_IFP_ACTION,
    BCMINT_LTSW_L3_EGRESS_TBL_L2_TAG,
    BCMINT_LTSW_L3_EGRESS_TBL_CNT
} bcmint_ltsw_l3_egress_tbl_id_t;

typedef enum bcmint_ltsw_l3_ecmp_tbl_id_e {
    BCMINT_LTSW_L3_ECMP_TBL_UL,
    BCMINT_LTSW_L3_ECMP_TBL_OL,
    BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL,
    BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL,
    BCMINT_LTSW_L3_ECMP_TBL_CNT
} bcmint_ltsw_l3_ecmp_tbl_id_t;

typedef enum bcmint_ltsw_l3_ecmp_flexctr_act_tbl_id_e {
    BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL,
    BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_OL,
    BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_CNT
} bcmint_ltsw_l3_ecmp_flexctr_act_tbl_id_t;

typedef enum bcmint_ltsw_l3_control_tbl_id_e {
    BCMINT_LTSW_L3_CTR_TBL_LIMIT,
    BCMINT_LTSW_L3_CTR_TBL_TBL_CNT
} bcmint_ltsw_l3_control_tbl_id_t;

/* The structure of L3 egress table. */
typedef struct bcmint_ltsw_l3_egress_tbl_s {
    /* Table name. */
    const char *name;

    /* Field count. */
    uint32_t fld_cnt;

    /* Bitmap of valid fields. */
    uint64_t fld_bmp;

    /* The fields of table. */
    const bcmint_ltsw_l3_egress_fld_t *flds;

} bcmint_ltsw_l3_egress_tbl_t;

/* L3 egress table database. */
typedef struct bcmint_ltsw_l3_egress_tbl_db_s {
    /* The table bitmap. */
    uint32_t tbl_bmp;

    /* The table array. */
    const bcmint_ltsw_l3_egress_tbl_t *tbls;
} bcmint_ltsw_l3_egress_tbl_db_t;

/* L3 overlay table Information. */
typedef struct bcmint_ltsw_l3_ol_tbl_info_s {
    /* Overlay egress object number. */
    int ol_egr_obj_num;

    /* Overlay ecmp member number. */
    int ol_ecmp_member_num;

    /* Overlay L3 interface number. */
    int ol_eif_num;

} bcmint_ltsw_l3_ol_tbl_info_t;

/*!
 * \brief Get the L3 egress table database.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_id L3 egress logical table description ID.
 * \param [out] tbl_info L3 egress table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmint_ltsw_l3_egress_tbl_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id,
    const bcmint_ltsw_l3_egress_tbl_t **tbl_info);

/*!
 * \brief Get the L3 ECMP table database.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_id L3 ecmp logical table description ID.
 * \param [out] tbl_info L3 ECMP table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmint_ltsw_l3_ecmp_tbl_get(
    int unit,
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id,
    const bcmint_ltsw_l3_egress_tbl_t **tbl_info);

/*!
 * \brief Get the L3 ECMP flex ctr action table database.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_id L3 ecmp flexctr action logical table description ID.
 * \param [out] tbl_info L3 ECMP flex ctr action table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(
    int unit,
    bcmint_ltsw_l3_ecmp_flexctr_act_tbl_id_t tbl_id,
    const bcmint_ltsw_l3_egress_tbl_t **tbl_info);

/*!
 * \brief Init L3 overlay information.
 *
 * \param [in] unit Unit number.
 * \param [in] info L3 overlay table info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmint_ltsw_l3_overlay_table_init(
    int unit,
    bcmint_ltsw_l3_ol_tbl_info_t *info);
#endif /* BCMI_LTSW_L3_EGRESS_INT_H */

