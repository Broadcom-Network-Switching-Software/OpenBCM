/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ESW VXLAN API
 */

#if defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/util.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/vxlan.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident2.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/vxlan.h>
#endif
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/virtual.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */


#define VXLAN_INFO(_unit_)   (_bcm_td2_vxlan_bk_info[_unit_])

/*
 * Function:
 *    bcm_esw_vxlan_init
 * Purpose:
 *    Init  VXLAN module
 * Parameters:
 *    IN :  unit
 * Returns:
 *    BCM_E_XXX
 */

int
bcm_esw_vxlan_init(int unit)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_vxlan)) {
        return bcm_td2_vxlan_init(unit);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        return bcmi_gh2_vxlan_init(unit);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}


 /* Function:
 *      bcm_vxlan_cleanup
 * Purpose:
 *      Detach the VXLAN module, clear all HW states
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_vxlan_cleanup(int unit)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_vxlan)) {
        return bcm_td2_vxlan_cleanup(unit);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        return bcmi_gh2_vxlan_cleanup(unit);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}

 /* Function:
 *      bcm_esw_vxlan_vpn_create
 * Purpose:
 *      Createl L2-VPN instance
 * Parameters:
 *      unit     - Device Number
 *      info     - VXLAN VPN Config
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_vpn_create( int unit, bcm_vxlan_vpn_config_t *info)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }

         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_vpn_create(unit, info);
              bcm_td2_vxlan_unlock (unit);
         }

    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (info == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_vpn_create(unit, info);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_vpn_destroy
 * Purpose:
 *      Delete L2-VPN instance
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_vpn_destroy(int unit, bcm_vpn_t l2vpn)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_vpn_destroy(unit, l2vpn);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_vpn_destroy(unit, l2vpn);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_vxlan_vpn_destroy_all
 * Purpose:
 *      Delete all L2-VPN instances
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_vpn_destroy_all(int unit)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_vpn_destroy_all(unit);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_vpn_destroy_all(unit);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_vxlan_vpn_traverse
 * Purpose:
 *      VXLAN VPN Traverse
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb   - (IN)  User callback function, called once per entry
 *      user_data  -  (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXXX
 */
