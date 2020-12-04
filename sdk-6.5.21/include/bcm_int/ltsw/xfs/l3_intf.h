/*! \file l3_intf.h
 *
 * L3 intf header file.
 * This file contains L3 interface definitions internal to XFS family.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_XFS_L3_INTF_H
#define BCMI_LTSW_XFS_L3_INTF_H

#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/l3_intf_int.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/types.h>

#include <sal/sal_types.h>

/* L3 interface tables ID. */
typedef enum xfs_ltsw_l3_intf_tbl_id_e {
    XFS_LTSW_L3_INTF_TBL_IIF,
    XFS_LTSW_L3_INTF_TBL_OIF_OL,
    XFS_LTSW_L3_INTF_TBL_OIF_UL,
    XFS_LTSW_L3_INTF_TBL_CNT
} xfs_ltsw_l3_intf_tbl_id_t;

/* L3 interface table attributes. */
/* Ingress interface. */
#define XFS_LTSW_L3_INTF_TBL_ATTR_ING   (1 << 0)
/* Egress interface. */
#define XFS_LTSW_L3_INTF_TBL_ATTR_EGR   (1 << 1)
/* Overlay interface. */
#define XFS_LTSW_L3_INTF_TBL_ATTR_OL    (1 << 2)
/* Underlay interface. */
#define XFS_LTSW_L3_INTF_TBL_ATTR_UL    (1 << 3)

/* IIF field ID. */
typedef enum xfs_ltsw_l3_iif_fld_e {
    XFS_LTSW_L3_INTF_TBL_ING_INDEX,
    XFS_LTSW_L3_INTF_TBL_ING_URPF_DEFAULT_ROUTE_CHECK_ENABLE,
    XFS_LTSW_L3_INTF_TBL_ING_URPF_MODE,
    XFS_LTSW_L3_INTF_TBL_ING_STRENGTH_PROFILE_INDEX,
    XFS_LTSW_L3_INTF_TBL_ING_VRF,
    XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION,
    XFS_LTSW_L3_INTF_TBL_ING_CLASS_ID,
    XFS_LTSW_L3_INTF_TBL_ING_DSCP_PTR,
    XFS_LTSW_L3_INTF_TBL_ING_RPA_ID_PROFILE,
    XFS_LTSW_L3_INTF_TBL_ING_OPAQUE_CTRL_ID,
    XFS_LTSW_L3_INTF_TBL_ING_SRC_REALM_ID,
    XFS_LTSW_L3_INTF_TBL_ING_USE_DSCP_FOR_PHB,
    XFS_LTSW_L3_INTF_TBL_ING_IP_MCAST_TYPE,
    XFS_LTSW_L3_INTF_TBL_ING_USE_L3_IIF_FOR_IPMC_LOOKUP,
    XFS_LTSW_L3_INTF_TBL_ING_IPV4UC_ENABLE,
    XFS_LTSW_L3_INTF_TBL_ING_IPV6UC_ENABLE,
    XFS_LTSW_L3_INTF_TBL_ING_IPV4MC_ENABLE,
    XFS_LTSW_L3_INTF_TBL_ING_IPV6MC_ENABLE,
    XFS_LTSW_L3_INTF_TBL_ING_NAPT_ENABLE,
    XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT
} xfs_ltsw_l3_iif_fld_t;

/* OIF field ID. */
typedef enum xfs_ltsw_l3_oif_fld_e {
    XFS_LTSW_L3_INTF_TBL_EGR_INDEX,
    XFS_LTSW_L3_INTF_TBL_EGR_VIEW_T,
    XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS,
    XFS_LTSW_L3_INTF_TBL_EGR_EDIT_CTRL_ID,
    XFS_LTSW_L3_INTF_TBL_EGR_VFI,
    XFS_LTSW_L3_INTF_TBL_EGR_L3_OIF_PROCESS_CTRL,
    XFS_LTSW_L3_INTF_TBL_EGR_TUNNEL_IDX,
    XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1,
    XFS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION,
    XFS_LTSW_L3_INTF_TBL_EGR_EFP_CTRL_ID,
    XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX,
    XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX,
    XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG,
    XFS_LTSW_L3_INTF_TBL_EGR_L2_HDR_ACTION,
    XFS_LTSW_L3_INTF_TBL_EGR_STG,
    XFS_LTSW_L3_INTF_TBL_EGR_MEMBERSHIP_PROFILE_IDX,
    XFS_LTSW_L3_INTF_TBL_EGR_UNTAG_PROFILE_INDEX,
    XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT
} xfs_ltsw_l3_oif_fld_t;

typedef enum xfs_ltsw_l3_intf_ipmc_type_e {
    XFS_LTSW_L3_INTF_IPMC_TYPE_PIM_SM,
    XFS_LTSW_L3_INTF_IPMC_TYPE_PIM_BIDIR
} xfs_ltsw_l3_intf_ipmc_type_t;

