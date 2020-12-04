/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls.c
 * Purpose: Manages MPLS functions
 */

#ifdef INCLUDE_L3

#include <sal/core/libc.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/l2u.h>
#include <soc/l3x.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/rate.h>
#include <bcm/ipmc.h>
#include <bcm/mpls.h>
#include <bcm/stack.h>
#include <bcm/topo.h>

#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
#include <bcm_int/esw/katana.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/xgs3.h>

/*
 * Function:
 *      bcm_mpls_init
 * Purpose:
 *      Initialize the MPLS software module, clear all HW MPLS states
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_mpls_init(int unit)
{
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
        return bcm_tr_mpls_init(unit);
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_mpls_cleanup
 * Purpose:
 *      Detach the MPLS software module, clear all HW MPLS states
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_mpls_cleanup(int unit)
{
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
        return bcm_tr_mpls_cleanup(unit);
    }
#endif
#endif
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_esw_mpls_sync(int unit)
{
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
        return bcm_tr_mpls_sync(unit);
    }
#endif
#endif

    return BCM_E_NONE;
}

int
_bcm_esw_mpls_match_key_recover(int unit)
{
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_mpls_match_key_recover(unit);
    } else
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    {
        return _bcm_tr_mpls_match_key_recover(unit);
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#endif /* MPLS_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_mpls_tunnel_switch_traverse_no_lock
 * Purpose:
 *      Traverse all valid MPLS label entries and call the
 *      supplied callback routine without lock.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per MPLS entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_mpls_tunnel_switch_traverse_no_lock(int unit,
                                  bcm_mpls_tunnel_switch_traverse_cb cb,
                                  void *user_data)
{
    int rv = BCM_E_NONE;

#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && soc_feature(unit, soc_feature_mpls)) {
       rv = bcm_tr3_mpls_tunnel_switch_traverse(unit, cb, user_data);
       return rv;
    }
#endif

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit) && soc_feature(unit, soc_feature_mpls)) {
        rv = bcm_kt_mpls_tunnel_switch_traverse(unit, cb, user_data);
        return rv;
    }
#endif

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
       if (soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))
           rv = bcmi_xgs5_mpls_tunnel_switch_traverse(unit, cb, user_data);
#endif
       } else {
           rv = bcm_tr_mpls_tunnel_switch_traverse(unit, cb, user_data);
       }
       return rv;
    }
#endif
#endif
    return rv;
}

#endif

/*
 * Function:
 *      bcm_mpls_vpn_id_create
 * Purpose:
 *      Create a VPN
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_esw_mpls_vpn_id_create(int unit, bcm_mpls_vpn_config_t *info)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_th3_style_simple_mpls) &&
       ((info->flags & BCM_MPLS_VPN_VPWS) ||
        (info->flags & BCM_MPLS_VPN_VPLS))) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
        rv = bcm_tr_mpls_lock (unit) ;
        if ( rv == BCM_E_NONE ) {
             rv = bcm_tr_mpls_vpn_id_create(unit, info);
             bcm_tr_mpls_unlock (unit);
        }
        return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_vpn_id_destroy
 * Purpose:
 *      Destroy a VPN
 * Parameters:
 *      unit       - (IN)  Device Number
 *      vpn        - (IN)  VPN instance
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_esw_mpls_vpn_id_destroy(int unit, bcm_vpn_t vpn)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
       rv = bcm_tr_mpls_lock(unit);
       if ( rv == BCM_E_NONE ) {
             rv = bcm_tr_mpls_vpn_id_destroy(unit, vpn);
             bcm_tr_mpls_unlock (unit);
       }
       return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_vpn_id_destroy_all
 * Purpose:
 *      Destroy all VPNs
 * Parameters:
 *      unit       - (IN)  Device Number
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_esw_mpls_vpn_id_destroy_all(int unit)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
       rv = bcm_tr_mpls_lock(unit);
       if ( rv == BCM_E_NONE ) {
           rv = bcm_tr_mpls_vpn_id_destroy_all(unit);
           bcm_tr_mpls_unlock (unit);
       }
       return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_vpn_id_get
 * Purpose:
 *      Get a VPN
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vpn   - (IN)  VPN instance
 *      info  - (OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_esw_mpls_vpn_id_get(int unit, bcm_vpn_t vpn, bcm_mpls_vpn_config_t *info)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
        rv = bcm_tr_mpls_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv =  bcm_tr_mpls_vpn_id_get(unit, vpn, info);
            bcm_tr_mpls_unlock (unit);
        }
        return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_vpn_traverse
 * Purpose:
 *      MPLS VPN Traverse
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb   - (IN)  User callback function, called once per entry
 *      user_data  -  (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXXX
 */
int 
bcm_esw_mpls_vpn_traverse(int unit, bcm_mpls_vpn_traverse_cb cb, 
                                                          void *user_data)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
        rv = bcm_tr_mpls_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_tr_mpls_vpn_traverse(unit, cb, user_data);
            bcm_tr_mpls_unlock (unit);
        }
        return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_port_add
 * Purpose:
 *      Add an mpls port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      mpls_port - (IN/OUT) mpls port information (OUT : mpls_port_id)
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_port_add(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
       L3_LOCK(unit);
       rv = bcm_tr_mpls_lock(unit);
       if ( rv == BCM_E_NONE ) {
           rv = bcm_tr_mpls_port_add(unit, vpn, mpls_port);
           bcm_tr_mpls_unlock (unit);
       }
       L3_UNLOCK(unit);
       return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_port_delete
 * Purpose:
 *      Delete an mpls port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      mpls_port_id - (IN) mpls port ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mpls_port_id)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
       L3_LOCK(unit);
       rv = bcm_tr_mpls_lock(unit);
       if ( rv == BCM_E_NONE ) {
           rv = bcm_tr_mpls_port_delete(unit, vpn, mpls_port_id);
           bcm_tr_mpls_unlock (unit);
       }
       L3_UNLOCK(unit);
       return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_port_delete_all
 * Purpose:
 *      Delete all mpls ports from a VPN
 * Parameters:
 *      unit - Device Number
 *      vpn - VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_port_delete_all(int unit, bcm_vpn_t vpn)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
       L3_LOCK(unit);
       rv = bcm_tr_mpls_lock(unit);
       if ( rv == BCM_E_NONE ) {
           rv = bcm_tr_mpls_port_delete_all(unit, vpn);
           bcm_tr_mpls_unlock (unit);
       }
       L3_UNLOCK(unit);
       return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_port_get
 * Purpose:
 *      Get an mpls port from a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      mpls_port - (IN/OUT) mpls port information (IN : mpls_port_id)
 */
int
bcm_esw_mpls_port_get(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
               rv = bcm_tr_mpls_port_get (unit, vpn, mpls_port);
               bcm_tr_mpls_unlock (unit);
           }
           return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_port_get_all
 * Purpose:
 *      Get an mpls port from a VPN
 * Parameters:
 *      unit     - (IN) Device Number
 *      vpn      - (IN) VPN instance ID
 *      port_max   - (IN) Maximum number of ports in array
 *      port_array - (OUT) Array of mpls ports
 *      port_count - (OUT) Number of ports returned in array
 */
int
bcm_esw_mpls_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                            bcm_mpls_port_t *port_array, int *port_count)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
       rv = bcm_tr_mpls_lock(unit);
       if ( rv == BCM_E_NONE ) {
            rv = bcm_tr_mpls_port_get_all(unit, vpn, port_max,
                                  port_array, port_count);
            bcm_tr_mpls_unlock (unit);
       }
       return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_initiator_set
 * Purpose:
 *      Set the MPLS tunnel initiator parameters for an L3 interface.
 * Parameters:
 *      unit - Device Number
 *      intf - The egress L3 interface
 *      num_labels  - Number of labels in the array
 *      label_array - Array of MPLS label and header information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_tunnel_initiator_set (int unit, bcm_if_t intf, int num_labels,
                                   bcm_mpls_egress_label_t *label_array)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
            L3_LOCK(unit);
            rv = bcm_tr_mpls_lock(unit);
            if (rv == BCM_E_NONE ) {
#if defined(BCM_APACHE_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
                defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
                if (soc_feature(unit, soc_feature_egr_ip_tnl_mpls_double_wide) ||
                    (soc_feature(unit, soc_feature_th3_style_simple_mpls))) {
                    rv = bcmi_xgs5_mpls_tunnel_initiator_set(unit, intf,
                                            num_labels, label_array);
                } else
#endif
                {
                    rv = bcm_tr_mpls_tunnel_initiator_set(unit, intf,
                                            num_labels, label_array);
                }
                bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_initiator_clear
 * Purpose:
 *      Clear the MPLS tunnel initiator parameters for an L3 interface.
 * Parameters:
 *      unit - Device Number
 *      intf - The egress L3 interface
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_tunnel_initiator_clear (int unit, bcm_if_t intf)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
#if defined(BCM_APACHE_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
               defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
               if (soc_feature(unit, soc_feature_egr_ip_tnl_mpls_double_wide) ||
                    (soc_feature(unit, soc_feature_th3_style_simple_mpls))) {
                   rv = bcmi_xgs5_mpls_tunnel_initiator_clear(unit, intf);
               } else
#endif
               {
                   rv = bcm_tr_mpls_tunnel_initiator_clear(unit, intf);
               }
               bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_initiator_clear_all
 * Purpose:
 *      Clear all the MPLS tunnel initiator parameters all L3 Interfaces
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_tunnel_initiator_clear_all (int unit)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
        L3_LOCK(unit);
       if ( bcm_tr_mpls_lock (unit) == BCM_E_NONE ) {
#if defined(BCM_APACHE_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
           defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
           if (soc_feature(unit, soc_feature_egr_ip_tnl_mpls_double_wide) ||
                    (soc_feature(unit, soc_feature_th3_style_simple_mpls))) {
               rv = bcmi_xgs5_mpls_tunnel_initiator_clear_all(unit);
           } else
#endif
           {
               rv = bcm_tr_mpls_tunnel_initiator_clear_all(unit);
           }
           bcm_tr_mpls_unlock (unit);
       }
       L3_UNLOCK(unit);
       return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_initiator_get
 * Purpose:
 *      Get the the MPLS tunnel initiator parameters for an L3 interface.
 * Parameters:
 *      unit        - (IN) Device Number
 *      intf        - (IN) The egress L3 interface
 *      label_max   - (IN) Number of entries in label_array
 *      label_array - (OUT) MPLS header information
 *      label_count - (OUT) Actual number of labels returned
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_tunnel_initiator_get (int unit, bcm_if_t intf, int label_max,
                                   bcm_mpls_egress_label_t *label_array,
                                   int *label_count)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
#if defined(BCM_APACHE_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
               defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
               if (soc_feature(unit, soc_feature_egr_ip_tnl_mpls_double_wide) ||
                    (soc_feature(unit, soc_feature_th3_style_simple_mpls))) {
                   rv  = bcmi_xgs5_mpls_tunnel_initiator_get(unit,
                                  intf, label_max,
                                  label_array, label_count);
               } else
#endif
               {
                   rv = bcm_tr_mpls_tunnel_initiator_get(unit,
                                  intf, label_max,
                                  label_array, label_count);
               }
               bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_add
 * Purpose:
 *      Add an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_tunnel_switch_add (int unit, bcm_mpls_tunnel_switch_t *info)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_mpls_tunnel_switch_add(unit, info);
              bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_kt_mpls_tunnel_switch_add(unit, info);
              bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              if (soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))
                  rv = bcmi_xgs5_mpls_tunnel_switch_add(unit, info);
#endif
              } else {
                  rv = bcm_tr_mpls_tunnel_switch_add(unit, info);
              }
           }
           bcm_tr_mpls_unlock (unit);
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_delete
 * Purpose:
 *      Delete an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_tunnel_switch_delete (int unit, bcm_mpls_tunnel_switch_t *info)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_mpls_tunnel_switch_delete(unit, info);
              bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_kt_mpls_tunnel_switch_delete(unit, info);
              bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              if (soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))
                  rv = bcmi_xgs5_mpls_tunnel_switch_delete(unit, info);
