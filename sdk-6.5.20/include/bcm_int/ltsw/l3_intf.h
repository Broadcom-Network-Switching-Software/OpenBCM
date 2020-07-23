/*! \file l3_intf.h
 *
 * L3 intf header file.
 * This file contains L3 interface definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMI_LTSW_L3_INTF_H
#define BCMI_LTSW_L3_INTF_H
#include <bcm_int/ltsw/types.h>

/*! Egress interface type. */
typedef enum bcmi_ltsw_l3_egr_intf_type_e {

    /*! Egress object for overlay forwarding. */
    BCMI_LTSW_L3_EGR_INTF_T_OL = 0,

    /*! Egress object for tunnel underlay forwarding. */
    BCMI_LTSW_L3_EGR_INTF_T_UL = 1,

    BCMI_LTSW_L3_EGR_INTF_T_CNT

} bcmi_ltsw_l3_egr_intf_type_t;

/*! VFI info associated with L3 interface. */
typedef struct bcmi_ltsw_l3_intf_vfi_s {
     /*! Bitmap that indicate the info to be updated. */
    uint32_t flags;
#define BCMI_LTSW_L3_INTF_VFI_F_TAG                    (1<<0)
#define BCMI_LTSW_L3_INTF_VFI_F_TAG_ACTION             (1<<1)
#define BCMI_LTSW_L3_INTF_VFI_F_MEMBERSHIP_PRFL_IDX    (1<<2)
#define BCMI_LTSW_L3_INTF_VFI_F_UNTAG_PRFL_IDX         (1<<3)
#define BCMI_LTSW_L3_INTF_VFI_F_STG                    (1<<4)

    /*! Vlan Tag. */
    uint16_t tag;

    /*! Tag action. 0: NOP; 1: add tag. */
    uint8_t tag_action;

    /*! Vlan membership profile index. */
    uint16_t membership_prfl_idx;

    /*! Untag port profile index. */
    uint16_t untag_prfl_idx;

    /*! Spanning tree group. */
    uint16_t stg;
} bcmi_ltsw_l3_intf_vfi_t;

#define BCMI_LTSW_L3_INTF_MAX_TM_STAT 2
typedef struct bcmi_ltsw_l3_iif_profile_s {
    /* DSCP mapping ptr. */
    uint32_t dscp_ptr;

    /* Enable global route. */
    bool     global_route;

    /* Enable unknown IPv4 Multicast flow copy to CPU. */
    bool     unknown_ipmcv4_to_cpu;

    /* Enable unknown IPv6 Multicast flow copy to CPU. */
    bool     unknown_ipmcv6_to_cpu;

    /* Enable drop IPv6 header type 0. */
    bool     ipv6_type0_drop;

    /* Enable unknown src ip flow copy to CPU. */
    bool     unknown_src_to_cpu;

    /* Enable IPMC redirect  to CPU. */
    bool     icmp_redirect_to_cpu;

    /* Enable unknown ipmc flow copy to CPU. */
    bool     unknown_ipmc_as_l2mc;

    /* TM stat array. */
    uint32_t tm_stat_type[BCMI_LTSW_L3_INTF_MAX_TM_STAT];
} bcmi_ltsw_l3_iif_profile_t;

/*! MPLS info associated with L3 interface. */
typedef struct bcmi_ltsw_l3_intf_mpls_s {

    /* VC label index. */
    int vc_label_index;
} bcmi_ltsw_l3_intf_mpls_t;

/*!
 * \brief Initialize L3 interface module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_intf_init(int unit);

/*!
 * \brief De-Initialize L3 interface module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_intf_deinit(int unit);

/*!
 * \brief Get the CPU interface index.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_intf_l2tocpu_idx_get(int unit);

/*!
 * \brief Validate the egress interface ID.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id egress interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT L3 interface module not initialized.
 * \retval SHR_E_PARAM Invalid ingress interface ID.
 */
