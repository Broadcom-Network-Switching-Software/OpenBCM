/*! \file l3_intf.h
 *
 * L3 intf header file.
 * This file contains L3 interface definitions internal to XGS family.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_XGS_L3_INTF_H
#define BCMI_LTSW_XGS_L3_INTF_H

#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/l3_intf_int.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/types.h>

#include <sal/sal_types.h>

/* L3 interface tables ID. */
typedef enum xgs_ltsw_l3_intf_tbl_id_e {
    XGS_LTSW_L3_INTF_TBL_IIF,
    XGS_LTSW_L3_INTF_TBL_EIF,
    XGS_LTSW_L3_INTF_TBL_CNT
} xgs_ltsw_l3_intf_tbl_id_t;

/* L3 interface table attributes. */
/* Ingress interface. */
#define XGS_LTSW_L3_INTF_TBL_ATTR_ING   (1 << 0)
/* Egress interface. */
#define XGS_LTSW_L3_INTF_TBL_ATTR_EGR   (1 << 1)

/* IIF field ID. */
typedef enum xgs_ltsw_l3_iif_fld_e {
    XGS_LTSW_L3_INTF_TBL_ING_INDEX,
	XGS_LTSW_L3_INTF_TBL_ING_VRF,
	XGS_LTSW_L3_INTF_TBL_ING_L3_IIF_PROFILE_ID,
	XGS_LTSW_L3_INTF_TBL_ING_L3_IP_OPTION_CONTROL_PROFILE_ID,
	XGS_LTSW_L3_INTF_TBL_ING_IPV4UC,
    XGS_LTSW_L3_INTF_TBL_ING_IPV6UC,
    XGS_LTSW_L3_INTF_TBL_ING_IPV4MC,
    XGS_LTSW_L3_INTF_TBL_ING_IPV6MC,
    XGS_LTSW_L3_INTF_TBL_ING_L3_MC_IIF_ID,
    XGS_LTSW_L3_INTF_TBL_ING_L3_OVERRIDE_IP_MC_DO_VLAN,
    XGS_LTSW_L3_INTF_TBL_ING_CLASS_ID,
    XGS_LTSW_L3_INTF_TBL_ING_ECN_TNL_DECAP_ID,
    XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION,
    XGS_LTSW_L3_INTF_TBL_ING_INT_IFA,
    XGS_LTSW_L3_INTF_TBL_ING_INT_IOAM,
    XGS_LTSW_L3_INTF_TBL_ING_INT_DATAPLANE,
    XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT
} xgs_ltsw_l3_iif_fld_t;

/* EIF field ID. */
typedef enum xgs_ltsw_l3_eif_fld_e {
    XGS_LTSW_L3_INTF_TBL_EGR_INDEX,
    XGS_LTSW_L3_INTF_TBL_EGR_VLAN_ID,
    XGS_LTSW_L3_INTF_TBL_EGR_MAC_SA,
    XGS_LTSW_L3_INTF_TBL_EGR_TTL,
    XGS_LTSW_L3_INTF_TBL_EGR_L2_SWITCH,
    XGS_LTSW_L3_INTF_TBL_EGR_CLASS_ID,
    XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV4_ENCAP_INDEX,
	XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV6_ENCAP_INDEX,
	XGS_LTSW_L3_INTF_TBL_EGR_TNL_MPLS_ENCAP_INDEX,
    XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE,
    XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION,
    XGS_LTSW_L3_INTF_TBL_EGR_DSCP,
    XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_IP_INT_PRI_TO_DSCP_ID,
    XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_OTAG_ACTION,
    XGS_LTSW_L3_INTF_TBL_EGR_OPRI,
    XGS_LTSW_L3_INTF_TBL_EGR_OCFI,
    XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_INT_PRI_TO_OTAG_ID,
    XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION,
    XGS_LTSW_L3_INTF_TBL_EGR_VFI,
    XGS_LTSW_L3_INTF_TBL_EGR_ADAPT_LOOKUP_KEY_MODE,
    XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT
} xgs_ltsw_l3_eif_fld_t;