#endif
              } else {
                  rv = bcm_tr_mpls_tunnel_switch_delete(unit, info);
              }
           }
           bcm_tr_mpls_unlock (unit);
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_delete_all
 * Purpose:
 *      Delete all MPLS label entries.
 * Parameters:
 *      unit   - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_tunnel_switch_delete_all (int unit)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_mpls_tunnel_switch_delete_all(unit);
              bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_kt_mpls_tunnel_switch_delete_all(unit);
              bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
              L3_LOCK(unit);
              rv = bcm_tr_mpls_lock(unit);
              if ( rv == BCM_E_NONE ) {
                  if (soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))
                      rv = bcmi_xgs5_mpls_tunnel_switch_delete_all(unit);
#endif
                  } else {
                      rv = bcm_tr_mpls_tunnel_switch_delete_all(unit);
                  }
              }
              bcm_tr_mpls_unlock (unit);
              L3_UNLOCK(unit);
              return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_get
 * Purpose:
 *      Get an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_tunnel_switch_get (int unit, bcm_mpls_tunnel_switch_t *info)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_mpls_tunnel_switch_get(unit, info);
              bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit) && soc_feature(unit, soc_feature_mpls)) {
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_kt_mpls_tunnel_switch_get(unit, info);
              bcm_tr_mpls_unlock (unit);
           }
           return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
              rv = bcm_tr_mpls_lock(unit);
              if ( rv == BCM_E_NONE ) {
                   if (soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))
                       rv = bcmi_xgs5_mpls_tunnel_switch_get(unit, info);
#endif
                   } else {
                       rv = bcm_tr_mpls_tunnel_switch_get(unit, info);
                   }
              }
              bcm_tr_mpls_unlock (unit);
              return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_traverse
 * Purpose:
 *      Traverse all valid MPLS label entries and call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per MPLS entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_tunnel_switch_traverse(int unit,
                                    bcm_mpls_tunnel_switch_traverse_cb cb,
                                    void *user_data)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && soc_feature(unit, soc_feature_mpls)) {
           L3_LOCK(unit);
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_mpls_tunnel_switch_traverse(unit, cb, user_data);
              bcm_tr_mpls_unlock (unit);
           }
           L3_UNLOCK(unit);
           return rv;
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit) && soc_feature(unit, soc_feature_mpls)) {
           rv = bcm_tr_mpls_lock(unit);
           if ( rv == BCM_E_NONE ) {
              rv = bcm_kt_mpls_tunnel_switch_traverse(unit, cb, user_data);
              bcm_tr_mpls_unlock (unit);
           }
           return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
              rv = bcm_tr_mpls_lock(unit);
              if ( rv == BCM_E_NONE ) {
                   if (soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))
                       rv = bcmi_xgs5_mpls_tunnel_switch_traverse(unit, cb, user_data);
#endif
                   } else {
                       rv = bcm_tr_mpls_tunnel_switch_traverse(unit, cb, user_data);
                   }
                   bcm_tr_mpls_unlock (unit);
              }
              return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_mpls_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a mpls virtual port.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      gport - (IN) mpls virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_mpls_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls)
        && soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_tr_mpls_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2_mpls_port_source_vp_lag_set(unit, gport, vp_lag_vp);
            bcm_tr_mpls_unlock(unit);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_mpls_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a mpls virtual port.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      gport - (IN) mpls virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_mpls_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls)
        && soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_tr_mpls_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2_mpls_port_source_vp_lag_clear(unit, gport, vp_lag_vp);
            bcm_tr_mpls_unlock(unit);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_mpls_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a mpls virtual port.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      gport - (IN) mpls virtual port GPORT ID.
 *      vp_lag_vp - (OUT) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_mpls_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls)
        && soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_tr_mpls_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2_mpls_port_source_vp_lag_get(unit, gport, vp_lag_vp);
            bcm_tr_mpls_unlock(unit);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_add
 * Purpose:
 *      Add an MPLS entropy label identifier
 * Parameters:
 *      unit - Device Number
 *      info - Entropy Label information
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_mpls_entropy_identifier_add(
    int unit, 
    uint32 options, 
    bcm_mpls_entropy_identifier_t *info)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if ((options & BCM_MPLS_ENTROPY_LABEL_IDENTIFIER_TOS) &&
            soc_feature(unit, soc_feature_td2p_mpls_entropy_label)) {
        L3_LOCK(unit);
        rv = bcm_tr_mpls_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2p_mpls_entropy_identifier_add(unit, options,
                    info);
            bcm_tr_mpls_unlock (unit);
        }
        L3_UNLOCK(unit);
        return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_delete
 * Purpose:
 *      Delete an MPLS entropy label identifier
 * Parameters:
 *      unit - Device Number
 *      info - Entropy Label information
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_mpls_entropy_identifier_delete(
    int unit, 
    bcm_mpls_entropy_identifier_t *info)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_entropy_label)) {
        L3_LOCK(unit);
        rv = bcm_tr_mpls_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2p_mpls_entropy_identifier_delete(unit, info);
            bcm_tr_mpls_unlock (unit);
        }
        L3_UNLOCK(unit);
        return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_delete_all
 * Purpose:
 *      Delete all MPLS entropy label identifier entries.
 * Parameters:
 *      unit   - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_mpls_entropy_identifier_delete_all(
    int unit)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_entropy_label)) {
        L3_LOCK(unit);
        rv = bcm_tr_mpls_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2p_mpls_entropy_identifier_delete_all(unit);
            bcm_tr_mpls_unlock (unit);
        }
        L3_UNLOCK(unit);
        return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_get
 * Purpose:
 *      Get an MPLS entrpoy label identifier entry
 * Parameters:
 *      unit - Device Number
 *      info - Entropy Label information
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_mpls_entropy_identifier_get(
    int unit, 
    bcm_mpls_entropy_identifier_t *info)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_entropy_label)) {
        L3_LOCK(unit);
        rv = bcm_tr_mpls_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2p_mpls_entropy_identifier_get(unit, info);
            bcm_tr_mpls_unlock (unit);
        }
        L3_UNLOCK(unit);
        return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_traverse
 * Purpose:
 *      Traverse all valid MPLS entropy label identifier entries 
 *      and call the supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per MPLS ELI entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_mpls_entropy_identifier_traverse(
    int unit, 
    bcm_mpls_entropy_identifier_traverse_cb cb, 
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_entropy_label)) {
        L3_LOCK(unit);
        rv = bcm_tr_mpls_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2p_mpls_entropy_identifier_traverse(unit, cb, user_data);
            bcm_tr_mpls_unlock (unit);
        }
        L3_UNLOCK(unit);
        return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_exp_map_create
 * Purpose:
 *      Create an MPLS EXP map instance.
 * Parameters:
 *      unit        - (IN)  SOC unit #
 *      flags       - (IN)  MPLS flags
 *      exp_map_id  - (OUT) Allocated EXP map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_exp_map_create(int unit, uint32 flags, int *exp_map_id)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
              rv = bcm_tr_mpls_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr_mpls_exp_map_create(unit, flags, exp_map_id);
              bcm_tr_mpls_unlock (unit);
         }
         return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_exp_map_destroy
 * Purpose:
 *      Destroy an existing MPLS EXP map instance.
 * Parameters:
 *      unit       - (IN) SOC unit #
 *      exp_map_id - (IN) EXP map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_exp_map_destroy(int unit, int exp_map_id)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
              rv = bcm_tr_mpls_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr_mpls_exp_map_destroy(unit, exp_map_id);
              bcm_tr_mpls_unlock (unit);
         }
         return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_exp_map_set
 * Purpose:
 *      Set the mapping of { internal priority, color }
 *      to a EXP value for MPLS headers
 *      in the specified EXP map instance.
 * Parameters:
 *      unit         - (IN) SOC unit #
 *      exp_map_id   - (IN) EXP map ID
 *      priority     - (IN) Internal priority
 *      color        - (IN) bcmColor*
 *      exp          - (IN) EXP value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_exp_map_set(int unit, int exp_map_id,
                         bcm_mpls_exp_map_t *exp_map)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
              rv = bcm_tr_mpls_lock(unit);
       if ( rv == BCM_E_NONE ) {
             rv = bcm_tr_mpls_exp_map_set(unit, exp_map_id, exp_map);
             bcm_tr_mpls_unlock (unit);
       }
       return rv;
    }
#endif
#endif
    return rv;
}
    
/*
 * Function:
 *      bcm_esw_mpls_exp_map_get
 * Purpose:
 *      Get the mapping of { internal priority, color }
 *      to a EXP value for MPLS headers
 *      in the specified EXP map instance.
 * Parameters:
 *      unit         - (IN)  SOC unit #
 *      exp_map_id   - (IN)  EXP map ID
 *      priority     - (IN)  Internal priority
 *      color        - (IN)  bcmColor*
 *      exp          - (OUT) EXP value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mpls_exp_map_get(int unit, int exp_map_id,
                         bcm_mpls_exp_map_t *exp_map)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
              rv = bcm_tr_mpls_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr_mpls_exp_map_get(unit, exp_map_id, exp_map);
              bcm_tr_mpls_unlock (unit);
         }
         return rv;
    }
#endif
#endif
    return rv;
}

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
 * Function:
 *     bcmi_td2p_mpls_tunnel_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction) based on the mpls tunnel info.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 */
static
bcm_error_t bcmi_td2p_mpls_tunnel_stat_get_table_info(
           int                        unit,
           bcm_if_t                   intf_id,
           uint32                     *num_of_tables,
           bcm_stat_flex_table_info_t *table_info)
{
    bcm_l3_egress_t egr_intf = {0};
    egr_l3_intf_entry_t if_entry;
    int mpls_index = 0;

    (*num_of_tables) = 0;
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(bcm_esw_l3_egress_get(unit, intf_id, &egr_intf));

    /* L3 interface info */
    SOC_IF_ERROR_RETURN(READ_EGR_L3_INTFm(unit,
                                MEM_BLOCK_ANY, egr_intf.intf, &if_entry));
    mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry,
                                              MPLS_TUNNEL_INDEXf);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && soc_feature(unit, soc_feature_td3_style_mpls)) {
        table_info[*num_of_tables].table = EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm;
    } else
#endif
    {
        table_info[*num_of_tables].table = EGR_IP_TUNNEL_MPLSm;
    }
    table_info[*num_of_tables].index = mpls_index;
    table_info[*num_of_tables].direction = bcmStatFlexDirectionEgress;
    (*num_of_tables)++;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_mpls_tunnel_stat_attach
 *
 * Description:
 *      Attach counters entries to the mpls tunnel derived from the
 *      given L3 Egress interface
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id
 *      stat_counter_id  - (IN) Stat Counter ID
 *
 * Return Value:
 *       BCM_E_XXX
 */
STATIC bcm_error_t bcmi_td2p_mpls_tunnel_stat_attach(
            int      unit,
            bcm_if_t intf_id,
            uint32   stat_counter_id)
{
    soc_mem_t                  table = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionEgress;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object = bcmStatObjectEgrMplsTunnelLabel;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;
    uint32                     count = 0;
    uint32                     actual_num_tables = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode, &object,
                  &offset_mode, &pool_number, &base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,
                         object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(unit,
                        object, 1, &actual_num_tables, &table, &direction));

    BCM_IF_ERROR_RETURN(bcmi_td2p_mpls_tunnel_stat_get_table_info(
                        unit, intf_id, &num_of_tables, &table_info[0]));
    for (count = 0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
             (table_info[count].table == table) ) {
              if (direction == bcmStatFlexDirectionIngress) {
                  return _bcm_esw_stat_flex_attach_ingress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index,
                         offset_mode,
                         base_index,
                         pool_number);
              } else {
                  return _bcm_esw_stat_flex_attach_egress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index,
                         object,
                         offset_mode,
                         base_index,
                         pool_number);
              }
         }
    }
    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *       bcmi_td2p_mpls_tunnel_stat_detach
 *
 * Description:
 *       Detach  counters entries to the mpls tunnel derived from the
 *       given L3 Egress interface.
 *
 * Parameters:
 *       unit             - (IN) unit number
 *       intf_id          - (IN) Interface id of a egress L3 object.
 *       stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *       BCM_E_XXX
 */
