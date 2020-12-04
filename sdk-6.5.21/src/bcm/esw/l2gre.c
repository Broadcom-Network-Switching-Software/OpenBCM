/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ESW L2GRE API
 */

#ifdef INCLUDE_L3

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif
#include <bcm_int/esw/l2gre.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/virtual.h>

/*
 * Function:
 *    bcm_esw_l2gre_init
 * Purpose:
 *    Init  L2GRE module
 * Parameters:
 *    IN :  unit
 * Returns:
 *    BCM_E_XXX
 */

int
bcm_esw_l2gre_init(int unit)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_l2gre)) {
        return bcm_tr3_l2gre_init(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
    return BCM_E_UNAVAIL;
}


 /* Function:
 *      bcm_l2gre_cleanup
 * Purpose:
 *      Detach the L2GRE module, clear all HW states
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_l2gre_cleanup(int unit)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_l2gre)) {
        return bcm_tr3_l2gre_cleanup(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
    return BCM_E_UNAVAIL;
}

 /* Function:
 *      bcm_esw_l2gre_vpn_create
 * Purpose:
 *      Createl L2-VPN instance
 * Parameters:
 *      unit     - Device Number
 *      info     - L2GRE VPN Config
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_vpn_create( int unit, bcm_l2gre_vpn_config_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              soc_mem_lock(unit, VFIm);
              rv = bcm_tr3_l2gre_vpn_create(unit, info);
              soc_mem_unlock(unit, VFIm);
              bcm_tr3_l2gre_unlock (unit);
         }

    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_vpn_create
 * Purpose:
 *      Delete L2-VPN instance
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - L2GRE Vpn
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_vpn_destroy(int unit, bcm_vpn_t l2vpn)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_vpn_destroy(unit, l2vpn);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_l2gre_cleanup
 * Purpose:
 *      Delete all L2-VPN instances
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_vpn_destroy_all(int unit)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_vpn_destroy_all(unit);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_l2gre_vpn_traverse
 * Purpose:
 *      L2GRE VPN Traverse
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb   - (IN)  User callback function, called once per entry
 *      user_data  -  (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXXX
 */
int 
bcm_esw_l2gre_vpn_traverse(int unit, bcm_l2gre_vpn_traverse_cb cb, 
                                                          void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_vpn_traverse(unit, cb, user_data);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}


 /* Function:
 *      bcm_l2gre_vpn_get
 * Purpose:
 *      Get L2-VPN instance
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - L2GRE Vpn
 *      info     - L2GRE VPN Config
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_vpn_get( int unit, bcm_vpn_t l2vpn, bcm_l2gre_vpn_config_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_vpn_get(unit, l2vpn, info);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_port_add
 * Purpose:
 *      Add Access/Network L2GRE port
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - L2GRE Vpn
 *      l2gre_port     - L2GRE Gport
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_port_add(int unit, bcm_vpn_t l2vpn, bcm_l2gre_port_t *l2gre_port)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (l2gre_port == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_port_add(unit, l2vpn, l2gre_port);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_port_delete
 * Purpose:
 *      Delete Access/Network L2GRE port
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - L2GRE Vpn
 *      l2gre_port_id     - L2GRE Gport Id
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_port_delete( int unit, bcm_vpn_t l2vpn, bcm_gport_t l2gre_port_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_port_delete(unit, l2vpn, l2gre_port_id);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_port_delete_all
 * Purpose:
 *      Delete all Access/Network L2GRE ports
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - L2GRE Vpn
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_port_delete_all(int unit, bcm_vpn_t l2vpn)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_port_delete_all(unit, l2vpn);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_port_get
 * Purpose:
 *      Get Access/Network L2GRE port info
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - L2GRE Vpn
 *      l2gre_port     - L2GRE Gport
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_port_get(int unit, bcm_vpn_t l2vpn, bcm_l2gre_port_t *l2gre_port)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (l2gre_port == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_port_get(unit, l2vpn, l2gre_port);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_port_get_all
 * Purpose:
 *      Get all Access/Network L2GRE port info
 * Parameters:
 *      unit     - (IN) Device Number
 *      l2vpn   - L2GRE Vpn
 *      port_max   - (IN) Maximum number of L2GRE ports in array
 *      port_array - (OUT) Array of L2GRE ports
 *      port_count - (OUT) Number of L2GRE ports returned in array
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_port_get_all(
    int unit, 
    bcm_vpn_t l2vpn, 
    int port_max, 
    bcm_l2gre_port_t *port_array, 
    int *port_count)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (port_count == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_port_get_all(unit, l2vpn, port_max, port_array, port_count);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_tunnel_initiator_create
 * Purpose:
 *      Create L2GRE Tunnel Initiator 
 * Parameters:
 *      unit     - Device Number
 *      info     - L2GRE Tunnel Initiator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_tunnel_initiator_create(unit, info);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}


 /* Function:
 *      bcm_esw_l2gre_tunnel_initiator_destroy
 * Purpose:
 *      Delete L2GRE Tunnel Initiator 
 * Parameters:
 *      unit     - Device Number
 *      l2gre_tunnel_id     - L2GRE Tunnel Initiator Id
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_tunnel_initiator_destroy(int unit, bcm_gport_t l2gre_tunnel_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_tunnel_initiator_destroy(unit, l2gre_tunnel_id);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_tunnel_initiator_destroy_all
 * Purpose:
 *      Delete all L2GRE Tunnel Initiators 
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_tunnel_initiator_destroy_all(int unit)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_tunnel_initiator_destroy_all(unit);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_tunnel_initiator_get
 * Purpose:
 *      Get  L2GRE Tunnel Initiator Info
 * Parameters:
 *      unit     - Device Number
 *      info     - L2GRE Tunnel Initiator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_tunnel_initiator_get(unit, info);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /*
 * Function:
 *      bcm_esw_l2gre_tunnel_initiator_traverse
 * Purpose:
 *      L2GRE Tunnel Initiator Traverse
 * Parameters:
 *      unit     - Device Number
 *      cb     - User callback function, called once per entry
 *      user_data     - Cookie
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_l2gre_tunnel_initiator_traverse(int unit, bcm_tunnel_initiator_traverse_cb cb, void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
        rv = bcm_tr3_l2gre_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_tr3_l2gre_tunnel_initiator_traverse(unit, cb, user_data);
            bcm_tr3_l2gre_unlock (unit);
        }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_tunnel_terminator_create
 * Purpose:
 *      Create L2GRE Tunnel terminator
 * Parameters:
 *      unit     - Device Number
 *      info     - L2GRE Tunnel terminator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_tunnel_terminator_create(int unit, bcm_tunnel_terminator_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_tunnel_terminator_create(unit, info);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_tunnel_terminator_update
 * Purpose:
 *      Update L2GRE Tunnel terminator
 * Parameters:
 *      unit     - Device Number
 *      info     - L2GRE Tunnel terminator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_tunnel_terminator_update(int unit, bcm_tunnel_terminator_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_tunnel_terminator_update(unit, info);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_l2gre_tunnel_terminator_destroy
 * Purpose:
 *      Destroy L2GRE Tunnel terminator
 * Parameters:
 *      unit     - Device Number
 *      l2gre_tunnel_id     - L2GRE Tunnel Initiator Id
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_tunnel_terminator_destroy(int unit, bcm_gport_t l2gre_tunnel_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_tunnel_terminator_destroy(unit, l2gre_tunnel_id);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}


 /* Function:
 *      bcm_esw_l2gre_tunnel_terminator_destroy_all
 * Purpose:
 *      Destroy all L2GRE Tunnel terminators
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_tunnel_terminator_destroy_all(int unit)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_tunnel_terminator_destroy_all(unit);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}


 /* Function:
 *      bcm_esw_l2gre_tunnel_terminator_get
 * Purpose:
 *      Get  L2GRE Tunnel terminator Info
 * Parameters:
 *      unit     - Device Number
 *      info     - L2GRE Tunnel terminator info
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_l2gre_tunnel_terminator_get( int unit, bcm_tunnel_terminator_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
         if (info == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcm_tr3_l2gre_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcm_tr3_l2gre_tunnel_terminator_get(unit, info);
              bcm_tr3_l2gre_unlock (unit);
         }
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_l2gre_tunnel_terminator_traverse
 * Purpose:
 *      L2GRE Tunnel Terminator Traverse
 * Parameters:
 *      unit     - Device Number
 *      cb     - User callback function, called once per entry
 *      user_data     - Cookie
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_l2gre_tunnel_terminator_traverse(int unit, bcm_tunnel_terminator_traverse_cb cb, void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2gre)) {
        rv = bcm_tr3_l2gre_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_tr3_l2gre_tunnel_terminator_traverse(unit, cb, user_data);
            bcm_tr3_l2gre_unlock (unit);
        }
    }
#endif
    return rv;
}

/* Function:
 *      _bcm_esw_l2gre_match_add
 */

int
_bcm_esw_l2gre_match_add(int unit, bcm_l2gre_port_t *l2gre_port, int vp, bcm_vpn_t vpn)
{
int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv = bcm_td2_l2gre_match_add(unit, l2gre_port, vp, vpn);
         return rv;
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_td2_l2gre)) {
         rv = bcm_td2_l2gre_match_add(unit, l2gre_port, vp, vpn);
         return rv;
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr3_l2gre_match_add(unit, l2gre_port, vp, vpn);
         return rv;
    }