int 
bcm_esw_vxlan_vpn_traverse(int unit, bcm_vxlan_vpn_traverse_cb cb, 
                                                          void *user_data)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_vpn_traverse(unit, cb, user_data);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_vpn_traverse(unit, cb, user_data);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_vxlan_port_traverse
 * Purpose:
 *      VXLAN port Traverse
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb   - (IN)  User callback function, called once per entry
 *      user_data  -  (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXXX
 */
int 
bcm_esw_vxlan_port_traverse(int unit, bcm_vxlan_port_traverse_cb cb, 
                                                          void *user_data)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_port_traverse(unit, cb, user_data);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_port_traverse(unit, cb, user_data);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_vxlan_vpn_get
 * Purpose:
 *      Get L2-VPN instance
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 *      info     - VXLAN VPN Config
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_vpn_get( int unit, bcm_vpn_t l2vpn, bcm_vxlan_vpn_config_t *info)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_vpn_get(unit, l2vpn, info);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (info == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_vpn_get(unit, l2vpn, info);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_port_add
 * Purpose:
 *      Add Access/Network VXLAN port
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 *      vxlan_port     - VXLAN Gport
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_port_add(int unit, bcm_vpn_t l2vpn, bcm_vxlan_port_t *vxlan_port)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (vxlan_port == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_port_add(unit, l2vpn, vxlan_port);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (vxlan_port == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_port_add(unit, l2vpn, vxlan_port);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_port_delete
 * Purpose:
 *      Delete Access/Network VXLAN port
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 *      vxlan_port_id     - VXLAN Gport Id
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_port_delete( int unit, bcm_vpn_t l2vpn, bcm_gport_t vxlan_port_id)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_port_delete(unit, l2vpn, vxlan_port_id);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_port_delete(unit, l2vpn, vxlan_port_id);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_port_delete_all
 * Purpose:
 *      Delete all Access/Network VXLAN ports
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_port_delete_all(int unit, bcm_vpn_t l2vpn)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_port_delete_all(unit, l2vpn);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_port_delete_all(unit, l2vpn);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_port_get
 * Purpose:
 *      Get Access/Network VXLAN port info
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 *      vxlan_port     - VXLAN Gport
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_port_get(int unit, bcm_vpn_t l2vpn, bcm_vxlan_port_t *vxlan_port)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (vxlan_port == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_port_get(unit, l2vpn, vxlan_port);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (vxlan_port == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_port_get(unit, l2vpn, vxlan_port);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_port_get_all
 * Purpose:
 *      Get all Access/Network VXLAN port info
 * Parameters:
 *      unit     - (IN) Device Number
 *      l2vpn   - VXLAN VPN
 *      port_max   - (IN) Maximum number of VXLAN ports in array
 *      port_array - (OUT) Array of VXLAN ports
 *      port_count - (OUT) Number of VXLAN ports returned in array
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_port_get_all(
    int unit, 
    bcm_vpn_t l2vpn, 
    int port_max, 
    bcm_vxlan_port_t *port_array, 
    int *port_count)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (port_count == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_port_get_all(unit, l2vpn, port_max, port_array, port_count);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (port_count == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_port_get_all(unit, l2vpn, port_max, port_array, port_count);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_tunnel_initiator_create
 * Purpose:
 *      Create VXLAN Tunnel Initiator 
 * Parameters:
 *      unit     - Device Number
 *      info     - VXLAN Tunnel Initiator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_tunnel_initiator_create(unit, info);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (info == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_initiator_create(unit, info);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}


 /* Function:
 *      bcm_esw_vxlan_tunnel_initiator_destroy
 * Purpose:
 *      Delete VXLAN Tunnel Initiator 
 * Parameters:
 *      unit     - Device Number
 *      vxlan_tunnel_id     - VXLAN Tunnel Initiator Id
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_tunnel_initiator_destroy(int unit, bcm_gport_t vxlan_tunnel_id)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_tunnel_initiator_destroy(unit, vxlan_tunnel_id);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_initiator_destroy(unit, vxlan_tunnel_id);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_tunnel_initiator_destroy_all
 * Purpose:
 *      Delete all VXLAN Tunnel Initiators 
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_tunnel_initiator_destroy_all(int unit)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_tunnel_initiator_destroy_all(unit);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_initiator_destroy_all(unit);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_tunnel_initiator_get
 * Purpose:
 *      Get  VXLAN Tunnel Initiator Info
 * Parameters:
 *      unit     - Device Number
 *      info     - VXLAN Tunnel Initiator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_tunnel_initiator_get(unit, info);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (info == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_initiator_get(unit, info);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_vxlan_tunnel_initiator_traverse
 * Purpose:
 *      VXLAN Tunnel Initiator Traverse
 * Parameters:
 *      unit     - Device Number
 *      cb     - User callback function, called once per entry
 *      user_data     - Cookie
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_vxlan_tunnel_initiator_traverse(int unit, bcm_tunnel_initiator_traverse_cb cb, void *user_data)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
        rv = bcm_td2_vxlan_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2_vxlan_tunnel_initiator_traverse(unit, cb, user_data);
            bcm_td2_vxlan_unlock (unit);
        }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_initiator_traverse(unit, cb, user_data);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_tunnel_terminator_create
 * Purpose:
 *      Create VXLAN Tunnel terminator
 * Parameters:
 *      unit     - Device Number
 *      info     - VXLAN Tunnel terminator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_tunnel_terminator_create(int unit, bcm_tunnel_terminator_t *info)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_tunnel_terminator_create(unit, info);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (info == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_terminator_create(unit, info);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_tunnel_terminator_update
 * Purpose:
 *      Update VXLAN Tunnel terminator
 * Parameters:
 *      unit     - Device Number
 *      info     - VXLAN Tunnel terminator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_tunnel_terminator_update(int unit, bcm_tunnel_terminator_t *info)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_tunnel_terminator_update(unit, info);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (info == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_terminator_update(unit, info);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_tunnel_terminator_destroy
 * Purpose:
 *      Destroy VXLAN Tunnel terminator
 * Parameters:
 *      unit     - Device Number
 *      vxlan_tunnel_id     - VXLAN Tunnel Initiator Id
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_tunnel_terminator_destroy(int unit, bcm_gport_t vxlan_tunnel_id)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_tunnel_terminator_destroy(unit, vxlan_tunnel_id);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_terminator_destroy(unit, vxlan_tunnel_id);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_tunnel_terminator_destroy_all
 * Purpose:
 *      Destroy all VXLAN Tunnel terminators
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_tunnel_terminator_destroy_all(int unit)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_tunnel_terminator_destroy_all(unit);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_terminator_destroy_all(unit);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

 /* Function:
 *      bcm_esw_vxlan_tunnel_terminator_get
 * Purpose:
 *      Get  VXLAN Tunnel terminator Info
 * Parameters:
 *      unit     - Device Number
 *      info     - VXLAN Tunnel terminator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_vxlan_tunnel_terminator_get( int unit, bcm_tunnel_terminator_t *info)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_td2_vxlan_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_td2_vxlan_tunnel_terminator_get(unit, info);
              bcm_td2_vxlan_unlock (unit);
         }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (info == NULL) {
            return BCM_E_PARAM;
        }
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_terminator_get(unit, info);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_vxlan_tunnel_terminator_traverse
 * Purpose:
 *      VXLAN Tunnel Terminator Traverse
 * Parameters:
 *      unit     - Device Number
 *      cb     - User callback function, called once per entry
 *      user_data     - Cookie
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_vxlan_tunnel_terminator_traverse(int unit, bcm_tunnel_terminator_traverse_cb cb, void *user_data)
{
    int rv;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)) {
        rv = bcm_td2_vxlan_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2_vxlan_tunnel_terminator_traverse(unit, cb, user_data);
            bcm_td2_vxlan_unlock (unit);
        }
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcmi_gh2_vxlan_tunnel_terminator_traverse(unit, cb, user_data);
            bcmi_gh2_vxlan_unlock(unit);
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_vxlan_trunk_member_add
 * Purpose:
 *      Set VXLAN state for trunk members being added to a trunk group.
 * Parameters:
 *     unit  - (IN) Device Number
 *     trunk_id - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be added
 *     trunk_member_array - (IN) Trunk member ports to be added
 * Returns:
 *      BCM_E_XXX
 */     
int
_bcm_esw_vxlan_trunk_member_add(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array) 
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_vxlan)) {
        rv = bcm_td2_vxlan_trunk_member_add(unit, trunk_id,
                 trunk_member_count, trunk_member_array);
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_trill_trunk_member_delete
 * Purpose:
 *      Clear VXLAN state for trunk members leaving a trunk group.
 * Parameters:
 *     unit  - (IN) Device Number
 *     trunk_id - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be deleted
 *     trunk_member_array - (IN) Trunk member ports to be deleted
 * Returns:
 *      BCM_E_XXX
 */     
int
_bcm_esw_vxlan_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array) 
{
    int rv;

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_vxlan)) {
        rv = bcm_td2_vxlan_trunk_member_delete(unit, trunk_id,
                 trunk_member_count, trunk_member_array);
    } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        rv = bcmi_gh2_vxlan_trunk_member_delete(unit, trunk_id,
                                               trunk_member_count,
                                               trunk_member_array);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_vxlan_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a VXLAN virtual port.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      gport - (IN) VXLAN virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_vxlan_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)
        && soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_td2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2_vxlan_port_source_vp_lag_set(unit, gport, vp_lag_vp);
            bcm_td2_vxlan_unlock(unit);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_vxlan_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a VXLAN virtual port.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      gport - (IN) VXLAN virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_vxlan_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)
        && soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_td2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2_vxlan_port_source_vp_lag_clear(unit, gport, vp_lag_vp);
            bcm_td2_vxlan_unlock(unit);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_vxlan_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a VXLAN virtual port.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      gport - (IN) VXLAN virtual port GPORT ID. 
 *      vp_lag_vp - (OUT) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_vxlan_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan)
        && soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_td2_vxlan_lock(unit);
        if (rv == BCM_E_NONE) {
            rv = bcm_td2_vxlan_port_source_vp_lag_get(unit, gport, vp_lag_vp);
            bcm_td2_vxlan_unlock(unit);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return rv;
}

#if defined(BCM_TRIDENT2_SUPPORT)
/*
 * Function:
 *      _bcm_esw_vxlan_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given ingress interface.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
static 
bcm_error_t _bcm_esw_vxlan_stat_get_table_info(
            int                        unit,
            bcm_gport_t                port,
            bcm_vpn_t                  vpn,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
    int vp = 0;
    int vfi = 0;
    int vp_type = 0;
    int egress_tunnel_flag = 0;
    int nh_idx = -1;
    int entry_type = 0;
    _bcm_vp_info_t vp_info;
    int d = 0; /*vxlan decoupled mode*/

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        d = 1;
    }
#endif

    if (!(BCM_GPORT_INVALID == port) && BCM_GPORT_IS_VXLAN_PORT(port)) {
        egr_dvp_attribute_entry_t  egr_dvp_attribute;
        ing_dvp_table_entry_t dvp;
        egr_l3_next_hop_entry_t egr_nh;

        /* Ingress Vxlan VP */
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            return BCM_E_NOT_FOUND;
        }

        table_info[*num_of_tables].table=SOURCE_VPm;
        table_info[*num_of_tables].index=vp;
        table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
        (*num_of_tables)++;

        BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp,
                                        &dvp));

        BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
        if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
            egress_tunnel_flag = TRUE;
        }

        /* Network port*/
        if (egress_tunnel_flag) {
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, EGR_DVP_ATTRIBUTEm, MEM_BLOCK_ANY, 
                             vp, &egr_dvp_attribute));
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm, DATA_TYPEf)) {
                vp_type = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                            &egr_dvp_attribute, DATA_TYPEf);
            } else