STATIC
bcm_error_t bcmi_td2p_mpls_tunnel_stat_detach(
            int      unit,
            bcm_if_t intf_id,
            uint32 stat_counter_id)
{
    uint32                     count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;
    bcm_stat_object_t          object = bcmStatObjectEgrMplsTunnelLabel;
    bcm_stat_flex_mode_t       offset_mode = 0;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;

    BCM_IF_ERROR_RETURN(bcmi_td2p_mpls_tunnel_stat_get_table_info(
                   unit, intf_id, &num_of_tables, &table_info[0]));
    if (stat_counter_id != 0) {
        _bcm_esw_stat_get_counter_id_info(
                unit, stat_counter_id,
                &group_mode, &object,
                &offset_mode, &pool_number, &base_index);
    }
    for (count = 0; count < num_of_tables ; count++) {
         if (table_info[count].direction == bcmStatFlexDirectionIngress) {
             rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                      unit, table_info[count].table, table_info[count].index);
            if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
            }
         } else {
             rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, object,
                        table_info[count].table, table_info[count].index);
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = 1;
             }
         }
    }

    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}

/*
 * Function:
 *      bcmi_td2p_mpls_tunnel_stat_counter_get
 *
 * Description:
 *      Get the specified counter statistic for a mpls tunnel derived from the
 *      given L3 Egress interface
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 */
STATIC
bcm_error_t bcmi_td2p_mpls_tunnel_stat_counter_get(
                    int              unit,
                    int              sync_mode,
                    bcm_if_t         intf_id,
                    uint32           stat_counter_id,
                    bcm_mpls_stat_t    stat,
                    uint32           num_entries,
                    uint32           *counter_indexes,
                    bcm_stat_value_t *counter_values)
{
    uint32                     table_count = 0;
    uint32                     index_count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     byte_flag = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object = bcmStatObjectEgrMplsTunnelLabel;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;

    if (stat_counter_id != 0) {
        _bcm_esw_stat_get_counter_id_info(
                unit, stat_counter_id,
                &group_mode, &object,
                &offset_mode, &pool_number, &base_index);
    }
    if ((stat == bcmMplsOutPkts) ||
        (stat == bcmMplsOutBytes)) {
         direction = bcmStatFlexDirectionEgress;
    } else {
         direction = bcmStatFlexDirectionIngress;
    }
    if ((stat == bcmMplsInPkts)|| (stat == bcmMplsOutPkts)) {
        byte_flag = 0;
    } else {
        byte_flag = 1;
    }

    BCM_IF_ERROR_RETURN(bcmi_td2p_mpls_tunnel_stat_get_table_info(
                        unit, intf_id, &num_of_tables, &table_info[0]));

    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                                      unit, sync_mode,
                                      table_info[table_count].index,
                                      table_info[table_count].table,
                                      object,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
}



/*
 * Function:
 *      bcmi_td2p_mpls_tunnel_stat_counter_set
 *
 * Description:
 *      Set the specified counter statistic for a mpls tunnel derived from the
 *      given L3 Egress interface
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 */
STATIC
bcm_error_t bcmi_td2p_mpls_tunnel_stat_counter_set(
            int              unit,
            bcm_if_t         intf_id,
            uint32           stat_counter_id,
            bcm_mpls_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                     table_count = 0;
    uint32                     index_count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     byte_flag = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object = bcmStatObjectEgrMplsTunnelLabel;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;

   if ((stat == bcmMplsOutPkts) ||
        (stat == bcmMplsOutBytes)) {
         direction = bcmStatFlexDirectionEgress;
    } else {
         direction = bcmStatFlexDirectionIngress;
    }
    if ((stat == bcmMplsInPkts) || (stat == bcmMplsOutPkts)) {
        byte_flag = 0;
    } else {
        byte_flag = 1;
    }
    if (stat_counter_id != 0) {
        _bcm_esw_stat_get_counter_id_info(
                unit, stat_counter_id,
                &group_mode, &object,
                &offset_mode, &pool_number, &base_index);
    }
    BCM_IF_ERROR_RETURN(bcmi_td2p_mpls_tunnel_stat_get_table_info(
                        unit, intf_id, &num_of_tables, &table_info[0]));

    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                      unit,
                                      table_info[table_count].index,
                                      table_info[table_count].table,
                                      object,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
            }
         }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_td2p_mpls_tunnel_stat_id_get
 *
 * Description:
 *      Get stat counter id associated with given egress interface id
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 *
 * Return Value:
 *      BCM_E_XXX
 */
STATIC
int bcmi_td2p_mpls_tunnel_stat_id_get(
            int             unit,
            bcm_if_t        intf_id,
            bcm_mpls_stat_t   stat,
            uint32          *stat_counter_id)
{
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index = 0;
    uint32                     num_stat_counter_ids = 0;

   if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    BCM_IF_ERROR_RETURN(bcmi_td2p_mpls_tunnel_stat_get_table_info(
                        unit, intf_id, &num_of_tables, &table_info[0]));
    for (index = 0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids, stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmi_td2p_mpls_tunnel_label_counter_id_num_get
 *
 * Description:
 *      Get array of counters ids' attached to the mpls tunnel
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id
 *      num_counters     - (IN) Number of counters attached
 *                              to the interface id
 *      Stat_counter_id  - (OUT) Array of stat counter ids
 *                               associated with the interface id
 *
 * Return Value:
 *       BCM_E_XXX
 */
STATIC bcm_error_t bcmi_td2p_mpls_tunnel_label_counter_id_num_get(
        int      unit,
        bcm_if_t intf_id,
        int      *num_counters,
        uint32   *stat_counter_id)
{
    uint32                     num_of_tables = 0;
    int                        num_labels = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    BCM_IF_ERROR_RETURN(bcmi_td2p_mpls_tunnel_stat_get_table_info(
                unit, intf_id, &num_of_tables, &table_info[0]));

    BCM_IF_ERROR_RETURN(_bcm_tr_mpls_tunnel_chain_label_count_get(unit,
                table_info[0].index, &num_labels));
#if defined (INCLUDE_L3)
    if (*num_counters <= num_labels) {
        return _bcm_esw_stat_flex_get_multi_counter_id(unit,&table_info[0],
                num_counters, stat_counter_id);
    } else {
        return BCM_E_PARAM;
    }
#endif
    return BCM_E_UNAVAIL;
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

/*
 * Function:
 *      bcm_esw_mpls_tunnel_stat_attach
 *
 * Description:
 *      Attach counters entries to the the mpls tunnel derived from the
 *      given L3 Egress interface
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_stat_attach(
            int      unit,
            bcm_if_t intf_id,
            uint32   stat_counter_id)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return bcmi_td2p_mpls_tunnel_stat_attach(unit, intf_id, stat_counter_id);
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_esw_mpls_tunnel_stat_detach
 *
 * Description:
 *      Detach  counters entries to the the mpls tunnel derived from the
 *      given L3 Egress interface.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_stat_detach(
            int      unit,
            bcm_if_t intf_id)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return bcmi_td2p_mpls_tunnel_stat_detach(unit, intf_id, 0);
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_tunnel_stat_counter_get
 *
 * Description:
 *      Get the specified counter statistic for a mpls tunnel derived from the
 *      given L3 Egress interface
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 *  Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_stat_counter_get(
            int              unit,
            bcm_if_t         intf_id,
            bcm_mpls_stat_t    stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    return bcmi_td2p_mpls_tunnel_stat_counter_get(unit, 0, intf_id, 0, stat,
                                        num_entries,
                                        counter_indexes,
                                        counter_values);
#endif
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_tunnel_stat_counter_sync_get
 *
 * Description:
 *      Retrieve set of counter statistics for a mpls tunnel derived from the
 *      given L3 Egress interface
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 *  Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_stat_counter_sync_get(
            int              unit,
            bcm_if_t         intf_id,
            bcm_mpls_stat_t    stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    return bcmi_td2p_mpls_tunnel_stat_counter_get(unit, 1, intf_id, 0, stat,
                                          num_entries, counter_indexes,
                                          counter_values);
#endif
#endif
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_esw_mpls_tunnel_stat_counter_set
 *
 * Description:
 *      Set the specified counter statistic for a mpls tunnel derived from the
 *      given L3 Egress interface
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_stat_counter_set(
            int              unit,
            bcm_if_t         intf_id,
            bcm_mpls_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return bcmi_td2p_mpls_tunnel_stat_counter_set(unit, intf_id, 0, stat,
                      num_entries, counter_indexes, counter_values);
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_tunnel_stat_id_get
 *
 * Description:
 *      Get stat counter id associated with given egress interface id
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 *
 * Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_stat_id_get(
            int             unit,
            bcm_if_t        intf_id,
            bcm_mpls_stat_t   stat,
            uint32          *stat_counter_id)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return bcmi_td2p_mpls_tunnel_stat_id_get(unit, intf_id, stat,
                                stat_counter_id);
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_tunnel_label_counter_id_detach
 *
 * Description:
 *      Detach  counter entry of the specific mpls tunnel label pointed
 *      to by the given stat counter id.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_label_counter_id_detach(
            int      unit,
            bcm_if_t intf_id,
            uint32 stat_counter_id)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return bcmi_td2p_mpls_tunnel_stat_detach(unit, intf_id, stat_counter_id);
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_tunnel_label_counter_id_stat_get
 *
 * Description:
 *      Get the specified counter statistic for the given mpls tunnel
 *      label and counter id associated with it
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat_counter_id  - (IN) Stat Counter Id
 *      stat_info        - (IN/OUT) Mpls Stat Info Structure
 *
 *  Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_label_counter_id_stat_get(
            int              unit,
            bcm_if_t         intf_id,
            uint32           stat_counter_id,
            bcm_mpls_stat_info_t *stat_info)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    return bcmi_td2p_mpls_tunnel_stat_counter_get(unit, 0, intf_id, stat_counter_id,
                                        stat_info->stat,stat_info->num_entries,
                                        stat_info->counter_indexes,
                                        stat_info->counter_values);
#endif
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_tunnel_label_counter_id_stat_sync_get
 *
 * Description:
 *      Get the specified counter statistic for the given mpls tunnel
 *      label and counter id associated with it
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat_counter_id  - (IN) Stat Counter Id
 *      stat_info        - (IN/OUT) Mpls Stat Info Structure
 *
 *  Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_label_counter_id_stat_sync_get(
            int              unit,
            bcm_if_t         intf_id,
            uint32           stat_counter_id,
            bcm_mpls_stat_info_t *stat_info)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    return bcmi_td2p_mpls_tunnel_stat_counter_get(unit, 1, intf_id, stat_counter_id,
                                          stat_info->stat,stat_info->num_entries,
                                          stat_info->counter_indexes,
                                          stat_info->counter_values);
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_tunnel_label_counter_id_stat_set
 *
 * Description:
 *      Set the specified counter statistic for the given mpls tunnel
 *      label and counter id associated with it
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      intf_id          - (IN) Interface id of a egress L3 object.
 *      stat_counter_id  - (IN) Stat Counter Id
 *      stat_info        - (IN) Mpls Stat Info Structure
 *
 *  Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_label_counter_id_stat_set(
            int              unit,
            bcm_if_t         intf_id,
            uint32           stat_counter_id,
            bcm_mpls_stat_info_t *stat_info)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return bcmi_td2p_mpls_tunnel_stat_counter_set(unit, intf_id,
                      stat_counter_id,stat_info->stat, stat_info->num_entries,
                      stat_info->counter_indexes,stat_info->counter_values);
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_tunnel_label_counter_id_num_get
 *
 * Description:
 *      Get stat counter id associated with given egress interface id
 *
 * Parameters:
 *      unit             - (IN)  unit number
 *      intf_id          - (IN)  Interface id of a egress L3 object.
 *      num_counters     - (IN/OUT) Number of counters attached
 *                               to the interface id
 *      Stat_counter_id  - (OUT) Array of stat counter ids
 *                               associated with the interface id
 *
 * Return Value:
 *      BCM_E_XXX
 */
int bcm_esw_mpls_tunnel_label_counter_id_num_get(
            int             unit,
            bcm_if_t        intf_id,
            int             *num_counters,
            uint32          *stat_counter_id)
{
#ifdef BCM_MPLS_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return bcmi_td2p_mpls_tunnel_label_counter_id_num_get(unit, intf_id,
                num_counters,stat_counter_id);
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}
#if defined(BCM_TRIDENT2_SUPPORT)

/* Duplicated from Katana code 
 * This should also serve as default MPLS flex stat counter support. 
 */
 
/*
 * Function:
 *      td2_mpls_port_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for specific vpn and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_port_stat_get_table_info(
            int                        unit,
            bcm_vpn_t                  vpn,
            bcm_gport_t                port,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
    int                   vp=0;
    ing_dvp_table_entry_t dvp={{0}};
    int                   nh_index=0;
    initial_prot_nhi_table_entry_t prot_entry;
    int vpless_failover_port = FALSE;
    int rv;

    if (_BCM_MPLS_GPORT_FAILOVER_VPLESS_GET(port)) {
        vpless_failover_port = TRUE;
        port = _BCM_MPLS_GPORT_FAILOVER_VPLESS_CLEAR(port);
    }

    (*num_of_tables)=0;
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }
    if (soc_feature(unit, soc_feature_mpls)) {
        BCM_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
        if (!_BCM_MPLS_VPN_IS_VPLS(vpn) &&
            !_BCM_MPLS_VPN_IS_VPWS(vpn)) {
            bcm_tr_mpls_unlock (unit);
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
        if (vp == -1) {
            bcm_tr_mpls_unlock (unit);
            return BCM_E_PARAM;
        }
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            bcm_tr_mpls_unlock (unit);
            return BCM_E_NOT_FOUND;
        }
        table_info[*num_of_tables].table= SOURCE_VPm;
        table_info[*num_of_tables].index=vp;
        table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
        if(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp) == BCM_E_NONE) {
           nh_index=soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
           rv = BCM_E_NONE;
           if (vpless_failover_port == TRUE) {
               rv = READ_INITIAL_PROT_NHI_TABLEm(unit,MEM_BLOCK_ANY, nh_index, &prot_entry);
               if (BCM_SUCCESS(rv)) {
                   nh_index = soc_INITIAL_PROT_NHI_TABLEm_field32_get(unit,&prot_entry,
                              PROT_NEXT_HOP_INDEXf);
               }
           }
           if (BCM_SUCCESS(rv)) {
               table_info[*num_of_tables].table= EGR_L3_NEXT_HOPm;
               table_info[*num_of_tables].index=nh_index;
               table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
               (*num_of_tables)++;
           }
        }
        bcm_tr_mpls_unlock (unit);
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

#ifdef BCM_TOMAHAWK3_SUPPORT
/* Convert key part of application format to HW entry. */
STATIC int
th3_mpls_entry_set_key(int unit, bcm_mpls_tunnel_switch_t *info,
                           mpls_entry_entry_t *ment)
{
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int rv, gport_id;
    soc_mem_t mem = MPLS_ENTRY_SINGLEm;
    sal_memset(ment, 0, sizeof(mpls_entry_entry_t));

    if (info->port == BCM_GPORT_INVALID) {
        /* Global label, mod/port not part of lookup key */
        soc_mem_field32_set(unit, mem,  ment, PORT_NUMf, 0);
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
            soc_mem_field32_set(unit, mem,  ment, MPLS__MPLS_LABELf, info->label);
        } else {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, ment, BASE_VALIDf, 1);
        return BCM_E_NONE;
    }

    rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out,
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_GPORT_IS_TRUNK(info->port)) {
        soc_mem_field32_set(unit, mem, ment, Tf, 1);
        soc_mem_field32_set(unit, mem, ment, TGIDf, trunk_id);
    } else {
        soc_mem_field32_set(unit, mem, ment, PORT_NUMf, port_out);
    }
    if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
        soc_mem_field32_set(unit, mem, ment, MPLS__MPLS_LABELf, info->label);
    } else {
        return BCM_E_PARAM;
    }

    soc_mem_field32_set(unit, mem, ment, BASE_VALIDf, 1);

    return BCM_E_NONE;
}
#endif