#endif
    return rv;

}

/* Function:
 *     bcm_esw_l2gre_match_delete
 */

int
_bcm_esw_l2gre_match_delete(int unit,  int vp)
{
int rv = BCM_E_UNAVAIL;    

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv = bcm_td2_l2gre_match_delete(unit, vp, NULL);
         return rv;
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_td2_l2gre)) {
         rv = bcm_td2_l2gre_match_delete(unit, vp, NULL);
         return rv;
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr3_l2gre_match_delete(unit, vp);
         return rv;
    }
#endif
    return rv;

}

/* Function:
 *      _bcm_esw_l2gre_match_get
 */

int
_bcm_esw_l2gre_match_get(int unit, bcm_l2gre_port_t *l2gre_port, int vp)
{
int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv = bcm_td2_l2gre_match_get(unit, l2gre_port, vp);
         return rv;
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_td2_l2gre)) {
         rv = bcm_td2_l2gre_match_get(unit, l2gre_port, vp);
         return rv;
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr3_l2gre_match_get(unit, l2gre_port, vp);
         return rv;
    }
#endif
    return rv;

}

/* Function:
 *     bcm_esw_l2gre_eline_vp_configure
 */

int
_bcm_esw_l2gre_eline_vp_configure (int unit, int vfi_index, int active_vp, 
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port)
{
int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv = _bcm_td2_l2gre_eline_vp_configure(unit, vfi_index, active_vp, svp, 
                                                    tpid_enable, l2gre_port);
         return rv;
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_td2_l2gre)) {
         rv = _bcm_td2_l2gre_eline_vp_configure(unit, vfi_index, active_vp, svp, 
                                                    tpid_enable, l2gre_port);
         return rv;
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv  = _bcm_tr3_l2gre_eline_vp_configure (unit, vfi_index, active_vp, svp, 
                                                    tpid_enable, l2gre_port);
         return rv;
    }