#endif
            {
                vp_type = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                            &egr_dvp_attribute, VP_TYPEf);
            }

            if (vp_type == _BCM_VXLAN_EGRESS_DEST_VP_TYPE) { /* vxlan vp type */
                /* Egress Vxlan (DVP) */
#if defined(BCM_TRIDENT3_SUPPORT)
                if (!SOC_MEM_IS_VALID(unit, EGR_DVP_ATTRIBUTE_1m)) {
                    table_info[*num_of_tables].table = EGR_DVP_ATTRIBUTEm;
                } else 
#endif
                {
                    table_info[*num_of_tables].table=EGR_DVP_ATTRIBUTE_1m;
                }
                table_info[*num_of_tables].index=vp;
                table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
                (*num_of_tables)++;
            } 
        } else { /* Access port */
            nh_idx = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                             nh_idx, &egr_nh));
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                entry_type = soc_mem_field32_get(unit,
                    EGR_L3_NEXT_HOPm, &egr_nh, DATA_TYPEf);
            } else
#endif
            {
                entry_type = soc_mem_field32_get(unit,
                    EGR_L3_NEXT_HOPm, &egr_nh, ENTRY_TYPEf);
            }
            if (entry_type == BCM_VXLAN_TAG_VIEW(d)) { /* vxlan sd tag */
                /* Next hop for access */
                table_info[*num_of_tables].table=EGR_L3_NEXT_HOPm;
                table_info[*num_of_tables].index=nh_idx;
                table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
                (*num_of_tables)++;
            }
 
        }
    }

    if ( !(_BCM_VXLAN_VPN_INVALID == vpn) &&
        ((_BCM_VPN_IS_VXLAN_ELINE(vpn) || _BCM_VPN_IS_VXLAN_ELAN(vpn)))) {
        /* Ingress Vxlan VPN */
        _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVxlan)) {
            return BCM_E_NOT_FOUND;
        }

        table_info[*num_of_tables].table=VFIm;
        table_info[*num_of_tables].index=vfi;
        table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
        (*num_of_tables)++;

        /* Egress Vxlan VPN */
        if (vp_type != 2) { 
            table_info[*num_of_tables].table=EGR_VFIm;
            table_info[*num_of_tables].index=vfi;
            table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
            (*num_of_tables)++;
        }
    }

    if (*num_of_tables == 0) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRIDENT2_SUPPORT */