/* Convert key part of application format to HW entry. */
STATIC int
td2_mpls_entry_set_key(int unit, bcm_mpls_tunnel_switch_t *info,
                           mpls_entry_entry_t *ment)
{
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int rv, gport_id;

    sal_memset(ment, 0, sizeof(mpls_entry_entry_t));

    if (info->port == BCM_GPORT_INVALID) {
        /* Global label, mod/port not part of lookup key */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MODULE_IDf, 0);
        soc_MPLS_ENTRYm_field32_set(unit, ment, PORT_NUMf, 0);
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_LABELf, info->label);
        } else {
            return BCM_E_PARAM;
        }

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, BASE_VALID_0f, 3);
            soc_MPLS_ENTRYm_field32_set(unit, ment, BASE_VALID_1f, 7);
        } else {
        soc_MPLS_ENTRYm_field32_set(unit, ment, VALIDf, 1);
        }
        return BCM_E_NONE;
    }

    rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out,
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_GPORT_IS_TRUNK(info->port)) {
        soc_MPLS_ENTRYm_field32_set(unit, ment, Tf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, ment, TGIDf, trunk_id);
    } else {
        soc_MPLS_ENTRYm_field32_set(unit, ment, MODULE_IDf, mod_out);
        soc_MPLS_ENTRYm_field32_set(unit, ment, PORT_NUMf, port_out);
    }
    if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_LABELf, info->label);
    } else {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        soc_MPLS_ENTRYm_field32_set(unit, ment, BASE_VALID_0f, 3);
        soc_MPLS_ENTRYm_field32_set(unit, ment, BASE_VALID_1f, 7);
    } else {
    soc_MPLS_ENTRYm_field32_set(unit, ment, VALIDf, 1);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      td2_mpls_label_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given mpls label and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      object           - (IN) Stat Object
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

STATIC
bcm_error_t td2_mpls_label_stat_get_table_info(
            int                        unit,
            bcm_mpls_label_t           label,
            bcm_gport_t                port,
            bcm_stat_object_t          object,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
    bcm_error_t              rv=BCM_E_NOT_FOUND;
    bcm_mpls_tunnel_switch_t mpls_tunnel_switch={0};
    mpls_entry_entry_t       ment={{0}};
    int                      index=0;
    soc_mem_t                mem = MPLS_ENTRYm;
    int                      found = -1;

    (*num_of_tables)=0;
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }
    if (!soc_feature(unit, soc_feature_mpls)) {
        return BCM_E_UNAVAIL;
    }
    
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
    }
#endif
    sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));

    mpls_tunnel_switch.port = port;
    if (BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
       mpls_tunnel_switch.label = label;
    } else {
        return BCM_E_PARAM;
    }
    if ((object == bcmStatObjectIngMplsSwitchLabel) ||
        (object == bcmStatObjectIngMplsSwitchSecondLabel)) {
    if (!soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        BCM_IF_ERROR_RETURN(td2_mpls_entry_set_key(unit,
                    &mpls_tunnel_switch,&ment));
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    else {
        BCM_IF_ERROR_RETURN(th3_mpls_entry_set_key(unit,
                    &mpls_tunnel_switch,&ment));
    }
#endif
        if ((rv=soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0)) == BCM_E_NONE) {
            found = 1;
        }
    } else if ((object == bcmStatObjectIngMplsFrrLabel) &&
            (soc_feature(unit, soc_feature_mpls_frr_lookup))){
        mem = L3_TUNNELm;
        rv =  _bcm_tr3_mpls_tunnel_switch_frr_get(unit, &mpls_tunnel_switch, &index);
        if (rv == BCM_E_NONE) {
            found = 1;
        }
    } else if (object == bcmStatObjectMaxValue) {
        if (!soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
            BCM_IF_ERROR_RETURN(td2_mpls_entry_set_key(unit,
                        &mpls_tunnel_switch,&ment));
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else {
            BCM_IF_ERROR_RETURN(th3_mpls_entry_set_key(unit,
                        &mpls_tunnel_switch,&ment));
        }
#endif
        if ((rv=soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0)) == BCM_E_NONE) {
            mem = MPLS_ENTRYm;
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
                mem = MPLS_ENTRY_SINGLEm;
            }
#endif
        } else if (soc_feature(unit, soc_feature_mpls_frr_lookup)) {
            if ((rv= _bcm_tr3_mpls_tunnel_switch_frr_get(unit,
                            &mpls_tunnel_switch, &index)) == BCM_E_NONE) {
                mem = L3_TUNNELm;
            } else {
                return rv;
            }
        } else {
            return rv;
        }
        found = 1;
    }

    if (found == 1) {
        table_info[*num_of_tables].table= mem;
        table_info[*num_of_tables].index=index;
        table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
    }
    return rv;
}


