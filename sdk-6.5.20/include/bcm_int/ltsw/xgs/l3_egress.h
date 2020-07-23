/*! \file l3_egress.h
 *
 * L3 egress header file.
 * This file contains L3 egress definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_XGS_L3_EGRESS_H
#define BCMI_XGS_L3_EGRESS_H

#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/l3_egress.h>

/* L3 egress fields. */
typedef enum bcmint_xgs_l3_egress_fld_e {
    BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID,
    BCMINT_XGS_L3_EGRESS_TBL_VLAN_ID,
    BCMINT_XGS_L3_EGRESS_TBL_MODPORT,
    BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID,
    BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK,
    BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU,
    BCMINT_XGS_L3_EGRESS_TBL_DROP,
    BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID,
    BCMINT_XGS_L3_EGRESS_TBL_DST_MAC,
    BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC,
    BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL,
    BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID,
    BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC,
    BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID,
    BCMINT_XGS_L3_EGRESS_TBL_REPLACE_DST_MAC,
    BCMINT_XGS_L3_EGRESS_TBL_DROP_L3,
    BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID,
    BCMINT_XGS_L3_EGRESS_TBL_LABEL,
    BCMINT_XGS_L3_EGRESS_TBL_LABEL_TTL,
    BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION,
    BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE,
    BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP,
    BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID,
    BCMINT_XGS_L3_EGRESS_TBL_PRI,
    BCMINT_XGS_L3_EGRESS_TBL_CFI,
    BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_ID,
    BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_PRIORITY,
    BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_PRIORITY,
    BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_ID,
    BCMINT_XGS_L3_EGRESS_TBL_VPN_LABEL,
    BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT,
    BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID,
    BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID,
    BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH,
    BCMINT_XGS_L3_EGRESS_TBL_DVP,
    BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID,
    BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID,
    BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_VALID,
    BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_ID,
    BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID,
    BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID,
    BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_VALID,
    BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_ID,
    BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_PRESENT_ACTION,
    BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_NOT_PRESENT_ACTION,
    BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_PRESENT_ACTION,
    BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_NOT_PRESENT_ACTION,
    BCMINT_XGS_L3_EGRESS_TBL_IS_PHB_MAP_ID,
    BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_L2_INT_PRI_TO_OTAG_ID,
    BCMINT_XGS_L3_EGRESS_TBL_OCFI,
    BCMINT_XGS_L3_EGRESS_TBL_OPRI,
    BCMINT_XGS_L3_EGRESS_TBL_REMARK_CFI,
    BCMINT_XGS_L3_EGRESS_TBL_OVID,
    BCMINT_XGS_L3_EGRESS_TBL_VLAN_OUTER_TPID_ID,
    BCMINT_XGS_L3_EGRESS_TBL_ES_IDENTIFIER,
    BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT
} bcmint_xgs_l3_egress_fld_t;

/* L3 ECMP fields. */
typedef enum bcmint_xgs_l3_ecmp_fld_e {
    BCMINT_XGS_L3_ECMP_TBL_ECMP_ID,
    BCMINT_XGS_L3_ECMP_TBL_LB_MODE,
    BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS,
    BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE,
    BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS,
    BCMINT_XGS_L3_ECMP_TBL_NHOP_ID,
    BCMINT_XGS_L3_ECMP_TBL_WEIGHT,
    BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID,
    BCMINT_XGS_L3_ECMP_TBL_NHOP_SORTED,
    BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_MODE,
    BCMINT_XGS_L3_ECMP_TBL_RH_SIZE,
    BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS,
    BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED,
    BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT,
    BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID,
    BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID,
    BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID,
    BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY,
    BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID,
    BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID,
    BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY,
    BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID,
    BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID,
    BCMINT_XGS_L3_ECMP_TBL_FLD_CNT
} bcmint_xgs_l3_ecmp_fld_t;

/* L3 ECMP flexctr action fields. */
typedef enum bcmint_xgs_l3_ecmp_flexctr_act_fld_e {
    BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID,
    BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ACTION,
    BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_FLD_CNT
} bcmint_xgs_l3_ecmp_flexctr_act_fld_t;

typedef enum tnl_mpls_label_action_e {
    BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_NONE = 0,
    BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_SWAP = 1,
    BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_PRESERVE = 2,
    BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_CNT
} tnl_mpls_label_action_t;

