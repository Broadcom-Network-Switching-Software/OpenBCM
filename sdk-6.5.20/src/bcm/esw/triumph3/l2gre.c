/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ESW L2GRE API
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/hash.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm/types.h>
#include <bcm/l3.h>
#include <soc/ism_hash.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/l2gre.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/triumph.h>
#include <soc/scache.h>
#include <bcm_int/esw/switch.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#include <soc/esw/cancun.h>
#endif

_bcm_tr3_l2gre_bookkeeping_t   *_bcm_tr3_l2gre_bk_info[BCM_MAX_NUM_UNITS] = { 0 };

#define L2GRE_INFO(_unit_)   (_bcm_tr3_l2gre_bk_info[_unit_])
#define L3_INFO(_unit_)   (&_bcm_l3_bk_info[_unit_])
#define VIRTUAL_INFO(unit)    (&_bcm_virtual_bk_info[unit])

/*
 * EGR_IP_TUNNEL table usage bitmap operations
 */
#define _BCM_L2GRE_IP_TNL_USED_GET(_u_, _tnl_) \
        SHR_BITGET(L2GRE_INFO(_u_)->l2gre_ip_tnl_bitmap, (_tnl_))
#define _BCM_L2GRE_IP_TNL_USED_SET(_u_, _tnl_) \
        SHR_BITSET(L2GRE_INFO((_u_))->l2gre_ip_tnl_bitmap, (_tnl_))
#define _BCM_L2GRE_IP_TNL_USED_CLR(_u_, _tnl_) \
        SHR_BITCLR(L2GRE_INFO((_u_))->l2gre_ip_tnl_bitmap, (_tnl_))

#define _BCM_L2GRE_CLEANUP(_rv_) \
       if ( (_rv_) < 0) { \
           goto cleanup; \
       }

#if defined(BCM_TRIDENT3_SUPPORT)
#define L2GRE_DVP_EVXLT_KEY_VFI 1
#endif


STATIC int _bcm_tr3_l2gre_bud_loopback_disable(int unit);
STATIC int _bcm_tr3_l2gre_sd_tag_set( int unit, bcm_l2gre_vpn_config_t *l2gre_vpn_info,
                         bcm_l2gre_port_t *l2gre_port,
                         _bcm_tr3_l2gre_nh_info_t  *egr_nh_info,
                         uint32   *vxlate_entry,
                         int *tpid_index );
STATIC void _bcm_tr3_l2gre_sd_tag_get( int unit, bcm_l2gre_vpn_config_t *l2gre_vpn_info,
                         bcm_l2gre_port_t *l2gre_port,
                         egr_l3_next_hop_entry_t *egr_nh,
                         uint32   *vxlate_entry,
                         int network_port_flag );
STATIC int _bcm_tr3_l2gre_egr_xlate_entry_set(int unit, bcm_l2gre_vpn_config_t *vpn_info);
STATIC int _bcm_tr3_l2gre_egr_xlate_entry_reset(int unit, bcm_vpn_t vpn);
STATIC int _bcm_tr3_l2gre_egr_xlate_entry_get(int unit, int vfi, soc_mem_t *, uint32 *vxlate_entry);
STATIC int _bcm_tr3_l2gre_match_vpnid_entry_set(int unit, bcm_l2gre_vpn_config_t *vpn_info);
STATIC int _bcm_tr3_l2gre_match_vpnid_entry_reset(int unit, uint32 vpnid);

int _bcm_tr3_l2gre_port_get(int unit, bcm_vpn_t vpn, int vp, bcm_l2gre_port_t  *l2gre_port);


/*
 * Function:
 *      _bcm_l2gre_check_init
 * Purpose:
 *      Check if L2GRE is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


STATIC int
_bcm_l2gre_check_init(int unit)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    l2gre_info = L2GRE_INFO(unit);

    if ((l2gre_info == NULL) || (l2gre_info->initialized == FALSE)) {
         return BCM_E_INIT;
    } else {
         return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcm_tr3_l2gre_lock
 * Purpose:
 *      Take L2GRE Lock Sempahore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


 int
 bcm_tr3_l2gre_lock(int unit)
{
   int rv;

   rv = _bcm_l2gre_check_init(unit);

   if ( rv == BCM_E_NONE ) {
           sal_mutex_take(L2GRE_INFO((unit))->l2gre_mutex, sal_mutex_FOREVER);
   }
   return rv;
}



/*
 * Function:
 *      bcm_tr3_l2gre_unlock
 * Purpose:
 *      Release  L2GRE Lock Semaphore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


void
bcm_tr3_l2gre_unlock(int unit)
{
    int rv;

   rv = _bcm_l2gre_check_init(unit);
    if ( rv == BCM_E_NONE ) {
         sal_mutex_give(L2GRE_INFO((unit))->l2gre_mutex);
    }
}


/*
 * Function:
 *      _bcm_tr3_l2gre_free_resource
 * Purpose:
 *      Free all allocated software resources
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */


STATIC void
_bcm_tr3_l2gre_free_resource(int unit)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);

    /* If software tables were not allocated we are done. */
    if (NULL == L2GRE_INFO(unit)) {
        return;
    }

    /* Destroy EGR_IP_TUNNEL usage bitmap */
    if (l2gre_info->l2gre_ip_tnl_bitmap) {
        sal_free(l2gre_info->l2gre_ip_tnl_bitmap);
        l2gre_info->l2gre_ip_tnl_bitmap = NULL;
    }

    if (l2gre_info->match_key) {
        sal_free(l2gre_info->match_key);
        l2gre_info->match_key = NULL;
    }

    if (l2gre_info->l2gre_tunnel_init) {
        sal_free(l2gre_info->l2gre_tunnel_init);
        l2gre_info->l2gre_tunnel_init = NULL;
    }

    if (l2gre_info->l2gre_tunnel_term) {
        sal_free(l2gre_info->l2gre_tunnel_term);
        l2gre_info->l2gre_tunnel_term = NULL;
    }

    /* Free module data. */
    sal_free(L2GRE_INFO(unit));
    L2GRE_INFO(unit) = NULL;
}

/*
 * Function:
 *      bcm_tr3_l2gre_allocate_bk
 * Purpose:
 *      Initialize L2GRe software book-kepping
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
bcm_tr3_l2gre_allocate_bk(int unit)
{
    /* Allocate/Init unit software tables. */
    if (NULL == L2GRE_INFO(unit)) {
        BCM_TR3_L2GRE_ALLOC(L2GRE_INFO(unit), sizeof(_bcm_tr3_l2gre_bookkeeping_t),
                          "l2gre_bk_module_data");
        if (NULL == L2GRE_INFO(unit)) {
            return (BCM_E_MEMORY);
        } else {
            L2GRE_INFO(unit)->initialized = FALSE;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_hw_clear
 * Purpose:
 *     Perform hw tables clean up for L2GRE module.
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_hw_clear(int unit)
{
    int rv = BCM_E_NONE, rv_error = BCM_E_NONE;

    rv = bcm_tr3_l2gre_tunnel_terminator_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    rv = bcm_tr3_l2gre_tunnel_initiator_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    rv = bcm_tr3_l2gre_vpn_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    rv = _bcm_tr3_l2gre_bud_loopback_disable(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    return rv_error;
}

/*
 * Function:
 *      bcm_tr3_l2gre_cleanup
 * Purpose:
 *      DeInit  L2GRE software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_cleanup(int unit)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int rv = BCM_E_UNAVAIL;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    l2gre_info = L2GRE_INFO(unit);

    if (FALSE == l2gre_info->initialized) {
        return (BCM_E_NONE);
    }

    rv = bcm_tr3_l2gre_lock (unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
        rv = _bcm_tr3_l2gre_hw_clear(unit);
    }

    /* Mark the state as uninitialized */
    l2gre_info->initialized = FALSE;

    sal_mutex_give(l2gre_info->l2gre_mutex);

    /* Destroy protection mutex. */
    sal_mutex_destroy(l2gre_info->l2gre_mutex );

    /* Free software resources */
    (void) _bcm_tr3_l2gre_free_resource(unit);

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_port_match_count_adjust
 * Purpose:
 *      Obtain ref-count for a L2GRE port
 * Returns:
 *      BCM_E_XXX
 */

void
bcm_tr3_l2gre_port_match_count_adjust(int unit, int vp, int step)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);

    l2gre_info->match_key[vp].match_count += step;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1, 1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_1

/*
 * Function:
 *      _bcm_tr3_l2gre_wb_alloc
 *
 * Purpose:
 *      Alloc persisitent memory for Level 2 Warm Boot scache.
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2gre_wb_alloc(int unit)
{
    int alloc_sz = 0;
    int rv = BCM_E_NONE;
    int num_vp = 0;
    uint8 *l2gre_state;
    int stable_size = 0;
    soc_scache_handle_t scache_handle;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    /* Size of Tunnel Terminator & Initiator of all VP */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    alloc_sz += num_vp * (sizeof(bcm_ip_t) *2 + sizeof(uint16)) * 2;

    /* Size of All match_key's flags & match_tunnel_index */
    alloc_sz += num_vp * sizeof(uint32) * 2;


    if (soc_feature(unit, soc_feature_td2_l2gre) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        /*
         * Size of All match_key's index && gport(trunk_id, modid, port) &&
         * match_vlan && match_inner_vlan
         */
        alloc_sz += num_vp * (sizeof(uint32) + sizeof(bcm_gport_t) +
                              sizeof(bcm_vlan_t) * 2);
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_L2GRE, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 alloc_sz, (uint8**)&l2gre_state,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_wb_recover
 *
 * Purpose:
 *      Recover L2GRE module info for Level 2 Warm Boot from persisitent memory
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
STATIC int
_bcm_tr3_l2gre_wb_recover(int unit)
{
    int i, rv = BCM_E_NONE;
    int num_vp = 0;
    int stable_size;
    uint16 recovered_ver = 0;
    uint8 *l2gre_state = NULL;
    soc_scache_handle_t scache_handle;
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    bcm_gport_t gport;
    int additional_scache_size = 0;

    l2gre_info = L2GRE_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_L2GRE, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &l2gre_state,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } else if (rv == BCM_E_NOT_FOUND) {
        return _bcm_tr3_l2gre_wb_alloc(unit);
    }

    if (l2gre_state == NULL) {
        return BCM_E_NONE;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    /* Recover the L2GRE Tunnnel Terminator information */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(&(l2gre_info->l2gre_tunnel_init[i].dip),
                   l2gre_state, sizeof(bcm_ip_t));
        l2gre_state += sizeof(bcm_ip_t);
        sal_memcpy(&(l2gre_info->l2gre_tunnel_init[i].sip),
                   l2gre_state, sizeof(bcm_ip_t));
        l2gre_state += sizeof(bcm_ip_t);
        sal_memcpy(&(l2gre_info->l2gre_tunnel_init[i].tunnel_state),
                   l2gre_state, sizeof(uint16));
        l2gre_state += sizeof(uint16);
    }

    /* Recover the L2GRE Tunnnel Initiator information */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(&(l2gre_info->l2gre_tunnel_term[i].dip),
                   l2gre_state, sizeof(bcm_ip_t));
        l2gre_state += sizeof(bcm_ip_t);
        sal_memcpy(&(l2gre_info->l2gre_tunnel_term[i].sip),
                   l2gre_state, sizeof(bcm_ip_t));
        l2gre_state += sizeof(bcm_ip_t);
        sal_memcpy(&(l2gre_info->l2gre_tunnel_term[i].tunnel_state),
                   l2gre_state, sizeof(uint16));
        l2gre_state += sizeof(uint16);
    }

    /* Recover the flags & match_tunnel_index of each Match Key */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(&(l2gre_info->match_key[i].flags),
                   l2gre_state, sizeof(uint32));
        l2gre_state += sizeof(uint32);
        sal_memcpy(&(l2gre_info->match_key[i].match_tunnel_index),
                   l2gre_state, sizeof(uint32));
        l2gre_state += sizeof(uint32);
    }

    if (soc_feature(unit, soc_feature_td2_l2gre) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        /*
         * Recover the index && gport(trunk_id, modid, port) && match_vlan &&
         * match_inner_vlan of each Match Key
         */
        if (recovered_ver >= BCM_WB_VERSION_1_1) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(l2gre_info->match_key[i].index), l2gre_state,
                           sizeof(uint32));
                l2gre_state += sizeof(uint32);
                sal_memcpy(&gport, l2gre_state, sizeof(bcm_gport_t));
                if (BCM_GPORT_IS_TRUNK(gport)) {
                    l2gre_info->match_key[i].trunk_id =
                        BCM_GPORT_TRUNK_GET(gport);
                } else if (BCM_GPORT_IS_MODPORT(gport)) {
                    l2gre_info->match_key[i].modid =
                        BCM_GPORT_MODPORT_MODID_GET(gport);
                    l2gre_info->match_key[i].port =
                        BCM_GPORT_MODPORT_PORT_GET(gport);
                }
                l2gre_state += sizeof(bcm_gport_t);
                sal_memcpy(&(l2gre_info->match_key[i].match_vlan), l2gre_state,
                           sizeof(bcm_vlan_t));
                l2gre_state += sizeof(bcm_vlan_t);
                sal_memcpy(&(l2gre_info->match_key[i].match_inner_vlan),
                           l2gre_state, sizeof(bcm_vlan_t));
                l2gre_state += sizeof(bcm_vlan_t);
            }
        } else {
            additional_scache_size +=
                num_vp *
                (sizeof(uint32) + sizeof(bcm_gport_t) + sizeof(bcm_vlan_t) * 2);
        }
    }

    if (additional_scache_size > 0) {
        rv = soc_scache_realloc(unit, scache_handle, additional_scache_size);
        if(BCM_FAILURE(rv)) {
           return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_sync
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
_bcm_tr3_l2gre_sync(int unit)
{
    int i, num_vp = 0;
    int stable_size;
    uint8 *l2gre_state;
    soc_scache_handle_t scache_handle;
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    bcm_gport_t gport;

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    l2gre_info = L2GRE_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_L2GRE, 0);
    BCM_IF_ERROR_RETURN(
        _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                0, &l2gre_state, BCM_WB_DEFAULT_VERSION, NULL));


    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    /* Store the L2GRE Tunnnel Terminator information */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(l2gre_state, &(l2gre_info->l2gre_tunnel_init[i].dip),
                   sizeof(bcm_ip_t));
        l2gre_state += sizeof(bcm_ip_t);
        sal_memcpy(l2gre_state, &(l2gre_info->l2gre_tunnel_init[i].sip),
                   sizeof(bcm_ip_t));
        l2gre_state += sizeof(bcm_ip_t);
        sal_memcpy(l2gre_state, &(l2gre_info->l2gre_tunnel_init[i].tunnel_state),
                   sizeof(uint16));
        l2gre_state += sizeof(uint16);
    }

    /* Store the L2GRE Tunnnel Initiator information */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(l2gre_state, &(l2gre_info->l2gre_tunnel_term[i].dip),
                   sizeof(bcm_ip_t));
        l2gre_state += sizeof(bcm_ip_t);
        sal_memcpy(l2gre_state, &(l2gre_info->l2gre_tunnel_term[i].sip),
                   sizeof(bcm_ip_t));
        l2gre_state += sizeof(bcm_ip_t);
        sal_memcpy(l2gre_state, &(l2gre_info->l2gre_tunnel_term[i].tunnel_state),
                   sizeof(uint16));
        l2gre_state += sizeof(uint16);
    }

    /* Store the flags & match_tunnel_index of each Match Key */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(l2gre_state, &(l2gre_info->match_key[i].flags),
                   sizeof(uint32));
        l2gre_state += sizeof(uint32);
        sal_memcpy(l2gre_state, &(l2gre_info->match_key[i].match_tunnel_index),
                   sizeof(uint32));
        l2gre_state += sizeof(uint32);
    }

    if (soc_feature(unit, soc_feature_td2_l2gre) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        /*
         * Since we can't recover index && gport(trunk_id, modid, port) &&
         * match_vlan && match_inner_vlan from HW if we add this l2gre port to
         * VPLAG, so we need to store it for warmboot recovery.
         */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(l2gre_state, &(l2gre_info->match_key[i].index),
                       sizeof(uint32));
            l2gre_state += sizeof(uint32);
            if (l2gre_info->match_key[i].trunk_id != -1) {
                BCM_GPORT_TRUNK_SET(gport, l2gre_info->match_key[i].trunk_id);
            } else if (l2gre_info->match_key[i].modid != -1) {
                BCM_GPORT_MODPORT_SET(gport,
                                      l2gre_info->match_key[i].modid,
                                      l2gre_info->match_key[i].port);
            } else {
                gport = BCM_GPORT_INVALID;
            }
            sal_memcpy(l2gre_state, &gport, sizeof(bcm_gport_t));
            l2gre_state += sizeof(bcm_gport_t);
            sal_memcpy(l2gre_state, &(l2gre_info->match_key[i].match_vlan),
                       sizeof(bcm_vlan_t));
            l2gre_state += sizeof(bcm_vlan_t);
            sal_memcpy(l2gre_state,
                       &(l2gre_info->match_key[i].match_inner_vlan),
                       sizeof(bcm_vlan_t));
            l2gre_state += sizeof(bcm_vlan_t);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_reinit
 * Purpose:
 *      Warm boot recovery for the L2GRE software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2gre_reinit(int unit)
{
    soc_mem_t mem;
    int i, index_min, index_max;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int port_type, vp, vxlate_flag;
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    uint32 trunk, tgid, mod_id, port_num, key_type;
    soc_field_t validf, keytypef;
    int keytype_ovid, keytype_ivid, keytype_iovid, keytype_cfi;
    soc_field_t type_field;
    int match_key_update_flag = FALSE;
    soc_field_t tgid_fld = TGIDf;

    l2gre_info = L2GRE_INFO(unit);

    /* Recover tunnel terminator endpoints */
    mem = EGR_IP_TUNNELm;
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_IP_TUNNELm, DATA_TYPEf)) {
        type_field = DATA_TYPEf;
    } else
#endif
    {
        type_field = ENTRY_TYPEf;
    }

    for (i = index_min; i <= index_max; i++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, entry));

        /* Looking for IPV4 tunnels of type L2GRE only */
        if (soc_EGR_IP_TUNNELm_field32_get(unit, entry,
                                           type_field) != 0x1) {
            continue;
        }

        if (soc_EGR_IP_TUNNELm_field32_get(unit, entry,
                                           TUNNEL_TYPEf) != 0x7) {
            continue;
        }

        l2gre_info->l2gre_tunnel_term[i].sip =
            soc_EGR_IP_TUNNELm_field32_get(unit, entry, SIPf);

        l2gre_info->l2gre_tunnel_term[i].dip =
            soc_EGR_IP_TUNNELm_field32_get(unit, entry, DIPf);

    }

    /* Recover l2gre match key */

    /* Recovery based on entry in VLAN_XLATE tbl */
    if (soc_mem_is_valid(unit, VLAN_XLATE_EXTDm)) {
        mem = VLAN_XLATE_EXTDm;
        validf = VALID_0f;
        keytypef = KEY_TYPE_0f;
    } else
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        validf = BASE_VALID_0f;
        keytypef = KEY_TYPEf;
    } else
#endif
    if (soc_mem_is_valid(unit, VLAN_XLATEm)) {
        mem = VLAN_XLATEm;
        validf = VALIDf;
        keytypef = KEY_TYPEf;
    } else {
        return BCM_E_INTERNAL;
    }

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
        keytype_ovid = VLXLT_HASH_KEY_TYPE_OVID;
        keytype_ivid =  VLXLT_HASH_KEY_TYPE_IVID;
        keytype_iovid = VLXLT_HASH_KEY_TYPE_IVID_OVID;
        keytype_cfi = VLXLT_HASH_KEY_TYPE_PRI_CFI;
    } else
#endif
    {
        /*coverity[mixed_enums]*/
        keytype_ovid = TR3_VLXLT_X_HASH_KEY_TYPE_OVID;
        /*coverity[mixed_enums]*/
        keytype_ivid = TR3_VLXLT_X_HASH_KEY_TYPE_IVID;
        /*coverity[mixed_enums]*/
        keytype_iovid = TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID;
        /*coverity[mixed_enums]*/
        keytype_cfi = TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    for (i = index_min; i <= index_max; i++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, entry));
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            if (soc_mem_field32_get(unit, mem, entry, validf) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 7) {
                continue;
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                continue;
            }
        }

        if (soc_mem_field32_get(unit, mem, entry, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }

        vxlate_flag = FALSE;
        match_key_update_flag = FALSE;

        vp = soc_mem_field32_get(unit, mem, entry, XLATE__SOURCE_VPf);

        /* Proceed only if this VP belongs to L2GRE */
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
             _bcm_vp_info_t vp_info;
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
            if (vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
                l2gre_info->match_key[vp].flags =
                    _BCM_L2GRE_PORT_MATCH_TYPE_SHARED;
            } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
            {
                match_key_update_flag = TRUE;
            }

            key_type = soc_mem_field32_get(unit, mem, entry, keytypef);
            if ((keytype_ovid == key_type) || (keytype_ivid == key_type) ||
                (keytype_iovid == key_type) || (keytype_cfi == key_type)) {
                vxlate_flag = TRUE;
            }
        }

        if (match_key_update_flag) {
            trunk = soc_mem_field32_get(unit, mem, entry, XLATE__Tf);
            tgid = soc_mem_field32_get(unit, mem, entry, XLATE__TGIDf);
            mod_id = soc_mem_field32_get(unit, mem, entry, XLATE__MODULE_IDf);
            port_num = soc_mem_field32_get(unit, mem, entry, XLATE__PORT_NUMf);

            if (keytype_ovid == key_type) {
                l2gre_info->match_key[vp].flags =
                                                _BCM_L2GRE_PORT_MATCH_TYPE_VLAN;
                l2gre_info->match_key[vp].match_vlan =
                    soc_mem_field32_get(unit, mem, entry, XLATE__OVIDf);
            } else if (keytype_ivid == key_type) {
                l2gre_info->match_key[vp].flags =
                                          _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN;
                l2gre_info->match_key[vp].match_inner_vlan =
                     soc_mem_field32_get(unit, mem, entry, XLATE__IVIDf);
            } else if (keytype_iovid == key_type) {
                l2gre_info->match_key[vp].flags =
                                        _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED;
                l2gre_info->match_key[vp].match_vlan =
                    soc_mem_field32_get(unit, mem, entry, XLATE__OVIDf);
                l2gre_info->match_key[vp].match_inner_vlan =
                     soc_mem_field32_get(unit, mem, entry, XLATE__IVIDf);
            } else if (keytype_cfi == key_type) {
                l2gre_info->match_key[vp].flags =
                                            _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI;
                l2gre_info->match_key[vp].match_vlan =
                    soc_mem_field32_get(unit, mem, entry, OTAGf);
            }

            if (vxlate_flag) {
                if (trunk) {
                    l2gre_info->match_key[vp].trunk_id = tgid;
                } else {
                    l2gre_info->match_key[vp].port = port_num;
                    l2gre_info->match_key[vp].modid = mod_id;
                }
            }
        }

        if (vxlate_flag) {
            bcm_tr3_l2gre_port_match_count_adjust(unit, vp, 1);
        }
    }

    /* Recovery based on entry in MPLS tbl */
    mem = MPLS_ENTRYm;
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    for (i = index_min; i <= index_max; i++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, entry));
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            if (soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 7) {
                continue;
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem, entry, VALIDf) == 0) {
                continue;
            }
        }

        /* Looking for entries of type l2gre only */
        key_type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);
        if ((key_type != 0x8) && (key_type != 0x6)) {
            continue;
        }

        vp = soc_mem_field32_get(unit, mem, entry, L2GRE_SIP__SVPf);
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_VPNID;
        }
    }

    /* Recovery based on entry in SOURCE_TRUNK_MAP tbl */
    mem = SOURCE_TRUNK_MAP_TABLEm;
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    if (SOC_IS_TRIDENT3X(unit) && soc_mem_field_valid(unit, mem, SRC_TGIDf)) {
        tgid_fld = SRC_TGIDf;
    }

    for (i = index_min; i <= index_max; i++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, entry));

        vp = soc_mem_field32_get(unit, mem, entry, SOURCE_VPf);

        /* Proceed only if this VP belongs to L2GRE */
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {

            port_type = soc_mem_field32_get(unit, mem, entry, PORT_TYPEf);
            if (port_type) { /* trunk port */
                l2gre_info->match_key[vp].flags =_BCM_L2GRE_PORT_MATCH_TYPE_TRUNK;
                l2gre_info->match_key[vp].trunk_id =
                    soc_mem_field32_get(unit, mem, entry, tgid_fld);
            } else {
                l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_PORT;
                l2gre_info->match_key[vp].index = i;
            }
        }
    }

    /* Recover L2 scache */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_wb_recover(unit));

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_tr3_l2gre_init
 * Purpose:
 *      Initialize the L2GRE software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


int
bcm_tr3_l2gre_init(int unit)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int i, num_tnl=0, num_vp=0;
    int rv = BCM_E_NONE;

    if (!L3_INFO(unit)->l3_initialized) {
        LOG_ERROR(BSL_LS_BCM_L2GRE,
                  (BSL_META_U(unit,
                              "L3 module must be initialized prior to L2GRE Init\n")));
        return BCM_E_CONFIG;
    }

    /* Allocate BK Info */
    BCM_IF_ERROR_RETURN(bcm_tr3_l2gre_allocate_bk(unit));
    l2gre_info = L2GRE_INFO(unit);

    /*
     * allocate resources
     */
    if (l2gre_info->initialized) {
         BCM_IF_ERROR_RETURN(bcm_tr3_l2gre_cleanup(unit));
         BCM_IF_ERROR_RETURN(bcm_tr3_l2gre_allocate_bk(unit));
         l2gre_info = L2GRE_INFO(unit);
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    l2gre_info->match_key =
        sal_alloc(sizeof(_bcm_l2gre_match_port_info_t) * num_vp, "match_key");
    if (l2gre_info->match_key == NULL) {
        _bcm_tr3_l2gre_free_resource(unit);
        return BCM_E_MEMORY;
    }

    sal_memset(l2gre_info->match_key, 0,
            sizeof(_bcm_l2gre_match_port_info_t) * num_vp);
    /* Stay same after recover */
    for (i = 0; i < num_vp; i++) {
        bcm_tr3_l2gre_match_clear(unit, i);
    }

    /* Create EGR_IP_TUNNEL usage bitmap */
    num_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);

    l2gre_info->l2gre_ip_tnl_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_tnl), "l2gre_ip_tnl_bitmap");
    if (l2gre_info->l2gre_ip_tnl_bitmap == NULL) {
        _bcm_tr3_l2gre_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(l2gre_info->l2gre_ip_tnl_bitmap, 0, SHR_BITALLOCSIZE(num_tnl));

    /* Create L2GRE protection mutex. */
    l2gre_info->l2gre_mutex = sal_mutex_create("l2gre_mutex");
    if (!l2gre_info->l2gre_mutex) {
         _bcm_tr3_l2gre_free_resource(unit);
         return BCM_E_MEMORY;
    }

    if (NULL == l2gre_info->l2gre_tunnel_term) {
        l2gre_info->l2gre_tunnel_term =
            sal_alloc(sizeof(_bcm_l2gre_tunnel_endpoint_t) * num_vp, "L2gre tunnel term store");
        if (l2gre_info->l2gre_tunnel_term == NULL) {
            _bcm_tr3_l2gre_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(l2gre_info->l2gre_tunnel_term, 0,
                sizeof(_bcm_l2gre_tunnel_endpoint_t) * num_vp);
    }

    if (NULL == l2gre_info->l2gre_tunnel_init) {
        l2gre_info->l2gre_tunnel_init =
            sal_alloc(sizeof(_bcm_l2gre_tunnel_endpoint_t) * num_vp, "L2gre tunnel init store");
        if (l2gre_info->l2gre_tunnel_init == NULL) {
            _bcm_tr3_l2gre_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(l2gre_info->l2gre_tunnel_init, 0,
                sizeof(_bcm_l2gre_tunnel_endpoint_t) * num_vp);
    }

#ifdef BCM_WARM_BOOT_SUPPORT

    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_tr3_l2gre_reinit(unit);
    } else {
        rv = _bcm_tr3_l2gre_wb_alloc(unit);
    }
    if (BCM_FAILURE(rv)) {
        _bcm_tr3_l2gre_free_resource(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Mark the state as initialized */
    l2gre_info->initialized = TRUE;

    return rv;
}

 /*
  * Function:
  * 	 _bcm_tr3_l2gre_bud_loopback_enable
  * Purpose:
  * 	 Enable loopback for L2GRE BUD node multicast
  * Parameters:
  *   IN : unit
  * Returns:
  * 	 BCM_E_XXX
  */

STATIC int
_bcm_tr3_l2gre_bud_loopback_enable(int unit)
{
    int field_num = 0;

    soc_field_t lport_fields[] = {
        L2GRE_TERMINATION_ALLOWEDf,
        V4IPMC_ENABLEf,
        L2GRE_VPNID_LOOKUP_KEY_TYPEf,
        PORT_TYPEf,
        L2GRE_DEFAULT_SVP_ENABLEf
    };
    uint32 lport_values[] = {
        0x1,  /* L2GRE_TERMINATION_ALLOWEDf */
        0x1,  /* V4IPMC_ENABLEf */
        0x0,  /* L2GRE_VPNID_LOOKUP_KEY_TYPEf */
        0x0,  /* PORT_TYPEf */
        0x1   /* L2GRE_DEFAULT_SVP_ENABLEf */
    };

    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, L2GRE_DEFAULT_SVP_ENABLEf)) {
        field_num = 5;
    } else {
        field_num = 3; /* TD3TBD */
    }

    /* Update LPORT Profile Table */
    return _bcm_lport_profile_fields32_modify(unit, LPORT_PROFILE_LPORT_TAB,
                                        field_num, lport_fields, lport_values);
}

 /*
  * Function:
  * 	 _bcm_tr3_l2gre_bud_loopback_disable
  * Purpose:
  * 	 Disable loopback for L2GRE BUD node multicast
  * Parameters:
  *   IN : unit
  * Returns:
  * 	 BCM_E_XXX
  */

STATIC int
_bcm_tr3_l2gre_bud_loopback_disable(int unit)
{
    int field_num = 0;

    soc_field_t lport_fields[] = {
        L2GRE_TERMINATION_ALLOWEDf,
        L2GRE_VPNID_LOOKUP_KEY_TYPEf,
        V4IPMC_ENABLEf,
        PORT_TYPEf,
        L2GRE_DEFAULT_SVP_ENABLEf
    };
    uint32 lport_values[] = {
        0x0,  /* L2GRE_TERMINATION_ALLOWEDf */
        0x0,  /* L2GRE_VPNID_LOOKUP_KEY_TYPEf */
        0x0,  /* V4IPMC_ENABLEf */
        0x0,  /* PORT_TYPEf */
        0x0,  /*  L2GRE_DEFAULT_SVP_ENABLEf */
    };

    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, L2GRE_DEFAULT_SVP_ENABLEf)) {
        field_num = 5;
    } else if (SOC_IS_TRIDENT3X(unit)) {
        field_num = 2;
    } else {
        field_num = 3;
    }

    /* Update LPORT Profile Table */
    return _bcm_lport_profile_fields32_modify(unit, LPORT_PROFILE_LPORT_TAB,
                                        field_num, lport_fields, lport_values);
}

 /* Function:
 *      _bcm_tr3_l2gre_vpn_is_valid
 * Purpose:
 *      Find if given L2GRE VPN is Valid
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - L2GRE VPN
 * Returns:
 *      BCM_E_XXXX
 */