/*
 * Function:
 *      td2_mpls_port_stat_attach
 * Description:
 *      Attach counters entries to the given mpls gport and vpn
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_port_stat_attach(
                   int         unit,
                   bcm_vpn_t   vpn,
                   bcm_gport_t port,
                   uint32      stat_counter_id)
{
    soc_mem_t                 table=0;
    bcm_stat_flex_direction_t direction=bcmStatFlexDirectionIngress;
    uint32                    pool_number=0;
    uint32                    base_index=0;
    bcm_stat_flex_mode_t      offset_mode=0;
    bcm_stat_object_t         object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t     group_mode= bcmStatGroupModeSingle;
    uint32                    count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));

    BCM_IF_ERROR_RETURN(td2_mpls_port_stat_get_table_info(
                   unit, vpn,port,&num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
           (table_info[count].table == table) ) {
              if (direction == bcmStatFlexDirectionIngress) {
              return _bcm_esw_stat_flex_attach_ingress_table_counters(
                     unit,
                     table_info[count].table,
                     table_info[count].index,
                     offset_mode,
                     base_index,
                     pool_number);
           } else {
              return _bcm_esw_stat_flex_attach_egress_table_counters(
                     unit,
                     table_info[count].table,
                     table_info[count].index,
                     0,
                     offset_mode,
                     base_index,
                     pool_number);
           } 
      }
    }
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      td2_mpls_port_stat_detach
 * Description:
 *      Detach counters entries to the given mpls port and vpn
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_port_stat_detach(
                   int         unit,
                   bcm_vpn_t   vpn,
                   bcm_gport_t port)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(td2_mpls_port_stat_get_table_info(
                   unit, vpn,port,&num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
      if (table_info[count].direction == bcmStatFlexDirectionIngress) {
           rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
           }
      } else {
           rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, 0, table_info[count].table, table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
           }
      }
    }

    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}
/*
 * Function:
 *      td2_mpls_port_stat_counter_get
 * Description:
 *      Get counter statistic values for specific vpn and gport
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_port_stat_counter_get(
                   int                  unit,
                   int                  sync_mode,
                   bcm_vpn_t            vpn,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
    
    BCM_IF_ERROR_RETURN(td2_mpls_port_stat_get_table_info(
                   unit, vpn,port,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                         unit, sync_mode,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0, byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      td2_mpls_port_stat_counter_set
 * Description:
 *      Set counter statistic values for specific vpn and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_port_stat_counter_set(
                   int                  unit,
                   bcm_vpn_t            vpn,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
    
    BCM_IF_ERROR_RETURN(td2_mpls_port_stat_get_table_info(
                   unit, vpn,port,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                         unit,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0, byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      td2_mpls_port_stat_id_get
 * Description:
 *      Get stat counter id associated with specific vpn and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_port_stat_id_get(
                   int             unit,
                   bcm_vpn_t       vpn,
                   bcm_gport_t     port,
                   bcm_mpls_stat_t stat,
                   uint32          *stat_counter_id)
{
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
      }
    BCM_IF_ERROR_RETURN(td2_mpls_port_stat_get_table_info(
                        unit,vpn,port,&num_of_tables,&table_info[0]));
    for (index=0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
  }
    return BCM_E_NOT_FOUND;
}
/*
 * Function:
 *      td2_mpls_label_stat_attach
 * Description:
 *      Attach counters entries to the given mpls label and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_label_stat_attach(
                   int              unit,
                   bcm_mpls_label_t label,
                   bcm_gport_t      port,
                   uint32           stat_counter_id)
{
    soc_mem_t                 table=0;
    bcm_stat_flex_direction_t direction=bcmStatFlexDirectionIngress;
    uint32                    pool_number=0;
    uint32                    base_index=0;
    bcm_stat_flex_mode_t      offset_mode=0;
    bcm_stat_object_t         object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t     group_mode= bcmStatGroupModeSingle;
    uint32                    count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(td2_mpls_label_stat_get_table_info(
                   unit, label,port, object, &num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
           (table_info[count].table == table) ) {
              if (direction == bcmStatFlexDirectionIngress) {
              return _bcm_esw_stat_flex_attach_ingress_table_counters(
                     unit,
                     table_info[count].table,
                     table_info[count].index,
                     offset_mode,
                     base_index,
                     pool_number);
           } else {
              return _bcm_esw_stat_flex_attach_egress_table_counters(
                     unit,
                     table_info[count].table,
                     table_info[count].index,
                     0,
                     offset_mode,
                     base_index,
                     pool_number);
           } 
      }
    }
    return BCM_E_NOT_FOUND;
}
/*
 * Function:
 *      td2_mpls_label_stat_detach
 * Description:
 *      Detach counters entries to the given mpls label and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_label_stat_detach(
                   int              unit,
                   bcm_mpls_label_t label,
                   bcm_gport_t      port)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(td2_mpls_label_stat_get_table_info(
                   unit, label,port, bcmStatObjectMaxValue, &num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
      if (table_info[count].direction == bcmStatFlexDirectionIngress) {
           rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
           }
      } else {
           rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, 0, table_info[count].table, table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
           }
      }
    }

    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}
/*
 * Function:
 *      td2_mpls_label_stat_counter_get
 * Description:
 *      Get counter statistic values for specific MPLS label and gport
 *      Get counter statistic values for specific vpn and gport
 *      if sync_mode is set, sync the sw accumulated count
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_label_stat_counter_get(
                   int                  unit,
                   int                  sync_mode,
                   bcm_mpls_label_t     label,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(td2_mpls_label_stat_get_table_info(
                   unit, label,port,bcmStatObjectMaxValue,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                         unit, sync_mode,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0, byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      td2_mpls_label_stat_counter_set
 * Description:
 *      Set counter statistic values for specific MPLS label and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_label_stat_counter_set(
                   int                  unit,
                   bcm_mpls_label_t     label,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_vpn_t                       vpn = 0;

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        /* Just create a valid vpn id to pass the check */
        _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, 1);
        BCM_IF_ERROR_RETURN(td2_mpls_port_stat_get_table_info(
            unit, vpn, port, &num_of_tables, &table_info[0]));
    } else {
        if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(td2_mpls_label_stat_get_table_info(
            unit, label, port, bcmStatObjectMaxValue, &num_of_tables, &table_info[0]));
    }

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                         unit,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0, byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      td2_mpls_label_stat_id_get
 * Description:
 *      Get stat counter id associated with given vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t td2_mpls_label_stat_id_get(
            int                  unit,
            bcm_mpls_label_t     label,
            bcm_gport_t          port,
            bcm_mpls_stat_t      stat,
            uint32               *stat_counter_id)
{
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    BCM_IF_ERROR_RETURN(td2_mpls_label_stat_get_table_info(
                        unit,label,port,bcmStatObjectMaxValue,&num_of_tables,&table_info[0]));
    for (index=0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
}
/*
 * Function:
 *      td2_mpls_label_stat_enable_set
 * Purpose:
 *      Enable statistics collection for MPLS label or MPLS gport
 * Parameters:
 *      unit - (IN) Unit number.
 *      label - (IN) MPLS label
 *      port - (IN) MPLS gport
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC
int td2_mpls_label_stat_enable_set(
    int              unit, 
    bcm_mpls_label_t label,
    bcm_gport_t      port, 
    int              enable)
{
    uint32                     num_of_tables=0;
    uint32                     num_stat_counter_ids=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_stat_object_t          object=bcmStatObjectIngPort;
    uint32                     stat_counter_id[
                                       BCM_STAT_FLEX_COUNTER_MAX_DIRECTION]={0};
    uint32                     num_entries=0;
    int                        index=0;

    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        /* When the gport is an MPLS port, we'll turn on the
         * port-based tracking to comply with API definition.
         */

        int vpn;

        /* just create a valid vpn id to pass the check */
        _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, 1);
        BCM_IF_ERROR_RETURN(td2_mpls_port_stat_get_table_info(
                        unit,vpn,port,&num_of_tables,&table_info[0]));
        if (enable ) {
            for(index=0;index < num_of_tables ;index++) {
                if (table_info[index].direction == bcmStatFlexDirectionIngress) {
                    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_ingress_object(
                                        unit,table_info[index].table,
                                        table_info[index].index,NULL,&object));
                } else {
                    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_egress_object(
                                        unit,table_info[index].table,
                                        table_info[index].index,NULL,&object));
                }
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_create(
                                    unit,object,bcmStatGroupModeSingle,
                                &stat_counter_id[table_info[index].direction],
                                &num_entries));
                BCM_IF_ERROR_RETURN(td2_mpls_port_stat_attach(
                                unit,vpn,port,
                                stat_counter_id[table_info[index].direction]));
            }
            return BCM_E_NONE;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_counter_id(
                            unit, num_of_tables,&table_info[0],
                            &num_stat_counter_ids,&stat_counter_id[0]));
            if ((stat_counter_id[bcmStatFlexDirectionIngress] == 0) &&
                (stat_counter_id[bcmStatFlexDirectionEgress] == 0)) {
                 return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(td2_mpls_port_stat_detach(unit,vpn,port));
            if (stat_counter_id[bcmStatFlexDirectionIngress] != 0) {
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionIngress]));
            }
            if (stat_counter_id[bcmStatFlexDirectionEgress] != 0) {
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionEgress]));
            }
            return BCM_E_NONE;
        }

    }

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(td2_mpls_label_stat_get_table_info(
                        unit,label,port,bcmStatObjectMaxValue,&num_of_tables,&table_info[0]));
    if (enable ) {
        for(index=0;index < num_of_tables ;index++) {
            if (table_info[index].direction == bcmStatFlexDirectionIngress) {
                BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_ingress_object(
                                    unit,table_info[index].table,
                                    table_info[index].index,NULL,&object));
            } else {
                BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_egress_object(
                                    unit,table_info[index].table,
                                    table_info[index].index,NULL,&object));
            }
            BCM_IF_ERROR_RETURN(bcm_esw_stat_group_create(
                                unit,object,bcmStatGroupModeSingle,
                                &stat_counter_id[table_info[index].direction],
                                &num_entries));
            BCM_IF_ERROR_RETURN(bcm_esw_mpls_label_stat_attach(
                                unit,label,port,
                                stat_counter_id[table_info[index].direction]));
        }
        return BCM_E_NONE;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_counter_id(
                            unit, num_of_tables,&table_info[0],
                            &num_stat_counter_ids,&stat_counter_id[0]));
        if ((stat_counter_id[bcmStatFlexDirectionIngress] == 0) &&
            (stat_counter_id[bcmStatFlexDirectionEgress] == 0)) {
             return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_mpls_label_stat_detach(unit,label,port));
        if (stat_counter_id[bcmStatFlexDirectionIngress] != 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionIngress]));
        }
        if (stat_counter_id[bcmStatFlexDirectionEgress] != 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionEgress]));
        }
        return BCM_E_NONE;
    }

}
/*
 * Function:
 *      td2_mpls_label_stat_get
 * Purpose:
 *      Get L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 *      val    - (OUT) 64-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */

STATIC
int td2_mpls_label_stat_get(
                            int              unit, 
                            int              sync_mode,
                            bcm_mpls_label_t label, 
                            bcm_gport_t      port,
                            bcm_mpls_stat_t  stat, 
                            uint64           *val)
{
    uint32           counter_indexes=0   ;
    bcm_stat_value_t counter_values={0};

    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        int vpn;

        /* just create a valid vpn id to pass the check */
        _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, 1);

        BCM_IF_ERROR_RETURN(td2_mpls_port_stat_counter_get(unit, sync_mode, 
                                                           vpn, port, stat, 
                                                           1, &counter_indexes,
                                                           &counter_values));
    } else {
        if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(td2_mpls_label_stat_counter_get(
                        unit, sync_mode, label, port, stat, 1,
                        &counter_indexes, &counter_values));
    }

    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
         COMPILER_64_SET(*val,
                         COMPILER_64_HI(counter_values.packets64),
                         COMPILER_64_LO(counter_values.packets64));
    } else {
         COMPILER_64_SET(*val,
                         COMPILER_64_HI(counter_values.bytes),
                         COMPILER_64_LO(counter_values.bytes));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      td2_mpls_label_stat_get32
 * Purpose:
 *      Get L2 MPLS PW Stats
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 * Parameters:
 *      unit      - (IN) SOC unit #
 *      sync_mode - (IN) hwcount is to be synced to sw count 
 *      label     - (IN) MPLS label
 *      port      - (IN) MPLS gport
 *      stat      - (IN)  specify the Stat type
 *      val       - (OUT) 32-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */

STATIC
int td2_mpls_label_stat_get32(
                              int              unit,
                              int              sync_mode,
                              bcm_mpls_label_t label, 
                              bcm_gport_t      port, 
                              bcm_mpls_stat_t  stat, 
                              uint32           *val)
{
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};

    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        int vpn;

        /* just create a valid vpn id to pass the check */
        _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, 1);

        BCM_IF_ERROR_RETURN(td2_mpls_port_stat_counter_get(
               unit, sync_mode, vpn, port, stat, 1,
               &counter_indexes, &counter_values));
    } else {

        if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(td2_mpls_label_stat_counter_get(
                        unit, sync_mode, label, port, stat, 1,
                        &counter_indexes, &counter_values));
    }

    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
         *val = counter_values.packets;
    } else {
         /* Ignoring Hi bytes value */
         *val = COMPILER_64_LO(counter_values.bytes);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      td2_mpls_label_stat_clear
 * Purpose:
 *      Clear L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 * Returns:
 *      BCM_E_XXX
 */

STATIC
int td2_mpls_label_stat_clear(
    int              unit, 
    bcm_mpls_label_t label, 
    bcm_gport_t      port, 
    bcm_mpls_stat_t  stat)
{
    uint32           counter_indexes=0   ;
    bcm_stat_value_t counter_values={0};

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_mpls_label_stat_counter_set(
                        unit,label,port,stat,1,
                        &counter_indexes,&counter_values));
    return BCM_E_NONE;
}


#endif /* BCM_TRIDENT2_SUPPORT */