#endif
    return rv;

}


/* Function:
 *     bcm_esw_l2gre_elan_vp_configure
 */

int
_bcm_esw_l2gre_elan_vp_configure (int unit, int vfi_index, int active_vp, 
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port)
{
int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv = _bcm_td2_l2gre_elan_vp_configure(unit, vfi_index, active_vp, svp, 
                                                    tpid_enable, l2gre_port);
         return rv;
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_td2_l2gre)) {
         rv = _bcm_td2_l2gre_elan_vp_configure(unit, vfi_index, active_vp, svp, 
                                                    tpid_enable, l2gre_port);
         return rv;
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv  = _bcm_tr3_l2gre_elan_vp_configure (unit, vfi_index, active_vp, svp, 
                                                    tpid_enable, l2gre_port);
         return rv;
    }
#endif
    return rv;

}

/*
 * Function:
 *      _bcm_esw_l2gre_trunk_member_add
 * Purpose:
 *      Set l2GRE state for trunk members being added to a trunk group.
 * Parameters:
 *     unit  - (IN) Device Number
 *     trunk_id - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be added
 *     trunk_member_array - (IN) Trunk member ports to be added
 * Returns:
 *      BCM_E_XXX
 */     
int
_bcm_esw_l2gre_trunk_member_add(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array) 
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_l2gre)) {
        rv = bcm_tr3_l2gre_trunk_member_add(unit, trunk_id,
                 trunk_member_count, trunk_member_array);
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_l2gre_trunk_member_delete
 * Purpose:
 *      Clear L2GRE state for trunk members leaving a trunk group.
 * Parameters:
 *     unit  - (IN) Device Number
 *     trunk_id - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be deleted
 *     trunk_member_array - (IN) Trunk member ports to be deleted
 * Returns:
 *      BCM_E_XXX
 */     
int
_bcm_esw_l2gre_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array) 
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_l2gre)) {
        rv = bcm_tr3_l2gre_trunk_member_delete(unit, trunk_id,
                 trunk_member_count, trunk_member_array);
    }