int
_bcm_tr3_l2gre_vpn_is_valid( int unit, bcm_vpn_t l2vpn)
{
    bcm_vpn_t l2gre_vpn_min=0;
    int vfi_index=-1, num_vfi=0;

    num_vfi = soc_mem_index_count(unit, VFIm);

    /* Check for Valid vpn */
     _BCM_L2GRE_VPN_SET(l2gre_vpn_min, _BCM_VPN_TYPE_VFI, 0);
     if ( l2vpn < l2gre_vpn_min || l2vpn > (l2gre_vpn_min+num_vfi-1) ) {
        return BCM_E_PARAM;
     }

    _BCM_L2GRE_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI,  l2vpn);
    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

 /* Function:
 *      _bcm_tr3_l2gre_vpn_is_eline
 * Purpose:
 *      Find if given L2GRE VPN is ELINE
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - L2GRE VPN
 *      isEline  - Flag
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      l2vpn is valid
 */

STATIC int
_bcm_tr3_l2gre_vpn_is_eline( int unit, bcm_vpn_t l2vpn, uint8 *isEline)
{
    int vfi_index=-1;
    vfi_entry_t vfi_entry;

    BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_valid(unit, l2vpn));

    _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELINE, l2vpn);
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        *isEline = 1;  /* ELINE */
    } else {
        *isEline = 0;  /* ELAN */
    }

    return BCM_E_NONE;
}

 /* Function:
 *      _bcm_tr3_l2gre_vp_is_eline
 * Purpose:
 *      Find if given L2GRE VP is ELINE
 * Parameters:
 *      unit     - Device Number
 *      vp   - L2GRE VP
 *      isEline  - Flag
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      l2vpn is valid
 */

int
_bcm_tr3_l2gre_vp_is_eline( int unit, int vp, uint8 *isEline)
{
    source_vp_entry_t svp;
    vfi_entry_t vfi_entry;
    int rv = BCM_E_PARAM;
    int vfi_index;

    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }

    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        return rv;
    }

    vfi_index = soc_SOURCE_VPm_field32_get(unit, &svp, VFIf);
    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    /* Set the returned VPN id */
    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
       *isEline = 0x1;
    }  else {
       *isEline = 0x0;
   }
   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_nexthop_glp_get
 * Purpose:
 *      Get the modid, port, trunk values for a L2GRE nexthop
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2gre_nexthop_glp_get(int unit, int nh_idx,
        bcm_module_t *modid, bcm_port_t *port, bcm_trunk_t *trunk_id)

{
    ing_l3_next_hop_entry_t ing_nh;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit,
        ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_idx, &ing_nh));

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dt, dv;

        dv = soc_mem_field32_dest_get(unit,
                ING_L3_NEXT_HOPm, &ing_nh, DESTINATIONf, &dt);
        if (dt == SOC_MEM_FIF_DEST_LAG) {
            *trunk_id = dv & SOC_MEM_FIF_DGPP_TGID_MASK;
        } else if (dt == SOC_MEM_FIF_DEST_DGPP) {
            *port = (dv & SOC_MEM_FIF_DGPP_PORT_MASK);
            *modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                        SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
        } else if (dt == SOC_MEM_FIF_DEST_DVP) {
            return BCM_E_NOT_FOUND;
        } else {
            return BCM_E_INTERNAL;
        }
    } else
#endif
    {
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
        } else {
            /* Only add this to replication set if destination is local */
            *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a L2GRE port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l2gre_port_resolve(int unit, bcm_gport_t l2gre_port_id,
                          bcm_if_t encap_id, bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE;
    egr_l3_next_hop_entry_t egr_nh;
    ing_dvp_table_entry_t dvp;
    int ecmp, nh_index, nh_ecmp_index, vp;
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry. */
    int  idx, max_ent_count, base_idx;

    rv = _bcm_l2gre_check_init(unit);
    if (rv < 0) {
        return rv;
    }

    if (!BCM_GPORT_IS_L2GRE_PORT(l2gre_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port_id);
    if (vp == -1) {
       return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (!ecmp) {
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_nexthop_glp_get(unit,
                                nh_index, modid, port, trunk_id));
    } else {
        /* Select the desired nhop from ECMP group - pointed by encap_id */
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit,
           L3_ECMP_COUNTm, MEM_BLOCK_ANY, nh_ecmp_index, hw_buf));

        if (soc_feature(unit, soc_feature_l3_ecmp_1k_groups)) {
             max_ent_count = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                                                hw_buf, COUNTf);
             base_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                                                hw_buf, BASE_PTRf);
        }  else {
             max_ent_count = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                                                hw_buf, COUNT_0f);
             base_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                                                hw_buf, BASE_PTR_0f);
        }
        max_ent_count++; /* Count is zero based. */

        if(encap_id == -1) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                L3_ECMPm, MEM_BLOCK_ANY, base_idx, hw_buf));
            nh_index = soc_mem_field32_get(unit,
                        L3_ECMPm, hw_buf, NEXT_HOP_INDEXf);

            BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_nexthop_glp_get(unit,
                                    nh_index, modid, port, trunk_id));
        } else {
            for (idx = 0; idx < max_ent_count; idx++) {
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMPm,
                          MEM_BLOCK_ANY, (base_idx+idx), hw_buf));
                nh_index = soc_mem_field32_get(unit,
                              L3_ECMPm, hw_buf, NEXT_HOP_INDEXf);
                BCM_IF_ERROR_RETURN (soc_mem_read(unit,
                  EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, &egr_nh));
                if (encap_id == soc_mem_field32_get(unit,
                                    EGR_L3_NEXT_HOPm, &egr_nh, INTF_NUMf)) {
                    BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_nexthop_glp_get(unit,
                                            nh_index, modid, port, trunk_id));
                  break;
                }
            }
        }
    }
    *id = vp;
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_port_nh_cnt_dec
 * Purpose:
 *      Decrease L2GRE nexthop count.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      vp    - (IN) Virtual port number.
 * Returns:
 *      BCM_E_XXX
 */


STATIC int
_bcm_tr3_l2gre_port_nh_cnt_dec(int unit, int vp)
{
    int nh_ecmp_index = -1;
    ing_dvp_table_entry_t dvp;
    uint32 flags = 0;
    int ref_count = 0;
    int ecmp = -1;

    BCM_IF_ERROR_RETURN(
        READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (ecmp) {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
        flags = BCM_L3_MULTIPATH;
        /* Decrement reference count */
        BCM_IF_ERROR_RETURN(
             bcm_xgs3_get_ref_count_from_nhi(unit, flags, &ref_count, nh_ecmp_index));
    } else {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
        if(nh_ecmp_index != 0) {
             /* Decrement reference count */
             BCM_IF_ERROR_RETURN(
                  bcm_xgs3_get_ref_count_from_nhi(unit, flags, &ref_count, nh_ecmp_index));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_port_cnt_update
 * Purpose:
 *      Update port's VP count.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      gport - (IN) GPORT ID.
 *      vp    - (IN) Virtual port number.
 *      incr  - (IN) If TRUE, increment VP count, else decrease VP count.
 * Returns:
 *      BCM_X_XXX
 */

STATIC int
_bcm_tr3_l2gre_port_cnt_update(int unit, bcm_gport_t gport,
        int vp, int incr_decr_flag)
{
    int mod_out=-1, port_out=-1, tgid_out=-1, vp_out=-1;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int idx=-1;
    int mod_local=-1;
    _bcm_port_info_t *port_info;
    uint32 port_flags;

    BCM_IF_ERROR_RETURN(
       _bcm_tr3_l2gre_port_resolve(unit, gport, -1, &mod_out,
                    &port_out, &tgid_out, &vp_out));

    if (vp_out == -1) {
       return BCM_E_PARAM;
    }

    /* Update the physical port's SW state. If associated with a trunk,
     * update each local physical port's SW state.
     */

    if (BCM_TRUNK_INVALID != tgid_out) {

        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid_out,
                    SOC_MAX_NUM_PORTS, local_member_array, &local_member_count));

        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &port_info);
            if (incr_decr_flag) {
                port_info->vp_count++;
            } else {
                port_info->vp_count--;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, local_member_array[idx], &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, local_member_array[idx], port_flags));
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        if (mod_local) {
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
            _bcm_port_info_access(unit, port_out, &port_info);
            if (incr_decr_flag) {
                port_info->vp_count++;
            } else {
                port_info->vp_count--;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, port_out, &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, port_out, port_flags));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2gre_vpn_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_vpn_create(int unit, bcm_l2gre_vpn_config_t *info)
{
    int rv = BCM_E_PARAM;
    vfi_entry_t vfi_entry;
    soc_mem_t mem;
    uint32 vxlate[SOC_MAX_MEM_WORDS];
    int vfi_index;
    int bc_group=0, umc_group=0, uuc_group=0;
    int bc_group_type, umc_group_type, uuc_group_type;
    uint8 vpn_alloc_flag = 0;
    uint8 vpnid_alloc_flag = 0;
    uint32 vpnid = 0;

    /* Allocate VFI */
    if (info->flags & BCM_L2GRE_VPN_REPLACE) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_valid(unit, info->vpn));
        _BCM_L2GRE_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, info->vpn);
    } else if (info->flags & BCM_L2GRE_VPN_WITH_ID) {
        rv = _bcm_tr3_l2gre_vpn_is_valid(unit, info->vpn);
        if (BCM_E_NONE == rv) {
            return BCM_E_EXISTS;
        } else if (BCM_E_NOT_FOUND != rv) {
            return rv;
        }

        _BCM_L2GRE_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, info->vpn);
        BCM_IF_ERROR_RETURN(_bcm_vfi_alloc_with_id(unit, VFIm, _bcmVfiTypeL2Gre, vfi_index));
        vpn_alloc_flag = 1;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_vfi_alloc(unit, VFIm, _bcmVfiTypeL2Gre, &vfi_index));
        _BCM_L2GRE_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi_index);
        vpn_alloc_flag = 1;
    }

    /* Initial and configurate VFI */
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));
    if (info->flags & BCM_L2GRE_VPN_ELINE) {
        soc_VFIm_field32_set(unit, &vfi_entry, PT2PT_ENf, 0x1);
    } else if (info->flags & BCM_L2GRE_VPN_ELAN) {
        /* Check that the groups are valid. */
        bc_group_type = _BCM_MULTICAST_TYPE_GET(info->broadcast_group);
        bc_group = _BCM_MULTICAST_ID_GET(info->broadcast_group);
        umc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_multicast_group);
        umc_group = _BCM_MULTICAST_ID_GET(info->unknown_multicast_group);
        uuc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_unicast_group);
        uuc_group = _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);

        if ((bc_group_type != _BCM_MULTICAST_TYPE_L2GRE) ||
            (umc_group_type != _BCM_MULTICAST_TYPE_L2GRE) ||
            (uuc_group_type != _BCM_MULTICAST_TYPE_L2GRE) ||
            (bc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (umc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (uuc_group >= soc_mem_index_count(unit, L3_IPMCm))) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }

        /* Configurate VFI for ELAN vpn */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, VFIm, &vfi_entry,
                UMC_DESTINATIONf, SOC_MEM_FIF_DEST_IPMC, umc_group);
            soc_mem_field32_dest_set(unit, VFIm, &vfi_entry,
                UUC_DESTINATIONf, SOC_MEM_FIF_DEST_IPMC, uuc_group);
            soc_mem_field32_dest_set(unit, VFIm, &vfi_entry,
                BC_DESTINATIONf, SOC_MEM_FIF_DEST_IPMC, bc_group);
        } else
#endif
        {
            soc_VFIm_field32_set(unit, &vfi_entry, UMC_INDEXf, umc_group);
            soc_VFIm_field32_set(unit, &vfi_entry, UUC_INDEXf, uuc_group);
            soc_VFIm_field32_set(unit, &vfi_entry, BC_INDEXf, bc_group);
        }
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        uint32 profile_idx = 0;
        rv = _bcm_td3_def_vfi_profile_add(unit,
            info->flags & BCM_L2GRE_VPN_ELINE, &profile_idx);
        if (BCM_E_NONE == rv) {
            soc_VFIm_field32_set(unit, &vfi_entry, VFI_PROFILE_PTRf, profile_idx);
        }
        _BCM_L2GRE_CLEANUP(rv);
    }
#endif
    /* Write VFI to ASIC */
    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
    _BCM_L2GRE_CLEANUP(rv);

    if (info->flags & BCM_L2GRE_VPN_WITH_VPNID) {
        /* Configurate mpls entry for ingress VPNID */
        rv = _bcm_tr3_l2gre_match_vpnid_entry_set(unit, info);
        _BCM_L2GRE_CLEANUP(rv);
        vpnid_alloc_flag = 1;

        /* Configurate EGR_VLAN_XLATE for Egress VPNID */
        rv = _bcm_tr3_l2gre_egr_xlate_entry_set(unit, info);
        _BCM_L2GRE_CLEANUP(rv);
    } else {
        /* Reset vpnid entry */
        rv = _bcm_tr3_l2gre_egr_xlate_entry_get(unit, vfi_index, &mem, vxlate);
        if (BCM_E_NOT_FOUND == rv) {
            return BCM_E_NONE;
        } else if (BCM_E_NONE == rv) {
            vpnid = soc_mem_field32_get(unit, mem, vxlate, L2GRE_VFI__VPNIDf);
            rv = _bcm_tr3_l2gre_match_vpnid_entry_reset(unit, vpnid);
            _BCM_L2GRE_CLEANUP(rv);
        } else {
            goto cleanup;
        }

        /* Reset egr_vxlate entry */
        rv = _bcm_tr3_l2gre_egr_xlate_entry_reset(unit, info->vpn);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;

cleanup:
    if (vpnid_alloc_flag) {
        _bcm_tr3_l2gre_match_vpnid_entry_reset(unit, info->vpnid);
    }
    if ( vpn_alloc_flag ) {
        _bcm_vfi_free(unit, _bcmVfiTypeVxlan, vfi_index);
    }

    return rv;
}

/* Function:
 *      _bcm_tr3_xlate_entry_get
 * Purpose:
 *      Get EGR_VLAN_XLATE per VPN instance
 * Parameters:
 *      unit          - Device Number
 *      vfi           - l2gre VPN
 *      vxlate_entry  - EGR_VLAN_XLATE entry
 * Returns:
 *      BCM_E_XXXX
 */

STATIC int
_bcm_tr3_l2gre_egr_xlate_entry_get( int unit, int vfi, soc_mem_t *mem, uint32 *vxlate_entry)
{
    int index=0;

    sal_memset(vxlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        *mem = EGR_VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, *mem, vxlate_entry, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, *mem, vxlate_entry, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, *mem, vxlate_entry,
            KEY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI);
        soc_mem_field32_set(unit, *mem, vxlate_entry,
            DATA_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI);
    } else
#endif
    {
        *mem = EGR_VLAN_XLATEm;

        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, *mem, vxlate_entry,
                KEY_TYPEf, _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_VFI);
        } else if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
            soc_mem_field32_set(unit, *mem, vxlate_entry,
                ENTRY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI);
        }
        soc_mem_field32_set(unit, *mem, vxlate_entry, VALIDf, 1);
    }
    soc_mem_field32_set(unit, *mem, vxlate_entry, L2GRE_VFI__VFIf, vfi);

    BCM_IF_ERROR_RETURN(soc_mem_search(unit, *mem,
        MEM_BLOCK_ANY, &index, vxlate_entry, vxlate_entry, 0));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_tr3_l2gre_vpn_destroy
 * Purpose:
 *      Delete a VPN instance
 * Parameters:
 *      unit - Device Number
 *      vpn - VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_vpn_destroy(int unit, bcm_vpn_t l2vpn)
{
    int vfi = 0;
    soc_mem_t mem;
    uint32 vxlate[SOC_MAX_MEM_WORDS];
    int rv;
    uint8 isEline;
    uint32 vpnid = 0;

    /* Get vfi index */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_eline(unit, l2vpn, &isEline));
    if (isEline == 0x1 ) {
        _BCM_L2GRE_VPN_GET(vfi, _BCM_L2GRE_VPN_TYPE_ELINE, l2vpn);
    } else if (isEline == 0x0 ) {
        _BCM_L2GRE_VPN_GET(vfi, _BCM_L2GRE_VPN_TYPE_ELAN, l2vpn);
    }

    rv = _bcm_tr3_l2gre_egr_xlate_entry_get(unit, vfi, &mem, vxlate);
    if (BCM_E_NONE == rv) {
        /* Reset match vnid table */
        vpnid = soc_mem_field32_get(unit, mem, vxlate, L2GRE_VFI__VPNIDf);
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_match_vpnid_entry_reset(unit, vpnid));

        /* Delete egr_vxlate entry */
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_egr_xlate_entry_reset(unit, l2vpn));
    } else if (BCM_E_NOT_FOUND != rv) {
        return rv;
    }

    /* Delete all L2GRE ports on this VPN */
    rv = bcm_tr3_l2gre_port_delete_all(unit, l2vpn);
    BCM_IF_ERROR_RETURN(rv);


    /* Reset VFI table */
    (void)_bcm_vfi_free(unit, _bcmVfiTypeL2Gre, vfi);
    return BCM_E_NONE;
}

 /* Function:
 *      bcm_tr3_l2gre_vpn_id_destroy_all
 * Purpose:
 *      Delete all L2-VPN instances
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_tr3_l2gre_vpn_destroy_all(int unit)
{
    int num_vfi, idx;
    bcm_vpn_t l2vpn;

    /* Destroy all L2GRE VPNs */
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (idx = 0; idx < num_vfi; idx++) {
         if (_bcm_vfi_used_get(unit, idx, _bcmVfiTypeL2Gre)) {
             _BCM_L2GRE_VPN_SET(l2vpn, _BCM_VPN_TYPE_VFI, idx);
             BCM_IF_ERROR_RETURN(bcm_tr3_l2gre_vpn_destroy(unit, l2vpn));
         }
    }
    return BCM_E_NONE;
}


 /* Function:
 *      bcm_tr3_l2gre_vpn_get
 * Purpose:
 *      Get L2-VPN instance
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - L2GRE VPN
 *      info     - L2GRE VPN Config
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_tr3_l2gre_vpn_get( int unit, bcm_vpn_t l2vpn, bcm_l2gre_vpn_config_t *info)
{
    int vfi_index = 0;
    vfi_entry_t vfi_entry;
    soc_mem_t mem;
    uint32 vxlate[SOC_MAX_MEM_WORDS];
    uint8 isEline;
    int rv = BCM_E_NONE;

    /* Get vfi index */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_eline(unit, l2vpn, &isEline));
    if (isEline == 0x1 ) {
        _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELINE, l2vpn);
    } else if (isEline == 0x0 ) {
        _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELAN, l2vpn);
    }
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    /* Set the returned VPN id */
    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        info->flags =  BCM_L2GRE_VPN_ELINE;
    } else {
        info->flags =  BCM_L2GRE_VPN_ELAN;
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_generic_dest)) {
            uint32 dt, dv;

            dv = soc_mem_field32_dest_get(unit, VFIm,
                        &vfi_entry, BC_DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_IPMC) {
                _BCM_MULTICAST_GROUP_SET(info->broadcast_group,
                                    _BCM_MULTICAST_TYPE_L2GRE, dv);
            } else {
                return BCM_E_INTERNAL;
            }

            dv = soc_mem_field32_dest_get(unit, VFIm,
                        &vfi_entry, UUC_DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_IPMC) {
                _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group,
                                        _BCM_MULTICAST_TYPE_L2GRE, dv);
            } else {
                return BCM_E_INTERNAL;
            }

            dv = soc_mem_field32_dest_get(unit, VFIm,
                        &vfi_entry, UMC_DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_IPMC) {
                _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group,
                                            _BCM_MULTICAST_TYPE_L2GRE, dv);
            } else {
                return BCM_E_INTERNAL;
            }
        } else
#endif
        {
            _BCM_MULTICAST_GROUP_SET(info->broadcast_group,
                                _BCM_MULTICAST_TYPE_L2GRE,
                 soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf));
            _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group,
                                _BCM_MULTICAST_TYPE_L2GRE,
                 soc_VFIm_field32_get(unit, &vfi_entry, UUC_INDEXf));
            _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group,
                                _BCM_MULTICAST_TYPE_L2GRE,
                 soc_VFIm_field32_get(unit, &vfi_entry, UMC_INDEXf));
        }
    }
    _BCM_L2GRE_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi_index);

    rv = _bcm_tr3_l2gre_egr_xlate_entry_get(unit, vfi_index, &mem, vxlate);
    if (BCM_E_NONE == rv) {
        /* Get VPNID */
        info->flags |= BCM_L2GRE_VPN_WITH_VPNID;
        info->vpnid = soc_mem_field32_get(unit, mem, vxlate, L2GRE_VFI__VPNIDf);

        /* Get SDTAG settings */
        (void)_bcm_tr3_l2gre_sd_tag_get( unit, info, NULL, NULL, vxlate, 1);
    } else if (BCM_E_NOT_FOUND != rv) {
        return rv;
    }

    return rv;
}

 /* Function:
 *      _bcm_tr3_l2gre_vpn_get
 * Purpose:
 *      Get L2-VPN instance for L2GRE
 * Parameters:
 *      unit     - Device Number
 *      vfi_index   - vfi_index
 *      vid     (OUT) VLAN Id (l2vpn id)
 * Returns:
 *      BCM_E_XXXX
 */
int
_bcm_tr3_l2gre_vpn_get(int unit, int vfi_index, bcm_vlan_t *vid)
{
    vfi_entry_t vfi_entry;

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        _BCM_L2GRE_VPN_SET(*vid, _BCM_L2GRE_VPN_TYPE_ELINE, vfi_index);
    } else {
        _BCM_L2GRE_VPN_SET(*vid, _BCM_L2GRE_VPN_TYPE_ELAN, vfi_index);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2gre_vpn_traverse
 * Purpose:
 *      Get information about a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb    - (IN)  User-provided callback
 *      info  - (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXX
 */

int bcm_tr3_l2gre_vpn_traverse(int unit, bcm_l2gre_vpn_traverse_cb cb,
                             void *user_data)
{
    int idx, num_vfi;
    int vpn;
    bcm_l2gre_vpn_config_t info;

    /* L2GRE VPNs */
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (idx = 0; idx < num_vfi; idx++) {
         if (_bcm_vfi_used_get(unit, idx, _bcmVfiTypeL2Gre)) {
             _BCM_L2GRE_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, idx);
             bcm_l2gre_vpn_config_t_init(&info);
             BCM_IF_ERROR_RETURN(bcm_tr3_l2gre_vpn_get(unit, vpn, &info));
             BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
         }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_tr3_l2gre_tunnel_terminate_entry_key_set
 * Purpose:
 *		Set L2GRE Tunnel Terminate entry key set
 * Parameters:
 *  IN :  Unit
 *  IN :  tnl_info
 *  IN(OUT) : tr_ent VLAN_XLATE entry pointer
 *  IN : clean_flag
 * Returns:
 *	BCM_E_XXX
 */

STATIC void
 _bcm_tr3_l2gre_tunnel_terminate_entry_key_set(int unit, soc_mem_t *mem,
                                bcm_tunnel_terminator_t *tnl_info, uint32 *tr_ent, uint8 out_mode)
 {
    sal_memset(tr_ent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        *mem = VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, *mem, tr_ent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, *mem, tr_ent, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, *mem, tr_ent, KEY_TYPEf,
                                _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
        soc_mem_field32_set(unit, *mem, tr_ent, DATA_TYPEf,
                                _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
    } else
#endif
    {
        *mem = VLAN_XLATEm;

        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, *mem, tr_ent, KEY_TYPEf,
                           _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_DIP); /* L2GRE_DIP Key */
        } else if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
            soc_mem_field32_set(unit, *mem, tr_ent, KEY_TYPEf,
                           _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP); /* L2GRE_DIP Key */
        }
        soc_mem_field32_set(unit, *mem, tr_ent, VALIDf, 0x1);
    }
    soc_mem_field32_set(unit, *mem, tr_ent, L2GRE_DIP__DIPf, tnl_info->dip);

    if (out_mode == _BCM_L2GRE_MULTICAST_BUD) {
        soc_mem_field32_set(unit, *mem, tr_ent,
                            L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf, 0x1);
        _bcm_tr3_l2gre_bud_loopback_enable(unit);
    } else if (out_mode == _BCM_L2GRE_MULTICAST_LEAF) {
        soc_mem_field32_set(unit, *mem, tr_ent,
                            L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf, 0x0);
    }
 }

/*
 * Function:
 *          _bcm_tr3_l2gre_tunnel_terminator_reference_count
 * Purpose:
 *          Find L2GRE Tunnel terminator DIP reference count
 * Returns: BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_tunnel_terminator_reference_count(int unit, bcm_ip_t dest_ip_addr)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int tunnel_idx=0, num_vp=0, ref_count=0;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    l2gre_info = L2GRE_INFO(unit);

      for (tunnel_idx=0; tunnel_idx< num_vp; tunnel_idx++) {
          if (l2gre_info->l2gre_tunnel_term[tunnel_idx].dip == dest_ip_addr) {
              ref_count ++;
          }
      }
    return ref_count;
}

/*
 * Function:
 *          _bcm_tr3_l2gre_multicast_tunnel_state_set
 * Purpose:
 *          Set L2GRE Tunnel terminator Multicast State
 * Returns: BCM_E_XXX
 */


STATIC void
_bcm_tr3_l2gre_multicast_tunnel_state_set (int unit, int  tunnel_idx, uint8 tunnel_state)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;

    l2gre_info = L2GRE_INFO(unit);

    if (tunnel_idx != -1) {
        l2gre_info->l2gre_tunnel_term[tunnel_idx].tunnel_state = tunnel_state;
    }
}


/*
 * Function:
 *          _bcm_tr3_l2gre_multicast_tunnel_state_index_get
 * Purpose:
 *          Get L2GRE Tunnel terminator Multicast State and Tunnel Index
 * Returns: BCM_E_XXX
 */


STATIC void
_bcm_tr3_l2gre_multicast_tunnel_state_index_get  (int unit,
                            bcm_ip_t mc_ip_addr, bcm_ip_t  src_ip_addr, uint8 *tunnel_state, int *tunnel_index)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int idx=0, num_vp=0;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    l2gre_info = L2GRE_INFO(unit);

      for (idx=0; idx< num_vp; idx++) {
          if (l2gre_info->l2gre_tunnel_term[idx].dip == mc_ip_addr) {
              if (l2gre_info->l2gre_tunnel_term[idx].sip == src_ip_addr) {
                    *tunnel_state = l2gre_info->l2gre_tunnel_term[idx].tunnel_state;
                    *tunnel_index = idx;
                     break;
              }
          }
      }
}

/*
 * Function:
 *          _bcm_tr3_l2gre_tunnel_terminator_state_find
 * Purpose:
 *          Find L2GRE Tunnel terminator DIP State
 * Returns: BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_tunnel_terminator_state_find(int unit, bcm_ip_t dest_ip_addr)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int tunnel_idx=0, num_vp=0, tunnel_state=0;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    l2gre_info = L2GRE_INFO(unit);

    for (tunnel_idx=0; tunnel_idx< num_vp; tunnel_idx++) {
          if (l2gre_info->l2gre_tunnel_term[tunnel_idx].dip == dest_ip_addr) {
              if (l2gre_info->l2gre_tunnel_term[tunnel_idx].tunnel_state == _BCM_L2GRE_TUNNEL_TERM_MULTICAST_LEAF ) {
                 tunnel_state += 0;
              } else if (l2gre_info->l2gre_tunnel_term[tunnel_idx].tunnel_state == _BCM_L2GRE_TUNNEL_TERM_MULTICAST_BUD ) {
                 tunnel_state += 1;
              }
          }
    }
    return tunnel_state;
}

/*
 * Function:
 *      bcm_tr3_l2gre_multicast_leaf_entry_check
 * Purpose:
 *      To find whether LEAF-multicast entry exists for DIP?
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_multicast_leaf_entry_check(int unit, bcm_ip_t mc_ip_addr, bcm_ip_t  src_ip_addr, int multicast_flag)
{
    int rv=BCM_E_NONE, index=0;
    int tunnel_ref_count = 0;
    uint8 tunnel_multicast_state = 0;
    int tunnel_mc_states = 0, tunnel_idx=0;
    soc_mem_t mem;
    uint32 vxlate[SOC_MAX_MEM_WORDS] = {0};

    tunnel_ref_count = _bcm_tr3_l2gre_tunnel_terminator_reference_count(unit, mc_ip_addr);

    if (tunnel_ref_count < 1) {
        /* Tunnel not created */
        return BCM_E_NONE;
    } else if (tunnel_ref_count >= 1) {
        (void )_bcm_tr3_l2gre_multicast_tunnel_state_index_get (unit,
            mc_ip_addr, src_ip_addr, &tunnel_multicast_state, &tunnel_idx);

        /* Set (SIP, DIP) based Software State */
        if (multicast_flag == BCM_L2GRE_MULTICAST_TUNNEL_STATE_BUD_ENABLE) {
            if (tunnel_multicast_state == _BCM_L2GRE_TUNNEL_TERM_MULTICAST_LEAF) {
                (void) _bcm_tr3_l2gre_multicast_tunnel_state_set(unit,
                                 tunnel_idx, _BCM_L2GRE_TUNNEL_TERM_MULTICAST_BUD);
            }
        } else  if (multicast_flag == BCM_L2GRE_MULTICAST_TUNNEL_STATE_BUD_DISABLE){
            if (tunnel_multicast_state == _BCM_L2GRE_TUNNEL_TERM_MULTICAST_BUD) {
                (void) _bcm_tr3_l2gre_multicast_tunnel_state_set(unit,
                                 tunnel_idx, _BCM_L2GRE_TUNNEL_TERM_MULTICAST_LEAF);
            }
        }

        /* Set DIP-based cumulative hardware State */
        tunnel_mc_states = _bcm_tr3_l2gre_tunnel_terminator_state_find(unit, mc_ip_addr);

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            mem = VLAN_XLATE_1_DOUBLEm;

            soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_1f, 7);
            soc_mem_field32_set(unit, mem, vxlate,
                KEY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
            soc_mem_field32_set(unit, mem, vxlate,
                DATA_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
        } else
#endif
        {
            mem = VLAN_XLATEm;

            if (SOC_IS_TRIUMPH3(unit)) {
                soc_mem_field32_set(unit, mem, vxlate,
                    KEY_TYPEf, _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_DIP);
            } else if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
                soc_mem_field32_set(unit, mem, vxlate,
                    KEY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
            }
            soc_mem_field32_set(unit, mem, vxlate, VALIDf, 1);
        }
        soc_mem_field32_set(unit, mem, vxlate, L2GRE_DIP__DIPf, mc_ip_addr);

        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vxlate, vxlate, 0);
        if (rv == SOC_E_NONE) {
            if (tunnel_mc_states) {
                /* Create Transit entry: LEAF to BUD */
                soc_mem_field32_set(unit, mem, vxlate,
                        L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf, 0x1);
                _bcm_tr3_l2gre_bud_loopback_enable(unit);
            } else {
                /* Delete Transit entry : BUD to LEAF */
                soc_mem_field32_set(unit, mem, vxlate,
                        L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf, 0x0);
            }
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, vxlate);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_terminator_create
 * Purpose:
 *      Set L2GRE Tunnel terminator
 * Parameters:
 *      unit - Device Number
 *      tnl_info
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l2gre_tunnel_terminator_create(int unit, bcm_tunnel_terminator_t *tnl_info)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int index=-1, tunnel_idx=-1;
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    uint32 vxlate[SOC_MAX_MEM_WORDS] = {0};

    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    if (tnl_info->type != bcmTunnelTypeL2Gre) {
       return BCM_E_PARAM;
    }

    /* Program tunnel id */
    if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
        if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
            return BCM_E_PARAM;
        }
        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
    } else {
        /* Only BCM_TUNNEL_TERM_TUNNEL_WITH_ID supported */
        /* Use tunnel_id retuned by _create */
        return BCM_E_PARAM;
    }

    /* Add First Tunnel term-entry for given DIP to Hardware */
    if (_bcm_tr3_l2gre_tunnel_terminator_reference_count(unit, tnl_info->dip) < 1)  {

         (void)_bcm_tr3_l2gre_tunnel_terminate_entry_key_set(unit, &mem, tnl_info, vxlate, 0);

         rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vxlate, vxlate, 0);
         if (rv == SOC_E_NOT_FOUND) {
             rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vxlate);
         } else if (rv < 0) {
              return rv;
         }
    }

    l2gre_info = L2GRE_INFO(unit);
    if (BCM_SUCCESS(rv) && (tunnel_idx != -1)) {
       l2gre_info->l2gre_tunnel_term[tunnel_idx].sip = tnl_info->sip;
       l2gre_info->l2gre_tunnel_term[tunnel_idx].dip = tnl_info->dip;
       l2gre_info->l2gre_tunnel_term[tunnel_idx].tunnel_state = _BCM_L2GRE_TUNNEL_TERM_MULTICAST_LEAF;
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_terminator_update
 * Purpose:
 *     Update L2GRE Tunnel terminator multicast state
 * Parameters:
 *      unit - Device Number
 *      tnl_info
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l2gre_tunnel_terminator_update(int unit, bcm_tunnel_terminator_t *tnl_info)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int rv = BCM_E_NONE;
    int tunnel_idx=-1;
    bcm_ip_t mc_ip_addr, src_ip_addr;

    l2gre_info = L2GRE_INFO(unit);

    /* Program tunnel id */
    if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
        if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
            return BCM_E_PARAM;
        }
        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
    } else {
        /* Only BCM_TUNNEL_TERM_TUNNEL_WITH_ID supported */
        /* Use tunnel_id retuned by _create */
        return BCM_E_PARAM;
    }

    mc_ip_addr = l2gre_info->l2gre_tunnel_term[tunnel_idx].dip;
    src_ip_addr = l2gre_info->l2gre_tunnel_term[tunnel_idx].sip;

    rv = bcm_tr3_l2gre_multicast_leaf_entry_check(unit,
                   mc_ip_addr, src_ip_addr, tnl_info->multicast_flag);
    return rv;
}