/*
 * Function:
 *      bcm_esw_vxlan_stat_attach
 * Description:
 *      Attach counters entries to the given  vxlan port and vpn. 
 *      Allocate flex counter and assign counter index and offset for the index 
 *      pointed by vxlan vpn(VFI) and port(tunnel)
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
    
int  bcm_esw_vxlan_stat_attach (
                            int                 unit,
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            uint32              stat_counter_id)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    int                        rv = BCM_E_NONE;
    int                        counter_flag = 0;
    soc_mem_t                  table[3]={0};
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object = bcmStatObjectIngPort;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;
    uint32                     count = 0;
    uint32                     actual_num_tables = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_VXLAN_FLEX_COUNTER_MAX_TABLES] = {{0, 0, 0}};
    uint32                     flex_table_index=0;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
        return BCM_E_PORT;
    }

    if ((BCM_GPORT_INVALID != port) && (BCM_VXLAN_VPN_INVALID != vpn)) {
        return BCM_E_PARAM;
    }

    /* Get pool, base index group mode and offset modes from stat counter id */
    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode, &object, &offset_mode, &pool_number, &base_index);

    /* Validate object and group mode */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit, object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, group_mode));

    /* Get Table index to attach flexible counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit, object, 3, &actual_num_tables, &table[0], &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_vxlan_stat_get_table_info(
                        unit, port, vpn, &num_of_tables, &table_info[0]));

    for (count = 0; count < num_of_tables; count++) {
        for (flex_table_index=0; 
             flex_table_index < actual_num_tables ; 
             flex_table_index++) {
            if ((table_info[count].direction == direction) &&
                (table_info[count].table == table[flex_table_index])) {
                if (direction == bcmStatFlexDirectionIngress) {
                    counter_flag = 1;
                    rv = _bcm_esw_stat_flex_attach_ingress_table_counters(
                           unit,
                           table_info[count].table,
                           table_info[count].index,
                           offset_mode,
                           base_index,
                           pool_number);
                    if(BCM_FAILURE(rv)) {
                        break;
                    }
                } else {
                    counter_flag = 1;
                    rv = _bcm_esw_stat_flex_attach_egress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index,
                         0,
                         offset_mode,
                         base_index,
                         pool_number);
                    if(BCM_FAILURE(rv)) {
                        break;
                    }
                } 
             }
        }
    }

    if (counter_flag == 0) {
        rv = BCM_E_NOT_FOUND;
    }
    return rv;
#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */
}


/*
 * Function:
 *      bcm_esw_vxlan_stat_detach
 * Description:
 *      Detach counter entries to the given  vxlan port and vpn. 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int   bcm_esw_vxlan_stat_detach (
                            int                 unit,
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn)
{

#if defined(BCM_TRIDENT2_SUPPORT)
    uint32                     count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_VXLAN_FLEX_COUNTER_MAX_TABLES];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t      err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
        return BCM_E_PORT;
    }

    /* Get the table index to be detached */
    BCM_IF_ERROR_RETURN(_bcm_esw_vxlan_stat_get_table_info(
                   unit, port, vpn, &num_of_tables, &table_info[0]));

    for (count = 0; count < num_of_tables; count++) {
        if (table_info[count].direction == bcmStatFlexDirectionIngress) {
            rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                    unit, table_info[count].table, table_info[count].index);
            /* remember the first error code, but allow loop finish */
            if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
            }
        } else  {
            rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                    unit, 0, table_info[count].table, table_info[count].index);
            /* remember the first error code, but allow loop finish */
            if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
            }
        }
    }

    /* In case of success on one direction, and return NOT_FOUND on the other,
       we still take it as a success */
    BCM_STAT_FLEX_DETACH_RETURN(err_code)

#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */
}

/*
 * Function:
 *      _bcm_esw_vxlan_stat_counter_get
 *
 * Description:
 *  Get Vxlan counter value for specified vxlan port and vpn
 *  if sync_mode is set, sync the sw accumulated count
 *  with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) Vxlan counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 int   
 _bcm_esw_vxlan_stat_counter_get(int                 unit, 
                                 int                 sync_mode, 
                                 bcm_gport_t         port,
                                 bcm_vpn_t           vpn,
                                 bcm_vxlan_stat_t    stat, 
                                 uint32              num_entries, 
                                 uint32              *counter_indexes, 
                                 bcm_stat_value_t    *counter_values)
{   

#if defined(BCM_TRIDENT2_SUPPORT)
    uint32                     table_count = 0;
    uint32                     index_count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     byte_flag = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_VXLAN_FLEX_COUNTER_MAX_TABLES];
    uint8 isEline=0xFF;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
        return BCM_E_PORT;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
         BCM_IF_ERROR_RETURN 
              (_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    }

    if ((num_entries > 0) &&
        ((NULL == counter_indexes) || (NULL == counter_values))) {
        return BCM_E_PARAM;
    }

    /* Get the flex counter direction - ingress/egress */
    if ((stat == bcmVxlanOutPackets) ||
        (stat == bcmVxlanOutBytes)) {
         direction = bcmStatFlexDirectionEgress;
    } else {
         direction = bcmStatFlexDirectionIngress;
    }
    if ((stat == bcmVxlanOutPackets) ||
        (stat == bcmVxlanInPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
   
    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_vxlan_stat_get_table_info(
                        unit, port, vpn, &num_of_tables, &table_info[0]));

    /* Get the flex counter for the attached table index */
    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                                      unit, sync_mode,
                                      table_info[table_count].index,
                                      table_info[table_count].table,
                                      0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }

    return BCM_E_NONE;
#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */

}