/*
 * Function:
 *      bcm_esw_mpls_label_stat_enable_set
 * Purpose:
 *      Enable statistics collection for MPLS label or MPLS gport
 * Parameters:
 *      unit - (IN) Unit number.
 *      label - (IN) MPLS label
 *      port - (IN) MPLS gport
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_mpls_label_stat_enable_set(int unit, bcm_mpls_label_t label, 
                                   bcm_gport_t port, int enable)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_gport_service_counters) &&
        soc_feature(unit, soc_feature_mpls)) {
        rv = bcm_tr_mpls_lock(unit);
        if (BCM_E_NONE == rv) {
            rv = bcm_tr2_mpls_label_stat_enable_set(unit, label,
                                                    port, enable,0);
            bcm_tr_mpls_unlock(unit);
        }
    }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT)
      else if (soc_feature(unit,soc_feature_advanced_flex_counter) &&
               soc_feature(unit, soc_feature_mpls)) {
               BCM_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
#if defined(BCM_KATANA_SUPPORT) 
               if (SOC_IS_KATANAX(unit)) {
                   rv = bcm_kt_mpls_label_stat_enable_set(unit, label,
                                                          port, enable);
               } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) 
               if (SOC_IS_TRIUMPH3(unit)) {
                   rv = bcm_tr3_mpls_label_stat_enable_set(unit, label,
                                                           port, enable);
               } else
#endif
               {
#if defined(BCM_TRIDENT2_SUPPORT) 
                   rv = td2_mpls_label_stat_enable_set(unit, label,
                                                           port, enable);
#endif
               }
               bcm_tr_mpls_unlock(unit);
    }
#endif
#if defined(BCM_MONTEREY_SUPPORT)
      else if(SOC_IS_MONTEREY(unit)) {
        /* Flex counters not supported in Monterey */
        return rv;
      }
#endif
#endif
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_mpls_label_stat_get
 * Purpose:
 *      Get L2 MPLS PW Stats
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 *      val    - (OUT) 64-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */     

int
_bcm_esw_mpls_label_stat_get(int unit, int sync_mode, 
                             bcm_mpls_label_t label, 
                             bcm_gport_t port, 
                             bcm_mpls_stat_t stat, 
                             uint64 *val)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter) &&
               soc_feature(unit, soc_feature_mpls)) {
               BCM_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
#if defined(BCM_KATANA_SUPPORT) 
               if (SOC_IS_KATANAX(unit)) {
                   rv = bcm_kt_mpls_label_stat_get(unit, sync_mode, 
                                                   label, port, 
                                                   stat, val);
               } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
               if (SOC_IS_TRIUMPH3(unit)) {
                   rv = bcm_tr3_mpls_label_stat_get(unit, sync_mode, 
                                                    label, port,
                                                    stat, val);
               } else
#endif
               {
#if defined(BCM_TRIDENT2_SUPPORT)
                   rv = td2_mpls_label_stat_get(unit, sync_mode, 
                                                label, port,
                                                stat, val);
#endif
               }
               bcm_tr_mpls_unlock(unit);
    } else
#endif
#if defined(BCM_MONTEREY_SUPPORT)
    if(SOC_IS_MONTEREY(unit)) {
        /* Flex counters not supported in Monterey */
        return rv;
    } else
#endif
    if ((SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) ||
        (soc_feature(unit, soc_feature_mpls) &&
         soc_feature(unit, soc_feature_gport_service_counters))) {
              rv = bcm_tr_mpls_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr_mpls_label_stat_get(unit, sync_mode, 
                                               label, port, 
                                               stat, val);
              bcm_tr_mpls_unlock (unit);
         }
        return rv;
    }
#endif
#endif
    return rv;
}


/*
 * Function:
 *      bcm_esw_mpls_label_stat_get
 * Purpose:
 *      Get L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 *      val    - (OUT) 64-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */     
int
bcm_esw_mpls_label_stat_get(int unit, bcm_mpls_label_t label, 
                            bcm_gport_t port, bcm_mpls_stat_t stat, 
                            uint64 *val)
{
    int rv = BCM_E_UNAVAIL;

    rv = _bcm_esw_mpls_label_stat_get(unit, 0, label, port, stat, val);

    return rv;
}


/*
 * Function:
 *      bcm_esw_mpls_label_stat_sync_get
 * Purpose:
 *      Get L2 MPLS PW Stats
 *      sw accumulated counters synced with hw count.
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 *      val    - (OUT) 64-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */     
int
bcm_esw_mpls_label_stat_sync_get(int unit, bcm_mpls_label_t label, 
                                 bcm_gport_t port, bcm_mpls_stat_t stat, 
                                 uint64 *val)
{
    int rv = BCM_E_UNAVAIL;

    rv = _bcm_esw_mpls_label_stat_get(unit, 1, label, port, stat, val);

    return rv;
}


/*
 * Function:
 *      bcm_esw_mpls_label_stat_get32
 * Purpose:
 *      Get L2 MPLS PW Stats
 * Parameters:
 *      unit      - (IN) SOC unit #
 *      sync_mode - (IN) hwcount is to be synced to sw count 
 *      label     - (IN) MPLS label
 *      port      - (IN) MPLS gport
 *      stat      - (IN)  specify the Stat type
 *      val       - (OUT) 64-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */     
int
_bcm_esw_mpls_label_stat_get32(int unit, int sync_mode, 
                               bcm_mpls_label_t label, 
                               bcm_gport_t port,
                               bcm_mpls_stat_t stat, 
                               uint32 *val)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter) &&
               soc_feature(unit, soc_feature_mpls)) {
               BCM_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
#if defined(BCM_KATANA_SUPPORT) 
               if (SOC_IS_KATANAX(unit)) {
                   rv = bcm_kt_mpls_label_stat_get32(unit, sync_mode, 
                                                     label, port,
                                                     stat, val);
               } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
               if (SOC_IS_TRIUMPH3(unit)) {
                   rv = bcm_tr3_mpls_label_stat_get32(unit, sync_mode, 
                                                      label, port,
                                                      stat, val);
               } else
#endif
               {
#if defined(BCM_TRIDENT2_SUPPORT)
                   rv = td2_mpls_label_stat_get32(unit, sync_mode, 
                                                  label, port,
                                                  stat, val);
#endif
               }
               bcm_tr_mpls_unlock(unit);
    } else
#endif
#if defined(BCM_MONTEREY_SUPPORT)
    if(SOC_IS_MONTEREY(unit)) {
        /* Flex counters not supported in Monterey */
        return rv;
    } else
#endif
    if ((SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) ||
        (soc_feature(unit, soc_feature_mpls) &&
         soc_feature(unit, soc_feature_gport_service_counters))) {
              rv = bcm_tr_mpls_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr_mpls_label_stat_get32(unit, sync_mode, 
                                               label, port, 
                                               stat, val);
              bcm_tr_mpls_unlock (unit);
         }
         return rv;
    }
#endif
#endif
    return rv;
}


int
bcm_esw_mpls_label_stat_get32(int unit, bcm_mpls_label_t label, 
                              bcm_gport_t port, 
                              bcm_mpls_stat_t stat, 
                              uint32 *val)
{
    int rv = BCM_E_UNAVAIL;

    rv = _bcm_esw_mpls_label_stat_get32(unit, 0, label, port, stat, val);

    return rv;
}


int
bcm_esw_mpls_label_stat_sync_get32(int unit, bcm_mpls_label_t label, 
                                   bcm_gport_t port, 
                                   bcm_mpls_stat_t stat, 
                                   uint32 *val)
{
    int rv = BCM_E_UNAVAIL;

    rv = _bcm_esw_mpls_label_stat_get32(unit, 1, label, port, stat, val);

    return rv;
}    

/*
 * Function:
 *      bcm_esw_mpls_label_stat_clear
 * Purpose:
 *      Clear L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 * Returns:
 *      BCM_E_XXX
 */     

int
bcm_esw_mpls_label_stat_clear(int unit, bcm_mpls_label_t label, bcm_gport_t port,
                            bcm_mpls_stat_t stat)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit,soc_feature_advanced_flex_counter) &&
               soc_feature(unit, soc_feature_mpls)) {
               BCM_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
#if defined(BCM_KATANA_SUPPORT) 
               if (SOC_IS_KATANAX(unit)) {
                   rv = bcm_kt_mpls_label_stat_clear(unit,label,port, stat);
               } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
              if (SOC_IS_TRIUMPH3(unit)) {
                  rv = bcm_tr3_mpls_label_stat_clear(unit,label,port,stat);
              } else
#endif
              {
#if defined(BCM_TRIDENT2_SUPPORT)
                  rv = td2_mpls_label_stat_clear(unit,label,port,stat);
#endif
              }
              bcm_tr_mpls_unlock(unit);
    } else
#endif
#if defined(BCM_MONTEREY_SUPPORT)
    if(SOC_IS_MONTEREY(unit)) {
        /* Flex counters not supported in Monterey */
        return rv;
    } else
#endif
    if ((SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) ||
        (soc_feature(unit, soc_feature_mpls) &&
         soc_feature(unit, soc_feature_gport_service_counters))) {
              rv = bcm_tr_mpls_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr_mpls_label_stat_clear(unit, label, port, stat);
              bcm_tr_mpls_unlock (unit);
         }
         return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_mpls_trunk_member_add
 * Purpose:
 *      Set Mpls state for trunk members joining a trunk group.
 * Parameters:
 *     unit  - (IN) Device Number
 *     trunk_id - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be added
 *     trunk_member_array - (IN) Trunk member ports to be added
 * Returns:
 *      BCM_E_XXX
 */     
int
_bcm_esw_mpls_trunk_member_add(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_mpls)) {
        rv = bcm_tr_mpls_trunk_member_add(unit, trunk_id,
                 trunk_member_count, trunk_member_array);
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_mpls_trunk_member_delete
 * Purpose:
 *      Clear Mpls state for trunk members leaving a trunk group.
 * Parameters:
 *     unit  - (IN) Device Number
 *     trunk_id - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be deleted
 *     trunk_member_array - (IN) Trunk member ports to be deleted
 * Returns:
 *      BCM_E_XXX
 */     
int
_bcm_esw_mpls_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_mpls)) {
        rv = bcm_tr_mpls_trunk_member_delete(unit, trunk_id,
                 trunk_member_count, trunk_member_array);
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_mpls_match_add
 * Purpose:
 *      Assign match criteria of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (IN) mpls port information
 *      vp  - (IN) Source Virtual Port
 *      vpn - (IN) VPN
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_mpls_match_add(int unit, bcm_mpls_port_t *mpls_port, int vp, int vpn,
                        int hw_update)
{
int rv = BCM_E_UNAVAIL;

#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = _bcm_tr3_mpls_match_add(unit, mpls_port, vp, hw_update);
         return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
         rv = _bcm_tr_mpls_match_add(unit, mpls_port, vp, vpn, hw_update);
         return rv;
    }
#endif
#endif
    return rv;

}

/*
 * Function:
 *      _bcm_esw_mpls_match_delete
 * Purpose:
 *      Delete match criteria of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_mpls_match_delete(int unit, int vp, int sw_clear)
{
int rv = BCM_E_UNAVAIL;

#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = _bcm_tr3_mpls_match_delete(unit, vp, sw_clear);
         return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
         rv = _bcm_tr_mpls_match_delete(unit, vp, NULL, sw_clear);
         return rv;
    }
#endif
#endif
    return rv;

}

/*
 * Function:
 *  _bcm_esw_mpls_match_failover_update
 * Purpose:
 *  Search mpls entry with provided source vp
 *  and replace with failover vp
 * Parameters:
 *  unit    - (IN) Device Number
 *  vp      - (IN) Source Virtual Port
 *  fo_vp   - (IN) Failover Virtual Port
 * Returns:
 *  BCM_E_XXX
 */

int
_bcm_esw_mpls_match_failover_update(int unit, int vp, int fo_vp)
{
    int rv = BCM_E_NONE;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_METROLITE(unit)) {
        rv = _bcm_tr_mpls_match_failover_update(unit, vp, fo_vp);
        return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_mpls_failover_nw_port_match_get
 * Purpose:
 *      Get match criteria of an MPLS port
 * Parameters:
 *      unit        - (IN) Device Number
 *      mpls_port   - (IN) mpls port
 *      vp          - (IN) Source Virtual Port
 *      return_ment - (OUT) matched mpls entry.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_mpls_failover_nw_port_match_get(int unit, bcm_mpls_port_t *mpls_port,
                                    int vp, mpls_entry_entry_t *return_ment)

{
int rv = BCM_E_NOT_FOUND;

#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = _bcm_tr3_mpls_failover_nw_port_match_get(unit, mpls_port, vp,
                  return_ment);
         return rv;
    }