typedef enum tnl_mpls_exp_mode_e {
    BCMINT_LTSW_TNL_MPLS_EXP_MODE_FIXED = 0,
    BCMINT_LTSW_TNL_MPLS_EXP_MODE_MAP = 1,
    BCMINT_LTSW_TNL_MPLS_EXP_MODE_USE_INNER_LABEL = 2,
    BCMINT_LTSW_TNL_MPLS_EXP_MODE_USE_SWAP_LABEL = 3,
    BCMINT_LTSW_TNL_MPLS_EXP_MODE_CNT
} tnl_mpls_exp_mode_t;

typedef enum l3_ecmp_lb_mode_e {
    BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR = 0,
    BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM = 1,
    BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT = 2,
    BCMINT_LTSW_L3_ECMP_LB_MODE_CNT
} l3_ecmp_lb_mode_t;


/* Weighted ECMP group size enum definition. */
typedef enum l3_ecmp_wgt_sz_e {
    BCMINT_LTSW_L3_ECMP_WGT_SZ_128 = 0,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_256 = 1,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_512 = 2,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_1K = 3,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_2K = 4,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_4K = 5,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_CNT
} l3_ecmp_wgt_sz_t;

/* Resilient Hash ECMP group size enum definition. */
typedef enum l3_ecmp_res_sz_e {
    BCMINT_LTSW_L3_ECMP_RES_SZ_64 = 0,
    BCMINT_LTSW_L3_ECMP_RES_SZ_128 = 1,
    BCMINT_LTSW_L3_ECMP_RES_SZ_256 = 2,
    BCMINT_LTSW_L3_ECMP_RES_SZ_512 = 3,
    BCMINT_LTSW_L3_ECMP_RES_SZ_1K = 4,
    BCMINT_LTSW_L3_ECMP_RES_SZ_2K = 5,
    BCMINT_LTSW_L3_ECMP_RES_SZ_4K = 6,
    BCMINT_LTSW_L3_ECMP_RES_SZ_8K = 7,
    BCMINT_LTSW_L3_ECMP_RES_SZ_16K = 8,
    BCMINT_LTSW_L3_ECMP_RES_SZ_32K = 9,
    BCMINT_LTSW_L3_ECMP_RES_SZ_CNT
} l3_ecmp_res_sz_t;

/* Weighted ECMP group mode. */
typedef enum l3_ecmp_wgt_mode_e {
    BCMINT_LTSW_L3_ECMP_MEMBER_REPLICATION = 0,
    BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHTED = 1,
    BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHT_MODE_CNT
} l3_ecmp_wgt_mode_t;

/*!
 * \brief Initialize L3 egress module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_init(int unit);

/*!
 * \brief De-Initialize L3 egress module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_deinit(int unit);

/*!
 * \brief Get the index range of a specified egress object type.
 *
 * \param [in] unit Unit number.
 * \param [in] type Egress object type.
 * \param [out] min Min index of the egress object type.
 * \param [out] max Max index of the egress object type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid egress object type.
 */
extern int
xgs_ltsw_l3_egress_id_range_get(int unit, int type, int *min, int *max);

/*!
 * \brief Create a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] egr Egress forwarding destination info.
 * \param [in/out] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_create(int unit, bcm_l3_egress_t *egr, int *idx);

/*!
 * \brief Destroy a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_destroy(int unit, int idx);

/*!
 * \brief Get a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Egress object ID.
 * \param [out] egr Egress object info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_get(int unit, int idx, bcm_l3_egress_t *egr);

/*!
 * \brief Find a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] egr Egress object info to match.
 * \param [out] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_find(int unit, bcm_l3_egress_t *egr, int *idx);

/*!
 * \brief Traverse L3 egress objects.
 *
 * Goes through egress objects table and runs the user callback
 * function at each valid egress objects entry passing back the
 * information for that object.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_traverse(int unit,
                             bcm_l3_egress_traverse_cb trav_fn,
                             void *user_data);

/*!
 * \brief Get nexthop index from egress object ID.
 *
 * \param [in] unit Unit number.
 * \param [in] intf Egress object ID.
 * \param [out] nhidx Nexthop index.
 * \param [out] type Egress object type.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_obj_id_resolve(int unit,
                               bcm_if_t intf,
                                  int *nh_ecmp_idx,
                               bcmi_ltsw_l3_egr_obj_type_t *type);

/*!
 * \brief Get egress object ID from nexthop index.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Nexthop index.
 * \param [in] type Egress object type.
 * \param [out] intf Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_obj_id_construct(
    int unit,
    int nh_ecmp_idx,
    bcmi_ltsw_l3_egr_obj_type_t type,
    bcm_if_t *intf);

/*!
 * \brief Get the L3 egress object maximum and used number.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Egress object type.
 * \param [out] max Maximum number of object.
 * \param [out] used Used number of object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_obj_cnt_get(
    int unit,
    bcmi_ltsw_l3_egr_obj_type_t type,
    int *max,
    int *used);

/*!
 * \brief Set the dvp of given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf Egress object ID.
 * \param [in] dvp Destination VP.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid egress object ID.
 * \retval SHR_E_NOT_FOUND The given egress object does not exist.
 */