typedef enum xfs_ltsw_l3_intf_egr_view_e {
    XFS_LTSW_L3_INTF_EGR_VT_L3_OIF,
    XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER,
    XFS_LTSW_L3_INTF_EGR_VT_CNT
} xfs_ltsw_l3_intf_egr_view_t;

/*!
 * \brief Initialize L3 intf module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_intf_init(int unit);

/*!
 * \brief De-Initialize L3 intf module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_intf_deinit(int unit);

/*!
 * \brief Get ingress interface id range.
 *
 * \param [in] unit Unit number.
 * \param [out] min Minimum interface id of the given type.
 * \param [out] max Maximum interface id of the given type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_id_range_get(int unit, int *min, int *max);

/*!
 * \brief Get ingress class id range.
 *
 * \param [in] unit Unit number.
 * \param [out] min Minimum class id.
 * \param [out] max Maximum class id.
 * \param [out] width Number of bits of class id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_class_id_range_get(int unit, int *min, int *max, int *width);

/*!
 * \brief Create an L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_create(int unit, bcm_l3_ingress_t *intf_info,
                            bcm_if_t *intf_id);

/*!
 * \brief Destroy an L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [In] intf_id L3 ingress interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_destroy(int unit, bcm_if_t intf_id);

/*!
 * \brief Get an ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [In] intf_id L3 ingress interface id.
 * \param [OUT] ing_intf Ingress interface information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_get(int unit, bcm_if_t intf_id,
                         bcm_l3_ingress_t *ing_intf);

/*!
 * \brief Find an ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] ing_intf Ingress interface information.
 * \param [OUT] intf_id L3 ingress interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_find(int unit, bcm_l3_ingress_t *ing_intf,
                          bcm_if_t *intf_id);

/*!
 * \brief Traverse all valid ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [In] trav_fn Callback function.
 * \param [In] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_traverse(int unit,
                              bcm_l3_ingress_traverse_cb trav_fn,
                              void *user_data);

/*!
 * \brief Update rp_id profile of a given ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 * \param [in] rp_id Rendezvous point ID.
 * \param [in] set Set or unset the rp_id to rp_id_profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_rp_op(int unit, int intf_id, int rp_id, int set);

/*!
 * \brief Get rp_id of a given ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 * \param [out] rp_bmp Bitmap of rendezvous point ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_rp_get(int unit, int intf_id, uint32_t *rp_bmp);

/*!
 * \brief Attach flexctr action to the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_flexctr_attach(int unit,
                                    bcm_if_t intf_id,
                                    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_flexctr_detach(int unit, bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ing_intf_flexctr_info_get(int unit,
                                      bcm_if_t intf_id,
                                      bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Get egress interface id for CPU.
 *
 * \param [in] unit Unit number.
 * \param [out] cpu_intf_id CPU interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egr_l2cpu_intf_id_get(int unit, int *cpu_intf_id);

/*!
 * \brief Get egress interface id range.
 *
 * \param [in] unit Unit number.
 * \param [in] type Egress interface type.
 * \param [out] min Minimum interface id of the given type.
 * \param [out] max Maximum interface id of the given type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egr_intf_id_range_get(int unit,
                                  bcmi_ltsw_l3_egr_intf_type_t type,
                                  int *min, int *max);

/*!
 * \brief Get egress interface LT index.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Egress interface id.
 * \param [out] index LT index.
 * \param [out] type Egress interface type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egr_intf_to_index(int unit, bcm_if_t intf_id, int *index,
                              bcmi_ltsw_l3_egr_intf_type_t *type);

/*!
 * \brief Get egress interface id from LT index.
 *
 * \param [in] unit Unit number.
 * \param [in] index LT index.
 * \param [in] type Egress interface type.
 * \param [out] intf_id Egress interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egr_index_to_intf(int unit, int index,
                              bcmi_ltsw_l3_egr_intf_type_t type,
                              bcm_if_t *intf_id);

/*!
 * \brief Create an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egr_intf_create(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Get an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in/out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egr_intf_get(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Find an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in/out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egr_intf_find(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Delete an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egr_intf_delete(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Delete all L3 egress interfaces.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egr_intf_delete_all(int unit);

/*!
 * \brief Internal function to get L3 egress interface tunnel initiator field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [out] tnl_type Tunnel type.
 * \param [out] tnl_idx Tunnel initiator index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_intf_tnl_init_get(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t *tnl_type,
    int *tnl_idx);

/*!
 * \brief Update the VFI information for a given vid.
 *
 * \param [in] unit Unit number.
 * \param [in] vid VFI ID.
 * \param [in] vfi_info VFI information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_intf_vfi_update(int unit, int vid,
                            bcmi_ltsw_l3_intf_vfi_t *vfi_info);

/*!
 * \brief Recover all MPLS information from L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback function.
 * \param [in] user_data User data of callback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_intf_mpls_recover_all(int unit,
                                  bcmi_ltsw_l3_intf_mpls_recover_cb cb,
                                  void *user_data);

#endif  /* BCMI_LTSW_XFS_L3_INTF_H */