/*
 * Function:
 *      bcm_esw_vxlan_stat_counter_get
 *
 * Description:
 *  Get Vxlan counter value for specified vxlan port and vpn
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) Vxlan counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 int   
 bcm_esw_vxlan_stat_counter_get(int                 unit, 
                                bcm_gport_t         port,
                                bcm_vpn_t           vpn,
                                bcm_vxlan_stat_t    stat, 
                                uint32              num_entries, 
                                uint32              *counter_indexes, 
                                bcm_stat_value_t    *counter_values)
{   

 return _bcm_esw_vxlan_stat_counter_get(unit, 0, port, vpn, stat, num_entries, 
                                        counter_indexes, counter_values);

}

/*
 * Function:
 *      bcm_esw_vxlan_stat_counter_sync_get
 *
 * Description:
 *  Get Vxlan counter value for specified vxlan port and vpn
 *  sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) Vxlan counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 int   
 bcm_esw_vxlan_stat_counter_sync_get(int                 unit, 
                                     bcm_gport_t         port,
                                     bcm_vpn_t           vpn,
                                     bcm_vxlan_stat_t    stat, 
                                     uint32              num_entries, 
                                     uint32              *counter_indexes, 
                                     bcm_stat_value_t    *counter_values)
{   

 return _bcm_esw_vxlan_stat_counter_get(unit, 1, port, vpn, stat, num_entries, 
                                        counter_indexes, counter_values);

}

/*
 * Function:
 *      bcm_esw_vxlan_stat_counter_set
 *
 * Description:
 *  Set Vxlan counter value for specified vxlan port and vpn
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) Vxlan counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int   bcm_esw_vxlan_stat_counter_set(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            bcm_vxlan_stat_t    stat, 
                            uint32              num_entries, 
                            uint32              *counter_indexes, 
                            bcm_stat_value_t    *counter_values)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    uint32                     table_count=0;
    uint32                     index_count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_VXLAN_FLEX_COUNTER_MAX_TABLES];
    uint8 isEline=0xFF;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
        return BCM_E_PORT;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
         BCM_IF_ERROR_RETURN 
              (_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    }

    if ((num_entries > 0) &&
        ((NULL == counter_indexes) || (NULL == counter_values))) {
        return BCM_E_PARAM;
    }

    /* Get the flex counter direction - ingress/egress */
    if ((stat == bcmVxlanOutPackets) ||
        (stat == bcmVxlanOutBytes)) {
         direction = bcmStatFlexDirectionEgress;
    } else {
         direction = bcmStatFlexDirectionIngress;
    }
    if (stat == bcmVxlanOutPackets || stat == bcmVxlanInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
   
    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_vxlan_stat_get_table_info(
                        unit, port, vpn, &num_of_tables, &table_info[0]));

    /* Get the flex counter for the attached table index */
    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                      unit,
                                      table_info[table_count].index,
                                      table_info[table_count].table,
                                      0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */
}


/*
 * Function:
 *      bcm_vxlan_stat_enable_set
 * Description:
 *     Enable/disable collection of statistics on the indicated port and vpn. 
 * Parameters:
 *      unit            - (IN) unit number
 *      port            - (IN) Vxlan gport
 *      vpn             - (IN) vpn (VFI)
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *     if vpn = BCM_VXLAN_VPN_INVALID,collection of statistics on the indicated port.
 */