extern int
xgs_ltsw_l3_egress_dvp_set(int unit, bcm_if_t intf, int dvp);

/*!
 * \brief Attach flexctr action to the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_if Interface id of a L3 egress object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_flexctr_attach(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 egress object.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_egress_flexctr_detach(
    int unit,
    bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_if Interface id of a L3 egress object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_flexctr_info_get(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Set flex counter object value for the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 egress object.
 * \param [in] value Flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t value);

/*!
 * \brief Get flex counter object value for the given egress object.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf_id L3 interface ID pointing to egress object.
 * \param [out] value Flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value);


/*!
 * \brief Set the dvp of given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf Egress object ID.
 * \param [in] dvp Destination VP.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_create(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array);

/*!
 * \brief Destroy an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id L3 ECMP forwarding object ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_destroy(
    int unit,
    bcm_if_t ecmp_group_id);

/*!
 * \brief Get an Egress ECMP forwarding object information.
 *
 * \param [in] unit Unit number.
 * \param [in/out] ecmp_info ECMP group information.
 * \param [in] ecmp_member_size Size of allocated entries in ecmp_member_array.
 * \param [out] ecmp_member_array Member array of egress forwarding objects.
 * \param [out] ecmp_member_count Number of elements in ecmp_member_array.
 *
 * If the ecmp_member_size is 0, then ecmp_member_array is ignored and
 * ecmp_member_count is filled in with the number of members that would have
 * been filled into ecmp_member_array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_get(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

/*!
 * \brief Add a member to an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Egress ECMP forwarding object ID.
 * \param [in] ecmp_member Pointer to ECMP objects member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_member_add(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_member_t *ecmp_member);

/*!
 * \brief Delete a member from an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Egress ECMP forwarding object ID.
 * \param [in] ecmp_member Pointer to ECMP objects member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_member_del(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_member_t *ecmp_member);

/*!
 * \brief Delete all member from an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Egress ECMP forwarding object ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_member_del_all(
    int unit,
    bcm_if_t ecmp_group_id);

/*!
 * \brief Find an Egress ECMP forwarding object by given member info.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_member_count Number of elements in ecmp_member_array.
 * \param [in] ecmp_member_array ECMP member array.
 * \param [out] ecmp_info ECMP group info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_l3_egress_ecmp_t *ecmp_info);

/*!
 * \brief Traverse L3 ECMP objects.
 *
 * Goes through Egress ECMP forwarding objects table and runs the
 * user callback function at each Egress ECMP forwarding objects
 * entry passing back the information for that ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fun Callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_trav(
    int unit,
    bcm_l3_ecmp_traverse_cb trav_fn,
    void *user_data);

/*!
 * \brief Get the maximum ECMP paths supported by LT.
 *
 * \param [in] unit Unit number.
 * \param [in] attr ECMP attributes.
 * \param [out] max Maximum number of paths for ECMP.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_ecmp_max_paths_get(int unit, uint32_t attr, int *max_paths);

/*!
 * \brief Configure the ECMP flow-based hash output selection.
 *
 * \param [in] unit Unit number.
 * \param [in] type ECMP hash output selection configure type.
 * \param [in] arg Configuration value.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xgs_ltsw_l3_ecmp_flow_hash_set(int unit,
                               bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                               int arg);

/*!
 * \brief Get the ECMP flow-based hash output selection info.
 *
 * \param [in] unit Unit number.
 * \param [in] type ECMP hash output selection configure type.
 * \param [out] arg Configuration value.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xgs_ltsw_l3_ecmp_flow_hash_get(int unit,
                               bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                               int *arg);

/*!
 * \brief Configure the ECMP port-based hash output selection.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type ECMP hash output selection configure type.
 * \param [in] arg Configuration value.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xgs_ltsw_l3_ecmp_port_hash_set(int unit,
                               int port,
                               bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                               int arg);

/*!
 * \brief Get the ECMP port-based hash output selection control.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type ECMP hash output selection configure type.
 * \param [out] arg Configuration value.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xgs_ltsw_l3_ecmp_port_hash_get(int unit,
                               int port,
                               bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                               int *arg);

/*!
 * \brief Set ECMP dynamic load balancing member status.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface ID pointing to an Egress Object.
 * \param [in] status ECMP member status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_member_status_set(int unit, bcm_if_t intf, int status);

/*!
 * \brief Get ECMP dynamic load balancing member status.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface ID pointing to an Egress Object.
 * \param [out] status ECMP member status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_member_status_get(int unit, bcm_if_t intf, int *status);

/*!
 * \brief Set DLB Eligibility Ethertype.
 *
 * \param [in] unit Unit Number.
 * \param [in] flags Flags.
 * \param [in] Ethertype count.
 * \param [in] Ethertype array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_l3_ecmp_ethertype_set(
    int unit,
    uint32_t flags,
    int ethertype_count,
    int *ethertype_array);

/*!
 * \brief Get DLB Eligibility Ethertype.
 *
 * \param [in] unit Unit Number.
 * \param [in] Max ethertype count.
 * \param [out] flags Flags.
 * \param [out] ethertype array.
 * \param [out] Actual ethertype count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_l3_ecmp_ethertype_get(
    int unit,
    int ethertype_max,
    uint32_t *flags,
    int *ethertype_array,
    int *ethertype_count);

/*!
 * \brief Set the ECMP dlb monitor configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid ECMP Identifier.
 * \param [in] dlb_mon_cfg ECMP DLB monitor configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_l3_ecmp_dlb_mon_config_set(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

/*!
 * \brief Get the ECMP dlb monitor configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid ECMP Identifier.
 * \param [out] dlb_mon_cfg ECMP DLB monitor configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_l3_ecmp_dlb_mon_config_get(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

/*!
 * \brief Set the DLB statistics based on the ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_intf ECMP interface id.
 * \param [in] type L3 ECMP DLB statistics type..
 * \param [in] value 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_l3_ecmp_dlb_stat_set(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t value);

/*!
 * \brief Get the DLB statistics based on the ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_intf ECMP interface id.
 * \param [in] type L3 ECMP DLB statistics type..
 * \param [out] value 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_l3_ecmp_dlb_stat_get(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value);

/*!
 * \brief Get the DLB statistics based on the ECMP interface in sync mode.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_intf ECMP interface id.
 * \param [in] type L3 ECMP DLB statistics type..
 * \param [out] value 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_l3_ecmp_dlb_stat_sync_get(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value);

/*!
 * \brief Public function to get L3 egress next hop table size.
 *
 * \param [in] unit Unit Number.
 * \param [out] size L3 egress next hop table size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
xgs_ltsw_l3_egress_size_get(
    int unit,
    int *size);

/*!
 * \brief Public function to get current L3 egress next hop entry count.
 *
 * \param [in] unit Unit Number.
 * \param [out] count L3 egress next hop entry count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_count_get(
    int unit,
    int *count);

/*!
 * \brief Public function to get L3 ECMP table size.
 *
 * \param [in] unit Unit Number.
 * \param [out] size L3 ECMP table size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
xgs_ltsw_l3_ecmp_size_get(
    int unit,
    int *size);

/*!
 * \brief Public function to get current L3 ECMP entry count.
 *
 * \param [in] unit Unit Number.
 * \param [out] count L3 ECMP entry count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_count_get(
    int unit,
    int *count);

/*!
 * \brief Attach flexctr action to the given L3 ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_if Interface id of a L3 ECMP object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_ecmp_flexctr_attach(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 ECMP object.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_ecmp_flexctr_detach(
    int unit,
    bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given L3 ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_if Interface id of a L3 ECMP object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_flexctr_info_get(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Set flex counter object value for the given L3 ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 ECMP object.
 * \param [in] value Flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t value);

/*!
 * \brief Get flex counter object value for the given L3 ECMP object.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf_id L3 interface ID pointing to L3 ECMP object.
 * \param [out] value Fex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ecmp_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value);

/*!
 * \brief Public function to add L2 tag for L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 * \param [in] l2_tag_info L2 tag information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_add_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info);

/*!
 * \brief Public function to get L2 tag from L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 * \param [out] l2_tag_info L2 tag information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_get_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info);

/*!
 * \brief Public function to delete L2 tag from L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_delete_l2tag(
    int unit,
    int nhidx);

/*!
 * \brief Public function to add php acion to L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_php_action_add(
    int unit,
    bcm_if_t intf);

/*!
 * \brief Public function to delete php acion from L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egress_php_action_del(
    int unit,
    bcm_if_t intf);

#endif /* BCMI_XGS_L3_EGRESS_H */