#endif
    return rv;
}


#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_l2gre_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given ingress interface.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
static 
bcm_error_t _bcm_esw_l2gre_stat_get_table_info(
            int                        unit,
            bcm_gport_t                port,
            bcm_vpn_t                  vpn,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
    int vp = -1;
    int vfi = -1;
    int vp_vfi = -1;
    int vp_type = -1;
    _bcm_vp_info_t vp_info;
    int nh_idx = -1;
    int entry_type = 0;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ( !(_BCM_L2GRE_VPN_INVALID == vpn) &&
          (_BCM_VPN_IS_L2GRE_ELINE(vpn) || _BCM_VPN_IS_L2GRE_ELAN(vpn))) {
        /* Get VFI */
        _BCM_L2GRE_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
            return BCM_E_NOT_FOUND;
        }
    }

    if (!(BCM_GPORT_INVALID == port) && BCM_GPORT_IS_L2GRE_PORT(port)) {
        egr_dvp_attribute_entry_t  egr_dvp_attribute;
        source_vp_entry_t svp;
        ing_dvp_table_entry_t dvp;
        egr_l3_next_hop_entry_t egr_nh;

        /* Ingress L2gre VP */
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            return BCM_E_NOT_FOUND;
        }
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        BCM_IF_ERROR_RETURN (
           READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

        /* For VP-VFI attach flex counters for the associated VP */
        if (soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) == 0x1) {
            table_info[*num_of_tables].table=SOURCE_VPm;
            table_info[*num_of_tables].index=vp;
            table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
            (*num_of_tables)++;

            if (soc_SOURCE_VPm_field32_get(unit, &svp, VFIf) == vfi) {
                vp_vfi = vfi;
            }
        }

        BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
        if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
            sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
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
            if (vp_type == 3) { /* l2gre vp type */
                /* Egress L2gre (DVP) */
#if defined(BCM_TRIDENT3_SUPPORT)
                if (SOC_IS_TRIDENT3X(unit)) {
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
            BCM_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp,
                                                    &dvp));
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
            if (entry_type == _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW) { /* sd tag */
                /* Next hop for access */
                table_info[*num_of_tables].table = EGR_L3_NEXT_HOPm;
                table_info[*num_of_tables].index = nh_idx;
                table_info[*num_of_tables].direction = bcmStatFlexDirectionEgress;
                (*num_of_tables)++;
            }
        }
    }

    if ((vp_vfi == -1) && (vfi != -1)) {
        table_info[*num_of_tables].table=VFIm;
        table_info[*num_of_tables].index=vfi;
        table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
    }

    if ((vp_type != 3) && (vfi != -1)){
        /* Egress L2gre VPN */
        table_info[*num_of_tables].table = EGR_VFIm;
        table_info[*num_of_tables].index = vfi;
        table_info[*num_of_tables].direction = bcmStatFlexDirectionEgress;
        (*num_of_tables)++;
    }

    if (*num_of_tables == 0) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRIDENT2_SUPPORT */