/*
 * Function:
 *  bcm_tr3_l2gre_tunnel_terminate_destroy
 * Purpose:
 *  Destroy L2GRE  tunnel_terminate Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  l2gre_tunnel_id
 * Returns:
 *  BCM_E_XXX
 */

int
bcm_tr3_l2gre_tunnel_terminator_destroy(int unit, bcm_gport_t l2gre_tunnel_id)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int tunnel_idx, index;
    uint32  tunnel_dip=0;
    int rv = BCM_E_NONE;
    soc_mem_t mem = VLAN_XLATEm;
    uint32 vxlate[SOC_MAX_MEM_WORDS] = {0};

    if (!BCM_GPORT_IS_TUNNEL(l2gre_tunnel_id)) {
        return BCM_E_PARAM;
    }
    tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(l2gre_tunnel_id);

    l2gre_info = L2GRE_INFO(unit);
    tunnel_dip = l2gre_info->l2gre_tunnel_term[tunnel_idx].dip;

    /* Remove Tunnel term-entry for given DIP from Hardware based on ref-count */
    if (_bcm_tr3_l2gre_tunnel_terminator_reference_count(unit, tunnel_dip) == 1)  {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            mem = VLAN_XLATE_1_DOUBLEm;

            soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_1f, 7);
            soc_mem_field32_set(unit, mem, vxlate,
                KEY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
            soc_mem_field32_set(unit, mem, vxlate,
                DATA_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
        } else
#endif
        {
            mem = VLAN_XLATEm;

            if (SOC_IS_TRIUMPH3(unit)) {
                soc_mem_field32_set(unit, mem, vxlate,
                    KEY_TYPEf, _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_DIP);
            } else if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
                soc_mem_field32_set(unit, mem, vxlate,
                    KEY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
            }
            soc_mem_field32_set(unit, mem, vxlate, VALIDf, 1);
        }
        soc_mem_field32_set(unit, mem, vxlate, L2GRE_DIP__DIPf, tunnel_dip);
        soc_mem_field32_set(unit, mem, vxlate, L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf, 0);

        BCM_IF_ERROR_RETURN(
            soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vxlate, vxlate, 0));

        /* Delete the entry from HW */
        BCM_IF_ERROR_RETURN(soc_mem_delete(unit, mem, MEM_BLOCK_ALL, vxlate));
    }

    if (BCM_SUCCESS(rv) && (tunnel_idx != -1)) {
       l2gre_info->l2gre_tunnel_term[tunnel_idx].dip = 0;
       l2gre_info->l2gre_tunnel_term[tunnel_idx].tunnel_state = _BCM_L2GRE_TUNNEL_TERM_MULTICAST_LEAF;
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_terminator_destroy_all
 * Purpose:
 *      Destroy all incoming L2GRE tunnel
 * Parameters:
 *      unit           - (IN) Device Number
 */
int
bcm_tr3_l2gre_tunnel_terminator_destroy_all(int unit)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int rv = BCM_E_UNAVAIL, tnl_idx=0,num_vp=0;
    uint32 *vtab, *vtabp;
    int i, imin, imax, nent;
    soc_mem_t mem;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        mem = VLAN_XLATEm;
    }

    l2gre_info = L2GRE_INFO(unit);

    if (!SOC_HW_RESET(unit) && (SOC_SWITCH_BYPASS_MODE(unit) == SOC_SWITCH_BYPASS_MODE_NONE)) {
        imin = soc_mem_index_min(unit, mem);
        imax = soc_mem_index_max(unit, mem);
        nent = soc_mem_index_count(unit, mem);

        vtab = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, mem), "vlan_xlate");
        if (vtab == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(vtab, 0, nent * sizeof(*vtab));

        soc_mem_lock(unit, mem);
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                imin, imax, vtab);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            soc_cm_sfree(unit, vtab);
            return rv; 
        }
        for (i = 0; i < nent; i++) {
            vtabp = soc_mem_table_idx_to_pointer(unit, mem,
                                                 void *, vtab, i);

#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                if (soc_mem_field32_get(unit, mem, vtabp, BASE_VALID_0f) != 3) {
                    continue;
                }
                if (soc_mem_field32_get(unit, mem, vtabp, BASE_VALID_1f) != 7) {
                    continue;
                }
                if (soc_mem_field32_get(unit, mem, vtabp, KEY_TYPEf) !=
                                        _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP) {
                    continue;
                }
                if (soc_mem_field32_get(unit, mem, vtabp, DATA_TYPEf) !=
                                        _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP) {
                    continue;
                }
            } else
#endif
            {
                if (!soc_mem_field32_get(unit, mem, vtabp, VALIDf)) {
                    continue;
                }

                if (SOC_IS_TRIUMPH3(unit)) {
                    if (soc_mem_field32_get(unit, mem, vtabp,
                                KEY_TYPEf) != _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_DIP) {
                        continue;
                    }
                } else if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
                    if (soc_mem_field32_get(unit, mem, vtabp,
                                KEY_TYPEf) != _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP) {
                        continue;
                    }
                }
            }

            /* Delete the entry from HW */
            rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, vtabp);
            if (rv < 0) {
                soc_mem_unlock(unit, mem);
                soc_cm_sfree(unit, vtab);
                return rv;
            }
        }
        soc_mem_unlock(unit, mem);
        soc_cm_sfree(unit, vtab);
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (tnl_idx = 0; tnl_idx < num_vp; tnl_idx++) {
       l2gre_info->l2gre_tunnel_term[tnl_idx].dip = 0;
       l2gre_info->l2gre_tunnel_term[tnl_idx].tunnel_state = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_initiator_get
 * Purpose:
 *      Get L2GRE  tunnel_terminate Entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Tunnel terminator structure
 */
int
bcm_tr3_l2gre_tunnel_terminator_get(int unit, bcm_tunnel_terminator_t *tnl_info)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int tunnel_idx=-1, index=-1;
    uint32  tunnel_dip=0;
    soc_mem_t mem = VLAN_XLATEm;
    uint32 vxlate[SOC_MAX_MEM_WORDS] = {0};

    /* Program tunnel id */
    if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
        return BCM_E_PARAM;
    }
    tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);

    l2gre_info = L2GRE_INFO(unit);
    tunnel_dip = l2gre_info->l2gre_tunnel_term[tunnel_idx].dip;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mem, vxlate,
            KEY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
        soc_mem_field32_set(unit, mem, vxlate,
            DATA_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
    } else
#endif
    {
        mem = VLAN_XLATEm;

        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, mem, vxlate,
                KEY_TYPEf, _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_DIP);
        } else if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
            soc_mem_field32_set(unit, mem, vxlate,
                KEY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
        }
        soc_mem_field32_set(unit, mem, vxlate, VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, vxlate, L2GRE_DIP__DIPf, tunnel_dip);

    BCM_IF_ERROR_RETURN(
        soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vxlate, vxlate, 0));

    tnl_info->sip = l2gre_info->l2gre_tunnel_term[tunnel_idx].sip;
    tnl_info->dip = l2gre_info->l2gre_tunnel_term[tunnel_idx].dip;
    tnl_info->type = bcmTunnelTypeL2Gre;
    if (l2gre_info->l2gre_tunnel_term[tunnel_idx].tunnel_state
                        == _BCM_L2GRE_TUNNEL_TERM_MULTICAST_BUD) {
        tnl_info->multicast_flag = BCM_L2GRE_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
    } else if (l2gre_info->l2gre_tunnel_term[tunnel_idx].tunnel_state
                        == _BCM_L2GRE_TUNNEL_TERM_MULTICAST_LEAF){
        tnl_info->multicast_flag = BCM_L2GRE_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
    } else {
        tnl_info->multicast_flag = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_terminator_traverse
 * Purpose:
 *      Traverse L2GRE tunnel terminator entries
 * Parameters:
 *      unit    - (IN) Device Number
 *      cb    - (IN) User callback function, called once per entry
 *      user_data    - (IN) User supplied cookie used in parameter in callback function
 */
int
bcm_tr3_l2gre_tunnel_terminator_traverse(int unit, bcm_tunnel_terminator_traverse_cb cb, void *user_data)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int num_tnl = 0, idx = 0;
    bcm_tunnel_terminator_t info;
    int rv = BCM_E_NONE;

    l2gre_info = L2GRE_INFO(unit);
    num_tnl = soc_mem_index_count(unit, SOURCE_VPm);

    /* Iterate over all the entries - search valid ones. */
    for(idx = 0; idx < num_tnl; idx++) {
        /* Check a valid entry */
        if (l2gre_info->l2gre_tunnel_term[idx].dip) {
            /* Reset buffer before read. */
            bcm_tunnel_terminator_t_init(&info);

            BCM_GPORT_TUNNEL_ID_SET(info.tunnel_id, idx);

            rv = bcm_tr3_l2gre_tunnel_terminator_get(unit,&info);

            /* search failure */
            if (BCM_FAILURE(rv)) {
                break;
            }

            if (cb) {
                rv = (*cb) (unit, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                    break;
                }
#endif
            }
        }
    }

    return rv;
}


/*
 * Function:
 *      _bcm_tr3_l2gre_tunnel_init_add
 * Purpose:
 *      Add tunnel initiator entry to hw.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      idx      - (IN)Index to insert tunnel initiator.
 *      info     - (IN)Tunnel initiator entry info.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2gre_tunnel_init_add(int unit, int idx, bcm_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;                  /* Return value                  */
    int df_val;                           /* Don't fragment encoding       */
    soc_mem_t mem = EGR_IP_TUNNELm;                        /* Tunnel initiator table memory */
    uint32 tnl_entry[SOC_MAX_MEM_WORDS] = {0};
    soc_field_t type_field;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        type_field = DATA_TYPEf;
    } else
#endif
    {
        type_field = ENTRY_TYPEf;
    }

    /* If replacing existing entry, first read the entry to get old
       profile pointer and TPID */
    if (info->flags & BCM_TUNNEL_REPLACE) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tnl_entry);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set destination address. */
    soc_mem_field_set(unit, mem, tnl_entry, DIPf, (uint32 *)&info->dip);

    /* Set source address. */
    soc_mem_field_set(unit, mem, tnl_entry, SIPf, (uint32 *)&info->sip);

    /* IP tunnel hdr DF bit for IPv4. */
    df_val = 0;
    if (info->flags & BCM_TUNNEL_INIT_USE_INNER_DF) {
        df_val |= 0x2;
    } else if (info->flags & BCM_TUNNEL_INIT_IPV4_SET_DF) {
        df_val |= 0x1;
    }
    soc_mem_field32_set(unit, mem, tnl_entry, IPV4_DF_SELf, df_val);

    /* Set DSCP value.  */
    soc_mem_field32_set(unit, mem, tnl_entry, DSCPf, info->dscp);

    /* Tunnel header DSCP select. */
    soc_mem_field32_set(unit, mem, tnl_entry, DSCP_SELf, info->dscp_sel);

    /* Set TTL. */
    soc_mem_field32_set(unit, mem, tnl_entry, TTLf, info->ttl);

    /* Set Tunnel type = L2GRE */
    soc_mem_field32_set(unit, mem, tnl_entry, TUNNEL_TYPEf, _BCM_L2GRE_TUNNEL_TYPE);

    /* Set entry type = IPv4 */
    soc_mem_field32_set(unit, mem, tnl_entry, type_field, 0x1);

    if ((info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_FIXED) ||
        (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM)) {
        int val;
        uint16 random;
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            uint64 rval;
            uint32 base_idx;
            egr_sequence_number_table_entry_t entry;

            BCM_IF_ERROR_RETURN(READ_EGR_SEQUENCE_NUMBER_CTRLr(unit, &rval));
            base_idx = soc_reg64_field32_get(unit, EGR_SEQUENCE_NUMBER_CTRLr,
                rval,EGR_IP_TUNNEL_OFFSET_BASEf);

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_SEQUENCE_NUMBER_TABLEm,
                MEM_BLOCK_ANY, idx + base_idx, &entry));
            if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_FIXED) {
                soc_EGR_SEQUENCE_NUMBER_TABLEm_field32_set(unit, &entry,
                                                       SEQUENCE_NUMBERf,
                                                       info->ip4_id);
            } else if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM) {
                random = (uint16) (sal_time_usecs() & 0xFFFF);
                soc_EGR_SEQUENCE_NUMBER_TABLEm_field32_set(unit, &entry,
                                                       SEQUENCE_NUMBERf,
                                                       random);
            }
            BCM_IF_ERROR_RETURN(soc_mem_write(
                unit, EGR_SEQUENCE_NUMBER_TABLEm, MEM_BLOCK_ANY,
                idx + base_idx, &entry));

        } else
#endif
        {
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchTunnelIp4IdShared, &val));
            if (val == 0) {
                egr_fragment_id_table_entry_t entry;
                sal_memset(&entry, 0, sizeof(egr_fragment_id_table_entry_t));

                if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_FIXED) {
                    soc_EGR_FRAGMENT_ID_TABLEm_field32_set(unit, &entry,
                                                           FRAGMENT_IDf,
                                                           info->ip4_id);
                } else if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM) {
                    random = (uint16) (sal_time_usecs() & 0xFFFF);
                    soc_EGR_FRAGMENT_ID_TABLEm_field32_set(unit, &entry,
                                                           FRAGMENT_IDf,
                                                           random);
                }
                BCM_IF_ERROR_RETURN(soc_mem_write(
                    unit, EGR_FRAGMENT_ID_TABLEm, SOC_BLOCK_ALL, idx, &entry));
            }
        }
    }

    /* Write buffer to hw. */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, tnl_entry);
    return rv;

}


/*
 * Function:
 *      _bcm_tr3_l2gre_tunnel_initiator_entry_add
 * Purpose:
 *      Configure L2GRE Tunnel initiator hardware Entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2gre_tunnel_initiator_entry_add(int unit, uint32 flags, bcm_tunnel_initiator_t *info, int *tnl_idx)
{
   int rv = BCM_E_NONE;

   rv = bcm_xgs3_tnl_init_add(unit, flags, info, tnl_idx);
   if (BCM_FAILURE(rv)) {
         return rv;
   }
   /* Write the entry to HW */
   rv = _bcm_tr3_l2gre_tunnel_init_add(unit, *tnl_idx, info);
   if (BCM_FAILURE(rv)) {
        flags = _BCM_L3_SHR_WRITE_DISABLE;
        (void) bcm_xgs3_tnl_init_del(unit, flags, *tnl_idx);
   }
   return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_initiator_create
 * Purpose:
 *      Set L2GRE Tunnel initiator
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l2gre_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int tnl_idx=-1, rv = BCM_E_NONE;
    uint32 flags;
    int idx=0, num_tnl=0, num_vp=0;
    uint8  sip_entry_present=0;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry.  */

    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }
    if (info->type != bcmTunnelTypeL2Gre)  {
        return BCM_E_PARAM;
    }
    if (!BCM_TTL_VALID(info->ttl)) {
        return BCM_E_PARAM;
    }
    if (info->dscp_sel > 2 || info->dscp_sel < 0) {
        return BCM_E_PARAM;
    }
    if (info->dscp > 63 || info->dscp < 0) {
        return BCM_E_PARAM;
    }

    l2gre_info = L2GRE_INFO(unit);
    num_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    /* Allocate either existing or new tunnel initiator entry */
    flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE |
             _BCM_L3_SHR_SKIP_INDEX_ZERO;

    /* Check if Same SIP exists: MAP first 512 entries to EGR_IP_TUNNEL
        Rest of entries map to  L2GRE_TUNNEL with Same SIP, different DIP */
    for (idx=0; idx < num_tnl; idx++) {
        if (l2gre_info->l2gre_tunnel_init[idx].sip == info->sip) {
            tnl_idx = idx; /* Entry_idx within Hardware-Range */
            sip_entry_present = 1;
            break;
        }
    }

    /*  Check if Same SIP exists in Soft-state */
    if (!sip_entry_present) {
       for (idx=num_tnl; idx < num_vp; idx++) {
          if (l2gre_info->l2gre_tunnel_init[idx].sip == info->sip) {
            for (tnl_idx=0; tnl_idx < num_tnl; tnl_idx++) {
                 BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY, tnl_idx, tnl_entry));
                 if (info->sip == soc_mem_field32_get(unit, EGR_IP_TUNNELm, tnl_entry, SIPf)) {
                    /* Entry_idx within Hardware-Range */
                    BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, tnl_idx);
                    info->flags |= BCM_TUNNEL_REPLACE;
                    break;
                 }
            }
            break;
          }
       }
    }

    if (sip_entry_present) {
         /* 2 Cases: Entry-update, (Multiple-dip, same-SIP) */
         if (l2gre_info->l2gre_tunnel_init[tnl_idx].dip != info->dip) {
             /* Obtain index to Software-State to store (SIP,DIP) */
             for (idx=num_tnl; idx < num_vp; idx++) {
                 if (l2gre_info->l2gre_tunnel_init[idx].sip == 0) {
                     tnl_idx = idx; /* Obtain Free index */
                     break;
                 }
             }
         } else {
             /* Parse tunnel replace flag */
             if (info->flags & BCM_TUNNEL_REPLACE) {
                flags |=  _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WITH_ID;
                tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
                rv = _bcm_tr3_l2gre_tunnel_initiator_entry_add(unit, flags, info, &tnl_idx);
                if (BCM_FAILURE(rv)) {
                   return rv;
                }
             }
         }
    } else {
         /* Parse tunnel replace flag */
         if (info->flags & BCM_TUNNEL_REPLACE) {
            flags |=  _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WITH_ID;
            tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
         }
         rv = _bcm_tr3_l2gre_tunnel_initiator_entry_add(unit, flags, info, &tnl_idx);
         if (BCM_FAILURE(rv)) {
             return rv;
         }
    }

    if (tnl_idx !=- 1) {
         l2gre_info->l2gre_tunnel_init[tnl_idx].sip = info->sip;
         l2gre_info->l2gre_tunnel_init[tnl_idx].dip = info->dip;
         BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, tnl_idx);
    } else {
        return BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_tunnel_init_get
 * Purpose:
 *      Get a tunnel initiator entry from hw.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      idx      - (IN/OUT)Index to read tunnel initiator.
 *      info     - (OUT)Tunnel initiator entry info.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2gre_tunnel_init_get(int unit, int *hw_idx, bcm_tunnel_initiator_t *info)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    egr_fragment_id_table_entry_t entry;
    int df_val;                            /* Don't fragment encoded value. */
    int tnl_type;                          /* Tunnel type.                  */
    uint32 entry_type = BCM_XGS3_TUNNEL_INIT_V4;/* Entry type (IPv4/IPv6/MPLS)*/
    int num_tnl=0, tnl_idx =0, idx=0;
    bcm_ip_t sip=0;                       /* Tunnel header SIP (IPv4). */
    soc_field_t type_field;
    soc_mem_t mem = EGR_IP_TUNNELm;
    uint32 tnl_entry[SOC_MAX_MEM_WORDS] = {0};

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        type_field = DATA_TYPEf;
    } else
#endif
    {
        type_field = ENTRY_TYPEf;
    }

    idx = *hw_idx;

    /* Initialize the buffer. */
    l2gre_info = L2GRE_INFO(unit);
    num_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);

    if (idx >= num_tnl) { /* Software State */
         sip = l2gre_info->l2gre_tunnel_init[idx].sip;
         info->dip = l2gre_info->l2gre_tunnel_init[idx].dip;

        /* Find matching entry within Hardware */
       for (tnl_idx=0; tnl_idx < num_tnl; tnl_idx++) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, tnl_idx, tnl_entry));
            if (sip == soc_mem_field32_get(unit, mem, tnl_entry, SIPf)) {
                idx = tnl_idx; /* Entry_idx within Hardware-Range */
                break;
            }
       }
    } else {
         info->dip = l2gre_info->l2gre_tunnel_init[idx].dip;
         BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tnl_entry));
    }

    /* Get tunnel type. */
    tnl_type = soc_mem_field32_get(unit, mem, tnl_entry, TUNNEL_TYPEf);
    if (tnl_type != _BCM_L2GRE_TUNNEL_TYPE) {
        return BCM_E_CONFIG;
    }

    /* Get entry_type. */
    entry_type = soc_mem_field32_get(unit, mem, tnl_entry, type_field);
    if (entry_type != 0x1) {
        return BCM_E_CONFIG;
    }

    /* Parse hw buffer. */
    /* Check will not fail. entry_type already validated */
    if (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) {
        /* Get destination ip. */
        info->dip = soc_mem_field32_get(unit, mem, tnl_entry, DIPf);

        /* Get source ip. */
        info->sip = soc_mem_field32_get(unit, mem, tnl_entry, SIPf);
    }

    /* Tunnel header DSCP select. */
    info->dscp_sel = soc_mem_field32_get(unit, mem, tnl_entry, DSCP_SELf);

    /* Tunnel header DSCP value. */
    info->dscp = soc_mem_field32_get(unit, mem, tnl_entry, DSCPf);

    /* IP tunnel hdr DF bit for IPv4 */
    df_val = soc_mem_field32_get(unit, mem, tnl_entry, IPV4_DF_SELf);
    if (0x2 <= df_val) {
        info->flags |= BCM_TUNNEL_INIT_USE_INNER_DF;
    } else if (0x1 == df_val) {
        info->flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;
    }

    /* Get TTL. */
    info->ttl = soc_mem_field32_get(unit, mem, tnl_entry, TTLf);

    /* Translate hw tunnel type into API encoding. - bcmTunnelTypeL2Gre */
    BCM_IF_ERROR_RETURN(_bcm_trx_tnl_hw_code_to_type(unit, tnl_type,
                                                     entry_type,
                                                     &info->type));
    if (idx < num_tnl) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            uint64 rval;
            uint32 base_idx;

            BCM_IF_ERROR_RETURN(READ_EGR_SEQUENCE_NUMBER_CTRLr(unit, &rval));
            base_idx = soc_reg64_field32_get(unit, EGR_SEQUENCE_NUMBER_CTRLr, rval,
                EGR_IP_TUNNEL_OFFSET_BASEf);
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_SEQUENCE_NUMBER_TABLEm,
                MEM_BLOCK_ANY, idx + base_idx, &entry));
            info->ip4_id = soc_mem_field32_get(unit, EGR_SEQUENCE_NUMBER_TABLEm,
                &entry, SEQUENCE_NUMBERf);
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, EGR_FRAGMENT_ID_TABLEm, MEM_BLOCK_ANY, idx, &entry));
            info->ip4_id = soc_mem_field32_get(
                unit, EGR_FRAGMENT_ID_TABLEm, &entry, FRAGMENT_IDf);
        }
    }

    *hw_idx = idx;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_initiator_destroy
 * Purpose:
 *      Destroy outgoing L2GRE tunnel
 * Parameters:
 *      unit           - (IN) Device Number
 *      l2gre_tunnel_id - (IN) Tunnel ID (Gport)
 */
int
bcm_tr3_l2gre_tunnel_initiator_destroy(int unit, bcm_gport_t l2gre_tunnel_id)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int tnl_idx=-1, temp_tnl_idx=-1, rv = BCM_E_NONE, ref_count=0, idx=0;
    uint32 flags = 0;
    bcm_tunnel_initiator_t info;           /* Tunnel initiator structure */
    int num_vp=0;
    bcm_ip_t sip=0;                       /* Tunnel header SIP (IPv4). */

    l2gre_info = L2GRE_INFO(unit);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    /* Input parameters check. */
    if (!BCM_GPORT_IS_TUNNEL(l2gre_tunnel_id)) {
        return BCM_E_PARAM;
    }
    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(l2gre_tunnel_id);

    bcm_tunnel_initiator_t_init(&info);
    sip = l2gre_info->l2gre_tunnel_init[tnl_idx].sip;
    /* Remove HW TNL entry only if all matching SIP gets removed */
    for (idx=0; idx < num_vp; idx++) {
        if (l2gre_info->l2gre_tunnel_init[idx].sip == sip) {
            ref_count++;
        }
    }
    if (ref_count == 1) {
       /* Get the entry first */
       temp_tnl_idx = tnl_idx;
       rv = _bcm_tr3_l2gre_tunnel_init_get(unit, &tnl_idx, &info);
       if (BCM_FAILURE(rv)) {
          return rv;
       }
       /* Destroy the tunnel entry */
       (void) bcm_xgs3_tnl_init_del(unit, flags, tnl_idx);
       l2gre_info->l2gre_tunnel_init[temp_tnl_idx].sip = 0;
       l2gre_info->l2gre_tunnel_init[temp_tnl_idx].dip = 0;
    } else if (ref_count) {
       l2gre_info->l2gre_tunnel_init[tnl_idx].sip = 0;
       l2gre_info->l2gre_tunnel_init[tnl_idx].dip = 0;
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_initiator_destroy_all
 * Purpose:
 *      Destroy all outgoing L2GRE tunnel
 * Parameters:
 *      unit           - (IN) Device Number
 */
int
bcm_tr3_l2gre_tunnel_initiator_destroy_all(int unit)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    soc_field_t type_field;
    egr_ip_tunnel_entry_t *tnl_entry, *tnl_entries;
    int rv = BCM_E_NONE, tnl_idx, num_entries, num_vp;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_IP_TUNNELm, DATA_TYPEf)) {
        type_field = DATA_TYPEf;
    } else
#endif
    {
        type_field = ENTRY_TYPEf;
    }

    l2gre_info = L2GRE_INFO(unit);
    num_entries = soc_mem_index_count(unit, EGR_IP_TUNNELm);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    tnl_entries = soc_cm_salloc(unit, num_entries * sizeof(*tnl_entries),
                                "egr_ip_tunnel");
    if (tnl_entries == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(tnl_entries, 0, num_entries * sizeof(*tnl_entries));

    soc_mem_lock(unit, EGR_IP_TUNNELm);
    rv = soc_mem_read_range(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY,
                            0, num_entries - 1, tnl_entries);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, EGR_IP_TUNNELm);
        soc_cm_sfree(unit, tnl_entries);
        return rv; 
    }
    for (tnl_idx = 0; tnl_idx < num_entries; tnl_idx++) {
        tnl_entry = soc_mem_table_idx_to_pointer(unit, EGR_IP_TUNNELm,
                                 egr_ip_tunnel_entry_t *, tnl_entries, tnl_idx);

        if (!soc_EGR_IP_TUNNELm_field32_get(unit, tnl_entry, type_field)) {
            continue;
        }
        if (soc_EGR_IP_TUNNELm_field32_get(unit, tnl_entry,
                                      TUNNEL_TYPEf) != _BCM_L2GRE_TUNNEL_TYPE) {
            continue;
        }

       /* Destroy the tunnel entry */
       (void) bcm_xgs3_tnl_init_del(unit, 0, tnl_idx);
       l2gre_info->l2gre_tunnel_init[tnl_idx].sip = 0;
       l2gre_info->l2gre_tunnel_init[tnl_idx].dip = 0;
    }
    soc_mem_unlock(unit, EGR_IP_TUNNELm);
    soc_cm_sfree(unit, tnl_entries);

    for (tnl_idx = num_entries; tnl_idx < num_vp; tnl_idx++) {
         l2gre_info->l2gre_tunnel_init[tnl_idx].sip = 0;
         l2gre_info->l2gre_tunnel_init[tnl_idx].dip = 0;
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_initiator_get
 * Purpose:
 *      Get an outgong L2GRE tunnel info
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Tunnel initiator structure
 */
int
bcm_tr3_l2gre_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info)
{
    int tnl_idx, rv = BCM_E_NONE;

    /* Input parameters check. */
    if (info == NULL) {
        return BCM_E_PARAM;
    }
    if (!BCM_GPORT_IS_TUNNEL(info->tunnel_id)) {
        return BCM_E_PARAM;
    }
    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);

    /* Get the entry */
    rv = _bcm_tr3_l2gre_tunnel_init_get(unit, &tnl_idx, info);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_tunnel_initiator_traverse
 * Purpose:
 *      Traverse L2GRE tunnel initiator entries
 * Parameters:
 *      unit    - (IN) Device Number
 *      cb    - (IN) User callback function, called once per entry
 *      user_data    - (IN) User supplied cookie used in parameter in callback function
 */