int   bcm_esw_vxlan_stat_enable_set(
                            int              unit, 
                            bcm_gport_t      port, 
                            bcm_vpn_t        vpn, 
                            int              enable)
{
#if defined(BCM_TRIDENT2_SUPPORT)
   uint32                     num_of_tables=0;
   uint32                     num_stat_counter_ids=0;
   bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
   bcm_stat_object_t          object=bcmStatObjectIngPort;
   uint32                     stat_counter_id[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION]={0};
   uint32                     num_entries=0;
   int                        index=0;

   if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
       return BCM_E_UNAVAIL;
   }
   
   /* Parameter validation checks */
   if (!SOC_UNIT_VALID(unit)) {
       return BCM_E_UNIT;
   }
   
   if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
       return BCM_E_PORT;
   }

   if ((BCM_GPORT_INVALID != port) && (BCM_VXLAN_VPN_INVALID != vpn)) {
       return BCM_E_PARAM;
   }

   BCM_IF_ERROR_RETURN(_bcm_esw_vxlan_stat_get_table_info(
                       unit,port,vpn,&num_of_tables,&table_info[0]));

   if (enable ) {
       for(index=0;index < num_of_tables ;index++) {
           if(table_info[index].direction == bcmStatFlexDirectionIngress) {
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
           BCM_IF_ERROR_RETURN(bcm_esw_vxlan_stat_attach(
                              unit,port,vpn,
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
       BCM_IF_ERROR_RETURN(bcm_esw_vxlan_stat_detach(unit,port,vpn));        
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
#else
    return BCM_E_UNAVAIL;   
#endif
}

/*
 * Function:
 *      bcm_esw_vxlan_stat_multi_get
 *
 * Description:
 *  Get Multiple Vxlan counter value for specified vxlan port and vpn for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int   bcm_esw_vxlan_stat_multi_get(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            int                 nstat, 
                            bcm_vxlan_stat_t    *stat_arr,
                            uint64              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};
    uint8 isEline=0xFF;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
        return BCM_E_PORT;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
         BCM_IF_ERROR_RETURN 
              (_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    }

    if ((nstat > 0) && ((NULL == stat_arr) || (NULL == value_arr))) {
        return BCM_E_PARAM;
    }

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         BCM_IF_ERROR_RETURN(bcm_esw_vxlan_stat_counter_get( 
                             unit, port, vpn, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmVxlanOutPackets) || 
            (stat_arr[idx] == bcmVxlanInPackets)) {
             COMPILER_64_SET(value_arr[idx],
                             COMPILER_64_HI(counter_values.packets64),
                             COMPILER_64_LO(counter_values.packets64));
         } else {
             COMPILER_64_SET(value_arr[idx],
                             COMPILER_64_HI(counter_values.bytes),
                             COMPILER_64_LO(counter_values.bytes));
         }
    }
    return rv;
#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */

}

/*
 * Function:
 *      bcm_esw_vxlan_stat_multi_get32
 *
 * Description:
 *  Get 32bit Vxlan counter value for specified vxlan port and vpn for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int   bcm_esw_vxlan_stat_multi_get32(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            int                 nstat, 
                            bcm_vxlan_stat_t    *stat_arr,
                            uint32              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};
    uint8 isEline=0xFF;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
        return BCM_E_PORT;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
         BCM_IF_ERROR_RETURN 
              (_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    }

    if ((nstat > 0) &&
        ((NULL == stat_arr) || (NULL == value_arr))) {
        return BCM_E_PARAM;
    }

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         BCM_IF_ERROR_RETURN(bcm_esw_vxlan_stat_counter_get( 
                             unit, port, vpn, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmVxlanOutPackets) || 
            (stat_arr[idx] == bcmVxlanInPackets)) {
                value_arr[idx] = counter_values.packets;
         } else {
             value_arr[idx] = COMPILER_64_LO(counter_values.bytes);
         }
    }
    return rv;
#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */
}

/*
 * Function:
 *      bcm_esw_vxlan_stat_multi_set
 *
 * Description:
 *  Set Vxlan counter value for specified vxlan port and vpn for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) Vxlan counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int  bcm_esw_vxlan_stat_multi_set(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            int                 nstat, 
                            bcm_vxlan_stat_t    *stat_arr,
                            uint64              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};
    uint8 isEline=0xFF;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
        return BCM_E_PORT;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
         BCM_IF_ERROR_RETURN 
              (_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    }

    if ((nstat > 0) && ((NULL == stat_arr) || (NULL == value_arr))) {
        return BCM_E_PARAM;
    }

    /* Iterate f all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         if ((stat_arr[idx] == bcmVxlanOutPackets) || 
            (stat_arr[idx] == bcmVxlanInPackets)) {
              counter_values.packets = COMPILER_64_LO(value_arr[idx]);
         } else {
              COMPILER_64_SET(counter_values.bytes,
                              COMPILER_64_HI(value_arr[idx]),
                              COMPILER_64_LO(value_arr[idx]));
         }
         BCM_IF_ERROR_RETURN(bcm_esw_vxlan_stat_counter_set( 
                             unit, port, vpn, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
    }
    return rv;
#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */
}

/*
 * Function:
 *      bcm_esw_vxlan_stat_multi_set32
 *
 * Description:
 *  Set 32bit Vxlan counter value for specified vxlan port and vpn for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) Vxlan counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int   bcm_esw_vxlan_stat_multi_set32(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            int                 nstat, 
                            bcm_vxlan_stat_t    *stat_arr,
                            uint32              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};
    uint8 isEline=0xFF;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
        return BCM_E_PORT;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
         BCM_IF_ERROR_RETURN 
              (_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    }

    if ((nstat > 0) && ((NULL == stat_arr) || (NULL == value_arr))) {
        return BCM_E_PARAM;
    }

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         if ((stat_arr[idx] == bcmVxlanOutPackets) || 
            (stat_arr[idx] == bcmVxlanInPackets)) {
             counter_values.packets = value_arr[idx];
         } else {
             COMPILER_64_SET(counter_values.bytes,0,value_arr[idx]);
         }
         BCM_IF_ERROR_RETURN(bcm_esw_vxlan_stat_counter_set( 
                             unit, port, vpn, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
    }
    return rv;
#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */
}

/*
 * Function: 
 *      bcm_esw_vxlan_stat_id_get
 *
 * Description: 
 *      Get Stat Counter if associated with given vxlan port and vpn
 *
 * Parameters: 
 *      unit             - (IN) bcm device
 *      port             - (IN) Vxlan gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) Vxlan counter stat types
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */

int   bcm_esw_vxlan_stat_id_get(
                            int                 unit,
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            bcm_vxlan_stat_t    stat, 
                            uint32              *stat_counter_id) 
{
#if defined(BCM_TRIDENT2_SUPPORT)
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_VXLAN_FLEX_COUNTER_MAX_TABLES];
    uint32                     index = 0;
    uint32                     num_stat_counter_ids = 0;
    uint8 isEline=0xFF;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ((BCM_GPORT_INVALID != port) && !(BCM_GPORT_IS_VXLAN_PORT(port))) {
        return BCM_E_PORT;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
         BCM_IF_ERROR_RETURN 
              (_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    }

    if (NULL == stat_counter_id) {
        return BCM_E_PARAM;
    }

    if ((stat == bcmVxlanOutPackets) ||
        (stat == bcmVxlanOutBytes)) {
         direction = bcmStatFlexDirectionEgress;
    } else {
         direction = bcmStatFlexDirectionIngress;
    }

    /*  Get Tables, for which flex counter are attached  */
    BCM_IF_ERROR_RETURN(_bcm_esw_vxlan_stat_get_table_info(
                        unit, port, vpn, &num_of_tables, &table_info[0]));

    /* Retrieve stat counter id */
    for (index = 0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids, stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */
}

/*
 * Function:
 *      bcm_esw_vxlan_dip_stat_attach
 * Description:
 *  Attach counters entries to the given VXLAN DIP.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip        - (IN) vxlan dip 
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 */
bcm_error_t bcm_esw_vxlan_dip_stat_attach(
            int unit,
            bcm_ip_t vxlan_dip,
            uint32 stat_counter_id)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        return _bcm_td2_vxlan_dip_stat_attach(unit, vxlan_dip, stat_counter_id);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vxlan_dip_stat_detach
 * Description:
 *      Detach counters entries to the given vxlan dip
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip        - (IN) vxlan dip
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_esw_vxlan_dip_stat_detach(
            int unit,
            bcm_ip_t vxlan_dip)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_vxlan_dip_stat_detach(unit, vxlan_dip);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vxlan_dip_stat_counter_get
 * Description:
 * Get the specified counter statistic for an VXLAN DIP.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan dip        - (IN) vxlan dip
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
bcm_error_t bcm_esw_vxlan_dip_stat_counter_get(
                              int              unit,
                              bcm_ip_t  vxlan_dip,
                              bcm_vxlan_dip_stat_t  stat,
                              uint32           num_entries,
                              uint32           *counter_indexes,
                              bcm_stat_value_t *counter_values)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_vxlan_dip_stat_counter_get(unit, 0, vxlan_dip, stat, 
                    num_entries, counter_indexes, counter_values);
    }