#endif
#if defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_xgs5_nw_port_match)) {
        rv = bcmi_xgs5_mpls_failover_nw_port_match_get(unit, mpls_port, vp,
                  return_ment);
        return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
if (SOC_IS_TR_VL(unit)) {
    rv = _bcm_tr_mpls_failover_nw_port_match_get(unit, mpls_port, vp,
                    return_ment);
        return rv;
    }
#endif
#endif

    return rv;
}

/*
 * Function:
 *      _bcm_esw_mpls_match_get
 * Purpose:
 *      Get match criteria of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_mpls_match_get(int unit, bcm_mpls_port_t *mpls_port, int vp)
{
int rv = BCM_E_UNAVAIL;

#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = _bcm_tr3_mpls_match_get(unit, mpls_port, vp);
         return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
         rv = _bcm_tr_mpls_match_get(unit, mpls_port, vp);
         return rv;
    }
#endif
#endif
    return rv;

}

/*
 * Function:
 *      _bcm_mpls_tunnel_switch_delete_all
 * Purpose:
 *      Delete all MPLS label entries.
 * Parameters:
 *      unit   - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_mpls_tunnel_switch_delete_all (int unit)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && soc_feature(unit, soc_feature_mpls)) {
         rv = bcm_tr3_mpls_tunnel_switch_delete_all(unit);
         return rv;
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit) && soc_feature(unit, soc_feature_mpls)) {
         rv = bcm_kt_mpls_tunnel_switch_delete_all(unit);
         return rv;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls)) {
#if (defined(BCM_TOMAHAWK3_SUPPORT))
                  if (SOC_IS_TOMAHAWK3(unit)) {
                      rv = bcmi_xgs5_mpls_tunnel_switch_delete_all(unit);
                  } else
#endif
                  {
                      rv = bcm_tr_mpls_tunnel_switch_delete_all(unit);
                  }
         return rv;
    }
#endif
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_port_stat_attach
 * Description:
 *      Attach counters entries to the given mpls gport and vpn 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_port_stat_attach(
            int         unit, 
            bcm_vpn_t   vpn, 
            bcm_gport_t port, 
            uint32      stat_counter_id)
{
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_port_stat_attach(
               unit,vpn,port,stat_counter_id);
    } else 
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_port_stat_attach(
               unit,vpn,port,stat_counter_id);
    }  else
#endif
    {
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_port_stat_attach(
                   unit,vpn,port,stat_counter_id);
        }
#endif
    }
#if defined(BCM_MPLS_SUPPORT)
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_TRIDENT(unit)) {
        if (soc_feature(unit, soc_feature_gport_service_counters) &&
            soc_feature(unit, soc_feature_mpls)) {
            _bcm_flex_stat_type_t fs_type;
            uint32 fs_inx;
            uint32 flag;
            int rv;

            fs_type = _BCM_FLEX_STAT_TYPE(stat_counter_id);
            fs_inx  = _BCM_FLEX_STAT_COUNT_INX(stat_counter_id);
            if (((fs_type != _bcmFlexStatTypeEgressGport) && 
                 (fs_type != _bcmFlexStatTypeGport)) || (!fs_inx)) {
                return BCM_E_PARAM;
            }
            flag = fs_type==_bcmFlexStatTypeGport? _BCM_FLEX_STAT_HW_INGRESS:
                         _BCM_FLEX_STAT_HW_EGRESS;

            rv = _bcm_esw_flex_stat_enable_set(unit, fs_type,
                             _bcm_esw_port_flex_stat_hw_index_set,
                                      INT_TO_PTR(flag),
                                      port, TRUE,fs_inx);
            return rv;
        }
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_port_stat_detach
 * Description:
 *      Detach counters entries to the given mpls port and vpn 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_port_stat_detach(
            int         unit, 
            bcm_vpn_t   vpn, 
            bcm_gport_t port)
{
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_port_stat_detach(unit,vpn,port);
    } else
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_port_stat_detach(unit,vpn,port);
    } else
#endif
    {
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_port_stat_detach(unit,vpn,port);
        }
#endif
    }
#if defined(BCM_MPLS_SUPPORT)
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_TRIDENT(unit)) {
        if (soc_feature(unit, soc_feature_gport_service_counters) &&
            soc_feature(unit, soc_feature_mpls)) {
            uint32 flag;
            int rv;
            int rv1;

            flag= _BCM_FLEX_STAT_HW_INGRESS;
            rv = _bcm_esw_flex_stat_enable_set(unit, _bcmFlexStatTypeGport,
                             _bcm_esw_port_flex_stat_hw_index_set,
                                      INT_TO_PTR(flag),
                                      port, FALSE,1);
            flag= _BCM_FLEX_STAT_HW_EGRESS;
            rv1 = _bcm_esw_flex_stat_enable_set(unit,
                         _bcmFlexStatTypeEgressGport,
                         _bcm_esw_port_flex_stat_hw_index_set,
                                  INT_TO_PTR(flag),
                                  port, FALSE,1);
            if (BCM_SUCCESS(rv) && BCM_SUCCESS(rv1)) {
                return BCM_E_NONE;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_mpls_port_stat_counter_get
 * Description:
 *      Get counter statistic values for specific vpn and gport 
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int 
_bcm_esw_mpls_port_stat_counter_get(
            int                  unit, 
            int                  sync_mode, 
            bcm_vpn_t            vpn, 
            bcm_gport_t          port, 
            bcm_mpls_stat_t      stat, 
            uint32               num_entries, 
            uint32               *counter_indexes, 
            bcm_stat_value_t     *counter_values)
{
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_port_stat_counter_get(
               unit, sync_mode, vpn, port, stat, num_entries,
               counter_indexes,counter_values);
    } else
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_port_stat_counter_get(
               unit, sync_mode, vpn, port, stat, num_entries, 
               counter_indexes,counter_values);
    } else
#endif
    {
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_port_stat_counter_get(
               unit, sync_mode, vpn, port, stat, num_entries, 
               counter_indexes,counter_values);
        }
#endif
    }
#if defined(BCM_MPLS_SUPPORT)
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if ((SOC_IS_TRIUMPH2(unit) || SOC_IS_TRIDENT(unit)) &&
        soc_feature(unit, soc_feature_mpls) &&
        soc_feature(unit, soc_feature_gport_service_counters)) {
        uint64 val;
        int rv = BCM_E_NONE;
        bcm_port_stat_t flex_stat=0;

        switch (stat) {
            case bcmMplsInPkts:
                flex_stat = bcmPortStatIngressPackets;
                break;
            case bcmMplsInBytes:
                flex_stat = bcmPortStatIngressBytes;
                break;
            case bcmMplsOutPkts:
                flex_stat = bcmPortStatEgressPackets;
                break;
            case bcmMplsOutBytes:
                flex_stat = bcmPortStatEgressBytes;
                break;
            default:
                return BCM_E_PARAM;
                break;
            }

        rv =  _bcm_esw_flex_stat_get(unit, 0,
               ((flex_stat == bcmPortStatIngressPackets) ||
                (flex_stat == bcmPortStatIngressBytes)) ?
                 _bcmFlexStatTypeGport:_bcmFlexStatTypeEgressGport,
                port, (_bcm_flex_stat_t)flex_stat, &val);

        if ((flex_stat == bcmPortStatIngressPackets) ||
            (flex_stat == bcmPortStatEgressPackets)) {
            counter_values->packets = COMPILER_64_LO(val);
        } else {
            COMPILER_64_SET(counter_values->bytes,
                      COMPILER_64_HI(val),
                      COMPILER_64_LO(val));
        }
        return rv;
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_port_stat_counter_get
 * Description:
 *      Get counter statistic values for specific vpn and gport 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int 
bcm_esw_mpls_port_stat_counter_get(int                  unit, 
                                   bcm_vpn_t            vpn, 
                                   bcm_gport_t          port, 
                                   bcm_mpls_stat_t      stat, 
                                   uint32               num_entries, 
                                   uint32               *counter_indexes, 
                                   bcm_stat_value_t     *counter_values)
{
    return _bcm_esw_mpls_port_stat_counter_get(unit, 0,  vpn, port, stat, 
                                               num_entries, 
                                               counter_indexes, 
                                               counter_values);
}

/*
 * Function:
 *      bcm_esw_mpls_port_stat_counter_sync_get
 * Description:
 *      Get counter statistic values for specific vpn and gport 
 *      sync the sw accumulated count with hw count value 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */


int 
bcm_esw_mpls_port_stat_counter_sync_get(int                  unit, 
                                        bcm_vpn_t            vpn, 
                                        bcm_gport_t          port, 
                                        bcm_mpls_stat_t      stat, 
                                        uint32               num_entries, 
                                        uint32               *counter_indexes, 
                                        bcm_stat_value_t     *counter_values)
{
    return _bcm_esw_mpls_port_stat_counter_get(unit, 1,  vpn, port, stat, 
                                               num_entries, 
                                               counter_indexes, 
                                              counter_values);
}