int
bcm_tr3_l2gre_tunnel_initiator_traverse(int unit, bcm_tunnel_initiator_traverse_cb cb, void *user_data)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int num_vp = 0, idx = 0;
    bcm_tunnel_initiator_t info;
    int rv = BCM_E_NONE;

    l2gre_info = L2GRE_INFO(unit);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    /* Iterate over all the entries - search valid ones. */
    for(idx = 0; idx < num_vp; idx++) {
        /* Check a valid entry */
        if (l2gre_info->l2gre_tunnel_init[idx].sip != 0) {
           /* Reset buffer before read. */
           bcm_tunnel_initiator_t_init(&info);

           BCM_GPORT_TUNNEL_ID_SET(info.tunnel_id, idx);
           rv = bcm_tr3_l2gre_tunnel_initiator_get(unit, &info);

           /* search failure */
           if (BCM_FAILURE(rv)) {
               if (rv != BCM_E_CONFIG) {
                   break;
               }
               continue;
           }

           if (cb) {
               rv = (*cb) (unit, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
               if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                   break;
               }
#endif
           }
        }
    }

    /* Reset last read status. */
    if (BCM_E_CONFIG == rv) {
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_tr3_l2gre_ingress_dvp_set
 * Purpose:
 *     Set Ingress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  l2gre_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_ingress_dvp_set(int unit, int vp, uint32 mpath_flag,
                                    int vp_nh_ecmp_index, bcm_l2gre_port_t *l2gre_port)
{
    int rv = BCM_E_NONE;
    int vp_type = -1;
    int network_port = -1;
    int flag = -1;
    bcm_if_t intf;

    if (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE) {
        flag = _bcmVpIngDvpConfigUpdate;
    } else {
        flag = _bcmVpIngDvpConfigSet;
    }

    if (mpath_flag) {
        intf = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit) + vp_nh_ecmp_index;
    } else {
        intf = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + vp_nh_ecmp_index;
    }

    /* Default Split Horizon Group Id
     * 0 - For Access Port;1 - For Network Port*/
    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        if (l2gre_port->flags & BCM_L2GRE_PORT_EGRESS_TUNNEL) {
            network_port = (_BCM_SWITCH_NETWORK_GROUP_ID_VALID(unit,
                l2gre_port->network_group_id)) ?
                    l2gre_port->network_group_id : 1;
        } else {
            network_port = (_BCM_SWITCH_NETWORK_GROUP_ID_VALID(unit,
                l2gre_port->network_group_id)) ?
                    l2gre_port->network_group_id : 0;
        }
    } else {
        network_port = (l2gre_port->flags & BCM_L2GRE_PORT_EGRESS_TUNNEL) ?
                           1 : 0;
    }

#ifdef BCM_RIOT_SUPPORT
    if ((l2gre_port->flags & BCM_L2GRE_PORT_EGRESS_TUNNEL) &&
        BCMI_RIOT_IS_ENABLED(unit)) {
        network_port = 0;
    }
#endif

    vp_type = (l2gre_port->flags & BCM_L2GRE_PORT_EGRESS_TUNNEL) ?
              _BCM_L2GRE_INGRESS_DEST_VP_TYPE : _BCM_L2GRE_DEST_VP_TYPE_ACCESS;
    rv = _bcm_vp_ing_dvp_config(unit, flag, vp, vp_type, intf, network_port);

    return rv;
}

/*
 * Function:
 *     _bcm_tr3_l2gre_ingress_dvp_reset
 * Purpose:
 *     Reset Ingress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_ingress_dvp_reset(int unit, int vp)
{
    int rv=BCM_E_UNAVAIL;
    rv =  _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp,
                               ING_DVP_CONFIG_INVALID_VP_TYPE,
                               ING_DVP_CONFIG_INVALID_INTF_ID,
                               ING_DVP_CONFIG_INVALID_PORT_TYPE);
    return rv;
}

/*
 * Function:
 *     _bcm_tr3_l2gre_egress_dvp_set
 * Purpose:
 *     Set Egress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  l2gre_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_egress_dvp_set(int unit, int vp, int drop, bcm_l2gre_port_t *l2gre_port)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int rv=BCM_E_UNAVAIL;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    egr_dvp_attribute_1_entry_t  egr_dvp_attribute_1;
    bcm_ip_t dip=0;                       /* DIP for tunnel header */
    bcm_ip_t sip=0;                       /* SIP for tunnel header */
    int tunnel_index=-1, idx=0, num_tnl=0;
    int network_group=0;


    l2gre_info = L2GRE_INFO(unit);
    num_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);

    tunnel_index = BCM_GPORT_TUNNEL_ID_GET(l2gre_port->egress_tunnel_id);
    if (tunnel_index < num_tnl) {
        /* Obtain Tunnel DIP */
        dip = l2gre_info->l2gre_tunnel_init[tunnel_index].dip;
    } else {
        /* Obtain Tunnel SIP */
        sip = l2gre_info->l2gre_tunnel_init[tunnel_index].sip;
        /* Obtain Tunnel DIP */
        dip = l2gre_info->l2gre_tunnel_init[tunnel_index].dip;
        /* Restrict tunnel_index to Hardware Range */
        for (idx=0; idx < num_tnl; idx++) {
            if (l2gre_info->l2gre_tunnel_init[idx].sip == sip) {
                tunnel_index = idx; /* Entry_idx for SIP within EGR_IP_TUNNEL */
                break;
            }
        }
    }

    /* Zero write buffer. */
    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm, DATA_TYPEf)) {
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, DATA_TYPEf, _BCM_L2GRE_EGRESS_DEST_VP_TYPE);
    } else
#endif
    {
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, VP_TYPEf, _BCM_L2GRE_EGRESS_DEST_VP_TYPE);
    }

    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            L2GRE__TUNNEL_INDEXf, tunnel_index);
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            L2GRE__DIPf, dip);

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        network_group = l2gre_port->network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
                l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
        BCM_IF_ERROR_RETURN(rv);

        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
            L2GRE__DVP_NETWORK_GROUPf, network_group);
    } else {
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                L2GRE__DVP_IS_NETWORK_PORTf, 0x1);
    }
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            L2GRE__DISABLE_VP_PRUNINGf, 0x0);
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            L2GRE__DELETE_VNTAGf, 0x1);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, L2GRE__CLASS_IDf, l2gre_port->if_class);

        /* L2 MTU - This is different from L3 MTU */
        if (l2gre_port->mtu > 0) {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                &egr_dvp_attribute, L2GRE__MTU_VALUEf, l2gre_port->mtu);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                &egr_dvp_attribute, L2GRE__MTU_ENABLEf, 0x1);
        }

        if (l2gre_port->flags & BCM_L2GRE_PORT_MULTICAST) {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                &egr_dvp_attribute, L2GRE__UUC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                &egr_dvp_attribute, L2GRE__UMC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                &egr_dvp_attribute, L2GRE__BC_DROPf, drop ? 1 : 0);
        }
    } else
#endif
    {
        /* Zero write buffer. */
        sal_memset(&egr_dvp_attribute_1, 0, sizeof(egr_dvp_attribute_1_entry_t));
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
            &egr_dvp_attribute_1, L2GRE__CLASS_IDf, l2gre_port->if_class);

        /* L2 MTU - This is different from L3 MTU */
        if (l2gre_port->mtu > 0) {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
                &egr_dvp_attribute_1, L2GRE__MTU_VALUEf, l2gre_port->mtu);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
                &egr_dvp_attribute_1, L2GRE__MTU_ENABLEf, 0x1);
        }

        if (l2gre_port->flags & BCM_L2GRE_PORT_MULTICAST) {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
                &egr_dvp_attribute_1, L2GRE__UUC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
                &egr_dvp_attribute_1, L2GRE__UMC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
                &egr_dvp_attribute_1, L2GRE__BC_DROPf, drop ? 1 : 0);
        }
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_DVP_ATTRIBUTE_1m,
                                MEM_BLOCK_ALL, vp, &egr_dvp_attribute_1));
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if(SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm, L2GRE__EVXLT_KEY_SELf)) {
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, L2GRE__EVXLT_KEY_SELf, L2GRE_DVP_EVXLT_KEY_VFI);
    }
#endif

    rv = soc_mem_write(unit, EGR_DVP_ATTRIBUTEm,
            MEM_BLOCK_ALL, vp, &egr_dvp_attribute);

    return rv;
}

#if defined(BCM_TRIDENT3_SUPPORT)
 /*
 * Function:
 *     _bcm_tr3_l2gre_acc_egress_dvp_set
 * Purpose:
 *     Set access egress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  l2gre_port
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2gre_acc_egress_dvp_set(int unit, int vp,
            bcm_l2gre_port_t *l2gre_port)
{
    int rv = BCM_E_NONE;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    int network_group = 0;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;

    if (SOC_MEM_FIELD_VALID(unit, mem, COMMON__DVP_NETWORK_GROUPf)) {
        sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
        BCM_IF_ERROR_RETURN(rv);
        network_group = l2gre_port->network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
                l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
        BCM_IF_ERROR_RETURN(rv);
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                COMMON__DVP_NETWORK_GROUPf, network_group);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    }
    return rv;
}
#endif

/*
 * Function:
 *     _bcm_tr3_l2gre_egress_dvp_get
 * Purpose:
 *     Get Egress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  l2gre_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_egress_dvp_get(int unit, int vp, bcm_l2gre_port_t *l2gre_port)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info;
    int rv=BCM_E_NONE;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    egr_dvp_attribute_1_entry_t  egr_dvp_attribute_1;
    int tunnel_index=-1;
    bcm_ip_t dip=0;                       /* DIP for tunnel header */
    int idx=0, num_vp=0;

    l2gre_info = L2GRE_INFO(unit);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
    rv = soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                                            MEM_BLOCK_ANY, vp, &egr_dvp_attribute);
    BCM_IF_ERROR_RETURN(rv);
    dip = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                            &egr_dvp_attribute, L2GRE__DIPf);

    /* Find DIP within Softare State */
    for (idx=0; idx < num_vp; idx++) {
        if (l2gre_info->l2gre_tunnel_init[idx].dip == dip) {
            tunnel_index = idx;
            break;
        }
    }

    BCM_GPORT_TUNNEL_ID_SET(l2gre_port->egress_tunnel_id, tunnel_index);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        l2gre_port->if_class = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                        &egr_dvp_attribute, L2GRE__CLASS_IDf);
        if (soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                    &egr_dvp_attribute, L2GRE__MTU_ENABLEf)) {
             l2gre_port->mtu = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                        &egr_dvp_attribute, L2GRE__MTU_VALUEf);
        }
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_DVP_ATTRIBUTE_1m,
                            MEM_BLOCK_ANY, vp, &egr_dvp_attribute_1));

        l2gre_port->if_class = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTE_1m,
                                        &egr_dvp_attribute_1, L2GRE__CLASS_IDf);
        if (soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTE_1m,
                    &egr_dvp_attribute_1, L2GRE__MTU_ENABLEf)) {
             l2gre_port->mtu = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTE_1m,
                                        &egr_dvp_attribute_1, L2GRE__MTU_VALUEf);
        }
    }

    return rv;
}

/*
 * Function:
 *		_bcm_tr3_l2gre_egress_dvp_reset
 * Purpose:
 *		Reet Egress DVP tables
 * Parameters:
 *		IN :  Unit
 *           IN :  vp
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_egress_dvp_reset(int unit, int vp)
{
    uint32 dvp[SOC_MAX_MEM_WORDS];

    sal_memset(dvp, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit,
            EGR_DVP_ATTRIBUTEm, MEM_BLOCK_ALL, vp, dvp));

    if (!SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                EGR_DVP_ATTRIBUTE_1m, MEM_BLOCK_ALL, vp, dvp));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_tr3_l2gre_egr_xlate_entry_set
 * Purpose:
 *   Reset L2GRE  egr_xlate Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  l2gre vpn info
 * Returns:
 *    BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_egr_xlate_entry_set(int unit, bcm_l2gre_vpn_config_t *vpn_info)
{
    int rv = BCM_E_NONE;
    int vfi_index = -1;
    int tpid_index = -1;
    soc_mem_t mem;
    uint32 vxlate[SOC_MAX_MEM_WORDS];

    if (NULL == vpn_info) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_valid(unit, vpn_info->vpn));
    _BCM_L2GRE_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn_info->vpn);

    /* Reset old egress vlan xlate */
    if (vpn_info->flags & BCM_L2GRE_VPN_REPLACE) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_egr_xlate_entry_reset(unit, vpn_info->vpn));
    }

    sal_memset(vxlate, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    /* Configurate EGR_VLAN_XLATE for Egress VPNID */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mem, vxlate,
            KEY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI);
        soc_mem_field32_set(unit, mem, vxlate,
            DATA_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI);
    } else
#endif
    {
        mem = EGR_VLAN_XLATEm;

        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, mem, vxlate,
                KEY_TYPEf, _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_VFI);
        } else if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
            soc_mem_field32_set(unit, mem, vxlate,
                ENTRY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI);
        }
        soc_mem_field32_set(unit, mem, vxlate, VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, vxlate, L2GRE_VFI__VFIf, vfi_index);
    soc_mem_field32_set(unit, mem, vxlate, L2GRE_VFI__VPNIDf,  vpn_info->vpnid);

    if (vpn_info->flags & BCM_L2GRE_VPN_SERVICE_TAGGED) {
        /* Configure EGR_VLAN_XLATE - SD_TAG setting */
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_sd_tag_set(unit, vpn_info, NULL, NULL, vxlate, &tpid_index));
    }

    rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vxlate);
    if (rv < 0)
    {
        if (tpid_index != -1) {
            _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        }
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *    _bcm_tr3_l2gre_egr_xlate_entry_reset
 * Purpose:
 *   Reset L2GRE  egr_xlate Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  vpn
 * Returns:
 *    BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_egr_xlate_entry_reset(int unit, bcm_vpn_t vpn)
{
    int rv = BCM_E_UNAVAIL;
    int index;
    int vfi;
    int tpid_index = -1;
    int action_present = 0, action_not_present = 0;
    soc_mem_t mem;
    uint32 vxlate[SOC_MAX_MEM_WORDS];

    sal_memset(vxlate, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    _BCM_L2GRE_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vxlate, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mem, vxlate,
            KEY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI);
        soc_mem_field32_set(unit, mem, vxlate,
            DATA_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI);
    } else
#endif
    {
        mem = EGR_VLAN_XLATEm;

        if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field32_set(unit, mem, vxlate,
                KEY_TYPEf, _BCM_TR3_L2GRE_KEY_TYPE_LOOKUP_VFI);
        } else if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
            soc_mem_field32_set(unit, mem, vxlate,
                ENTRY_TYPEf, _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_VFI);
        }
        soc_mem_field32_set(unit, mem, vxlate, VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, vxlate, L2GRE_VFI__VFIf, vfi);

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vxlate, vxlate, 0);
    if (rv == BCM_E_NOT_FOUND) {
        return BCM_E_NONE;
    } else if (rv != BCM_E_NONE) {
        return rv;
    }

    /* If tpid exist, delete it */
    action_present = soc_mem_field32_get(unit, mem, vxlate,
                        L2GRE_VFI__SD_TAG_ACTION_IF_PRESENTf);
    action_not_present = soc_mem_field32_get(unit, mem, vxlate,
                            L2GRE_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf);
    if (1 == action_present || 4 == action_present ||
        7 == action_present || 1 == action_not_present) {
        tpid_index = soc_mem_field32_get(unit, mem, vxlate,
                                L2GRE_VFI__SD_TAG_TPID_INDEXf);
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit, tpid_index));
    }

    /* Delete the entry from HW */
    BCM_IF_ERROR_RETURN(soc_mem_delete(unit, mem, MEM_BLOCK_ALL, vxlate));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_tr3_l2gre_port_untagged_profile_set
 * Purpose:
 *      Set  VLan profile per Physical port entry
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port  - (IN) Physical port
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_port_untagged_profile_set (int unit, bcm_port_t port)
{
    bcm_vlan_action_set_t action;
    uint32 ing_profile_idx = 0xffffffff;

    bcm_vlan_action_set_t_init(&action);
    action.ut_outer = 0x0; /* No Op */
    action.ut_inner = 0x0; /* No Op */

    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_action_profile_entry_add(
                unit, &action, &ing_profile_idx));

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
        _BCM_CPU_TABS_NONE, TAG_ACTION_PROFILE_PTRf, ing_profile_idx));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2gre_port_untagged_profile_reset
 * Purpose:
 *      Reset  VLan profile per Physical port entry
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port  - (IN) Physical port
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_port_untagged_profile_reset(int unit, bcm_port_t port)
{
    int ing_profile_idx = -1;
    int def_profile_idx = 0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit,
        port, TAG_ACTION_PROFILE_PTRf, &ing_profile_idx));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && (ing_profile_idx == 0)) {
        /*index 0 is invalid in Ingress Vlan Tag Action Profile*/
        return BCM_E_NONE;
    }
#endif
    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_action_profile_entry_delete(unit, ing_profile_idx));
#if defined(BCM_TRIDENT3_SUPPORT)
    /* default profile defined in _bcm_trx_vlan_action_profile_init */
    if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm)) {
        def_profile_idx = 1;
    }
#endif
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit,
        port, _BCM_CPU_TABS_NONE, TAG_ACTION_PROFILE_PTRf, def_profile_idx));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_sd_tag_set
 * Purpose:
 *      Program  SD_TAG settings
 * Parameters:
 *      unit           - (IN)  SOC unit #
 *      l2gre_port  - (IN)  L2GRE VP
 *      egr_nh_info  (IN/OUT) Egress NHop Info
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_sd_tag_set( int unit, bcm_l2gre_vpn_config_t *l2gre_vpn_info,
                         bcm_l2gre_port_t *l2gre_port,
                         _bcm_tr3_l2gre_nh_info_t  *egr_nh_info,
                         uint32   *vxlate_entry,
                         int *tpid_index )
{
   soc_mem_t  hw_mem;

    if (l2gre_vpn_info != NULL ) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            hw_mem = EGR_VLAN_XLATE_1_DOUBLEm;
        } else
#endif
        {
            hw_mem = EGR_VLAN_XLATEm;
        }

        if (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_VLAN_ADD) {
            if (l2gre_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_VIDf,
                                l2gre_vpn_info->egress_service_vlan);
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf,
                                0x1); /* ADD */
        }

        if (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_VLAN_TPID_REPLACE) {
            if (l2gre_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_VIDf,
                                l2gre_vpn_info->egress_service_vlan);
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x1); /* REPLACE_VID_TPID */
        } else if (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_VLAN_REPLACE) {
            if (l2gre_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_VIDf,
                                l2gre_vpn_info->egress_service_vlan);
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x2); /* REPLACE_VID_ONLY */
        } else if (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_VLAN_DELETE) {
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x3); /* DELETE */
        } else if (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_VLAN_PRI_TPID_REPLACE) {
               if (l2gre_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                   return  BCM_E_PARAM;
               }
               if (l2gre_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                   return  BCM_E_PARAM;
               }
               if (l2gre_vpn_info->pkt_cfi > 1) {
                   return  BCM_E_PARAM;
               }
               soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_VIDf,
                                l2gre_vpn_info->egress_service_vlan);
               if (soc_mem_field_valid(unit, hw_mem,
                                       L2GRE_VFI__SD_TAG_NEW_PRIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_NEW_PRIf,
                                l2gre_vpn_info->pkt_pri);
               }
               if (soc_mem_field_valid(unit, hw_mem,
                                       L2GRE_VFI__SD_TAG_NEW_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_NEW_CFIf,
                                l2gre_vpn_info->pkt_cfi);
               }

               soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x4); /* REPLACE_VID_PRI_TPID */

        } else if (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_VLAN_PRI_REPLACE ){
                if (l2gre_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                    return  BCM_E_PARAM;
                }
                if (l2gre_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (l2gre_vpn_info->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_VIDf,
                                l2gre_vpn_info->egress_service_vlan);

                if (soc_mem_field_valid(unit, hw_mem,
                                       L2GRE_VFI__SD_TAG_NEW_PRIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_NEW_PRIf,
                                l2gre_vpn_info->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       L2GRE_VFI__SD_TAG_NEW_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_NEW_CFIf,
                                l2gre_vpn_info->pkt_cfi);
                }

                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x5); /* REPLACE_VID_PRI */
        }else if (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_PRI_REPLACE ) {
                if (l2gre_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (l2gre_vpn_info->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       L2GRE_VFI__SD_TAG_NEW_PRIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_NEW_PRIf,
                                l2gre_vpn_info->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       L2GRE_VFI__SD_TAG_NEW_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_NEW_CFIf,
                                l2gre_vpn_info->pkt_cfi);
                }

                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x6); /* REPLACE_PRI_ONLY */

        } else if (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_TPID_REPLACE ) {
                 soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x7); /* REPLACE_TPID_ONLY */
        }

        if ((l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_VLAN_ADD) ||
            (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_VLAN_TPID_REPLACE) ||
            (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_TPID_REPLACE) ||
            (l2gre_vpn_info->flags & BCM_L2GRE_VPN_SERVICE_VLAN_PRI_TPID_REPLACE)) {
            /* TPID value is used */
            BCM_IF_ERROR_RETURN(
                _bcm_fb2_outer_tpid_entry_add(unit, l2gre_vpn_info->egress_service_tpid, tpid_index));
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, L2GRE_VFI__SD_TAG_TPID_INDEXf, *tpid_index);
        }
    } else  {

        if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_VLAN_ADD) {
            if (l2gre_port->egress_service_vlan >= BCM_VLAN_INVALID) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) 
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;
                } else
#endif
                {
                    return  BCM_E_PARAM;
                }
            } else {
                egr_nh_info->sd_tag_vlan = l2gre_port->egress_service_vlan;
            }

            egr_nh_info->sd_tag_action_not_present = 0x1; /* ADD */
        }

        if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_VLAN_TPID_REPLACE) {
            if (l2gre_port->egress_service_vlan >= BCM_VLAN_INVALID) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) 
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;
                } else
#endif
                {
                    return  BCM_E_PARAM;
                }
            } else {
                egr_nh_info->sd_tag_vlan = l2gre_port->egress_service_vlan;
            }
            /* REPLACE_VID_TPID */
            egr_nh_info->sd_tag_action_present = 0x1;
        } else if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_VLAN_REPLACE) {
            if (l2gre_port->egress_service_vlan >= BCM_VLAN_INVALID) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) 
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;
                } else
#endif
                {
                    return  BCM_E_PARAM;
                }
            } else {
                egr_nh_info->sd_tag_vlan = l2gre_port->egress_service_vlan;
            }
             /* REPLACE_VID_ONLY */
            egr_nh_info->sd_tag_action_present = 0x2;
        } else if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_VLAN_DELETE) {
            egr_nh_info->sd_tag_action_present = 0x3; /* DELETE */
            egr_nh_info->sd_tag_action_not_present = 0;
        } else if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_VLAN_PRI_TPID_REPLACE) {
            if (l2gre_port->egress_service_vlan >= BCM_VLAN_INVALID) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) 
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;
                } else
#endif
                {
                    return  BCM_E_PARAM;
                }
            } else {
                egr_nh_info->sd_tag_vlan = l2gre_port->egress_service_vlan;
            }

            if (l2gre_port->pkt_pri > BCM_PRIO_MAX) {
                return  BCM_E_PARAM;
            }
            if (l2gre_port->pkt_cfi > 1) {
                return  BCM_E_PARAM;
            }

            /* REPLACE_VID_PRI_TPID */
            egr_nh_info->sd_tag_pri = l2gre_port->pkt_pri;
            egr_nh_info->sd_tag_cfi = l2gre_port->pkt_cfi;
            egr_nh_info->sd_tag_action_present = 0x4;
        } else if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_VLAN_PRI_REPLACE ) {
            if (l2gre_port->egress_service_vlan >= BCM_VLAN_INVALID) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;
                } else
#endif
                {
                    return  BCM_E_PARAM;
                }
            } else {
                egr_nh_info->sd_tag_vlan = l2gre_port->egress_service_vlan;
            }
            if (l2gre_port->pkt_pri > BCM_PRIO_MAX) {
                return  BCM_E_PARAM;
            }
            if (l2gre_port->pkt_cfi > 1) {
                return  BCM_E_PARAM;
            }

            /* REPLACE_VID_PRI_ONLY */
            egr_nh_info->sd_tag_pri = l2gre_port->pkt_pri;
            egr_nh_info->sd_tag_cfi = l2gre_port->pkt_cfi;
            egr_nh_info->sd_tag_action_present = 0x5;
        } else if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_PRI_REPLACE ) {
            if (l2gre_port->pkt_pri > BCM_PRIO_MAX) {
                return  BCM_E_PARAM;
            }
            if (l2gre_port->pkt_cfi > 1) {
                return  BCM_E_PARAM;
            }
            /* REPLACE_PRI_ONLY */
            egr_nh_info->sd_tag_pri = l2gre_port->pkt_pri;
            egr_nh_info->sd_tag_cfi = l2gre_port->pkt_cfi;
            egr_nh_info->sd_tag_action_present = 0x6;
        } else if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_TPID_REPLACE ) {
            /* REPLACE_TPID_ONLY */
            egr_nh_info->sd_tag_action_present = 0x7;
        }

        if ((l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_VLAN_ADD) ||
            (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_VLAN_TPID_REPLACE) ||
            (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_TPID_REPLACE) ||
            (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_VLAN_PRI_TPID_REPLACE)) {
            /* TPID value is used */
            BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_add(unit, l2gre_port->egress_service_tpid, tpid_index));
            egr_nh_info->tpid_index = *tpid_index;
        }
    }
    return  BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_sd_tag_get
 * Purpose:
 *      Get  SD_TAG settings
 * Parameters:
 *      unit           - (IN)  SOC unit #
 *      l2gre_port  - (IN)  L2GRE VP
 *      egr_nh_info  (IN/OUT) Egress NHop Info
 * Returns:
 *      BCM_E_XXX
 */

STATIC void
_bcm_tr3_l2gre_sd_tag_get( int unit, bcm_l2gre_vpn_config_t *l2gre_vpn_info,
                         bcm_l2gre_port_t *l2gre_port,
                         egr_l3_next_hop_entry_t *egr_nh,
                         uint32   *vxlate_entry,
                         int network_port_flag )
{
    int action_present, action_not_present;
    soc_mem_t  hw_mem;

    if (network_port_flag) {
#if defined(BCM_TRIDENT3_SUPPORT)
       if (soc_feature(unit, soc_feature_base_valid)) {
           hw_mem = EGR_VLAN_XLATE_1_DOUBLEm;
       } else
#endif
       {
           hw_mem = EGR_VLAN_XLATEm;
       }

       action_present = soc_mem_field32_get(unit, hw_mem, vxlate_entry,
                                    L2GRE_VFI__SD_TAG_ACTION_IF_PRESENTf);
       action_not_present = soc_mem_field32_get(unit, hw_mem, vxlate_entry,
                                    L2GRE_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf);

       switch (action_present) {
         case 0:
            if (action_not_present == 1) {
                l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_ADD;
            }
            l2gre_vpn_info->egress_service_vlan =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_VIDf);
            break;

         case 1:
            if (action_not_present == 1) {
                l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_ADD;
            }
            l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_TPID_REPLACE;
            l2gre_vpn_info->egress_service_vlan =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_VIDf);
            break;

         case 2:
            if (action_not_present == 1) {
                l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_ADD;
            }
            l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_REPLACE;
            l2gre_vpn_info->egress_service_vlan =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_VIDf);
            break;

         case 3:
            if (action_not_present == 1) {
                l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_ADD;
            }
            l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_DELETE;
            l2gre_vpn_info->egress_service_vlan = BCM_VLAN_INVALID;
            break;

         case 4:
            if (action_not_present == 1) {
                l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_ADD;
            }
            l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_PRI_TPID_REPLACE;
            l2gre_vpn_info->egress_service_vlan =
                 soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_VIDf);
            l2gre_vpn_info->pkt_pri =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_NEW_PRIf);
            l2gre_vpn_info->pkt_cfi =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_NEW_CFIf);
            break;

         case 5:
            if (action_not_present == 1) {
                l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_ADD;
            }
            l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_PRI_REPLACE;
            l2gre_vpn_info->egress_service_vlan =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_VIDf);
            l2gre_vpn_info->pkt_pri =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_NEW_PRIf);
            l2gre_vpn_info->pkt_cfi =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_NEW_CFIf);
            break;

         case 6:
            if (action_not_present == 1) {
                l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_ADD;
            }
            l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_PRI_REPLACE;
            l2gre_vpn_info->egress_service_vlan =   BCM_VLAN_INVALID;
            l2gre_vpn_info->pkt_pri =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_NEW_PRIf);
            l2gre_vpn_info->pkt_cfi =
                soc_mem_field32_get(unit, hw_mem, vxlate_entry, L2GRE_VFI__SD_TAG_NEW_CFIf);
            break;

         case 7:
            if (action_not_present == 1) {
                l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_VLAN_ADD;
            }
            l2gre_vpn_info->flags |= BCM_L2GRE_VPN_SERVICE_TPID_REPLACE;
            l2gre_vpn_info->egress_service_vlan = BCM_VLAN_INVALID;
            break;

         default:
            break;
       }
    }else {
       action_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                     egr_nh,
                                     SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
       action_not_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                     egr_nh,
                                     SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);

       switch (action_present) {
         case 0:
            if (action_not_present == 1) {
                l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_ADD;
            }
            l2gre_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
            break;

         case 1:
            if (action_not_present == 1) {
                l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_ADD;
            }
            l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_TPID_REPLACE;
            l2gre_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
            break;

         case 2:
            if (action_not_present == 1) {
                l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_ADD;
            }
            l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_REPLACE;
            l2gre_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
            break;

         case 3:
            if (action_not_present == 1) {
                l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_ADD;
            }
            l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_DELETE;
            l2gre_port->egress_service_vlan = BCM_VLAN_INVALID;
            break;

         case 4:
            if (action_not_present == 1) {
                l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_ADD;
            }
            l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_PRI_TPID_REPLACE;
            l2gre_port->egress_service_vlan =
                 soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
            l2gre_port->pkt_pri =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_PRIf);
            l2gre_port->pkt_cfi =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_CFIf);
            break;

         case 5:
            if (action_not_present == 1) {
                l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_ADD;
            }
            l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_PRI_REPLACE;
            l2gre_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
            l2gre_port->pkt_pri =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_PRIf);
            l2gre_port->pkt_cfi =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_CFIf);
            break;

         case 6:
            if (action_not_present == 1) {
                l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_ADD;
            }
            l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_PRI_REPLACE;
            l2gre_port->egress_service_vlan =   BCM_VLAN_INVALID;
            l2gre_port->pkt_pri =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_PRIf);
            l2gre_port->pkt_cfi =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_CFIf);
            break;

         case 7:
            if (action_not_present == 1) {
                l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_VLAN_ADD;
            }
            l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_TPID_REPLACE;
            l2gre_port->egress_service_vlan = BCM_VLAN_INVALID;
            break;

         default:
            break;
       }
    }
}