/*
 * Function:
 *      bcm_esw_l2gre_stat_attach
 * Description:
 *      Attach counters entries to the given  L2gre port and vpn. 
 *      Allocate flex counter and assign counter index and offset for the index 
 *      pointed by L2gre vpn(VFI) and port(tunnel)
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
    
bcm_error_t  bcm_esw_l2gre_stat_attach (
                            int                 unit,
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            uint32              stat_counter_id)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int                        rv = BCM_E_NONE;
    int                        counter_flag = 0;
    soc_mem_t                  table = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object = bcmStatObjectIngPort;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;
    uint32                     count = 0;
    uint32                     actual_num_tables = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_L2GRE_FLEX_COUNTER_MAX_TABLES];

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Get pool, base index group mode and offset modes from stat counter id */
    _bcm_esw_stat_get_counter_id_info(
                  unit,
                  stat_counter_id,
                  &group_mode, &object, &offset_mode, &pool_number, &base_index);

    /* Validate object and group mode */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit, object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, group_mode));

    /* Get Table index to attach flexible counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit, object, 1, &actual_num_tables, &table, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_l2gre_stat_get_table_info(
                        unit, port, vpn, &num_of_tables, &table_info[0]));

    for (count = 0; count < num_of_tables; count++) {
         if ((table_info[count].direction == direction)) {
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
 *      bcm_esw_l2gre_stat_detach
 * Description:
 *      Detach counter entries to the given  L2gre port and vpn. 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t   bcm_esw_l2gre_stat_detach (
                            int                 unit,
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn)
{

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32                     count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_L2GRE_FLEX_COUNTER_MAX_TABLES];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }


    /* Get the table index to be detached */
    BCM_IF_ERROR_RETURN(_bcm_esw_l2gre_stat_get_table_info(
                   unit, port, vpn, &num_of_tables, &table_info[0]));

    for (count = 0; count < num_of_tables; count++) {
         if (table_info[count].direction == bcmStatFlexDirectionIngress) {
             rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
             }
         } else if (table_info[count].direction == bcmStatFlexDirectionEgress) {
             rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, 0, table_info[count].table,
                        table_info[count].index);
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
             }
         }
    }

    BCM_STAT_FLEX_DETACH_RETURN(err_code)
#else 
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIDENT2_SUPPORT */
}