/*
 * Function:
 *      bcm_esw_mpls_port_stat_counter_set
 * Description:
 *      Set counter statistic values for specific vpn and gport 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_port_stat_counter_set(
            int                  unit, 
            bcm_vpn_t            vpn, 
            bcm_gport_t          port, 
            bcm_mpls_stat_t      stat, 
            uint32               num_entries, 
            uint32               *counter_indexes, 
            bcm_stat_value_t     *counter_values)
{
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_port_stat_counter_set(
               unit,vpn,port,stat,num_entries,counter_indexes,counter_values);
    } else
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_port_stat_counter_set(
               unit,vpn,port,stat,num_entries,counter_indexes,counter_values);
    } else
#endif
    {
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_port_stat_counter_set(
               unit,vpn,port,stat,num_entries,counter_indexes,counter_values);
        }
#endif
    }
#if defined(BCM_MPLS_SUPPORT)
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if ((SOC_IS_TRIUMPH2(unit) || SOC_IS_TRIDENT(unit)) &&
        soc_feature(unit, soc_feature_mpls) &&
        soc_feature(unit, soc_feature_gport_service_counters)) {
        uint64 val;
        int rv = BCM_E_NONE;
        bcm_port_stat_t flex_stat=0;

        switch (stat) {
            case bcmMplsInPkts:
                flex_stat = bcmPortStatIngressPackets;
                break;
            case bcmMplsInBytes:
                flex_stat = bcmPortStatIngressBytes;
                break;
            case bcmMplsOutPkts:
                flex_stat = bcmPortStatEgressPackets;
                break;
            case bcmMplsOutBytes:
                flex_stat = bcmPortStatEgressBytes;
                break;
            default:
                return BCM_E_PARAM;
                break;
            }

        if ((flex_stat == bcmPortStatIngressPackets) ||
            (flex_stat == bcmPortStatEgressPackets)) {
            COMPILER_64_SET(val,0,counter_values->packets);
        } else {
            COMPILER_64_SET(val,
                      COMPILER_64_HI(counter_values->bytes),
                      COMPILER_64_LO(counter_values->bytes));
        }
        rv =  _bcm_esw_flex_stat_set(unit,
               ((flex_stat == bcmPortStatIngressPackets) ||
                (flex_stat == bcmPortStatIngressBytes)) ?
                 _bcmFlexStatTypeGport:_bcmFlexStatTypeEgressGport,
                port, (_bcm_flex_stat_t) flex_stat, val);
        return rv;
    }
#endif
#endif 
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_esw_mpls_port_stat_id_get
 * Description:
 *      Get stat counter id associated with specific vpn and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_port_stat_id_get(
            int                  unit,
            bcm_vpn_t            vpn, 
            bcm_gport_t          port, 
            bcm_mpls_stat_t      stat, 
            uint32               *stat_counter_id)
{
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_port_stat_id_get(
               unit,vpn,port,stat,stat_counter_id);
    } else
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_port_stat_id_get(
               unit,vpn,port,stat,stat_counter_id);
    } else
#endif
    {
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_port_stat_id_get(
               unit,vpn,port,stat,stat_counter_id);
        }
#endif
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_label_stat_attach
 * Description:
 *      Attach counters entries to the given mpls label and gport 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label  
 *      port             - (IN) MPLS Gport  
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_label_stat_attach(
            int              unit, 
            bcm_mpls_label_t label, 
            bcm_gport_t      port, 
            uint32           stat_counter_id)
{
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_label_stat_attach(
               unit,label,port,stat_counter_id);
    } else 
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_label_stat_attach(
               unit,label,port,stat_counter_id);
    } else
#endif
    {
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_label_stat_attach(
               unit,label,port,stat_counter_id);
        }
#endif
    } 
#if defined(BCM_MPLS_SUPPORT)
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_TRIDENT(unit)) {
        if (soc_feature(unit, soc_feature_gport_service_counters) &&
            soc_feature(unit, soc_feature_mpls)) {
            _bcm_flex_stat_type_t fs_type;
            uint32 fs_inx;
            int rv;

            fs_type = _BCM_FLEX_STAT_TYPE(stat_counter_id);
            fs_inx  = _BCM_FLEX_STAT_COUNT_INX(stat_counter_id);
            if ((fs_type != _bcmFlexStatTypeMplsLabel) || (!fs_inx)) {
                return BCM_E_PARAM;
            }

            rv = bcm_tr_mpls_lock(unit);
            if (BCM_E_NONE == rv) {
                rv = bcm_tr2_mpls_label_stat_enable_set(unit, label,
                                                    port, TRUE, fs_inx);
                bcm_tr_mpls_unlock(unit);
            }
            return rv;
        }
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_esw_mpls_label_stat_detach
 * Description:
 *      Detach counters entries to the given mpls label and gport 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label  
 *      port             - (IN) MPLS Gport  
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_label_stat_detach(
            int              unit, 
            bcm_mpls_label_t label, 
            bcm_gport_t      port)
{
#if defined(BCM_KATANA_SUPPORT) 
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_label_stat_detach(
               unit,label,port);
    } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_label_stat_detach(
               unit,label,port);
    } else
#endif
    {
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_label_stat_detach(
                   unit,label,port);
        } 
#endif
    }
#if defined(BCM_MPLS_SUPPORT)
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_TRIDENT(unit)) {
        if (soc_feature(unit, soc_feature_gport_service_counters) &&
            soc_feature(unit, soc_feature_mpls)) {
            int rv;

            rv = bcm_tr_mpls_lock(unit);
            if (BCM_E_NONE == rv) {
                rv = bcm_tr2_mpls_label_stat_enable_set(unit, label,
                                                    port, FALSE, 1);
                bcm_tr_mpls_unlock(unit);
            }
            return rv;
        }
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_mpls_label_stat_counter_get
 * Description:
 *      Get counter statistic values for specific MPLS label and gport 
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      label            - (IN) MPLS Label  
 *      port             - (IN) MPLS Gport  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_mpls_label_stat_counter_get(int                  unit, 
                                     int                  sync_mode, 
                                     bcm_mpls_label_t     label, 
                                     bcm_gport_t          port, 
                                     bcm_mpls_stat_t      stat, 
                                     uint32               num_entries, 
                                     uint32               *counter_indexes, 
                                     bcm_stat_value_t     *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) 
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_label_stat_counter_get(
               unit, sync_mode, label, port, stat, num_entries,
               counter_indexes,counter_values);
    } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_label_stat_counter_get(
               unit, sync_mode, label, port, stat,
               num_entries, counter_indexes, counter_values);
    } else
#endif
    {
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_label_stat_counter_get(
               unit, sync_mode, label, port, stat, 
               num_entries, counter_indexes, counter_values);
        }
#endif
    }
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_mpls) &&
        soc_feature(unit, soc_feature_gport_service_counters)) {
        uint64 val;
        int rv;

        rv = bcm_tr_mpls_lock(unit);
        if (rv == BCM_E_NONE ) {
            rv = bcm_tr_mpls_label_stat_get(unit, sync_mode, label, 
                                            port, stat, &val);
            bcm_tr_mpls_unlock (unit);
            if ((stat == bcmMplsInPkts) || (stat == bcmMplsOutPkts)) {
                 counter_values->packets = COMPILER_64_LO(val);
            } else {
                 COMPILER_64_SET(counter_values->bytes,
                           COMPILER_64_HI(val),
                           COMPILER_64_LO(val));
            }
        }
        return rv;
    }
#endif
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_mpls_label_stat_counter_get
 * Description:
 *      Get counter statistic values for specific MPLS label and gport 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label  
 *      port             - (IN) MPLS Gport  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_mpls_label_stat_counter_get(int                  unit, 
                                    bcm_mpls_label_t     label, 
                                    bcm_gport_t          port, 
                                    bcm_mpls_stat_t      stat, 
                                    uint32               num_entries, 
                                    uint32               *counter_indexes, 
                                    bcm_stat_value_t     *counter_values)
{
    return _bcm_esw_mpls_label_stat_counter_get(unit, 0, label, port, stat,
                                                num_entries, counter_indexes,
                                                counter_values);    
}

/*
 * Function:
 *      bcm_esw_mpls_label_stat_counter_sync_get
 * Description:
 *      Get counter statistic values for specific MPLS label and gport 
 *      sync the sw accumulated count with hw count value 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label  
 *      port             - (IN) MPLS Gport  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_mpls_label_stat_counter_sync_get(int                  unit, 
                                         bcm_mpls_label_t     label, 
                                         bcm_gport_t          port, 
                                         bcm_mpls_stat_t      stat, 
                                         uint32               num_entries, 
                                         uint32               *counter_indexes,
                                         bcm_stat_value_t     *counter_values)
{
    return _bcm_esw_mpls_label_stat_counter_get(unit, 1, label, port, stat,
                                                num_entries, counter_indexes,
                                                counter_values);    
}
/*
 * Function:
 *      bcm_esw_mpls_label_stat_counter_set
 * Description:
 *      Set counter statistic values for specific MPLS label and gport 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label  
 *      port             - (IN) MPLS Gport  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_label_stat_counter_set(
            int                  unit, 
            bcm_mpls_label_t     label, 
            bcm_gport_t          port, 
            bcm_mpls_stat_t      stat, 
            uint32               num_entries, 
            uint32               *counter_indexes, 
            bcm_stat_value_t     *counter_values)
{
#if defined(BCM_KATANA_SUPPORT) 
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_label_stat_counter_set(
               unit,label,port,stat,num_entries,counter_indexes,counter_values);
    } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_label_stat_counter_set(
               unit,label,port,stat,num_entries,counter_indexes,counter_values);
    } else
#endif
    {
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_label_stat_counter_set(
               unit,label,port,stat,num_entries,counter_indexes,counter_values);
        }
#endif
    }
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_esw_mpls_label_stat_id_get
 * Description:
 *      Get stat counter id associated with specific MPLS label and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label  
 *      port             - (IN) MPLS Gport  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_label_stat_id_get(
            int                  unit,
            bcm_mpls_label_t     label, 
            bcm_gport_t          port, 
            bcm_mpls_stat_t      stat, 
            uint32               *stat_counter_id)
{
#if defined(BCM_KATANA_SUPPORT) 
    if (SOC_IS_KATANAX(unit)) {
        return bcm_kt_mpls_label_stat_id_get(
               unit,label,port,stat,stat_counter_id);
    } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_mpls_label_stat_id_get(
               unit,label,port,stat,stat_counter_id);
    } else
#endif
    {
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return td2_mpls_label_stat_id_get(
               unit,label,port,stat,stat_counter_id);
        }
#endif
    }
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_esw_mpls_special_label_identifier_add
 * Description:
 *      Add identifier entry of special label.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_identifier_add(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_identifier_add(unit, label_type,
                label_info);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/

    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_special_label_identifier_get
 * Description:
 *      Get identifier entry of special label.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_identifier_get(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t *label_info)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_identifier_get(unit, label_type,
                label_info);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/

    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_special_label_identifier_delete
 * Description:
 *      Delete identifiers of special label.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_identifier_delete(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_identifier_delete(unit, label_type,
                label_info);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/

    return rv;

}

/*
 * Function:
 *      bcm_esw_mpls_special_label_identifier_delete_all
 * Description:
 *      Delete all identifiers ofs pecial labels.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_identifier_delete_all(int unit)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_identifier_delete_all(unit);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/

    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_special_label_identifier_traverse
 * Description:
 *      Traverse and call user call back for all special labels identifiers.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      cb                  - (IN) user callback
 *      user data           - (IN) option user data
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_special_label_identifier_traverse(int unit,
            bcm_mpls_special_label_identifier_traverse_cb cb, void *user_data)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_identifier_traverse(unit, cb, user_data);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/

    return rv;
}
/*
 * Function:
 *      bcm_esw_mpls_special_label_egress_add
 * Description:
 *      Add special label in MPLS stack for packets
 *      going into MPLS tunnel.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_egress_add(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_egress_add(unit, label_type, label_info);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/

    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_special_label_egress_get
 * Description:
 *      Get special label in MPLS stack for packets
 *      going into MPLS tunnel.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_egress_get(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t *label_info)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_egress_get(unit, label_type, label_info);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/
    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_special_label_egress_delete
 * Description:
 *      Delete special label in MPLS stack for packets
 *      going into MPLS tunnel.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_egress_delete(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_egress_delete(unit, label_type, label_info);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/

    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_special_label_egress_delete_all
 * Description:
 *      Delete all special labels in MPLS stack for packets
 *      going into MPLS tunnel.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_egress_delete_all(int unit)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_egress_delete_all(unit);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/

    return rv;
}

/*
 * Function:
 *      bcm_esw_mpls_special_label_egress_traverse
 * Description:
 *      Traverse and call user call back for all special labels
 *      going into MPLS tunnel.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      cb                  - (IN) user callback
 *      user data           - (IN) option user data
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mpls_special_label_egress_traverse(int unit,
            bcm_mpls_special_label_egress_traverse_cb cb,void *user_data)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {
        rv = bcmi_mpls_special_label_egress_traverse(unit, cb, user_data);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/

    return rv;
}
/*
 * Function:
 *      bcm_esw_mpls_special_label_push_action_set
 * Description:
 *      Set mpls push action in MPLS objects.
 *      going into MPLS tunnel.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      element             - (IN) MPLS object
 *      push_type           - (IN) push action
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_push_action_set(int unit,
            bcm_mpls_special_label_push_element_t *element,
            int push_type)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {

        rv = bcmi_mpls_special_label_push_action_set(
                unit, element, (bcm_mpls_special_label_push_type_t)push_type);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/
    return rv;
}
/*
 * Function:
 *      bcm_esw_mpls_special_label_push_action_get
 * Description:
 *      Get mpls push action in MPLS objects.
 *      going into MPLS tunnel.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      element             - (IN) MPLS object
 *      push_type           - (OUT) push action
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_mpls_special_label_push_action_get(int unit,
            bcm_mpls_special_label_push_element_t *element,
            int *push_type)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
#ifdef BCM_SPECIAL_LABEL_SUPPORT

    if ((!soc_feature(unit, soc_feature_mpls)) ||
       (!(soc_feature(unit, soc_feature_mpls_special_label)))) {
        return BCM_E_UNAVAIL;
    }
    if (push_type == NULL) {
        return BCM_E_PARAM;
    }

    rv = bcm_tr_mpls_lock(unit);
    if (BCM_E_NONE == rv) {

        rv = bcmi_mpls_special_label_push_action_get(
                unit, element, (bcm_mpls_special_label_push_type_t *)push_type);
    }
    bcm_tr_mpls_unlock(unit);
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIDENT_SUPPORT */
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/
    return rv;
}

#else   /* INCLUDE_L3 */
typedef int bcm_esw_mpls_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