/*
 * Function:
 *      _bcm_tr3_l2gre_nexthop_entry_modify
 * Purpose:
 *      Modify Egress entry
 * Parameters:
 *      unit - Device Number
 *      nh_index - Next Hop Index
 *      new_entry_type - New Entry type
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_nexthop_entry_modify(int unit, int nh_index, int drop,
              _bcm_tr3_l2gre_nh_info_t  *egr_nh_info, int new_entry_type)
{
    egr_l3_next_hop_entry_t egr_nh;
    int rv = BCM_E_NONE;
    uint32 old_entry_type=0, intf_num=0, vntag_actions=0;
    uint32 vntag_dst_vif=0, vntag_pf=0, vntag_force_lf=0;
    uint32 etag_pcp_de_sourcef=0, etag_pcpf=0, etag_dot1p_mapping_ptr=0;
    bcm_mac_t mac_addr;                 /* Next hop forwarding destination mac. */
    soc_field_t type_field;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        type_field = DATA_TYPEf;
    } else
#endif
    {
        type_field = ENTRY_TYPEf;
    }

    sal_memset(&mac_addr, 0, sizeof (mac_addr));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                      nh_index, &egr_nh));

    old_entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, type_field);

    if ((new_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW) &&
         (old_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_L3_VIEW)) {
            /*Zero buffers.*/
            sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
            if (egr_nh_info->sd_tag_vlan != -1) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, SD_TAG__SD_TAG_NOT_VALIDf)) {
                        if (egr_nh_info->sd_tag_vlan == BCM_VLAN_INVALID) {
                            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                   &egr_nh, SD_TAG__SD_TAG_NOT_VALIDf, 1);
                        } else {
                            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                   &egr_nh, SD_TAG__SD_TAG_NOT_VALIDf, 0);
                            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG__SD_TAG_VIDf, egr_nh_info->sd_tag_vlan);
                        }
                } else {
                    if (egr_nh_info->sd_tag_vlan < BCM_VLAN_INVALID) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG__SD_TAG_VIDf, egr_nh_info->sd_tag_vlan);
                    }
                }
            } else
#endif
                {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG__SD_TAG_VIDf, egr_nh_info->sd_tag_vlan);
                }
            }

            if (egr_nh_info->sd_tag_action_present != -1) {
                   soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__SD_TAG_ACTION_IF_PRESENTf,
                             egr_nh_info->sd_tag_action_present);
            }

            if (egr_nh_info->sd_tag_action_not_present != -1) {
                   soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf,
                             egr_nh_info->sd_tag_action_not_present);
            }

            if (egr_nh_info->sd_tag_pri != -1) {
                 if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__NEW_PRIf)) {
                       soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__NEW_PRIf,
                             egr_nh_info->sd_tag_pri);
                 }
            }

            if (egr_nh_info->sd_tag_cfi != -1) {
                   if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__NEW_CFIf)) {
                       soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__NEW_CFIf,
                             egr_nh_info->sd_tag_cfi);
                   }
            }

            if (egr_nh_info->tpid_index != -1) {
                  soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf,
                                egr_nh_info->tpid_index);
            }

            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    SD_TAG__BC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    SD_TAG__UUC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    SD_TAG__UMC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    SD_TAG__DVPf, egr_nh_info->dvp);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
            } else
#endif
            {
                if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, SD_TAG__DVP_NETWORK_GROUPf,
                            egr_nh_info->dvp_network_group_id);
                } else {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, SD_TAG__DVP_IS_NETWORK_PORTf,
                            egr_nh_info->dvp_is_network);
                }
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    SD_TAG__HG_LEARN_OVERRIDEf, egr_nh_info->is_eline ? 1 : 0);
            /* HG_MODIFY_ENABLE must be 0x0 for Ingress and Egress Chip */
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    SD_TAG__HG_MODIFY_ENABLEf, 0x0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    SD_TAG__HG_HDR_SELf, 1);

            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            type_field, _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW); /* SD_TAG view */
    } else if ((new_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_L3_VIEW) &&
            (old_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            type_field, _BCM_L2GRE_EGR_NEXT_HOP_L3_VIEW); /* Normal view */
            /* No intf_num in SDTAG view, get first valid index */
            if (soc_feature(unit, soc_feature_l3_egr_intf_zero_invalid)) {
                intf_num = soc_mem_index_min(unit,
                    BCM_XGS3_L3_MEM(unit, intf)) + 1;
            } else {
                intf_num = soc_mem_index_min(unit,
                    BCM_XGS3_L3_MEM(unit, intf));
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                   L3__INTF_NUMf, intf_num);
    } else if ((new_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_L3MC_VIEW) &&
         (old_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_L3_VIEW)) {

        /* Get mac address. */
        soc_mem_mac_addr_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__MAC_ADDRESSf, mac_addr);

        /* Get int_num */
        intf_num = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__INTF_NUMf);

        vntag_actions = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__VNTAG_ACTIONSf);

        vntag_dst_vif = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__VNTAG_DST_VIFf);

        vntag_pf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__VNTAG_Pf);

        vntag_force_lf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__VNTAG_FORCE_Lf);

        etag_pcp_de_sourcef = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__ETAG_PCP_DE_SOURCEf);

        etag_pcpf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__ETAG_PCPf);

        etag_dot1p_mapping_ptr = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__ETAG_DOT1P_MAPPING_PTRf);

        sal_memset(&egr_nh, 0, sizeof (egr_nh));
        soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__MAC_ADDRESSf, mac_addr);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__INTF_NUMf, intf_num);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__DVPf, egr_nh_info->dvp);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__NEXT_PTR_TYPEf, 1);
        } else
#endif
        {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                        L3MC__DVP_VALIDf, 0x1);
        }
        if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__L2_MC_DA_DISABLEf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_DA_DISABLEf, 0x1);
        }
        if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__L2_MC_SA_DISABLEf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_SA_DISABLEf, 0x1);
        }
        if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__L2_MC_VLAN_DISABLEf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_VLAN_DISABLEf, 0x1);
        }
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_DROPf, 0x0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L3_DROPf, drop ? 1 : 0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L3MC_USE_CONFIGURED_MACf, 0x1);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__VNTAG_ACTIONSf, vntag_actions);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__VNTAG_DST_VIFf, vntag_dst_vif);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__VNTAG_Pf, vntag_pf);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__VNTAG_FORCE_Lf, vntag_force_lf);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__ETAG_PCP_DE_SOURCEf, etag_pcp_de_sourcef);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__ETAG_PCPf, etag_pcpf);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__ETAG_DOT1P_MAPPING_PTRf, etag_dot1p_mapping_ptr);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                type_field, _BCM_L2GRE_EGR_NEXT_HOP_L3MC_VIEW);
    } else if ((new_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_L3MC_VIEW) &&
             (old_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_L3MC_VIEW)) {

        /* Get mac address. */
        soc_mem_mac_addr_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__MAC_ADDRESSf, mac_addr);

        /* Get int_num */
        intf_num = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__INTF_NUMf);

        vntag_actions = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__VNTAG_ACTIONSf);

        vntag_dst_vif = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__VNTAG_DST_VIFf);

        vntag_pf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__VNTAG_Pf);

        vntag_force_lf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__VNTAG_FORCE_Lf);

        etag_pcp_de_sourcef = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__ETAG_PCP_DE_SOURCEf);

        etag_pcpf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__ETAG_PCPf);

        etag_dot1p_mapping_ptr = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__ETAG_DOT1P_MAPPING_PTRf);

        soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3MC__MAC_ADDRESSf, mac_addr);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__INTF_NUMf, intf_num);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    L3MC__NEXT_PTR_TYPEf, 0x1);
        } else
#endif
        {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    L3MC__DVP_VALIDf, 0x1);
        }
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__DVPf, egr_nh_info->dvp);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_DA_DISABLEf, 0x1);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_SA_DISABLEf, 0x1);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_VLAN_DISABLEf, 0x1);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_DROPf, 0x0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L3_DROPf, drop ? 1 : 0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L3MC_USE_CONFIGURED_MACf, 0x1);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__VNTAG_ACTIONSf, vntag_actions);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__VNTAG_DST_VIFf, vntag_dst_vif);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__VNTAG_Pf, vntag_pf);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__VNTAG_FORCE_Lf, vntag_force_lf);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__ETAG_PCP_DE_SOURCEf, etag_pcp_de_sourcef);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__ETAG_PCPf, etag_pcpf);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__ETAG_DOT1P_MAPPING_PTRf, etag_dot1p_mapping_ptr);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                type_field, _BCM_L2GRE_EGR_NEXT_HOP_L3MC_VIEW);
    } else if ((new_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_L3_VIEW) &&
         (old_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_L3MC_VIEW)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            type_field, _BCM_L2GRE_EGR_NEXT_HOP_L3_VIEW);
    } else if ((new_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW) &&
        (old_entry_type == _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW)) {

        /* Case of L2GRE Proxy Next-Hop */
        if (egr_nh_info->sd_tag_vlan != -1) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_vp_sharing)) {
                if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, SD_TAG__SD_TAG_NOT_VALIDf)) {
                    if (egr_nh_info->sd_tag_vlan == BCM_VLAN_INVALID) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                   &egr_nh, SD_TAG__SD_TAG_NOT_VALIDf, 1);
                    } else {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                   &egr_nh, SD_TAG__SD_TAG_NOT_VALIDf, 0);
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG__SD_TAG_VIDf, egr_nh_info->sd_tag_vlan);
                    }
                } else {
                    if (egr_nh_info->sd_tag_vlan < BCM_VLAN_INVALID) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG__SD_TAG_VIDf, egr_nh_info->sd_tag_vlan);
                    }
                }
            } else
#endif
            {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG__SD_TAG_VIDf, egr_nh_info->sd_tag_vlan);
            }
        }

        if (egr_nh_info->sd_tag_action_present != -1) {
               soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         SD_TAG__SD_TAG_ACTION_IF_PRESENTf,
                         egr_nh_info->sd_tag_action_present);
        }

        if (egr_nh_info->sd_tag_action_not_present != -1) {
               soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf,
                         egr_nh_info->sd_tag_action_not_present);
        }

        if (egr_nh_info->sd_tag_pri != -1) {
             if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__NEW_PRIf)) {
                   soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         SD_TAG__NEW_PRIf,
                         egr_nh_info->sd_tag_pri);
             }
        }

        if (egr_nh_info->sd_tag_cfi != -1) {
               if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__NEW_CFIf)) {
                   soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         SD_TAG__NEW_CFIf,
                         egr_nh_info->sd_tag_cfi);
               }
        }

        if (egr_nh_info->tpid_index != -1) {
              soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf,
                            egr_nh_info->tpid_index);
        }

        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                SD_TAG__DVPf, egr_nh_info->dvp);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            if (egr_nh_info->sd_tag_cfi != -1) {
                if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, SD_TAG__DVP_NETWORK_GROUPf,
                            egr_nh_info->dvp_network_group_id);
                } else {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, SD_TAG__DVP_IS_NETWORK_PORTf,
                            egr_nh_info->dvp_is_network);
                }
            }
        }

    }

    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, &egr_nh);
    return rv;
}

/*
 * Function:
 *           _bcm_tr3_l2gre_next_hop_set
 * Purpose:
 *           Set L2GRE NextHop entry
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
_bcm_tr3_l2gre_next_hop_set(int unit, int nh_index, uint32 flags)
{
    int rv=BCM_E_NONE;
    bcm_port_t port = -1;
    bcm_module_t modid = -1, local_modid = -1;
    bcm_trunk_t tgid = -1;
    int  num_ports=0, idx=-1;
    bcm_port_t   local_ports[SOC_MAX_NUM_PORTS];
    int  old_nh_index=-1;
    int replace=0;
    int gport = 0;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_trunk_member_t *tmp_member_gports_array = NULL;
#endif

    replace = flags & BCM_L3_REPLACE;

    BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_nexthop_glp_get(unit,
                            nh_index, &modid, &port, &tgid));
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &local_modid));
    if (tgid != -1) {
         rv = _bcm_trunk_id_validate(unit, tgid);
         if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
         }
         rv = bcm_esw_trunk_get(unit, tgid, NULL, 0, NULL, &num_ports);
         if (BCM_FAILURE(rv)) {
             return BCM_E_PORT;
         }
         if (num_ports == 0) {
             return BCM_E_NONE;
         }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
         if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
             tmp_member_gports_array = sal_alloc(
                     sizeof(bcm_trunk_member_t) * num_ports, "tmp_member_gports_arry");
             if (tmp_member_gports_array == NULL) {
                 return BCM_E_MEMORY;
             }
             sal_memset(tmp_member_gports_array, 0,
                 sizeof(bcm_trunk_member_t) * num_ports);
             rv = bcm_esw_trunk_get(unit, tgid, NULL,
                      num_ports, tmp_member_gports_array, &num_ports);
             if (BCM_FAILURE(rv)) {
                 goto clean_up;
             }
         } else
#endif
         {
             BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                        SOC_MAX_NUM_PORTS, local_ports, &num_ports));
             modid = local_modid;
         }
    } else {
        if (modid != local_modid) {
            if (!soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                    /* Ignore EGR_PORT_TO_NHI_MAPPINGs */
                    return BCM_E_NONE;
            }
        }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
            /* Alloc one trunk member size memory */
            tmp_member_gports_array = sal_alloc(
                 sizeof(bcm_trunk_member_t), "tmp_member_gports_arry");
            if (tmp_member_gports_array == NULL) {
                 return BCM_E_MEMORY;
            }
            sal_memset(tmp_member_gports_array, 0,
                 sizeof(bcm_trunk_member_t));
            BCM_GPORT_MODPORT_SET(gport, modid, port);
            tmp_member_gports_array[num_ports++].gport = gport;
        } else
#endif
        {
            local_ports[num_ports++] = port;
        }
    }

    if (!(flags & BCM_L3_IPMC)) { /* Only for L2GRE Unicast Nexthops */
       for (idx = 0; idx < num_ports; idx++) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                gport = tmp_member_gports_array[idx].gport;
            } else
#endif
            {
                BCM_GPORT_MODPORT_SET(gport, modid, local_ports[idx]);
            }

            rv = _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
            if (old_nh_index && !replace) {
               if (old_nh_index != nh_index) {
                    /* Limitation: For TD/TR3,
                    L2GRE egress port can be configured with one NHI*/
                    rv = BCM_E_RESOURCE;
                    goto clean_up;
                }
            } else {
                rv = _bcm_trx_gport_to_nhi_set(unit, gport, nh_index);
                if (BCM_FAILURE(rv)) {
                    goto clean_up;
                }
            }
        }
    }

clean_up:
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (NULL != tmp_member_gports_array) {
        sal_free(tmp_member_gports_array);
    }
#endif
    return rv;
}

/*
 * Function:
 *           bcm_tr3_l2gre_nexthop_get
 * Purpose:
 *          Get L2GRE NextHop entry
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_tr3_l2gre_nexthop_get(int unit, int nh_index, bcm_l2gre_port_t *l2gre_port)
{
    soc_mem_t      hw_mem;
    egr_l3_next_hop_entry_t egr_nh;

    hw_mem = EGR_L3_NEXT_HOPm;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem,
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3__DVP_VALIDf)) {
        if (0x1 == soc_mem_field32_get(unit, hw_mem, &egr_nh,
                                      L3__DVP_VALIDf)) {
             l2gre_port->flags |= BCM_L2GRE_PORT_MULTICAST;
        }
    } else {
        if (soc_mem_field32_get(unit, hw_mem, &egr_nh, L3__DVPf) != 0) {
             l2gre_port->flags |= BCM_L2GRE_PORT_MULTICAST;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *           bcm_tr3_l2gre_egr_nexthop_multicast_set
 * Purpose:
 *           Set L2GRE Egress NextHop Multicast
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

STATIC int
bcm_tr3_l2gre_egr_nexthop_multicast_set(int unit, int nh_index, int vp, bcm_l2gre_port_t *l2gre_port)
{
    soc_mem_t      hw_mem;
    egr_l3_next_hop_entry_t egr_nh;

    if (l2gre_port->flags & BCM_L2GRE_PORT_MULTICAST) {
        hw_mem = EGR_L3_NEXT_HOPm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem,
                                      MEM_BLOCK_ANY, nh_index, &egr_nh));
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3__DVP_VALIDf)) {
            soc_mem_field32_set(unit, hw_mem, &egr_nh,
                                      L3__DVP_VALIDf, 0x1);
        }
        soc_mem_field32_set(unit, hw_mem, &egr_nh,
                                  L3__DVPf, vp);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, hw_mem,
                                  MEM_BLOCK_ALL, nh_index, &egr_nh));
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *           bcm_tr3_l2gre_port_egress_nexthop_reset
 * Purpose:
 *           Reset L2GRE Egress NextHop
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_tr3_l2gre_port_egress_nexthop_reset(int unit, int nh_index, int vp_type, int vp, bcm_vpn_t vpn)
{
    egr_l3_next_hop_entry_t egr_nh;
    int rv=BCM_E_NONE;
    int action_present=0, action_not_present=0, old_tpid_idx = -1;
    uint32  entry_type=0;

    if (vp_type== _BCM_L2GRE_DEST_VP_TYPE_ACCESS) {
        /* egressing into a regular port */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf);
        } else
#endif
        {
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
        }
        if (entry_type != _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW) {
            /* coverity[assigned_value] */
            rv = BCM_E_PARAM;
        }

        action_present =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
        action_not_present =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
        if ((action_not_present == 0x1) || (action_present == 0x1)) {
            old_tpid_idx =
                  soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_TPID_INDEXf);
        }

        /* Normalize Next-hop Entry -- L3 View */
        rv = _bcm_tr3_l2gre_nexthop_entry_modify(unit, nh_index, 0,
                             NULL, _BCM_L2GRE_EGR_NEXT_HOP_L3_VIEW);

    }

    if (old_tpid_idx != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx);
    }
    return rv;
}

/*
 * Function:
 *		bcm_tr3_l2gre_nexthop_reset
 * Purpose:
 *		Reset L2GRE NextHop entry
 * Parameters:
 *		IN :  Unit
 *           IN :  nh_index
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
bcm_tr3_l2gre_nexthop_reset(int unit, int nh_index)
{
    int rv=BCM_E_NONE;
    ing_l3_next_hop_entry_t ing_nh;
    bcm_port_t      port=0;
    bcm_trunk_t tgid= BCM_TRUNK_INVALID;
    int  num_ports=0, idx=-1;
    bcm_port_t   local_ports[SOC_MAX_NUM_PORTS];
    soc_mem_t      hw_mem;
    bcm_module_t modid=0, local_modid=0;
    int  old_nh_index=-1;
    int  gport = 0;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_trunk_member_t *tmp_member_gports_array = NULL;
#endif

    hw_mem = ING_L3_NEXT_HOPm;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem,
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
                                            DROPf, 0x0);
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
                                            ENTRY_TYPEf, 0x0);
    if (soc_mem_field_valid(unit, ING_L3_NEXT_HOPm, DVP_ATTRIBUTE_1_INDEXf) ) {
         soc_mem_field32_set(unit, hw_mem,
                        &ing_nh, DVP_ATTRIBUTE_1_INDEXf, 0);
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, hw_mem,
                                            MEM_BLOCK_ALL, nh_index, &ing_nh));
    rv = _bcm_tr3_l2gre_nexthop_glp_get(unit,
            nh_index, &modid, &port, &tgid);

    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_NOT_FOUND) {
        /* not a GLP, could be a DVP in riot case. Do not process GLP */
            return BCM_E_NONE;
        } else {
            return rv;
        }
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &local_modid));
    if (tgid != BCM_TRUNK_INVALID) {
         rv = _bcm_trunk_id_validate(unit, tgid);
         if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
         }
         rv = bcm_esw_trunk_get(unit, tgid, NULL, 0, NULL, &num_ports);
         if (BCM_FAILURE(rv)) {
             return BCM_E_PORT;
         }
         if (num_ports == 0) {
             return BCM_E_NONE;
         }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
         if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
             tmp_member_gports_array = sal_alloc(
                 sizeof(bcm_trunk_member_t) * num_ports, "tmp_member_gports_arry");
             if (tmp_member_gports_array == NULL) {
                 return BCM_E_MEMORY;
             }
             sal_memset(tmp_member_gports_array, 0,
                        sizeof(bcm_trunk_member_t) * num_ports);
             rv = bcm_esw_trunk_get(unit, tgid, NULL,
                          num_ports, tmp_member_gports_array, &num_ports);
             if (BCM_FAILURE(rv)) {
                 goto clean_up;
             }
         } else
#endif
         {
             BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                     SOC_MAX_NUM_PORTS, local_ports, &num_ports));
             modid = local_modid;
         }
    } else {
        if (modid != local_modid) {
            if (!soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                /* Ignore EGR_PORT_TO_NHI_MAPPINGs */
                return BCM_E_NONE;
            }
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
            /* Alloc one trunk member size memory */
            tmp_member_gports_array = sal_alloc(
                 sizeof(bcm_trunk_member_t), "tmp_member_gports_arry");
            if (tmp_member_gports_array == NULL) {
                 return BCM_E_MEMORY;
            }
            sal_memset(tmp_member_gports_array, 0,
                       sizeof(bcm_trunk_member_t));
            BCM_GPORT_MODPORT_SET(gport, modid, port);
            tmp_member_gports_array[num_ports++].gport = gport;
        } else
#endif
        {
            local_ports[num_ports++] = port;
        }
    }

     for (idx = 0; idx < num_ports; idx++) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
            gport = tmp_member_gports_array[idx].gport;
        } else
#endif
        {
            BCM_GPORT_MODPORT_SET(gport, modid, local_ports[idx]);
        }

        rv = _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }
        if (old_nh_index == nh_index) {
            rv = _bcm_trx_gport_to_nhi_set(unit, gport, 0);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
        }
    }

clean_up:
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (NULL != tmp_member_gports_array) {
        sal_free(tmp_member_gports_array);
    }
#endif
    return rv;
}


/*
 * Function:
 *           bcm_tr3_l2gre_egress_set
 * Purpose:
 *           Set [MTU] L2GRE Egress NextHop
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_tr3_l2gre_egress_set(int unit, int nh_index, uint32 flags)
{
    ing_l3_next_hop_entry_t ing_nh;
    soc_mem_t      hw_mem;
    int rv=BCM_E_NONE;

    hw_mem = ING_L3_NEXT_HOPm;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem,
        MEM_BLOCK_ANY, nh_index, &ing_nh));
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
        ENTRY_TYPEf, _BCM_L2GRE_INGRESS_NEXT_HOP_ENTRY_TYPE); /* L2 DVP */
    if (SOC_MEM_FIELD_VALID(unit, hw_mem, MTU_SIZEf)) {
        soc_mem_field32_set(unit, hw_mem, &ing_nh, MTU_SIZEf, 0x3fff);
    }
    else if (soc_mem_field_valid(unit, ING_L3_NEXT_HOPm,
                 DVP_ATTRIBUTE_1_INDEXf)) {
    /* Set default ING_L3_NEXT_HOP_ATTRIBUTE_1 MTU_SIZE
       * Note: Assumption that first entry has MTU size of 0x3fff
       */
        soc_mem_field32_set(unit, hw_mem,
            &ing_nh, DVP_ATTRIBUTE_1_INDEXf, 0);
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, hw_mem,
                            MEM_BLOCK_ALL, nh_index, &ing_nh));

    rv = _bcm_tr3_l2gre_next_hop_set(unit, nh_index, flags);
    return rv;
}

/*
 * Function:
 *           bcm_tr3_l2gre_egress_reset
 * Purpose:
 *           Reset L2GRE Egress NextHop
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_tr3_l2gre_egress_reset(int unit, int nh_index)
{
    ing_l3_next_hop_entry_t ing_nh;
    soc_mem_t      hw_mem;
    int rv=BCM_E_NONE;

    hw_mem = ING_L3_NEXT_HOPm;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem,
                MEM_BLOCK_ANY, nh_index, &ing_nh));
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
                ENTRY_TYPEf, 0x0);
    if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm, MTU_SIZEf)) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, MTU_SIZEf, 0x0);
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, hw_mem,
                                  MEM_BLOCK_ALL, nh_index, &ing_nh));

    rv = bcm_tr3_l2gre_nexthop_reset(unit, nh_index);
    return rv;
}


/*
 * Function:
 *           bcm_tr3_l2gre_egress_get
 * Purpose:
 *           Get L2GRE Egress NextHop
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_tr3_l2gre_egress_get(int unit, bcm_l3_egress_t *egr, int nh_index)
{
    int int_l3_flag;
    int_l3_flag = BCM_XGS3_L3_ENT_FLAG(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                                       nh_index);
    if (int_l3_flag == _BCM_L3_L2GRE_ONLY) {
        egr->flags |= BCM_L3_L2GRE_ONLY;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_tr3_l2gre_port_nh_add
 * Purpose:
 *      Add L2GRE Next Hop entry
 * Parameters:
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_port_nh_add(int unit, bcm_l2gre_port_t *l2gre_port, int vp,
                            bcm_vpn_t vpn, int drop)
{
    egr_l3_next_hop_entry_t egr_nh;
    _bcm_tr3_l2gre_nh_info_t egr_nh_info;
    int rv=BCM_E_NONE;
    int action_present, action_not_present, tpid_index = -1;
    int old_tpid_idx = -1;
    uint32 mpath_flag=0;
    int vp_nh_ecmp_index=-1;
    int ref_count = 0;


    egr_nh_info.dvp = vp;
    egr_nh_info.entry_type = -1;
    egr_nh_info.dvp_is_network = -1;
    egr_nh_info.sd_tag_action_present = -1;
    egr_nh_info.sd_tag_action_not_present = -1;
    egr_nh_info.intf_num = -1;
    egr_nh_info.sd_tag_vlan = -1;
    egr_nh_info.sd_tag_pri = -1;
    egr_nh_info.sd_tag_cfi = -1;
    egr_nh_info.tpid_index = -1;
    egr_nh_info.is_eline = 0; /* Change to 1 for Eline */
    egr_nh_info.dvp_network_group_id = 0;

    /*
     * Get egress next-hop index from egress object and
     * increment egress object reference count.
     */
    rv = bcm_xgs3_get_nh_from_egress_object(unit, l2gre_port->egress_if,
                                            &mpath_flag, 1, &vp_nh_ecmp_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Read the existing egress next_hop entry */
    if (mpath_flag == 0) {
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                      vp_nh_ecmp_index, &egr_nh);
        _BCM_L2GRE_CLEANUP(rv)
    }

    if (l2gre_port->flags & BCM_L2GRE_PORT_EGRESS_TUNNEL) {

        /* Program DVP entry - Egress */
        rv = _bcm_tr3_l2gre_egress_dvp_set(unit, vp, drop, l2gre_port);
        _BCM_L2GRE_CLEANUP(rv);


        /* Program DVP entry  - Ingress  */
        rv = _bcm_tr3_l2gre_ingress_dvp_set(unit, vp, mpath_flag, vp_nh_ecmp_index, l2gre_port);
        _BCM_L2GRE_CLEANUP(rv);

        /* Default Split Horizon Group Id
         * 0 - For Access Port;1 - For Network Port*/
        if (l2gre_port->flags & BCM_L2GRE_PORT_MULTICAST) {
            if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
                egr_nh_info.dvp_network_group_id =
                    (_BCM_SWITCH_NETWORK_GROUP_ID_VALID
                        (unit, l2gre_port->network_group_id)) ?
                            l2gre_port->network_group_id : 1;
            } else {
                egr_nh_info.dvp_is_network = 1; /* Enable */
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
                egr_nh_info.entry_type =
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf);
            } else
#endif
            {
                egr_nh_info.entry_type =
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
            }
            if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit) ||SOC_IS_TRIDENT3X(unit)) {
                  rv = _bcm_tr3_l2gre_nexthop_entry_modify(unit, vp_nh_ecmp_index, drop,
                             &egr_nh_info, _BCM_TD2_L2GRE_EGR_NEXT_HOP_L3MC_VIEW);
            } else if (SOC_IS_TRIUMPH3(unit)){
                  rv = bcm_tr3_l2gre_egr_nexthop_multicast_set(unit, vp_nh_ecmp_index, vp, l2gre_port);
            }
            _BCM_L2GRE_CLEANUP(rv);
        }
    } else {
        /* Egress into Access - Non Tunnel */
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            egr_nh_info.dvp_network_group_id =
                (_BCM_SWITCH_NETWORK_GROUP_ID_VALID
                    (unit, l2gre_port->network_group_id)) ?
                        l2gre_port->network_group_id : 0;
#if defined(BCM_TRIDENT3_SUPPORT)
            if(SOC_IS_TRIDENT3X(unit)) {
                rv = _bcm_tr3_l2gre_acc_egress_dvp_set(unit, vp, l2gre_port);
                _BCM_L2GRE_CLEANUP(rv);
            }
#endif
        } else {
            egr_nh_info.dvp_is_network = 0; /* Disable */
        }
        if (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE) {
            action_present =
                 soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
            action_not_present =
                 soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
            if ((action_not_present == 0x1) || (action_present == 0x1)) {
                /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
                 * index of the entry getting replaced is valid. Save
                 * the old tpid index to be deleted later.
                 */
                old_tpid_idx =
                      soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG__SD_TAG_TPID_INDEXf);
            }
        }

        if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_TAGGED) {
            egr_nh_info.entry_type = _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW; /* SD_TAG_ACTIONS */
            /* Configure SD_TAG setting */
            rv = _bcm_tr3_l2gre_sd_tag_set(unit, NULL, l2gre_port, &egr_nh_info, NULL, &tpid_index);
            _BCM_L2GRE_CLEANUP(rv);

           /* Configure EGR Next Hop entry -- SD_TAG view */
           rv = _bcm_tr3_l2gre_nexthop_entry_modify(unit, vp_nh_ecmp_index, drop,
                             &egr_nh_info, _BCM_L2GRE_EGR_NEXT_HOP_SDTAG_VIEW);
           _BCM_L2GRE_CLEANUP(rv);
        }
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_MULTICAST)) {
            /* Program DVP entry  with ECMP / Next Hop Index */
            rv = _bcm_tr3_l2gre_ingress_dvp_set(unit, vp, mpath_flag, vp_nh_ecmp_index, l2gre_port);
            _BCM_L2GRE_CLEANUP(rv);
        }
    }

    /* Delete old TPID, Nexthop indexes */
    if (old_tpid_idx != -1) {
        (void)_bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx);
    }
    return rv;