typedef enum xgs_ltsw_l3_intf_tnl_type_e {
    XGS_LTSW_L3_INTF_TNL_TYPE_NONE,
    XGS_LTSW_L3_INTF_TNL_TYPE_IPV4,
    XGS_LTSW_L3_INTF_TNL_TYPE_IPV6,
    XGS_LTSW_L3_INTF_TNL_TYPE_MPLS,
    XGS_LTSW_L3_INTF_TNL_TYPE_CNT
} xgs_ltsw_l3_intf_tnl_type_t;

typedef enum xgs_ltsw_l3_intf_dscp_action_e {
    XGS_LTSW_L3_INTF_DSCP_ACTION_NONE,
    XGS_LTSW_L3_INTF_DSCP_ACTION_FIXED,
    XGS_LTSW_L3_INTF_DSCP_ACTION_MAP,
    XGS_LTSW_L3_INTF_DSCP_ACTION_CNT
} xgs_ltsw_l3_intf_dscp_action_t;

typedef enum xgs_ltsw_l3_intf_l2_tag_action_e {
    XGS_LTSW_L3_INTF_L2_TAG_ACTION_NONE,
    XGS_LTSW_L3_INTF_L2_TAG_ACTION_FIXED,
    XGS_LTSW_L3_INTF_L2_TAG_ACTION_MAP,
    XGS_LTSW_L3_INTF_L2_TAG_ACTION_CNT
} xgs_ltsw_l3_intf_l2_tag_action_t;

/*!
 * \brief Initialize L3 intf module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_intf_init(int unit);

/*!
 * \brief De-Initialize L3 intf module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_intf_deinit(int unit);

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
xgs_ltsw_l3_ing_intf_id_range_get(int unit, int *min, int *max);

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
xgs_ltsw_l3_ing_class_id_range_get(int unit, int *min, int *max, int *width);

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
xgs_ltsw_l3_ing_intf_create(int unit, bcm_l3_ingress_t *intf_info,
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
xgs_ltsw_l3_ing_intf_destroy(int unit, bcm_if_t intf_id);

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
xgs_ltsw_l3_ing_intf_get(int unit, bcm_if_t intf_id,
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
xgs_ltsw_l3_ing_intf_find(int unit, bcm_l3_ingress_t *ing_intf,
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
xgs_ltsw_l3_ing_intf_traverse(int unit,
                              bcm_l3_ingress_traverse_cb trav_fn,
                              void *user_data);

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
xgs_ltsw_l3_ing_intf_flexctr_attach(int unit,
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
xgs_ltsw_l3_ing_intf_flexctr_detach(int unit, bcm_if_t intf_id);

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
xgs_ltsw_l3_ing_intf_flexctr_info_get(int unit,
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
xgs_ltsw_l3_egr_l2cpu_intf_id_get(int unit, int *cpu_intf_id);

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
xgs_ltsw_l3_egr_intf_id_range_get(int unit,
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
xgs_ltsw_l3_egr_intf_to_index(int unit, bcm_if_t intf_id, int *index,
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
xgs_ltsw_l3_egr_index_to_intf(int unit, int index,
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
xgs_ltsw_l3_egr_intf_create(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

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
xgs_ltsw_l3_egr_intf_get(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

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
xgs_ltsw_l3_egr_intf_find(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

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
xgs_ltsw_l3_egr_intf_delete(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Delete all L3 egress interfaces.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egr_intf_delete_all(int unit);

/*!
 * \brief Attach flexctr action to the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egr_intf_flexctr_attach(int unit,
                                    bcm_if_t intf_id,
                                    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egr_intf_flexctr_detach(int unit, bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_egr_intf_flexctr_info_get(int unit,
                                      bcm_if_t intf_id,
                                      bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Internal function to set L3 egress interface tunnel initiator field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [in] tnl_type Tunnel type.
 * \param [in] tnl_idx Tunnel initiator index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_intf_tnl_init_set(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t tnl_type,
    int tnl_idx);

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
xgs_ltsw_l3_intf_tnl_init_get(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t *tnl_type,
    int *tnl_idx);

/*!
 * \brief Internal function to reset L3 egress interface tunnel initiator field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_intf_tnl_init_reset(int unit, int intf_id);


/*!
 * \brief Internal function to set L3 egress interface adaptation key field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [in] key_type Key type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_intf_adapt_lookup_key_set(
    int unit,
    int intf_id,
    int key_type);


#endif  /* BCMI_LTSW_XGS_L3_INTF_H */

