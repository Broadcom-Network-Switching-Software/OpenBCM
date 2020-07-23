/*! \file virtual.h
 *
 * Vitual management header file.
 * This file contains the resource management for VPN & VP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_VIRTUAL_H
#define BCMI_LTSW_VIRTUAL_H

#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/switch.h>

#include <bcm_int/ltsw/port.h>

#include <shr/shr_types.h>

/*!
 * \brief Enumerator for VP type.
 */
typedef enum bcmi_ltsw_virtual_vp_type_e {
    BCMI_LTSW_VP_TYPE_MPLS  = 0x0,
    BCMI_LTSW_VP_TYPE_VPLAG = 0x1,
    BCMI_LTSW_VP_TYPE_FLOW  = 0x2,
    BCMI_LTSW_VP_TYPE_ANY   = 0x3,
    BCMI_LTSW_VP_TYPE_CNT,
} bcmi_ltsw_virtual_vp_type_t;

/*!
 * \brief Enumerator for VFI type.
 */
typedef enum bcmi_ltsw_virtual_vfi_type_e {
    BCMI_LTSW_VFI_TYPE_MPLS  = 0x0,
    BCMI_LTSW_VFI_TYPE_VPLAG = 0x1,
    BCMI_LTSW_VFI_TYPE_FLOW  = 0x2,
    BCMI_LTSW_VFI_TYPE_ANY   = 0x3,
    BCMI_LTSW_VFI_TYPE_CNT,
} bcmi_ltsw_virtual_vfi_type_t;

/*! Indicator flag for VP info. */
#define BCMI_LTSW_VIRTUAL_VP_TYPE_MPLS    0x00000001
#define BCMI_LTSW_VIRTUAL_VP_TYPE_VPLAG   0x00000002
#define BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW    0x00000004
#define BCMI_LTSW_VIRTUAL_VP_TYPE_ANY     0x00000008
#define BCMI_LTSW_VIRTUAL_VP_TYPE_MASK    0x0000000F

#define BCMI_LTSW_VIRTUAL_VP_VALID        0x00010000
#define BCMI_LTSW_VIRTUAL_VP_NETWORK_PORT 0x00020000
#define BCMI_LTSW_VIRTUAL_VP_CASCADED     0x00040000

#define BCMI_LTSW_VIRTUAL_VP_ALL_MASK     0x0003000F


/*!
 * \brief Defines VP related information.
 *
 * This data structure is used to keep VP info.
 */
typedef struct bcmi_ltsw_virtual_vp_info_s {
    /*! VP control indicator. */
    uint32_t flags;

    /*! VPN associated with this VP. */
    uint32_t vpn;

    /*! L2_oif associated with this VP. */
    uint32_t l2_oif;

    /*! Dvp port group id associated with this VP. */
    uint32_t dvp_pg;
} bcmi_ltsw_virtual_vp_info_t;

/*!
 * \brief Virtual vp user.
 */
typedef enum bcmi_ltsw_virtual_vp_user_s {

    /*! L2_oif. */
    BCMI_LTSW_VIRTUAL_VP_USER_L2_OIF = 0,

    /*! Dvp port group id. */
    BCMI_LTSW_VIRTUAL_VP_USER_DVP_PG = 1,

    /*! Awlays last. */
    BCMI_LTSW_VIRTUAL_VP_USER_CNT = 2
} bcmi_ltsw_virtual_vp_user_t;

/*! Flag for VP user info. */
#define BCMI_LTSW_VIRTUAL_VP_USER_FLAG_TRUNK       0x0001
#define BCMI_LTSW_VIRTUAL_VP_USER_FLAG_REMOTE      0x0002

#define BCMI_LTSW_VIRTUAL_VPN_TYPE_L3        BCM_VPN_TYPE_MPLS_L3
#define BCMI_LTSW_VIRTUAL_VPN_TYPE_VPWS      BCM_VPN_TYPE_MPLS_VPWS
#define BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI       BCM_VPN_TYPE_MPLS_VPLS
#define BCMI_LTSW_VIRTUAL_VPN_TYPE_FLOW      BCM_VPN_TYPE_MPLS_VPLS