cleanup:
    if (tpid_index != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }
    if (vp_nh_ecmp_index != -1) {
        /* Decrement reference count */
        (void) bcm_xgs3_get_ref_count_from_nhi(
            unit, mpath_flag, &ref_count, vp_nh_ecmp_index);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_port_nh_delete
 * Purpose:
 *      Delete L2GRE Next Hop entry
 * Parameters:
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_port_nh_delete(int unit, bcm_vpn_t vpn, int vp)
{
    int rv=BCM_E_NONE;
    int  nh_ecmp_index=-1;
    ing_dvp_table_entry_t dvp;
    uint32  vp_type=0;
    uint32  flags=0;
    int  ref_count=0;
    int ecmp =-1;

    BCM_IF_ERROR_RETURN(
        READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    vp_type = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf);
    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (ecmp) {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
        flags = BCM_L3_MULTIPATH;
        /* Decrement reference count */
        BCM_IF_ERROR_RETURN(
             bcm_xgs3_get_ref_count_from_nhi(unit, flags, &ref_count, nh_ecmp_index));
    } else {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
        if(nh_ecmp_index != 0) {
             /* Decrement reference count */
             BCM_IF_ERROR_RETURN(
                  bcm_xgs3_get_ref_count_from_nhi(unit, flags, &ref_count, nh_ecmp_index));
        }

        if (ref_count == 1) {
              if (nh_ecmp_index) {
                  BCM_IF_ERROR_RETURN(
                     bcm_tr3_l2gre_port_egress_nexthop_reset
                                (unit, nh_ecmp_index, vp_type, vp, vpn));
              }
         }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_port_nh_get
 * Purpose:
 *      Get L2GRE Next Hop entry
 * Parameters:
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_port_nh_get(int unit, bcm_vpn_t vpn,  int vp, bcm_l2gre_port_t *l2gre_port)
{
    egr_l3_next_hop_entry_t egr_nh;
    ing_dvp_table_entry_t dvp;
    int nh_ecmp_index=0;
    uint32 ecmp;
    int rv = BCM_E_NONE;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf) ==
                              _BCM_L2GRE_INGRESS_DEST_VP_TYPE) {
        /* Egress into L2GRE tunnel, find the tunnel_if */
        l2gre_port->flags |= BCM_L2GRE_PORT_EGRESS_TUNNEL;
        BCM_IF_ERROR_RETURN(
             READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
        ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
        if (ecmp) {
            nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
            l2gre_port->egress_if  =  nh_ecmp_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        } else {
            nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
            /* Extract next hop index from unipath egress object. */
            l2gre_port->egress_if  =  nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
        }

        rv = bcm_tr3_l2gre_nexthop_get(unit, nh_ecmp_index, l2gre_port);
        BCM_IF_ERROR_RETURN(rv);

    } else if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                VP_TYPEf) == _BCM_L2GRE_DEST_VP_TYPE_ACCESS) {
        /* Egress into Access-side, find the tunnel_if */
        BCM_IF_ERROR_RETURN(
            READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
        /* Extract next hop index from unipath egress object. */
        l2gre_port->egress_if  =  nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit);

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_ecmp_index, &egr_nh));
         if (BCM_SUCCESS(rv)) {
              (void) _bcm_tr3_l2gre_sd_tag_get( unit, NULL, l2gre_port, &egr_nh,
                         NULL, 0);
         }

    } else {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_tr3_l2gre_match_vpnid_entry_set
 * Purpose:
 *		Reset L2GRE Match VPNID Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  Vpnid
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_tr3_l2gre_match_vpnid_entry_set(int unit, bcm_l2gre_vpn_config_t *vpn_info)
{
    uint32 vpnid = 0;
    int rv = BCM_E_NONE;
    int vfi_index = -1;
    soc_mem_t mmem = MPLS_ENTRYm;
    uint32 ment[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    uint32 vxlate[SOC_MAX_MEM_WORDS];

    if (NULL == vpn_info) {
        return BCM_E_PARAM;
    }
    if (vpn_info->vpnid == 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_valid(unit, vpn_info->vpn));
    _BCM_L2GRE_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn_info->vpn);
    /* Delete old match vpnid entry */
    if (vpn_info->flags & BCM_L2GRE_VPN_REPLACE) {
        rv = _bcm_tr3_l2gre_egr_xlate_entry_get(unit, vfi_index, &mem, vxlate);
        if (BCM_E_NONE == rv) {
            vpnid = soc_mem_field32_get(unit, mem, vxlate, L2GRE_VFI__VPNIDf);
            BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_match_vpnid_entry_reset(unit, vpnid));
        } else if (BCM_E_NOT_FOUND != rv) {
            return rv;
        }
    }

    /* Configure Ingress VPNID */
    sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mmem, ment, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mmem, ment, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mmem, ment,
            DATA_TYPEf, _BCM_L2GRE_KEY_TYPE_VPNID_VFI);
    } else
#endif
    {
        soc_mem_field32_set(unit, mmem, ment, VALIDf, 1);
    }
    soc_mem_field32_set(unit, mmem, ment, L2GRE_VPNID__VPNIDf, vpn_info->vpnid);
    soc_mem_field32_set(unit, mmem, ment, L2GRE_VPNID__VFIf, vfi_index);
    soc_mem_field32_set(unit, mmem, ment, KEY_TYPEf, _BCM_L2GRE_KEY_TYPE_VPNID_VFI);
    BCM_IF_ERROR_RETURN(soc_mem_insert(unit, mmem, MEM_BLOCK_ALL, ment));

    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_tr3_mpls_match_label_entry_reset
 * Purpose:
 *		Reset L2GRE Match VPNID Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  Vpnid
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_tr3_l2gre_match_vpnid_entry_reset(int unit, uint32 vpnid)
{
    soc_mem_t mem = MPLS_ENTRYm;
    uint32 ment[SOC_MAX_MEM_WORDS];

    sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, ment, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, ment, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mem, ment,
            DATA_TYPEf, _BCM_L2GRE_KEY_TYPE_VPNID_VFI);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, ment, VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, ment, L2GRE_VPNID__VPNIDf, vpnid);
    soc_mem_field32_set(unit, mem, ment, KEY_TYPEf, _BCM_L2GRE_KEY_TYPE_VPNID_VFI);

    BCM_IF_ERROR_RETURN(soc_mem_delete(unit, mem, MEM_BLOCK_ALL, ment));

    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_tr3_mpls_match_tunnel_entry_set
 * Purpose:
 *		Set L2GRE Match Tunnel Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  mpls_entry
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr3_l2gre_match_tunnel_entry_set(int unit, int vp, bcm_l2gre_port_t *l2gre_port)
{
    int rv = BCM_E_UNAVAIL;
    int index;
    int tunnel_idx = -1;
    uint32 tunnel_sip;
    soc_mem_t mem = MPLS_ENTRYm;
    uint32 ment[SOC_MAX_MEM_WORDS] = {0};
    uint32 rment[SOC_MAX_MEM_WORDS] = {0};
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);

    if (!BCM_GPORT_IS_TUNNEL(l2gre_port->match_tunnel_id)) {
        return BCM_E_PARAM;
    }

    tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(l2gre_port->match_tunnel_id);
    tunnel_sip = l2gre_info->l2gre_tunnel_term[tunnel_idx].sip;
    l2gre_info->match_key[vp].match_tunnel_index = tunnel_idx;

    if (l2gre_port->flags & BCM_L2GRE_PORT_MULTICAST) {
          return BCM_E_NONE;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, ment, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, ment, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mem, ment,
            DATA_TYPEf, _BCM_L2GRE_KEY_TYPE_TUNNEL);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, ment, VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, ment, L2GRE_SIP__SIPf, tunnel_sip);
    soc_mem_field32_set(unit, mem, ment, L2GRE_SIP__SVPf, vp);
    soc_mem_field32_set(unit, mem, ment, KEY_TYPEf, _BCM_L2GRE_KEY_TYPE_TUNNEL);

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, ment, &rment, 0);
    if (rv == SOC_E_NONE) {
         BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, ment));
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
        BCM_IF_ERROR_RETURN(soc_mem_insert(unit, mem, MEM_BLOCK_ALL, ment));
    }
    l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_VPNID;

    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_tr3_mpls_match_tunnel_entry_reset
 * Purpose:
 *		Reet L2GRE Match Tunnel Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  mpls_entry
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr3_l2gre_match_tunnel_entry_reset(int unit, int vp)
{
    int rv = BCM_E_UNAVAIL;
    int index;
    int tunnel_idx = -1;
    soc_mem_t mem = MPLS_ENTRYm;
    uint32 ment[SOC_MAX_MEM_WORDS] = {0};
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);

    tunnel_idx = l2gre_info->match_key[vp].match_tunnel_index;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, ment, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, ment, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mem, ment,
            DATA_TYPEf, _BCM_L2GRE_KEY_TYPE_TUNNEL);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, ment, VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, ment, L2GRE_SIP__SIPf,
            l2gre_info->l2gre_tunnel_term[tunnel_idx].sip);
    soc_mem_field32_set(unit, mem, ment, KEY_TYPEf, _BCM_L2GRE_KEY_TYPE_TUNNEL);

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, ment, ment, 0);
    if (BCM_E_NONE == rv) {
        sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, ment));
    } else if (BCM_E_NOT_FOUND == rv) {
        return BCM_E_NONE;
    } else {
        return rv;
    }

    l2gre_info->l2gre_tunnel_term[tunnel_idx].sip = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_tr3_mpls_match_vlan_extd_entry_update
 * Purpose:
 *		Update L2GRE Match Vlan_xlate_extd  Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  vlan_xlate_extd_entry_t
 *  OUT :  vlan_xlate_extd_entry_t

 */

STATIC int
_bcm_tr3_l2gre_match_vxlate_extd_entry_update(int unit, vlan_xlate_extd_entry_t *vent,
                                                                     vlan_xlate_extd_entry_t *return_ent)
{
    uint32  vp, key_type, value;

    /* Check if Key_Type identical */
    key_type = soc_VLAN_XLATE_EXTDm_field32_get (unit, vent, KEY_TYPE_0f);
    value = soc_VLAN_XLATE_EXTDm_field32_get (unit, return_ent, KEY_TYPE_0f);
    if (key_type != value) {
         return BCM_E_PARAM;
    }

    /* Retain original Keys -- Update data only */
    soc_VLAN_XLATE_EXTDm_field32_set(unit, return_ent, XLATE__MPLS_ACTIONf, 0x1);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, return_ent, XLATE__DISABLE_VLAN_CHECKSf, 1);
    vp = soc_VLAN_XLATE_EXTDm_field32_get (unit, vent, XLATE__SOURCE_VPf);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, return_ent, XLATE__SOURCE_VPf, vp);

   return BCM_E_NONE;
}


/*
 * Function:
 *        _bcm_tr3_l2gre_match_vxlate_extd_entry_set
 * Purpose:
 *       Set L2GRE Match Vxlate Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  l2gre_port
 *  IN :  vlan_xlate_extd_entry_t
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_match_vxlate_extd_entry_set(int unit, bcm_l2gre_port_t *l2gre_port, vlan_xlate_extd_entry_t *vent)
{
    vlan_xlate_extd_entry_t return_vent;
    int rv = BCM_E_UNAVAIL;
    int index;

    rv = soc_mem_search(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &index,
                        vent, &return_vent, 0);

    if (rv == SOC_E_NONE) {
         BCM_IF_ERROR_RETURN(
              _bcm_tr3_l2gre_match_vxlate_extd_entry_update (unit, vent, &return_vent));
         rv = soc_mem_write(unit, VLAN_XLATE_EXTDm,
                                           MEM_BLOCK_ALL, index,
                                           &return_vent);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
         if (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE) {
             return BCM_E_NOT_FOUND;
         } else {
              rv = soc_mem_insert(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL, vent);
         }
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_match_clear
 * Purpose:
 *      Clear L2GRE Match Software State
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      void
 */

void
bcm_tr3_l2gre_match_clear (int unit, int vp)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);

    l2gre_info->match_key[vp].flags = 0;
    l2gre_info->match_key[vp].match_vlan = 0;
    l2gre_info->match_key[vp].match_inner_vlan = 0;
    l2gre_info->match_key[vp].trunk_id = -1;
    l2gre_info->match_key[vp].port = 0;
    l2gre_info->match_key[vp].modid = -1;
    l2gre_info->match_key[vp].match_tunnel_index= 0;
    l2gre_info->match_key[vp].match_vpnid = 0;

    return;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_trunk_table_set
 * Purpose:
 *      Configure L2GRE Trunk port entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      port   - (IN) Trunk member port
 *      tgid - (IN) Trunk group Id
 *      vp   - (IN) Virtual port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_trunk_table_set(int unit, bcm_port_t port, bcm_trunk_t tgid, int vp)
{
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/
    bcm_module_t my_modid;
    int    src_trk_idx = -1;
    int    port_type;
    int rv = BCM_E_NONE;

    if (tgid != BCM_TRUNK_INVALID) {
        port_type = 1; /* TRUNK_PORT_TYPE */
    } else {
        return BCM_E_PARAM;
    }

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                port, &src_trk_idx));

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);

    /* Read source trunk map table. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm,
                MEM_BLOCK_ANY, src_trk_idx, &trunk_map_entry));


    /* Set trunk group id. */
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
                SRC_TGIDf, tgid);
    } else {
        soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
                TGIDf, tgid);
    }

    /* Set port is part of Trunk group */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
            PORT_TYPEf, port_type);

    /* Enable SVP Mode */
    if (SOC_MEM_FIELD_VALID(unit,SOURCE_TRUNK_MAP_TABLEm,SVP_VALIDf)) {
        soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
               SVP_VALIDf, 0x1);
    }

    /* Set SVP */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
            SOURCE_VPf, vp);

    /* Write entry to hw. */
    rv = soc_mem_write(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ALL,
            src_trk_idx, &trunk_map_entry);

    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);

    return rv;
}


/*
 * Function:
 *      _bcm_tr3_l2gre_trunk_table_reset
 * Purpose:
 *      Reset L2GRE Trunk port entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      port   - (IN) Trunk member port
 *      tgid   - (IN) Trunk group Id
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_trunk_table_reset(int unit, bcm_port_t port,
                                 bcm_trunk_t tgid,
                                 bcm_module_t my_modid)
{
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/
    int    src_trk_idx = -1;
    int rv = BCM_E_NONE;
    int vp;
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                port, &src_trk_idx));

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);

    /* Read source trunk map table. */
    rv = soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm,
                MEM_BLOCK_ANY, src_trk_idx, &trunk_map_entry);
    if (rv < 0) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    /* Get Source vp from SOURCE_TRUNK_MAP_TABLE table */
    vp = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit,
                                                 &trunk_map_entry, SOURCE_VPf);
    /*
     * If this trunk Id is not present in l2gre structure,
     * there is no need to reset.
     */
    if (l2gre_info->match_key[vp].trunk_id != tgid) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }
    /* Disable SVP Mode */
    if (SOC_MEM_FIELD_VALID(unit,SOURCE_TRUNK_MAP_TABLEm,SVP_VALIDf)) {
       soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
            SVP_VALIDf, 0);
    }

    /* Set SVP */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
            SOURCE_VPf, 0);

    /* Write entry to hw. */
    rv = soc_mem_write(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ALL,
            src_trk_idx, &trunk_map_entry);

    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);

    return rv;
}
/*
 * Function:
 *      bcm_tr3_l2gre_match_trunk_add
 * Purpose:
 *      Assign SVP of  L2GRE Trunk port
 * Parameters:
 *      unit    - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
 bcm_tr3_l2gre_match_trunk_add(int unit, bcm_trunk_t tgid, int vp)
{
     int port_idx=0;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int rv = BCM_E_NONE;
    bcm_module_t my_modid;

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                SOC_MAX_NUM_PORTS, local_port_out, &local_member_count));
    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        rv = _bcm_tr3_l2gre_trunk_table_set(unit, local_port_out[port_idx], tgid, vp);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
        rv = _bcm_esw_port_tab_set(
                 unit, local_port_out[port_idx],
                 _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0x1); /* L2_SVP */
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
    }

    return BCM_E_NONE;

 trunk_cleanup:
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    for (;port_idx >= 0; port_idx--) {
        (void)_bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                 _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0x0);
        (void) _bcm_tr3_l2gre_trunk_table_reset(unit,
                                                local_port_out[port_idx], tgid,
                                                my_modid);
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_match_trunk_delete
 * Purpose:
 *      Remove SVP of L2GRE Trunk port
 * Parameters:
 *      unit    - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_match_trunk_delete(int unit, bcm_trunk_t tgid, int vp)
{
    int port_idx=0;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int rv = BCM_E_NONE;
    bcm_module_t my_modid;

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                SOC_MAX_NUM_PORTS, local_port_out, &local_member_count));
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        rv = _bcm_tr3_l2gre_trunk_table_reset(unit,
                                              local_port_out[port_idx], tgid,
                                              my_modid);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
        rv = _bcm_esw_port_tab_set(
                 unit, local_port_out[port_idx],
                 _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0x0);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
    }

    return BCM_E_NONE;

 trunk_cleanup:
    for (;port_idx >= 0; port_idx--) {
        (void)_bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                 _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0x1);/* L2_SVP */
        (void) _bcm_tr3_l2gre_trunk_table_set(unit, local_port_out[port_idx], tgid, vp);
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_match_add
 * Purpose:
 *      Assign match criteria of an L2GRE port
 * Parameters:
 *      unit    - (IN) Device Number
 *      l2gre_port - (IN) l2gre port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_match_add(int unit, bcm_l2gre_port_t *l2gre_port, int vp, bcm_vpn_t vpn)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);
    int rv = BCM_E_NONE;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    vlan_xlate_extd_entry_t vent;
    int    mod_id_idx=0; /* Module Id */
    int    src_trk_idx=0;  /*Source Trunk table index.*/
    int    gport_id=-1;

    rv = _bcm_tr3_l2gre_port_resolve(unit, l2gre_port->l2gre_port_id, -1, &mod_out,
                    &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&vent, 0, sizeof(vlan_xlate_extd_entry_t));

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mod_id_idx));

    if (l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_PORT_VLAN ) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);
        if (!BCM_VLAN_VALID(l2gre_port->match_vlan)) {
             return BCM_E_PARAM;
        }
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                        TR3_VLXLT_X_HASH_KEY_TYPE_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                        TR3_VLXLT_X_HASH_KEY_TYPE_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OVIDf,
                                        l2gre_port->match_vlan);
        l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_VLAN;
        l2gre_info->match_key[vp].match_vlan = l2gre_port->match_vlan;

        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
            l2gre_info->match_key[vp].trunk_id = trunk_id;
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
            l2gre_info->match_key[vp].port = port_out;
            l2gre_info->match_key[vp].modid = mod_out;
        }
        rv = _bcm_tr3_l2gre_match_vxlate_extd_entry_set(unit, l2gre_port, &vent);
        BCM_IF_ERROR_RETURN(rv);
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
              bcm_tr3_l2gre_port_match_count_adjust(unit, vp, 1);
        }
    } else if (l2gre_port->criteria ==
                            BCM_L2GRE_PORT_MATCH_PORT_INNER_VLAN) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);
        if (!BCM_VLAN_VALID(l2gre_port->match_inner_vlan)) {
            return BCM_E_PARAM;
        }
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__IVIDf,
                                    l2gre_port->match_inner_vlan);
        l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN;
        l2gre_info->match_key[vp].match_inner_vlan = l2gre_port->match_inner_vlan;

        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
            l2gre_info->match_key[vp].trunk_id = trunk_id;
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
            l2gre_info->match_key[vp].port = port_out;
            l2gre_info->match_key[vp].modid = mod_out;
        }
        rv = _bcm_tr3_l2gre_match_vxlate_extd_entry_set(unit, l2gre_port, &vent);
        BCM_IF_ERROR_RETURN(rv);
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
             bcm_tr3_l2gre_port_match_count_adjust(unit, vp, 1);
        }
    } else if (l2gre_port->criteria ==
                            BCM_L2GRE_PORT_MATCH_PORT_VLAN_STACKED) {

         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);
         if (!BCM_VLAN_VALID(l2gre_port->match_vlan) ||
                !BCM_VLAN_VALID(l2gre_port->match_inner_vlan)) {
              return BCM_E_PARAM;
         }
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                   TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                   TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OVIDf,
                   l2gre_port->match_vlan);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__IVIDf,
                   l2gre_port->match_inner_vlan);
         l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED;
         l2gre_info->match_key[vp].match_vlan = l2gre_port->match_vlan;
         l2gre_info->match_key[vp].match_inner_vlan = l2gre_port->match_inner_vlan;
         if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
            l2gre_info->match_key[vp].trunk_id = trunk_id;
         } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
            l2gre_info->match_key[vp].port = port_out;
            l2gre_info->match_key[vp].modid = mod_out;
         }
         rv = _bcm_tr3_l2gre_match_vxlate_extd_entry_set(unit, l2gre_port, &vent);
         BCM_IF_ERROR_RETURN(rv);
         if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
             bcm_tr3_l2gre_port_match_count_adjust(unit, vp, 1);
         }
    } else if (l2gre_port->criteria ==
                                            BCM_L2GRE_PORT_MATCH_VLAN_PRI) {

              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                                 TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                                 TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI);
              /* match_vlan : Bits 12-15 contains VLAN_PRI + CFI, vlan=BCM_E_NONE */
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, OTAGf,
                                                 l2gre_port->match_vlan);
              l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI;
              l2gre_info->match_key[vp].match_vlan = l2gre_port->match_vlan;

              if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
                   soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
                   soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
                        l2gre_info->match_key[vp].trunk_id = trunk_id;
              } else {
                   soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
                   soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
                   l2gre_info->match_key[vp].port = port_out;
                   l2gre_info->match_key[vp].modid = mod_out;
              }
              rv = _bcm_tr3_l2gre_match_vxlate_extd_entry_set(unit, l2gre_port, &vent);
              BCM_IF_ERROR_RETURN(rv);
              if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
                   bcm_tr3_l2gre_port_match_count_adjust(unit, vp, 1);
              }
    }else if (l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_PORT) {
        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
             rv = bcm_tr3_l2gre_match_trunk_add(unit, trunk_id, vp);
             if (rv >= 0) {
                   l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_TRUNK;
                   l2gre_info->match_key[vp].trunk_id = trunk_id;
             }
             BCM_IF_ERROR_RETURN(rv);
        } else {
            int is_local;

            BCM_IF_ERROR_RETURN
                ( _bcm_esw_modid_is_local(unit, mod_out, &is_local));

            /* Get index to source trunk map table */
            BCM_IF_ERROR_RETURN(
                   _bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                     port_out, &src_trk_idx));

            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        src_trk_idx, SOURCE_VPf, vp);
            BCM_IF_ERROR_RETURN(rv);

            if (is_local) {
                rv = soc_mem_field32_modify(unit, PORT_TABm, port_out,
                                        PORT_OPERATIONf, 0x1); /* L2_SVP */
                BCM_IF_ERROR_RETURN(rv);

                /* Set TAG_ACTION_PROFILE_PTR */
                rv = bcm_tr3_l2gre_port_untagged_profile_set(unit, port_out);
                BCM_IF_ERROR_RETURN(rv);
            }

            l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_PORT;
            l2gre_info->match_key[vp].index = src_trk_idx;
        }
    }else if (l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_VPNID) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_l2gre_match_tunnel_entry_set(unit, vp, l2gre_port));
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_match_delete
 * Purpose:
 *      Delete match criteria of an L2GRE port
 * Parameters:
 *      unit    - (IN) Device Number
 *      l2gre_port - (IN) L2GRE port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_match_delete(int unit,  int vp)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);
    int rv=BCM_E_NONE;
    vlan_xlate_extd_entry_t vent;
    bcm_trunk_t trunk_id;
    int    src_trk_idx=0;   /*Source Trunk table index.*/
    int    mod_id_idx=0;   /* Module_Id */
    int port=0;

    sal_memset(&vent, 0, sizeof(vlan_xlate_extd_entry_t));

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mod_id_idx));

    if  (l2gre_info->match_key[vp].flags == _BCM_L2GRE_PORT_MATCH_TYPE_VLAN) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OVIDf,
                                    l2gre_info->match_key[vp].match_vlan);
        if (l2gre_info->match_key[vp].modid != -1) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf,
                                        l2gre_info->match_key[vp].modid);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf,
                                        l2gre_info->match_key[vp].port);
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf,
                                        l2gre_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
         bcm_tr3_l2gre_port_match_count_adjust(unit, vp, -1);

    } else if  (l2gre_info->match_key[vp].flags ==
                     _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__IVIDf,
                                    l2gre_info->match_key[vp].match_inner_vlan);
        if (l2gre_info->match_key[vp].modid != -1) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf,
                                        l2gre_info->match_key[vp].modid);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf,
                                        l2gre_info->match_key[vp].port);
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf,
                                        l2gre_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
        bcm_tr3_l2gre_port_match_count_adjust(unit, vp, -1);
    }else if (l2gre_info->match_key[vp].flags ==
                    _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OVIDf,
                                    l2gre_info->match_key[vp].match_vlan);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__IVIDf,
                              l2gre_info->match_key[vp].match_inner_vlan);
        if (l2gre_info->match_key[vp].modid != -1) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf,
                                        l2gre_info->match_key[vp].modid);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf,
                                        l2gre_info->match_key[vp].port);
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf,
                                        l2gre_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
        bcm_tr3_l2gre_port_match_count_adjust(unit, vp, -1);

    } else if	(l2gre_info->match_key[vp].flags ==
                                               _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                                      TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                                      TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OTAGf,
                                                       l2gre_info->match_key[vp].match_vlan);
        if (l2gre_info->match_key[vp].modid != -1) {
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf,
                                                      l2gre_info->match_key[vp].modid);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf,
                                                      l2gre_info->match_key[vp].port);
        } else {
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf,
                                                      l2gre_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
        bcm_tr3_l2gre_port_match_count_adjust(unit, vp, -1);
    }else if  (l2gre_info->match_key[vp].flags ==
                    _BCM_L2GRE_PORT_MATCH_TYPE_PORT) {
         int is_local;

         src_trk_idx = l2gre_info->match_key[vp].index;
         rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                  src_trk_idx, SOURCE_VPf, 0);
         BCM_IF_ERROR_RETURN(rv);

         BCM_IF_ERROR_RETURN(_bcm_esw_src_modid_port_get( unit, src_trk_idx,
                                                          &mod_id_idx, &port));
         BCM_IF_ERROR_RETURN
             ( _bcm_esw_modid_is_local(unit, mod_id_idx, &is_local));

         if (is_local) {
             /* Convert system ports to physical ports */
             if (soc_feature(unit, soc_feature_sysport_remap)) {
                 BCM_XLATE_SYSPORT_S2P(unit, &port);
             }

             rv = soc_mem_field32_modify(unit, PORT_TABm, port,
                                           PORT_OPERATIONf, 0x0); /* NORMAL */
             BCM_IF_ERROR_RETURN(rv);

             /* Reset TAG_ACTION_PROFILE_PTR */
             rv = bcm_tr3_l2gre_port_untagged_profile_reset(unit, port);
             BCM_IF_ERROR_RETURN(rv);
         }
         (void) bcm_tr3_l2gre_match_clear (unit, vp);
    } else if  (l2gre_info->match_key[vp].flags ==
                  _BCM_L2GRE_PORT_MATCH_TYPE_TRUNK) {
         trunk_id = l2gre_info->match_key[vp].trunk_id;
         rv = bcm_tr3_l2gre_match_trunk_delete(unit, trunk_id, vp);
         BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
    } else if (l2gre_info->match_key[vp].flags ==
                  _BCM_L2GRE_PORT_MATCH_TYPE_VPNID) {
        rv = bcm_tr3_l2gre_match_tunnel_entry_reset(unit, vp);
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_match_get
 * Purpose:
 *      Obtain match information of an L2GRE port
 * Parameters:
 *      unit    - (IN) Device Number
 *      l2gre_port - (OUT) L2GRE port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_match_get(int unit, bcm_l2gre_port_t *l2gre_port, int vp)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);
    int rv = BCM_E_NONE;
    bcm_module_t mod_in=0, mod_out;
    bcm_port_t port_in=0, port_out;
    bcm_trunk_t trunk_id=0;
    int    src_trk_idx=0;    /*Source Trunk table index.*/
    int    tunnel_idx = -1;
    int    mode_in=0;   /* Module_Id */

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mode_in));

    if  (l2gre_info->match_key[vp].flags &  _BCM_L2GRE_PORT_MATCH_TYPE_VLAN) {

         l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT_VLAN;
         l2gre_port->match_vlan = l2gre_info->match_key[vp].match_vlan;

        if (l2gre_info->match_key[vp].trunk_id != -1) {
             trunk_id = l2gre_info->match_key[vp].trunk_id;
             BCM_GPORT_TRUNK_SET(l2gre_port->match_port, trunk_id);
        } else {
             port_in = l2gre_info->match_key[vp].port;
             mod_in = l2gre_info->match_key[vp].modid;
             rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
             BCM_GPORT_MODPORT_SET(l2gre_port->match_port, mod_out, port_out);
        }
    } else if (l2gre_info->match_key[vp].flags &
                 _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN) {
         l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT_INNER_VLAN;
         l2gre_port->match_inner_vlan = l2gre_info->match_key[vp].match_inner_vlan;

        if (l2gre_info->match_key[vp].trunk_id != -1) {
             trunk_id = l2gre_info->match_key[vp].trunk_id;
             BCM_GPORT_TRUNK_SET(l2gre_port->match_port, trunk_id);
        } else {
             port_in = l2gre_info->match_key[vp].port;
             mod_in = l2gre_info->match_key[vp].modid;
             rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
             BCM_GPORT_MODPORT_SET(l2gre_port->match_port, mod_out, port_out);
        }
    }else if (l2gre_info->match_key[vp].flags &
                    _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED) {

         l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT_VLAN_STACKED;
         l2gre_port->match_vlan = l2gre_info->match_key[vp].match_vlan;
         l2gre_port->match_inner_vlan = l2gre_info->match_key[vp].match_inner_vlan;

         if (l2gre_info->match_key[vp].trunk_id != -1) {
              trunk_id = l2gre_info->match_key[vp].trunk_id;
              BCM_GPORT_TRUNK_SET(l2gre_port->match_port, trunk_id);
         } else {
              port_in = l2gre_info->match_key[vp].port;
              mod_in = l2gre_info->match_key[vp].modid;
              rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                             mod_in, port_in, &mod_out, &port_out);
              BCM_GPORT_MODPORT_SET(l2gre_port->match_port, mod_out, port_out);
         }
    } else if  (l2gre_info->match_key[vp].flags &  _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI) {

         l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_VLAN_PRI;
         l2gre_port->match_vlan = l2gre_info->match_key[vp].match_vlan;

        if (l2gre_info->match_key[vp].trunk_id != -1) {
             trunk_id = l2gre_info->match_key[vp].trunk_id;
             BCM_GPORT_TRUNK_SET(l2gre_port->match_port, trunk_id);
        } else {
             port_in = l2gre_info->match_key[vp].port;
             mod_in = l2gre_info->match_key[vp].modid;
             rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
             BCM_GPORT_MODPORT_SET(l2gre_port->match_port, mod_out, port_out);
        }
    } else if (l2gre_info->match_key[vp].flags &
                   _BCM_L2GRE_PORT_MATCH_TYPE_PORT) {

         src_trk_idx = l2gre_info->match_key[vp].index;
         BCM_IF_ERROR_RETURN(_bcm_esw_src_modid_port_get( unit, src_trk_idx,
                                                          &mod_in, &port_in));

         l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT;
         rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                               mod_in, port_in, &mod_out, &port_out);
         BCM_GPORT_MODPORT_SET(l2gre_port->match_port, mod_out, port_out);
    }else if (l2gre_info->match_key[vp].flags &
                  _BCM_L2GRE_PORT_MATCH_TYPE_TRUNK) {

         trunk_id = l2gre_info->match_key[vp].trunk_id;
         l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT;
         BCM_GPORT_TRUNK_SET(l2gre_port->match_port, trunk_id);
    } else if ((l2gre_info->match_key[vp].flags &
                          _BCM_L2GRE_PORT_MATCH_TYPE_VPNID)) {
        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_VPNID;

        tunnel_idx = l2gre_info->match_key[vp].match_tunnel_index;
        BCM_GPORT_TUNNEL_ID_SET(l2gre_port->match_tunnel_id, tunnel_idx);

    } else {
        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_NONE;
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_eline_vp_map_set
 * Purpose:
 *      Set L2GRE ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2gre_eline_vp_map_set(int unit, int vfi_index, int vp1, int vp2)
{
    vfi_entry_t vfi_entry;
    int rv=BCM_E_NONE;
    int num_vp;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
         if ((vp1 > 0) && (vp1 < num_vp)) {
              soc_VFIm_field32_set(unit, &vfi_entry, VP_0f, vp1);
         } else {
              return BCM_E_PARAM;
         }
         if ((vp2 > 0) && (vp2 < num_vp)) {
              soc_VFIm_field32_set(unit, &vfi_entry, VP_1f, vp2);
         } else {
              return BCM_E_PARAM;
         }
    } else {
         /* ELAN */
         return BCM_E_PARAM;
    }

    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_eline_vp_map_get
 * Purpose:
 *      Get L2GRE ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_eline_vp_map_get(int unit, int vfi_index, int *vp1, int *vp2)
{
    vfi_entry_t vfi_entry;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        *vp1 = soc_VFIm_field32_get(unit, &vfi_entry, VP_0f);
        *vp2 = soc_VFIm_field32_get(unit, &vfi_entry, VP_1f);
         return BCM_E_NONE;
    } else {
         return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      _bcm_tr3_l2gre_eline_vp_map_clear
 * Purpose:
 *      Clear L2GRE ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_eline_vp_map_clear(int unit, int vfi_index, int vp1, int vp2)
{
    vfi_entry_t vfi_entry;
    int rv=BCM_E_NONE;
    int num_vp;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
         if ((vp1 > 0) && (vp1 < num_vp)) {
             soc_VFIm_field32_set(unit, &vfi_entry, VP_0f, 0);
         } else if ((vp2 > 0) && (vp2 < num_vp)) {
             soc_VFIm_field32_set(unit, &vfi_entry, VP_1f, 0);
         } else {
              return BCM_E_PARAM;
         }
    } else {
         /* ELAN */
         return BCM_E_PARAM;
    }
    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_eline_vp_configure
 * Purpose:
 *      Configure L2GRE ELINE virtual port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_l2gre_eline_vp_configure (int unit, int vfi_index, int active_vp,
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port)
{
    int rv = BCM_E_NONE;
    int network_group=0;

    /* Set SOURCE_VP */
    soc_SOURCE_VPm_field32_set(unit, svp, CLASS_IDf,
                                                                l2gre_port->if_class);
    /* Default Split Horizon Group Id
     * 0 - For Access Port;1 - For Network Port*/
    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
            network_group = l2gre_port->network_group_id;
            rv = _bcm_validate_splithorizon_network_group(unit,
                    l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
            BCM_IF_ERROR_RETURN(rv);
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                        network_group);
        } else {
            network_group = l2gre_port->network_group_id;
            rv = _bcm_validate_splithorizon_network_group(unit,
                    l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
            BCM_IF_ERROR_RETURN(rv);
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                        network_group);
        }
    } else {
        soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf,
                (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) ? 1 : 0);
    }
    if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_TAGGED) {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 1);
         soc_SOURCE_VPm_field32_set(unit, svp, TPID_ENABLEf, tpid_enable);
    } else {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 0);
    }

     soc_SOURCE_VPm_field32_set(unit, svp, DISABLE_VLAN_CHECKSf, 1);
     soc_SOURCE_VPm_field32_set(unit, svp,
                           ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_VFI); /* VFI Type */
     soc_SOURCE_VPm_field32_set(unit, svp, VFIf, vfi_index);

     BCM_IF_ERROR_RETURN
           (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, svp));

    return rv;
}