#endif

    return BCM_E_UNAVAIL;
}



/*
 * Function:
 *      bcm_esw_vxlan_dip_stat_counter_set
 * Description:
 *  Set the specified counter statistic for an VXLAN DIP.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip        - (IN) vxlan dip
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
bcm_error_t bcm_esw_vxlan_dip_stat_counter_set(
            int unit,
            bcm_ip_t vxlan_dip,
            bcm_vxlan_dip_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        return _bcm_td2_vxlan_dip_stat_counter_set(unit, vxlan_dip, stat,
                    num_entries, counter_indexes, counter_values);
    }
#endif

    return BCM_E_UNAVAIL;
}




/*
 * Function:
 *      bcm_esw_vxlan_dip_stat_multi_get
 *
 * Description:
 *  Get 64-bit counter value for multiple VXLAN DIP statistic types.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip        - (IN) vxlan dip
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */  
 bcm_error_t  bcm_esw_vxlan_dip_stat_multi_get(
                            int                   unit, 
                            bcm_ip_t              vxlan_dip,
                            int                   nstat, 
                            bcm_vxlan_dip_stat_t  *stat_arr,
                            uint64                *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_vxlan_dip_stat_multi_get (
                    unit, vxlan_dip, nstat, stat_arr, value_arr);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vxlan_dip_stat_multi_get32
 *
 * Description:
 *  Get 32-bit counter value for multiple VXLAN DIP statistic types.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip        - (IN) Ivxlan dip
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t  bcm_esw_vxlan_dip_stat_multi_get32(
                            int                   unit, 
                            bcm_ip_t              vxlan_dip,
                            int                   nstat, 
                            bcm_vxlan_dip_stat_t  *stat_arr,
                            uint32                *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_vxlan_dip_stat_multi_get32 (
                     unit, vxlan_dip, nstat, stat_arr, value_arr);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_vxlan_dip_stat_multi_set
 *
 * Description:
 *  Set 64-bit counter value for multiple VXLAN DIP statistic types.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip        - (IN) vxlan dip
 *      stat             - (IN) vxlan dip entry counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t  bcm_esw_vxlan_dip_stat_multi_set(
                            int                 unit, 
                            bcm_ip_t            vxlan_dip,
                            int                 nstat, 
                            bcm_vxlan_dip_stat_t    *stat_arr,
                            uint64              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_vxlan_dip_stat_multi_set (
                    unit, vxlan_dip, nstat, stat_arr, value_arr);
    }
#endif

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_esw_vxlan_dip_stat_multi_set32
 *
 * Description:
 *  Set 32-bit counter value for multiple VXLAN DIP statistic types.
 *  
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip        - (IN) vxlan dip
 *      stat             - (IN) vxlan dip entry counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

 bcm_error_t  bcm_esw_vxlan_dip_stat_multi_set32(
                            int                   unit, 
                            bcm_ip_t              vxlan_dip,
                            int                   nstat, 
                            bcm_vxlan_dip_stat_t  *stat_arr,
                            uint32                *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_vxlan_dip_stat_multi_set32 (
                    unit, vxlan_dip, nstat, stat_arr, value_arr);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vxlan_dip_stat_id_get
 * Description:
 *    Retrieve associated stat counter for given VXLAN DIP.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip        - (IN) vxlan dip entry information
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_esw_vxlan_dip_stat_id_get(
            int unit,
            bcm_ip_t vxlan_dip,
            bcm_vxlan_dip_stat_t stat,
            uint32 *stat_counter_id)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) { 
        return _bcm_td2_vxlan_dip_stat_id_get(unit, vxlan_dip, stat, stat_counter_id);
    }
#endif

    return BCM_E_UNAVAIL;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_vxlan_sw_dump
 * Purpose:
 *     Displays VXLAN information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_vxlan_sw_dump(int unit)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_vxlan)) {
        _bcm_td2_vxlan_sw_dump(unit);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        bcmi_gh2_vxlan_sw_dump(unit);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1, 1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1, 2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1, 3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1, 4)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1, 5)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1, 6)
#define BCM_WB_VERSION_1_7                SOC_SCACHE_VERSION(1, 7)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_7

/*
 * Function:
 *      _bcm_esw_vxlan_sync
 *
 * Purpose:
 *      Record VXLAN module persistent info for Level 2 Warm Boot
 *
 * Warm Boot Version Map:
 *  WB_VERSION_1_0
 *    vxlan_tunnel_term         Each entry stores a Vxlan Tunnel Terminator 
 *                              (SIP, DIP, Tunnel State). Max entries: 16K
 *    vxlan_tunnel_init         Each entry stores a Vxlan Tunnel Initiator 
 *                              (SIP, DIP, Tunnel State). Max entries: 16K 
 *    vxlan_ip_tnl_bitmap       Stores the BITMAP of Vxlan Tunnel usage
 *    mk.flags                  From match_key, Each entry stores a flags for
 *                              multicast tunnel. Max entries: 16K
 *    mk.match_tunnel_index     From match_key, Each entry stores a index 
 *                              for multicast tunnel. Max entries: 16K
 *
 *    If these fields are not stored in scache, after warmboot, VXlan API's 
 *    will corrupt hardware-entries and randomly-destroy Vxlan datapath 
 *    forwarding behavior.
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 * 
 * Returns:
 *      BCM_E_XXX
 */
 