extern int
bcmi_ltsw_l3_egr_intf_id_validate(int unit, int intf_id);


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
bcmi_ltsw_l3_egr_intf_id_range_get(int unit,
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
bcmi_ltsw_l3_egr_intf_to_index(int unit, bcm_if_t intf_id, int *index,
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
bcmi_ltsw_l3_egr_index_to_intf(int unit, int index,
                               bcmi_ltsw_l3_egr_intf_type_t type,
                               bcm_if_t *intf_id);

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
bcmi_ltsw_l3_intf_tnl_init_set(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t tnl_type,
    int tnl_idx);

/*!
 * \brief Internal function to get L3 egress interface tunnel initiator field.
 *
 * \param [in] unit number.
 * \param [in] intf_id index.
 * \param [out] tnl_type Tunnel type.
 * \param [out] tnl_idx initiator index.
 *
 * \retval SHR_E_NONE  errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_intf_tnl_init_get(
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
bcmi_ltsw_l3_intf_tnl_init_reset(int unit, int intf_id);

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
bcmi_ltsw_l3_intf_vfi_update(int unit, int vid,
                             bcmi_ltsw_l3_intf_vfi_t *vfi_info);

/*!
 * \brief Validate the ingress interface ID.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT L3 interface module not initialized.
 * \retval SHR_E_PARAM Invalid ingress interface ID.
 */
extern int
bcmi_ltsw_l3_ing_intf_id_validate(int unit, int intf_id);

/*!
 * \brief Get ingress interface id range.
 *
 * \param [in] unit Unit number.
 * \param [out] min Minimum interface id.
 * \param [out] max Maximum interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT L3 interface module not initialized.
 */
extern int
bcmi_ltsw_l3_ing_intf_id_range_get(int unit, int *min, int *max);

/*!
 * \brief Get class id range.
 *
 * \param [in] unit Unit number.
 * \param [out] min Minimum class id.
 * \param [out] max Maximum class id.
 * \param [out] width Number of bits of class id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT L3 interface module not initialized.
 */
extern int
bcmi_ltsw_l3_ing_class_id_range_get(int unit, int *min, int *max, int *width);

/*!
 * \brief Add rp_id to rpa_id_profile of the given ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface id.
 * \param [in] rp_id Rendezvous point id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT L3 interface module not initialized.
 * \retval SHR_E_NOT_FOUND The ingress interface is not created.
 */
extern int
bcmi_ltsw_l3_ing_intf_rp_add(int unit, int intf_id, int rp_id);

/*!
 * \brief Delete rp_id from rpa_id_profile of the given ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface id.
 * \param [in] rp_id Rendezvous point id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT L3 interface module not initialized.
 * \retval SHR_E_NOT_FOUND The ingress interface is not created.
 */
extern int
bcmi_ltsw_l3_ing_intf_rp_delete(int unit, int intf_id, int rp_id);

/*!
 * \brief Get all rp_id of the given ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface id.
 * \param [out] rp_bmp Bitmap of rendezvous point id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT L3 interface module not initialized.
 * \retval SHR_E_NOT_FOUND The ingress interface is not created.
 */
extern int
bcmi_ltsw_l3_ing_intf_rp_get(int unit, int intf_id, uint32_t *rp_bmp);

/*!
 * \brief Callback function to recover the MPLS information.
 *
 * \param [in] unit Unit number.
 * \param [in] mpls_info MPLS information.
 * \param [in] user_data User data of callback.
 */
typedef int (*bcmi_ltsw_l3_intf_mpls_recover_cb)(
    int unit,
    bcmi_ltsw_l3_intf_mpls_t *mpls_info,
    void *user_data
);

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
bcmi_ltsw_l3_intf_mpls_recover_all(int unit,
                                   bcmi_ltsw_l3_intf_mpls_recover_cb cb,
                                   void *user_data);

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
bcmi_ltsw_l3_intf_adapt_lookup_key_set(
    int unit,
    int intf_id,
    int key_type);

#endif /* BCMI_LTSW_L3_INTF_H */