/*
 * Function:
 *      _bcm_tr3_l2gre_elan_vp_configure
 * Purpose:
 *      Configure L2GRE ELINE virtual port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_l2gre_elan_vp_configure (int unit, int vfi_index, int active_vp,
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port)
{
    int rv = BCM_E_NONE;
    int network_group=0;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;

    /* Set SOURCE_VP */
    soc_SOURCE_VPm_field32_set(unit, svp, CLASS_IDf,
                         l2gre_port->if_class);

    /* Default Split Horizon Group Id
     * 0 - For Access Port;1 - For Network Port*/
    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
            network_group = l2gre_port->network_group_id;
            rv = _bcm_validate_splithorizon_network_group(unit,
                    l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
            BCM_IF_ERROR_RETURN(rv);
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                        network_group);
        } else {
            network_group = l2gre_port->network_group_id;
            rv = _bcm_validate_splithorizon_network_group(unit,
                    l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
            BCM_IF_ERROR_RETURN(rv);
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                        network_group);
        }
    } else {
        soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf,
                (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) ? 1 : 0);
    }

    if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_TAGGED) {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 1);
         soc_SOURCE_VPm_field32_set(unit, svp, TPID_ENABLEf, tpid_enable);
    } else {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 0);
    }
    if (vfi_index == _BCM_L2GRE_VFI_INVALID) {
        soc_SOURCE_VPm_field32_set(unit, svp,
                                   ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_INVALID); /* INVALID */
    } else {
        /* Initialize VP parameters */
        soc_SOURCE_VPm_field32_set(unit, svp,
                                   ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_VFI); /* VFI Type */
    }
    soc_SOURCE_VPm_field32_set(unit, svp, VFIf, vfi_index);
    rv = _bcm_vp_default_cml_mode_get (unit,
                       &cml_default_enable, &cml_default_new,
                       &cml_default_move);
    if (rv < 0) {
         return rv;
    }
    if (cml_default_enable) {
        /* Set the CML to default values */
        soc_SOURCE_VPm_field32_set(unit, svp, CML_FLAGS_NEWf, cml_default_new);
        soc_SOURCE_VPm_field32_set(unit, svp, CML_FLAGS_MOVEf, cml_default_move);
    } else {
        /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
        soc_SOURCE_VPm_field32_set(unit, svp, CML_FLAGS_NEWf, 0x8);
        soc_SOURCE_VPm_field32_set(unit, svp, CML_FLAGS_MOVEf, 0x8);
    }
    if (soc_mem_field_valid(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
          soc_SOURCE_VPm_field32_set(unit, svp, DISABLE_VLAN_CHECKSf, 0x1);
    }

    BCM_IF_ERROR_RETURN
           (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, svp));

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_access_niv_pe_set
 * Purpose:
 *      Set L2GRE Access virtual port which is of NIV/PE type
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp      -  (IN) Access Virtual Port
 *      vfi      -  (IN) Virtial forwarding instance
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_access_niv_pe_set (int unit, int vp, int vfi)
{
    source_vp_entry_t svp;
    int  rv = BCM_E_PARAM;

        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            return rv;
        }

        if (vfi == _BCM_L2GRE_VFI_INVALID) {
            soc_SOURCE_VPm_field32_set(unit, &svp,
                                       ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_INVALID);
        } else {
            /* Initialize VP parameters */
            soc_SOURCE_VPm_field32_set(unit, &svp,
                                       ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_VFI);
        }
        soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi);
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_access_niv_pe_reset
 * Purpose:
 *      Reset L2GRE Access virtual port which is of NIV/PE type
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp      -  (IN) Access Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_access_niv_pe_reset (int unit, int vp)
{
    source_vp_entry_t svp;
    int  rv = BCM_E_PARAM;

        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            return rv;
        }

        /* Initialize VP parameters */
        soc_SOURCE_VPm_field32_set(unit, &svp,
                                       ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_VLAN);
        soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, 0);
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_eline_port_add
 * Purpose:
 *      Add L2GRE ELINE port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      l2gre_port - (IN/OUT) L2GRE port information (OUT : l2gre_port_id)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_eline_port_add(int unit, bcm_vpn_t vpn, bcm_l2gre_port_t  *l2gre_port)
{
    int active_vp = 0; /* default VP */
    source_vp_entry_t svp1, svp2;
    uint8 vp_valid_flag = 0;
    int tpid_enable = 0, tpid_index=-1;
    int customer_drop=0;
    int  rv = BCM_E_PARAM;
    int num_vp=0;
    int vp1=0, vp2=0, vfi_index= -1;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeL2Gre;

        if ( vpn != BCM_L2GRE_VPN_INVALID) {
            _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELINE,  vpn);
             if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
                return BCM_E_NOT_FOUND;
             }
        } else {
             vfi_index = _BCM_L2GRE_VFI_INVALID;
        }

        num_vp = soc_mem_index_count(unit, SOURCE_VPm);

        if ( vpn != BCM_L2GRE_VPN_INVALID) {

             /* ---- Read in current table values for VP1 and VP2 ----- */
             (void) _bcm_tr3_l2gre_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);

            if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeL2Gre)) {
                BCM_IF_ERROR_RETURN (
                   READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp1, &svp1));
                if (soc_SOURCE_VPm_field32_get(unit, &svp1, ENTRY_TYPEf) == 0x1) {
                    vp_valid_flag |= 0x1;  /* -- VP1 Valid ----- */
                }
            }

            if (_bcm_vp_used_get(unit, vp2, _bcmVpTypeL2Gre)) {
                BCM_IF_ERROR_RETURN (
                   READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp2, &svp2));
                if (soc_SOURCE_VPm_field32_get(unit, &svp2, ENTRY_TYPEf) == 0x1) {
                    vp_valid_flag |= 0x2;        /* -- VP2 Valid ----- */
                }
            }

            if (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE) {
                active_vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
                if (active_vp == -1) {
                   return BCM_E_PARAM;
                }
                if (!_bcm_vp_used_get(unit, active_vp, _bcmVpTypeL2Gre)) {
                     return BCM_E_NOT_FOUND;
                }

                /* Decrement old-port's nexthop count */
                rv = _bcm_tr3_l2gre_port_nh_cnt_dec(unit, active_vp);
                if(rv < 0) {
                    return rv;
                }

                /* Decrement old-port's VP count */
                rv = _bcm_tr3_l2gre_port_cnt_update(unit, l2gre_port->l2gre_port_id,
                                                     active_vp, FALSE);
                if (rv < 0) {
                    return rv;
                }
            }
        }

        switch (vp_valid_flag) {

              case 0x0: /* No VP is valid */
                             if (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE) {
                                  return BCM_E_NOT_FOUND;
                             }

                             if (l2gre_port->flags & BCM_L2GRE_PORT_WITH_ID) {
                                vp1 = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
                                if (vp1 == -1) {
                                     return BCM_E_PARAM;
                                }

                                if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeL2Gre)) {
                                     return BCM_E_EXISTS;
                                }
                                if (vp1 >= num_vp) {
                                     return (BCM_E_BADID);
                                }
                                vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
                                BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp1, vp_info));

                             } else {

                                /* No entries are used, allocate a new VP index for VP1 */

                                vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
                                rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp1);
                                if (rv < 0) {
                                  return rv;
                                }
#if defined(BCM_TRIUMPH3_SUPPORT)
                                /*
                                 * EGR_MPLS_VC_AND_SWAP_LABEL_TABLE and EGR_DVP_ATTRIBUTE_1 are overlaid
                                 * tables on TR3. If there is conflict between thesw two tables, the entries for
                                 * EGR_DVP_ATTRIBUTE_1 own higher priority.
                                 */
                                if (soc_feature(unit, soc_feature_mpls) &&
                                    soc_feature(unit, soc_feature_vc_and_swap_table_overlaid)) {
                                    rv = _bcm_tr_mpls_vc_and_swap_table_entry_adjust(unit, vp1);
                                    if (rv < 0) {
                                        (void)_bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp1);
                                        return rv;
                                    }
                                }
#endif
                                /* Allocate a new VP index for VP2 */
                                vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
                                rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp2);
                                  if (rv < 0) {
                                     (void) _bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp1);
                                      return rv;
                                  }
                             }

                             active_vp = vp1;
                             vp_valid_flag = 1;
                             sal_memset(&svp1, 0, sizeof(source_vp_entry_t));
                             sal_memset(&svp2, 0, sizeof(source_vp_entry_t));
                             (void) _bcm_tr3_l2gre_eline_vp_map_set(unit, vfi_index, vp1, vp2);
                             break;


         case 0x1:    /* Only VP1 is valid */
                             if (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE) {
                                  if (active_vp != vp1) {
                                       return BCM_E_NOT_FOUND;
                                  }
                             } else if (l2gre_port->flags & BCM_L2GRE_PORT_WITH_ID) {
                                vp2 = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
                                if (vp2 == -1) {
                                     return BCM_E_PARAM;
                                }

                                if (_bcm_vp_used_get(unit, vp2, _bcmVpTypeL2Gre)) {
                                     return BCM_E_EXISTS;
                                }
                                if (vp2 >= num_vp) {
                                     return (BCM_E_BADID);
                                }
                                vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
                                BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp2, vp_info));

                             } else {
                                  active_vp = vp2;
                                  vp_valid_flag = 3;
                                  sal_memset(&svp2, 0, sizeof(source_vp_entry_t));
                             }

                             (void) _bcm_tr3_l2gre_eline_vp_map_set(unit, vfi_index, vp1, vp2);
                             break;



         case 0x2: /* Only VP2 is valid */
                             if (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE) {
                                  if (active_vp != vp2) {
                                       return BCM_E_NOT_FOUND;
                                  }
                             } else if (l2gre_port->flags & BCM_L2GRE_PORT_WITH_ID) {
                                vp1 = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
                                if (vp1 == -1) {
                                     return BCM_E_PARAM;
                                }

                                if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeL2Gre)) {
                                     return BCM_E_EXISTS;
                                }
                                if (vp1 >= num_vp) {
                                     return (BCM_E_BADID);
                                }
                                vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
                                BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp1, vp_info));

                             } else {
                                  active_vp = vp1;
                                  vp_valid_flag = 3;
                                   sal_memset(&svp1, 0, sizeof(source_vp_entry_t));
                             }

                             (void) _bcm_tr3_l2gre_eline_vp_map_set(unit, vfi_index, vp1, vp2);
                             break;



         case 0x3: /* VP1 and VP2 are valid */
                              if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
                                   return BCM_E_FULL;
                              }
                             break;
       }

       if (active_vp == -1) {
           return BCM_E_CONFIG;
       }

        /* Set L2GRE port ID */
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
            BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port->l2gre_port_id, active_vp);
        }

       /* ======== Configure Next-Hop Properties ========== */
       customer_drop = (l2gre_port->flags & BCM_L2GRE_PORT_DROP) ? 1 : 0;
       rv = _bcm_tr3_l2gre_port_nh_add(unit, l2gre_port, active_vp, vpn, customer_drop);
       if (rv < 0) {
            if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
                (void) _bcm_vp_free(unit, _bcmVfiTypeL2Gre, 1, active_vp);
            }
            return rv;
       }

       /* ======== Configure Service-Tag Properties =========== */
       if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_TAGGED) {
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                rv = _bcm_fb2_outer_tpid_entry_add(unit,l2gre_port->egress_service_tpid, &tpid_index);
            } else
#endif
            {
                rv = _bcm_fb2_outer_tpid_lkup(unit, l2gre_port->egress_service_tpid,
                                           &tpid_index);
            }
            if (rv < 0) {
                goto l2gre_cleanup;
            }
            tpid_enable = (1 << tpid_index);
       }

       /* ======== Configure SVP Properties ========== */
       if (active_vp == vp1) {
           rv  = _bcm_esw_l2gre_eline_vp_configure (unit, vfi_index, active_vp, &svp1,
                                                    tpid_enable, l2gre_port);
       } else if (active_vp == vp2) {
           rv  = _bcm_esw_l2gre_eline_vp_configure (unit, vfi_index, active_vp, &svp2,
                                                    tpid_enable, l2gre_port);
       }
       if (rv < 0) {
            goto l2gre_cleanup;
       }

        rv = _bcm_esw_l2gre_match_add(unit, l2gre_port, active_vp, vpn);
        if (rv < 0) {
            goto l2gre_cleanup;
        }

        /* Increment new-port's VP count */
        rv = _bcm_tr3_l2gre_port_cnt_update(unit, l2gre_port->l2gre_port_id, active_vp, TRUE);
        if (rv < 0) {
            goto l2gre_cleanup;
        }

        /* Set L2GRE port ID */
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
            BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port->l2gre_port_id, active_vp);
        }

l2gre_cleanup:
        if (rv < 0) {
            if (tpid_enable) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            }
            /* Free the VP's */
            if (vp_valid_flag) {
                if (vp1 != -1) {
                    (void) _bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp1);
                }
                if (vp2 != -1) {
                    (void) _bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp2);
                }
            }
        }
        return rv;
}

/*
 * Function:
 *     _bcm_tr3_l2gre_elan_port_add
 * Purpose:
 *      Add L2GRE ELAN port to a VPN
 * Parameters:
 *   unit - (IN) Device Number
 *   vpn - (IN) VPN instance ID
 *   l2gre_port - (IN/OUT) l2gre port information (OUT : l2gre_port_id)
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_elan_port_add(int unit, bcm_vpn_t vpn, bcm_l2gre_port_t *l2gre_port)
{
    int vp, num_vp, vfi=0;
    source_vp_entry_t svp;
    int tpid_enable = 0, tpid_index=-1;
    int drop, rv = BCM_E_PARAM;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeL2Gre;
    if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
        vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
    } else {
        vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
    }

    if (l2gre_port->criteria != BCM_L2GRE_PORT_MATCH_SHARE) {
        if ( vpn != BCM_L2GRE_VPN_INVALID) {
            _BCM_L2GRE_VPN_GET(vfi, _BCM_L2GRE_VPN_TYPE_ELAN,  vpn);
            if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            vfi = _BCM_L2GRE_VFI_INVALID;
        }
    } else {
        vp_info.flags |= _BCM_VP_INFO_SHARED_PORT;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

   /* ======== Allocate/Get Virtual_Port =============== */
    if (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE) {
        if (BCM_GPORT_IS_NIV_PORT(l2gre_port->l2gre_port_id) ||
            BCM_GPORT_IS_EXTENDER_PORT(l2gre_port->l2gre_port_id)) {
            if (BCM_GPORT_IS_NIV_PORT(l2gre_port->l2gre_port_id)) {
               vp = BCM_GPORT_NIV_PORT_ID_GET((l2gre_port->l2gre_port_id));
            } else if (BCM_GPORT_IS_EXTENDER_PORT(l2gre_port->l2gre_port_id)) {
               vp = BCM_GPORT_EXTENDER_PORT_ID_GET((l2gre_port->l2gre_port_id));
            }
            rv = _bcm_tr3_l2gre_access_niv_pe_set (unit, vp, vfi);
            if (BCM_SUCCESS(rv)) {
               /* Set L2GRE port ID */
               BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port->l2gre_port_id, vp);
               rv = _bcm_vp_used_set(unit, vp, vp_info);
            }
            return rv;
        }

        vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }

        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            return BCM_E_NOT_FOUND;
        }
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            return rv;
        }
        /* Decrement old-port's nexthop count */
        rv = _bcm_tr3_l2gre_port_nh_cnt_dec(unit, vp);
        if(rv < 0) {
            return rv;
        }
        /* Decrement old-port's VP count */
        rv = _bcm_tr3_l2gre_port_cnt_update(unit, l2gre_port->l2gre_port_id, vp, FALSE);
        if (rv < 0) {
            return rv;
        }

    } else if (l2gre_port->flags & BCM_L2GRE_PORT_WITH_ID ) {
        if (!BCM_GPORT_IS_L2GRE_PORT(l2gre_port->l2gre_port_id)) {
            return (BCM_E_BADID);
        }

        vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }
        if (vp >= num_vp) {
            return (BCM_E_BADID);
        }

        if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
            vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
        } else {
            vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
        }
        BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));

#if defined(BCM_TRIUMPH3_SUPPORT)
        /*
         * EGR_MPLS_VC_AND_SWAP_LABEL_TABLE and EGR_DVP_ATTRIBUTE_1 are overlaid
         * tables on TR3. If there is conflict between thesw two tables, the entries for
         * EGR_DVP_ATTRIBUTE_1 own higher priority.
         */
        if (soc_feature(unit, soc_feature_mpls) &&
            soc_feature(unit, soc_feature_vc_and_swap_table_overlaid)) {
            rv = _bcm_tr_mpls_vc_and_swap_table_entry_adjust(unit, vp);
            if (rv < 0) {
                (void)_bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp);
                return rv;
            }
        }
#endif

        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    } else {
        /* allocate a new VP index */
        rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp);
        if (rv < 0) {
           return rv;
        }
#if defined(BCM_TRIUMPH3_SUPPORT)
        /*
         * EGR_MPLS_VC_AND_SWAP_LABEL_TABLE and EGR_DVP_ATTRIBUTE_1 are overlaid
         * tables on TR3. If there is conflict between thesw two tables, the entries for
         * EGR_DVP_ATTRIBUTE_1 own higher priority.
         */
        if (soc_feature(unit, soc_feature_mpls) &&
            soc_feature(unit, soc_feature_vc_and_swap_table_overlaid)) {
            rv = _bcm_tr_mpls_vc_and_swap_table_entry_adjust(unit, vp);
            if (rv < 0) {
                (void)_bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp);
                return rv;
            }
        }
#endif
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));
    }

    /* ======== Configure Next-Hop/DVP Properties ========== */
    drop = (l2gre_port->flags & BCM_L2GRE_PORT_DROP) ? 1 : 0;
    rv = _bcm_tr3_l2gre_port_nh_add(unit, l2gre_port, vp, vpn, drop);
    if (rv < 0) {
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
            (void) _bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp);
        }
        return rv;
    }

    /* ======== Configure Service-Tag Properties =========== */
    if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_TAGGED) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = _bcm_fb2_outer_tpid_entry_add(unit,l2gre_port->egress_service_tpid, &tpid_index);
        } else
#endif
        {
            rv = _bcm_fb2_outer_tpid_lkup(unit, l2gre_port->egress_service_tpid,
                                       &tpid_index);
        }
        if (rv < 0) {
            goto l2gre_cleanup;
        }
        if (tpid_index != -1) {
            tpid_enable = (1 << tpid_index);
        }
    }

    /* ======== Configure SVP Properties ========== */
    rv  = _bcm_esw_l2gre_elan_vp_configure (unit, vfi, vp, &svp,
                               tpid_enable, l2gre_port);
    if (rv < 0) {
        goto l2gre_cleanup;
    }

    if (rv == BCM_E_NONE) {
        /* Set L2GRE port ID */
        BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port->l2gre_port_id, vp);
    }

    /* Increment new-port's VP count */
    rv = _bcm_tr3_l2gre_port_cnt_update(unit, l2gre_port->l2gre_port_id, vp, TRUE);
    if (rv < 0) {
        goto l2gre_cleanup;
    }

    /* ======== Configure match to VP Properties ========== */
    rv = _bcm_esw_l2gre_match_add(unit, l2gre_port, vp, vpn);
    if (rv < 0) {
        goto l2gre_cleanup;
    }

  l2gre_cleanup:
    if (rv < 0) {
        if (tpid_enable) {
            (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        }
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
            (void) _bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp);
            _bcm_tr3_l2gre_port_nh_delete(unit, vpn, vp);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2gre_eline_port_delete
 * Purpose:
 *      Delete L2GRE ELINE port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      l2gre_port_id - (IN) l2gre port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_l2gre_eline_port_delete(int unit, bcm_vpn_t vpn, int active_vp)
{
    source_vp_entry_t svp;
    int vp1=0, vp2=0, vfi_index= -1;
    int rv = BCM_E_UNAVAIL;
    bcm_gport_t  l2gre_port_id;

    if ( vpn != BCM_L2GRE_VPN_INVALID) {
         _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELINE,  vpn);
         if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
            return BCM_E_NOT_FOUND;
         }
    } else {
         vfi_index = _BCM_L2GRE_VFI_INVALID;
    }

    if (!_bcm_vp_used_get(unit, active_vp, _bcmVpTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }

    /* Set L2GRE port ID */
    BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port_id, active_vp);

    /* Delete the next-hop info */
    rv = _bcm_tr3_l2gre_port_nh_delete(unit, vpn, active_vp);
    if ( rv < 0 ) {
        if (rv != BCM_E_NOT_FOUND) {
            return rv;
        } else {
                rv = 0;
        }
    }

    rv = _bcm_esw_l2gre_match_delete(unit, active_vp);
    if ( rv < 0 ) {
        if (rv != BCM_E_NOT_FOUND) {
             return rv;
        } else {
             rv = BCM_E_NONE;
        }
    }

     /* ---- Read in current table values for VP1 and VP2 ----- */
     (void) _bcm_tr3_l2gre_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);

    /* If the other port is valid, point it to itself */
    if (active_vp == vp1) {
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_tr3_l2gre_eline_vp_map_clear (unit, vfi_index, active_vp, 0);
        }
    } else if (active_vp == vp2) {
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_tr3_l2gre_eline_vp_map_clear (unit, vfi_index, 0, active_vp);
        }
    }

    if (rv >= 0) {

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_mem_is_valid(unit, SOURCE_VP_2m)) {
           source_vp_2_entry_t svp_2_entry;
           int network_port_flag=0;
           _bcm_vp_info_t vp_info;

            /* Check for Network-Port */
           BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, active_vp, &vp_info));
           if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
               network_port_flag = TRUE;
           }

           if (!network_port_flag) {
                /* Clear SOURCE_VP_2 entry */
                sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
                BCM_IF_ERROR_RETURN
                    (WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, active_vp, &svp_2_entry));
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        /* Invalidate the VP being deleted */
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, &svp);
        if (BCM_SUCCESS(rv)) {
             rv = _bcm_tr3_l2gre_egress_dvp_reset(unit, active_vp);
             if (rv < 0) {
                 return rv;
             }
             rv = _bcm_tr3_l2gre_ingress_dvp_reset(unit, active_vp);
        }
    }

    if (rv < 0) {
        return rv;
    }

    /* Decrement port's VP count */
    rv = _bcm_tr3_l2gre_port_cnt_update(unit, l2gre_port_id, active_vp, FALSE);
    if (rv < 0) {
        return rv;
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVfiTypeL2Gre, 1, active_vp);
    return rv;

}

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
 * Function:
 *      bcm_tr3_share_vp_get
 * Purpose:
 *      Get shared VPs info per VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN that shared vp belong to
 *      vp_bitmap  - (OUT) shared vp bitmap
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_share_vp_get(int unit, bcm_vpn_t vpn, SHR_BITDCL *vp_bitmap)
{
    void *vent;
    soc_mem_t mem;
    int index_min = 0;
    int index_max = 0;
    uint8 *xlate_buf = NULL;
    int buf_size = 0;
    int i = 0;
    int rv = BCM_E_NONE;
    int vfi = -1;
    int vp = -1;

    if (vpn != BCM_L2GRE_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_valid(unit, vpn));
        _BCM_L2GRE_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        mem = VLAN_XLATEm;
    }
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    xlate_buf = soc_cm_salloc(unit, buf_size, "VLAN_XLATE buffer");
    if (NULL == xlate_buf) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, xlate_buf);
    if (rv < 0) {
        soc_cm_sfree(unit, xlate_buf);
        return rv;
    }

    for (i = index_min; i <= index_max; i++) {
        vent = soc_mem_table_idx_to_pointer(unit,
            mem, void *, xlate_buf, i);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {        
            if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_1f) != 7) {
                continue;
            }   
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem, vent, VALIDf) == 0) {
                continue;
            }
        }

        if (soc_mem_field32_get(unit, mem, vent, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }

        if (vfi != -1 && soc_mem_field32_get(unit, mem, vent, XLATE__VFIf) != vfi) {
            continue;
        }

        vp = soc_mem_field32_get(unit, mem, vent, XLATE__SOURCE_VPf);
        SHR_BITSET(vp_bitmap, vp);
    }

    (void)soc_cm_sfree(unit, xlate_buf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_share_vp_delete
 * Purpose:
 *      Delete match criteria of shared VPs
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN that shared vp belong to
 *      vp      - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_share_vp_delete(int unit, bcm_vpn_t vpn, int vp)
{
    void *vent;
    soc_mem_t mem;
    int index_min = 0;
    int index_max = 0;
    uint8 *xlate_buf = NULL;
    int buf_size = 0;
    int i = 0;
    int rv = BCM_E_NONE;
    int vfi = -1;
    int source_vp = -1;

    if (vpn != BCM_L2GRE_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_valid(unit, vpn));
        _BCM_L2GRE_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {   
        mem = VLAN_XLATEm;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    xlate_buf = soc_cm_salloc(unit, buf_size, "VLAN_XLATE buffer");
    if (NULL == xlate_buf) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, xlate_buf);
    if (rv < 0) {
        soc_cm_sfree(unit, xlate_buf);
        return rv;
    }

    for (i = index_min; i <= index_max; i++) {
        vent = soc_mem_table_idx_to_pointer(unit,
            mem, void *, xlate_buf, i);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_1f) != 7) {
                continue;
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem, vent, VALIDf) == 0) {
                continue;
            }    
        }
        if (soc_mem_field32_get(unit, mem, vent, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }

        if (vfi != -1 && soc_mem_field32_get(unit, mem, vent, XLATE__VFIf) != vfi) {
            continue;
        }

        source_vp = soc_mem_field32_get(unit, mem, vent, XLATE__SOURCE_VPf);
        if (vp != source_vp) {
            continue;
        }
        rv = soc_mem_delete_index(unit, mem, MEM_BLOCK_ALL, i);
        if (rv < 0) {
            soc_cm_sfree(unit, xlate_buf);
            return rv;
        }
        bcm_tr3_l2gre_port_match_count_adjust(unit, source_vp, -1);
    }

    (void)soc_cm_sfree(unit, xlate_buf);

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_tr3_l2gre_elan_port_delete
 * Purpose:
 *      Delete L2GRE ELAN port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      l2gre_port_id - (IN) l2gre port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_l2gre_elan_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    source_vp_entry_t svp;
    int rv = BCM_E_UNAVAIL;
    int vfi_index= -1;
    bcm_gport_t l2gre_port_id;
    int network_port_flag=0;
    bcm_l2gre_port_t  l2gre_port;
    _bcm_vp_info_t vp_info;

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }

    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        network_port_flag = TRUE;
    }

    if ( vpn != BCM_L2GRE_VPN_INVALID) {
         if (!network_port_flag) { /* Check VPN only for Access VP */
             _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELAN,  vpn);
             if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
                return BCM_E_NOT_FOUND;
             }
             if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) ||
                  _bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                   (void) _bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp);
                   rv = _bcm_tr3_l2gre_access_niv_pe_reset (unit, vp);
                   return rv;
             }
         }
    }

    /* Set L2GRE port ID */
    BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port_id, vp);
    bcm_l2gre_port_t_init(&l2gre_port);
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_port_get(unit, vpn, vp, &l2gre_port));

    /* Delete the next-hop info */
    rv = _bcm_tr3_l2gre_port_nh_delete(unit, vpn, vp);

    if ( rv < 0 ) {
        if (rv != BCM_E_NOT_FOUND) {
            return rv;
        } else {
                rv = 0;
        }
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if ((!network_port_flag) && (vp_info.flags & _BCM_VP_INFO_SHARED_PORT)) {
        if (L2GRE_INFO(unit)->match_key[vp].match_count > 0) {
            BCM_IF_ERROR_RETURN(_bcm_tr3_share_vp_delete(unit, vpn, vp));
        }
        if (vpn != BCM_L2GRE_VPN_INVALID) {
            return BCM_E_NONE;
        }

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
    } else
#endif
    {
        rv = _bcm_esw_l2gre_match_delete(unit, vp);
        if ( rv < 0 ) {
            if (rv != BCM_E_NOT_FOUND) {
                return rv;
            }
        }
    }

    /* Clear the SVP and DVP table entries */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    if (rv < 0) {
        return rv;
    }

    if (!network_port_flag) {
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_mem_is_valid(unit, SOURCE_VP_2m)) {
           source_vp_2_entry_t svp_2_entry;

                /* Clear SOURCE_VP_2 entry */
                sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
                BCM_IF_ERROR_RETURN
                    (WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry));
        }
