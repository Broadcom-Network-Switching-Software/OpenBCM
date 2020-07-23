/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       vlan.c
 * Purpose:    Broadcom StrataSwitch VLAN switch API for TH3.
 */
#include <soc/drv.h>
#include <soc/rcpu.h>
#include <bcm/error.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>

#ifdef BCM_TOMAHAWK3_SUPPORT
#include <bcm_int/tomahawk3_dispatch.h>
#include <bcm_int/esw/tomahawk3.h>

#define CHECK_INIT(unit)                                    \
        do {                                                \
            if (!vlan_info[unit].init) {                    \
                return BCM_E_INIT;                          \
            }                                               \
        } while(0)                                          \

#define CHECK_INIT_AND_VLAN_ID(unit, vlan)                  \
        do {                                                \
            CHECK_INIT(unit);                               \
            VLAN_CHK_ID(unit, vlan);                        \
        } while(0)

#define BCM_TH3_VLAN_CONTROL_VLAN_ALL_VALID_MASK            \
    (BCM_VLAN_CONTROL_VLAN_VRF_MASK |                       \
     BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK |           \
     BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK |                \
     BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK |                \
     BCM_VLAN_CONTROL_VLAN_IP6_MCAST_FLOOD_MODE_MASK |      \
     BCM_VLAN_CONTROL_VLAN_IP4_MCAST_FLOOD_MODE_MASK |      \
     BCM_VLAN_CONTROL_VLAN_L2_MCAST_FLOOD_MODE_MASK |       \
     BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK |                  \
     BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK |           \
     BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK |           \
     BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK |   \
     BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK |     \
     BCM_VLAN_CONTROL_VLAN_PROTOCOL_PKT_MASK |              \
     BCM_VLAN_CONTROL_VLAN_L3_IF_CLASS_MASK)

#define BCM_TH3_VLAN_CONTROL_VLAN_SUPPORTED_CTRL_FLAGS      \
    (BCM_VLAN_LEARN_DISABLE |                               \
     BCM_VLAN_ICMP_REDIRECT_TOCPU |                         \
     BCM_VLAN_IGMP_SNOOP_DISABLE |                          \
     BCM_VLAN_IP4_DISABLE |                                 \
     BCM_VLAN_IP4_MCAST_DISABLE |                           \
     BCM_VLAN_IP4_MCAST_L2_DISABLE |                        \
     BCM_VLAN_IP6_DISABLE |                                 \
     BCM_VLAN_IP6_MCAST_DISABLE |                           \
     BCM_VLAN_IP6_MCAST_L2_DISABLE |                        \
     BCM_VLAN_IPMC_DO_VLAN_DISABLE |                        \
     BCM_VLAN_L2_CLASS_ID_ONLY |                            \
     BCM_VLAN_L3_CLASS_ID |                                 \
     BCM_VLAN_L3_VRF_GLOBAL_DISABLE |                       \
     BCM_VLAN_MPLS_DISABLE |                                \
     BCM_VLAN_NON_UCAST_DROP |                              \
     BCM_VLAN_NON_UCAST_TOCPU |                             \
     BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU |                     \
     BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU |                     \
     BCM_VLAN_UNKNOWN_UCAST_DROP |                          \
     BCM_VLAN_UNKNOWN_UCAST_TOCPU)

#define BCM_TH3_VLAN_GPORT_ADD_SUPPORTED_FLAGS              \
    (BCM_VLAN_GPORT_ADD_UNTAGGED |                          \
     BCM_VLAN_GPORT_ADD_INGRESS_ONLY |                      \
     BCM_VLAN_GPORT_ADD_EGRESS_ONLY |                       \
     BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)

typedef enum bcm_th3_cmd_e {
    BCM_TH3_GET,
    BCM_TH3_SET,
    BCM_TH3_DEL
} bcm_th3_cmd_t;

#if defined(BCM_WARM_BOOT_SUPPORT)
#include <soc/scache.h>
#include <bcm/module.h>
#include <bcm_int/esw/switch.h>

#define BCM_WB_VERSION_1_0          SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION      BCM_WB_VERSION_1_0

/*
 * Function:
 *      _bcm_th3_vlan_reinit
 *
 * Purpose:
 *      Recover VLAN state from hardware (reload)
 *
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX - Other failures.
 *
 * Note:
 */