#define BCMI_LTSW_VIRTUAL_VPN_SET(_vpn_, _type_, _id_) \
        do { \
            if (BCM_VPN_TYPE_MPLS_L3 == (_type_)) { \
                BCM_VPN_MPLS_L3_VPN_ID_SET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MPLS_VPWS == (_type_)) { \
                BCM_VPN_MPLS_VPWS_VPN_ID_SET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MPLS_VPLS == (_type_)) { \
                BCM_VPN_MPLS_VPLS_VPN_ID_SET(_vpn_, _id_); \
            } \
        } while (0)

#define BCMI_LTSW_VIRTUAL_VPN_GET(_id_, _type_,  _vpn_) \
        do { \
            if (BCM_VPN_TYPE_MPLS_L3 == (_type_)) { \
                BCM_VPN_MPLS_L3_VPN_ID_GET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MPLS_VPWS == (_type_)) { \
                BCM_VPN_MPLS_VPWS_VPN_ID_GET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MPLS_VPLS == (_type_)) { \
                BCM_VPN_MPLS_VPLS_VPN_ID_GET(_vpn_, _id_); \
            } \
        } while (0)


#define BCMI_LTSW_VIRTUAL_VPN_IS_L3(_vpn_) \
            (((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_MPLS_L3) \
            && ((((_vpn_) >> 12) & 0xf) < BCM_VPN_TYPE_MPLS_VPWS))

#define BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(_vpn_) \
            ((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_MPLS_VPLS)

#define BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(_vpn_) \
            (((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_MPLS_VPWS) \
            && ((((_vpn_) >> 12) & 0xf) < BCM_VPN_TYPE_MPLS_VPLS))

#define BCMI_LTSW_VIRTUAL_VPN_VFI_IS_SET(_vpn_) \
        (BCMI_LTSW_VIRTUAL_VPN_IS_VPLS(_vpn_))

#define BCMI_LTSW_VIRTUAL_VPN_IS_SET(_vpn_) \
        (BCMI_LTSW_VIRTUAL_VPN_IS_L3(_vpn_) \
        || BCMI_LTSW_VIRTUAL_VPN_IS_VPWS(_vpn_) \
        || BCMI_LTSW_VIRTUAL_VPN_VFI_IS_SET(_vpn_))

/*!
 * \brief Initialize virtual resource management.
 *
 * \param [in]  unit               Unit number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_init(int unit);

/*!
 * \brief Cleanup virtual resource management.
 *
 * \param [in]  unit               Unit number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern void
bcmi_ltsw_virtual_cleanup(int unit);

/*!
 * \brief Allocate a group of VPs.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   start     First VP index to allocate from.
 * \param [in]   end       Last VP index to allocate from.
 * \param [in]   count     Num of consecutive VPs to allocate.
 * \param [in]   vp_info   VP configuration.
 * \param [out]  base_vp   Base VP index allocated.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_alloc(int unit, int start, int end, int count,
                           bcmi_ltsw_virtual_vp_info_t vp_info, int *base_vp);

/*!
 * \brief Free a group of VPs.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   type      VP type.
 * \param [in]   count     Num of consecutive VPs to allocate.
 * \param [in]   base_vp   Base VP index allocated.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_free(int unit, bcmi_ltsw_virtual_vp_type_t type, int count,
                          int base_vp);

/*!
 * \brief Update VP info.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VP ID.
 * \param [in]   vp_info   VP configuration.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_used_set(int unit, int vp,
                              bcmi_ltsw_virtual_vp_info_t vp_info);

/*!
 * \brief Check whether a VP is used or not.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VP ID.
 * \param [in]   vp_type   VP type.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_used_get(int unit, int vp,
                              bcmi_ltsw_virtual_vp_type_t vp_type);

/*!
 * \brief Initialize bcmi_ltsw_virtual_vp_info_t structure.
 *
 * \param [in-out]   vp_info   VP info structure.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern void
bcmi_ltsw_virtual_vp_info_init (bcmi_ltsw_virtual_vp_info_t *vp_info);

/*!
 * \brief Get VP info based on VP ID.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VP ID.
 * \param [out]  vp_info   VP configuration.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_info_get(int unit, int vp,
                              bcmi_ltsw_virtual_vp_info_t *vp_info);

/*!
 * \brief Allocate a VFI.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VFI type.
 * \param [out]  vfi       VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vfi_alloc(int unit, bcmi_ltsw_virtual_vfi_type_t type,
                            int *vfi);

/*!
 * \brief Allocate a VFI with given ID.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VFI type.
 * \param [out]  vfi       VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vfi_alloc_with_id(int unit, bcmi_ltsw_virtual_vfi_type_t type,
                                    int vfi);

/*!
 * \brief Free a VFI.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VFI type.
 * \param [in]   vfi       VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vfi_free(int unit, bcmi_ltsw_virtual_vfi_type_t type,
                           int vfi);

 /*!
 * \brief Check whether a VFI is used or not.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   vfi        VFI index.
 * \param [in]   type       VFI type.
 *
 * \retval TRUE   Used.
 * \retval FALSE  Not used.
 */
extern int
bcmi_ltsw_virtual_vfi_used_get(int unit, int vfi,
                               bcmi_ltsw_virtual_vfi_type_t type);

/*!
 * \brief Get physical VFI index of VPN.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vpn          VPN ID.
 * \param [out]  vfi          Physical VFI index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vfi_idx_get(int unit, bcm_vpn_t vpn, uint32_t *vfi);

/*!
 * \brief Check whether a VFI based VPN is valid.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vpn          VPN or VLAN ID.
 *
 * \retval TRUE    ID is valid.
 * \retval FALSE   ID is invalid.
 */
extern int
bcmi_ltsw_virtual_vlan_vpn_valid(int unit, bcm_vpn_t vpn);

/*!
 * \brief Get VPN ID based on physical VFI index.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vfi          Physical VFI index.
 * \param [out]  vpn          VPN ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vpnid_get(int unit, uint32_t vfi, bcm_vpn_t *vpn);

/*!
 * \brief Get VFI num.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  vfi_num      Num of VFI entires.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vfi_num_get(int unit, uint32_t *vfi_num);

/*!
 * \brief Get VP num.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  vp_num       Num of VP entires.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_num_get(int unit, uint32_t *vp_num);

/*!
 * \brief Consruct gport based on VP.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vp           VP ID.
 * \param [out]  gport        gport of VP.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_encode_gport(int unit, int vp, int *gport);

/*!
 * \brief Consruct gport info based on VP.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   vp           VP ID.
 * \param [out]  gport_dest   gport info.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_gport_dest_fill(int unit, int vp,
                                     bcmi_ltsw_gport_info_t *gport_dest);

/*!
 * \brief  Finds out if GPORT is VFI type virtual port.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        gport ID.
 *
 * \retval TRUE/FALSE
 */
extern int
bcmi_ltsw_virtual_vp_is_vfi_type(int unit, bcm_gport_t gport);

/*!
 * \brief Checks and gets a valid vp.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        gport ID.
 * \param [out]  vp           VP ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vfi_type_vp_get(int unit, bcm_gport_t gport, int *vp);

/*!
 * \brief Get VFI index range.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  start        VFI start index.
 * \param [out]  end          VFI end index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vfi_range_get(int unit, uint32_t *start, uint32_t *end);

/*!
 * \brief Get VP index range.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  start        VP start index.
 * \param [out]  end          VP end index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_range_get(int unit, uint32_t *start, uint32_t *end);

/*!
 * \brief Get L2_IIF index range.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  start        L2_IIF start index.
 * \param [out]  end          L2_IIF end index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_l2_iif_range_get(int unit, uint32_t *start, uint32_t *end);

/*!
 * \brief Get L2_IIF num.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  l2_iif_num   Num of L2_IIF entires.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_l2_iif_num_get(int unit, uint32_t *l2_iif_num);

/*!
 * \brief Get the ARL hardware learning options for this port.
 *
 * This function defines what the hardware will do when a packet
 * is seen with an unknown address.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_learn_get(int unit, bcm_port_t port, uint32_t *flags);

/*!
 * \brief Set the ARL hardware learning options for this port.
 *
 * This function defines what the hardware will do when a packet
 * is seen with an unknown address.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_learn_set(int unit, bcm_port_t port, uint32_t flags);

/*!
 * \brief Set virtual port's vlan actions.
 *
 * \param [in] unit   Unit number.
 * \param [in] port   Gport.
 * \param [in] action Vlan tag actions.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_vlan_action_set(int unit, bcm_port_t port,
                                       bcm_vlan_action_set_t *action);

/*!
 * \brief Get virtual port's vlan actions.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  port   Gport.
 * \param [out] action Vlan tag actions.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_vlan_action_get(int unit, bcm_port_t port,
                                       bcm_vlan_action_set_t *action);

/*!
 * \brief Reset virtual port's vlan actions.
 *
 * \param [in] unit   Unit number.
 * \param [in] port   Gport.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_vlan_action_reset(int unit, bcm_port_t port);

/*!
 * \brief Get the class ID of this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pclass Classification type.
 * \param [out] pclass_id class ID of the port.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_class_get(int unit, bcm_port_t port,
                                 bcm_port_class_t pclass,
                                 uint32_t *pclass_id);

/*!
 * \brief Set the class ID of this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pclass Classification type.
 * \param [in] pclass_id Class ID of the port.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_class_set(int unit, bcm_port_t port,
                                 bcm_port_class_t pclass,
                                 uint32_t pclass_id);

/*!
 * \brief Get status of VLAN specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Vlan control type.
 * \param [out] arg Vlaue for given feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_vlan_ctrl_get(int unit, bcm_port_t port,
                                     bcm_vlan_control_port_t type,
                                     int *arg);

/*!
 * \brief Enable/Disable VLAN specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Vlan control type.
 * \param [in] arg Vlaue for given feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_vlan_ctrl_set(int unit, bcm_port_t port,
                                     bcm_vlan_control_port_t type,
                                     int arg);

/*!
 * \brief Get status of Mirror specified port feature.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   port       Port number.
 * \param [out]  enable     Bitmap of mirror session status.
 * \param [out]  mirror_idx Index array.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_mirror_ctrl_get(int unit,  bcm_port_t port, int *enable,
                                       int *mirror_idx);

/*!
 * \brief Enable/Disable Mirror specified port feature.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   port       Port number.
 * \param [in]   enable     Bitmap of mirror session status.
 * \param [in]   mirror_idx Index array.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_mirror_ctrl_set(int unit,  bcm_port_t port, int enable,
                                       int *mirror_idx);

/*!
 * \brief Save the virtual port cascaded state.
 *
 * \param [in] unit         Unit number.
 * \param [in] port         Gport number.
 * \param [in] cascaded  cascaded state.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_virtual_port_cascaded_set(
    int unit, bcm_gport_t port, int cascaded);

/*!
 * \brief Set egress object to port.
 *
 * \param [in] unit        Unit number.
 * \param [in] port        Gport number.
 * \param [in] egress_if   Egress object.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
bcmi_ltsw_virtual_port_egress_obj_set(int unit, bcm_gport_t port,
                                      bcm_if_t egress_if);

/*!
 * \brief Get egress object from port.
 *
 * \param [in]  unit        Unit number.
 * \param [in]  port        Gport number.
 * \param [out] egress_if   Egress object.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
bcmi_ltsw_virtual_port_egress_obj_get(int unit, bcm_gport_t port,
                                      bcm_if_t *egress_if);

/*!
 * \brief Check bind status of egress object.
 *
 * \param [in]  unit        Unit number.
 * \param [in]  egress_if   Egress object.
 * \param [out] used        bind status.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
bcmi_ltsw_virtual_port_egress_obj_used(int unit, bcm_if_t egress_if, int *used);

/*!
 * \brief Set DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode DSCP mapping mode.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode);

/*!
 * \brief Get DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] mode DSCP mapping mode.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode);

/*!
 * \brief Get VLAN based PHB mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr VLAN based PHB mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_phb_vlan_map_get(int unit, bcm_port_t port, int *map_ptr);

/*!
 * \brief Set VLAN based PHB mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr VLAN based PHB mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_phb_vlan_map_set(int unit, bcm_port_t port, int map_ptr);

/*!
 * \brief Get DSCP based PHB mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr DSCP based PHB mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_phb_dscp_map_get(int unit, bcm_port_t port, int *map_ptr);

/*!
 * \brief Set DSCP based PHB mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr DSCP based PHB mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_phb_dscp_map_set(int unit, bcm_port_t port, int map_ptr);

/*!
* \brief Get VLAN based PHB remarking mapping pointer.
*
* \param [in] unit Unit number.
* \param [in] port Port number.
* \param [out] map_ptr VLAN based PHB remarking mapping  ID.
*
* \retval SHR_E_NONE              No errors.
* \retval !SHR_E_NONE             Failure.
*/
extern int
bcmi_ltsw_virtual_port_phb_vlan_remark_map_get(int unit, bcm_port_t port,
                                               int *map_ptr);

/*!
 * \brief Set VLAN based PHB remarking mapping pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_ptr VLAN based PHB remarking mapping pointer.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_php_vlan_remark_map_set(int unit, bcm_port_t port,
                                               int map_ptr);
/*!
 * \brief Get the supported network group number.
 *
 * \param [in] unit Unit number.
 * \param [out] num  Number of the supported network group.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_nw_group_num_get(int unit, int *num);

/*!
 * \brief Get pipe bitmap for a dvp port group.
 *
 * \param [in]  unit      Unit number.
 * \param [in]  pg        Port goup.
 * \param [out] pipe_bmp  Pipe bitmap.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_group_pipe_get(int unit, int pg, uint32_t *pipe_bmp);

/*!
 * \brief Update VP info based on certain VP id.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vp        VP ID.
 * \param [in]   user_bmp  User bitmap.
 * \param [in]   flags     Flags about l2 if type
 * \param [out]  vp_info   VP configuration.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_vp_info_update(int unit, int vp, int flags,
                                 uint32_t user_bmp,
                                 bcmi_ltsw_virtual_vp_info_t *vp_info_new);

/*!
 * \brief Update VP info based on trunk change.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   tid          Tunk id.
 * \param [in]   del          Whether to delete this trunk.
 * \param [in]   leave_pbmp   Leaving ports pipe bitmap.
 * \param [in]   stay_pbmp    Staying ports pipe bitmap.
 * \param [in]   join_pbmp    Joining ports pipe bitmap.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_trunk_pipe_update(int unit, int l2_oif, int del,
     uint32_t leave_pbmp, uint32_t stay_pbmp, uint32_t join_pbmp);

/*!
 * \brief Clean up trunk user in vp info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_virtual_vp_user_trunk_cleanup(int unit);

/*!
 * \brief Get the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [out] value Current value of the port feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_control_get(int unit, bcm_port_t port,
                                   bcm_port_control_t type, int *value);

/*!
 * \brief Set the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [in] value Value of the port feature to be set.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_port_control_set(int unit, bcm_port_t port,
                                   bcm_port_control_t type, int value);

/*!
 * \brief Attach counter entries to the given port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_virtual_port_stat_attach(int unit, bcm_port_t port,
                                   uint32_t stat_counter_id);

/*!
 * \brief Detach counter entries to given port with a given stat counter id.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_virtual_port_stat_detach_with_id(int unit, bcm_port_t port,
                                           uint32_t stat_counter_id);

/*!
 * \brief Configures the split horizon network group configuration for
 * a given pair of source and destination network group.
 *
 * \param [in] unit Unit Number.
 * \param [in] source_network_group_id Source network group id.
 * \param [in] config Configuration information.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_virtual_network_group_split_config_set(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config);

/*!
 * \brief Gets the split horizon network group configuration for
 * a given pair of source and destination network group.
 *
 * \param [in] unit Unit Number.
 * \param [in] source_network_group_id Source network group id.
 * \param [out] config Configuration information.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_virtual_network_group_split_config_get(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config);

/*!
 * \brief Set egress adaptation key to port.
 *
 * \param [in] unit        Unit number.
 * \param [in] port        Gport number.
 * \param [in] key_type    Lookup key.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
bcmi_ltsw_virtual_port_egress_adapt_key_set(
    int unit,
    bcm_gport_t port,
    int key_type);

/*!
 * \brief Get egress adaptation key from port.
 *
 * \param [in]  unit        Unit number.
 * \param [in]  port        Gport number.
 * \param [out] key_type    Lookup key.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
bcmi_ltsw_virtual_port_egress_adapt_key_get(
    int unit,
    bcm_gport_t port,
    int *key_type);

/*!
 * \brief sync egress adaptation key to HW.
 *
 * \param [in] unit        Unit number.
 * \param [in] port        Gport number.
 * \param [in] egress_if   Egress object.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
bcmi_ltsw_virtual_port_egress_adapt_key_sync(
    int unit,
    bcm_gport_t port,
    bcm_if_t egress_if);

/*!
 * \brief Get tunnel index range.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  start        Tunnel start index.
 * \param [out]  end          Tunnel end index.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_virtual_tnl_range_get(int unit, uint32_t *start, uint32_t *end);

#endif /* BCMI_LTSW_VIRTUAL_H */