#endif /* BCM_TRIDENT2_SUPPORT */
    }

    rv = _bcm_tr3_l2gre_egress_dvp_reset(unit, vp);
    if (rv < 0) {
        return rv;
    }

    rv = _bcm_tr3_l2gre_ingress_dvp_reset(unit, vp);
    if (rv < 0) {
        return rv;
    }

    /* Decrement port's VP count */
    rv = _bcm_tr3_l2gre_port_cnt_update(unit, l2gre_port_id, vp, FALSE);
    if (rv < 0) {
        return rv;
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVfiTypeL2Gre, 1, vp);
    return rv;
}


/*
 * Function:
 *      _bcm_tr3_l2gre_port_get
 * Purpose:
 *      Get L2GRE port information from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      l2gre_port_id - (IN) l2gre port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_l2gre_port_get(int unit, bcm_vpn_t vpn, int vp, bcm_l2gre_port_t  *l2gre_port)
{
    int i, rv = BCM_E_NONE;
    uint32 tpid_enable = 0;
    source_vp_entry_t svp;
    int split_horizon_port_flag=0;
    _bcm_vp_info_t vp_info;

    /* Initialize the structure */
    bcm_l2gre_port_t_init(l2gre_port);
    BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port->l2gre_port_id, vp);

    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        split_horizon_port_flag = TRUE;
    }

    if ( vpn != BCM_L2GRE_VPN_INVALID) {
         if (!split_horizon_port_flag) { /* Check VPN only for Access VP */
             if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) ||
                  _bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                   return BCM_E_NONE;
             }
         }
    }

    /* Get the match parameters */
    rv = _bcm_esw_l2gre_match_get(unit, l2gre_port, vp);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the next-hop parameters */
    rv = _bcm_tr3_l2gre_port_nh_get(unit, vpn, vp, l2gre_port);
    BCM_IF_ERROR_RETURN(rv);

    /* Get Tunnel index */
    rv = _bcm_tr3_l2gre_egress_dvp_get(unit, vp, l2gre_port);
    BCM_IF_ERROR_RETURN(rv);

    /* Fill in SVP parameters */
    BCM_IF_ERROR_RETURN (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
    l2gre_port->if_class = soc_SOURCE_VPm_field32_get(unit, &svp, CLASS_IDf);
    if (split_horizon_port_flag) {
        l2gre_port->flags |= BCM_L2GRE_PORT_NETWORK;
        l2gre_port->flags |= BCM_L2GRE_PORT_EGRESS_TUNNEL;
    }

    if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 svp1[SOC_MAX_MEM_WORDS];

            BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, svp1));
            soc_SVP_ATTRS_1m_field_get(unit, &svp1, TPID_ENABLEf, &tpid_enable);
        } else
#endif
        {
            tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
        }

        if (tpid_enable) {
            l2gre_port->flags |= BCM_L2GRE_PORT_SERVICE_TAGGED;
            for (i = 0; i < 4; i++) {
                if (tpid_enable & (1 << i)) {
                    _bcm_fb2_outer_tpid_entry_get(unit,
                                        &l2gre_port->egress_service_tpid, i);
                }
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_tr3_l2gre_default_port_add
 * Purpose:
 *      Add L2GRE port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      l2gre_port - (IN/OUT) l2gre_port information (OUT : l2gre_port_id)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_l2gre_default_port_add(int unit, bcm_l2gre_port_t  *l2gre_port)
{
    source_vp_entry_t svp;
    int rv = BCM_E_PARAM, vp=0, num_vp=0;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;
    int network_group=0;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeL2Gre;
    if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
        vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
    } else {
        vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

   /* ======== Allocate/Get Virtual_Port =============== */
    if (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE) {
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }

        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            return BCM_E_NOT_FOUND;
        }
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            return rv;
        }

        /* Decrement old-port's nexthop count */
        rv = _bcm_tr3_l2gre_port_nh_cnt_dec(unit, vp);
        if(rv < 0) {
            return rv;
        }

        /* Decrement old-port's VP count */
        rv = _bcm_tr3_l2gre_port_cnt_update(unit, l2gre_port->l2gre_port_id, vp, FALSE);
        if (rv < 0) {
            return rv;
        }

    } else if (l2gre_port->flags & BCM_L2GRE_PORT_WITH_ID ) {
       if (!BCM_GPORT_IS_L2GRE_PORT(l2gre_port->l2gre_port_id)) {
            return (BCM_E_BADID);
        }

        vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }
        if (vp >= num_vp) {
            return (BCM_E_BADID);
        }

        BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    } else {
       rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp);
       if (rv < 0) {
           return rv;
       }
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));
    }

    /* ======== Configure SVP/DVP Properties ========== */
    soc_SOURCE_VPm_field32_set(unit, &svp, CLASS_IDf,
                               l2gre_port->if_class);

    /* Default Split Horizon Group Id
     * 0 - For Access Port;1 - For Network Port*/
    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
            network_group = l2gre_port->network_group_id;
            rv = _bcm_validate_splithorizon_network_group(unit,
                    l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
            BCM_IF_ERROR_RETURN(rv);
            soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                        network_group);
        } else {
            network_group = l2gre_port->network_group_id;
            rv = _bcm_validate_splithorizon_network_group(unit,
                    l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
            BCM_IF_ERROR_RETURN(rv);
            soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                        network_group);
        }
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf,
                (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) ? 1 : 0);
    }

     soc_SOURCE_VPm_field32_set(unit, &svp,
                                       ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_VFI);

        rv = _bcm_vp_default_cml_mode_get (unit,
                           &cml_default_enable, &cml_default_new,
                           &cml_default_move);
         if (rv < 0) {
            return rv;
         }
        if (cml_default_enable) {
            /* Set the CML to default values */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml_default_new);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, 0x8);
        }
        if (soc_mem_field_valid(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
              soc_SOURCE_VPm_field32_set(unit, &svp, DISABLE_VLAN_CHECKSf, 1);
        }

    /* Configure L2GRE_DEFAULT_NETWORK_SVPr.SVP */
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit, SOURCE_VPm,
            vp, ENTRY_TYPEf, CANCUN_APP__SOURCE_VP__No_Control_Word,
            _BCM_L2GRE_SOURCE_VP_TYPE_VFI)) {
            LOG_ERROR(BSL_LS_BCM_L2GRE,
               (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
        }
    }
#endif

    BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port->l2gre_port_id, vp);

    /* Increment new-port's VP count */
    rv = _bcm_tr3_l2gre_port_cnt_update(unit, l2gre_port->l2gre_port_id, vp, TRUE);
    BCM_IF_ERROR_RETURN(rv);

    return (soc_reg_field32_modify(unit, L2GRE_DEFAULT_NETWORK_SVPr,
             REG_PORT_ANY, SVPf, vp));

}

/*
 * Function:
 *      _bcm_tr3_l2gre_default_port_delete
 * Purpose:
 *      Delete L2GRE Default port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      l2gre_port_id - (IN) L2gre port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_l2gre_default_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    source_vp_entry_t svp;
    int rv = BCM_E_UNAVAIL;
    int vfi_index= -1;
    bcm_gport_t l2gre_port_id;

    if ( vpn != BCM_L2GRE_VPN_INVALID) {
         _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELAN,  vpn);
         if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
            return BCM_E_NOT_FOUND;
         }
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }

    /* Set L2GRE port ID */
    BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port_id, vp);

    /* Clear the SVP and DVP table entries */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    if (rv < 0) {
        return rv;
    }

    /* Decrement port's VP count */
    rv = _bcm_tr3_l2gre_port_cnt_update(unit, l2gre_port_id, vp, FALSE);
    if (rv < 0) {
        return rv;
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVpTypeL2Gre, 1, vp);

    rv = soc_reg_field32_modify(unit, L2GRE_DEFAULT_NETWORK_SVPr,
             REG_PORT_ANY, SVPf, 0);
    return rv;

}

/*
 * Function:
 *      bcm_tr3_l2gre_port_add
 * Purpose:
 *      Add L2GRE port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      l2gre_port - (IN/OUT) l2gre_port information (OUT : l2gre_port_id)
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_port_add(int unit, bcm_vpn_t vpn, bcm_l2gre_port_t  *l2gre_port)
{
    int mode, rv = BCM_E_PARAM;
    uint8 isEline=0xFF;
    int vfi_index=0;
    int bit_num_dvp = 0, bit_num_svp = 0;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if (l2gre_port->if_class) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            bit_num_dvp = soc_mem_field_length(unit, EGR_DVP_ATTRIBUTEm,
                                               L2GRE__CLASS_IDf);
        } else
#endif
        {
            bit_num_dvp = soc_mem_field_length(unit, EGR_DVP_ATTRIBUTE_1m,
                                               L2GRE__CLASS_IDf);
        }
        bit_num_svp = soc_mem_field_length(unit, SOURCE_VPm, CLASS_IDf);
        if ((l2gre_port->if_class > (( 1 << bit_num_dvp ) - 1)) ||
            (l2gre_port->if_class > (( 1 << bit_num_svp ) - 1))){
            return BCM_E_PARAM;
        }
    }

    if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
         _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELINE,  vpn);
        if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
            isEline = 0x0; /* ELAN Dont care VPN Case */
        }
    }

    if (l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_SHARE) {
        if (soc_feature(unit, soc_feature_vp_sharing)) {
            if (vpn != BCM_L2GRE_VPN_INVALID) {
                return BCM_E_PARAM;
            }
            isEline = 0x0;
        } else {
            return BCM_E_UNAVAIL;
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if ((l2gre_port->flags & BCM_L2GRE_PORT_DEFAULT) &&
              !SOC_REG_IS_VALID(unit, L2GRE_DEFAULT_NETWORK_SVPr)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if ((vpn != BCM_L2GRE_VPN_INVALID) && (isEline != 0x0)) {
         BCM_IF_ERROR_RETURN
              (_bcm_tr3_l2gre_vpn_is_eline(unit, vpn, &isEline));
    }

    if (l2gre_port->flags & BCM_L2GRE_PORT_DEFAULT) {
       if (soc_feature(unit, soc_feature_l2gre_default_tunnel)) {
           rv = _bcm_tr3_l2gre_default_port_add(unit, l2gre_port);
#if defined(BCM_TRIDENT3_SUPPORT)
           if (BCM_SUCCESS(rv) && SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
               rv = soc_reg_field32_modify
                      (unit, DEFAULT_SVP_ENABLEr, REG_PORT_ANY,
                                    L2GRE_DEFAULT_SVP_ENABLEf, 1);
               if (rv == SOC_E_NOT_FOUND) {
                   rv = SOC_E_NONE;
               }
           }
#endif
           return rv;
       }
    }

    if (isEline == 0x1 ) {
        rv = _bcm_tr3_l2gre_eline_port_add(unit, vpn, l2gre_port);
    } else if (isEline == 0x0 ) {
        rv = _bcm_tr3_l2gre_elan_port_add(unit, vpn, l2gre_port);
    }

    return rv;
}


/*
 * Function:
 *      bcm_tr3_l2gre_port_delete
 * Purpose:
 *      Delete L2GRE port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      l2gre_port_id - (IN) l2gre port information
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_l2gre_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t l2gre_port_id)
{
    int vp;
    int rv = BCM_E_UNAVAIL;
    uint32 reg_val=0;
    uint8 isEline=0;
    uint32 stat_counter_id;
    int num_ctr = 0;

    if (vpn != BCM_L2GRE_VPN_INVALID) {
        /* Check for Valid Vpn */
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_valid(unit, vpn));
    }

    vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }

    /* Check counters before delete port */
    if (bcm_esw_l2gre_stat_id_get(unit, l2gre_port_id, vpn,
                   bcmL2greOutPackets, &stat_counter_id) == BCM_E_NONE) {
        num_ctr++;
    }
    if (bcm_esw_l2gre_stat_id_get(unit, l2gre_port_id, vpn,
                   bcmL2greInPackets, &stat_counter_id) == BCM_E_NONE) {
        num_ctr++;
    }

    if (num_ctr != 0) {
        return BCM_E_RESOURCE;/* Need detach counters before delete port */
    }

    /* Check for L2GRE default port */
    if (SOC_REG_IS_VALID(unit, L2GRE_DEFAULT_NETWORK_SVPr)) {
        if (soc_feature(unit, soc_feature_l2gre_default_tunnel)) {
            BCM_IF_ERROR_RETURN(READ_L2GRE_DEFAULT_NETWORK_SVPr(unit, &reg_val));
            if (vp == soc_reg_field_get(unit, L2GRE_DEFAULT_NETWORK_SVPr,
                                             reg_val, SVPf)) {
                rv = _bcm_tr3_l2gre_default_port_delete(unit, vpn, vp);
#if defined(BCM_TRIDENT3_SUPPORT)
                if (BCM_SUCCESS(rv) && SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
                    rv = soc_reg_field32_modify
                              (unit, DEFAULT_SVP_ENABLEr, REG_PORT_ANY,
                                           L2GRE_DEFAULT_SVP_ENABLEf, 0);
                    if (rv == SOC_E_NOT_FOUND) {
                        rv = SOC_E_NONE;
                    }
                }
#endif
                return rv;
            }
        }
    }
    if (vpn != BCM_L2GRE_VPN_INVALID) {
        BCM_IF_ERROR_RETURN
             (_bcm_tr3_l2gre_vpn_is_eline(unit, vpn, &isEline));
    } else {
        isEline = 0;
    }

    if (isEline == 0x1 ) {
       rv = _bcm_tr3_l2gre_eline_port_delete(unit, vpn, vp);
    } else if (isEline == 0x0 ) {
       rv = _bcm_tr3_l2gre_elan_port_delete(unit, vpn, vp);
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_port_delete_all
 * Purpose:
 *      Delete all L2GRE ports from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l2gre_port_delete_all(int unit, bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    int vfi_index;
    int vp1 = 0, vp2 = 0;
    uint8 isEline=0xFF;
    bcm_gport_t l2gre_port_id;
    uint32 reg_val=0;

    BCM_IF_ERROR_RETURN
         (_bcm_tr3_l2gre_vpn_is_eline(unit, vpn, &isEline));

    if (isEline == 0x1 ) {
         if ( vpn != BCM_L2GRE_VPN_INVALID) {
              _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELINE,  vpn);
              if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
                   return BCM_E_NOT_FOUND;
              }
         } else {
              vfi_index = _BCM_L2GRE_VFI_INVALID;
         }

         /* ---- Read in current table values for VP1 and VP2 ----- */
         (void) _bcm_tr3_l2gre_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);
         if (vp1 != 0) {
              rv = _bcm_tr3_l2gre_eline_port_delete(unit, vpn, vp1);
              BCM_IF_ERROR_RETURN(rv);
         }
         if (vp2 != 0) {
              rv = _bcm_tr3_l2gre_eline_port_delete(unit, vpn, vp2);
              BCM_IF_ERROR_RETURN(rv);
         }
    } else if (isEline == 0x0 ) {
        uint32 vfi, vp, num_vp;
        source_vp_entry_t svp;

        _BCM_L2GRE_VPN_GET(vfi, _BCM_L2GRE_VPN_TYPE_ELAN,  vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
            rv =  BCM_E_NOT_FOUND;
                return rv;
        }
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        for (vp = 0; vp < num_vp; vp++) {
            /* Check for the validity of the VP */
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                continue;
            }
            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
            if (rv < 0) {
                return rv;
            }
            if (SOC_REG_IS_VALID(unit, L2GRE_DEFAULT_NETWORK_SVPr)) {
                if (soc_feature(unit, soc_feature_l2gre_default_tunnel)) {
                    BCM_IF_ERROR_RETURN(READ_L2GRE_DEFAULT_NETWORK_SVPr(unit, &reg_val));
                    if (vp == soc_reg_field_get(unit, L2GRE_DEFAULT_NETWORK_SVPr,
                                             reg_val, SVPf)) {
                        rv = _bcm_tr3_l2gre_default_port_delete(unit, vpn, vp);
                        if (rv < 0) {
                            return rv;
                        }
#if defined(BCM_TRIDENT3_SUPPORT)
                        if (SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
                            rv = (soc_reg_field32_modify
                                      (unit, DEFAULT_SVP_ENABLEr, REG_PORT_ANY, L2GRE_DEFAULT_SVP_ENABLEf, 0));
                            if (BCM_FAILURE(rv)) {
                                if (rv != BCM_E_NOT_FOUND) {
                                    return rv;
                                } else {
                                    rv = BCM_E_NONE;
                                }
                            }
                        }
#endif
                    }
                }
            }
            if ((soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) == 0x1 &&
                vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf))
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                    || SHR_BITGET(VIRTUAL_INFO(unit)->vp_shared_vp_bitmap, vp)
#endif
          ) {
                   /* Set L2GRE port ID */
                   BCM_GPORT_L2GRE_PORT_ID_SET(l2gre_port_id, vp);
                   rv = bcm_tr3_l2gre_port_delete(unit, vpn, l2gre_port_id);
                   if (rv < 0) {
                      return rv;
                   }
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_port_get
 * Purpose:
 *      Get an L2GRE port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      l2gre_port  - (IN/OUT) l2gre port information
 */
int
bcm_tr3_l2gre_port_get(int unit, bcm_vpn_t vpn, bcm_l2gre_port_t *l2gre_port)
{
    int vp;
    int rv = BCM_E_NONE;

    if (vpn != BCM_L2GRE_VPN_INVALID) {
        /* Check for Valid Vpn */
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_vpn_is_valid(unit, vpn));
    }
    vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_tr3_l2gre_port_get(unit, vpn, vp, l2gre_port);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_port_get_all
 * Purpose:
 *      Get all L2GRE ports from a VPN
 * Parameters:
 *      unit     - (IN) Device Number
 *      vpn      - (IN) VPN instance ID
 *      port_max   - (IN) Maximum number of interfaces in array
 *      port_array - (OUT) Array of L2GRE ports
 *      port_count - (OUT) Number of interfaces returned in array
 */
int
bcm_tr3_l2gre_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                         bcm_l2gre_port_t *port_array, int *port_count)
{
    int vp, rv = BCM_E_NONE;
    int vfi_index;
    int vp1 = 0, vp2 = 0;
    uint8 isEline=0xFF;
    SHR_BITDCL *share_vp_bitmap = NULL;

    BCM_IF_ERROR_RETURN
         (_bcm_tr3_l2gre_vpn_is_eline(unit, vpn, &isEline));

    *port_count = 0;

    if (isEline == 0x1 ) {
         if ( vpn != BCM_L2GRE_VPN_INVALID) {
              _BCM_L2GRE_VPN_GET(vfi_index, _BCM_L2GRE_VPN_TYPE_ELAN,  vpn);
              if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeL2Gre)) {
                   return BCM_E_NOT_FOUND;
              }
         } else {
              vfi_index = _BCM_L2GRE_VFI_INVALID;
         }

        /* ---- Read in current table values for VP1 and VP2 ----- */
        (void) _bcm_tr3_l2gre_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);
        vp = vp1;
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            if (*port_count < port_max) {
                rv = _bcm_tr3_l2gre_port_get(unit, vpn, vp,
                                           &port_array[*port_count]);
                if (rv < 0) {
                    return rv;
                }
                (*port_count)++;
            }
        }

        vp = vp2;
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            if (*port_count < port_max) {
                rv = _bcm_tr3_l2gre_port_get(unit, vpn, vp,
                                           &port_array[*port_count]);
                if (rv < 0) {
                    return rv;
                }
                (*port_count)++;
            }
        }
    } else if (isEline == 0x0 ) {
        uint32 vfi, entry_type;
        int num_vp;
        source_vp_entry_t svp;

        _BCM_L2GRE_VPN_GET(vfi, _BCM_L2GRE_VPN_TYPE_ELAN,  vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
            rv = BCM_E_NOT_FOUND;
            return rv;
        }

        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        /*get sharing VP bitmap*/
        share_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "sharr_vp_bitmap");
        if (share_vp_bitmap == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(share_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vp_sharing)) {
            rv = bcm_tr3_share_vp_get(unit, vpn, share_vp_bitmap);
            _BCM_L2GRE_CLEANUP(rv)
        }
#endif
        for (vp = 0; vp < num_vp; vp++) {
            /* Check for the validity of the VP */
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                continue;
            }
            if (*port_count == port_max) {
                break;
            }
            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
            _BCM_L2GRE_CLEANUP(rv)
            entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);

            if ((vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf) &&
                 entry_type == _BCM_L2GRE_SOURCE_VP_TYPE_VFI)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                 || SHR_BITGET(share_vp_bitmap, vp)
#endif

                ) {
                /* Check if number of ports is requested */
                if (0 == port_max) {
                    (*port_count)++;
                    continue;
                } else if (*port_count == port_max) {
                    break;
                }

               rv = _bcm_tr3_l2gre_port_get(unit, vpn, vp,
                                           &port_array[*port_count]);
                _BCM_L2GRE_CLEANUP(rv)
                (*port_count)++;
            }
        }
        sal_free(share_vp_bitmap);
    }
    return rv;
cleanup:
    sal_free(share_vp_bitmap);
    return rv;

}

/*
 * Function:
 *      bcm_tr3_l2gre_port_learn_set
 * Purpose:
 *      Set the CML bits for a l2gre port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l2gre_port_learn_set(int unit, bcm_gport_t l2gre_port_id,
                                  uint32 flags)
{
    int cml, rv = BCM_E_NONE, entry_type;
    uint32 vp;
    source_vp_entry_t svp;

    rv = _bcm_l2gre_check_init(unit);
    if (rv != BCM_E_NONE){
        return rv;
    }

    cml = 0;
    if (!(flags & BCM_PORT_LEARN_FWD)) {
       cml |= (1 << 0);
    }
    if (flags & BCM_PORT_LEARN_CPU) {
       cml |= (1 << 1);
    }
    if (flags & BCM_PORT_LEARN_PENDING) {
       cml |= (1 << 2);
    }
    if (flags & BCM_PORT_LEARN_ARL) {
       cml |= (1 << 3);
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port_id);

    MEM_LOCK(unit, SOURCE_VPm);
    /* Be sure the entry is used and is set for L2GRE */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return rv;
    }

    /* Ensure that the entry_type is 1 for L2GRE */
    entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);
    if (entry_type != 1){
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }

    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml);
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml);
    /* coverity[negative_returns : FALSE] */
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    MEM_UNLOCK(unit, SOURCE_VPm);

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2gre_port_learn_get
 * Purpose:
 *      Get the CML bits for a L2GRE port
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l2gre_port_learn_get(int unit, bcm_gport_t l2gre_port_id,
                                  uint32 *flags)
{
    int rv, vp, cml = 0, entry_type;
    source_vp_entry_t svp;

    rv = _bcm_l2gre_check_init(unit);
    if (rv != BCM_E_NONE){
        return rv;
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port_id);
    if (vp == -1) {
       return BCM_E_PARAM;
    }

    /* Be sure the entry is used and is set for L2GRE */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        return rv;
    }

    /* Ensure that the entry_type is 1 for L2GRE */
    entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);
    if (entry_type != 1){
        return BCM_E_NOT_FOUND;
    }

    cml = soc_SOURCE_VPm_field32_get(unit, &svp, CML_FLAGS_NEWf);

    *flags = 0;
    if (!(cml & (1 << 0))) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (cml & (1 << 1)) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (cml & (1 << 2)) {
       *flags |= BCM_PORT_LEARN_PENDING;
    }
    if (cml & (1 << 3)) {
       *flags |= BCM_PORT_LEARN_ARL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2gre_trunk_member_add
 * Purpose:
 *      Activate Trunk Port-membership from
 *      EGR_PORT_TO_NHI_MAPPING or EGR_DGPP_TO_NHI
 * Parameters:
 *     unit  - (IN) Device Number
 *     tgid - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be added
 *     trunk_member_array - (IN) Trunk member ports to be added
*/
int
bcm_tr3_l2gre_trunk_member_add(int unit, bcm_trunk_t trunk_id,
                int trunk_member_count, bcm_port_t *trunk_member_array)
{
    int idx = 0;
    int rv = BCM_E_NONE;
    int nh_index = 0, old_nh_index = -1;
    bcm_l3_egress_t egr;
    int gport = 0;
    int modid = 0;
    int local_member = 0;

    rv = _bcm_trunk_id_validate(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        return BCM_E_PORT;
    }

    /* Obtain nh_index for trunk_id */
    rv = _bcm_xgs3_trunk_nh_store_get(unit, trunk_id, &nh_index);
    if (nh_index == 0) {
        return rv;
    }
    bcm_l3_egress_t_init(&egr);
    BCM_IF_ERROR_RETURN(bcm_tr3_l2gre_egress_get(unit, &egr, nh_index));
    if (egr.flags & BCM_L3_L2GRE_ONLY) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
        for (idx = 0; idx < trunk_member_count; idx++) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                gport = trunk_member_array[idx];
            } else
#endif
            {
                rv = bcm_esw_port_local_get(unit, trunk_member_array[idx], &local_member);
                /* Only support trunk local ports */
                if(BCM_FAILURE(rv)) {
                    continue;
                }
                BCM_GPORT_MODPORT_SET(gport, modid, local_member);
            }
            BCM_IF_ERROR_RETURN(
                _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index));
            if (!old_nh_index) {
                BCM_IF_ERROR_RETURN(
                    _bcm_trx_gport_to_nhi_set(unit, gport, nh_index));
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2gre_trunk_member_delete
 * Purpose:
 *      Clear Trunk Port-membership from
 *      EGR_PORT_TO_NHI_MAPPING or EGR_DGPP_TO_NHI
 * Parameters:
 *     unit  - (IN) Device Number
 *     tgid - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be deleted
 *     trunk_member_array - (IN) Trunk member ports to be deleted
*/
int
bcm_tr3_l2gre_trunk_member_delete(int unit, bcm_trunk_t trunk_id,
                int trunk_member_count, bcm_port_t *trunk_member_array)
{
    int idx = 0;
    int rv = BCM_E_NONE, rv2 = BCM_E_NONE;
    int old_nh_index = 0;
    int nh_index = 0;
    bcm_l3_egress_t egr;
    int gport = 0;
    bcm_trunk_t local_tgid;
    int local_id;
    bcm_port_t port;
    bcm_module_t my_modid;

    rv = _bcm_trunk_id_validate(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        return BCM_E_PORT;
    }

    /* Obtain nh_index for trunk_id */
    rv = _bcm_xgs3_trunk_nh_store_get(unit, trunk_id, &nh_index);
    if (nh_index == 0) {
        for (idx = 0; idx < trunk_member_count; idx++) {
            rv2 = _bcm_esw_gport_resolve(unit, trunk_member_array[idx],
                                         &my_modid, &port, &local_tgid,
                                         &local_id);
            if(BCM_FAILURE(rv2)) {
                rv += rv2;
                continue;
            }
            rv += _bcm_tr3_l2gre_trunk_table_reset(unit, port, trunk_id,
                                                   my_modid);
        }
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        bcm_l3_egress_t_init(&egr);
        BCM_IF_ERROR_RETURN(bcm_tr3_l2gre_egress_get(unit, &egr, nh_index));

        if (egr.flags & BCM_L3_L2GRE_ONLY) {
            for (idx = 0; idx < trunk_member_count; idx++) {
                gport = trunk_member_array[idx];
                BCM_IF_ERROR_RETURN(
                    _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index));
                if (old_nh_index) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_trx_gport_to_nhi_set(unit, gport, 0));
                }
            }
        }
    }
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_tr3_l2gre_sw_dump
 * Purpose:
 *     Displays TR3 L2GRE information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_tr3_l2gre_sw_dump(int unit)
{
    int i, num_vp;
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    LOG_CLI((BSL_META_U(unit,
                        "Tunnel Initiator Endpoints:\n")));
    for (i = 0; i < num_vp; i++) {
        if (L2GRE_INFO(unit)->l2gre_tunnel_init[i].dip != 0 &&
            L2GRE_INFO(unit)->l2gre_tunnel_init[i].sip != 0 ) {
            LOG_CLI((BSL_META_U(unit,
                                "Tunnel idx:%d, sip:%x, dip:%x\n"), i,
                     l2gre_info->l2gre_tunnel_init[i].sip,
                     l2gre_info->l2gre_tunnel_init[i].dip));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "Tunnel Terminator Endpoints:\n")));
    for (i = 0; i < num_vp; i++) {
        if (L2GRE_INFO(unit)->l2gre_tunnel_term[i].dip != 0 &&
            L2GRE_INFO(unit)->l2gre_tunnel_term[i].sip != 0 ) {
            LOG_CLI((BSL_META_U(unit,
                                "Tunnel idx:%d, sip:%x, dip:%x\n"), i,
                     l2gre_info->l2gre_tunnel_term[i].sip,
                     l2gre_info->l2gre_tunnel_term[i].dip));
        }
    }

    LOG_CLI((BSL_META_U(unit, "Match Info: \n")));

    for (i = 0; i < num_vp; i++) {
        _bcm_l2gre_match_port_info_t *key = &l2gre_info->match_key[i];

        if ((key->trunk_id == -1) && (key->modid == -1) &&
            (key->port == 0) && (key->flags == 0)) {
            continue;
        }

        LOG_CLI((BSL_META_U(unit, "L2GRE port vp = %d, "), i));
        LOG_CLI((BSL_META_U(unit, "Flags = %x, "), key->flags));
        LOG_CLI((BSL_META_U(unit, "Index = %x, "), key->index));
        LOG_CLI((BSL_META_U(unit, "TGID = %d, "), key->trunk_id));
        LOG_CLI((BSL_META_U(unit, "Modid = %d, "), key->modid));
        LOG_CLI((BSL_META_U(unit, "Port = %d, "), key->port));
        LOG_CLI((BSL_META_U(unit, "Match VLAN = %d, "), key->match_vlan));
        LOG_CLI((BSL_META_U(unit, "Match Inner VLAN = %d, "),
                                  key->match_inner_vlan));
        LOG_CLI((BSL_META_U(unit, "Match VPNid = %d, "),
                                  key->match_vpnid));
        LOG_CLI((BSL_META_U(unit, "Match Tunnel Index = %x\n"),
                                  key->match_tunnel_index));
    }

    return;
}
#endif

#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