STATIC int
_bcm_th3_vlan_reinit(int unit)
{
    bcm_vlan_info_t *vi = &vlan_info[unit];
    int rv;
    soc_scache_handle_t scache_handle;
    uint8 *vlan_scache_ptr;
    uint8  flood_mode;
    uint16 recovered_ver = BCM_WB_VERSION_1_0;

    /* Clean up existing vlan_info */
    vi->init = TRUE;
    BCM_IF_ERROR_RETURN(_bcm_vlan_cleanup(unit));

#ifdef BCM_MCAST_FLOOD_DEFAULT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_flood_default_set(unit, BCM_MCAST_FLOOD_DEFAULT));
#else
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_flood_default_set(unit, BCM_VLAN_MCAST_FLOOD_UNKNOWN));
#endif

    /*
     * Initialize software structures
     */
    vi->defl = BCM_VLAN_DEFAULT;
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VLAN, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &vlan_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (rv == SOC_E_NOT_FOUND) {
        /* Use non-Level 2 case */
    } else if (SOC_FAILURE(rv)) {
        return rv;
    } else {
        /* Success, recover default VLAN */
        sal_memcpy(&vi->defl, vlan_scache_ptr, sizeof(vi->defl));
        vlan_scache_ptr += sizeof(vi->defl);

        /* Flood mode */
        /* Type for 'flood_mode' is an enum, which values fit in uint8 */
        sal_memcpy(&flood_mode, vlan_scache_ptr, sizeof(flood_mode));
        vi->flood_mode = flood_mode;
        vlan_scache_ptr += sizeof(flood_mode);
    }

    vi->count = 0;

    /* Reinit the VLAN Cross Connect Internal Database */
    BCM_IF_ERROR_RETURN(_bcm_th3_vlan_xconnect_db_reinit(unit));

    vi->init = TRUE;

    BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->bmp));
    BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->pre_cfg_bmp));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_reload(unit, &vi->bmp, &vi->count));

    if (0 == _BCM_VBMP_LOOKUP(vi->bmp, vi->defl)) {
        LOG_WARN(BSL_LS_BCM_VLAN,
                 (BSL_META_U(unit,
                             "Warm Reboot: Default VLAN %4d does not exist!\n"),
                  vi->defl));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_esw_vlan_sync
 *
 * Purpose:
 *      Record VLAN module persisitent info for Level 2 Warm Boot
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 */
int
_bcm_th3_vlan_sync(int unit)
{
    bcm_vlan_info_t     *vi = &vlan_info[unit];
    soc_scache_handle_t scache_handle;
    uint8 *vlan_scache_ptr;
    uint8  flood_mode;

    CHECK_INIT(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VLAN, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &vlan_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL));
    /* Default VLAN */
    sal_memcpy(vlan_scache_ptr, &vi->defl, sizeof(vi->defl));
    vlan_scache_ptr += sizeof(vi->defl);

    /* Flood mode */
    /* Type for 'flood_mode' is an enum, which values fit in uint8 */
    flood_mode = vlan_info[unit].flood_mode;
    sal_memcpy(vlan_scache_ptr, &flood_mode, sizeof(flood_mode));
    vlan_scache_ptr += sizeof(flood_mode);

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_th3_vlan_port_add
 *
 * Purpose:
 *      Main part of bcm_vlan_port_add; assumes locking already done.
 *
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid - (IN) VLAN ID to add port to as a member.
 *      pbmp - (IN) egress port bitmap for members of VLAN
 *      ubmp - (IN) untagged members of VLAN
 *      ing_pbmp - (IN) ingress port bitmap for members of VLAN
 *      flags - (IN) VLAN_PORT flags qualifying the member ports.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX - Other failures.
 *
 * Note:
 */
STATIC int
_bcm_th3_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp,
                   pbmp_t ing_pbmp, int flags)
{
    pbmp_t  vlan_pbmp, vlan_ubmp, vlan_ing_pbmp, orig_pbmp;

    /* No such thing as untagged CPU */
    SOC_PBMP_REMOVE(ubmp, PBMP_CMIC(unit));

    /* Remove internal loopback port from vlan port bitmaps. Hardware does not
     * perform ingress/egress VLAN membership checks on internal loopback port.
     */
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_PBMP_REMOVE(ubmp, PBMP_LB(unit));
    SOC_PBMP_REMOVE(ing_pbmp, PBMP_LB(unit));

    /* Don't add ports that are already there */
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_port_get(
            unit, vid, &vlan_pbmp, &vlan_ubmp, &vlan_ing_pbmp));

    SOC_PBMP_ASSIGN(orig_pbmp, pbmp);
    COMPILER_REFERENCE(orig_pbmp);
    /* But do allow for making the port tagged/
     * untagged if its already added to the vlan
     */
    SOC_PBMP_XOR(vlan_ubmp, ubmp);
    SOC_PBMP_REMOVE(vlan_pbmp, vlan_ubmp);
    SOC_PBMP_REMOVE(pbmp, vlan_pbmp);
    SOC_PBMP_REMOVE(ubmp, vlan_pbmp);
    SOC_PBMP_REMOVE(ing_pbmp, vlan_ing_pbmp);

    if (SOC_PBMP_NOT_NULL(pbmp) ||
        SOC_PBMP_NOT_NULL(ubmp) || SOC_PBMP_NOT_NULL(ing_pbmp)) {
        BCM_IF_ERROR_RETURN(
            mbcm_driver[unit]->mbcm_vlan_port_add(
                    unit, vid, pbmp, ubmp, ing_pbmp));
    }
    if (soc_feature(unit, soc_feature_vlan_egress_membership_l3_only) &&
        (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        if (SOC_PBMP_NOT_NULL(pbmp)) {
            SOC_PBMP_CLEAR(ing_pbmp);
            SOC_PBMP_CLEAR(ubmp);
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_vlan_table_port_remove(unit, vid, pbmp, ubmp,
                                                  ing_pbmp, VLAN_2_TABm));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_vlan_control_port_resolve
 *
 * Purpose:
 *     Internal function to validate the given port to be a valid port or
 *     supported GPORT. Once validation successfully completes, resolve the
 *     given port to its local port representation.
 *
 * Parameters:
 *     unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *     in_port - (IN) port/gport to be validated and resolved.
 *     only_allow_gport - (IN) in_port has to be a gport.
 *     out_port - (OUT) local port. If NULL then the function would only
 *                      validate the given port.
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_PORT - If the given port is not supported for TH3 VLAN operation
 *                  or in_port is not a gport and only_allow_gport is nonzero
 *
 * Note:
 */
STATIC INLINE int
_bcm_th3_vlan_control_port_resolve(
    int unit, bcm_port_t in_port, uint8 only_allow_gport, bcm_port_t *out_port)
{
    int rv;
    int local_modid;
    bcm_port_t local_port;

    /* Check for TH3 supported port/gports */
    if (BCM_GPORT_IS_SET(in_port)) {
        if (BCM_GPORT_IS_LOCAL(in_port)) {
            local_port = BCM_GPORT_LOCAL_GET(in_port);
        } else if (BCM_GPORT_IS_LOCAL_CPU(in_port)) {
            local_port = CMIC_PORT(unit);
        } else if (BCM_GPORT_IS_DEVPORT(in_port)) {
            local_port = BCM_GPORT_DEVPORT_PORT_GET(in_port);
            if (unit != BCM_GPORT_DEVPORT_DEVID_GET(in_port)) {
                return BCM_E_PORT;
            }
        } else if (BCM_GPORT_IS_MODPORT(in_port)) {
            rv = bcm_esw_stk_my_modid_get(unit, &local_modid);
            if (BCM_FAILURE(rv) ||
                (local_modid != BCM_GPORT_MODPORT_MODID_GET(in_port))) {
                return BCM_E_PORT;
            }
            local_port = BCM_GPORT_MODPORT_PORT_GET(in_port);
        } else {
            /* All other GPORTS are not supported for the VLAN operations */
            return BCM_E_PORT;
        }
    } else if (TRUE == only_allow_gport) {
        return BCM_E_PORT;
    } else {
        local_port = in_port;
    }

    /* Check validity of deduced local port */
    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    /* If caller wants the port resolution then provide */
    if (out_port) {
        *out_port = local_port;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_vlan_control_port_config_command
 *
 * Purpose:
 *     Internal function to get/set port based VLAN configuration
 *
 * Parameters:
 *     unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *     localport - (IN) port on which the VLAN configuration has to be applied
 *     type - (IN) value from bcm_vlan_control_port_t enumeration list
 *     cmd - (IN) If BCM_TH3_SET then SET the setting else GET it
 *     arg - (INOUT) value to be set or to be retrieved is stored
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_PARAM - Given arg is NULL or type is not supported
 *     BCM_E_XXX - Other failures
 *
 * Note:
 *     It is assumed that localport parameter is a valid port and hence no
 *     further checks performed.
 */
STATIC int
_bcm_th3_vlan_control_port_config_command(int unit,
    bcm_port_t localport, bcm_vlan_control_port_t type, uint8 cmd, int *arg)
{
    uint32 data, evc;

    if (NULL == arg) {
        return BCM_E_PARAM;
    }

    switch (type) {
        case bcmVlanPortVerifyOuterTpid:
            /* Call the appr. port module interface */
            if (BCM_TH3_SET == cmd) {
                return (_bcm_esw_port_config_set(
                            unit, localport, _bcmPortVerifyOuterTpid, !!*arg));
            } else {
                return (_bcm_esw_port_config_get(
                            unit, localport, _bcmPortVerifyOuterTpid, arg));
            }

        case bcmVlanPortOuterTpidSelect:
            /*
             * Access and/or Modify the OUTER_TPID_SEL field in the register
             * EGR_VLAN_CONTROL_1 which is indexed by the given port parameter
             */
            if (BCM_TH3_SET == cmd) {
                if (*arg != BCM_PORT_OUTER_TPID &&
                    *arg != BCM_VLAN_OUTER_TPID) {
                    return BCM_E_PARAM;
                }
                SOC_IF_ERROR_RETURN(
                    READ_EGR_VLAN_CONTROL_1r(unit, localport, &evc));
                data = evc;
                soc_reg_field_set(
                        unit, EGR_VLAN_CONTROL_1r, &evc, OUTER_TPID_SELf, *arg);
                if (evc != data) {
                    SOC_IF_ERROR_RETURN(
                        WRITE_EGR_VLAN_CONTROL_1r(unit, localport, evc));
                }
            } else {
                SOC_IF_ERROR_RETURN(READ_EGR_VLAN_CONTROL_1r(unit, localport, &evc));
                *arg = soc_reg_field_get(
                           unit, EGR_VLAN_CONTROL_1r, evc, OUTER_TPID_SELf);
            }
            return BCM_E_NONE;

        default:
            return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     _bcm_th3_vlan_port_get
 *
 * Purpose:
 *     Helper routine to retrieve the list of
 *     the member ports of an existing VLAN.
 *
 * Parameters:
 *     unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *     vlan - (IN) VLAN ID to retrieve the gport from.
 *     pbmp - (OUT) EGRESS port bitmap
 *     ubmp - (OUT) Untagged EGRESS port bitmap
 *     ing_pbmp - (OUT) INGRESS port bitmap
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_NOT_FOUND - VLAN ID not existing
 *     BCM_E_PARAM
 *     BCM_E_UNAVAIL - Feature unavailable
 *     BCM_E_FAIL - Other failures
 *
 * Note:
 */
STATIC INLINE int
_bcm_th3_vlan_port_get(
    int unit, bcm_vlan_t vid, pbmp_t *pbmp, pbmp_t *ubmp, pbmp_t *ing_pbmp)
{
    int rv;

    /* Check if the given VLAN ID is an existing vlan */
    if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
        return BCM_E_NOT_FOUND;
    }

    /* Init all given port bitmaps */
    if (pbmp) {
        BCM_PBMP_CLEAR(*pbmp);
    }
    if (ubmp) {
        BCM_PBMP_CLEAR(*ubmp);
    }
    if (ing_pbmp) {
        BCM_PBMP_CLEAR(*ing_pbmp);
    }

    /* Call lower level routine to retrieve bounded ports */
    BCM_LOCK(unit);
    rv = mbcm_driver[unit]->
        mbcm_vlan_port_get(unit, vid, pbmp, ubmp, ing_pbmp);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_th3_ing_vlan_tag_action_command
 *
 * Purpose:
 *      Generic function to get/set/delete the port's ingress vlan tag actions
 *
 * Parameters:
 *      unit    - (IN) BCM unit.
 *      port    - (IN) Port number.
 *      cmd     - (IN) One of the values from ENUM bcm_th3_cmd_t.
 *      action  - (IN/OUT) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 *      Based on the value of 'cmd', the parameter 'action' changes its behavior
 *      .It is dont care if cmd is BCM_TH3_DEL. If BCM_TH3_GET then 'action' is
 *      OUT direction, else if BCM_TH3_SET then it is IN direction.
 */
STATIC int
_bcm_th3_ing_vlan_tag_action_command (
    int unit, bcm_port_t port, uint8 cmd, bcm_vlan_action_set_t *action) {

    int rv = BCM_E_UNAVAIL;
    bcm_port_t  local_port;

    if (soc_feature(unit, soc_feature_vlan_action)) {

        /* Validate and resolve the given gport parameter */
        BCM_IF_ERROR_RETURN(
            _bcm_th3_vlan_control_port_resolve(unit, port, FALSE, &local_port));

        /* Lock the PORT TABLE as the operation on
         * VLAN table and Port table has to be atomic
         */
        soc_mem_lock(unit, PORT_TABm);
        /* Based on the type of API, call appropriate internal functions */
        if (BCM_TH3_GET == cmd) {
            rv =
                _bcm_th3_vlan_port_default_action_get(unit, local_port, action);
        } else if (BCM_TH3_SET == cmd) {
            rv =
                _bcm_th3_vlan_port_default_action_set(unit, local_port, action);
        } else  if (BCM_TH3_DEL == cmd) {
            rv = _bcm_th3_vlan_port_default_action_delete(unit, local_port);
        }
        /* Unlock the PORT Table */
        soc_mem_unlock(unit, PORT_TABm);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_th3_egr_vlan_tag_action_command
 *
 * Purpose:
 *      Generic function to get/set/delete the port's egress vlan tag actions
 *
 * Parameters:
 *      unit    - (IN) BCM unit.
 *      port    - (IN) Port number.
 *      cmd     - (IN) One of the values from ENUM bcm_th3_cmd_t.
 *      action  - (IN/OUT) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 *      Based on the value of 'cmd', the parameter 'action' changes its behavior
 *      .It is dont care if cmd is BCM_TH3_DEL. If BCM_TH3_GET then 'action' is
 *      OUT direction, else if BCM_TH3_SET then it is IN direction.
 */
STATIC int
_bcm_th3_egr_vlan_tag_action_command (
    int unit, bcm_port_t port, uint8 cmd, bcm_vlan_action_set_t *action) {

    int rv = BCM_E_UNAVAIL;
    bcm_port_t  local_port;

    if (soc_feature(unit, soc_feature_vlan_action)) {

        /* Validate and resolve the given gport parameter */
        BCM_IF_ERROR_RETURN(
            _bcm_th3_vlan_control_port_resolve(unit, port, FALSE, &local_port));

        /* Based on the type of API, call appropriate internal functions */
        if (BCM_TH3_GET == cmd) {
            rv = _bcm_th3_vlan_port_egress_default_action_get(unit,
                                                              local_port,
                                                              action);
        } else if (BCM_TH3_SET == cmd) {
            rv = _bcm_th3_vlan_port_egress_default_action_set(unit,
                                                              local_port,
                                                              action);
        } else  if (BCM_TH3_DEL == cmd) {
            rv =
              _bcm_th3_vlan_port_egress_default_action_delete(unit, local_port);
            if (BCM_SUCCESS(rv)) {
                _bcm_th3_egr_vlan_action_profile_entry_increment(
                                    unit, BCM_TH3_EGR_ACTION_PROFILE_DEFAULT);
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_vlan_init
 *
 * Purpose:
 *      Initialize the VLAN module.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_init(int unit)
{
    bcm_vlan_info_t *vi = &vlan_info[unit];
    int rv = BCM_E_NONE;
#ifdef BCM_WARM_BOOT_SUPPORT
    soc_scache_handle_t scache_handle;
    uint8 *vlan_scache_ptr;
    uint32 vlan_scache_size;
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Set the auto stack as 0 as stacking not supported on TH3 */
    vi->vlan_auto_stack = 0;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return(_bcm_th3_vlan_reinit(unit));
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        bcm_vlan_data_t vd;
        bcm_pbmp_t      temp_pbmp;

        /*
         * Initialize hardware tables
         */
        BCM_PBMP_CLEAR(temp_pbmp);
        BCM_PBMP_ASSIGN(temp_pbmp, PBMP_ALL(unit));

        vd.vlan_tag = BCM_VLAN_DEFAULT;
        BCM_PBMP_ASSIGN(vd.port_bitmap, temp_pbmp);
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_LB(unit));
        BCM_PBMP_ASSIGN(vd.ut_port_bitmap, temp_pbmp);
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_CMIC(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_LB(unit));

        /*
         * A compile-time application policy may prefer to not add
         * Ethernet or CPU ports to the default VLAN.
         */
#ifdef  BCM_VLAN_NO_DEFAULT_ETHER
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_E_ALL(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_E_ALL(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_ETHER */
#ifdef  BCM_VLAN_NO_DEFAULT_CPU
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_CMIC(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_CPU */

#if defined(BCM_RCPU_SUPPORT)
        if (SOC_IS_RCPU_ONLY(unit) && vlan_info[unit].init) {
            bcm_vlan_t idx;
            soc_rcpu_cfg_t cfg;

            if (SOC_E_NONE != soc_cm_get_rcpu_cfg(unit, &cfg)) {
                cfg.vlan = BCM_VLAN_INVALID;
            }

            /* Destroy previous VLANs */
            for (idx = BCM_VLAN_MIN; idx < BCM_VLAN_COUNT; idx++) {
                if ((idx != vlan_info[unit].defl) &&
                    (idx != cfg.vlan) &&
                    (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, idx))) {
                    BCM_IF_ERROR_RETURN
                        (mbcm_driver[unit]->mbcm_vlan_destroy(unit, idx));
                }
            }
        }
#endif  /* BCM_RCPU_SUPPORT */

#ifdef BCM_MCAST_FLOOD_DEFAULT
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_flood_default_set(unit, BCM_MCAST_FLOOD_DEFAULT));
#else
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_flood_default_set(unit,
                                             BCM_VLAN_MCAST_FLOOD_UNKNOWN));
#endif

        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_vlan_init(unit, &vd));

        /*
         * Initialize software structures
         */
        vi->defl = BCM_VLAN_DEFAULT;

#ifdef BCM_WARM_BOOT_SUPPORT
        /* Warm boot level 2 cache size */
        /* Type for 'flood_mode' is an enum, which values fit in uint8 */
        vlan_scache_size = sizeof(vi->defl);/* Default VLAN */
        vlan_scache_size += sizeof(uint8);  /* Flood mode */

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VLAN, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                     vlan_scache_size,
                                     &vlan_scache_ptr,
                                     BCM_WB_DEFAULT_VERSION, NULL);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
        rv = BCM_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */

        /* In case bcm_vlan_init is called more than once */
        BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->bmp));
        _BCM_VBMP_INSERT(vi->bmp, vd.vlan_tag);

        BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->pre_cfg_bmp));

        /* Free vlan translation arrays if any as not supported. */
        if (NULL != vi->egr_trans) {
            sal_free(vi->egr_trans);
            vi->egr_trans = NULL;
        }
        if (NULL != vi->ing_trans) {
            sal_free(vi->ing_trans);
            vi->ing_trans = NULL;
        }
        vi->count = 1;
    }

    /*
     * Initialize Cross Connect Internal Database
     */
    BCM_IF_ERROR_RETURN(_bcm_th3_vlan_xconnect_db_init(unit));

    vi->init = TRUE;

#if defined(BCM_RCPU_SUPPORT)
    if (SOC_IS_RCPU_ONLY(unit)) {
        soc_rcpu_cfg_t cfg;
        if (SOC_E_NONE == soc_cm_get_rcpu_cfg(unit, &cfg) &&
            BCM_VLAN_DEFAULT != cfg.vlan) {
            /*
             * RCPU vlan should have been created in remote device, sync
             * software status.
             */
            _BCM_VBMP_INSERT(vlan_info[unit].bmp, cfg.vlan);
            vlan_info[unit].count++;
        }
    }
#endif  /* BCM_RCPU_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_vlan_detach
 *
 * Purpose:
 *      De-initialize the VLAN module.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_detach(int unit)
{
    bcm_vlan_info_t *vi = &vlan_info[unit];
    int rv = BCM_E_NONE;

    BCM_LOCK(unit);

    _bcm_vbmp_destroy(&vi->bmp);
    rv = _bcm_vbmp_destroy(&vi->pre_cfg_bmp);
    if (BCM_FAILURE(rv)) {
        BCM_UNLOCK(unit);
        return rv;
    }

    if (vi->egr_trans != NULL) {
        sal_free(vi->egr_trans);
        vi->egr_trans = NULL;
    }
    if (vi->ing_trans != NULL) {
        sal_free(vi->ing_trans);
        vi->ing_trans = NULL;
    }

    vi->init = 0;

#if defined(BCM_FIREBOLT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        rv = _bcm_xgs3_vlan_profile_detach(unit);
        if (BCM_FAILURE(rv)) {
            BCM_UNLOCK(unit);
            return rv;
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

     rv= _bcm_th3_vlan_xconnect_db_destroy(unit);

    BCM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_add
 *
 * Purpose:
 *      Add ports to the specified vlan.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to add port to as a member.
 *      pbmp - port bitmap for members of VLAN
 *      ubmp - untagged members of VLAN
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp)
{
    int                 rv = BCM_E_NONE;
    bcm_pbmp_t          pbm, ubm;

    CHECK_INIT_AND_VLAN_ID(unit, vid);

    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_CLEAR(ubm);

    /* we need to check if pbmp and ubmp are valid as well */
    BCM_PBMP_ASSIGN(pbm,PBMP_ALL(unit));
    BCM_PBMP_ASSIGN(ubm,PBMP_ALL(unit));
    BCM_PBMP_AND(pbm,pbmp);
    BCM_PBMP_AND(ubm,ubmp);

    if(!(BCM_PBMP_EQ(pbm, pbmp) && BCM_PBMP_EQ(ubm, ubmp))) {
        return BCM_E_PARAM;
    }

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: port add: vid %d. pbm 0x%x. upbm 0x%x\n"),
                 unit, vid, SOC_PBMP_WORD_GET(pbmp, 0),
                 SOC_PBMP_WORD_GET(ubmp, 0)));

    BCM_LOCK(unit);
    rv = _bcm_th3_vlan_port_add(unit, vid, pbmp, ubmp, pbmp, 0);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_remove
 *
 * Purpose:
 *      Remove ports from a specified vlan.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to remove port(s) from.
 *      pbmp - port bitmap for ports to remove.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    int rv = BCM_E_NONE;

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: port remove: vid %d. pbm 0x%x.\n"),
                 unit, vid, SOC_PBMP_WORD_GET(pbmp, 0)));

    CHECK_INIT_AND_VLAN_ID(unit, vid);
    /* As we are passing only one pbmp parameter to API bcm_vlan_port_remove,
     * the expected result is to remove same pbmp from PORT_BITMAPf, UT_BITMAPf
     * and ING_PORT_BITMAPf. Hence, same pbmp is passed as argument for ubmp
     * and ing_pbmp
     */
    BCM_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_vlan_port_remove(
                                unit, vid, pbmp, pbmp, pbmp);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_control_vlan_selective_get
 *
 * Purpose:
 *      Get per VLAN configuration.
 *
 * Parameters:
 *      unit     - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid      - (IN) VLAN to get the setting for.
 *      valid_fields - (IN) Valid fields for VLAN control
 *                          structure, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *      control  - (OUT) VLAN control structure
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_control_vlan_selective_get(int unit, bcm_vlan_t vlan,
                    uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int rv = BCM_E_UNAVAIL;

    CHECK_INIT_AND_VLAN_ID(unit, vlan);
    if (NULL == control) {
        return (BCM_E_PARAM);
    }

    if (soc_feature(unit, soc_feature_vlan_ctrl)) {

        /* Check incoming fields validity for TH3 specific support */
        if (valid_fields & ~BCM_TH3_VLAN_CONTROL_VLAN_ALL_VALID_MASK) {
            return (BCM_E_PARAM);
        }

        BCM_LOCK(unit);
        rv = _bcm_xgs3_vlan_control_vlan_get(unit, vlan, valid_fields, control);
        BCM_UNLOCK(unit);
        /*
         * Clean the flag BCM_VLAN_L3_URPF_DEFAULT_ROUTE_CHECK_DISABLE from the
         * control->flags as URPF is not supported by TH3, but underlying layer
         * would check for the URPF support and if it does not find it, the
         * default outcome is chosen i.e. l3_urpf_default_route_check_disable.
         * However, for TH3 we do not want to provide this default flag to Appl.
         */
        control->flags &= ~BCM_VLAN_L3_URPF_DEFAULT_ROUTE_CHECK_DISABLE;

        /* Check for TH3 specific output validity */
        if (control->flags & ~BCM_TH3_VLAN_CONTROL_VLAN_SUPPORTED_CTRL_FLAGS) {
            return BCM_E_CONFIG;
        }

        if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK) {
            if (control->forwarding_mode != bcmVlanForwardBridging &&
                control->forwarding_mode != bcmVlanForwardSingleCrossConnect) {

                return BCM_E_CONFIG;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_vlan_control_vlan_selective_set
 *
 * Purpose:
 *      Set per VLAN configuration.
 *
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid     - (IN) VLAN to set the configuration for.
 *      valid_fields - (IN) Valid fields for VLAN control structure,
 *                           BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *      control - (IN) VLAN control structure
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_control_vlan_selective_set(int unit, bcm_vlan_t vlan,
                        uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int rv = BCM_E_UNAVAIL;

    CHECK_INIT_AND_VLAN_ID(unit, vlan);

    /* Perform incoming parameter validity for TH3 device chip */
    if (NULL == control) {
        return (BCM_E_PARAM);
    }

    if (valid_fields & ~BCM_TH3_VLAN_CONTROL_VLAN_ALL_VALID_MASK) {
        return (BCM_E_PARAM);
    }
    if (control->flags & ~BCM_TH3_VLAN_CONTROL_VLAN_SUPPORTED_CTRL_FLAGS) {
        return (BCM_E_PARAM);
    }
    if ((0 != control->learn_flags) || (0 != control->sr_flags)) {
        return (BCM_E_PARAM);
    }
    if ((control->vrf > SOC_VRF_MAX(unit))) {
        return (BCM_E_PARAM);
    }
    if (bcmVlanUrpfDisable != control->urpf_mode) {
        return (BCM_E_PARAM);
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK) {
        if ((control->forwarding_mode != bcmVlanForwardBridging) &&
            (control->forwarding_mode != bcmVlanForwardSingleCrossConnect)) {
            return (BCM_E_PARAM);
        }
    }

    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        BCM_LOCK(unit);
        rv = _bcm_xgs3_vlan_control_vlan_set(unit, vlan, valid_fields, control);
        BCM_UNLOCK(unit);
    }

    return rv;
}

/*
 * Function:
 *      bcm_vlan_control_vlan_get
 *
 * Purpose:
 *      Get all supported per VLAN configurations.
 *
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid     - (IN) VLAN to get the setting for.
 *      control - (OUT) VLAN control structure
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 *
 * Note:
 *      No checks are performed here as the callee function will do all checks
 */
int
bcm_tomahawk3_vlan_control_vlan_get(int unit, bcm_vlan_t vid,
                                    bcm_vlan_control_vlan_t *control)
{
    return(
        bcm_tomahawk3_vlan_control_vlan_selective_get(unit, vid,
            (uint32)BCM_TH3_VLAN_CONTROL_VLAN_ALL_VALID_MASK, control));
}

/*
 * Function:
 *      bcm_vlan_control_vlan_set
 *
 * Purpose:
 *      Set per VLAN configuration.
 *
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid     - (IN) VLAN to get the flood setting for.
 *      control - (IN) VLAN control structure
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 *
 * Note:
 *      No checks are performed here as the callee function will do all checks
 */
int
bcm_tomahawk3_vlan_control_vlan_set(int unit, bcm_vlan_t vid,
                              /* coverity[pass_by_value] */
                              bcm_vlan_control_vlan_t control)
{
    return (
        bcm_tomahawk3_vlan_control_vlan_selective_set(unit, vid,
            (uint32)BCM_TH3_VLAN_CONTROL_VLAN_ALL_VALID_MASK, &control));
}

/*
 *   Function
 *      bcm_vlan_vector_flags_set
 *   Purpose
 *      Set a one or more VLAN control vlan flags on a vlan_vector on this unit
 *
 *   Parameters
 *      (in) int unit = unit number
 *      (in) bcm_vlan_vector_t vlan_vector = Vlan vector for values to be set
 *      (in) uint32 flags_mask
 *      (in) uint32 flags_value
 *
 *   Returns
 *      bcm_error_t = BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *
 *   Note
 */
int
bcm_tomahawk3_vlan_vector_flags_set(int unit,
                              bcm_vlan_vector_t vlan_vector,
                              uint32 flags_mask,
                              uint32 flags_value)
{
    int rv;

    bcm_vlan_t              vid;
    bcm_vlan_control_vlan_t control;

    /* non-supported vector fields */
    if (flags_mask & ~BCM_TH3_VLAN_CONTROL_VLAN_SUPPORTED_CTRL_FLAGS) {
        return BCM_E_PARAM;
    }

    /* Optimistically assume success from here */
    rv = BCM_E_NONE;

    for (vid = BCM_VLAN_MIN + 1; vid < BCM_VLAN_MAX; vid++) {

        if (BCM_VLAN_VEC_GET(vlan_vector, vid)) {

            bcm_vlan_control_vlan_t_init(&control);

            rv = bcm_tomahawk3_vlan_control_vlan_selective_get(unit, vid,
                    (uint32)BCM_TH3_VLAN_CONTROL_VLAN_ALL_VALID_MASK, &control);

            if (BCM_SUCCESS(rv)) {
                control.flags =
                    (~flags_mask & control.flags) | (flags_mask & flags_value);

                rv = bcm_tomahawk3_vlan_control_vlan_selective_set(
                            unit,
                            vid,
                            (uint32)BCM_TH3_VLAN_CONTROL_VLAN_ALL_VALID_MASK,
                            &control);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }
        }
    }
    return rv;
}

/*
 * Function:
 *     bcm_vlan_control_set
 *
 * Purpose:
 *     Set miscellaneous VLAN-specific chip options
 *
 * Parameters:
 *     unit - StrataSwitch PCI device unit number (driver internal).
 *     type - A value from bcm_vlan_control_t enumeration list
 *     arg  - state whose meaning is dependent on 'type'
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_INTERNAL - Chip access failure.
 *     BCM_E_UNAVAIL  - type not supported on unit
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_control_set(int unit, bcm_vlan_control_t type, int arg)
{
    uint32  data;
    uint32  vlan_ctrl;

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "VLAN %d: control set: type %d, arg %d\n"),
                 unit, type, arg));

    if (SOC_IS_XGS3_FABRIC(unit)) {
        return BCM_E_UNAVAIL;
    }

    switch (type) {
        case bcmVlanDropUnknown:
            return bcm_esw_switch_control_port_set(unit,
                                                   CMIC_PORT(unit),
                                                   bcmSwitchUnknownVlanToCpu,
                                                   !arg);
        case bcmVlanShared:
            arg = (arg ? 1 : 0);
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            data = vlan_ctrl;
            soc_reg_field_set(unit, VLAN_CTRLr, &vlan_ctrl,
                              USE_LEARN_VIDf, arg);
            if (data != vlan_ctrl) {
                SOC_IF_ERROR_RETURN(WRITE_VLAN_CTRLr(unit, vlan_ctrl));
                SOC_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit,
                        EGR_CONFIGr, CMIC_PORT(unit), USE_LEARN_VIDf, arg));
            }
            break;

        case bcmVlanSharedID:
            VLAN_CHK_ID(unit, arg);
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            data = vlan_ctrl;
            soc_reg_field_set(unit, VLAN_CTRLr, &vlan_ctrl,
                              LEARN_VIDf, arg);
            if (data != vlan_ctrl) {
                SOC_IF_ERROR_RETURN(WRITE_VLAN_CTRLr(unit, vlan_ctrl));
                SOC_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit,
                        EGR_CONFIGr, CMIC_PORT(unit), LEARN_VIDf, arg));
            }
            break;

        default:
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_vlan_control_get
 *
 * Purpose:
 *     Get miscellaneous VLAN-specific chip options
 *
 * Parameters:
 *     unit - StrataSwitch PCI device unit number (driver internal).
 *     type - A value from bcm_vlan_control_t enumeration list
 *     arg  - (OUT) state whose meaning is dependent on 'type'
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_PARAM    - arg points to NULL
 *     BCM_E_INTERNAL - Chip access failure.
 *     BCM_E_UNAVAIL  - type not supported on unit
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg)
{
    int     rv = BCM_E_NONE;
    uint32  vlan_ctrl;

    if (NULL == arg) {
        return BCM_E_PARAM;
    }

    if (SOC_IS_XGS3_FABRIC(unit)) {
        return BCM_E_UNAVAIL;
    }

    switch (type) {
        case bcmVlanDropUnknown:
            rv = bcm_esw_switch_control_port_get(unit,
                                                 CMIC_PORT(unit),
                                                 bcmSwitchUnknownVlanToCpu,
                                                 arg);
            if (BCM_SUCCESS(rv)) {
                *arg = !(*arg);
            }
            break;

        case bcmVlanShared:
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            *arg = soc_reg_field_get(unit, VLAN_CTRLr,
                                     vlan_ctrl, USE_LEARN_VIDf);
            break;

        case bcmVlanSharedID:
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            *arg = soc_reg_field_get(unit, VLAN_CTRLr,
                                     vlan_ctrl, LEARN_VIDf);
            break;

        default:
            rv = BCM_E_UNAVAIL;
            break;
    }

    return rv;
}

/*
 * Function:
 *     bcm_tomahawk3_vlan_control_port_set
 *
 * Purpose:
 *     BCM dispatched API to set port based VLAN configuration.
 *
 * Parameters:
 *     unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *     port - (IN) port/gport on which the VLAN configuration has to be applied
 *     type - (IN) value from bcm_vlan_control_port_t enumeration list
 *     arg - (IN) value to be set for the given type
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_PORT - If the given port/gport is not supported for TH3 VLAN module
 *     BCM_E_PARAM - Given arg or type is not supported for TH3
 *     BCM_E_XXX - Other failures
 *
 * Note:
 *     If the given port is -1 then apply the setting to all the local ports.
 */
int
bcm_tomahawk3_vlan_control_port_set(int unit, bcm_port_t port,
                              bcm_vlan_control_port_t type, int arg)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t temp_pbmp;

    if (-1 == port) {
        BCM_PBMP_CLEAR(temp_pbmp);
        BCM_PBMP_ASSIGN(temp_pbmp, PBMP_E_ALL(unit));

        /* Iterate through all local ports and apply the VLAN configuration */
        PBMP_ITER(temp_pbmp, port) {
            BCM_IF_ERROR_RETURN(
                _bcm_th3_vlan_control_port_config_command(
                                unit, port, type, BCM_TH3_SET, &arg));
        }
    } else {
        /* Validate and resolve the given port parameter */
        BCM_IF_ERROR_RETURN(
                _bcm_th3_vlan_control_port_resolve(unit, port, FALSE, &port));
        rv =  _bcm_th3_vlan_control_port_config_command(
                        unit, port, type, BCM_TH3_SET, &arg);
    }
    return rv;
}


/*
 * Function:
 *     bcm_tomahawk3_vlan_control_port_get
 *
 * Purpose:
 *     BCM dispatched API to get port based VLAN configuration.
 *
 * Parameters:
 *     unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *     port - (IN) port/gport for which the VLAN config has to be retrieved
 *     type - (IN) value from bcm_vlan_control_port_t enumeration list
 *     arg - (OUT) retrieved config value is to be stored in here
 *
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_PORT - If the given port/gport is not supported for TH3 VLAN module
 *     BCM_E_PARAM - Given arg is NULL or type is not supported for TH3
 *     BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_control_port_get(int unit, bcm_port_t port,
                              bcm_vlan_control_port_t type, int *arg)
{
    /* Validate and resolve the given port parameter */
    BCM_IF_ERROR_RETURN(
                _bcm_th3_vlan_control_port_resolve(unit, port, FALSE, &port));

    return (_bcm_th3_vlan_control_port_config_command(
                        unit, port, type, BCM_TH3_GET, arg));
}

/*
 * Function:
 *      bcm_vlan_gport_add
 *
 * Purpose:
 *      Add a Gport to the specified vlan.
 *
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vlan - (IN) VLAN ID to retrieve the gport from.
 *      gport - (IN) Gport to be deleted from the VLAN.
 *      flags - (IN) VLAN_PORT flags qualifying the GPORT.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INIT - VLAN module is not initialized.
 *      BCM_E_PARAM - Given parameter validation failed.
 *      BCM_E_PORT - Given GPORT not supported.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 *      BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_gport_add(
    int unit, bcm_vlan_t vlan, bcm_gport_t gport, int flags)
{
    int         rv;
    bcm_pbmp_t  pbmp;
    bcm_pbmp_t  ubmp;
    bcm_pbmp_t  ing_pbmp;
    bcm_port_t  local_port;

    /* Validate given VLAN ID */
    CHECK_INIT_AND_VLAN_ID(unit, vlan);

    /* Validate and resolve the given gport parameter */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_vlan_control_port_resolve(unit, gport, TRUE, &local_port));

    /* Validate the given flags */
    if (flags & ~BCM_TH3_VLAN_GPORT_ADD_SUPPORTED_FLAGS) {
        return BCM_E_PARAM;
    }

    /* If EGRESS_L3_ONLY is set then check if it is supported */
    if ((flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY) &&
        !soc_feature(unit, soc_feature_vlan_egress_membership_l3_only)) {
        return BCM_E_UNAVAIL;
    }

    /* Both EGRESS_ONLY and EGREE_L3_ONLY cannot be set */
    if ((flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY) &&
        (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        return BCM_E_PARAM;
    }

    /* With INGRESS flag EGRESS_(L3)_ONLY or UNTAGGED flags cannot be set */
    if (flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY) {
        if (flags & BCM_VLAN_GPORT_ADD_UNTAGGED) {
            return BCM_E_PARAM;
        }

        if ((flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY) ||
            (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
            return BCM_E_PARAM;
        }
    }

    /* Based on the VLAN_GPORT_ADD flags form the port bitmaps appropriately */
    if (flags &
        (BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        BCM_PBMP_CLEAR(ing_pbmp);
    } else {
        BCM_PBMP_PORT_SET(ing_pbmp, local_port);
    }
    if (flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY) {
        BCM_PBMP_CLEAR(pbmp);
    } else {
        BCM_PBMP_PORT_SET(pbmp, local_port);
    }
    if (flags & BCM_VLAN_GPORT_ADD_UNTAGGED) {
        BCM_PBMP_PORT_SET(ubmp, local_port);
    } else {
        BCM_PBMP_CLEAR(ubmp);
    }

    /* Call low level routine to add the local physical port to the VLAN */
    BCM_LOCK(unit);
    rv = _bcm_th3_vlan_port_add(unit, vlan, pbmp, ubmp, ing_pbmp, flags);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_gport_delete
 *
 * Purpose:
 *      Delete a given Gport from the specified vlan.
 *
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vlan - (IN) VLAN ID to retrieve the gport from.
 *      gport - (IN) Gport to be deleted from the VLAN
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INIT - VLAN module is not initialized.
 *      BCM_E_PARAM - Given parameter validation failed.
 *      BCM_E_PORT - Given GPORT not supported.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 *      BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_gport_delete(int unit, bcm_vlan_t vlan, bcm_gport_t gport)
{
    int         rv;
    bcm_pbmp_t  pbmp;
    bcm_port_t  local_port;

    /* Validate given VLAN ID */
    CHECK_INIT_AND_VLAN_ID(unit, vlan);

    /* Validate and resolve the given gport parameter */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_vlan_control_port_resolve(unit, gport, TRUE, &local_port));

    /* Create a port bitmap from the retrieved local physical port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, local_port);

    /* Call low level routine to remove the retrieved local phys port */
    BCM_LOCK(unit);
    /* As we are passing only one pbmp parameter to API bcm_vlan_port_remove,
     * the expected result is to remove same pbmp from PORT_BITMAPf, UT_BITMAPf
     * and ING_PORT_BITMAPf. Hence, same pbmp is passed as argument for ubmp
     * and ing_pbmp
     */
    rv = mbcm_driver[unit]->mbcm_vlan_port_remove(
                                unit, vlan, pbmp, pbmp, pbmp);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_gport_extended_delete
 *
 * Purpose:
 *      Delete the Gport qualified by the given VLAN_PORT
 *      flags from the specified vlan.
 *
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vlan - (IN) VLAN ID to retrieve the gport from.
 *      gport - (IN) Gport to be deleted from the VLAN
 *      flags - (IN) VLAN_PORT flags qualifying the GPORT.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INIT - VLAN module is not initialized.
 *      BCM_E_PARAM - Given parameter validation failed.
 *      BCM_E_PORT - Given GPORT not supported.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 *      BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_gport_extended_delete(
    int unit, bcm_vlan_t vlan, bcm_gport_t gport, int flags)
{
    int         rv;
    bcm_pbmp_t  pbmp;
    bcm_pbmp_t  ubmp;
    bcm_pbmp_t  ing_pbmp;
    bcm_port_t  local_port;

    /* Validate given VLAN ID */
    CHECK_INIT_AND_VLAN_ID(unit, vlan);

    /* Validate and resolve the given gport parameter */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_vlan_control_port_resolve(unit, gport, TRUE, &local_port));

    /* Validate the given flags */
    if (flags & ~BCM_TH3_VLAN_GPORT_ADD_SUPPORTED_FLAGS) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY) {
        if (!soc_feature(unit, soc_feature_vlan_egress_membership_l3_only)) {
            return BCM_E_PARAM;
        }
    }

    if (flags &
        (BCM_VLAN_PORT_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        BCM_PBMP_CLEAR(ing_pbmp);
    } else {
        BCM_PBMP_PORT_SET(ing_pbmp, local_port);
    }
    if (flags & BCM_VLAN_PORT_INGRESS_ONLY) {
        BCM_PBMP_CLEAR(pbmp);
    } else {
        BCM_PBMP_PORT_SET(pbmp, local_port);
    }
    if (flags & BCM_VLAN_PORT_UNTAGGED) {
        BCM_PBMP_PORT_SET(ubmp, local_port);
    } else {
        BCM_PBMP_CLEAR(ubmp);
    }

    /* Call low level routine to remove the retrieved local phys port */
    BCM_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_vlan_port_remove(
                                unit, vlan, pbmp, ubmp, ing_pbmp);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_gport_delete_all
 *
 * Purpose:
 *      Delete all Gports from the specified vlan.
 *
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vlan - (IN) VLAN ID to retrieve the gport from.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INIT - VLAN module is not initialized.
 *      BCM_E_PARAM - Given parameter validation failed.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 *      BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_gport_delete_all(int unit, bcm_vlan_t vlan)
{
    int         rv;
    bcm_pbmp_t  pbmp;
    bcm_pbmp_t  ubmp;
    bcm_pbmp_t  ing_pbmp;

    /* Validate given VLAN ID */
    CHECK_INIT_AND_VLAN_ID(unit, vlan);

    /* Retrieve all local physical ports bound to the given VLAN */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_vlan_port_get(unit, vlan, &pbmp, &ubmp, &ing_pbmp));

    LOG_VERBOSE(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                 "Vlan %d: gport rem all: pbm 0x%x, upbm 0x%x.\n"), vlan,
                 SOC_PBMP_WORD_GET(pbmp, 0), SOC_PBMP_WORD_GET(ubmp, 0)));

    /* Call low level routine to remove all local physical ports */
    BCM_LOCK(unit);
    rv = mbcm_driver[unit]->
            mbcm_vlan_port_remove(unit, vlan, pbmp, ubmp, ing_pbmp);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_gport_get
 *
 * Purpose:
 *      Retrieve VLAN_PORT flags for the given Gport on the specified vlan.
 *
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vlan - (IN) VLAN ID to retrieve the gport and its flags from.
 *      port - (IN) Gport for which the flags are to be retrieved.
 *      flags - (OUT) VLAN_PORT flags to be stored here.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INIT - VLAN module is not initialized.
 *      BCM_E_PARAM - Given parameter validation failed.
 *      BCM_E_PORT - Not supported Gport.
 *      BCM_E_NOT_FOUND - VLAN ID not in use or invalid flags on the VLAN.
 *      BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_gport_get(
    int unit, bcm_vlan_t vlan, bcm_gport_t gport, int *flags)
{
    uint32      local_flags = 0;
    bcm_pbmp_t  pbmp;
    bcm_pbmp_t  ubmp;
    bcm_pbmp_t  ing_pbmp;
    bcm_port_t  local_port;

    /* Validate given VLAN ID */
    CHECK_INIT_AND_VLAN_ID(unit, vlan);

    /* Validate other input parameters */
    if (NULL == flags) {
        return BCM_E_PARAM;
    }

    /* Validate and resolve the given gport parameter */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_vlan_control_port_resolve(unit, gport, TRUE, &local_port));

    /* Retrieve all local physical ports bound to the given VLAN */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_vlan_port_get(unit, vlan, &pbmp, &ubmp, &ing_pbmp));

    /* Deduce the VLAN_PORT flags */
    if (!BCM_PBMP_MEMBER(pbmp, local_port)) {
        local_flags |= BCM_VLAN_PORT_INGRESS_ONLY;
    }
    if (!BCM_PBMP_MEMBER(ing_pbmp, local_port)) {
        local_flags |= BCM_VLAN_PORT_EGRESS_ONLY;
    }
    if (local_flags ==
        (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
        return BCM_E_NOT_FOUND;
    }
    if (BCM_PBMP_MEMBER(ubmp, local_port)) {
        local_flags |= BCM_VLAN_PORT_UNTAGGED;
    }
    if ((local_flags & BCM_VLAN_PORT_EGRESS_ONLY) &&
        soc_feature(unit, soc_feature_vlan_egress_membership_l3_only)) {

        BCM_PBMP_CLEAR(pbmp);
        BCM_IF_ERROR_RETURN(
            _bcm_xgs3_vlan_table_port_get(
                unit, vlan, &pbmp, NULL, NULL, VLAN_2_TABm));

        if (!BCM_PBMP_MEMBER(pbmp, local_port)) {
            local_flags &= ~BCM_VLAN_PORT_EGRESS_ONLY;
            local_flags |= BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
        }
    }

    /* Store the scrutinized Flags value */
    *flags = local_flags;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_gport_get_all
 *
 * Purpose:
 *      Retrieve VLAN_PORT flags for all Gports from the specified vlan.
 *
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vlan - (IN) VLAN ID to retrieve all gports from.
 *      array_max - (IN) Max slize for the provided arrays.
 *      gport_array - (OUT) Array to be stored with the gports.
 *      flags_array - (OUT) Array to be stored with VLAN_PORT flags.
 *      array_size - (OUT) Number of retrieved gports.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INIT - VLAN module is not initialized.
 *      BCM_E_PARAM - Given parameter validation failed.
 *      BCM_E_NOT_FOUND - VLAN ID not in use or no gports on the VLAN.
 *      BCM_E_XXX - Other failures
 *
 * Note:
 *     1)  Parameter 'array_max' should be passed with positive value.
 *     2)  Paremeter 'gport_array' and 'flags_array' are optional. If not
 *          given then API would just return number of gports atatched to
 *          the given VLAN. If either of the parameter is given then that
 *          information is filled in by the API.
 */
int
bcm_tomahawk3_vlan_gport_get_all(int unit, bcm_vlan_t vlan,
    int array_max, bcm_gport_t *gport_array, int *flags_array, int* array_size)
{
    int                 rv = BCM_E_NONE;
    int                 local_modid;
    int                 port_cnt = 0;
    uint32              local_flags = 0;
    bcm_pbmp_t          bmp;
    bcm_pbmp_t          pbmp;
    bcm_pbmp_t          ubmp;
    bcm_pbmp_t          ing_pbmp;
    bcm_port_t          port;
    bcm_gport_t         gport;
    _bcm_gport_dest_t   gport_dest;

    /* Validate given VLAN ID */
    CHECK_INIT_AND_VLAN_ID(unit, vlan);

    /* Validate other incoming parameters */
    if ((NULL == array_size) || (array_max <= 0)) {
        return BCM_E_PARAM;
    }

    /* Retrieve the local mod id */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_modid));

    /* Retrieve the local physical ports bound to the given VLAN */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_vlan_port_get(unit, vlan, &pbmp, &ubmp, &ing_pbmp));

    /* Prepare a superset of bitmaps to iterate over */
    BCM_PBMP_ASSIGN(bmp, pbmp);
    BCM_PBMP_OR(bmp, ubmp);
    BCM_PBMP_OR(bmp, ing_pbmp);

    /* Iterate through the list of ports from superset */
    BCM_PBMP_ITER(bmp, port) {

        /* Check for boundary conditions */
        if ((port_cnt == array_max) && gport_array) {
            break;
        }

        /* Formulate the MODPORT gport from the chosen local port */
        gport_dest.port = port;
        gport_dest.modid = local_modid;
        gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_construct(unit, &gport_dest, &gport));

        /* Deduce the VLAN_PORT flags */
        local_flags = 0;
        if (!BCM_PBMP_MEMBER(pbmp, port)) {
            local_flags |= BCM_VLAN_PORT_INGRESS_ONLY;
        }
        if (!BCM_PBMP_MEMBER(ing_pbmp, port)) {
            local_flags |= BCM_VLAN_PORT_EGRESS_ONLY;
        }
        if (local_flags ==
            (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
            continue;
        }

        /* Fill in the flags for the chosen port, if the flags_array given */
        if (flags_array) {
            if (BCM_PBMP_MEMBER(ubmp, port)) {
                local_flags |= BCM_VLAN_PORT_UNTAGGED;
            }
            flags_array[port_cnt] = local_flags;
        }

        /* Fill in the gport for the chosen port, if the gport_array given */
        if (gport_array) {
            gport_array[port_cnt] = gport;
        }

        port_cnt++;
    }

    /* If all attempts found nothing return not found */
    if (port_cnt) {
        *array_size = port_cnt;
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_default_action_set
 *
 * Purpose:
 *      Set the port's default vlan tag actions
 *
 * Parameters:
 *      unit    - (IN) BCM unit.
 *      port    - (IN) Port number (Gport or Local).
 *      action  - (IN) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_port_default_action_set(
    int unit, bcm_port_t port, bcm_vlan_action_set_t *action)
{
    return(
        _bcm_th3_ing_vlan_tag_action_command(unit, port, BCM_TH3_SET, action));
}

/*
 * Function:
 *      bcm_vlan_port_default_action_get
 *
 * Purpose:
 *      Get the port's default vlan tag actions
 *
 * Parameters:
 *      unit    - (IN) BCM unit.
 *      port    - (IN) Port number.
 *      action  - (OUT) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_port_default_action_get(
    int unit, bcm_port_t port, bcm_vlan_action_set_t *action)
{
    return(
        _bcm_th3_ing_vlan_tag_action_command(unit, port, BCM_TH3_GET, action));
}

/*
 * Function:
 *      bcm_vlan_port_default_action_delete
 *
 * Purpose:
 *      Delete the port's default vlan tag actions
 *
 * Parameters:
 *      unit    - (IN) BCM unit.
 *      port    - (IN) Port number.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_port_default_action_delete(int unit, bcm_port_t port)
{
    return(_bcm_th3_ing_vlan_tag_action_command(unit, port, BCM_TH3_DEL, NULL));
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_set
 *
 * Purpose:
 *      Set the egress port's default vlan tag actions
 *
 * Parameters:
 *      unit    - (IN) BCM unit.
 *      port    - (IN) Port number (Gport or Local).
 *      action  - (IN) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_port_egress_default_action_set(
    int unit, bcm_port_t port, bcm_vlan_action_set_t *action)
{
    return(
        _bcm_th3_egr_vlan_tag_action_command(unit, port, BCM_TH3_SET, action));
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_get
 *
 * Purpose:
 *      Get the port's egress default vlan tag actions
 *
 * Parameters:
 *      unit    - (IN) BCM unit.
 *      port    - (IN) Port number.
 *      action  - (OUT) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_port_egress_default_action_get(
    int unit, bcm_port_t port, bcm_vlan_action_set_t *action)
{
    return(
        _bcm_th3_egr_vlan_tag_action_command(unit, port, BCM_TH3_GET, action));
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_delete
 *
 * Purpose:
 *      Delete the port's egress default vlan tag actions
 *
 * Parameters:
 *      unit    - (IN) BCM unit.
 *      port    - (IN) Port number.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_port_egress_default_action_delete(int unit, bcm_port_t port)
{
    return _bcm_th3_egr_vlan_tag_action_command(unit, port, BCM_TH3_DEL, NULL);
}

/*
 * Function:
 *      bcm_vlan_cross_connect_add
 *
 * Purpose:
 *      Add a VLAN cross connect entry
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      outer_vlan - (IN) Outer vlan ID
 *      inner_vlan - (IN) Inner vlan ID
 *      port_1     - (IN) First port in the cross-connect
 *      port_2     - (IN) Second port in the cross-connect
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Vlan IDs are invalid
 *      BCM_E_PORT - Given gports are invalid or not supported
 *      BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_cross_connect_add(int unit, bcm_vlan_t outer_vlan,
        bcm_vlan_t inner_vlan, bcm_gport_t port_1, bcm_gport_t port_2)
{
    int     rv;
    int     dest_val[2];
    uint8   is_trunk_gport[2] = {0, 0};

    /* Validate if the VLAN module has been initialized */
    CHECK_INIT(unit);

    /* Validate the outer and inner VLAN ID values */
    if (BCM_VLAN_INVALID != inner_vlan) {
        return BCM_E_UNAVAIL;
    }

    if ((outer_vlan < BCM_VLAN_DEFAULT) || (outer_vlan > BCM_VLAN_MAX)) {
        return BCM_E_PARAM;
    }

    /* Validate and resolve the given gport parameters */
    rv = _bcm_th3_vlan_control_port_resolve(unit, port_1, TRUE, &dest_val[0]);

    if (BCM_E_NONE != rv) {
        /* _bcm_th3_vlan_control_port_resolve does not check for Trunk
         * GPORT. Check for it explicitly and retrieve the trunk ID
         */
        if (BCM_GPORT_IS_TRUNK(port_1)) {
            is_trunk_gport[0] = 1;
            dest_val[0] = BCM_GPORT_TRUNK_GET(port_1);
        } else {
            return rv;
        }
    }

    rv =  _bcm_th3_vlan_control_port_resolve(unit, port_2, TRUE, &dest_val[1]);
    if (BCM_E_NONE != rv) {
        /* _bcm_th3_vlan_control_port_resolve does not check for Trunk
         * GPORT. Check for it explicitly and retrieve the trunk ID
         */
        if (BCM_GPORT_IS_TRUNK(port_2)) {
            is_trunk_gport[1] = 1;
            dest_val[1] = BCM_GPORT_TRUNK_GET(port_2);
        } else {
            return rv;
        }
    }

    return _bcm_th3_vlan_xconnect_db_entry_add(
                unit, outer_vlan, is_trunk_gport, dest_val);
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete
 *
 * Purpose:
 *      Delete a VLAN cross connect entry
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      outer_vlan - (IN) Outer vlan ID
 *      inner_vlan - (IN) Inner vlan ID
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Vlan IDs are invalid
 *      BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_cross_connect_delete(
        int unit, bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan)
{
    /* Validate if the VLAN module has been initialized */
    CHECK_INIT(unit);

    /* Validate the inner VLAN ID value */
    if (BCM_VLAN_INVALID != inner_vlan) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_th3_vlan_xconnect_db_entry_delete(unit, outer_vlan);
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete_all
 *
 * Purpose:
 *      Delete all VLAN cross connect entries
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_cross_connect_delete_all(int unit)
{
    /* Validate if the VLAN module has been initialized */
    CHECK_INIT(unit);

    return _bcm_th3_vlan_xconnect_db_entry_delete_all(unit);
}

/*
 * Function:
 *      bcm_vlan_cross_connect_traverse
 *
 * Purpose:
 *      Walks through the valid cross connect entries and calls
 *      the user supplied callback function for each entry.
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      cb         - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Call back function not provided
 *      BCM_E_XXX - Other failures
 *
 * Note:
 */
int
bcm_tomahawk3_vlan_cross_connect_traverse(int unit,
                                    bcm_vlan_cross_connect_traverse_cb cb,
                                    void *user_data)
{
    /* Validate if the VLAN module has been initialized */
    CHECK_INIT(unit);

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    return _bcm_th3_vlan_xconnect_traverse(unit, cb, user_data);
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