int
_bcm_esw_vxlan_sync(int unit)
{
#ifdef BCM_TRIDENT2_SUPPORT
    int sz, rv = BCM_E_NONE;
    int i, num_tnl = 0, num_vp = 0;
    int stable_size;
    bcm_gport_t gport;
    uint8 *vxlan_state;
    soc_scache_handle_t scache_handle;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int num_vfi = 0;
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        return bcmi_gh2_vxlan_sync(unit);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    if (!soc_feature(unit, soc_feature_vxlan)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    vxlan_info = VXLAN_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VXLAN, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &vxlan_state, 
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INTERNAL)) {
        return rv;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);
    sz = num_vp * sizeof(_bcm_vxlan_tunnel_endpoint_t);

    /* Store each Vxlan Tunnnel Terminator entry (SIP, DIP, Tunnel State) */
    /* 
     *  vxlan_tunnel_term points to a struct array with a base type of 
     *  _bcm_vxlan_tunnel_endpoint_t, sized to hold the info of each virtual 
     *  port tunnel.
     *
     *  SOURCE_VPm hw index equals to array index, related to the IP & state 
     *  info of each VP.
     *  
     *  _bcm_vxlan_tunnel_endpoint_t[size of (SOURCE_VPm)]  *vxlan_tunnel_term
     */
    
    sal_memcpy(vxlan_state, vxlan_info->vxlan_tunnel_term, sz);
    vxlan_state += sz;
    
    /* Store each Vxlan Tunnel Initiator entry (SIP, DIP, Tunnel State) */
    /* 
     *  Same as vxlan_tunnel_term 
     */
     
    sal_memcpy(vxlan_state, vxlan_info->vxlan_tunnel_init, sz);
    vxlan_state += sz;
    
    /* Store the BITMAP of Vxlan Tunnel usage */
    /* 
     *  vxlan_ip_tnl_bitmap points to a struct array with a base type of uint32.
     *  sized to hold the maximum number of Egress IP Tunnel.
     *
     *  EGR_IP_TUNNELm hw index used as offset of this wide bitmap.
     *
     *  uint32[(size of (EGR_IP_TUNNELm) + 31) / 32]    *vxlan_ip_tnl_bitmap 
     */
    
    sal_memcpy(vxlan_state, vxlan_info->vxlan_ip_tnl_bitmap, 
                SHR_BITALLOCSIZE(num_tnl));
    vxlan_state += SHR_BITALLOCSIZE(num_tnl);

    /* Store the flags & match_tunnel_index of each Match Key */
    /* 
     *  Since we can't recover flags & match_tunnel_index from HW if we 
     *  configure a multicast tunnel(see _bcm_td2_vxlan_match_add with 
     *  BCM_VXLAN_PORT_MULTICAST flag), so we need to store it for warmboot 
     *  recovery. 
 */
     
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].flags), 
                    sizeof(uint32));
        vxlan_state += sizeof(uint32);        
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].match_tunnel_index), 
                    sizeof(uint32));
        vxlan_state += sizeof(uint32);
    }

    /*
     * Since we can't recover index && gport(trunk_id, modid, port) &&
     * match_vlan && match_inner_vlan from HW if we add this vxlan port to
     * VPLAG, so we need to store it for warmboot recovery. 
     */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].index), 
                   sizeof(uint32));
        vxlan_state += sizeof(uint32); 
        if (vxlan_info->match_key[i].trunk_id != -1) {
            BCM_GPORT_TRUNK_SET(gport, vxlan_info->match_key[i].trunk_id);
        } else if (vxlan_info->match_key[i].modid != -1) {
            BCM_GPORT_MODPORT_SET(gport,
                                  vxlan_info->match_key[i].modid,
                                  vxlan_info->match_key[i].port);
        } else {
            gport = BCM_GPORT_INVALID;
        }
        sal_memcpy(vxlan_state, &gport, sizeof(bcm_gport_t));
        vxlan_state += sizeof(bcm_gport_t);
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].match_vlan), 
                   sizeof(bcm_vlan_t));
        vxlan_state += sizeof(bcm_vlan_t); 
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].match_inner_vlan), 
                   sizeof(bcm_vlan_t));
        vxlan_state += sizeof(bcm_vlan_t); 
    }

    num_vfi = soc_mem_index_count(unit, VFIm);
    for (i = 0; i < num_vfi; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vpn_vlan[i]),
                   sizeof(bcm_vlan_t));
        vxlan_state += sizeof(bcm_vlan_t);
    }
    /* Store the type of each vxlan vpn */
    for (i = 0; i < num_vfi; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vpn_info[i].sip),
                   sizeof(bcm_ip_t));
        vxlan_state += sizeof(bcm_ip_t);
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vpn_info[i].vnid),
                   sizeof(uint32));
        vxlan_state += sizeof(uint32);
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vpn_info[i].vxlan_vpn_type),
                   sizeof(uint8));
        vxlan_state += sizeof(uint8);        
    }
    /* Store the vfi index for vp */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].vfi),
                   sizeof(int));
        vxlan_state += sizeof(int);

    }

    for (i = 0; i < num_vp; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].flags2),
                   sizeof(uint32));
        vxlan_state += sizeof(uint32);
    }

    return rv;
    
#endif

    return BCM_E_UNAVAIL;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#else   /* INCLUDE_L3 */
typedef int bcm_esw_vxlan_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