/*
 * Function:
 *      _bcm_esw_l2gre_stat_counter_get
 *
 * Description:
 *  Get L2gre counter value for specified L2gre port and vpn
 *  if sync_mode is set, sync the sw accumulated count
 *  with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) L2gre counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter Offset indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t   _bcm_esw_l2gre_stat_counter_get(
                            int                 unit, 
                            int                 sync_mode,
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            bcm_l2gre_stat_t    stat, 
                            uint32              num_entries, 
                            uint32              *counter_indexes, 
                            bcm_stat_value_t    *counter_values)
{   

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32                     table_count = 0;
    uint32                     index_count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     byte_flag = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_L2GRE_FLEX_COUNTER_MAX_TABLES]; 

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Get the flex counter direction - ingress/egress */
    if ((stat == bcmL2greOutPackets) ||
        (stat == bcmL2greOutBytes)) {
         direction = bcmStatFlexDirectionEgress;
    } else {
         direction = bcmStatFlexDirectionIngress;
    }
    if ((stat == bcmL2greOutPackets) ||
       (stat == bcmL2greInPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
   
    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_l2gre_stat_get_table_info(
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
 *      bcm_esw_l2gre_stat_counter_get
 *
 * Description:
 *  Get L2gre counter value for specified L2gre port and vpn
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) L2gre counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter Offset indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int   bcm_esw_l2gre_stat_counter_get(
                            int                 unit,
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            bcm_l2gre_stat_t    stat, 
                            uint32              num_entries, 
                            uint32              *counter_indexes, 
                            bcm_stat_value_t    *counter_values)
{   

    return  _bcm_esw_l2gre_stat_counter_get(unit, 0, port, 
                                            vpn, stat, 
                                            num_entries,
                                            counter_indexes, 
                                            counter_values);
    
}    



/*
 * Function:
 *      bcm_esw_l2gre_stat_counter_sync_get
 *
 * Description:
 *  Get L2gre counter value for specified L2gre port and vpn
 *  sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) L2gre counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter Offset indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int   bcm_esw_l2gre_stat_counter_sync_get(
                            int                 unit,
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            bcm_l2gre_stat_t    stat, 
                            uint32              num_entries, 
                            uint32              *counter_indexes, 
                            bcm_stat_value_t    *counter_values)
{   

    return  _bcm_esw_l2gre_stat_counter_get(unit, 1, port, 
                                            vpn, stat, 
                                            num_entries,
                                            counter_indexes, 
                                            counter_values);
    
}    
/*
 * Function:
 *      bcm_esw_l2gre_stat_counter_set
 *
 * Description:
 *  Set L2gre counter value for specified L2gre port and vpn
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) L2gre counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter offset indexes
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t   bcm_esw_l2gre_stat_counter_set(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            bcm_l2gre_stat_t    stat, 
                            uint32              num_entries, 
                            uint32              *counter_indexes, 
                            bcm_stat_value_t    *counter_values)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    uint32                     table_count=0;
    uint32                     index_count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_L2GRE_FLEX_COUNTER_MAX_TABLES];

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Get the flex counter direction - ingress/egress */
    if ((stat == bcmL2greOutPackets) ||
        (stat == bcmL2greOutBytes)) {
         direction = bcmStatFlexDirectionEgress;
    } else {
         direction = bcmStatFlexDirectionIngress;
    }
    if (stat == bcmL2greOutPackets || stat == bcmL2greInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
   
    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_l2gre_stat_get_table_info(
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
 *      bcm_esw_l2gre_stat_multi_get
 *
 * Description:
 *  Get Multiple L2gre counter value for specified L2gre port and vpn for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t   bcm_esw_l2gre_stat_multi_get(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            int                 nstat, 
                            bcm_l2gre_stat_t    *stat_arr,
                            uint64              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         BCM_IF_ERROR_RETURN(bcm_esw_l2gre_stat_counter_get( 
                             unit, port, vpn, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmL2greOutPackets) || 
            (stat_arr[idx] == bcmL2greInPackets)) {
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
 *      bcm_esw_l2gre_stat_multi_get32
 *
 * Description:
 *  Get 32bit L2gre counter value for specified L2gre port and vpn for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t   bcm_esw_l2gre_stat_multi_get32(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            int                 nstat, 
                            bcm_l2gre_stat_t    *stat_arr,
                            uint32              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         BCM_IF_ERROR_RETURN(bcm_esw_l2gre_stat_counter_get( 
                             unit, port, vpn, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmL2greOutPackets) || 
            (stat_arr[idx] == bcmL2greInPackets)) {
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
 *      bcm_esw_l2gre_stat_multi_set
 *
 * Description:
 *  Set L2gre counter value for specified L2gre port and vpn for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) L2gre counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter Offset indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t  bcm_esw_l2gre_stat_multi_set(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            int                 nstat, 
                            bcm_l2gre_stat_t    *stat_arr,
                            uint64              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Iterate f all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         if ((stat_arr[idx] == bcmL2greOutPackets) || 
            (stat_arr[idx] == bcmL2greInPackets)) {
              counter_values.packets = COMPILER_64_LO(value_arr[idx]);
         } else {
              COMPILER_64_SET(counter_values.bytes,
                              COMPILER_64_HI(value_arr[idx]),
                              COMPILER_64_LO(value_arr[idx]));
         }
         BCM_IF_ERROR_RETURN(bcm_esw_l2gre_stat_counter_set( 
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
 *      bcm_esw_l2gre_stat_multi_set32
 *
 * Description:
 *  Set 32bit L2gre counter value for specified L2gre port and vpn for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) L2gre counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter Offset indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

 bcm_error_t   bcm_esw_l2gre_stat_multi_set32(
                            int                 unit, 
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            int                 nstat, 
                            bcm_l2gre_stat_t    *stat_arr,
                            uint32              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         if ((stat_arr[idx] == bcmL2greOutPackets) || 
            (stat_arr[idx] == bcmL2greInPackets)) {
             counter_values.packets = value_arr[idx];
         } else {
             COMPILER_64_SET(counter_values.bytes,0,value_arr[idx]);
         }
         BCM_IF_ERROR_RETURN(bcm_esw_l2gre_stat_counter_set( 
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
 *      bcm_esw_l2gre_stat_id_get
 *
 * Description: 
 *      Get Stat Counter if associated with given L2gre port and vpn
 *
 * Parameters: 
 *      unit             - (IN) bcm device
 *      port             - (IN) L2gre gport
 *      vpn              - (IN) vpn (VFI)
 *      stat             - (IN) L2gre counter stat types
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */

 bcm_error_t   bcm_esw_l2gre_stat_id_get(
                            int                 unit,
                            bcm_gport_t         port,
                            bcm_vpn_t           vpn,
                            bcm_l2gre_stat_t    stat, 
                            uint32              *stat_counter_id) 
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_L2GRE_FLEX_COUNTER_MAX_TABLES];
    uint32                     index = 0;
    uint32                     num_stat_counter_ids = 0;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if ((stat == bcmL2greOutPackets) ||
        (stat == bcmL2greOutBytes)) {
         direction = bcmStatFlexDirectionEgress;
    } else {
         direction = bcmStatFlexDirectionIngress;
    }

    /*  Get Tables, for which flex counter are attached  */
    BCM_IF_ERROR_RETURN(_bcm_esw_l2gre_stat_get_table_info(
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

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_l2gre_sync
 *
 * Purpose:
 *      Record L2GRE module persistent info for Level 2 Warm Boot
 *
 * Warm Boot Version Map:
 *      WB_VERSION_1_0
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_l2gre_sync(int unit)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_l2gre)) {
        return _bcm_tr3_l2gre_sync(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_l2gre_sw_dump
 * Purpose:
 *     Displays L2GRE information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_l2gre_sw_dump(int unit)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_l2gre)) {
        _bcm_tr3_l2gre_sw_dump(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
    return;
}
#endif

/*
 * Function:
 *      _bcm_esw_l2gre_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a L2GRE virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) L2GRE virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_l2gre_port_source_vp_lag_set(
    int unit, bcm_gport_t gport, int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_td2_l2gre) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_tr3_l2gre_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2_l2gre_port_source_vp_lag_set(unit, gport, vp_lag_vp);
            bcm_tr3_l2gre_unlock(unit);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_l2gre_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a L2GRE virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) L2GRE virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_l2gre_port_source_vp_lag_clear(
    int unit, bcm_gport_t gport, int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_td2_l2gre) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_tr3_l2gre_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2_l2gre_port_source_vp_lag_clear(unit, gport, vp_lag_vp);
            bcm_tr3_l2gre_unlock(unit);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_l2gre_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a L2GRE virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) L2GRE virtual port GPORT ID.
 *      vp_lag_vp - (OUT) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_l2gre_port_source_vp_lag_get(
    int unit, bcm_gport_t gport, int *vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_td2_l2gre) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_tr3_l2gre_lock(unit);
        if ( rv == BCM_E_NONE ) {
            rv = bcm_td2_l2gre_port_source_vp_lag_get(unit, gport, vp_lag_vp);
            bcm_tr3_l2gre_unlock(unit);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}
#else   /* INCLUDE_L3 */
typedef int bcm_esw_l2gre_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */
