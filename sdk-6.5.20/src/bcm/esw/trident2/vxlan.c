/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * VXLAN API
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/scache.h>
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
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/common/multicast.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/port.h>
#include <soc/td2_td2p.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT*/

#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/flow.h>
#include <soc/esw/cancun.h>
#endif
_bcm_td2_vxlan_bookkeeping_t   *_bcm_td2_vxlan_bk_info[BCM_MAX_NUM_UNITS] = { 0 };

#define VXLAN_INFO(_unit_)      (_bcm_td2_vxlan_bk_info[_unit_])
#define L3_INFO(_unit_)         (&_bcm_l3_bk_info[_unit_])
#define VIRTUAL_INFO(_unit_)    (&_bcm_virtual_bk_info[_unit_])

/*
 * EGR_IP_TUNNEL table usage bitmap operations
 */
#define _BCM_VXLAN_IP_TNL_USED_GET(_u_, _tnl_) \
        SHR_BITGET(VXLAN_INFO(_u_)->vxlan_ip_tnl_bitmap, (_tnl_))
#define _BCM_VXLAN_IP_TNL_USED_SET(_u_, _tnl_) \
        SHR_BITSET(VXLAN_INFO((_u_))->vxlan_ip_tnl_bitmap, (_tnl_))
#define _BCM_VXLAN_IP_TNL_USED_CLR(_u_, _tnl_) \
        SHR_BITCLR(VXLAN_INFO((_u_))->vxlan_ip_tnl_bitmap, (_tnl_))

#define _BCM_VXLAN_CLEANUP(_rv_) \
       if ( (_rv_) < 0) { \
           goto cleanup; \
       }

STATIC int _bcm_td2_vxlan_match_vnid_entry_reset(int unit, uint32 vnid,
                                                            bcm_vlan_t ovid);
STATIC int _bcm_td2_vxlan_bud_loopback_disable(int unit);
STATIC int _bcm_td2_vxlan_sd_tag_set( int unit, bcm_vxlan_vpn_config_t *vxlan_vpn_info, 
                         bcm_vxlan_port_t *vxlan_port, 
                         _bcm_td2_vxlan_nh_info_t  *egr_nh_info, 
                         uint32   *vxlate_entry,
                         int *tpid_index );
STATIC int _bcm_td2_vxlan_egr_xlate_entry_reset(int unit, bcm_vpn_t vpn);
STATIC int
_bcm_td2_vxlan_egr_xlate_entry_get(int unit, int vfi, int vp, uint32 *vxlate_entry);
STATIC void _bcm_td2_vxlan_sd_tag_get( int unit, bcm_vxlan_vpn_config_t *vxlan_vpn_info, 
                         bcm_vxlan_port_t *vxlan_port, 
                         egr_l3_next_hop_entry_t *egr_nh, 
                         uint32   *vxlate_entry,
                         int network_port_flag );
STATIC int _bcm_td2_vxlan_tunnel_initiator_idx_translate(int unit, 
                                                         int soft_idx, 
                                                         int *hw_idx);
#if defined(BCM_TRIDENT3_SUPPORT)
STATIC void
_bcm_td3_vxlan_vxlate_entry_assemble(int unit, _bcm_td3_vxlan_vxlate_entry_t info, uint32* ment);
STATIC int
_bcm_td3_vxlan_tpid_delete(int unit, uint32* vxlate, uint32* egr_nh);
#endif

void bcm_td2_vxlan_match_clear (int unit, int vp);
int _bcm_td2_vxlan_vpn_is_tunnel_based_vnid( int unit, bcm_vpn_t l2vpn, uint8 *vxlan_vpn_type);
STATIC int _bcm_td2_vxlan_vp_based_vnid_delete_all(int unit);

/*
 * Function:
 *      _bcm_vxlan_check_init
 * Purpose:
 *      Check if VXLAN is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


STATIC int 
_bcm_vxlan_check_init(int unit)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    vxlan_info = VXLAN_INFO(unit);

    if ((vxlan_info == NULL) || (vxlan_info->initialized == FALSE)) { 
         return BCM_E_INIT;
    } else {
         return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcm_td2_vxlan_lock
 * Purpose:
 *      Take VXLAN Lock Sempahore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


 int 
 bcm_td2_vxlan_lock(int unit)
{
   int rv=BCM_E_NONE;

   rv = _bcm_vxlan_check_init(unit);
   
   if ( rv == BCM_E_NONE ) {
           sal_mutex_take(VXLAN_INFO((unit))->vxlan_mutex, sal_mutex_FOREVER);
   }
   return rv; 
}



/*
 * Function:
 *      bcm_td2_vxlan_unlock
 * Purpose:
 *      Release  VXLAN Lock Semaphore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


void
bcm_td2_vxlan_unlock(int unit)
{
   int rv=BCM_E_NONE;

   rv = _bcm_vxlan_check_init(unit);
    if ( rv == BCM_E_NONE ) {
         sal_mutex_give(VXLAN_INFO((unit))->vxlan_mutex);
    }
}

/*
 * Function:
 *      bcm_td2_vxlan_udpDestPort_set
 * Purpose:
 *      Set UDP Dest port for VXLAN
 * Parameters:
 *      unit - SOC unit number.
 *      UDP Dest port  -  Non-zero value
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td2_vxlan_udpDestPort_set(int unit, int udpDestPort)
{
    int rv = BCM_E_NONE;
    uint64 reg64;

    COMPILER_64_ZERO(reg64);

    if ((udpDestPort < 0) || (udpDestPort > 0xFFFF) ) {
         return BCM_E_PARAM;
    }

    if (SOC_REG_FIELD_VALID(unit, VXLAN_CONTROLr, UDP_DEST_PORTf)){
        SOC_IF_ERROR_RETURN(READ_VXLAN_CONTROLr(unit, &reg64));
        soc_reg64_field32_set(unit, VXLAN_CONTROLr, &reg64,
                              UDP_DEST_PORTf, udpDestPort);
        SOC_IF_ERROR_RETURN(WRITE_VXLAN_CONTROLr(unit, reg64));
    } else {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_udpSourcePort_set
 * Purpose:
 *      Enable UDP Source port based HASH for VXLAN
 * Parameters:
 *      unit - SOC unit number.
 *      hashEnable - Enable Hash for UDP SourcePort
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td2_vxlan_udpSourcePort_set(int unit, int hashEnable)
{
    int rv = BCM_E_NONE;
    uint64 reg64;

    COMPILER_64_ZERO(reg64);

    if ((hashEnable < 0) || (hashEnable > 1) ) {
         return BCM_E_PARAM;
    }

    if (!SOC_REG_IS_VALID(unit, EGR_VXLAN_CONTROLr)) {
        return rv;
    }

    if (SOC_REG_FIELD_VALID(unit, EGR_VXLAN_CONTROLr, USE_SOURCE_PORT_SELf)){
         soc_reg64_field32_set(unit, EGR_VXLAN_CONTROLr, &reg64,
                      USE_SOURCE_PORT_SELf, hashEnable);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_REG_IS_VALID(unit,FLEX_EDITOR_VXLAN_CONTROL_64r)) {
        SOC_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, FLEX_EDITOR_VXLAN_CONTROL_64r,
            REG_PORT_ANY, VXLAN_FLAGSf, 0x8));
    } else
#endif
    {
        soc_reg64_field32_set(unit, EGR_VXLAN_CONTROLr, &reg64,
                      VXLAN_FLAGSf, 0x8);
    }

    SOC_IF_ERROR_RETURN(WRITE_EGR_VXLAN_CONTROLr(unit, reg64));
    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_udpDestPort_get
 * Purpose:
 *      Get UDP Dest port for VXLAN
 * Parameters:
 *      unit - SOC unit number.
 *      UDP Dest port  -  Non-zero value
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td2_vxlan_udpDestPort_get(int unit, int *udpDestPort)
{
    int rv = BCM_E_NONE;
    uint64 reg64;

    COMPILER_64_ZERO(reg64);

    if (SOC_REG_FIELD_VALID(unit, VXLAN_CONTROLr, UDP_DEST_PORTf)) {
        SOC_IF_ERROR_RETURN(READ_VXLAN_CONTROLr(unit, &reg64));
        *udpDestPort = soc_reg64_field32_get(unit, VXLAN_CONTROLr,
                                                  reg64, UDP_DEST_PORTf);
    } else {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_udpSourcePort_get
 * Purpose:
 *      Get UDP Source port based HASH for VXLAN
 * Parameters:
 *      unit - SOC unit number.
 *      hashEnable - Enable Hash for UDP SourcePort
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td2_vxlan_udpSourcePort_get(int unit, int *hashEnable)
{
    int rv = BCM_E_NONE;
    uint64 reg64;

    COMPILER_64_ZERO(reg64);

    SOC_IF_ERROR_RETURN(READ_EGR_VXLAN_CONTROLr(unit, &reg64));
    *hashEnable = soc_reg64_field32_get(unit, EGR_VXLAN_CONTROLr, 
                                              reg64, USE_SOURCE_PORT_SELf);
    return rv;
}


/*
 * Function:
 *      _bcm_td2_vxlan_free_resource
 * Purpose:
 *      Free all allocated software resources 
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */


STATIC void
_bcm_td2_vxlan_free_resource(int unit)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);

    /* If software tables were not allocated we are done. */ 
    if (NULL == VXLAN_INFO(unit)) {
        return;
    }

    /* Destroy EGR_IP_TUNNEL usage bitmap */
    if (vxlan_info->vxlan_ip_tnl_bitmap) {
        sal_free(vxlan_info->vxlan_ip_tnl_bitmap);
        vxlan_info->vxlan_ip_tnl_bitmap = NULL;
    }

    if (vxlan_info->match_key) {
        sal_free(vxlan_info->match_key);
        vxlan_info->match_key = NULL;
    }

    if (vxlan_info->vxlan_tunnel_init) {
        sal_free(vxlan_info->vxlan_tunnel_init);
        vxlan_info->vxlan_tunnel_init = NULL;
    }

    if (vxlan_info->vxlan_tunnel_term) {
        sal_free(vxlan_info->vxlan_tunnel_term);
        vxlan_info->vxlan_tunnel_term = NULL;
    }

    if (vxlan_info->vxlan_vpn_vlan) {
        sal_free(vxlan_info->vxlan_vpn_vlan);
        vxlan_info->vxlan_vpn_vlan = NULL;
    }
    if (vxlan_info->vxlan_vpn_info) {
        sal_free(vxlan_info->vxlan_vpn_info);
        vxlan_info->vxlan_vpn_info = NULL;
    }
    if (vxlan_info->vfi_vnid_map_count) {
        sal_free(vxlan_info->vfi_vnid_map_count);
        vxlan_info->vfi_vnid_map_count = NULL;
    }
    /* Free module data. */
    sal_free(VXLAN_INFO(unit));
    VXLAN_INFO(unit) = NULL;
}

/*
 * Function:
 *      bcm_td2_vxlan_allocate_bk
 * Purpose:
 *      Initialize VXLAN software book-kepping
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
bcm_td2_vxlan_allocate_bk(int unit)
{
    /* Allocate/Init unit software tables. */
    if (NULL == VXLAN_INFO(unit)) {
        BCMI_VXLAN_ALLOC(VXLAN_INFO(unit), sizeof(_bcm_td2_vxlan_bookkeeping_t),
                         "vxlan_bk_module_data");
        if (NULL == VXLAN_INFO(unit)) {
            return (BCM_E_MEMORY);
        } else {
            VXLAN_INFO(unit)->initialized = FALSE;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_hw_clear
 * Purpose:
 *     Perform hw tables clean up for VXLAN module. 
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
 
STATIC int
_bcm_td2_vxlan_hw_clear(int unit)
{
    int rv = BCM_E_NONE, rv_error = BCM_E_NONE;

    rv = bcm_td2_vxlan_tunnel_terminator_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    rv = bcm_td2_vxlan_tunnel_initiator_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    rv =_bcm_td2_vxlan_vp_based_vnid_delete_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)){
        rv_error = rv;
    }

    rv = bcm_td2_vxlan_vpn_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    rv = _bcm_td2_vxlan_bud_loopback_disable(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    return rv_error;
}

/*
 * Function:
 *      bcm_td2_vxlan_cleanup
 * Purpose:
 *      DeInit  VXLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2_vxlan_cleanup(int unit)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int rv = BCM_E_UNAVAIL;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    vxlan_info = VXLAN_INFO(unit);

    if (FALSE == vxlan_info->initialized) {
        return (BCM_E_NONE);
    } 

    rv = bcm_td2_vxlan_lock (unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) { 
        rv = _bcm_td2_vxlan_hw_clear(unit);
    }

    /* Mark the state as uninitialized */
    vxlan_info->initialized = FALSE;

    sal_mutex_give(vxlan_info->vxlan_mutex);

    /* Destroy protection mutex. */
    sal_mutex_destroy(vxlan_info->vxlan_mutex );

    /* Free software resources */
    (void) _bcm_td2_vxlan_free_resource(unit);

    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_match_count_adjust
 * Purpose:
 *      Obtain ref-count for a VXLAN port
 * Returns:
 *      BCM_E_XXX
 */

void
bcm_td2_vxlan_port_match_count_adjust(int unit, int vp, int step)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
 
    vxlan_info->match_key[vp].match_count += step;
}


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

STATIC int _bcm_td2_vxlan_wb_alloc(int unit);

/*
 * Function:
 *      _bcm_td2_vxlan_wb_recover
 *
 * Purpose:
 *      Recover VXLAN module info for Level 2 Warm Boot from persisitent memory
 *
 * Warm Boot Version Map:
 *      see _bcm_esw_vxlan_sync definition
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */
 
STATIC int
_bcm_td2_vxlan_wb_recover(int unit)
{
    int i, sz = 0, rv = BCM_E_NONE;
    int num_tnl = 0, num_vp = 0;
    int stable_size;
    int additional_scache_size = 0;
    uint16 recovered_ver = 0;
    int add_sz_1_0 = 0;
    int add_sz_1_1 = 0;
    int add_sz_1_2 = 0;
    int add_sz_1_4 = 0;
    int tnl_idx = 0;
    bcm_ip_t sip = 0;
    bcm_gport_t gport;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS];
    uint8 *vxlan_state = NULL;
    soc_scache_handle_t scache_handle;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int num_vfi = 0;

    vxlan_info = VXLAN_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VXLAN, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &vxlan_state, 
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } else if (rv == BCM_E_NOT_FOUND) {
        return _bcm_td2_vxlan_wb_alloc(unit);
    }

    if (vxlan_state != NULL) {
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        num_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);
        num_vfi = soc_mem_index_count(unit, VFIm);

        if (recovered_ver >= BCM_WB_VERSION_1_4) {
            sz = num_vp * sizeof(_bcm_vxlan_tunnel_endpoint_t);
        } else if (recovered_ver >= BCM_WB_VERSION_1_2) {
            sz = num_vp * (sizeof(_bcm_vxlan_tunnel_endpoint_t) - sizeof(bcm_vlan_t));
            additional_scache_size =
                     ((num_vp * sizeof(_bcm_vxlan_tunnel_endpoint_t)) - sz) * 2;
        } else if (recovered_ver >= BCM_WB_VERSION_1_1) {
            /* Warm boot upgrade case, activate_flag was added.*/
            sz = num_vp * (sizeof(_bcm_vxlan_tunnel_endpoint_t) - sizeof(int)
                            - sizeof(bcm_vlan_t));
            additional_scache_size =
                     ((num_vp * sizeof(_bcm_vxlan_tunnel_endpoint_t)) - sz) * 2;
        } else {
            /* Warm boot upgrade case, in the previous version (SDK 6.3.4)
                tunnel_state was uint8, now changed to uint16 */
            sz = num_vp * (sizeof(_bcm_vxlan_tunnel_endpoint_t) - 1 - sizeof(int)
                            - sizeof(bcm_vlan_t));
            additional_scache_size =
                      ((num_vp * sizeof(_bcm_vxlan_tunnel_endpoint_t)) - sz) * 2;
        } 

        /* Version_1_0: _bcm_vxlan_tunnel_endpoint_t in vxlan_bookkeeping_t
         * bcm_ip_t dip + bcm_ip_t sip + uint8 tunnel_state
         * comments: (tunnel_state is ONLY valid to Tunnel Terminator in v_1_0)
         * Version_1_1: _bcm_vxlan_tunnel_endpoint_t in vxlan_bookkeeping_t
         * bcm_ip_t dip +  bcm_ip_t  sip + uint16 tunnel_state; 
         *   
         * Version_1_2: _bcm_vxlan_tunnel_endpoint_t in vxlan_bookkeeping_t
         * bcm_ip_t dip + bcm_ip_t sip + uint16 tunnel_state + int activate_flag
         * comments: (activate_flag is ONLY valid to Tunnel Terminator in v_1_2)
         * Version_1_0 -> Version_1_1: tunnel_state (uni8->unit16)
         * Version_1_0 -> Version_1_2: tunnel_state (uni8->unit16)
         *                             + activate_flag
         * Version_1_1 -> Version_1_2: + activate_flag
         */
        
        /* size of (DIP, SIP, Tunnel State(uint8)) */
        add_sz_1_0 = sizeof(bcm_ip_t) + sizeof(bcm_ip_t) + sizeof(uint8);
        
        /* Tunnel State uint8==> uint16 */
        add_sz_1_1 = 1;

        /* Add activate_flag */
        add_sz_1_2 = sizeof(int);

        /* Add vlan */
        add_sz_1_4 = sizeof(bcm_vlan_t);

        /* Recover Vxlan Tunnel Terminator (DIP, SIP, Tunnel State(unit8))
         * To correspond with the sync actions in Version_1_0
         */
        if (recovered_ver == BCM_WB_VERSION_1_0) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->vxlan_tunnel_term[i]),
                    vxlan_state, add_sz_1_0 - sizeof(uint8));
                vxlan_state += add_sz_1_0 - sizeof(uint8);
                /* Tunnel State uint8 --> uint16 */
                vxlan_info->vxlan_tunnel_term[i].tunnel_state = 
                    *((uint8 *)vxlan_state);
                vxlan_state += sizeof(uint8);
            }
        }

        /* Recover Vxlan Tunnel Terminator (DIP, SIP, Tunnel State(uint16))
         * To correspond with the sync actions in Version_1_1
         */
        if (recovered_ver == BCM_WB_VERSION_1_1) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->vxlan_tunnel_term[i]),
                    vxlan_state, add_sz_1_0 + add_sz_1_1);
                vxlan_state += add_sz_1_0 + add_sz_1_1;
            }
        }

        /* Recover Vxlan Tunnel Terminator 
         * (DIP, SIP, Tunnel State(uint16), activate_flag)
         * To correspond with the sync actions in Version_1_2
         */
        if ((recovered_ver >= BCM_WB_VERSION_1_2) 
             && (recovered_ver < BCM_WB_VERSION_1_4)) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->vxlan_tunnel_term[i]),
                    vxlan_state, add_sz_1_0 + add_sz_1_1 + add_sz_1_2);
                vxlan_state += add_sz_1_0 + add_sz_1_1 + add_sz_1_2;
            }
        }

        /* Recover Vxlan Tunnel Terminator 
          * (DIP, SIP, Tunnel State(uint16), activate_flag, vlan)
          * To correspond with the sync actions in Version_1_4
          */
        if (recovered_ver >= BCM_WB_VERSION_1_4) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->vxlan_tunnel_term[i]),
                    vxlan_state,
                    add_sz_1_0 + add_sz_1_1 + add_sz_1_2 + add_sz_1_4);
                vxlan_state += add_sz_1_0 + add_sz_1_1 + add_sz_1_2 + add_sz_1_4;
            }
        }
        /* Recover Vxlan Tunnel Initiator (DIP, SIP, Tunnel State(unit8))
         * To correspond with the sync actions in Version_1_0
         */
        if (recovered_ver == BCM_WB_VERSION_1_0) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->vxlan_tunnel_init[i]),
                    vxlan_state, add_sz_1_0 - sizeof(uint8));
                vxlan_state += add_sz_1_0 - sizeof(uint8);
                vxlan_info->vxlan_tunnel_init[i].tunnel_state = 
                    *((uint8 *)vxlan_state);
                vxlan_state += sizeof(uint8);
            }
        }

        /* Recover Vxlan Tunnel Initiator (DIP, SIP, Tunnel State(uint16)) 
         * To correspond with the sync actions in Version_1_1
         */
        if (recovered_ver == BCM_WB_VERSION_1_1) {
            for (i = 0; i < num_vp; i++) {                
                sal_memcpy(&(vxlan_info->vxlan_tunnel_init[i]),
                    vxlan_state, add_sz_1_0 + add_sz_1_1);
                vxlan_state += (add_sz_1_0 + add_sz_1_1);
            }
        }
        
        /* Recover Vxlan Tunnel Initiator
         * (DIP, SIP, Tunnel State(uint16), activate_flag)
         * To correspond with the sync actions in Version_1_2
         */
        if (recovered_ver >= BCM_WB_VERSION_1_2 
            && (recovered_ver < BCM_WB_VERSION_1_4)) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->vxlan_tunnel_init[i]),
                    vxlan_state, add_sz_1_0 + add_sz_1_1 + add_sz_1_2);
                vxlan_state += add_sz_1_0 + add_sz_1_1 + add_sz_1_2;
            }
        }
		
        /* Recover Vxlan Tunnel Initiator
               * (DIP, SIP, Tunnel State(uint16), activate_flag, vlan)
               * To correspond with the sync actions in Version_1_4
               */

        if (recovered_ver >= BCM_WB_VERSION_1_4) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->vxlan_tunnel_init[i]),
                    vxlan_state, add_sz_1_0 + add_sz_1_1 + add_sz_1_2 + add_sz_1_4);
                vxlan_state += add_sz_1_0 + add_sz_1_1 + add_sz_1_2 + add_sz_1_4;
            }
        }

        /* Recover the BITMAP of Vxlan Tunnel usage */
        sal_memcpy(vxlan_info->vxlan_ip_tnl_bitmap, vxlan_state, 
                    SHR_BITALLOCSIZE(num_tnl));
        vxlan_state += SHR_BITALLOCSIZE(num_tnl);

        /* Recover the flags & match_tunnel_index of each Match Key */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(&(vxlan_info->match_key[i].flags), vxlan_state, 
                        sizeof(uint32));
            vxlan_state += sizeof(uint32);        
            sal_memcpy(&(vxlan_info->match_key[i].match_tunnel_index), vxlan_state, 
                        sizeof(uint32));
            vxlan_state += sizeof(uint32);
        }

        /* set default information for warm boot upgrade case */
        if (recovered_ver < BCM_WB_VERSION_1_2) {
            for (i = 0; i < num_vp; i++) {
                /* Version_1_0/Version_1_1 -> Version_1_2: 
                 * activate_flag is introduced by Version_1_2
                 * and ONLY valid to Tunnel Terminator.
                 * Here is to set default flag with TERM_ENABLE.
                 */
                if (vxlan_info->vxlan_tunnel_term[i].sip != 0) {
                    vxlan_info->vxlan_tunnel_term[i].activate_flag |=
                        _BCM_VXLAN_TUNNEL_TERM_ENABLE;
                }
            }

            if (recovered_ver == BCM_WB_VERSION_1_0) {
                /* Version_1_0 -> Version_1_2:
                 * Start from Version_1_1 - Set dest udp port on tunnel_state 
                 * field for Tunnel Initiator. Here is to get dest udp port 
                 * from HW entry.
                 */
                for (tnl_idx = 0; tnl_idx < num_tnl; tnl_idx++) {
                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_IP_TUNNELm,
                        MEM_BLOCK_ANY, tnl_idx, tnl_entry));
                    sip = soc_mem_field32_get(unit, EGR_IP_TUNNELm,
                        tnl_entry, SIPf);
                    if (sip != 0) {
                        for (i = 0; i < num_vp; i++) {
                            if (sip == vxlan_info->vxlan_tunnel_init[i].sip) {
                                vxlan_info->vxlan_tunnel_init[i].tunnel_state =
                                    soc_mem_field32_get(unit, EGR_IP_TUNNELm,
                                        tnl_entry, L4_DEST_PORTf);
                            }
                        }
                    }
                }
            }
        }

        /* 
         * Recover the index && gport(trunk_id, modid, port) && match_vlan && 
         * match_inner_vlan of each Match Key
         */
        if (recovered_ver >= BCM_WB_VERSION_1_3) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->match_key[i].index), vxlan_state, 
                           sizeof(uint32));
                vxlan_state += sizeof(uint32);        
                sal_memcpy(&gport, vxlan_state, sizeof(bcm_gport_t));
                if (BCM_GPORT_IS_TRUNK(gport)) {
                    vxlan_info->match_key[i].trunk_id = 
                                             BCM_GPORT_TRUNK_GET(gport);
                } else if (BCM_GPORT_IS_MODPORT(gport)) {
                    vxlan_info->match_key[i].modid = 
                                             BCM_GPORT_MODPORT_MODID_GET(gport);
                    vxlan_info->match_key[i].port = 
                                             BCM_GPORT_MODPORT_PORT_GET(gport);
                }
                vxlan_state += sizeof(bcm_gport_t);        
                sal_memcpy(&(vxlan_info->match_key[i].match_vlan), vxlan_state, 
                           sizeof(bcm_vlan_t));
                vxlan_state += sizeof(bcm_vlan_t);
                sal_memcpy(&(vxlan_info->match_key[i].match_inner_vlan),
                           vxlan_state, sizeof(bcm_vlan_t));
                vxlan_state += sizeof(bcm_vlan_t);
            }
        } else {
            additional_scache_size += num_vp * (sizeof(uint32) + 
                                                sizeof(bcm_gport_t) +
                                                sizeof(bcm_vlan_t) * 2);
        }

        if (recovered_ver >= BCM_WB_VERSION_1_5) {
            for (i = 0; i < num_vfi; i++) {
                sal_memcpy(&(vxlan_info->vxlan_vpn_vlan[i]), vxlan_state,
                           sizeof(bcm_vlan_t));
                vxlan_state += sizeof(bcm_vlan_t);
            }
        } else {
            additional_scache_size += num_vfi * sizeof(bcm_vlan_t);
        }
        if (recovered_ver >= BCM_WB_VERSION_1_6) {
             /* Recover the type of each vxlan vpn */
            for (i = 0; i < num_vfi; i++) {
                sal_memcpy(&(vxlan_info->vxlan_vpn_info[i].sip), vxlan_state,
                           sizeof(bcm_ip_t));
                vxlan_state += sizeof(bcm_ip_t);
                sal_memcpy(&(vxlan_info->vxlan_vpn_info[i].vnid), vxlan_state,
                           sizeof(uint32));
                vxlan_state += sizeof(uint32);
                sal_memcpy(&(vxlan_info->vxlan_vpn_info[i].vxlan_vpn_type), vxlan_state,
                           sizeof(uint8));
                vxlan_state += sizeof(uint8);          
            }
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->match_key[i].vfi), vxlan_state,
                           sizeof(int));
                vxlan_state += sizeof(int);
            }
        } else {
            additional_scache_size += num_vfi * (sizeof(uint8) + sizeof(bcm_ip_t) +
                                                 sizeof(uint32)) + num_vp * sizeof(int);
        }

        /*
         * Recover flags2 of each Match Key
         */
        if (recovered_ver >= BCM_WB_VERSION_1_7) {
            for (i = 0; i < num_vp; i++) {
                sal_memcpy(&(vxlan_info->match_key[i].flags2), vxlan_state,
                           sizeof(uint32));
                vxlan_state += sizeof(uint32);
            }
        } else {
            additional_scache_size += num_vp * (sizeof(uint32));
        }

        /* Reallocate additional scache size required for tunnel_state in 
           Vxlan Tunnel Terminator and Vxlan Tunnel Initiator */ 
        if (additional_scache_size > 0) {
            rv = soc_scache_realloc(unit,scache_handle,additional_scache_size);
            if(BCM_FAILURE(rv)) {
               return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_wb_alloc
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
_bcm_td2_vxlan_wb_alloc(int unit)
{
    int alloc_sz = 0, rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    int num_tnl = 0, num_vp = 0;
    int num_vfi = 0;

    uint8 *vxlan_state;
    int stable_size;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    /* Size of Tunnel Terminator & Initiator of all VP */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    alloc_sz += num_vp * sizeof(_bcm_vxlan_tunnel_endpoint_t) * 2;

    /* Size of EGR_IP_TUNNEL index bitmap */
    num_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);
    alloc_sz += SHR_BITALLOCSIZE(num_tnl);

    /* Size of All match_key's flags & match_tunnel_index */
    alloc_sz += num_vp * sizeof(uint32) * 2;

    /*
     * Size of All match_key's index && gport(trunk_id, modid, port) && 
     * match_vlan && match_inner_vlan
     */
    alloc_sz += num_vp * (sizeof(uint32) + sizeof(bcm_gport_t) +
                          sizeof(bcm_vlan_t) * 2);

    /* BCM_WB_VERSION_1_5 */
    num_vfi = soc_mem_index_count(unit, VFIm);
    alloc_sz += num_vfi * sizeof(bcm_vlan_t);

    /* BCM_WB_VERSION_1_6 */
    alloc_sz += num_vfi * sizeof(_bcm_vxlan_vpn_info_t);
    alloc_sz += num_vp * sizeof(int);

    /* BCM_WB_VERSION_1_7 */
    alloc_sz += num_vp * sizeof(uint32);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VXLAN, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 alloc_sz, (uint8**)&vxlan_state, 
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_tunnel_initiator_reinit
 * Purpose:
 *      Tunnel initiator hash and ref-count recovery.
 * Parameters:
 *      unit          - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_vxlan_tunnel_initiator_reinit(int unit)
{
    bcm_tunnel_initiator_t tnl_init_info;
    int rv = BCM_E_NONE;
    int i = 0;
    int idx_min = 0;
    int idx_max = 0;
    egr_ip_tunnel_entry_t *egr_tnl_entry = NULL;
    uint8                 *egr_tnl_buf   = NULL;

    idx_min = soc_mem_index_min(unit, EGR_IP_TUNNELm);
    idx_max = soc_mem_index_max(unit, EGR_IP_TUNNELm);

    egr_tnl_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, EGR_IP_TUNNELm),
                                "EGR_IP_TUNNEL buffer");
    if (NULL == egr_tnl_buf) {
        return BCM_E_MEMORY;
    }

    rv = soc_mem_read_range(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY,
                            idx_min, idx_max, egr_tnl_buf);
    if (SOC_FAILURE(rv)) {
        goto clean_up;
    }

    for (i = idx_min; i <= idx_max; i++) {

        egr_tnl_entry = soc_mem_table_idx_to_pointer(unit, EGR_IP_TUNNELm,
                                                     egr_ip_tunnel_entry_t *,
                                                     egr_tnl_buf, i);
        if (_BCM_VXLAN_TUNNEL_TYPE ==
            soc_mem_field32_get(unit, EGR_IP_TUNNELm,
                                egr_tnl_entry, TUNNEL_TYPEf)) {

            bcm_tunnel_initiator_t_init(&tnl_init_info);
            tnl_init_info.type = bcmTunnelTypeVxlan;
            tnl_init_info.sip =
                soc_mem_field32_get(unit, EGR_IP_TUNNELm,
                                    egr_tnl_entry, SIPf);
            tnl_init_info.udp_dst_port =
                soc_mem_field32_get(unit, EGR_IP_TUNNELm,
                                    egr_tnl_entry, L4_DEST_PORTf);
            rv = _bcm_td2_vxlan_tunnel_initiator_hash_calc(
                     unit, &tnl_init_info,
                     &BCM_XGS3_L3_ENT_HASH(
                     BCM_XGS3_L3_TBL_PTR(unit, tnl_init), i));
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
            BCM_XGS3_L3_ENT_REF_CNT_INC(
                BCM_XGS3_L3_TBL_PTR(unit, tnl_init), i,
                ((_BCM_TUNNEL_OUTER_HEADER_IPV6(tnl_init_info.type)) ?
                _BCM_DOUBLE_WIDE : _BCM_SINGLE_WIDE));
        }
    }
clean_up:

    if (NULL != egr_tnl_buf) {
        soc_cm_sfree(unit, egr_tnl_buf);
    }
    return rv;
}


/*
 * Function:
 *      _bcm_d2_vxlan_vp_based_vnid_reinit
 * Purpose:
 *      Per VP VFI-VNID mappings rules ref-count recovery.
 * Parameters:
 *      unit          - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_vxlan_vp_based_vnid_reinit(int unit)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
    soc_field_t valid_field;
    soc_field_t type_field;
    soc_mem_t egr_vxlate_mem;
    int rv = BCM_E_NONE;
    int i = 0;
    int idx_min = 0;
    int idx_max = 0;
    int decouple = 0;
    void *egr_vxlate_entry = NULL;
    uint8 *egr_vxlate_buf = NULL;
    uint32 entry_type;
    int vfi;
    bcm_vpn_t vpn;
    int vp;
    uint8 vxlan_vpn_type;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        egr_vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
        valid_field = BASE_VALIDf;
        type_field = KEY_TYPEf;
        decouple = 1;
    } else
#endif
    {
        egr_vxlate_mem = EGR_VLAN_XLATEm;
        valid_field = VALIDf;
        type_field = ENTRY_TYPEf;
    }

    idx_min = soc_mem_index_min(unit, egr_vxlate_mem);
    idx_max = soc_mem_index_max(unit, egr_vxlate_mem);

    egr_vxlate_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, egr_vxlate_mem),
                                "EGR_VLAN_XLATE buffer");
    if (NULL == egr_vxlate_buf) {
        return BCM_E_MEMORY;
    }

    rv = soc_mem_read_range(unit, egr_vxlate_mem, MEM_BLOCK_ANY,
                            idx_min, idx_max, egr_vxlate_buf);
    if (SOC_FAILURE(rv)) {
        goto clean_up;
    }

    for (i = idx_min; i <= idx_max; i++) {
        egr_vxlate_entry = soc_mem_table_idx_to_pointer(unit, egr_vxlate_mem,
                                                        void*, egr_vxlate_buf, i);

        if (soc_mem_field32_get(unit, egr_vxlate_mem, egr_vxlate_entry, valid_field)) {
            entry_type = soc_mem_field32_get(unit, egr_vxlate_mem, egr_vxlate_entry, type_field);
            if (entry_type != _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP) {
                continue;
            }

            vfi = soc_mem_field32_get(unit, egr_vxlate_mem,
                egr_vxlate_entry, BCM_VXLAN_VFI_FIELD(decouple, VFIf));
            _BCM_VXLAN_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, vfi);
            rv = _bcm_td2_vxlan_vpn_is_tunnel_based_vnid(unit, vpn, &vxlan_vpn_type);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
            if (vxlan_vpn_type) {
                continue;
            }

            vp  = soc_mem_field32_get(unit, egr_vxlate_mem,
                egr_vxlate_entry, BCM_VXLAN_VFI_FIELD(decouple, DVPf));
            vxlan_info->vfi_vnid_map_count[vp]++;

#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                int nh_index;
                int entry_type = 0;
                ing_dvp_table_entry_t ing_dvp_entry;
                egr_l3_next_hop_entry_t egr_nh_entry;

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_DVP_TABLEm,
                                    MEM_BLOCK_ANY, vp, &ing_dvp_entry));
                nh_index = soc_ING_DVP_TABLEm_field32_get(unit,
                             &ing_dvp_entry, NEXT_HOP_INDEXf);
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                                     MEM_BLOCK_ANY, nh_index, &egr_nh_entry));
                entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                        &egr_nh_entry, ENTRY_TYPEf);
                if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW) {
                    /* new VxLAN decoupled mode view - recover TPID ref count */
                    rv = _bcm_td3_flex_egrnh_tpid_recover(unit, &egr_nh_entry);
                    BCM_IF_ERROR_RETURN(rv);
                }
            }
#endif
        }
    }

clean_up:

    if (NULL != egr_vxlate_buf) {
        soc_cm_sfree(unit, egr_vxlate_buf);
    }
    return rv;
}
/*
 * Function:
 *      _bcm_td2_vxlan_source_vp_tpid_recover
 * Purpose:
 *      - Recover SVP TPIDs
 * Parameters:
 *      unit    : (IN) Device Unit Number
 *      vp      : (IN) Index into SOURCE_VPm table
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_vxlan_source_vp_tpid_recover(int unit, int vp)
{
    int index, tpid_enable;
    source_vp_entry_t svp_entry;

    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SOURCE_VPm, MEM_BLOCK_ANY, vp, &svp_entry));

    if (soc_SOURCE_VPm_field32_get(unit, &svp_entry, SD_TAG_MODEf) == 1) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            uint32 svp_attrs_1[SOC_MAX_MEM_WORDS];

            BCM_IF_ERROR_RETURN(
              READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, svp_attrs_1));

            soc_SVP_ATTRS_1m_field_get(unit,
                                       &svp_attrs_1,
                                       TPID_ENABLEf,
                                       (uint32 *)&tpid_enable);
        } else
#endif
        {
            tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                     TPID_ENABLEf);
        }

        for (index = 0; index < 4; index++) {
            if (tpid_enable & (1 << index)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1));
                break;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_reinit
 * Purpose:
 *      Warm boot recovery for the VXLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_td2_vxlan_reinit(int unit)
{
    soc_mem_t mem;  
    int i, index_min, index_max, buf_size;
    int vp;
    int rv = BCM_E_NONE;
    void *entry;
    uint32 trunk, tgid, mod_id, port_num, key_type;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    uint8 *trunk_buf = NULL;
    uint8 *xlate_buf = NULL;
    uint8 *mpls_buf = NULL;
    soc_field_t vld_fld = SOURCE_VPf;
    soc_field_t tgid_fld = TGIDf;
    int d = 0;

    vxlan_info = VXLAN_INFO(unit);

    mem = SOURCE_TRUNK_MAP_TABLEm;
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    /* We check SOURCE_TRUNK_MAP_TABLE table outside to avoid the HUGE loop.
     * Set each match_key[vp].index first, and override it to zero later if 
     *  it's not needed.
     */
    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    trunk_buf = soc_cm_salloc(unit, buf_size, "SOURCE_TRUNK_MAP_TABLE buffer");
    if (NULL == trunk_buf) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, trunk_buf);
    if (SOC_FAILURE(rv)) {
        goto clean_up;
    }

    if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
        vld_fld = SVP_VALIDf;
    }

    if (SOC_IS_TRIDENT3X(unit) && soc_mem_field_valid(unit, mem, SRC_TGIDf)) {
        tgid_fld = SRC_TGIDf;
    }

    for (i = index_min; i <= index_max; i++) {
        bcm_trunk_t tgid = -1;
        int port_type = 0;
        entry = soc_mem_table_idx_to_pointer(unit, mem, void *, trunk_buf, i);
        if (soc_mem_field32_get(unit, mem, entry, vld_fld) == 0) {
            continue;
        }

        port_type = soc_mem_field32_get(unit, mem, entry, PORT_TYPEf);
        if (port_type == 1) {                   /* Trunk */
            tgid = soc_mem_field32_get(unit, mem, entry, tgid_fld);
        }

        vp = soc_mem_field32_get(unit, mem, entry, SOURCE_VPf);
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            if (port_type == 1) {               /* Trunk */
                vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK;
                vxlan_info->match_key[vp].trunk_id = tgid; 
                vxlan_info->match_key[vp].modid = -1;
            } else {
                vxlan_info->match_key[vp].index = i; /* SRC Trunk table index */
                vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_PORT;
                vxlan_info->match_key[vp].trunk_id = -1; 
                vxlan_info->match_key[vp].modid = -1;            
            }
        }
    }    

    /* 
     * For Trident2 device currently
     *
     * foreach valid Source VP used by VXLAN of VLAN_XLANTE table
     *   build the inner/outer 802.1Q tag info, trunk/port info, flag of each SVP.
     *   base on hash key type and trunk.
     * 
     * Sets the following fields:
     *   
     *   VXLAN_INFO[unit].match_key[SVP].flags
     *   VXLAN_INFO[unit].match_key[SVP].match_vlan
     *   VXLAN_INFO[unit].match_key[SVP].match_inner_vlan
     *   VXLAN_INFO[unit].match_key[SVP].trunk_id
     *   VXLAN_INFO[unit].match_key[SVP].port
     *   VXLAN_INFO[unit].match_key[SVP].modid     
     *   VXLAN_INFO[unit].match_key[SVP].index 
 */

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
        rv = BCM_E_MEMORY;
        goto clean_up;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, xlate_buf);
    if (SOC_FAILURE(rv)) {
        goto clean_up;
    } 
    
    for (i = index_min; i <= index_max; i++) {
        entry = soc_mem_table_idx_to_pointer(unit, mem, void *, xlate_buf, i);

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

        if (soc_mem_field32_get(unit, mem, entry, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }
        
        vp = soc_mem_field32_get(unit, mem, entry, XLATE__SOURCE_VPf);

        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            _bcm_vp_info_t vp_info;
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));

            if (vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
                vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_SHARED;
            } else
#endif 
            {
                key_type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);
                trunk = soc_mem_field32_get(unit, mem, entry, XLATE__Tf);
                tgid = soc_mem_field32_get(unit, mem, entry, XLATE__TGIDf);
                mod_id = soc_mem_field32_get(unit, mem, entry, XLATE__MODULE_IDf);
                port_num = soc_mem_field32_get(unit, mem, entry, XLATE__PORT_NUMf);
            
                if (TR_VLXLT_HASH_KEY_TYPE_OVID == key_type) {
                    vxlan_info->match_key[vp].flags =
                                            _BCM_VXLAN_PORT_MATCH_TYPE_VLAN;
                    vxlan_info->match_key[vp].match_vlan =
                        soc_mem_field32_get(unit, mem, entry, XLATE__OVIDf);
                } else if (TR_VLXLT_HASH_KEY_TYPE_IVID == key_type) {
                    vxlan_info->match_key[vp].flags =
                                          _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN;
                    vxlan_info->match_key[vp].match_inner_vlan =
                         soc_mem_field32_get(unit, mem, entry, XLATE__IVIDf);
                } else if (TR_VLXLT_HASH_KEY_TYPE_IVID_OVID == key_type) {
                    vxlan_info->match_key[vp].flags =
                                        _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED;
                    vxlan_info->match_key[vp].match_vlan =
                        soc_mem_field32_get(unit, mem, entry, XLATE__OVIDf);
                    vxlan_info->match_key[vp].match_inner_vlan =
                         soc_mem_field32_get(unit, mem, entry, XLATE__IVIDf);
                } else if (TR_VLXLT_HASH_KEY_TYPE_PRI_CFI == key_type) {
                    vxlan_info->match_key[vp].flags =
                                            _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI;
                    vxlan_info->match_key[vp].match_vlan = 
                        soc_mem_field32_get(unit, mem, entry, OTAGf);
                }
            
                if (trunk) {
                    vxlan_info->match_key[vp].trunk_id = tgid;
                    vxlan_info->match_key[vp].modid = -1;
                    vxlan_info->match_key[vp].index = 0;
                } else {
                    vxlan_info->match_key[vp].port = port_num;
                    vxlan_info->match_key[vp].modid = mod_id;
                    vxlan_info->match_key[vp].trunk_id = -1;
                    vxlan_info->match_key[vp].index = 0;
                }
            }
            bcm_td2_vxlan_port_match_count_adjust(unit, vp, 1);
        }
    }

    /* Recovery based on entry in MPLS tbl */

    mem = MPLS_ENTRYm;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        mem = MPLS_ENTRY_SINGLEm;
        d = 1;
    }
#endif
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    /* 
     * foreach valid entry with key_type TUNNEL/VFI of MPLS_ENTRY table
     *   recover the tunnel sip info and flag of each SVP used by VXLAN.
     * 
     * Sets the following fields:
     *   
     *   VXLAN_INFO[unit].match_key[SVP].flags
     *   VXLAN_INFO[unit].match_key[SVP].match_tunnel_index
 */

    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    mpls_buf = soc_cm_salloc(unit, buf_size, "MPLS_ENTRY buffer");
    if (NULL == mpls_buf) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, mpls_buf);
    if (SOC_FAILURE(rv)) {
        goto clean_up;
    } 
    
    for (i = index_min; i <= index_max; i++) {
        entry = soc_mem_table_idx_to_pointer(unit, mem, void *, mpls_buf, i);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            if (soc_mem_field32_get(unit, mem, entry, BASE_VALIDf) != 1) {
                continue;
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem, entry, VALIDf) == 0) {
                continue;
            }
        }

        /* Looking for entries of type vxlan only */
        key_type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);
        if ((key_type != _BCM_VXLAN_KEY_TYPE_TUNNEL) &&
            (key_type != _BCM_VXLAN_KEY_TYPE_VNID_VFI) && 
            (key_type != _BCM_VXLAN_KEY_TYPE_TUNNEL_VNID_VFI)) {
            continue;
        }    

        vp = soc_mem_field32_get(unit, mem, entry, BCM_VXLAN_SIP_FIELD(d, SVPf));
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_VNID;
        }
        
    }

    /* Tunnel initiator hash and ref-count recovery */
    rv = _bcm_td2_vxlan_tunnel_initiator_reinit(unit);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    } 

    /* Recover L2 scache */
    rv = _bcm_td2_vxlan_wb_recover(unit);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    rv = _bcm_td2_vxlan_vp_based_vnid_reinit(unit);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    mem = SOURCE_VPm;
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    for (i = index_min; i <= index_max; i++) {
        if (_bcm_vp_used_get(unit, i, _bcmVpTypeVxlan)) {
            rv = _bcm_td2_vxlan_source_vp_tpid_recover(unit, i);
        }
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }
    }

clean_up:
    if (NULL != trunk_buf) {
        soc_cm_sfree(unit, trunk_buf);
    }
    if (NULL != xlate_buf) {
        soc_cm_sfree(unit, xlate_buf);
    }
    if (NULL != mpls_buf) {
        soc_cm_sfree(unit, mpls_buf);
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_td2_vxlan_init
 * Purpose:
 *      Initialize the VXLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


int
bcm_td2_vxlan_init(int unit)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int i, num_tnl=0, num_vp=0;
    int rv = BCM_E_NONE;
    int num_vfi = 0;

    if (!L3_INFO(unit)->l3_initialized) {
        LOG_ERROR(BSL_LS_BCM_VXLAN,
                  (BSL_META_U(unit,
                              "L3 module must be initialized prior to VXLAN Init\n")));
        return BCM_E_CONFIG;
    }

    /* Allocate BK Info */
    BCM_IF_ERROR_RETURN(bcm_td2_vxlan_allocate_bk(unit));
    vxlan_info = VXLAN_INFO(unit);

    /*
     * allocate resources
     */
    if (vxlan_info->initialized) {
         BCM_IF_ERROR_RETURN(bcm_td2_vxlan_cleanup(unit));
         BCM_IF_ERROR_RETURN(bcm_td2_vxlan_allocate_bk(unit));
         vxlan_info = VXLAN_INFO(unit);
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    vxlan_info->match_key =
        sal_alloc(sizeof(_bcm_vxlan_match_port_info_t) * num_vp, "match_key");
    if (vxlan_info->match_key == NULL) {
        _bcm_td2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(vxlan_info->match_key, 0, 
            sizeof(_bcm_vxlan_match_port_info_t) * num_vp);
    /* Stay same after recover */
    for (i = 0; i < num_vp; i++) {
        bcm_td2_vxlan_match_clear(unit, i);
    }

    /* Create EGR_IP_TUNNEL usage bitmap */
    num_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);

    vxlan_info->vxlan_ip_tnl_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_tnl), "vxlan_ip_tnl_bitmap");
    if (vxlan_info->vxlan_ip_tnl_bitmap == NULL) {
        _bcm_td2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(vxlan_info->vxlan_ip_tnl_bitmap, 0, SHR_BITALLOCSIZE(num_tnl));

    /* Create VXLAN protection mutex. */
    vxlan_info->vxlan_mutex = sal_mutex_create("vxlan_mutex");
    if (!vxlan_info->vxlan_mutex) {
         _bcm_td2_vxlan_free_resource(unit);
         return BCM_E_MEMORY;
    }

    if (NULL == vxlan_info->vxlan_tunnel_term) {
        vxlan_info->vxlan_tunnel_term =
            sal_alloc(sizeof(_bcm_vxlan_tunnel_endpoint_t) * num_vp, "vxlan tunnel term store");
        if (vxlan_info->vxlan_tunnel_term == NULL) {
            _bcm_td2_vxlan_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(vxlan_info->vxlan_tunnel_term, 0, 
                sizeof(_bcm_vxlan_tunnel_endpoint_t) * num_vp);
    }

    if (NULL == vxlan_info->vxlan_tunnel_init) {
        vxlan_info->vxlan_tunnel_init =
            sal_alloc(sizeof(_bcm_vxlan_tunnel_endpoint_t) * num_vp, "vxlan tunnel init store");
        if (vxlan_info->vxlan_tunnel_init == NULL) {
            _bcm_td2_vxlan_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(vxlan_info->vxlan_tunnel_init, 0, 
                sizeof(_bcm_vxlan_tunnel_endpoint_t) * num_vp);
    }

    num_vfi = soc_mem_index_count(unit, VFIm);
    if (NULL == vxlan_info->vxlan_vpn_vlan) {
        vxlan_info->vxlan_vpn_vlan =
            sal_alloc(sizeof(bcm_vlan_t) * num_vfi, "vxlan vpn vlan store");
        if (vxlan_info->vxlan_vpn_vlan == NULL) {
            _bcm_td2_vxlan_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(vxlan_info->vxlan_vpn_vlan, 0, sizeof(bcm_vlan_t) * num_vfi);
    }

    if (NULL == vxlan_info->vxlan_vpn_info) {
        vxlan_info->vxlan_vpn_info =
            sal_alloc(sizeof(_bcm_vxlan_vpn_info_t) * num_vfi, "vxlan vpn info store");
        if (vxlan_info->vxlan_vpn_info == NULL) {
            _bcm_td2_vxlan_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(vxlan_info->vxlan_vpn_info, 0, sizeof(_bcm_vxlan_vpn_info_t) * num_vfi);
    }

    if (vxlan_info->vfi_vnid_map_count == NULL) {
        vxlan_info->vfi_vnid_map_count = sal_alloc(sizeof(uint32) * num_vp, "vxlan vfi vnid map count");
        if (vxlan_info->vfi_vnid_map_count == NULL) {
            _bcm_td2_vxlan_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(vxlan_info->vfi_vnid_map_count, 0, sizeof(uint32) * num_vp);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_td2_vxlan_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_td2_vxlan_free_resource(unit);
        }
    } else {
        rv = _bcm_td2_vxlan_wb_alloc(unit);    
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    /* Mark the state as initialized */
    vxlan_info->initialized = TRUE;

    return rv;
}

 /*
  * Function:
  *      _bcm_td2_vxlan_bud_loopback_enable
  * Purpose:
  *      Enable loopback for VXLAN BUD node multicast
  * Parameters:
  *   IN : unit
  * Returns:
  *      BCM_E_XXX
  */
 
STATIC int
_bcm_td2_vxlan_bud_loopback_enable(int unit)
{
    int field_num = 0;

    soc_field_t lport_fields[] = {
        VXLAN_TERMINATION_ALLOWEDf,
        V4IPMC_ENABLEf,
        VXLAN_VN_ID_LOOKUP_KEY_TYPEf,
        PORT_TYPEf,
        VXLAN_DEFAULT_SVP_ENABLEf
    };
    uint32 lport_values[] = {
        0x1,  /* VXLAN_TERMINATION_ALLOWEDf */
        0x1,  /* V4IPMC_ENABLEf */
        0x0,  /* VXLAN_VN_ID_LOOKUP_KEY_TYPEf */
        0x0,  /* PORT_TYPEf */
        0x1,  /* VXLAN_DEFAULT_SVP_ENABLEf */
    };

    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, VXLAN_DEFAULT_SVP_ENABLEf)) {
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
  *      _bcm_td2_vxlan_bud_loopback_disable
  * Purpose:
  *      Disable loopback for VXLAN BUD node multicast
  * Parameters:
  *   IN : unit
  * Returns:
  *      BCM_E_XXX
  */
 
STATIC int
_bcm_td2_vxlan_bud_loopback_disable(int unit)
{
     int field_num = 0;

     soc_field_t lport_fields[] = {
         VXLAN_TERMINATION_ALLOWEDf,
         VXLAN_VN_ID_LOOKUP_KEY_TYPEf,
         V4IPMC_ENABLEf,
         PORT_TYPEf,
         VXLAN_DEFAULT_SVP_ENABLEf
     };
     uint32 lport_values[] = {
         0x0,  /* VXLAN_TERMINATION_ALLOWEDf */
         0x0,  /* VXLAN_VN_ID_LOOKUP_KEY_TYPEf */
         0x0,  /* V4IPMC_ENABLEf */
         0x0,  /* PORT_TYPEf */
         0x0   /* VXLAN_DEFAULT_SVP_ENABLEf */
     };

     if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, PORT_TYPEf)) {
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
 *      _bcm_td2_vxlan_vpn_is_valid
 * Purpose:
 *      Find if given VXLAN VPN is Valid 
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 * Returns:
 *      BCM_E_XXXX
 */
int
_bcm_td2_vxlan_vpn_is_valid( int unit, bcm_vpn_t l2vpn)
{
    bcm_vpn_t vxlan_vpn_min=0;
    int vfi_index=-1, num_vfi=0;

    num_vfi = soc_mem_index_count(unit, VFIm);

    /* Check for Valid vpn */
    _BCM_VXLAN_VPN_SET(vxlan_vpn_min, _BCM_VPN_TYPE_VFI, 0);
    if ( l2vpn < vxlan_vpn_min || l2vpn > (vxlan_vpn_min+num_vfi-1) ) {
        return BCM_E_PARAM;
    }

    _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI,  l2vpn);

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}

 /* Function:
 *      _bcm_td2_vxlan_vpn_is_eline
 * Purpose:
 *      Find if given VXLAN VPN is ELINE
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 *      isEline  - Flag 
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      l2vpn is valid
 */

int
_bcm_td2_vxlan_vpn_is_eline( int unit, bcm_vpn_t l2vpn, uint8 *isEline)
{
    int vfi_index=-1;
    vfi_entry_t vfi_entry;

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, l2vpn));

    _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI,  l2vpn);
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    /* Set the returned VPN id */
    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        *isEline = 1;  /* ELINE */
    } else {
        *isEline = 0;  /* ELAN */
    }
    return BCM_E_NONE;
}

/* Function:
 *      _bcm_td2_vxlan_vpn_is_based_vnid
 * Purpose:
 *      Find if given VXLAN VPN is vnid basedE
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 *      vxlan_vpn_type  - type 
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      l2vpn is valid
 */ 
int
_bcm_td2_vxlan_vpn_is_tunnel_based_vnid( int unit, bcm_vpn_t l2vpn, uint8 *vxlan_vpn_type)
{
    int vfi = 0;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    if (l2vpn == BCM_VXLAN_VPN_INVALID) {
        *vxlan_vpn_type = 0;
        return BCM_E_NONE;
    }
    vxlan_info = VXLAN_INFO(unit);
    _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, l2vpn);
    if (vfi > soc_mem_index_count(unit, VFIm)) {
        return BCM_E_PARAM;
    }
    if (vxlan_info->vxlan_vpn_info[vfi].vxlan_vpn_type) {
        *vxlan_vpn_type = _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID;
    } else {
        *vxlan_vpn_type = 0;
    }
    return BCM_E_NONE;
}




 /* Function:
 *      _bcm_td2_vxlan_vp_is_eline
 * Purpose:
 *      Find if given VXLAN VP is ELINE
 * Parameters:
 *      unit     - Device Number
 *      vp   - VXLAN VP
 *      isEline  - Flag 
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      l2vpn is valid
 */

int
_bcm_td2_vxlan_vp_is_eline( int unit, int vp, uint8 *isEline)
{
    source_vp_entry_t svp;
    vfi_entry_t vfi_entry;
    int rv = BCM_E_PARAM;
    int vfi_index;

    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
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
 *      _bcm_td2_vxlan_nexthop_glp_get
 * Purpose:
 *      Get the modid, port, trunk values for a VXLAN nexthop
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_vxlan_nexthop_glp_get(int unit, int nh_idx,
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
 *      _bcm_td2_vxlan_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a VXLAN port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_vxlan_port_resolve(int unit, bcm_gport_t vxlan_port_id, 
                          bcm_if_t encap_id, bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE;
    egr_l3_next_hop_entry_t egr_nh;
    ing_dvp_table_entry_t dvp;
    int ecmp=0, nh_index=-1, nh_ecmp_index=-1, vp=-1;
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry. */
    int  idx, max_ent_count, base_idx;

    rv = _bcm_vxlan_check_init(unit);
    if (rv < 0) {
        return rv;
    }

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port_id);
    if (vp == -1) {
       return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (!ecmp) {
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_nexthop_glp_get(unit,
                                nh_index, modid, port, trunk_id));
    } else {
         /* Select the desired nhop from ECMP group - pointed by encap_id */
         nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
         BCM_IF_ERROR_RETURN(
              soc_mem_read(unit, L3_ECMP_COUNTm, MEM_BLOCK_ANY, 
                        nh_ecmp_index, hw_buf));

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

            BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_nexthop_glp_get(unit,
                                    nh_index, modid, port, trunk_id));
        } else {
            for (idx = 0; idx < max_ent_count; idx++) {
                BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                    L3_ECMPm, MEM_BLOCK_ANY, (base_idx+idx), hw_buf));
                nh_index = soc_mem_field32_get(unit,
                    L3_ECMPm, hw_buf, NEXT_HOP_INDEXf);
                BCM_IF_ERROR_RETURN (soc_mem_read(unit,
                    EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, &egr_nh));
                if (encap_id == soc_mem_field32_get(unit,
                        EGR_L3_NEXT_HOPm, &egr_nh, INTF_NUMf)) {
                    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_nexthop_glp_get(unit,
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
 *      _bcm_td2_vxlan_port_nh_cnt_dec
 * Purpose:
 *      Decrease vxlan port's nexthop count.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      vp    - (IN) Virtual port number.
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_td2_vxlan_port_nh_cnt_dec(int unit, int vp)
{
    int nh_ecmp_index = -1;
    ing_dvp_table_entry_t dvp;
    uint32  flags = 0;
    int  ref_count = 0;
    int ecmp = 0;

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
 *      _bcm_td2_vxlan_port_cnt_update
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
_bcm_td2_vxlan_port_cnt_update(int unit, bcm_gport_t gport,
        int vp, int incr_decr_flag)
{
    int mod_out=-1, port_out=-1, tgid_out=-1, vp_out=-1;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int idx=-1;
    int mod_local=-1;
    _bcm_port_info_t *port_info;
    uint32 port_flags;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_gport_t tmp_gport;
#endif

    BCM_IF_ERROR_RETURN(
       _bcm_td2_vxlan_port_resolve(unit, gport, -1, &mod_out,
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
#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            (_bcm_xgs5_subport_coe_mod_port_local(unit, mod_out, port_out))) {
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_mod_port_physical_port_get(
                    unit, mod_out, port_out, &port_out));
            mod_local = 1;
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, mod_out, &mod_local));
            if (mod_local && soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (!mod_local && soc_feature(unit, soc_feature_multi_next_hops_on_port) &&
                _bcm_esw_is_multi_nexthop_mod_port(unit, mod_out, port_out)) {
                BCM_GPORT_MODPORT_SET(tmp_gport, mod_out, port_out);
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_modport_local_get(unit, tmp_gport, &port_out));
                mod_local = 1;
            }
#endif
        }
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

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _bcm_td3_vxlan_vxlate_entry_assemble
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN)  configuration entry info
 *      ment  -  (IN/OUT) point of mem entry
 * Returns:
 *      BCM_E_XXX
 */
 STATIC void
_bcm_td3_vxlan_vxlate_entry_assemble(int unit, _bcm_td3_vxlan_vxlate_entry_t info, uint32* ment)
{
    soc_mem_t mmem = VLAN_XLATE_1_DOUBLEm;
    uint32 profile_idx = 0;
    
    soc_mem_field32_set(unit, mmem, ment, BASE_VALID_0f, 3);
    soc_mem_field32_set(unit, mmem, ment, BASE_VALID_1f, 7);
    soc_mem_field32_set(unit, mmem, ment, DATA_TYPEf, info.dtype);
    soc_mem_field32_set(unit, mmem, ment, KEY_TYPEf, info.ktype);
    soc_mem_field32_set(unit, mmem, ment, VXLAN_FLEX__VFI_ACTION_SETf, info.vfi);
    /* action profile */
    if (info.vlan != 0) {
        soc_mem_field32_set(unit, mmem, ment, VXLAN_FLEX__OVIDf, info.vlan);
        _bcm_td3_def_vlan_profile_get(unit, &profile_idx);
        soc_mem_field32_set(unit,mmem, ment,
                    VXLAN_FLEX__ING_TAG_ACTION_PROFILE_IDXf,profile_idx);
    }

}
#endif

/*
 * Function:
 *      bcm_td2_vxlan_vpn_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */

int 
bcm_td2_vxlan_vpn_create(int unit, bcm_vxlan_vpn_config_t *info)
{
    int rv = BCM_E_PARAM;
    vfi_entry_t vfi_entry;
    int vfi_index=-1;
    int bc_group=0, umc_group=0, uuc_group=0;
    int bc_group_type=0, umc_group_type=0, uuc_group_type=0;
    uint32 vnid = 0;
    uint32 ment[SOC_MAX_MEM_WORDS];
    soc_mem_t mmem = MPLS_ENTRYm;
    soc_mem_t vxlate_mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];
    int tpid_index = -1;
    int action_present=0, action_not_present=0;
    uint8 vpn_alloc_flag=0;
    uint8 vnid_alloc_flag = 0;
    int proto_pkt_idx = 0;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int d = 0; /*vxlan decoupled mode*/

    sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    sal_memset(vxlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
        d = 1;
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
    }

    vxlan_info = VXLAN_INFO(unit);
    /*Allocate VFI*/
    if (info->flags & BCM_VXLAN_VPN_REPLACE) {
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, info->vpn));
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, info->vpn);
    } else if (info->flags & BCM_VXLAN_VPN_WITH_ID) {
        rv = _bcm_td2_vxlan_vpn_is_valid(unit, info->vpn);
        if (BCM_E_NONE == rv) {
            return BCM_E_EXISTS;
        } else if (BCM_E_NOT_FOUND != rv) {
            return rv;
        }
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, info->vpn);
        BCM_IF_ERROR_RETURN(_bcm_vfi_alloc_with_id(unit, VFIm, _bcmVfiTypeVxlan, vfi_index));
        vpn_alloc_flag = 1;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_vfi_alloc(unit, VFIm, _bcmVfiTypeVxlan, &vfi_index));
        _BCM_VXLAN_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi_index);
        vpn_alloc_flag = 1;
    }

    /*Initial and configure VFI*/
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));
    if (info->flags & BCM_VXLAN_VPN_ELINE) {
        soc_VFIm_field32_set(unit, &vfi_entry, PT2PT_ENf, 0x1);
    } else if (info->flags & BCM_VXLAN_VPN_ELAN) {
        /* Check that the groups are valid. */
        bc_group_type = _BCM_MULTICAST_TYPE_GET(info->broadcast_group);
        bc_group = _BCM_MULTICAST_ID_GET(info->broadcast_group);
        umc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_multicast_group);
        umc_group = _BCM_MULTICAST_ID_GET(info->unknown_multicast_group);
        uuc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_unicast_group);
        uuc_group = _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);

        if ((bc_group_type != _BCM_MULTICAST_TYPE_VXLAN) ||
            (umc_group_type != _BCM_MULTICAST_TYPE_VXLAN) ||
            (uuc_group_type != _BCM_MULTICAST_TYPE_VXLAN) ||
            (bc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (umc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (uuc_group >= soc_mem_index_count(unit, L3_IPMCm))) {
            rv = BCM_E_PARAM;
            _BCM_VXLAN_CLEANUP(rv)
        }

        /* Commit the entry to HW */
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
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 profile_idx = 0;
            rv = _bcm_td3_def_vfi_profile_add(unit,
                info->flags & BCM_VXLAN_VPN_ELINE, &profile_idx);
            if (BCM_E_NONE == rv) {
                soc_VFIm_field32_set(unit, &vfi_entry, VFI_PROFILE_PTRf, profile_idx);
            }
            _BCM_VXLAN_CLEANUP(rv)
        }
#endif
    }

    /* Configure protocol packet control */
    rv = _bcm_xgs3_protocol_packet_actions_validate(unit, &info->protocol_pkt);
    _BCM_VXLAN_CLEANUP(rv)
    proto_pkt_idx = soc_VFIm_field32_get(unit, &vfi_entry, PROTOCOL_PKT_INDEXf);
    rv = _bcm_xgs3_protocol_pkt_ctrl_set(unit, proto_pkt_idx, &info->protocol_pkt, &proto_pkt_idx);
    _BCM_VXLAN_CLEANUP(rv)
    soc_VFIm_field32_set(unit, &vfi_entry, PROTOCOL_PKT_INDEXf, proto_pkt_idx);
    /*Write VFI to ASIC*/
    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
    _BCM_VXLAN_CLEANUP(rv)

    if (info->flags & BCM_VXLAN_VPN_WITH_VPNID) {
        if (info->vnid > 0xFFFFFF || 
            (info->flags & BCM_VXLAN_VPN_TUNNEL_BASED_VNID)) {
           rv = BCM_E_PARAM;
           _BCM_VXLAN_CLEANUP(rv)
        }

        /* Configure Ingress VNID */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
             _bcm_td3_vxlan_vxlate_entry_t ent;

             mmem = VLAN_XLATE_1_DOUBLEm;
             sal_memset(&ent, 0, sizeof(_bcm_td3_vxlan_vxlate_entry_t));
             ent.dtype = _BCM_VXLAN_DATA_TYPE_VNID_VFI_FLEX;
             ent.ktype = _BCM_VXLAN_KEY_TYPE_VNID_VFI_FLEX;
             ent.vfi = vfi_index;
             if (info->default_vlan > BCM_VLAN_MAX) {
                  rv = BCM_E_PARAM;
                  _BCM_VXLAN_CLEANUP(rv)
             }
             ent.vlan = info->default_vlan;
            _bcm_td3_vxlan_vxlate_entry_assemble(unit, ent, ment);
        } else
#endif
        {
            soc_mem_field32_set(unit, mmem, &ment, VALIDf, 1);
            soc_mem_field32_set(unit, mmem, &ment, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_VNID_VFI);
            soc_mem_field32_set(unit, mmem, &ment, VXLAN_VN_ID__VFIf, vfi_index);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                if (info->vlan > BCM_VLAN_MAX) {
                    rv = BCM_E_PARAM;
                    _BCM_VXLAN_CLEANUP(rv)
                }
                soc_mem_field32_set(unit, mmem, &ment, VXLAN_VN_ID__OVIDf, info->vlan);
            }
#endif
        }

        if (info->flags & BCM_VXLAN_VPN_REPLACE) {
            /*Delete old match vnid entry*/
            rv = _bcm_td2_vxlan_egr_xlate_entry_get(unit, vfi_index, -1, vxlate_entry);
            if (rv == BCM_E_NONE) {
                vnid = soc_mem_field32_get(unit, vxlate_mem,
                            vxlate_entry, (d ? VXLAN_VFI_FLEX__VNIDf : VXLAN_VFI__VN_IDf));
                soc_mem_field32_set(unit, mmem, &ment, (d ? VXLAN_FLEX__VNIDf : VXLAN_VN_ID__VN_IDf),
                                     vnid);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                    soc_mem_field32_set(unit, mmem, &ment, VXLAN_VN_ID__OVIDf,
                                        vxlan_info->vxlan_vpn_vlan[vfi_index]);
                }
#endif
                rv = soc_mem_delete(unit, mmem, MEM_BLOCK_ALL, &ment);
                _BCM_VXLAN_CLEANUP(rv)
            } else if (rv != BCM_E_NOT_FOUND) {
                _BCM_VXLAN_CLEANUP(rv)
            }
        }
        soc_mem_field32_set(unit, mmem, &ment,  (d ? VXLAN_FLEX__VNIDf : VXLAN_VN_ID__VN_IDf),
                                info->vnid);
        rv = soc_mem_insert(unit, mmem, MEM_BLOCK_ALL, &ment);
        _BCM_VXLAN_CLEANUP(rv)
        vnid_alloc_flag = 1;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
            vxlan_info->vxlan_vpn_vlan[vfi_index] = info->vlan;
        }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            /*on td3, soc_feature_vrf_aware_vxlan_termination disabled*/
            vxlan_info->vxlan_vpn_vlan[vfi_index] = info->default_vlan;
        }
#endif
        if (info->flags & BCM_VXLAN_VPN_REPLACE) {
            soc_field_t tpid_fld;
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            /*TAP process, delete tpid*/
                int profile_index = -1;

                tpid_fld = VXLAN_VFI_FLEX__OUTER_TPID_INDEXf;
                profile_index = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                                   VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf);
                (void)_bcm_td3_sd_tag_action_get(unit, profile_index, &action_present,
                               &action_not_present);
            } else
#endif
            {
                tpid_fld = VXLAN_VFI__SD_TAG_TPID_INDEXf;
                /*If old tpid exist, delete it*/
                action_present = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                                                    VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf);
                action_not_present = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                                                    VXLAN_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf);
            }

            if (1 == action_present || 4 == action_present ||
                7 == action_present || 1 == action_not_present) {
                tpid_index = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                                                        tpid_fld);
                rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
                if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                    _BCM_VXLAN_CLEANUP(rv)
                }            
            }
 
            rv = soc_mem_delete(unit, vxlate_mem, MEM_BLOCK_ALL, vxlate_entry);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                _BCM_VXLAN_CLEANUP(rv)
            }
        }

        /* Configure Egress VNID */
        sal_memset(vxlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            soc_mem_field32_set(unit, vxlate_mem, vxlate_entry, BASE_VALIDf, 1);
            soc_mem_field32_set(unit, vxlate_mem, vxlate_entry,
                                    DATA_TYPEf, _BCM_VXLAN_DATA_TYPE_LOOKUP_VFI);
            soc_mem_field32_set(unit, vxlate_mem, vxlate_entry,
                                    KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI);
        } else
#endif
        {
            soc_mem_field32_set(unit, vxlate_mem, vxlate_entry, VALIDf, 0x1);
            soc_mem_field32_set(unit, vxlate_mem, vxlate_entry,
                                ENTRY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI);  		
        }
        soc_mem_field32_set(unit, vxlate_mem, vxlate_entry, BCM_VXLAN_VFI_FIELD(d,VFIf), vfi_index);
        soc_mem_field32_set(unit, vxlate_mem, vxlate_entry, (d ? VXLAN_VFI_FLEX__VNIDf : VXLAN_VFI__VN_IDf), info->vnid);

        /* Configure EGR_VLAN_XLATE - SD_TAG setting */
        if (info->flags & BCM_VXLAN_VPN_SERVICE_TAGGED) {
            rv = _bcm_td2_vxlan_sd_tag_set(unit, info, NULL, NULL, vxlate_entry, &tpid_index);
            _BCM_VXLAN_CLEANUP(rv)
        }

        rv = soc_mem_insert(unit, vxlate_mem, MEM_BLOCK_ALL, vxlate_entry);
        _BCM_VXLAN_CLEANUP(rv)
        vxlan_info->vxlan_vpn_info[vfi_index].vxlan_vpn_type = 0;        
    } else {

        /* Reset match vnid table */
        rv = _bcm_td2_vxlan_egr_xlate_entry_get(unit, vfi_index, -1, vxlate_entry);
        if (rv == BCM_E_NONE) {
            vnid = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                              (d ? VXLAN_VFI_FLEX__VNIDf : VXLAN_VFI__VN_IDf));
            /* Delete mpls_entry */
            rv = _bcm_td2_vxlan_match_vnid_entry_reset(unit, vnid,
                                        vxlan_info->vxlan_vpn_vlan[vfi_index]);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                return rv;
            }
            /* Delete egr_vxlate entry */
            rv = _bcm_td2_vxlan_egr_xlate_entry_reset(unit, info->vpn);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                return rv;
            }
        } else if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }
        if (info->flags & BCM_VXLAN_VPN_TUNNEL_BASED_VNID) {
            vxlan_info->vxlan_vpn_info[vfi_index].vxlan_vpn_type = _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID;
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                vxlan_info->vxlan_vpn_vlan[vfi_index] = info->default_vlan;
            }
#endif
        }
    }

    return BCM_E_NONE;

cleanup:
    if (tpid_index != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }
    if (vnid_alloc_flag) {
        soc_mem_delete(unit, mmem, MEM_BLOCK_ALL, &ment);
    }
    if (vpn_alloc_flag) {
        (void) _bcm_vfi_free(unit, _bcmVfiTypeVxlan, vfi_index);
    }
    return rv;
}

 /* Function:
 *      bcm_td2_vxlan_vpn_vnid_get
 * Purpose:
 *      Get VNID per VPN instance
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 *      info     - VXLAN VPN Config
 * Returns:
 *      BCM_E_XXXX
 */

STATIC int
_bcm_td2_vxlan_egr_xlate_entry_get(int unit, int vfi, int vp, uint32 *vxlate_entry)
{
    soc_mem_t vxlate_mem;
    uint32 key_vxlate[SOC_MAX_MEM_WORDS];
    int index=0;

    sal_memset(key_vxlate, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;

        soc_mem_field32_set(unit, vxlate_mem, key_vxlate, BASE_VALIDf, 1);
        if (vp != -1) {
            soc_mem_field32_set(unit, vxlate_mem, key_vxlate, VXLAN_VFI_FLEX__DVPf, vp);
            soc_mem_field32_set(unit, vxlate_mem, key_vxlate,
                                KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP);
            soc_mem_field32_set(unit, vxlate_mem, key_vxlate,
                                DATA_TYPEf, _BCM_VXLAN_DATA_TYPE_LOOKUP_VFI);
        } else {
            soc_mem_field32_set(unit, vxlate_mem, key_vxlate,
                                KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI);
            soc_mem_field32_set(unit, vxlate_mem, key_vxlate,
                                DATA_TYPEf, _BCM_VXLAN_DATA_TYPE_LOOKUP_VFI);
        }
        soc_mem_field32_set(unit, vxlate_mem, key_vxlate, VXLAN_VFI_FLEX__VFIf, vfi);
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
        soc_mem_field32_set(unit, vxlate_mem, key_vxlate, VALIDf, 0x1);
        soc_mem_field32_set(unit, vxlate_mem, key_vxlate, ENTRY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI);
        if (vp != -1) {
            soc_mem_field32_set(unit, vxlate_mem, key_vxlate, VXLAN_VFI__DVPf, vp);
            soc_mem_field32_set(unit, vxlate_mem, key_vxlate,
                                ENTRY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP);
        } else {
            soc_mem_field32_set(unit, vxlate_mem, key_vxlate,
                                ENTRY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI);
        }
        soc_mem_field32_set(unit, vxlate_mem, key_vxlate, VXLAN_VFI__VFIf, vfi);
    }

    return soc_mem_search(unit, vxlate_mem, MEM_BLOCK_ANY,
                        &index, key_vxlate, vxlate_entry, 0);
}


/*
 * Function:
 *      bcm_td2_vxlan_vpn_destroy
 * Purpose:
 *      Delete a VPN instance
 * Parameters:
 *      unit - Device Number
 *      vpn - VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2_vxlan_vpn_destroy(int unit, bcm_vpn_t l2vpn)
{
    int vfi = 0;
    vfi_entry_t vfi_entry;
    uint8 isEline = 0;
    uint32 vnid = 0;
    uint32 stat_counter_id;
    int num_ctr = 0; 
    int ref_count;
    int index;
    int rv = 0;
    soc_mem_t vxlate_mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
    int d = 0; /*vxlan decoupled mode*/

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        d = 1;
        vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
    }

    /* Get vfi index */
    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_eline(unit, l2vpn, &isEline));
    if (isEline == 0x1 ) {
        _BCM_VXLAN_VPN_GET(vfi, _BCM_VXLAN_VPN_TYPE_ELINE, l2vpn);
    } else if (isEline == 0x0 ) {
        _BCM_VXLAN_VPN_GET(vfi, _BCM_VXLAN_VPN_TYPE_ELAN, l2vpn);
    }
    if (!vxlan_info->vxlan_vpn_info[vfi].vxlan_vpn_type) {
        /* Reset match vnid table */
        rv = _bcm_td2_vxlan_egr_xlate_entry_get(unit, vfi, -1, vxlate_entry);
        if (rv == BCM_E_NONE) {         
            vnid = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry, (d ? VXLAN_VFI_FLEX__VNIDf : VXLAN_VFI__VN_IDf));
            /* Delete mpls_entry */
            rv = _bcm_td2_vxlan_match_vnid_entry_reset(
                     unit, vnid, vxlan_info->vxlan_vpn_vlan[vfi]);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                return rv;    
            }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                vxlan_info->vxlan_vpn_vlan[vfi] = 0;
            }
#endif
            /* Delete egr_vxlate entry */
            rv = _bcm_td2_vxlan_egr_xlate_entry_reset(unit, l2vpn);            
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                return rv;    
            }            
        } else if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }

    /* Delete all VXLAN ports on this VPN */
    BCM_IF_ERROR_RETURN(bcm_td2_vxlan_port_delete_all(unit, l2vpn));

    /* Check counters before delete vpn */
    if (bcm_esw_vxlan_stat_id_get(unit, BCM_GPORT_INVALID, l2vpn,
                   bcmVxlanOutPackets, &stat_counter_id) == BCM_E_NONE) { 
        num_ctr++;
    } 
    if (bcm_esw_vxlan_stat_id_get(unit, BCM_GPORT_INVALID, l2vpn,
                   bcmVxlanInPackets, &stat_counter_id) == BCM_E_NONE) {
        num_ctr++;
    }

    if (num_ctr != 0) {
        BCM_IF_ERROR_RETURN(
           bcm_esw_vxlan_stat_detach(unit, BCM_GPORT_INVALID, l2vpn));  
    }

    /* Delete protocol packet control */
    sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry));
    index = soc_VFIm_field32_get(unit, &vfi_entry, PROTOCOL_PKT_INDEXf);
    BCM_IF_ERROR_RETURN(
        _bcm_prot_pkt_ctrl_ref_count_get(unit, index, &ref_count));
    if ((ref_count > 0) && index) {
        BCM_IF_ERROR_RETURN(_bcm_prot_pkt_ctrl_delete(unit, index));
    }

    /* Reset VFI table */
    (void)_bcm_vfi_free(unit, _bcmVfiTypeVxlan, vfi);
    vxlan_info->vxlan_vpn_info[vfi].vxlan_vpn_type = 0;
    return BCM_E_NONE;
}

 /* Function:
 *      bcm_td2_vxlan_vpn_id_destroy_all
 * Purpose:
 *      Delete all L2-VPN instances
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_td2_vxlan_vpn_destroy_all(int unit)
{
    int num_vfi = 0, idx = 0;
    bcm_vpn_t l2vpn = 0;

    /* Destroy all VXLAN VPNs */
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (idx = 0; idx < num_vfi; idx++) {
        if (_bcm_vfi_used_get(unit, idx, _bcmVfiTypeVxlan)) {
            _BCM_VXLAN_VPN_SET(l2vpn, _BCM_VPN_TYPE_VFI, idx);
            BCM_IF_ERROR_RETURN(bcm_td2_vxlan_vpn_destroy(unit, l2vpn));
        }
    }
    return BCM_E_NONE;
}

 /* Function:
 *      bcm_td2_vxlan_vpn_get
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
bcm_td2_vxlan_vpn_get( int unit, bcm_vpn_t l2vpn, bcm_vxlan_vpn_config_t *info)
{
    int vfi_index = -1;
    vfi_entry_t vfi_entry;
    uint8 isEline=0;
    int rv = BCM_E_NONE;
    int proto_pkt_inx;
    soc_mem_t vxlate_mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];
    int d = 0; /*vxlan decoupled mode*/

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        d = 1;
        vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
    }

    /*Get vfi table*/
    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_eline(unit, l2vpn, &isEline));
    if (isEline == 0x1 ) {
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VXLAN_VPN_TYPE_ELINE, l2vpn);
    } else if (isEline == 0x0 ) {
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VXLAN_VPN_TYPE_ELAN, l2vpn);
    }
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    /* Get info associated with vfi table */
    if (isEline) {
        info->flags =  BCM_VXLAN_VPN_ELINE;
    } else {
         info->flags =  BCM_VXLAN_VPN_ELAN;

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_generic_dest)) {
            uint32 dt, dv;

            dv = soc_mem_field32_dest_get(unit, VFIm,
                        &vfi_entry, BC_DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_IPMC) {
                _BCM_MULTICAST_GROUP_SET(info->broadcast_group,
                                    _BCM_MULTICAST_TYPE_VXLAN, dv);
            } else {
                return BCM_E_INTERNAL;
            }

            dv = soc_mem_field32_dest_get(unit, VFIm,
                        &vfi_entry, UUC_DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_IPMC) {
                _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group,
                                        _BCM_MULTICAST_TYPE_VXLAN, dv);
            } else {
                return BCM_E_INTERNAL;
            }

            dv = soc_mem_field32_dest_get(unit, VFIm,
                        &vfi_entry, UMC_DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_IPMC) {
                _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group,
                                            _BCM_MULTICAST_TYPE_VXLAN, dv);
            } else {
                return BCM_E_INTERNAL;
            }
        } else
#endif
        {
            _BCM_MULTICAST_GROUP_SET(info->broadcast_group,
                                _BCM_MULTICAST_TYPE_VXLAN,
                 soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf));
            _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group,
                                _BCM_MULTICAST_TYPE_VXLAN,
                 soc_VFIm_field32_get(unit, &vfi_entry, UUC_INDEXf));
            _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group,
                                _BCM_MULTICAST_TYPE_VXLAN,
                 soc_VFIm_field32_get(unit, &vfi_entry, UMC_INDEXf));
        }
    }
    _BCM_VXLAN_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi_index);
    /*Get protocol packet control*/
    proto_pkt_inx = soc_VFIm_field32_get(unit, &vfi_entry, PROTOCOL_PKT_INDEXf);
    BCM_IF_ERROR_RETURN(_bcm_xgs3_protocol_pkt_ctrl_get(unit,proto_pkt_inx, &info->protocol_pkt));

    /* Get VNID */
    rv = _bcm_td2_vxlan_egr_xlate_entry_get(unit, vfi_index, -1, vxlate_entry);
    if (rv == BCM_E_NONE) {
        info->flags |= BCM_VXLAN_VPN_WITH_VPNID;
        info->vnid = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,(d ? VXLAN_VFI_FLEX__VNIDf : VXLAN_VFI__VN_IDf));
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
            info->vlan = VXLAN_INFO(unit)->vxlan_vpn_vlan[vfi_index];
        }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            info->default_vlan = VXLAN_INFO(unit)->vxlan_vpn_vlan[vfi_index];
        }
#endif
        /* Get SDTAG settings */
        (void)_bcm_td2_vxlan_sd_tag_get( unit, info, NULL, NULL, vxlate_entry, 1);
    } else if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }
    if (VXLAN_INFO(unit)->vxlan_vpn_info[vfi_index].vxlan_vpn_type) {
        info->flags |= BCM_VXLAN_VPN_TUNNEL_BASED_VNID;
    }
    return rv;
}

 /* Function:
 *      _bcm_td2_vxlan_vpn_get
 * Purpose:
 *      Get L2-VPN instance for VXLAN
 * Parameters:
 *      unit     - Device Number
 *      vfi_index   - vfi_index 
 *      vid     (OUT) VLAN Id (l2vpn id)

 * Returns:
 *      BCM_E_XXXX
 */
int 
_bcm_td2_vxlan_vpn_get(int unit, int vfi_index, bcm_vlan_t *vid)
{
    vfi_entry_t vfi_entry;

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        _BCM_VXLAN_VPN_SET(*vid, _BCM_VXLAN_VPN_TYPE_ELINE, vfi_index);
    } else {
        _BCM_VXLAN_VPN_SET(*vid, _BCM_VXLAN_VPN_TYPE_ELAN, vfi_index);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_vxlan_vpn_traverse
 * Purpose:
 *      Get information about a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb    - (IN)  User-provided callback
 *      info  - (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXX
 */

int bcm_td2_vxlan_vpn_traverse(int unit, bcm_vxlan_vpn_traverse_cb cb, 
                             void *user_data)
{
    int idx, num_vfi;
    int vpn;
    bcm_vxlan_vpn_config_t info;

    if (cb == NULL) {
         return BCM_E_PARAM;
    }

    /* VXLAN VPNs */
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (idx = 0; idx < num_vfi; idx++) {
         if (_bcm_vfi_used_get(unit, idx, _bcmVfiTypeVxlan)) {
              _BCM_VXLAN_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, idx);
              bcm_vxlan_vpn_config_t_init(&info);
              BCM_IF_ERROR_RETURN(bcm_td2_vxlan_vpn_get(unit, vpn, &info));
              BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
         }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_td2_vxlan_port_traverse
 * Purpose:
 *      Get information about a port instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb    - (IN)  User-provided callback
 *      info  - (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXX
 */

int bcm_td2_vxlan_port_traverse(int unit, bcm_vxlan_port_traverse_cb cb,
                             void *user_data)
{
    int idx = 0;
    int vp = 0;
    bcm_vxlan_port_t info;
    int split_horizon_port_flag = 0;
    _bcm_vp_info_t vp_info;
    int default_vp = -1;
    uint32 reg_val = 0;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    vxlan_info = VXLAN_INFO(unit);

    if (cb == NULL) {
         return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(READ_VXLAN_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        default_vp = soc_reg_field_get(unit,
                        VXLAN_DEFAULT_NETWORK_SVPr, reg_val, SVPf);
    }

    /* VXLAN VPNs */
    vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (idx = 0; idx < vp; idx++) {
        if (_bcm_vp_used_get(unit, idx, _bcmVpTypeVxlan)) {
            bcm_vxlan_port_t_init(&info);

            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, idx, &vp_info));
            if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
                split_horizon_port_flag = TRUE;
            }

            /*only for Access VP */
            if ((_bcm_vp_used_get(unit, idx, _bcmVpTypeNiv) ||
                 _bcm_vp_used_get(unit, idx, _bcmVpTypeExtender)) &&
                 (!split_horizon_port_flag)) {
                BCM_GPORT_VXLAN_PORT_ID_SET(info.vxlan_port_id, idx);
                BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
            } else if (_bcm_vp_used_get(unit, idx, _bcmVpTypeVpLag)) {
                bcm_trunk_t        tid = 0;
                bcm_trunk_member_t trunk_member;
                int                port_count = 0;
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_trunk_vp_lag_vp_to_tid(unit, idx, &tid));
                BCM_IF_ERROR_RETURN(
                    bcm_td2_vp_lag_get(unit, tid, NULL, 1,
                                       &trunk_member, &port_count));
                if (BCM_GPORT_IS_NIV_PORT(trunk_member.gport) ||
                    BCM_GPORT_IS_EXTENDER_PORT(trunk_member.gport)) {
                    BCM_GPORT_TRUNK_SET(info.vxlan_port_id, tid);
                    BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
                } else {
                    int vfi;
                    uint32 tunnel_sip;
                    uint32 tunnel_index;
                    int tunnel_based_vpn_get = 0;
                    tunnel_index = vxlan_info->match_key[idx].match_tunnel_index;
                    tunnel_sip = vxlan_info->vxlan_tunnel_term[tunnel_index].sip;
                    vfi = vxlan_info->match_key[idx].vfi;
                    if (vxlan_info->vxlan_vpn_info[vfi].vxlan_vpn_type == 
                        _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID && 
                        tunnel_sip == vxlan_info->vxlan_vpn_info[vfi].sip) {
                        int l2vpn;
                        _BCM_VXLAN_VPN_SET(l2vpn, _BCM_VPN_TYPE_VFI, vfi);
                        BCM_IF_ERROR_RETURN(
                            _bcm_td2_vxlan_port_get(unit, l2vpn, idx, &info));
                        BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
                        tunnel_based_vpn_get = 1;
                    }
                    if (!tunnel_based_vpn_get) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_td2_vxlan_port_get(unit, BCM_VXLAN_VPN_INVALID, idx, &info));
                        BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
                    }
                }
            } else {

                if (idx == default_vp) {
                    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_default_port_get(unit, 
                                          idx, &info));
                    BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
                } else {
                    int vfi;
                    uint32 tunnel_sip;
                    uint32 tunnel_index;
                    int tunnel_based_vpn_get = 0;
                    tunnel_index = vxlan_info->match_key[idx].match_tunnel_index;
                    tunnel_sip = vxlan_info->vxlan_tunnel_term[tunnel_index].sip;
                    vfi = vxlan_info->match_key[idx].vfi;
                    if (vxlan_info->vxlan_vpn_info[vfi].vxlan_vpn_type == 
                        _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID && 
                        tunnel_sip == vxlan_info->vxlan_vpn_info[vfi].sip) {
                        int l2vpn;
                        _BCM_VXLAN_VPN_SET(l2vpn, _BCM_VPN_TYPE_VFI, vfi);
                        BCM_IF_ERROR_RETURN(
                            _bcm_td2_vxlan_port_get(unit, l2vpn, idx, &info));
                        BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
                        tunnel_based_vpn_get = 1;
                    }
                    if (!tunnel_based_vpn_get) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_td2_vxlan_port_get(unit, BCM_VXLAN_VPN_INVALID, idx, &info));
                        BCM_IF_ERROR_RETURN(cb(unit, &info, user_data));
                    }
                }
            }
            
        }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *          _bcm_td2_vxlan_tunnel_terminate_entry_key_set
 * Purpose:
 *          Set VXLAN Tunnel Terminate entry key set
 * Parameters:
 *  IN :  Unit
 *  IN :  tnl_info
 *  IN(OUT) : tr_ent VLAN_XLATE entry pointer
 *  IN : clean_flag
 * Returns: BCM_E_XXX
 */

STATIC void
_bcm_td2_vxlan_tunnel_terminate_entry_key_set(int unit,
    bcm_tunnel_terminator_t *tnl_info, uint32 *tr_ent, uint8 out_mode, int clean_flag)
{
    soc_mem_t mem;
    int d = 0; /*vxlan decoupled mode*/
    if (clean_flag) {
         sal_memset(tr_ent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        mem = MPLS_ENTRY_SINGLEm;
        d =1;
        soc_mem_field32_set(unit, mem, tr_ent, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, mem, tr_ent, DATA_TYPEf,
                                _BCM_VXLAN_DATA_TYPE_LOOKUP_DIP_IN_MPLS_ENTRY);
        soc_mem_field32_set(unit, mem, tr_ent, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP_IN_MPLS_ENTRY);
        soc_mem_field32_set(unit, mem, tr_ent, VXLAN_FLEX_IPV4_DIP__TERM_TUNNELf, 1);
        if (tnl_info->flags & BCM_TUNNEL_TERM_L3_IIF_MISMATCH_TO_CPU) { 
            soc_mem_field32_set(unit, mem, tr_ent,
                VXLAN_FLEX_IPV4_DIP__EXPECTED_L3_IIF_MISMATCH_TO_CPUf, 1);
        }
        if (tnl_info->flags & BCM_TUNNEL_TERM_L3_IIF_MISMATCH_DROP) { 
            soc_mem_field32_set(unit, mem, tr_ent,
                VXLAN_FLEX_IPV4_DIP__EXPECTED_L3_IIF_MISMATCH_DROPf, 1);
        }
        if ((tnl_info->tunnel_if == BCM_IF_INVALID) ||
            (tnl_info->tunnel_if > BCM_VLAN_MAX) ) {
            soc_mem_field32_set(unit, mem, tr_ent,
                VXLAN_FLEX_IPV4_DIP__EXPECTED_L3_IIFf, 0);
        } else {
            soc_mem_field32_set(unit, mem, tr_ent,
                VXLAN_FLEX_IPV4_DIP__EXPECTED_L3_IIFf, tnl_info->tunnel_if);
        }
    } else
#endif
    {
        mem = VLAN_XLATEm;
        soc_mem_field32_set(unit, mem, tr_ent, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, tr_ent, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
    }
    soc_mem_field32_set(unit, mem, tr_ent, BCM_VXLAN_DIP_FIELD(d, DIPf), tnl_info->dip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
        soc_mem_field32_set(unit, mem, tr_ent, VXLAN_DIP__OVIDf, tnl_info->vlan);
    }
#endif
    if (out_mode == _BCM_VXLAN_MULTICAST_BUD) {
        soc_mem_field32_set(unit, mem, tr_ent,
            BCM_VXLAN_DIP_FIELD(d,NETWORK_RECEIVERS_PRESENTf), 0x1);
        _bcm_td2_vxlan_bud_loopback_enable(unit);
    } else if (out_mode == _BCM_VXLAN_MULTICAST_LEAF) {
        soc_mem_field32_set(unit, mem, tr_ent,
            BCM_VXLAN_DIP_FIELD(d,NETWORK_RECEIVERS_PRESENTf), 0x0);
    }

    soc_mem_field32_set(unit, mem, tr_ent, BCM_VXLAN_DIP_FIELD(d,IGNORE_UDP_CHECKSUMf),
        (tnl_info->flags & BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE) ? 0x0 : 0x1);

    if (tnl_info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) {
        soc_mem_field32_set(unit, mem, tr_ent,
            BCM_VXLAN_DIP_FIELD(d,USE_OUTER_HEADER_PHBf), 0x1);
    } else if (tnl_info->flags & BCM_TUNNEL_TERM_USE_OUTER_PCP) {
        soc_mem_field32_set(unit, mem, tr_ent,
            BCM_VXLAN_DIP_FIELD(d,USE_OUTER_HEADER_PHBf), 0x2);
    }
}

/*
 * Function:
 *          _bcm_td2_vxlan_tunnel_terminator_reference_count
 * Purpose:
 *          Find VXLAN Tunnel terminator DIP reference count
 * Returns: BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_tunnel_terminator_reference_count(int unit,
                                                 bcm_ip_t dest_ip_addr,
                                                 int threshold_count)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int tunnel_idx=0, num_vp=0, ref_count=0;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    vxlan_info = VXLAN_INFO(unit);

      for (tunnel_idx=0; tunnel_idx< num_vp; tunnel_idx++) {
          if (vxlan_info->vxlan_tunnel_term[tunnel_idx].dip == dest_ip_addr) {
              ref_count ++;
              if (ref_count == threshold_count) {
                  break;
              }
          }
      }
    return ref_count;
}

/*
 * Function:
 *          _bcm_td2_vxlan_multicast_tunnel_state_set
 * Purpose:
 *          Set VXLAN Tunnel terminator Multicast State
 * Returns: BCM_E_XXX
 */


STATIC void
_bcm_td2_vxlan_multicast_tunnel_state_set (int unit, int  tunnel_idx, uint16 tunnel_state)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;

    vxlan_info = VXLAN_INFO(unit);

    if (tunnel_idx != -1) {
        vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state = tunnel_state;
    }
}

/*
 * Function:
 *          _bcm_td2_vxlan_tunnel_terminator_state_find
 * Purpose:
 *          Find VXLAN Tunnel terminator DIP State
 * Returns: BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_tunnel_terminator_state_find(int unit, bcm_ip_t dest_ip_addr)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int tunnel_idx=0, num_vp=0, tunnel_state=0;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    vxlan_info = VXLAN_INFO(unit);

    for (tunnel_idx=0; tunnel_idx< num_vp; tunnel_idx++) {
          if (vxlan_info->vxlan_tunnel_term[tunnel_idx].dip == dest_ip_addr) {
              if (vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state == _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF ) {
                 tunnel_state += 0;
              } else if (vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state == _BCM_VXLAN_TUNNEL_TERM_MULTICAST_BUD ) {
                 tunnel_state += 1;
                 break;
              }
          }
    }
    return tunnel_state;
}

/*
 * Function:
 *      bcm_td2_vxlan_multicast_leaf_entry_check
 * Purpose:
 *      To find whether LEAF-multicast entry exists for DIP?
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2_vxlan_multicast_leaf_entry_check(int unit,
                                         int tunnel_idx,
                                         int multicast_flag)
{
    soc_mem_t mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];
    int rv=BCM_E_NONE, index=0;
    uint16 tunnel_multicast_state = 0;
    int tunnel_mc_states = 0;
    bcm_ip_t mc_ip_addr;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    uint8 net_receiver = 0;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_vlan_t    vlan = 0;
#endif
    int d = 0; /*vxlan decoupled mode*/

    vxlan_info = VXLAN_INFO(unit);
    tunnel_multicast_state =
        vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state;
    mc_ip_addr = vxlan_info->vxlan_tunnel_term[tunnel_idx].dip;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    vlan = vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan;
#endif
    
    /* Set (SIP, DIP) based Software State */
    if (multicast_flag == BCM_VXLAN_MULTICAST_TUNNEL_STATE_BUD_ENABLE) {
        if (tunnel_multicast_state == _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF) {
             (void) _bcm_td2_vxlan_multicast_tunnel_state_set(unit, 
                           tunnel_idx, _BCM_VXLAN_TUNNEL_TERM_MULTICAST_BUD);
        }
        tunnel_mc_states = 1;
    } else  if (multicast_flag == BCM_VXLAN_MULTICAST_TUNNEL_STATE_BUD_DISABLE){
        if (tunnel_multicast_state == _BCM_VXLAN_TUNNEL_TERM_MULTICAST_BUD) {
             (void) _bcm_td2_vxlan_multicast_tunnel_state_set(unit, 
                           tunnel_idx, _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF);
        }
    }
    
     /* Set DIP-based cumulative hardware State */
    if (!tunnel_mc_states) {
        tunnel_mc_states =
            _bcm_td2_vxlan_tunnel_terminator_state_find(unit, mc_ip_addr);
    }

    sal_memset(vxlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        bcm_tunnel_terminator_t tnl_info;

        d =1;
        mem = MPLS_ENTRY_SINGLEm;
        bcm_tunnel_terminator_t_init(&tnl_info);
        tnl_info.dip = mc_ip_addr;
        _bcm_td2_vxlan_tunnel_terminate_entry_key_set(unit, &tnl_info, vxlate_entry, 0, 1);
    } else
#endif      
    {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            mem = VLAN_XLATE_1_DOUBLEm;
            soc_mem_field32_set(unit, mem, vxlate_entry, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, vxlate_entry, BASE_VALID_1f, 7);
            soc_mem_field32_set(unit, mem, vxlate_entry, DATA_TYPEf,
                                    _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
        } else
#endif
    {
        mem = VLAN_XLATEm;
        soc_mem_field32_set(unit, mem, vxlate_entry, VALIDf, 0x1);
    }

        soc_mem_field32_set(unit, mem, vxlate_entry,
            KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
        soc_mem_field32_set(unit, mem, vxlate_entry, VXLAN_DIP__DIPf, mc_ip_addr);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
        soc_mem_field32_set(unit, mem, vxlate_entry, VXLAN_DIP__OVIDf, vlan);
    }
#endif
    }
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            vxlate_entry, vxlate_entry, 0);
    
    if (rv == SOC_E_NONE) {
        net_receiver = tunnel_mc_states ? 0x1 : 0x0;

        if (net_receiver != soc_mem_field32_get(unit, mem,
            vxlate_entry, BCM_VXLAN_DIP_FIELD(d,NETWORK_RECEIVERS_PRESENTf))) {
            soc_mem_field32_set(unit, mem, vxlate_entry,
                BCM_VXLAN_DIP_FIELD(d,NETWORK_RECEIVERS_PRESENTf), net_receiver);
            if (net_receiver) {
                _bcm_td2_vxlan_bud_loopback_enable(unit);
            }
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, vxlate_entry));
        }
    }

    return BCM_E_NONE;
}

/*  
 * Function:
 *      bcm_td2_vxlan_tunnel_terminator_create
 * Purpose:
 *      Set VXLAN Tunnel terminator
 * Parameters:
 *      unit - Device Number
 *      tnl_info
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vxlan_tunnel_terminator_create(int unit, bcm_tunnel_terminator_t *tnl_info)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int index=-1, tunnel_idx=-1;
    soc_mem_t mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];
    int rv = BCM_E_NONE;

    if (tnl_info->type != bcmTunnelTypeVxlan) {
       return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
        VLAN_CHK_ID(unit, tnl_info->vlan);
    }
#endif

    /* Program tunnel id */
    if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
        if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
            return BCM_E_PARAM;
        }
        if ((0 == tnl_info->dip) || (0 == tnl_info->sip)) {
            return BCM_E_PARAM; 
        }
        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
        _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);
    } else {
        /* Only BCM_TUNNEL_TERM_TUNNEL_WITH_ID supported */
        /* Use tunnel_id retuned by _create */
        return BCM_E_PARAM; 
    }

    vxlan_info = VXLAN_INFO(unit);

    if ((0 != vxlan_info->vxlan_tunnel_term[tunnel_idx].dip) && 
        (vxlan_info->vxlan_tunnel_term[tunnel_idx].dip != tnl_info->dip)) {
        /* Entry exists with the same idx*/
        return BCM_E_EXISTS;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        mem = MPLS_ENTRY_SINGLEm;
    } else
#endif
    {
        mem = VLAN_XLATEm;
    }

    /* Search Tunnel term-entry for given DIP to Hardware */
    (void)_bcm_td2_vxlan_tunnel_terminate_entry_key_set(unit, tnl_info, vxlate_entry, 0, 1);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vxlate_entry, vxlate_entry, 0);
    if (rv == SOC_E_NOT_FOUND) {
        /* Insert Tunnel term-entry */
        (void)_bcm_td2_vxlan_tunnel_terminate_entry_key_set(unit, tnl_info, vxlate_entry, 0, 0);
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vxlate_entry);
    } else if (rv != SOC_E_NONE) {
        return rv;
    }

    sal_memset(&(vxlan_info->vxlan_tunnel_term[tunnel_idx]), 0, sizeof(_bcm_vxlan_tunnel_endpoint_t));
    if (BCM_SUCCESS(rv) && (tunnel_idx != -1)) {
       vxlan_info->vxlan_tunnel_term[tunnel_idx].sip = tnl_info->sip;
       vxlan_info->vxlan_tunnel_term[tunnel_idx].dip = tnl_info->dip;
       vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan = tnl_info->vlan;
       vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state = _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF;
       vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag |= _BCM_VXLAN_TUNNEL_TERM_ENABLE;
       if (tnl_info->flags & BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE) {    
           vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag |= 
                _BCM_VXLAN_TUNNEL_TERM_UDP_CHECKSUM_ENABLE;
       }
       if (tnl_info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) {
           vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag |=
               _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_DSCP;
       } else if (tnl_info->flags & BCM_TUNNEL_TERM_USE_OUTER_PCP) {
           vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag |=
               _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_PCP;
       }
    }
    return rv;
}

/*  
 * Function:
 *      bcm_td2_vxlan_tunnel_terminator_update
 * Purpose:
 *     Update VXLAN Tunnel terminator multicast state
 * Parameters:
 *      unit - Device Number
 *      tnl_info
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vxlan_tunnel_terminator_update(int unit, bcm_tunnel_terminator_t *tnl_info)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int rv = BCM_E_NONE;
    int tunnel_idx=-1, index=-1;
    uint32  tunnel_dip=0;
    soc_mem_t mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_vlan_t    vlan = 0;
#endif
    int d = 0; /*vxlan decoupled mode*/

    vxlan_info = VXLAN_INFO(unit);

    /* Program tunnel id */
    if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
        if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
            return BCM_E_PARAM;
        }
        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
        _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);
    } else {
        /* Only BCM_TUNNEL_TERM_TUNNEL_WITH_ID supported */
        /* Use tunnel_id retuned by _create */
        return BCM_E_PARAM; 
    }


    tunnel_dip = vxlan_info->vxlan_tunnel_term[tunnel_idx].dip;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    vlan = vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan;
#endif

    sal_memset(vxlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        bcm_tunnel_terminator_t tnl_info_1;

        d = 1;
        mem = MPLS_ENTRY_SINGLEm;
        bcm_tunnel_terminator_t_init(&tnl_info_1);
        tnl_info_1.dip = tunnel_dip;
        _bcm_td2_vxlan_tunnel_terminate_entry_key_set(unit, &tnl_info_1, vxlate_entry, 0, 1);
    } else
#endif 
    {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            mem = VLAN_XLATE_1_DOUBLEm;

            soc_mem_field32_set(unit, mem, vxlate_entry, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, vxlate_entry, BASE_VALID_1f, 7);
            soc_mem_field32_set(unit, mem, vxlate_entry, DATA_TYPEf,
                                    _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
        } else
#endif
    {
        mem = VLAN_XLATEm;
        soc_mem_field32_set(unit, mem, vxlate_entry, VALIDf, 0x1);
    }

        soc_mem_field32_set(unit, mem, vxlate_entry,
            KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
        soc_mem_field32_set(unit, mem, vxlate_entry, VXLAN_DIP__DIPf, tunnel_dip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
        soc_mem_field32_set(unit, mem, vxlate_entry, VXLAN_DIP__OVIDf, vlan);
    }
#endif
    }
    if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_DEACTIVATE) {        
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                                 vxlate_entry, vxlate_entry, 0);
        if (rv == BCM_E_NONE) {
            rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, vxlate_entry);
            if (BCM_FAILURE(rv)) {
                 return rv;
            }
        } else if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }  
        vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag &= (~_BCM_VXLAN_TUNNEL_TERM_ENABLE);
        return BCM_E_NONE;
    } else if (!(vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag & _BCM_VXLAN_TUNNEL_TERM_ENABLE)) {    
        if (vxlan_info->vxlan_tunnel_term[tunnel_idx].dip != 0) {
            soc_mem_field32_set(unit, mem, vxlate_entry, BCM_VXLAN_DIP_FIELD(d,IGNORE_UDP_CHECKSUMf),
                   (vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag
                   & _BCM_VXLAN_TUNNEL_TERM_UDP_CHECKSUM_ENABLE) ? 0x0 : 0x1);
            if (vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag &
                _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_DSCP) {
                soc_mem_field32_set(unit, mem, vxlate_entry, BCM_VXLAN_DIP_FIELD(d,USE_OUTER_HEADER_PHBf), 0x1);
            } else if (vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag &
                _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_PCP) {
                soc_mem_field32_set(unit, mem, vxlate_entry, BCM_VXLAN_DIP_FIELD(d,USE_OUTER_HEADER_PHBf), 0x2);
            }
				   
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vxlate_entry, vxlate_entry, 0);
            if (rv == BCM_E_NOT_FOUND) {
                rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vxlate_entry);
                if (BCM_FAILURE(rv)) {
                     return rv;
                }
            } else if (rv != BCM_E_NONE) {
                return rv;
            }
            vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag |= _BCM_VXLAN_TUNNEL_TERM_ENABLE;
        }
    }

    rv = bcm_td2_vxlan_multicast_leaf_entry_check(unit, tunnel_idx,
                                                  tnl_info->multicast_flag);
    return rv;
}

/*
 * Function:
 *  bcm_td2_vxlan_tunnel_terminate_destroy
 * Purpose:
 *  Destroy VXLAN  tunnel_terminate Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  vxlan_tunnel_id
 * Returns:
 *  BCM_E_XXX
 */

int
bcm_td2_vxlan_tunnel_terminator_destroy(int unit, bcm_gport_t vxlan_tunnel_id)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int tunnel_idx=-1, index=-1;
    soc_mem_t mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];
    uint32  tunnel_dip=0;
    int rv = BCM_E_NONE;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_vlan_t    vlan = 0;
#endif
    int d = 0; /*vxlan decoupled mode*/

    if (!BCM_GPORT_IS_TUNNEL(vxlan_tunnel_id)) {
        return BCM_E_PARAM;
    }
    tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(vxlan_tunnel_id);
    _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);

    vxlan_info = VXLAN_INFO(unit);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    vlan = vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan;
#endif
    tunnel_dip = vxlan_info->vxlan_tunnel_term[tunnel_idx].dip;
    if (tunnel_dip == 0) {
        return BCM_E_NOT_FOUND;
    }

    if (!(_BCM_VXLAN_TUNNEL_TERM_ENABLE &
        vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag)) {
        /* Deactivated tunnel terminator can not be destroyed */
        return BCM_E_DISABLED;
    }

    sal_memset(vxlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    /* Remove Tunnel term-entry for given DIP from Hardware based on ref-count */
    if (_bcm_td2_vxlan_tunnel_terminator_reference_count(unit, tunnel_dip, 2) == 1)  {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            bcm_tunnel_terminator_t tnl_info;

            mem = MPLS_ENTRY_SINGLEm;
            d = 1;
            bcm_tunnel_terminator_t_init(&tnl_info);
            _bcm_td2_vxlan_tunnel_terminate_entry_key_set(unit, &tnl_info,
                           vxlate_entry, 0, 1);
        } else
#endif
        {
            mem = VLAN_XLATEm;
            soc_mem_field32_set(unit, mem, vxlate_entry, VALIDf, 0x1);
            soc_mem_field32_set(unit, mem, vxlate_entry,
                KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
        }
         soc_mem_field32_set(unit, mem, vxlate_entry, BCM_VXLAN_DIP_FIELD(d,DIPf), tunnel_dip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
         if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
             soc_mem_field32_set(unit, mem, vxlate_entry, VXLAN_DIP__OVIDf, vlan);
         }
#endif
         soc_mem_field32_set(unit, mem, vxlate_entry,
                BCM_VXLAN_DIP_FIELD(d,NETWORK_RECEIVERS_PRESENTf), 0);

         rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                &index, vxlate_entry, vxlate_entry, 0);
         if (BCM_FAILURE(rv)) {
              return rv;
         }
         /* Delete the entry from HW */
         rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, vxlate_entry);
         if (BCM_FAILURE(rv)) {
              return rv;
         }
    }

    /* Activate the entry , then Delete the entry */
    if (BCM_SUCCESS(rv) && (tunnel_idx != -1) && 
        (vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag & _BCM_VXLAN_TUNNEL_TERM_ENABLE)) {
       vxlan_info->vxlan_tunnel_term[tunnel_idx].dip = 0;
       vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan = 0;
       vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state = _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF;       
       vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag = 0;
    }
 
    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_tunnel_terminator_destroy_all
 * Purpose:
 *      Destroy all incoming VXLAN tunnel
 * Parameters:
 *      unit           - (IN) Device Number
 */
int
bcm_td2_vxlan_tunnel_terminator_destroy_all(int unit) 
{
    soc_mem_t vxlate_mem;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
    int rv = BCM_E_UNAVAIL, tnl_idx=0,num_vp=0;
    uint32 *vtab, *vtabp;
    int i, imin, imax, nent;
    int d = 0; /*vxlan decoupled mode*/

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        d = 1;
        vxlate_mem = MPLS_ENTRY_SINGLEm;
    } else
#endif
    {
        vxlate_mem = VLAN_XLATEm;
    }

    if (!SOC_HW_RESET(unit) && (SOC_SWITCH_BYPASS_MODE(unit) == SOC_SWITCH_BYPASS_MODE_NONE)) {
        imin = soc_mem_index_min(unit, vxlate_mem);
        imax = soc_mem_index_max(unit, vxlate_mem);
        nent = soc_mem_index_count(unit, vxlate_mem);

        vtab = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, vxlate_mem), "vlan_xlate");
        if (vtab == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(vtab, 0, SOC_MEM_TABLE_BYTES(unit, vxlate_mem));

        soc_mem_lock(unit, vxlate_mem);
        rv = soc_mem_read_range(unit, vxlate_mem, MEM_BLOCK_ANY,
                                imin, imax, vtab);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, vxlate_mem);
            soc_cm_sfree(unit, vtab);
            return rv; 
        }
        for (i = 0; i < nent; i++) {
            vtabp = soc_mem_table_idx_to_pointer(unit, vxlate_mem,
                                                 void *, vtab, i);

#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                if (soc_mem_field32_get(unit, vxlate_mem,
                        vtabp, BASE_VALIDf) != 1) {
                    continue;
                }
                if (soc_mem_field32_get(unit, vxlate_mem, vtabp, DATA_TYPEf)
                                                != _BCM_VXLAN_DATA_TYPE_LOOKUP_DIP_IN_MPLS_ENTRY) {
                    continue;
                }
            } else
#endif
            {
                if (!soc_mem_field32_get(unit, vxlate_mem, vtabp, VALIDf)) {
                    continue;
                }
            }

            if (soc_mem_field32_get(unit, vxlate_mem, vtabp, KEY_TYPEf) != 
                (d ? _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP_IN_MPLS_ENTRY : _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP)) {
                continue;
            }
            /* Delete the entry from HW */
            rv = soc_mem_delete(unit, vxlate_mem, MEM_BLOCK_ALL, vtabp);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, vxlate_mem);
                soc_cm_sfree(unit, vtab);
                return rv;
            }
        }
        soc_mem_unlock(unit, vxlate_mem);
        soc_cm_sfree(unit, vtab);
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (tnl_idx = 0; tnl_idx < num_vp; tnl_idx++) {
       vxlan_info->vxlan_tunnel_term[tnl_idx].dip = 0;
       vxlan_info->vxlan_tunnel_term[tnl_idx].vlan = 0;
       vxlan_info->vxlan_tunnel_term[tnl_idx].tunnel_state = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_vxlan_tunnel_initiator_get
 * Purpose:
 *      Get VXLAN  tunnel_terminate Entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Tunnel terminator structure
 */
int
bcm_td2_vxlan_tunnel_terminator_get(int unit, bcm_tunnel_terminator_t *tnl_info)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int tunnel_idx=-1, index=-1;
    soc_mem_t mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];
    uint32  tunnel_dip=0;
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_vlan_t vlan = 0;
#endif
    int d = 0; /*vxlan decoupled mode*/

    /* Program tunnel id */
    if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
        return BCM_E_PARAM;
    }
    tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
    _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);

    vxlan_info = VXLAN_INFO(unit);
    tunnel_dip = vxlan_info->vxlan_tunnel_term[tunnel_idx].dip;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    vlan = vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan;
#endif

     sal_memset(vxlate_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = MPLS_ENTRY_SINGLEm;
        d = 1;
        soc_mem_field32_set(unit, mem, vxlate_entry, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, mem, vxlate_entry, DATA_TYPEf,
                                _BCM_VXLAN_DATA_TYPE_LOOKUP_DIP_IN_MPLS_ENTRY);
        soc_mem_field32_set(unit, mem, vxlate_entry, KEY_TYPEf,
                                _BCM_VXLAN_DATA_TYPE_LOOKUP_DIP_IN_MPLS_ENTRY);
    } else
#endif
    {
        mem = VLAN_XLATEm;
        soc_mem_field32_set(unit, mem, vxlate_entry, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, vxlate_entry, KEY_TYPEf,
                               _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
    }

    soc_mem_field32_set(unit, mem, vxlate_entry, BCM_VXLAN_DIP_FIELD(d,DIPf), tunnel_dip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
        soc_mem_field32_set(unit, mem, vxlate_entry, VXLAN_DIP__OVIDf, vlan);
    }
#endif

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
            &index, vxlate_entry, vxlate_entry, 0);
    
    /* not exist or  De-activated */
    if (rv == BCM_E_NOT_FOUND) {
        if (!(vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag & _BCM_VXLAN_TUNNEL_TERM_ENABLE) && 
           (vxlan_info->vxlan_tunnel_term[tunnel_idx].dip != 0)) {
            tnl_info->flags |= BCM_TUNNEL_TERM_TUNNEL_DEACTIVATE;
        } else {
            return rv; 
        }
    } else if (rv != BCM_E_NONE) {
        return rv;    
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        if (soc_mem_field32_get(unit, mem, vxlate_entry,
                   VXLAN_FLEX_IPV4_DIP__EXPECTED_L3_IIF_MISMATCH_TO_CPUf)) {
            tnl_info->flags |= BCM_TUNNEL_TERM_L3_IIF_MISMATCH_TO_CPU; 
        }        
        if (soc_mem_field32_get(unit, mem, vxlate_entry,
               VXLAN_FLEX_IPV4_DIP__EXPECTED_L3_IIF_MISMATCH_DROPf)) {
           tnl_info->flags |= BCM_TUNNEL_TERM_L3_IIF_MISMATCH_DROP; 
        }
        
        tnl_info->tunnel_if = soc_mem_field32_get(unit, mem, vxlate_entry,
            VXLAN_FLEX_IPV4_DIP__EXPECTED_L3_IIFf);
        if (!tnl_info->tunnel_if) {
            tnl_info->tunnel_if = BCM_IF_INVALID;
        }
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    tnl_info->sip = vxlan_info->vxlan_tunnel_term[tunnel_idx].sip;
    tnl_info->dip = vxlan_info->vxlan_tunnel_term[tunnel_idx].dip;
    tnl_info->vlan = vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan;
    tnl_info->type = bcmTunnelTypeVxlan;
    if (vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state == _BCM_VXLAN_TUNNEL_TERM_MULTICAST_BUD) {
        tnl_info->multicast_flag = BCM_VXLAN_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
    } else if (vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state == _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF){
        tnl_info->multicast_flag = BCM_VXLAN_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
    } else {
        tnl_info->multicast_flag = 0;
    }

    if (vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag & _BCM_VXLAN_TUNNEL_TERM_UDP_CHECKSUM_ENABLE) {
        tnl_info->flags |= BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE;
    }

    if (vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag &
        _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_DSCP) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
    } else if (vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag &
        _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_PCP) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_PCP;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_vxlan_tunnel_terminator_traverse
 * Purpose:
 *      Traverse VXLAN tunnel terminator entries
 * Parameters:
 *      unit    - (IN) Device Number
 *      cb    - (IN) User callback function, called once per entry
 *      user_data    - (IN) User supplied cookie used in parameter in callback function 
 */
int
bcm_td2_vxlan_tunnel_terminator_traverse(int unit, bcm_tunnel_terminator_traverse_cb cb, void *user_data)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int num_tnl = 0, idx = 0;
    bcm_tunnel_terminator_t info;
    int rv = BCM_E_NONE;

    vxlan_info = VXLAN_INFO(unit);
    num_tnl = soc_mem_index_count(unit, SOURCE_VPm);

    /* Iterate over all the entries - search valid ones. */
    for(idx = 0; idx < num_tnl; idx++) {
        /* Check a valid entry */
        if ((vxlan_info->vxlan_tunnel_term[idx].dip) 
            || (vxlan_info->vxlan_tunnel_term[idx].sip)){
            /* Reset buffer before read. */
            bcm_tunnel_terminator_t_init(&info);

            BCM_GPORT_TUNNEL_ID_SET(info.tunnel_id, idx);

            rv = bcm_td2_vxlan_tunnel_terminator_get(unit,&info);

            /* search failure */
            if (BCM_FAILURE(rv)) {
                if (rv != BCM_E_NOT_FOUND) {
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
    if (BCM_E_NOT_FOUND == rv) {
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_tunnel_init_add
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
_bcm_td2_vxlan_tunnel_init_add(int unit, int idx, bcm_tunnel_initiator_t *info)
{
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */
    soc_mem_t mem;                        /* Tunnel initiator table memory */  
    int rv = BCM_E_NONE;                  /* Return value                  */
    int df_val=0;                           /* Don't fragment encoding       */

    mem = EGR_IP_TUNNELm; 

    /* Zero write buffer. */
    sal_memset(tnl_entry, 0, sizeof(tnl_entry));

    /* If replacing existing entry, first read the entry to get old 
       profile pointer and TPID */
    if (info->flags & BCM_TUNNEL_REPLACE) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tnl_entry);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set source address only within Tunnel Table */
    soc_mem_field_set(unit, mem, tnl_entry, SIPf, (uint32 *)&info->sip);

    /* IP tunnel hdr DF bit for IPv4. */
    df_val = 0;
    if (info->flags & BCM_TUNNEL_INIT_USE_INNER_DF) {
        df_val |= 0x2;
    } else if (info->flags & BCM_TUNNEL_INIT_IPV4_SET_DF) { 
        df_val |= 0x1;
    }
    soc_mem_field32_set(unit, mem, tnl_entry, IPV4_DF_SELf, df_val);

    /* IP tunnel hdr DF bit for IPv6. */
    if (info->flags & BCM_TUNNEL_INIT_IPV6_SET_DF) { 
        soc_mem_field32_set(unit, mem, tnl_entry, IPV6_DF_SELf, 1);
    }

    /* Tunnel header DSCP select. */
    soc_mem_field32_set(unit, mem, tnl_entry, DSCP_SELf, info->dscp_sel);

    if (info->dscp_sel == bcmTunnelDscpMap) {
        /* Set DSCP_MAP value. */
        int hw_map_idx = 0;
        BCM_IF_ERROR_RETURN(
            _bcm_tr2_qos_id2idx(unit, info->dscp_map, &hw_map_idx));
        soc_mem_field32_set(unit, mem, tnl_entry,
                            DSCP_MAPPING_PTRf, hw_map_idx);
    } else {
        /* Set DSCP value. */
        soc_mem_field32_set(unit, mem, tnl_entry, DSCPf, info->dscp);
    }

    /* Set TTL. */
    soc_mem_field32_set(unit, mem, tnl_entry, TTLf, info->ttl);

    /* Set UDP Dest Port */
    soc_mem_field32_set(unit, mem, tnl_entry, L4_DEST_PORTf, info->udp_dst_port);

    /* Set UDP Src Port */
    soc_mem_field32_set(unit, mem, tnl_entry, L4_SRC_PORTf, info->udp_src_port);

    /* Set Tunnel type = VXLAN */
    soc_mem_field32_set(unit, mem, tnl_entry, TUNNEL_TYPEf, _BCM_VXLAN_TUNNEL_TYPE);

    /* Set entry type = IPv4 */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, tnl_entry, DATA_TYPEf, 0x1);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, tnl_entry, ENTRY_TYPEf, 0x1);
    }

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
 *      _bcm_td2_vxlan_tunnel_initiator_entry_add
 * Purpose:
 *      Configure VXLAN Tunnel initiator hardware Entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_vxlan_tunnel_initiator_entry_add(int unit, uint32 flags, bcm_tunnel_initiator_t *info, int *tnl_idx)
{
   int rv = BCM_E_NONE;

   rv = bcm_xgs3_tnl_init_add(unit, flags, info, tnl_idx);
   if (BCM_FAILURE(rv)) {
         return rv;
   }
   /* Write the entry to HW */
   rv = _bcm_td2_vxlan_tunnel_init_add(unit, *tnl_idx, info);
   if (BCM_FAILURE(rv)) {
        flags = _BCM_L3_SHR_WRITE_DISABLE;
        (void) bcm_xgs3_tnl_init_del(unit, flags, *tnl_idx);
   }
   return rv;
}

/*
  * Function:
  *      _bcm_td2_vxlan_tunnel_initiator_get_tunnel_id
  * Purpose:
  *      Get an available tunnel id for vxlan tunnel initiator
  * Parameters:
  *      unit - (IN) SOC unit number.
  *      info - (IN) the informaton of tunnel initiator
  *      hw_match_flag - (IN) Indicate if an existing EGR_IP_TUNNEL entry
  *      soft_tnl_idx - (out) Tunnel id for tunnel initiator
  *      new_id_flag - (out) Indicate if a new free tunnel id
  * Returns:
  *      BCM_E_XXX
  */
STATIC int
_bcm_td2_vxlan_tunnel_initiator_get_tunnel_id(int unit,
    bcm_tunnel_initiator_t *info, int hw_match_flag, 
    int *soft_tnl_idx, int *new_id_flag)
{
    int rv = BCM_E_NONE;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int idx = 0;
    int num_vp = 0;
    int free_index = -1;
    int free_entry_found = 0;

    if ((NULL == info) || (NULL == soft_tnl_idx) || (NULL == new_id_flag)) {
        return BCM_E_PARAM;
    }

    vxlan_info = VXLAN_INFO(unit);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    *new_id_flag = FALSE;

    for (idx = 0; idx < num_vp; idx++) {
        if (vxlan_info->vxlan_tunnel_init[idx].sip == info->sip &&
            vxlan_info->vxlan_tunnel_init[idx].tunnel_state ==
            info->udp_dst_port &&
            vxlan_info->vxlan_tunnel_init[idx].dip == info->dip) {
            /* Obtain existed index */
            *soft_tnl_idx = idx;
            return BCM_E_NONE;
        }

        if (vxlan_info->vxlan_tunnel_init[idx].sip == 0 && !free_entry_found) {
            /* Obtain Free index */
            free_index = idx;
            free_entry_found = 1;
            if (!hw_match_flag) {
                break;
            }
        }
    }

    if (free_entry_found) {
        *new_id_flag = TRUE;
        *soft_tnl_idx = free_index;
    } else {
        /* No free resource */
        return BCM_E_RESOURCE;
    }

    return rv;
}

/*  
 * Function:
 *      bcm_td2_vxlan_tunnel_initiator_create
 * Purpose:
 *      Set VXLAN Tunnel initiator
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vxlan_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int tnl_idx=-1, rv = BCM_E_NONE,  ref_count=0;
    uint32 flags=0;
    int idx = 0, num_vp = 0;
    uint8  match_entry_present=0;
    int soft_tnl_idx = -1;
    int new_idx_flag = FALSE;

    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }   
    if (info->type != bcmTunnelTypeVxlan)  {
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
    if ((0 == info->dip) || (0 == info->sip)) {
        return BCM_E_PARAM; 
    }
   
    vxlan_info = VXLAN_INFO(unit);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if (info->flags & BCM_TUNNEL_REPLACE) {
        int tunnel_idx = -1;
        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);
        if(info->sip != vxlan_info->vxlan_tunnel_init[tunnel_idx].sip 
            || info->udp_dst_port != 
                vxlan_info->vxlan_tunnel_init[tunnel_idx].tunnel_state
            || info->dip != vxlan_info->vxlan_tunnel_init[tunnel_idx].dip) {
            return BCM_E_UNAVAIL;
        }    
    }

    /* Allocate either existing or new tunnel initiator entry */
    flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE |
             _BCM_L3_SHR_SKIP_INDEX_ZERO;

    if (BCM_E_NONE == _bcm_xgs3_l3_tnl_init_find(unit, info, &tnl_idx)) {
        match_entry_present = 1;
    }
    /* Get an available tunnel id */
    BCM_IF_ERROR_RETURN(
        _bcm_td2_vxlan_tunnel_initiator_get_tunnel_id(
            unit, info, match_entry_present, &soft_tnl_idx, &new_idx_flag));

    if (match_entry_present) {
        if (info->flags & BCM_TUNNEL_REPLACE) {
            if (new_idx_flag) {
                return BCM_E_UNAVAIL;
            }
            flags |= _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WITH_ID;
            /* Check for Ref-count in (SIP, multi-DIP) situation */
            for (idx = 0; idx < num_vp; idx++) {
                if ((info->sip ==
                    vxlan_info->vxlan_tunnel_init[idx].sip) &&
                    (info->udp_dst_port ==
                    vxlan_info->vxlan_tunnel_init[idx].tunnel_state)) {
                    if (++ref_count > 1) {
                        return BCM_E_RESOURCE;
                    }
                }
            }
            rv = _bcm_td2_vxlan_tunnel_initiator_entry_add(
                unit, flags, info, &tnl_idx);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    } else {
         /* Parse tunnel replace flag */
         if (info->flags & BCM_TUNNEL_REPLACE) {
             /* replace an inexistent tunnel initiator */
             return BCM_E_NOT_FOUND;
         }
         rv = _bcm_td2_vxlan_tunnel_initiator_entry_add(unit, flags, info, &tnl_idx);
         if (BCM_FAILURE(rv)) {
             return rv;
         }
    }

    if (soft_tnl_idx !=- 1) {
         vxlan_info->vxlan_tunnel_init[soft_tnl_idx].sip = info->sip;
         vxlan_info->vxlan_tunnel_init[soft_tnl_idx].dip = info->dip;
         vxlan_info->vxlan_tunnel_init[soft_tnl_idx].tunnel_state = info->udp_dst_port;
         BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, soft_tnl_idx);
    } else {
        return BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_tunnel_init_get
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
_bcm_td2_vxlan_tunnel_init_get(int unit, int *hw_idx, bcm_tunnel_initiator_t *info)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    egr_fragment_id_table_entry_t entry;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry.  */
    soc_mem_t mem;                         /* Tunnel initiator table memory.*/  
    int df_val=0;                            /* Don't fragment encoded value. */
    int tnl_type=0;                          /* Tunnel type.                  */
    uint32 entry_type = BCM_XGS3_TUNNEL_INIT_V4;/* Entry type (IPv4/IPv6/MPLS)*/
    int idx = 0;

    /* Get table memory. */
    mem = EGR_IP_TUNNELm; 
    idx = *hw_idx;

    /* Initialize the buffer. */
    sal_memset(tnl_entry, 0, sizeof(tnl_entry));
    vxlan_info = VXLAN_INFO(unit);
    info->dip = vxlan_info->vxlan_tunnel_init[idx].dip;
    BCM_IF_ERROR_RETURN(
        _bcm_td2_vxlan_tunnel_initiator_idx_translate(unit, idx, hw_idx));
    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, mem, MEM_BLOCK_ANY, *hw_idx, tnl_entry));

    /* Get tunnel type. */
    tnl_type = soc_mem_field32_get(unit, mem, tnl_entry, TUNNEL_TYPEf);
    if (tnl_type != _BCM_VXLAN_TUNNEL_TYPE) {
        return BCM_E_NOT_FOUND;
    }

    /* Get entry_type. */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        entry_type = soc_mem_field32_get(unit, mem, tnl_entry, DATA_TYPEf);
    } else
#endif
    {
        entry_type = soc_mem_field32_get(unit, mem, tnl_entry, ENTRY_TYPEf);
    }
    if (entry_type != 0x1) {
        return BCM_E_NOT_FOUND;
    }

    /* Parse hw buffer. */
    /* Check will not fail. entry_type already validated */
    if (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) {
        /* Get source ip. */
        info->sip = soc_mem_field32_get(unit, mem, tnl_entry, SIPf);
    } 

    /* Tunnel header DSCP select. */
    info->dscp_sel = soc_mem_field32_get(unit, mem, tnl_entry, DSCP_SELf);

    if (info->dscp_sel == bcmTunnelDscpMap) {
        /* Tunnel header DSCP_MAP value. */
        BCM_IF_ERROR_RETURN(
            _bcm_tr2_qos_idx2id(unit, soc_mem_field32_get(unit, mem, tnl_entry,
                                                          DSCP_MAPPING_PTRf),
                                _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,
                                &info->dscp_map));
    } else {
        /* Tunnel header DSCP value. */
        info->dscp = soc_mem_field32_get(unit, mem, tnl_entry, DSCPf);
    }

    /* IP tunnel hdr DF bit for IPv4 */
    df_val = soc_mem_field32_get(unit, mem, tnl_entry, IPV4_DF_SELf);
    if (0x2 <= df_val) {
        info->flags |= BCM_TUNNEL_INIT_USE_INNER_DF;  
    } else if (0x1 == df_val) {
        info->flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;  
    }

    df_val = soc_mem_field32_get(unit, mem, tnl_entry, IPV6_DF_SELf);
    if (0x1 == df_val) {
        info->flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;  
    }

    /* Get TTL. */
    info->ttl = soc_mem_field32_get(unit, mem, tnl_entry, TTLf);

    /* Get UDP Dest Port */
    info->udp_dst_port = soc_mem_field32_get(unit, mem, tnl_entry, L4_DEST_PORTf);

    /* Get UDP Src Port */
    info->udp_src_port = soc_mem_field32_get(unit, mem, tnl_entry, L4_SRC_PORTf);

    /* Translate hw tunnel type into API encoding. - bcmTunnelTypeVxlan */
    BCM_IF_ERROR_RETURN(_bcm_trx_tnl_hw_code_to_type(unit, tnl_type,
                                                     entry_type,
                                                     &info->type));
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
            unit, EGR_FRAGMENT_ID_TABLEm, MEM_BLOCK_ANY, *hw_idx, &entry));
        info->ip4_id = soc_mem_field32_get(
            unit, EGR_FRAGMENT_ID_TABLEm, &entry, FRAGMENT_IDf);
    }

    *hw_idx = idx;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_vxlan_tunnel_initiator_destroy
 * Purpose:
 *      Destroy outgoing VXLAN tunnel
 * Parameters:
 *      unit           - (IN) Device Number
 *      vxlan_tunnel_id - (IN) Tunnel ID (Gport)
 */
int
bcm_td2_vxlan_tunnel_initiator_destroy(int unit, bcm_gport_t vxlan_tunnel_id) 
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int tnl_idx=-1, temp_tnl_idx=-1, rv = BCM_E_NONE, ref_count=0, idx=0;
    uint32 flags = 0;
    int num_vp=0;
    bcm_ip_t sip=0;                       /* Tunnel header SIP (IPv4). */
    uint16 udp_dst_port;                /* Destination UDP port */

    vxlan_info = VXLAN_INFO(unit);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    /* Input parameters check. */
    if (!BCM_GPORT_IS_TUNNEL(vxlan_tunnel_id)) {
        return BCM_E_PARAM;
    }
    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(vxlan_tunnel_id);
    _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tnl_idx);

    sip = vxlan_info->vxlan_tunnel_init[tnl_idx].sip;
    udp_dst_port =  vxlan_info->vxlan_tunnel_init[tnl_idx].tunnel_state;
    if (sip == 0) {
        return BCM_E_NOT_FOUND;
    }
    /* Remove HW TNL entry only if all matching SIP, UDP_DEST_PORT gets removed */
    for (idx=0; idx < num_vp; idx++) {
        if ((vxlan_info->vxlan_tunnel_init[idx].sip == sip) &&
             (vxlan_info->vxlan_tunnel_init[idx].tunnel_state == udp_dst_port)) {
                ref_count++;
        }
    }

    if (ref_count == 1) {
       /* Get the entry first */
       temp_tnl_idx = tnl_idx;
       BCM_IF_ERROR_RETURN(
           _bcm_td2_vxlan_tunnel_initiator_idx_translate(
               unit, temp_tnl_idx, &tnl_idx));
       /* Destroy the tunnel entry */
       (void) bcm_xgs3_tnl_init_del(unit, flags, tnl_idx);
       vxlan_info->vxlan_tunnel_init[temp_tnl_idx].sip = 0;
       vxlan_info->vxlan_tunnel_init[temp_tnl_idx].dip = 0;
       vxlan_info->vxlan_tunnel_init[temp_tnl_idx].tunnel_state = 0;
    } else if (ref_count) {
       vxlan_info->vxlan_tunnel_init[tnl_idx].sip = 0;
       vxlan_info->vxlan_tunnel_init[tnl_idx].dip = 0;
       vxlan_info->vxlan_tunnel_init[tnl_idx].tunnel_state = 0;
    }
    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_tunnel_initiator_destroy_all
 * Purpose:
 *      Destroy all outgoing VXLAN tunnel
 * Parameters:
 *      unit           - (IN) Device Number
 */
int
bcm_td2_vxlan_tunnel_initiator_destroy_all(int unit) 
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    soc_field_t type_field;
    egr_ip_tunnel_entry_t *tnl_entry, *tnl_entries;
    int rv = BCM_E_NONE, tnl_idx, num_entries, num_vp;

    vxlan_info = VXLAN_INFO(unit);
    num_entries = soc_mem_index_count(unit, EGR_IP_TUNNELm);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_IP_TUNNELm, DATA_TYPEf)) {
        type_field = DATA_TYPEf;
    } else
#endif
    {
        type_field = ENTRY_TYPEf;
    }

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

        vxlan_info->vxlan_tunnel_init[tnl_idx].sip = 0;
        vxlan_info->vxlan_tunnel_init[tnl_idx].dip = 0;
        vxlan_info->vxlan_tunnel_init[tnl_idx].tunnel_state = 0;

        if (!soc_EGR_IP_TUNNELm_field32_get(unit, tnl_entry, type_field)) {
            continue;
        }
        if (soc_EGR_IP_TUNNELm_field32_get(unit, tnl_entry,
                                      TUNNEL_TYPEf) != _BCM_VXLAN_TUNNEL_TYPE) {
            continue;
        }

       /* Delete the entry from HW */
       (void) bcm_xgs3_tnl_init_del(unit, 0, tnl_idx);

        vxlan_info->vxlan_tunnel_init[tnl_idx].sip = 0;
        vxlan_info->vxlan_tunnel_init[tnl_idx].dip = 0;
        vxlan_info->vxlan_tunnel_init[tnl_idx].tunnel_state = 0;
    }
    soc_mem_unlock(unit, EGR_IP_TUNNELm);
    soc_cm_sfree(unit, tnl_entries);

    for (tnl_idx = num_entries; tnl_idx < num_vp; tnl_idx++) {
         vxlan_info->vxlan_tunnel_init[tnl_idx].sip = 0;
         vxlan_info->vxlan_tunnel_init[tnl_idx].dip = 0;
         vxlan_info->vxlan_tunnel_init[tnl_idx].tunnel_state = 0;
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_td2_vxlan_tunnel_initiator_get
 * Purpose:
 *      Get an outgong VXLAN tunnel info
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Tunnel initiator structure
 */
int
bcm_td2_vxlan_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info)
{
    int tnl_idx=-1, rv = BCM_E_NONE;

    /* Input parameters check. */
    if (info == NULL) {
        return BCM_E_PARAM;
    }
    if (!BCM_GPORT_IS_TUNNEL(info->tunnel_id)) {
        return BCM_E_PARAM;
    }
    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
    _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tnl_idx);

    /* Get the entry */
    rv = _bcm_td2_vxlan_tunnel_init_get(unit, &tnl_idx, info);
    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_tunnel_initiator_traverse
 * Purpose:
 *      Traverse VXLAN tunnel initiator entries
 * Parameters:
 *      unit    - (IN) Device Number
 *      cb    - (IN) User callback function, called once per entry
 *      user_data    - (IN) User supplied cookie used in parameter in callback function 
 */
int
bcm_td2_vxlan_tunnel_initiator_traverse(int unit, bcm_tunnel_initiator_traverse_cb cb, void *user_data)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int num_vp = 0, idx = 0;
    bcm_tunnel_initiator_t info;
    int rv = BCM_E_NONE;

    vxlan_info = VXLAN_INFO(unit);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    /* Iterate over all the entries - search valid ones. */
    for(idx = 0; idx < num_vp; idx++) {
        /* Check a valid entry */
        if (vxlan_info->vxlan_tunnel_init[idx].sip != 0) {
            /* Reset buffer before read. */
            bcm_tunnel_initiator_t_init(&info);

            BCM_GPORT_TUNNEL_ID_SET(info.tunnel_id, idx);
            rv = bcm_td2_vxlan_tunnel_initiator_get(unit, &info);

            /* search failure */
            if (BCM_FAILURE(rv)) {
                if (rv != BCM_E_NOT_FOUND) {
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
    if (BCM_E_NOT_FOUND == rv) {
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
  * Function:
  *      _bcm_td2_vxlan_tunnel_initiator_idx_translate
  * Purpose:
  *      Translate the index of software object to the index 
  *      of hw entry for tunnel initiator
  * Parameters:
  *      unit     - (IN) SOC unit number.
  *      soft_idx - (IN) The index of software object 
  *      hw_idx   - (IN) The index of hw entry
  * Returns:
  *      BCM_E_XXX
  */
STATIC int
_bcm_td2_vxlan_tunnel_initiator_idx_translate(int unit, int soft_idx, 
                                                     int *hw_idx)
{
    bcm_tunnel_initiator_t init_tnl_info;
    int rv = BCM_E_NONE;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    vxlan_info = VXLAN_INFO(unit);

    if (NULL == hw_idx) {
        return BCM_E_PARAM;
    }

    bcm_tunnel_initiator_t_init(&init_tnl_info);
    init_tnl_info.type = bcmTunnelTypeVxlan;
    init_tnl_info.sip =  vxlan_info->vxlan_tunnel_init[soft_idx].sip;
    init_tnl_info.udp_dst_port = 
        vxlan_info->vxlan_tunnel_init[soft_idx].tunnel_state;

    rv = _bcm_xgs3_l3_tnl_init_find(unit, &init_tnl_info, hw_idx);

    return rv;
}

/*
  * Function:
  *      _bcm_td2_vxlan_tunnel_initiator_cmp
  * Purpose:
  *      Routine compares vxlan tunnel initiator entry with entry in the chip
  *      with specified index.
  * Parameters:
  *      unit       - (IN) SOC unit number.
  *      buf        - (IN) Tunnel initiator entry to compare.
  *      index      - (IN) Entry index in the chip to compare.
  *      cmp_result - (OUT)Compare result.
  * Returns:
  *      BCM_E_XXX
  */
int
_bcm_td2_vxlan_tunnel_initiator_cmp(int unit, void *buf, 
                                          int index, int *cmp_result)
{
    bcm_tunnel_initiator_t hw_entry;   /* Entry read from hw */
    bcm_tunnel_initiator_t *tnl_entry; /* Api passed buffer */
    int rv = BCM_E_NONE;

    if ((NULL == buf) || (NULL == cmp_result)) {
        return BCM_E_PARAM;
    }

    *cmp_result = BCM_L3_CMP_NOT_EQUAL;

    /* Copy buffer to structure */
    tnl_entry = (bcm_tunnel_initiator_t *)buf;

    /* Initialization */
    bcm_tunnel_initiator_t_init(&hw_entry);

    /* Get tunnel initiator entry from hw */
    if (BCM_XGS3_L3_HWCALL_CHECK(unit, tnl_init_get)) {
        BCM_XGS3_L3_MODULE_LOCK(unit);
        rv = (BCM_XGS3_L3_HWCALL_EXEC(unit, tnl_init_get)(
            unit, index, &hw_entry));
        BCM_XGS3_L3_MODULE_UNLOCK(unit);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        return BCM_E_UNAVAIL;
    }

    /* Compare source ip */
    if (tnl_entry->sip != hw_entry.sip) {
        return BCM_E_NONE;
    }
                
    /* Compare udp dst port */
    if (tnl_entry->udp_dst_port != hw_entry.udp_dst_port) {
        return BCM_E_NONE;
    }
            
    *cmp_result = BCM_L3_CMP_EQUAL;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_tunnel_initiator_hash_calc
 * Purpose:
 *      Calculate tunnel initiator entry hash(signature).
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      buf  - (IN) Tunnel initiator entry information.
 *      hash - (OUT)Hash(signature) calculated value.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_td2_vxlan_tunnel_initiator_hash_calc(int unit, void *buf, uint16 *hash)
{

    bcm_tunnel_initiator_t tnl_entry;

    if ((NULL == buf) || (NULL == hash)) {
        return (BCM_E_PARAM);
    }

    bcm_tunnel_initiator_t_init(&tnl_entry);
    tnl_entry.type = ((bcm_tunnel_initiator_t *)buf)->type;

    /* Set source ip */
    tnl_entry.sip = ((bcm_tunnel_initiator_t *)buf)->sip;

    tnl_entry.udp_dst_port =
        ((bcm_tunnel_initiator_t *)buf)->udp_dst_port;
    /* Calculate hash */
    *hash = _shr_crc16(0, (uint8 *)&tnl_entry,
        sizeof(bcm_tunnel_initiator_t));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_vxlan_ingress_dvp_set
 * Purpose:
 *     Set Ingress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  vxlan_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_ingress_dvp_set(int unit, int vp, uint32 mpath_flag,
                                    int vp_nh_ecmp_index, bcm_vxlan_port_t *vxlan_port)
{
    ing_dvp_table_entry_t dvp;
    int network_group;
    int rv = BCM_E_NONE;

         if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            BCM_IF_ERROR_RETURN(
                READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
         } else if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID ) {
            sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
         } else {
            sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
         }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
             if (mpath_flag) {
                  soc_ING_DVP_TABLEm_field32_set(unit, &dvp, 
                                           NEXT_HOP_INDEXf, 0x0);
                  soc_ING_DVP_TABLEm_field32_set(unit, &dvp, 
                                           ECMPf, 0x1);
                  soc_ING_DVP_TABLEm_field32_set(unit, &dvp, 
                                           ECMP_PTRf, vp_nh_ecmp_index);
             } else {
                  soc_ING_DVP_TABLEm_field32_set(unit, &dvp, 
                                           ECMPf, 0x0);
                  soc_ING_DVP_TABLEm_field32_set(unit, &dvp, 
                                           ECMP_PTRf, 0);
                  soc_ING_DVP_TABLEm_field32_set(unit, &dvp, 
                                           NEXT_HOP_INDEXf, vp_nh_ecmp_index);
             }
        }

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            network_group = vxlan_port->network_group_id;
            BCM_IF_ERROR_RETURN(_bcm_validate_splithorizon_network_group(unit,
                    vxlan_port->flags & BCM_VXLAN_PORT_NETWORK, &network_group));
            if (vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) {
                 /* Not setting dvp as network port for RIOT as all
                    routed packet gets dropped if overlay SVP and DVP
                    both are network ports.
                    SVP and DVP as network ports is a valid used case
                    but reg file says that this field
                    is set when DVP is a TRILL port */
#ifdef BCM_RIOT_SUPPORT
                 if ((BCMI_RIOT_IS_ENABLED(unit))) {
                    /* explicitly setting it to zero. */
                     if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
                         soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NETWORK_GROUPf,0x0);
                     } else {
                         soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NETWORK_PORTf, 0x0);
                     }
                 } else
#endif
                 {
                     if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
                         soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NETWORK_GROUPf,
                                                         network_group);
                     } else {
                         if (vxlan_port->flags &
                             BCM_VXLAN_PORT_NO_SPLIT_HORIZON_INGRESS) {
                             soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                                                            NETWORK_PORTf, 0x0);
                         } else {
                             soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                                                            NETWORK_PORTf, 0x1);
                         }
                     }
                 }
            } else {
                if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
                    soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NETWORK_GROUPf,
                                                   network_group);
                } else {
                    soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NETWORK_PORTf, 0x0);
                }
            }
        }

        if (vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) {
             soc_ING_DVP_TABLEm_field32_set(unit, &dvp, 
                           VP_TYPEf, _BCM_VXLAN_INGRESS_DEST_VP_TYPE);
        } else {
             soc_ING_DVP_TABLEm_field32_set(unit, &dvp, 
                           VP_TYPEf, _BCM_VXLAN_DEST_VP_TYPE_ACCESS);
        }

        rv =  WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);   
        return rv;
}

/*
 * Function:
 *     _bcm_td2_vxlan_ingress_dvp_get
 * Purpose:
 *     Get Ingress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  vxlan_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_ingress_dvp_get(int unit, int vp, bcm_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_NONE;
    ing_dvp_table_entry_t dvp;
    int vp_type = 0; /* access / tunnel vp type*/
    int network_port = 0;
    _bcm_vp_info_t vp_info;

#ifdef BCM_RIOT_SUPPORT
    if ((BCMI_RIOT_IS_ENABLED(unit))) {
        return rv;
    } else
#endif
    {
        sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_DVP_TABLEm,
                          MEM_BLOCK_ANY, vp, &dvp));

        vp_type = soc_mem_field32_get(unit,
                                      ING_DVP_TABLEm, &dvp, VP_TYPEf);
        if (soc_mem_field_valid(unit, ING_DVP_TABLEm, NETWORK_PORTf) &&
            (vp_type == _BCM_VXLAN_INGRESS_DEST_VP_TYPE)) {
            network_port = soc_mem_field32_get(unit, ING_DVP_TABLEm, &dvp,
                                               NETWORK_PORTf);
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
            if ((vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) &&
                network_port == 0) {
                vxlan_port->flags |= BCM_VXLAN_PORT_NO_SPLIT_HORIZON_INGRESS;
            }
        }
    }

    return rv;
}


/*
 * Function:
 *     _bcm_td2_vxlan_ingress_dvp_2_set
 * Purpose:
 *     Set Ingress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  vxlan_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_ingress_dvp_2_set(int unit, int vp, uint32 mpath_flag,
                                    int vp_nh_ecmp_index, bcm_vxlan_port_t *vxlan_port)
{
    ing_dvp_2_table_entry_t dvp;
    int rv = BCM_E_NONE;
    int network_group=0;

         if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            BCM_IF_ERROR_RETURN(
                READ_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
         } else if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID ) {
            sal_memset(&dvp, 0, sizeof(ing_dvp_2_table_entry_t));
         } else {
            sal_memset(&dvp, 0, sizeof(ing_dvp_2_table_entry_t));
         }

         if (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
             if (mpath_flag) {
                  soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, 
                                           NEXT_HOP_INDEXf, 0x0);
                  soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, 
                                           ECMPf, 0x1);
                  soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, 
                                           ECMP_PTRf, vp_nh_ecmp_index);
             } else {
                  soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, 
                                           ECMPf, 0x0);
                  soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, 
                                           ECMP_PTRf, 0);
                  soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, 
                                           NEXT_HOP_INDEXf, vp_nh_ecmp_index);
             }
        }

         network_group = vxlan_port->network_group_id;
         rv = _bcm_validate_splithorizon_network_group(unit,
                 vxlan_port->flags & BCM_VXLAN_PORT_NETWORK, &network_group);
         BCM_IF_ERROR_RETURN(rv);
        /* Enable DVP as Network_port */
        if (vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) {
             /* Not setting dvp as network port for RIOT as all
                routed packet gets dropped if overlay SVP and DVP
                both are network ports.
                SVP and DVP as network ports is a valid used case
                but reg file says that this field
                is set when DVP is a TRILL port */
#ifdef BCM_RIOT_SUPPORT
             if ((BCMI_RIOT_IS_ENABLED(unit))) {
                 if (soc_feature(unit,
                     soc_feature_multiple_split_horizon_group)) {
                     soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                             NETWORK_GROUPf, 0x0);
                 } else { 
                     soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                             NETWORK_PORTf, 0x0);
                 }
             } else
#endif
             {

                 if (soc_feature(unit, 
                             soc_feature_multiple_split_horizon_group)) {
                     soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, NETWORK_GROUPf,
                                 network_group);
                 } else { 
                     if (vxlan_port->flags &
                         BCM_VXLAN_PORT_NO_SPLIT_HORIZON_INGRESS) {
                         soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                                                          NETWORK_PORTf, 0x0);
                     } else {
                         soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                                                          NETWORK_PORTf, 0x1);
                     }
                 }
             }
             soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, 
                           VP_TYPEf, _BCM_VXLAN_INGRESS_DEST_VP_TYPE);
        } else {

            if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
                soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, NETWORK_GROUPf,
                            network_group);
            } else { 
                soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, NETWORK_PORTf, 0x0);
            }
             soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, 
                           VP_TYPEf, _BCM_VXLAN_DEST_VP_TYPE_ACCESS);
        }

        rv =  WRITE_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);   
        return rv;
}


/*
 * Function:
 *     _bcm_td2_vxlan_ingress_dvp_reset
 * Purpose:
 *     Reset Ingress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_ingress_dvp_reset(int unit, int vp)
{
    int rv=BCM_E_UNAVAIL;
    ing_dvp_table_entry_t dvp;
    ing_dvp_2_table_entry_t dvp_2;

  
    sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
    rv =  WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);   
    BCM_IF_ERROR_RETURN(rv); 
    sal_memset(&dvp_2, 0, sizeof(ing_dvp_2_table_entry_t));
    rv =  WRITE_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_2);  

    return rv;
}

/*
 * Function:
 *     _bcm_td2_vxlan_egress_access_dvp_set
 * Purpose:
 *     Set Egress DVP tables - For access virtual ports
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  drop (unused for now)
 *   IN :  vxlan_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_egress_access_dvp_set(int unit, int vp, int drop,
                                     bcm_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_UNAVAIL;
    int network_group=0;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;

    /* Access ports only */
    if (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) {
        return BCM_E_PARAM;
    }

    /* Read entry and then update */
    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                DATA_TYPEf, _BCM_VXLAN_DEST_VP_TYPE_ACCESS);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                VP_TYPEf, _BCM_VXLAN_DEST_VP_TYPE_ACCESS);
    }


    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        network_group = vxlan_port->network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
                vxlan_port->flags & BCM_VXLAN_PORT_NETWORK, &network_group);
        BCM_IF_ERROR_RETURN(rv);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, COMMON__DVP_NETWORK_GROUPf)) {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                    COMMON__DVP_NETWORK_GROUPf, network_group);
        } else
#endif
        {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                    VXLAN__DVP_NETWORK_GROUPf, network_group);
        }
    }

    /* L2 MTU - This is different from L3 MTU */
    if (vxlan_port->mtu > 0) {
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                            COMMON__MTU_VALUEf, vxlan_port->mtu);
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                            COMMON__MTU_ENABLEf, 0x1);
    } else {
        /* Disable mtu checking if mtu = 0 */
        soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                            COMMON__MTU_ENABLEf, 0x0);
    }


    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    BCM_IF_ERROR_RETURN(rv);

    return rv;

}

/*
 * Function:
 *     _bcm_td2_vxlan_egress_dvp_set
 * Purpose:
 *     Set Egress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  vxlan_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_egress_dvp_set(int unit, int vp, int drop, bcm_vxlan_port_t *vxlan_port)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int rv=BCM_E_UNAVAIL;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    egr_dvp_attribute_1_entry_t  egr_dvp_attribute_1;
    bcm_ip_t dip=0;                       /* DIP for tunnel header */
    int tunnel_index = -1;
    int local_drop = 0;
    int network_group=0;

    vxlan_info = VXLAN_INFO(unit);

    tunnel_index = BCM_GPORT_TUNNEL_ID_GET(vxlan_port->egress_tunnel_id);
    _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_index);

    /* Obtain Tunnel DIP */
    dip = vxlan_info->vxlan_tunnel_init[tunnel_index].dip;

    /* Got HW tunnel index */
    BCM_IF_ERROR_RETURN(
           _bcm_td2_vxlan_tunnel_initiator_idx_translate(
               unit, tunnel_index, &tunnel_index));
   
    /* Zero write buffer. */
    if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                EGR_DVP_ATTRIBUTEm, MEM_BLOCK_ANY, vp, &egr_dvp_attribute));
    } else {
        /* Zero write buffer. */
        sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm, DATA_TYPEf)) {
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, DATA_TYPEf, _BCM_VXLAN_EGRESS_DEST_VP_TYPE);
    } else
#endif
    {
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, VP_TYPEf, _BCM_VXLAN_EGRESS_DEST_VP_TYPE);
    }
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            VXLAN__TUNNEL_INDEXf, tunnel_index);
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            VXLAN__DIPf, dip);

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)){
            network_group = vxlan_port->network_group_id;
            rv = _bcm_validate_splithorizon_network_group(unit,
                    vxlan_port->flags & BCM_VXLAN_PORT_NETWORK, &network_group);
            BCM_IF_ERROR_RETURN(rv);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                    VXLAN__DVP_NETWORK_GROUPf, network_group);
    } else {
        if (vxlan_port->flags & BCM_VXLAN_PORT_NO_SPLIT_HORIZON_EGRESS) {
           soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                               VXLAN__DVP_IS_NETWORK_PORTf, 0x0);
        } else {
           soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                               VXLAN__DVP_IS_NETWORK_PORTf, 0x1);
        }
    }
    
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            VXLAN__DISABLE_VP_PRUNINGf, 0x0);
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            VXLAN__DELETE_VNTAGf, 0x1);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, VXLAN__CLASS_IDf, vxlan_port->if_class);

        /* L2 MTU - This is different from L3 MTU */
        if (vxlan_port->mtu > 0) {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                &egr_dvp_attribute, VXLAN__MTU_VALUEf, vxlan_port->mtu);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                &egr_dvp_attribute, VXLAN__MTU_ENABLEf, 0x1);
        } else {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                &egr_dvp_attribute, VXLAN__MTU_VALUEf, 0x0);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                &egr_dvp_attribute, VXLAN__MTU_ENABLEf, 0x0);
        }
        if (vxlan_port->flags & BCM_VXLAN_PORT_MULTICAST) {
            local_drop = drop ? 1 : 0;
        }
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, VXLAN__UUC_DROPf, local_drop);
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, VXLAN__UMC_DROPf, local_drop);
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
            &egr_dvp_attribute, VXLAN__BC_DROPf, local_drop);
    } else
#endif
    {
        if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_DVP_ATTRIBUTE_1m,
                                MEM_BLOCK_ANY, vp, &egr_dvp_attribute_1));
        } else {
            /* Zero write buffer. */
            sal_memset(&egr_dvp_attribute_1, 0, sizeof(egr_dvp_attribute_1_entry_t));
        }

        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
            &egr_dvp_attribute_1, VXLAN__CLASS_IDf, vxlan_port->if_class);
        /* L2 MTU - This is different from L3 MTU */
        if (vxlan_port->mtu > 0) {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
                &egr_dvp_attribute_1, VXLAN__MTU_VALUEf, vxlan_port->mtu);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
                &egr_dvp_attribute_1, VXLAN__MTU_ENABLEf, 0x1);
        } else {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
                &egr_dvp_attribute_1, VXLAN__MTU_VALUEf, 0x0);
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
                &egr_dvp_attribute_1, VXLAN__MTU_ENABLEf, 0x0);
        }
        if (vxlan_port->flags & BCM_VXLAN_PORT_MULTICAST) {
            local_drop = drop ? 1 : 0;
        }
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
            &egr_dvp_attribute_1, VXLAN__UUC_DROPf, local_drop);
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
            &egr_dvp_attribute_1, VXLAN__UMC_DROPf, local_drop);
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m,
            &egr_dvp_attribute_1, VXLAN__BC_DROPf, local_drop);

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_DVP_ATTRIBUTE_1m,
                            MEM_BLOCK_ALL, vp, &egr_dvp_attribute_1));
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if(SOC_MEM_FIELD_VALID(unit,EGR_DVP_ATTRIBUTEm,VXLAN__EVXLT_KEY_SELf)) {
        if (vxlan_port->vnid <= 0xFFFFFF) {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
                   &egr_dvp_attribute, VXLAN__EVXLT_KEY_SELf, TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_DVP_VFI);
        } else {
            soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm,
               &egr_dvp_attribute, VXLAN__EVXLT_KEY_SELf, TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_VFI);
        }
    }
#endif

    rv = soc_mem_write(unit, EGR_DVP_ATTRIBUTEm,
            MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    return rv;
}


/*
 * Function:
 *     _bcm_td2_vxlan_egress_dvp_get
 * Purpose:
 *     Get Egress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  vxlan_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_egress_dvp_get(int unit, int vp, bcm_vxlan_port_t *vxlan_port)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info;
    int rv=BCM_E_NONE;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    egr_dvp_attribute_1_entry_t  egr_dvp_attribute_1;
    int tunnel_index=-1;
    bcm_ip_t dip=0;                       /* DIP for tunnel header */
    int idx=0, num_vp=0;
    int vp_type = 0; /* access / tunnel vp type*/
    int network_port = 0;
    _bcm_vp_info_t vp_info;

    vxlan_info = VXLAN_INFO(unit);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
    rv = soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                                            MEM_BLOCK_ANY, vp, &egr_dvp_attribute);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm, DATA_TYPEf)) {
        vp_type = soc_mem_field32_get(unit,
            EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute, DATA_TYPEf);
    } else
#endif
    {
        vp_type = soc_mem_field32_get(unit,
            EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute, VP_TYPEf);
    }

    /* Only mtu field is valid for access DVPs */
    if (vp_type == 0) {
        if (soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                    &egr_dvp_attribute, COMMON__MTU_ENABLEf)) {
            vxlan_port->mtu = soc_mem_field32_get(unit,
                EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute, COMMON__MTU_VALUEf);
        }
        return rv;
    }

    /* Extract Network port details */
    dip = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, 
                                            &egr_dvp_attribute, VXLAN__DIPf);

    /* Find DIP within Softare State */
    for (idx=0; idx < num_vp; idx++) {
        if (vxlan_info->vxlan_tunnel_init[idx].dip == dip) {
            tunnel_index = idx;
            break;
        }
    }

    BCM_GPORT_TUNNEL_ID_SET(vxlan_port->egress_tunnel_id, tunnel_index);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        vxlan_port->if_class = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                        &egr_dvp_attribute, VXLAN__CLASS_IDf);
        if (soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                    &egr_dvp_attribute, VXLAN__MTU_ENABLEf)) {
             vxlan_port->mtu = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                        &egr_dvp_attribute, VXLAN__MTU_VALUEf);
        }
        if (soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                &egr_dvp_attribute, VXLAN__BC_DROPf)
            || soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                               &egr_dvp_attribute, VXLAN__UUC_DROPf)
            || soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                               &egr_dvp_attribute, VXLAN__UMC_DROPf)) {
             vxlan_port->flags |= BCM_VXLAN_PORT_DROP;
        }
    } else
#endif
    {
        sal_memset(&egr_dvp_attribute_1, 0, sizeof(egr_dvp_attribute_1_entry_t));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_DVP_ATTRIBUTE_1m,
                            MEM_BLOCK_ANY, vp, &egr_dvp_attribute_1));

        vxlan_port->if_class = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTE_1m,
                                        &egr_dvp_attribute_1, VXLAN__CLASS_IDf);
        if (soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTE_1m,
                                    &egr_dvp_attribute_1, VXLAN__MTU_ENABLEf)) {
             vxlan_port->mtu = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTE_1m,
                                        &egr_dvp_attribute_1, VXLAN__MTU_VALUEf);
        }
        if (soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTE_1m,
                                &egr_dvp_attribute_1, VXLAN__BC_DROPf)
            || soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTE_1m,
                               &egr_dvp_attribute_1, VXLAN__UUC_DROPf)
            || soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTE_1m,
                               &egr_dvp_attribute_1, VXLAN__UMC_DROPf)) {
             vxlan_port->flags |= BCM_VXLAN_PORT_DROP;
        }
    }

    if (soc_mem_field_valid(unit, EGR_DVP_ATTRIBUTEm, VXLAN__DVP_IS_NETWORK_PORTf) &&
        (vp_type == _BCM_VXLAN_EGRESS_DEST_VP_TYPE)) {
        network_port = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                           &egr_dvp_attribute,
                                           VXLAN__DVP_IS_NETWORK_PORTf);
        BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
        if ((vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) &&
            network_port == 0) {
            vxlan_port->flags |= BCM_VXLAN_PORT_NO_SPLIT_HORIZON_EGRESS;
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_egress_dvp_reset
 * Purpose:
 *      Reet Egress DVP tables
 * Parameters:
 *      IN :  Unit
 *           IN :  vp
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_egress_dvp_reset(int unit, int vp)
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

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _bcm_td3_vxlan_tpid_delete
 * Purpose:
 *      del TPID
 * Parameters:
 *      IN :  Unit
 *      IN :  ptr of vxlate entry
 *      IN :  ptr of egr nh entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_vxlan_tpid_delete(int unit, uint32* vxlate, uint32* egr_nh){
    int rv = BCM_E_NONE;
    int profile_idx = -1;
    int action_present = 0;
    int action_not_present = 0;
    int tpid_index = -1;

    if (vxlate != NULL) {
        soc_mem_t vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm; 
        profile_idx = soc_mem_field32_get(unit, vxlate_mem, vxlate,
                                                     VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf);
        (void)_bcm_td3_sd_tag_action_get(unit, profile_idx, &action_present,
                       &action_not_present);

        if (profile_idx != 0) {
            BCM_IF_ERROR_RETURN(
                _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                    profile_idx));
        }

        if (1 == action_present || 4 == action_present ||
            7 == action_present || 1 == action_not_present) {
            tpid_index = soc_mem_field32_get(unit,
                             vxlate_mem, vxlate, 
                              VXLAN_VFI_FLEX__OUTER_TPID_INDEXf);
            rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                return rv;
            }
        }
    } else if (egr_nh != NULL) {
        soc_mem_t vxlate_mem = EGR_L3_NEXT_HOPm; 
        profile_idx = soc_mem_field32_get(unit, vxlate_mem, egr_nh,
                                                     L2_OTAG__TAG_ACTION_PROFILE_PTRf);
        (void)_bcm_td3_sd_tag_action_get(unit, profile_idx, &action_present,
                       &action_not_present);

        if (profile_idx != 0) {
            BCM_IF_ERROR_RETURN(
                _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                    profile_idx));
        }

        if (1 == action_present || 4 == action_present ||
            7 == action_present || 1 == action_not_present) {
            tpid_index = soc_mem_field32_get(unit,
                             vxlate_mem, egr_nh, 
                             L2_OTAG__TPID_INDEXf);
            rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                return rv;
            }
        }
    }

    return rv;
}
#endif

/*
 * Function:
 *      _bcm_td2_vxlan_egr_xlate_entry_reset
 * Purpose:
 *      Reset VXLAN  egr_xlate Entry 
 * Parameters:
 *      IN :  Unit
 *      IN :  vpn id
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_egr_xlate_entry_reset(int unit, bcm_vpn_t vpn)
{
    int index = 0;
    int vfi = 0;
    int tpid_index = -1;
    soc_mem_t vxlate_mem;
    uint32 vxlate[SOC_MAX_MEM_WORDS];
    int action_present = 0, action_not_present = 0;
    int d = 0; /*vxlan decoupled mode*/

    sal_memset(vxlate, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
        d = 1;
        soc_mem_field32_set(unit, vxlate_mem, vxlate, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, vxlate_mem, vxlate,
                            KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI);
        soc_mem_field32_set(unit, vxlate_mem, vxlate,
                            DATA_TYPEf, _BCM_VXLAN_DATA_TYPE_LOOKUP_VFI);
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
        soc_mem_field32_set(unit, vxlate_mem, vxlate, VALIDf, 0x1);
        soc_mem_field32_set(unit, vxlate_mem, vxlate,
                    ENTRY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI);
    }
    soc_mem_field32_set(unit, vxlate_mem, vxlate, BCM_VXLAN_VFI_FIELD(d,VFIf), vfi);

    BCM_IF_ERROR_RETURN(soc_mem_search(unit, vxlate_mem,
                MEM_BLOCK_ANY, &index, vxlate, vxlate, 0));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        _bcm_td3_vxlan_tpid_delete(unit, vxlate, NULL);
    
    } else
#endif
    {
        /* If tpid entry exist, delete it */
        action_present = soc_mem_field32_get(unit, vxlate_mem,
                            vxlate, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf);
        action_not_present = soc_mem_field32_get(unit, vxlate_mem,
                            vxlate, VXLAN_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf);
        if (1 == action_present || 4 == action_present ||
            7 == action_present || 1 == action_not_present) {
            tpid_index = soc_mem_field32_get(unit, vxlate_mem,
                            vxlate, VXLAN_VFI__SD_TAG_TPID_INDEXf);
            BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit, tpid_index));
        }
    }

    /* Delete the entry from HW */
    BCM_IF_ERROR_RETURN(soc_mem_delete(unit, vxlate_mem, MEM_BLOCK_ALL, vxlate));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_port_untagged_profile_set
 * Purpose:
 *      Set  VLan profile per Physical port entry
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port  - (IN) Physical port
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_td2_vxlan_port_untagged_profile_set (int unit, bcm_port_t port)
{
    int rv=BCM_E_NONE;
    bcm_vlan_action_set_t action;
    uint32 ing_profile_idx = 0xffffffff;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
          return BCM_E_NONE;
    }
#endif

    bcm_vlan_action_set_t_init(&action);
    action.ut_outer = 0x0; /* No Op */
    action.ut_inner = 0x0; /* No Op */

    rv = _bcm_trx_vlan_action_profile_entry_add(unit, &action, &ing_profile_idx);
    if (rv < 0) {
        return rv;
    }

    rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                               TAG_ACTION_PROFILE_PTRf, ing_profile_idx);
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_port_untagged_profile_reset
 * Purpose:
 *      Reset  VLan profile per Physical port entry
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port  - (IN) Physical port
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_td2_vxlan_port_untagged_profile_reset(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    int ing_profile_idx = -1;
    int def_profile_idx = 0;

    SOC_IF_ERROR_RETURN(
        _bcm_esw_port_tab_get(unit, port, TAG_ACTION_PROFILE_PTRf,
                              &ing_profile_idx));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && (ing_profile_idx == 0)) {
        /*index 0 is invalid in Ingress Vlan Tag Action Profile*/
        return rv;
    }
#endif
    rv = _bcm_trx_vlan_action_profile_entry_delete(unit, ing_profile_idx);
    if (rv < 0) {
        return rv;
    }
    /* default profile defined in _bcm_trx_vlan_action_profile_init */
    if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm)) {
        def_profile_idx = 1;
    }

    rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                               TAG_ACTION_PROFILE_PTRf, def_profile_idx);

    return rv;
}

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _bcm_td3_vxlan_vlan_action_set
 * Purpose:
 *      Program  SD_TAG settings
 * Parameters:
 *      unit           - (IN)  SOC unit #
 *      vxlan_port  - (IN)  VXLAN VP
 *      egr_nh_info  (IN/OUT) Egress NHop Info
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_td3_vxlan_vlan_action_set( int unit, bcm_vxlan_vpn_config_t *vxlan_vpn_info, 
                         bcm_vxlan_port_t *vxlan_port, 
                         uint32   *vxlate_entry,
                         int *tpid_index )
{
    soc_mem_t  hw_mem = EGR_VLAN_XLATE_1_SINGLEm;
    soc_field_t vid_fld = VXLAN_VFI_FLEX__OVIDf;
    soc_field_t pri_fld = VXLAN_VFI_FLEX__OPRIf;
    soc_field_t cfi_fld = VXLAN_VFI_FLEX__OCFIf;
    soc_field_t pri_cfi_sel_fld = VXLAN_VFI_FLEX__OPRI_OCFI_SELf;
    int action_if_not_prt = 0;
    int action_prt = 0;
    uint32 profile_index = -1;

    if (vxlan_vpn_info != NULL ) {
        if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_ADD) {
            if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            if (vxlan_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                   return  BCM_E_PARAM;
            }
            if (vxlan_vpn_info->pkt_cfi > 1) {
               return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_vpn_info->egress_service_vlan);
            if (soc_mem_field_valid(unit, hw_mem,
                                       pri_fld)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, pri_fld,
                            vxlan_vpn_info->pkt_pri);
            }
            if (soc_mem_field_valid(unit, hw_mem,
                                   cfi_fld)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, cfi_fld,
                            vxlan_vpn_info->pkt_cfi);
            }
            if (soc_mem_field_valid(unit, hw_mem,
                                   pri_cfi_sel_fld)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, pri_cfi_sel_fld,
                            0x1);
            }
            action_if_not_prt = 0x1; /* ADD */
        }

        if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_TPID_REPLACE) {
            if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_vpn_info->egress_service_vlan);
            action_prt = 0x1; /* REPLACE_VID_TPID */
        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_REPLACE) {
            if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_vpn_info->egress_service_vlan);
            action_prt = 0x2; /* REPLACE_VID_ONLY */
        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_DELETE) {
            action_prt = 0x3; /* DELETE */
        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_TPID_REPLACE) {
               if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                   return  BCM_E_PARAM;
               }
               if (vxlan_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                   return  BCM_E_PARAM;
               }
               if (vxlan_vpn_info->pkt_cfi > 1) {
                   return  BCM_E_PARAM;
               }
               soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_vpn_info->egress_service_vlan);
               if (soc_mem_field_valid(unit, hw_mem,
                                       pri_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_fld,
                                vxlan_vpn_info->pkt_pri);
               }
               if (soc_mem_field_valid(unit, hw_mem,
                                       cfi_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, cfi_fld,
                                vxlan_vpn_info->pkt_cfi);
               }
               if (soc_mem_field_valid(unit, hw_mem,
                                       pri_cfi_sel_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_cfi_sel_fld,
                                0x1);
               }
               action_prt = 0x4; /* REPLACE_VID_PRI_TPID */

        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_REPLACE ){
                if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_vpn_info->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_vpn_info->egress_service_vlan);

                if (soc_mem_field_valid(unit, hw_mem,
                                       pri_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_fld,
                                vxlan_vpn_info->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       cfi_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, cfi_fld,
                                vxlan_vpn_info->pkt_cfi);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       pri_cfi_sel_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_cfi_sel_fld,
                                0x1);
               }

                action_prt = 0x5; /* REPLACE_VID_PRI */
        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_PRI_REPLACE ) {
                if (vxlan_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_vpn_info->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       pri_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_fld,
                                vxlan_vpn_info->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       cfi_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, cfi_fld,
                                vxlan_vpn_info->pkt_cfi);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       pri_cfi_sel_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_cfi_sel_fld,
                                0x1);
                }
                action_prt = 0x6; /* REPLACE_PRI_ONLY */
        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_TPID_REPLACE ) {
                action_prt = 0x7; /* REPLACE_TPID_ONLY */
        }
        
        profile_index = soc_mem_field32_get(unit, hw_mem, vxlate_entry,
                        VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf);
        _bcm_td3_sd_tag_action_profile_get(unit, action_prt,
                         action_if_not_prt, &profile_index);
        soc_mem_field32_set(unit, hw_mem, vxlate_entry, VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf,
                         profile_index);

        if ((vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_ADD) ||
            (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_TPID_REPLACE) ||
            (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_TPID_REPLACE) ||
            (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_TPID_REPLACE)) {
            /* TPID value is used */
            BCM_IF_ERROR_RETURN(
                _bcm_fb2_outer_tpid_entry_add(unit, vxlan_vpn_info->egress_service_tpid, tpid_index));
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI_FLEX__OUTER_TPID_INDEXf, *tpid_index);
        }
    } else if (vxlan_port != NULL) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) {
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                   return  BCM_E_PARAM;
            }
            if (vxlan_port->pkt_cfi > 1) {
               return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_port->egress_service_vlan);
            if (soc_mem_field_valid(unit, hw_mem,
                                       pri_fld)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, pri_fld,
                            vxlan_port->pkt_pri);
            }
            if (soc_mem_field_valid(unit, hw_mem,
                                   cfi_fld)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, cfi_fld,
                            vxlan_port->pkt_cfi);
            }
            if (soc_mem_field_valid(unit, hw_mem,
                                   pri_cfi_sel_fld)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, pri_cfi_sel_fld,
                            0x1);
            }
            action_if_not_prt = 0x1; /* ADD */
        }

        if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) {
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_port->egress_service_vlan);
            action_prt = 0x1; /* REPLACE_VID_TPID */
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE) {
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_port->egress_service_vlan);
            action_prt = 0x2; /* REPLACE_VID_ONLY */
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_DELETE) {
            action_prt = 0x3; /* DELETE */
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE) {
               if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                   return  BCM_E_PARAM;
               }
               if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                   return  BCM_E_PARAM;
               }
               if (vxlan_port->pkt_cfi > 1) {
                   return  BCM_E_PARAM;
               }
               soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_port->egress_service_vlan);
               if (soc_mem_field_valid(unit, hw_mem,
                                       pri_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_fld,
                                vxlan_port->pkt_pri);
               }
               if (soc_mem_field_valid(unit, hw_mem,
                                       cfi_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, cfi_fld,
                                vxlan_port->pkt_cfi);
               }
               if (soc_mem_field_valid(unit, hw_mem,
                                       pri_cfi_sel_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_cfi_sel_fld,
                                0x1);
               }

               action_prt = 0x4; /* REPLACE_VID_PRI_TPID */

        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE ){
                if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_port->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, vid_fld,
                                vxlan_port->egress_service_vlan);

                if (soc_mem_field_valid(unit, hw_mem,
                                       pri_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_fld,
                                vxlan_port->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       cfi_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, cfi_fld,
                                vxlan_port->pkt_cfi);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       pri_cfi_sel_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_cfi_sel_fld,
                                0x1);
               }

               action_prt = 0x5; /* REPLACE_VID_PRI */
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_PRI_REPLACE ) {
                if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_port->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       pri_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_fld,
                                vxlan_port->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       cfi_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, cfi_fld,
                                vxlan_port->pkt_cfi);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       pri_cfi_sel_fld)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, pri_cfi_sel_fld,
                                0x1);
               }
                action_prt = 0x6; /* REPLACE_PRI_ONLY */
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TPID_REPLACE ) {
                action_prt = 0x7; /* REPLACE_TPID_ONLY */
        }

        profile_index = soc_mem_field32_get(unit, hw_mem, vxlate_entry,
                        VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf);
        _bcm_td3_sd_tag_action_profile_get(unit, action_prt,
                         action_if_not_prt, &profile_index);
        soc_mem_field32_set(unit, hw_mem, vxlate_entry, VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf,
                         profile_index);
        if ((vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) ||
            (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) ||
            (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TPID_REPLACE) ||
            (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE)) {
            /* TPID value is used */
            BCM_IF_ERROR_RETURN(
                _bcm_fb2_outer_tpid_entry_add(unit, vxlan_port->egress_service_tpid, tpid_index));
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI_FLEX__OUTER_TPID_INDEXf, *tpid_index);
        }
    }
    return  BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_vxlan_vlan_action_get
 * Purpose:
 *      Get  SD_TAG settings
 * Parameters:
 *      unit           - (IN)  SOC unit #
 *      vxlan_port  - (IN)  VXLAN VP
 *      egr_nh_info  (IN/OUT) Egress NHop Info
 * Returns:
 *      BCM_E_XXX
 */

STATIC void
_bcm_td3_vxlan_vlan_action_get( int unit, bcm_vxlan_vpn_config_t *vxlan_vpn_info,
                         bcm_vxlan_port_t *vxlan_port,
                         egr_l3_next_hop_entry_t *egr_nh,
                         uint32   *vxlate_entry,
                         int network_port_flag )
{
    int action_present=0, action_not_present=0, tpid_index = 0;
    soc_mem_t vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
    int profile_index = -1;
    uint32 flags                   = 0;
    bcm_vlan_t egress_service_vlan = 0;
    uint8 pkt_pri                  = 0;
    uint8 pkt_cfi                  = 0;
    uint16 egress_service_tpid     = 0;
    soc_field_t vid_fld = VXLAN_VFI_FLEX__OVIDf;
    soc_field_t pri_fld = VXLAN_VFI_FLEX__OPRIf;
    soc_field_t cfi_fld = VXLAN_VFI_FLEX__OCFIf;

    if (network_port_flag) {
       profile_index = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                               VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf);
       (void)_bcm_td3_sd_tag_action_get(unit, profile_index, &action_present,
                           &action_not_present);

       if (action_not_present == 1) {
            flags |= BCM_VXLAN_VPN_SERVICE_TAGGED;
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_ADD;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, vid_fld);
       }

       if (action_present) {
            flags |= BCM_VXLAN_VPN_SERVICE_TAGGED;
       }
       switch (action_present) {
         case 1:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_TPID_REPLACE;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, vid_fld);
            break;

         case 2:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_REPLACE;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, vid_fld);
            break;

         case 3:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_DELETE;
            egress_service_vlan = BCM_VLAN_INVALID;
            break;

         case 4:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_PRI_TPID_REPLACE;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, vid_fld);
            pkt_pri = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, pri_fld);
            pkt_cfi = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, cfi_fld);
            break;

         case 5:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_PRI_REPLACE;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, vid_fld);
            pkt_pri = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, pri_fld);
            pkt_cfi = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, cfi_fld);
            break;

         case 6:
            flags |= BCM_VXLAN_VPN_SERVICE_PRI_REPLACE;
            egress_service_vlan =   BCM_VLAN_INVALID;
            pkt_pri = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, pri_fld);
            pkt_cfi = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, cfi_fld);
            break;

         case 7:
            flags |= BCM_VXLAN_VPN_SERVICE_TPID_REPLACE;
            egress_service_vlan = BCM_VLAN_INVALID;
            break;

         default:
            break;
       }

       if ((flags & BCM_VXLAN_VPN_SERVICE_VLAN_ADD) ||
            (flags & BCM_VXLAN_VPN_SERVICE_VLAN_TPID_REPLACE) ||
            (flags & BCM_VXLAN_VPN_SERVICE_TPID_REPLACE) ||
            (flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_TPID_REPLACE)) {
            tpid_index = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                                                VXLAN_VFI_FLEX__OUTER_TPID_INDEXf);
            _bcm_fb2_outer_tpid_entry_get(unit,
                &egress_service_tpid, tpid_index);
       }

       if (vxlan_vpn_info != NULL) {
           vxlan_vpn_info->flags               |= flags;
           vxlan_vpn_info->egress_service_vlan  = egress_service_vlan;
           vxlan_vpn_info->egress_service_tpid  = egress_service_tpid;
           vxlan_vpn_info->pkt_pri              = pkt_pri;
           vxlan_vpn_info->pkt_cfi              = pkt_cfi;
       }
       if (vxlan_port != NULL) {
           vxlan_port->flags               |= flags;
           vxlan_port->egress_service_vlan  = egress_service_vlan;
           vxlan_port->egress_service_tpid  = egress_service_tpid;
           vxlan_port->pkt_pri              = pkt_pri;
           vxlan_port->pkt_cfi              = pkt_cfi;
       }

    } else {
       vid_fld = L2_OTAG__VIDf;
       pri_fld = L2_OTAG__PCPf;
       cfi_fld = L2_OTAG__DEf;
       profile_index = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                               L2_OTAG__TAG_ACTION_PROFILE_PTRf);
       (void)_bcm_td3_sd_tag_action_get(unit, profile_index, &action_present,
                           &action_not_present);

       if (action_not_present == 1) {
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_ADD;
            vxlan_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, vid_fld);
       }

       switch (action_present) {
         case 1:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE;
            vxlan_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, vid_fld);
            break;

         case 2:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE;
            vxlan_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, vid_fld);
            break;

         case 3:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_DELETE;
            vxlan_port->egress_service_vlan = BCM_VLAN_INVALID;
            break;

         case 4:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE;
            vxlan_port->egress_service_vlan =
                 soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, vid_fld);
            vxlan_port->pkt_pri = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, pri_fld);
            vxlan_port->pkt_cfi = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, cfi_fld);
            break;

         case 5:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE;
            vxlan_port->egress_service_vlan = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, vid_fld);
            vxlan_port->pkt_pri = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, pri_fld);
            vxlan_port->pkt_cfi = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, cfi_fld);
            break;

         case 6:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_PRI_REPLACE;
            vxlan_port->egress_service_vlan =   BCM_VLAN_INVALID;
            vxlan_port->pkt_pri = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, pri_fld);
            vxlan_port->pkt_cfi = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, cfi_fld);
            break;

         case 7:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_TPID_REPLACE;
            vxlan_port->egress_service_vlan = BCM_VLAN_INVALID;
            break;

         default:
            break;
       }

       if (soc_feature(unit, soc_feature_vp_sharing) &&
              soc_feature(unit, soc_feature_egr_intf_vlan_vfi_deoverlay)) {
           if ((vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) ||
              (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) ||
              (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE) ||
              (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE) ||
              (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE)) {
               vxlan_port->egress_service_vlan = (vxlan_port->egress_service_vlan == 0) ?
                                                 BCM_VLAN_INVALID : vxlan_port->egress_service_vlan;
           }
       }

       if ((vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) ||
           (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) ||
           (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TPID_REPLACE) ||
           (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE)) {
           /* TPID value is used */
           tpid_index = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, L2_OTAG__TPID_INDEXf);
           _bcm_fb2_outer_tpid_entry_get(unit, &vxlan_port->egress_service_tpid, tpid_index);
       }
    }
}

#endif

/*
 * Function:
 *      _bcm_td2_vxlan_sd_tag_set
 * Purpose:
 *      Program  SD_TAG settings
 * Parameters:
 *      unit           - (IN)  SOC unit #
 *      vxlan_port  - (IN)  VXLAN VP
 *      egr_nh_info  (IN/OUT) Egress NHop Info
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_sd_tag_set( int unit, bcm_vxlan_vpn_config_t *vxlan_vpn_info, 
                         bcm_vxlan_port_t *vxlan_port, 
                         _bcm_td2_vxlan_nh_info_t  *egr_nh_info, 
                         uint32   *vxlate_entry,
                         int *tpid_index )
{
    soc_mem_t  hw_mem;

    if (vxlan_vpn_info != NULL ) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            int rv = BCM_E_NONE;
            rv = _bcm_td3_vxlan_vlan_action_set(unit, vxlan_vpn_info, NULL, vxlate_entry, tpid_index);
            return rv;
        }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_base_valid)) {
            hw_mem = EGR_VLAN_XLATE_1_DOUBLEm;
        } else
#endif
        {
            hw_mem = EGR_VLAN_XLATEm;
        }

        if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_ADD) {
            if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            if (vxlan_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                   return  BCM_E_PARAM;
            }
            if (vxlan_vpn_info->pkt_cfi > 1) {
               return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_vpn_info->egress_service_vlan);
            if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_PRIf)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf,
                            vxlan_vpn_info->pkt_pri);
            }
            if (soc_mem_field_valid(unit, hw_mem,
                                   VXLAN_VFI__SD_TAG_NEW_CFIf)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf,
                            vxlan_vpn_info->pkt_cfi);
            }
            if (soc_mem_field_valid(unit, hw_mem,
                                   VXLAN_VFI__SD_TAG_REMARK_CFIf)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, VXLAN_VFI__SD_TAG_REMARK_CFIf,
                            0x1);
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf,
                                0x1); /* ADD */
        }

        if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_TPID_REPLACE) {
            if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_vpn_info->egress_service_vlan);
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x1); /* REPLACE_VID_TPID */
        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_REPLACE) {
            if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_vpn_info->egress_service_vlan);
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x2); /* REPLACE_VID_ONLY */
        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_DELETE) {
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x3); /* DELETE */
        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_TPID_REPLACE) {
               if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                   return  BCM_E_PARAM;
               }
               if (vxlan_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                   return  BCM_E_PARAM;
               }
               if (vxlan_vpn_info->pkt_cfi > 1) {
                   return  BCM_E_PARAM;
               }
               soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_vpn_info->egress_service_vlan);
               if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_PRIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf,
                                vxlan_vpn_info->pkt_pri);
               }
               if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf,
                                vxlan_vpn_info->pkt_cfi);
               }
               if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_REMARK_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_REMARK_CFIf,
                                0x1);
               }

               soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x4); /* REPLACE_VID_PRI_TPID */

        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_REPLACE ){
                if (vxlan_vpn_info->egress_service_vlan >= BCM_VLAN_INVALID) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_vpn_info->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_vpn_info->egress_service_vlan);

                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_PRIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf,
                                vxlan_vpn_info->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf,
                                vxlan_vpn_info->pkt_cfi);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_REMARK_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_REMARK_CFIf,
                                0x1);
               }

                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x5); /* REPLACE_VID_PRI */
        }else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_PRI_REPLACE ) {
                if (vxlan_vpn_info->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_vpn_info->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_PRIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf,
                                vxlan_vpn_info->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf,
                                vxlan_vpn_info->pkt_cfi);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_REMARK_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_REMARK_CFIf,
                                0x1);
                }

                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x6); /* REPLACE_PRI_ONLY */
      
        } else if (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_TPID_REPLACE ) {
                 soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x7); /* REPLACE_TPID_ONLY */
        }

        if ((vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_ADD) ||
            (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_TPID_REPLACE) ||
            (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_TPID_REPLACE) ||
            (vxlan_vpn_info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_TPID_REPLACE)) {
            /* TPID value is used */
            BCM_IF_ERROR_RETURN(
                _bcm_fb2_outer_tpid_entry_add(unit, vxlan_vpn_info->egress_service_tpid, tpid_index));
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_TPID_INDEXf, *tpid_index);
        }
    } else if (vxlan_port != NULL && egr_nh_info != NULL) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) {
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) { 
#if defined(BCM_TRIDENT2PLUS_SUPPORT) 
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID;    
                } else
#endif
                {
                    return  BCM_E_PARAM;
                }
                    
            }
            if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                return  BCM_E_PARAM;
            }
            if (vxlan_port->pkt_cfi > 1) {
                return  BCM_E_PARAM;
            }
            if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
                egr_nh_info->sd_tag_vlan = vxlan_port->egress_service_vlan; 
            }
            egr_nh_info->sd_tag_pri = vxlan_port->pkt_pri;
            egr_nh_info->sd_tag_cfi = vxlan_port->pkt_cfi;
            egr_nh_info->sd_tag_action_not_present = 0x1; /* ADD */
        }

        if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) {
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) { 
#if defined(BCM_TRIDENT2PLUS_SUPPORT) 
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID; 
                } else
#endif
                {
                    return  BCM_E_PARAM;
                }
            }
            /* REPLACE_VID_TPID */
            if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
                egr_nh_info->sd_tag_vlan = vxlan_port->egress_service_vlan;
            }
            egr_nh_info->sd_tag_action_present = 0x1;
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE) {

            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) 
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID; 
                } else
#endif
                {
                    return  BCM_E_PARAM;
                }
            }
            /* REPLACE_VID_ONLY */
            if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
                egr_nh_info->sd_tag_vlan = vxlan_port->egress_service_vlan;
            }
            egr_nh_info->sd_tag_action_present = 0x2;
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_DELETE) {
            egr_nh_info->sd_tag_action_present = 0x3; /* DELETE */
            egr_nh_info->sd_tag_action_not_present = 0;
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE) { 
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) { 
#if defined(BCM_TRIDENT2PLUS_SUPPORT) 
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID; 
                } else
#endif
                {
                    return  BCM_E_PARAM;
                }
            }
              
            if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                return  BCM_E_PARAM;
            }
            if (vxlan_port->pkt_cfi <= 1) {
                egr_nh_info->sd_tag_cfi = vxlan_port->pkt_cfi;
            } else if (vxlan_port->pkt_cfi == (uint8)-1) {
                egr_nh_info->sd_tag_cfi = -1;
            } else {
                return BCM_E_PARAM;
            }

            /* REPLACE_VID_PRI_TPID */          
            if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
                egr_nh_info->sd_tag_vlan = vxlan_port->egress_service_vlan;
            }
            egr_nh_info->sd_tag_pri = vxlan_port->pkt_pri;
            egr_nh_info->sd_tag_action_present = 0x4;
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE ) {
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)             
                if (soc_feature(unit, soc_feature_vp_sharing)) {
                    egr_nh_info->sd_tag_vlan = BCM_VLAN_INVALID; 
                } else 
#endif
                {
                    return  BCM_E_PARAM;
                }
            }
            if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                return  BCM_E_PARAM;
            }
            if (vxlan_port->pkt_cfi <= 1) {
                egr_nh_info->sd_tag_cfi = vxlan_port->pkt_cfi;
            } else if (vxlan_port->pkt_cfi == (uint8)-1) {
                egr_nh_info->sd_tag_cfi = -1;
            } else {
                return BCM_E_PARAM;
            }

            /* REPLACE_VID_PRI_ONLY */
            if (egr_nh_info->sd_tag_vlan != BCM_VLAN_INVALID) {
                egr_nh_info->sd_tag_vlan = vxlan_port->egress_service_vlan;
            }
            egr_nh_info->sd_tag_pri = vxlan_port->pkt_pri;
            egr_nh_info->sd_tag_action_present = 0x5;
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_PRI_REPLACE ) {
                if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_port->pkt_cfi <= 1) {
                    egr_nh_info->sd_tag_cfi = vxlan_port->pkt_cfi;
                } else if (vxlan_port->pkt_cfi == (uint8)-1) {
                    egr_nh_info->sd_tag_cfi = -1;
                } else {
                    return BCM_E_PARAM;
                }

                 /* REPLACE_PRI_ONLY */
                egr_nh_info->sd_tag_pri = vxlan_port->pkt_pri;
                egr_nh_info->sd_tag_action_present = 0x6;
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TPID_REPLACE ) {
                 /* REPLACE_TPID_ONLY */
                egr_nh_info->sd_tag_action_present = 0x7;
        }

        if ((vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) ||
            (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) ||
            (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TPID_REPLACE) ||
            (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE)) {
            /* TPID value is used */
            BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_add(unit, vxlan_port->egress_service_tpid, tpid_index));
            egr_nh_info->tpid_index = *tpid_index;
        }
    } else if (vxlan_port != NULL) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            int rv = BCM_E_NONE;
            rv = _bcm_td3_vxlan_vlan_action_set(unit, NULL, vxlan_port, vxlate_entry, tpid_index);
            return rv;
        }
#endif 
        hw_mem = EGR_VLAN_XLATEm;
        if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) {
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                   return  BCM_E_PARAM;
            }
            if (vxlan_port->pkt_cfi > 1) {
               return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_port->egress_service_vlan);
            if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_PRIf)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf,
                            vxlan_port->pkt_pri);
            }
            if (soc_mem_field_valid(unit, hw_mem,
                                   VXLAN_VFI__SD_TAG_NEW_CFIf)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf,
                            vxlan_port->pkt_cfi);
            }
            if (soc_mem_field_valid(unit, hw_mem,
                                   VXLAN_VFI__SD_TAG_REMARK_CFIf)) {
                soc_mem_field32_set(unit, hw_mem,
                            vxlate_entry, VXLAN_VFI__SD_TAG_REMARK_CFIf,
                            0x1);
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf,
                                0x1); /* ADD */
        }

        if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) {
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_port->egress_service_vlan);
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x1); /* REPLACE_VID_TPID */
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE) {
            if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                return  BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_port->egress_service_vlan);
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x2); /* REPLACE_VID_ONLY */
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_DELETE) {
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x3); /* DELETE */
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE) {
               if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                   return  BCM_E_PARAM;
               }
               if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                   return  BCM_E_PARAM;
               }
               if (vxlan_port->pkt_cfi > 1) {
                   return  BCM_E_PARAM;
               }
               soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_port->egress_service_vlan);
               if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_PRIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf,
                                vxlan_port->pkt_pri);
               }
               if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf,
                                vxlan_port->pkt_cfi);
               }
               if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_REMARK_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_REMARK_CFIf,
                                0x1);
               }

               soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x4); /* REPLACE_VID_PRI_TPID */

        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE ){
                if (vxlan_port->egress_service_vlan >= BCM_VLAN_INVALID) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_port->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_VIDf,
                                vxlan_port->egress_service_vlan);

                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_PRIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf,
                                vxlan_port->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf,
                                vxlan_port->pkt_cfi);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_REMARK_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_REMARK_CFIf,
                                0x1);
               }

                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x5); /* REPLACE_VID_PRI */
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_PRI_REPLACE ) {
                if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
                    return  BCM_E_PARAM;
                }
                if (vxlan_port->pkt_cfi > 1) {
                    return  BCM_E_PARAM;
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_PRIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf,
                                vxlan_port->pkt_pri);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_NEW_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf,
                                vxlan_port->pkt_cfi);
                }
                if (soc_mem_field_valid(unit, hw_mem,
                                       VXLAN_VFI__SD_TAG_REMARK_CFIf)) {
                   soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_REMARK_CFIf,
                                0x1);
               }

                soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x6); /* REPLACE_PRI_ONLY */
      
        } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TPID_REPLACE ) {
                 soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf,
                                0x7); /* REPLACE_TPID_ONLY */
        }

        if ((vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) ||
            (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) ||
            (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TPID_REPLACE) ||
            (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE)) {
            /* TPID value is used */
            BCM_IF_ERROR_RETURN(
                _bcm_fb2_outer_tpid_entry_add(unit, vxlan_port->egress_service_tpid, tpid_index));
            soc_mem_field32_set(unit, hw_mem,
                                vxlate_entry, VXLAN_VFI__SD_TAG_TPID_INDEXf, *tpid_index);
        }
    }
    return  BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_sd_tag_get
 * Purpose:
 *      Get  SD_TAG settings
 * Parameters:
 *      unit           - (IN)  SOC unit #
 *      vxlan_port  - (IN)  VXLAN VP
 *      egr_nh_info  (IN/OUT) Egress NHop Info
 * Returns:
 *      BCM_E_XXX
 */     

STATIC void
_bcm_td2_vxlan_sd_tag_get( int unit, bcm_vxlan_vpn_config_t *vxlan_vpn_info, 
                         bcm_vxlan_port_t *vxlan_port, 
                         egr_l3_next_hop_entry_t *egr_nh, 
                         uint32   *vxlate_entry,
                         int network_port_flag )
{
    int action_present=0, action_not_present=0, tpid_index = 0;
    soc_mem_t vxlate_mem;
    uint32 flags                   = 0;
    bcm_vlan_t egress_service_vlan = 0;
    uint16 egress_service_tpid     = 0;
    uint8 pkt_pri                  = 0;
    uint8 pkt_cfi                  = 0;
    
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        _bcm_td3_vxlan_vlan_action_get(unit, vxlan_vpn_info, vxlan_port, egr_nh,
                                  vxlate_entry, network_port_flag);
        return;
    }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        vxlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
    }

    if (network_port_flag) {
       action_present = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                                        VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf);
       action_not_present = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                                        VXLAN_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf);

       if (action_not_present == 1) {
            flags |= BCM_VXLAN_VPN_SERVICE_TAGGED;
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_ADD;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_VIDf);
       }

       if (action_present) {
            flags |= BCM_VXLAN_VPN_SERVICE_TAGGED;
       }
       switch (action_present) {
         case 1:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_TPID_REPLACE;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_VIDf);
            break;

         case 2:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_REPLACE;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_VIDf);
            break;

         case 3:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_DELETE;
            egress_service_vlan = BCM_VLAN_INVALID;
            break;

         case 4:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_PRI_TPID_REPLACE;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_VIDf);
            pkt_pri = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf);
            pkt_cfi = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf);
            break;

         case 5:
            flags |= BCM_VXLAN_VPN_SERVICE_VLAN_PRI_REPLACE;
            egress_service_vlan = soc_mem_field32_get(unit,
                            vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_VIDf);
            pkt_pri = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf);
            pkt_cfi = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf);
            break;

         case 6:
            flags |= BCM_VXLAN_VPN_SERVICE_PRI_REPLACE;
            egress_service_vlan =   BCM_VLAN_INVALID;
            pkt_pri = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_NEW_PRIf);
            pkt_cfi = soc_mem_field32_get(unit,
                vxlate_mem, vxlate_entry, VXLAN_VFI__SD_TAG_NEW_CFIf);
            break;

         case 7:
            flags |= BCM_VXLAN_VPN_SERVICE_TPID_REPLACE;
            egress_service_vlan = BCM_VLAN_INVALID;
            break;

         default:
            break;
       }

       if ((flags & BCM_VXLAN_VPN_SERVICE_VLAN_ADD) ||
            (flags & BCM_VXLAN_VPN_SERVICE_VLAN_TPID_REPLACE) ||
            (flags & BCM_VXLAN_VPN_SERVICE_TPID_REPLACE) ||
            (flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_TPID_REPLACE)) {
            tpid_index = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                                                VXLAN_VFI__SD_TAG_TPID_INDEXf);
            _bcm_fb2_outer_tpid_entry_get(unit,
                &egress_service_tpid, tpid_index);
       }
       if (vxlan_vpn_info) {
           vxlan_vpn_info->flags |= flags;
           vxlan_vpn_info->egress_service_vlan = egress_service_vlan;
           vxlan_vpn_info->egress_service_tpid = egress_service_tpid;
           vxlan_vpn_info->pkt_cfi = pkt_cfi;
           vxlan_vpn_info->pkt_pri = pkt_pri;
       }

       if (vxlan_port) {
           vxlan_port->flags |= flags;
           vxlan_port->egress_service_vlan = egress_service_vlan;
           vxlan_port->egress_service_tpid = egress_service_tpid;
           vxlan_port->pkt_cfi = pkt_cfi;
           vxlan_port->pkt_pri = pkt_pri;
       }

    } else {
       action_present = 
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, 
                                     egr_nh,
                                     SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
       action_not_present = 
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, 
                                     egr_nh,
                                     SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);

       if (action_not_present == 1) {
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_ADD;
            vxlan_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
       }

       switch (action_present) {
         case 1:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE;
            vxlan_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
            break;

         case 2:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE;
            vxlan_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
            break;

         case 3:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_DELETE;
            vxlan_port->egress_service_vlan = BCM_VLAN_INVALID;
            break;

         case 4:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE;
            vxlan_port->egress_service_vlan =                 
                 soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
            vxlan_port->pkt_pri = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_PRIf);
            vxlan_port->pkt_cfi = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_CFIf);
            break;

         case 5:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE;
            vxlan_port->egress_service_vlan = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_VIDf);
            vxlan_port->pkt_pri = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_PRIf);
            vxlan_port->pkt_cfi = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_CFIf);
            break;

         case 6:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_PRI_REPLACE;
            vxlan_port->egress_service_vlan =   BCM_VLAN_INVALID;
            vxlan_port->pkt_pri = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_PRIf);
            vxlan_port->pkt_cfi = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__NEW_CFIf);
            break;

         case 7:
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_TPID_REPLACE;
            vxlan_port->egress_service_vlan = BCM_VLAN_INVALID;
            break;

         default:
            break;
       }

#if defined(BCM_TRIDENT2PLUS_SUPPORT) 
    if (soc_feature(unit, soc_feature_vp_sharing) &&
        SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, SD_TAG__SD_TAG_NOT_VALIDf)) {
        int sd_tag_invalid = -1;
        sd_tag_invalid = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, 
                                                  SD_TAG__SD_TAG_NOT_VALIDf);
        if (sd_tag_invalid) {
            vxlan_port->egress_service_vlan = BCM_VLAN_INVALID;
        }
    }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
       if (soc_feature(unit, soc_feature_vp_sharing) &&
              soc_feature(unit, soc_feature_egr_intf_vlan_vfi_deoverlay)) {
           if ((vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) ||
              (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) ||
              (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE) ||
              (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE) ||
              (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE)) {
               vxlan_port->egress_service_vlan = (vxlan_port->egress_service_vlan == 0) ?
                                                 BCM_VLAN_INVALID : vxlan_port->egress_service_vlan;
           }
       }
#endif

       if ((vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) ||
           (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) ||
           (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TPID_REPLACE) ||
           (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE)) {
           /* TPID value is used */
           tpid_index = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, SD_TAG__SD_TAG_TPID_INDEXf);
           _bcm_fb2_outer_tpid_entry_get(unit, &vxlan_port->egress_service_tpid, tpid_index);
       }
    }
}

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _bcm_td3_vxlan_nexthop_l2otag_set 
 * Purpose:
 *     set l2otag in nexthop
 * Parameters:
 *      unit - Device Number
 *      egr_nh_info - ptr of Next Hop Info
 *      egr_nh - ptr of egr nh 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_vxlan_nexthop_l2otag_set(int unit,
                  _bcm_td2_vxlan_nh_info_t  *egr_nh_info, egr_l3_next_hop_entry_t* egr_nh)
{
    uint32 profile_idx;
    uint8 pcp_de_sel = 0;

    if (egr_nh_info->sd_tag_vlan != -1) { 
        if (egr_nh_info->sd_tag_vlan < BCM_VLAN_INVALID) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                        egr_nh, L2_OTAG__VIDf, egr_nh_info->sd_tag_vlan);
        }
    }

    if ((egr_nh_info->sd_tag_action_not_present != -1) || (egr_nh_info->sd_tag_action_present != -1)) {
        profile_idx = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                        L2_OTAG__TAG_ACTION_PROFILE_PTRf);
        _bcm_td3_sd_tag_action_profile_get(unit, egr_nh_info->sd_tag_action_present, 
                                                      egr_nh_info->sd_tag_action_not_present, &profile_idx);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                         L2_OTAG__TAG_ACTION_PROFILE_PTRf,
                         profile_idx);
    }

    if (egr_nh_info->sd_tag_pri != -1) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
               L2_OTAG__PCPf,
               egr_nh_info->sd_tag_pri);
        pcp_de_sel = 1;
    }

    if (egr_nh_info->sd_tag_cfi != -1) {
       soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
             L2_OTAG__DEf,
             egr_nh_info->sd_tag_cfi);
       pcp_de_sel = 1;
    }

    if (pcp_de_sel != 0) {
       soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
             L2_OTAG__PCP_DE_SELf,
             pcp_de_sel);
    }

    if (egr_nh_info->tpid_index != -1) {
          soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                        egr_nh, L2_OTAG__TPID_INDEXf,
                        egr_nh_info->tpid_index);
    }

    return BCM_E_NONE;

}
#endif

/*
 * Function:
 *      _bcm_td2_vxlan_nexthop_entry_modify
 * Purpose:
 *      Modify Egress entry
 * Parameters:
 *      unit - Device Number
 *      nh_index - Next Hop Index
 *      new_entry_type - New Entry type
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_nexthop_entry_modify(int unit, int nh_index, int drop, 
              _bcm_td2_vxlan_nh_info_t  *egr_nh_info, int new_entry_type)
{
    egr_l3_next_hop_entry_t egr_nh;
    int rv = BCM_E_NONE;
    uint32 old_entry_type=0, intf_num=0, vntag_actions=0;
    uint32 vntag_dst_vif=0, vntag_pf=0, vntag_force_lf=0;
    uint32 etag_pcp_de_sourcef=0, etag_pcpf=0, etag_dot1p_mapping_ptr=0, etag_def=0;
    bcm_mac_t mac_addr;                 /* Next hop forwarding destination mac. */
    uint32 hg_ppd_type = 0;
    soc_field_t type_field;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    ing_l3_next_hop_entry_t ing_nh;
    bcm_port_t port = 0;
    bcm_module_t modid = 0;
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    int d = 0; /*vxlan decoupled mode*/

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        d = 1;
    }
#endif

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

    if ((new_entry_type == BCM_VXLAN_TAG_VIEW(d)) &&
        (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW)) {
            hg_ppd_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__HG_HDR_SELf);
            /*Zero buffers.*/
            sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            /*mapping to L2OTA TAP*/
                BCM_IF_ERROR_RETURN(_bcm_td3_vxlan_nexthop_l2otag_set(unit, egr_nh_info, &egr_nh));
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L2_OTAG__FLOW_SELECT_CTRL_IDf, 8);
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L2_OTAG__NEXT_PTR_TYPEf, 1);
            } else
#endif      
            {
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
                       if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__SD_TAG_REMARK_CFIf)) {
                           soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                 SD_TAG__SD_TAG_REMARK_CFIf,
                                 0x1);
                       }
                }

                if (egr_nh_info->tpid_index != -1) {
                      soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf,
                                    egr_nh_info->tpid_index);
                }
            }

            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                    BCM_VXLAN_TAG_FIELD(d, BC_DROPf), drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                    BCM_VXLAN_TAG_FIELD(d, UUC_DROPf), drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                    BCM_VXLAN_TAG_FIELD(d, UMC_DROPf), drop ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    BCM_VXLAN_TAG_FIELD(d, DVPf), egr_nh_info->dvp);

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
                    BCM_VXLAN_TAG_FIELD(d, HG_LEARN_OVERRIDEf), egr_nh_info->is_eline ? 1 : 0);
            /* HG_MODIFY_ENABLE must be 0x0 for Ingress and Egress Chip */
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                    BCM_VXLAN_TAG_FIELD(d, HG_MODIFY_ENABLEf), 0x0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                    BCM_VXLAN_TAG_FIELD(d, HG_HDR_SELf), hg_ppd_type);

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                    BCM_VXLAN_TAG_FIELD(d, HG_MC_DST_PORT_NUMf))) {
                int is_trunk = 0;
                BCM_IF_ERROR_RETURN(
                    soc_mem_read(unit, ING_L3_NEXT_HOPm,
                                 MEM_BLOCK_ANY, nh_index, &ing_nh));

#if defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    uint32 dt, dv;

                    dv = soc_mem_field32_dest_get(unit,
                            ING_L3_NEXT_HOPm, &ing_nh, DESTINATIONf, &dt);
                    if (dt == SOC_MEM_FIF_DEST_DGPP) {
                        port = (dv & SOC_MEM_FIF_DGPP_PORT_MASK);
                        modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                    SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                        is_trunk = 0;
                    } else if (dt == SOC_MEM_FIF_DEST_LAG) {
                        is_trunk = 1;
                    }
                } else
#endif
                {
                    if (!soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf)) {
                        port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                                   &ing_nh, PORT_NUMf);
                        modid = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                                    &ing_nh, MODULE_IDf);
                        is_trunk = 0;
                    } else {
                        is_trunk = 1;
                    }
                }

                if (!is_trunk) {
                    if (_bcm_xgs5_subport_coe_mod_port_local(unit, modid, port)) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            BCM_VXLAN_TAG_FIELD(d, HG_MC_DST_PORT_NUMf), port);
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            BCM_VXLAN_TAG_FIELD(d, HG_MC_DST_MODIDf), modid);
                    }
                }
            }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            type_field, BCM_VXLAN_TAG_VIEW(d));
    } else if ((new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) &&
        (old_entry_type == BCM_VXLAN_TAG_VIEW(d))) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            type_field, _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW);

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                    BCM_VXLAN_TAG_FIELD(d, HG_MC_DST_PORT_NUMf))) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    BCM_VXLAN_TAG_FIELD(d, HG_MC_DST_PORT_NUMf), 0);
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    BCM_VXLAN_TAG_FIELD(d, HG_MC_DST_MODIDf), 0);
            }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
            if( SOC_IS_APACHE(unit) && (egr_nh_info == NULL)) {
               soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__INTF_NUMf, 0);
           }
    } else if ((new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) &&
         (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW)) {

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
                         L3__VNTAG_Pf);

        vntag_force_lf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__VNTAG_FORCE_Lf);

        etag_pcp_de_sourcef = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__ETAG_PCP_DE_SOURCEf);

        etag_pcpf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__ETAG_PCPf);

        etag_dot1p_mapping_ptr = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__ETAG_DOT1P_MAPPING_PTRf);

        etag_def = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__ETAG_DEf);
        hg_ppd_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L3__HG_HDR_SELf);
        /*Zero buffers.*/
        sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));

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
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    L3MC__L2_MC_DA_DISABLEf, 0x1);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    L3MC__L2_MC_SA_DISABLEf, 0x1);
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
                                L3MC__ETAG_DEf, etag_def);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_HDR_SELf, hg_ppd_type);

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__HG_MC_DST_PORT_NUMf) &&
            soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__USE_MAC_DA_PROFILEf)) {
            int is_trunk = 0;
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, ING_L3_NEXT_HOPm,
                             MEM_BLOCK_ANY, nh_index, &ing_nh));
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_generic_dest)) {
                uint32 dt, dv;

                dv = soc_mem_field32_dest_get(unit,
                        ING_L3_NEXT_HOPm, &ing_nh, DESTINATIONf, &dt);
                if (dt == SOC_MEM_FIF_DEST_DGPP) {
                    port = (dv & SOC_MEM_FIF_DGPP_PORT_MASK);
                    modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                    is_trunk = 0;
                } else if (dt == SOC_MEM_FIF_DEST_LAG) {
                    is_trunk = 1;
                }
            } else
#endif
            {
                if (!soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf)) {
                    port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                               &ing_nh, PORT_NUMf);
                    modid = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                                &ing_nh, MODULE_IDf);
                    is_trunk = 0;
                } else {
                    is_trunk = 1;
                }
            }

            if (!is_trunk) {
                if (_bcm_xgs5_subport_coe_mod_port_local(unit, modid, port)) {
                    uint32 macda_index;
                    void *entries[1];
                    egr_mac_da_profile_entry_t macda;

                    sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
                    soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm,
                             &macda, MAC_ADDRESSf, mac_addr);
                    entries[0] = &macda;
                    BCM_IF_ERROR_RETURN(
                        _bcm_mac_da_profile_entry_add(unit, entries, 1, &macda_index));

                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                        L3MC__HG_MC_DST_PORT_NUMf, port);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                        L3MC__HG_MC_DST_MODIDf, modid);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                        L3MC__USE_MAC_DA_PROFILEf, 1);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                        L3MC__MAC_DA_PROFILE_INDEXf, macda_index);
                }
            }
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                type_field, _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW);

    } else if ((new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) &&
        (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW)) {
        
        /* Update entry with DVP info if it is already IPMC entry */ 
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
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    L3MC__L2_MC_DA_DISABLEf, 0x1);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    L3MC__L2_MC_SA_DISABLEf, 0x1);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    L3MC__L2_MC_VLAN_DISABLEf, 0x1);
        }
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    L3MC__L2_DROPf, 0x0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                                L3MC__L3_DROPf, drop ? 1 : 0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                                L3MC__L3MC_USE_CONFIGURED_MACf, 0x1);
        
    } else if ((new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) &&
         (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW)) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            type_field, _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__HG_MC_DST_PORT_NUMf) &&
            soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__USE_MAC_DA_PROFILEf)) {
            if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    L3MC__USE_MAC_DA_PROFILEf)) {
                uint32 macda_index;

                macda_index = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                    L3MC__MAC_DA_PROFILE_INDEXf);
                BCM_IF_ERROR_RETURN(
                    _bcm_mac_da_profile_entry_delete(unit, macda_index));
            }
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
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

        etag_pcp_de_sourcef = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                                  &egr_nh,
                                                  L3MC__ETAG_PCP_DE_SOURCEf);

        etag_pcpf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                        L3MC__ETAG_PCPf);

        etag_dot1p_mapping_ptr = soc_mem_field32_get(
            unit, EGR_L3_NEXT_HOPm, &egr_nh, L3MC__ETAG_DOT1P_MAPPING_PTRf);

        etag_def = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                       L3MC__ETAG_DEf);
        hg_ppd_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                          L3MC__HG_HDR_SELf);
        /*Zero buffers.*/
        sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));

        soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             L3__MAC_ADDRESSf, mac_addr);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__INTF_NUMf, intf_num);

        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__VNTAG_ACTIONSf, vntag_actions);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__VNTAG_DST_VIFf, vntag_dst_vif);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__VNTAG_Pf, vntag_pf);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__VNTAG_FORCE_Lf, vntag_force_lf);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__ETAG_PCP_DE_SOURCEf, etag_pcp_de_sourcef);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__ETAG_PCPf, etag_pcpf);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__ETAG_DOT1P_MAPPING_PTRf,
                            etag_dot1p_mapping_ptr);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__ETAG_DEf, etag_def);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__HG_HDR_SELf, hg_ppd_type);

        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            type_field, _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW);
    } else if ((new_entry_type == BCM_VXLAN_TAG_VIEW(d)) &&
        (old_entry_type == BCM_VXLAN_TAG_VIEW(d))) {
        /* Case of VXLAN Proxy Next-Hop */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            BCM_IF_ERROR_RETURN(_bcm_td3_vxlan_nexthop_l2otag_set(unit, egr_nh_info, &egr_nh));
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L2_OTAG__FLOW_SELECT_CTRL_IDf, 8);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                         L2_OTAG__NEXT_PTR_TYPEf, 1);
        } else
#endif
        {
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
            } else {
                   soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 0x0);

            }

            if (egr_nh_info->sd_tag_action_not_present != -1) {
                   soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf,
                             egr_nh_info->sd_tag_action_not_present);
            } else {
                   soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf, 0x0);
            }

            if (egr_nh_info->sd_tag_pri != -1) {
                 if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__NEW_PRIf)) {
                       soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__NEW_PRIf,
                             egr_nh_info->sd_tag_pri);
                 }
            } else {
                 if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__NEW_PRIf)) {
                       soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__NEW_PRIf, 0x0);
                 }
            }

            if (egr_nh_info->sd_tag_cfi != -1) {
                   if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__NEW_CFIf)) {
                       soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__NEW_CFIf,
                             egr_nh_info->sd_tag_cfi);
                   }
                   if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__SD_TAG_REMARK_CFIf)) {
                       soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             SD_TAG__SD_TAG_REMARK_CFIf,
                             0x1);
                   }
            } else {
                   if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__SD_TAG_REMARK_CFIf)) {
                          soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__SD_TAG_REMARK_CFIf,
                                0x0);
                   }
            }

            if (egr_nh_info->tpid_index != -1) {
                  soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf,
                                egr_nh_info->tpid_index);
            }
        }
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                BCM_VXLAN_TAG_FIELD(d, DVPf), egr_nh_info->dvp);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                BCM_VXLAN_TAG_FIELD(d, BC_DROPf), drop ? 1 : 0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                BCM_VXLAN_TAG_FIELD(d, UUC_DROPf), drop ? 1 : 0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                BCM_VXLAN_TAG_FIELD(d, UMC_DROPf), drop ? 1 : 0);

#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
            } else
#endif
        {
            /* On an exisitng entry, if the network-port/network-group
               has changed, update the same here */
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

    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, &egr_nh);
    return rv;
}

/*
 * Function:
 *           bcm_td2_vxlan_next_hop_set
 * Purpose:
 *           Set VXLAN NextHop Entry
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_td2_vxlan_next_hop_set(int unit, int nh_index, uint32 flags, int vp, int drop)
{
    int rv = BCM_E_NONE;
    ing_l3_next_hop_entry_t ing_nh;
    bcm_port_t  port = -1;
    bcm_module_t modid = -1, local_modid = -1;
    bcm_trunk_t tgid = -1;
    int  num_ports=0, idx=-1;
    bcm_port_t   local_ports[SOC_MAX_NUM_PORTS];
    int  old_nh_index=-1;
    int replace=0;
    int gport = 0;
    _bcm_td2_vxlan_nh_info_t  egr_nh_info;
    _bcm_vp_info_t vp_info;
    soc_mem_t hw_mem = ING_L3_NEXT_HOPm;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_trunk_member_t *tmp_member_gports_array = NULL;
#endif

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
        hw_mem, MEM_BLOCK_ANY, nh_index, &ing_nh));
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
        ENTRY_TYPEf, _BCM_VXLAN_INGRESS_NEXT_HOP_ENTRY_TYPE);
    if (SOC_MEM_FIELD_VALID(unit, hw_mem, MTU_SIZEf)) {
        soc_mem_field32_set(unit, hw_mem, &ing_nh, MTU_SIZEf, 0x3fff);
    }
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, hw_mem, MEM_BLOCK_ALL, nh_index, &ing_nh));

    if (flags & BCM_L3_IPMC) {
        egr_l3_next_hop_entry_t egr_nh;
        int entry_type = 0;

        (void)_bcm_vp_info_get(unit, vp, &vp_info);

        if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                                              nh_index, &egr_nh));
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
                entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh, DATA_TYPEf);
            } else
#endif
            {
                entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh, ENTRY_TYPEf);
            }
            if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, L3__HG_HDR_SELf, 1);
            }
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                              MEM_BLOCK_ALL, nh_index, &egr_nh));
            egr_nh_info.dvp = vp;
            BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_nexthop_entry_modify(unit,
                nh_index, drop, &egr_nh_info, _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW));
        }
    } else { /* Only for VXLAN Unicast Nexthops */
        replace = flags & BCM_L3_REPLACE;

        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_nexthop_glp_get(unit,
                                nh_index, &modid, &port, &tgid));

        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_modid));
        if (tgid != -1) {
             BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, tgid));
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
            if (old_nh_index && !replace) {
                if (old_nh_index != nh_index) {
                    /* Limitation: For TD/TR3/TD2,
                       VxLAN egress port can be configured with one NHI */
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
 *           bcm_td2_vxlan_nexthop_get
 * Purpose:
 *          Get VXLAN NextHop entry
 * Parameters:
 *          unit       - (IN) SOC unit
 *          egr_nh     - (IN) Egress NHop Info
 *          vxlan_port - (IN/OUT) VXLAN port
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_td2_vxlan_nexthop_get(int unit, egr_l3_next_hop_entry_t *egr_nh,
                          bcm_vxlan_port_t *vxlan_port)
{
    soc_mem_t      hw_mem;
    uint32 entry_type;
    int d = 0; /*vxlan decoupled mode*/

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        d = 1;
    }
#endif

    hw_mem = EGR_L3_NEXT_HOPm;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        entry_type = soc_mem_field32_get(unit, hw_mem, egr_nh, DATA_TYPEf);
    } else
#endif
    {
        entry_type = soc_mem_field32_get(unit, hw_mem, egr_nh, ENTRY_TYPEf);
    }

    if (entry_type == BCM_VXLAN_TAG_VIEW(d)) {
        if (soc_mem_field32_get(unit, hw_mem, egr_nh, BCM_VXLAN_TAG_FIELD(d, BC_DROPf))
            || soc_mem_field32_get(unit, hw_mem, egr_nh, BCM_VXLAN_TAG_FIELD(d, UUC_DROPf))
            || soc_mem_field32_get(unit, hw_mem, egr_nh, BCM_VXLAN_TAG_FIELD(d, UMC_DROPf))) {
            vxlan_port->flags |= BCM_VXLAN_PORT_DROP;
        }

        if (!soc_mem_field32_get(unit, hw_mem, egr_nh, BCM_VXLAN_TAG_FIELD(d, HG_HDR_SELf))) {
            vxlan_port->flags |= BCM_VXLAN_PORT_ENCAP_LOCAL;
        }
    } else if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) {
        if (soc_mem_field32_get(unit, hw_mem, egr_nh, L3MC__L3_DROPf)) {
            vxlan_port->flags |= BCM_VXLAN_PORT_DROP;
        }

        if (!soc_mem_field32_get(unit, hw_mem, egr_nh, L3MC__HG_HDR_SELf)) {
            vxlan_port->flags |= BCM_VXLAN_PORT_ENCAP_LOCAL;
        }

        vxlan_port->flags |= BCM_VXLAN_PORT_MULTICAST;
    } else if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) {
        if (!soc_mem_field32_get(unit, hw_mem, egr_nh, L3__HG_HDR_SELf)) {
            vxlan_port->flags |= BCM_VXLAN_PORT_ENCAP_LOCAL;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *           _bcm_td2_vxlan_port_egress_nexthop_reset
 * Purpose:
 *           Reset VXLAN Egress NextHop 
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
_bcm_td2_vxlan_port_egress_nexthop_reset(int unit, int nh_index, int vp_type, int vp, bcm_vpn_t vpn)
{
    egr_l3_next_hop_entry_t egr_nh;
    int rv=BCM_E_NONE;
    int action_present=0, action_not_present=0, old_tpid_idx = -1;
    uint32  entry_type=0;
    bcm_vxlan_port_t  vxlan_port;
    int d = 0; /*vxlan decoupled mode*/

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        d = 1;
    }
#endif

    BCM_IF_ERROR_RETURN(
        _bcm_td2_vxlan_port_get(unit, vpn, vp, &vxlan_port));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                                MEM_BLOCK_ANY, nh_index, &egr_nh));
    /* egressing into a regular port */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf);
    } else
#endif
    {
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
    }
    if (vp_type== _BCM_VXLAN_DEST_VP_TYPE_ACCESS) {
        if (entry_type == BCM_VXLAN_TAG_VIEW(d)) {
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                _bcm_td3_vxlan_tpid_delete(unit, NULL, (uint32*)&egr_nh);
            } else
#endif
            {      
                action_present = 
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
                action_not_present = 
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
                if ((action_not_present == 0x1) || (action_present == 0x1) ||
                    (action_present == 0x4) || (action_present == 0x7)) {
                    old_tpid_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                        &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf);
                    BCM_IF_ERROR_RETURN(
                        _bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx));
                }
            }
        }
        /* Normalize Next-hop Entry -- L3 View */
        rv = _bcm_td2_vxlan_nexthop_entry_modify(unit, nh_index, 0, 
                             NULL, _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW);

    } else if ( vp_type == _BCM_VXLAN_INGRESS_DEST_VP_TYPE ){
        /* egressing into a tunnel, vp type should be _BCM_VXLAN_INGRESS_DEST_VP_TYPE */
        /* Normalize Next-hop Entry -- L3 View */
        if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) {
            rv = _bcm_td2_vxlan_nexthop_entry_modify(unit, nh_index, 0, NULL, 
                                            _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW);
        }
    }

    return rv;
}

/*
 * Function:
 *           bcm_td2_vxlan_ecmp_port_egress_nexthop_reset
 * Purpose:
 *           Reset VXLAN ecmp Egress NextHop 
 * Parameters:
 *           IN :  Unit
 *           IN :  ecmp_index
 * Returns:
 *           BCM_E_XXX
 */

int
_bcm_td2_vxlan_ecmp_port_egress_nexthop_reset(int unit, int ecmp_index, int vp_type, int vp, bcm_vpn_t vpn)
{
    int i = 0, ecmp_member_count = 0;
    int alloc_size, index;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_if_t *ecmp_member_array = NULL;
    int rv = BCM_E_NONE;

    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_index)) {
        return BCM_E_PARAM;
    }
    ecmp_info.ecmp_intf = ecmp_index;
    alloc_size = sizeof(bcm_if_t)*BCM_XGS3_L3_ECMP_MAX(unit);
    ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
    if (NULL == ecmp_member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(ecmp_member_array, 0, alloc_size);

    rv = bcm_esw_l3_egress_ecmp_get(unit, &ecmp_info, BCM_XGS3_L3_ECMP_MAX(unit),
                                  ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    while(i < ecmp_member_count) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                    ecmp_member_array[i])) {
            index = (ecmp_member_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit));
        } else {
            rv = BCM_E_PARAM;
            goto clean_up;
        }

        rv = _bcm_td2_vxlan_port_egress_nexthop_reset(unit, index, vp_type,
                                     vp, vpn);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }
        i++;
    }

clean_up:
    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *           _bcm_td3_vxlan_ecmp_acess_port_egress_nexthop_reset
 * Purpose:
 *           Reset VXLAN access ecmp Egress NextHop
 * Parameters:
 *           IN :  Unit
 *           IN :  vp_type
 *           IN :  vp
 *           IN :  vpn
 * Returns:
 *           BCM_E_XXX
 */

int
_bcm_td3_vxlan_ecmp_acess_port_egress_nexthop_reset(int unit, int vp_type, int vp, bcm_vpn_t vpn){
    int rv = BCM_E_NONE;
    uint32 *vtab, *vtabp;
    int i, imin, imax, nent;
    soc_mem_t egr_l3_nh_mem = EGR_L3_NEXT_HOPm;
    int d = 0; /*vxlan decoupled mode*/

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        d = 1;
    }
#endif

    imin = soc_mem_index_min(unit, egr_l3_nh_mem);
    imax = soc_mem_index_max(unit, egr_l3_nh_mem);
    nent = soc_mem_index_count(unit, egr_l3_nh_mem);

    vtab = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, egr_l3_nh_mem), "egr_l3_next_hop");
    if (vtab == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(vtab, 0, SOC_MEM_TABLE_BYTES(unit, egr_l3_nh_mem));

    soc_mem_lock(unit, egr_l3_nh_mem);
    rv = soc_mem_read_range(unit, egr_l3_nh_mem, MEM_BLOCK_ANY,
                            imin, imax, vtab);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, egr_l3_nh_mem);
        soc_cm_sfree(unit, vtab);
        return rv;
    }

    for (i = 0; i < nent; i++) {
        vtabp = soc_mem_table_idx_to_pointer(unit, egr_l3_nh_mem,
                                             void *, vtab, i);
        if (soc_mem_field32_get(unit, egr_l3_nh_mem, vtabp, DATA_TYPEf)
                                        != BCM_VXLAN_TAG_VIEW(d)) {
            continue;
        }

        if (soc_mem_field32_get(unit, egr_l3_nh_mem, vtabp, BCM_VXLAN_TAG_FIELD(d, DVPf)) != vp) {
            continue;
        }
        /* Delete the entry from HW */
        rv = _bcm_td2_vxlan_port_egress_nexthop_reset(unit, i, vp_type,
                                     vp, vpn);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, egr_l3_nh_mem);
            soc_cm_sfree(unit, vtab);
            return rv;
        }
    }
    soc_mem_unlock(unit, egr_l3_nh_mem);
    soc_cm_sfree(unit, vtab);

    return rv;
}
#endif

/*
 * Function:
 *      bcm_td2_vxlan_nexthop_reset
 * Purpose:
 *      Reset VXLAN NextHop entry
 * Parameters:
 *      IN :  Unit
 *           IN :  nh_index
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_nexthop_reset(int unit, int nh_index)
{
    int rv=BCM_E_NONE;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    bcm_port_t      port=0;
    bcm_trunk_t tgid= BCM_TRUNK_INVALID;
    int  num_ports=0, idx=-1;
    bcm_port_t   local_ports[SOC_MAX_NUM_PORTS];
    soc_mem_t      hw_mem;
    bcm_module_t modid=0, local_modid=0;
    uint8 multicast_entry=0;
    int  old_nh_index=-1;
    int gport = 0;
    soc_field_t type_field;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_trunk_member_t *tmp_member_gports_array = NULL;
#endif

    hw_mem = EGR_L3_NEXT_HOPm;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, hw_mem, DATA_TYPEf)) {
        type_field = DATA_TYPEf;
    } else
#endif
    {
        type_field = ENTRY_TYPEf;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem, 
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));
    if (_BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW == soc_mem_field32_get(unit,
                hw_mem, &egr_nh, type_field)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
       if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
           if (0x1 == soc_mem_field32_get(unit, hw_mem, &egr_nh,
                            L3MC__NEXT_PTR_TYPEf)) {
               multicast_entry = 1; /* Multicast NextHop */
           }
       } else
#endif
       if (0x1 == soc_mem_field32_get(unit, hw_mem, &egr_nh,
                        L3MC__DVP_VALIDf)) {
           multicast_entry = 1; /* Multicast NextHop */
       }
    }

    hw_mem = ING_L3_NEXT_HOPm;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
                                            DROPf, 0x0);
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
                                            ENTRY_TYPEf, 0x0);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, hw_mem,
                                            MEM_BLOCK_ALL, nh_index, &ing_nh));
    rv = _bcm_td2_vxlan_nexthop_glp_get(unit,
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

    if (!multicast_entry) {
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
 *           bcm_td2_vxlan_egress_reset
 * Purpose:
 *           Reset VXLAN Egress NextHop 
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_td2_vxlan_egress_reset(int unit, int nh_index)
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

    rv = _bcm_td2_vxlan_nexthop_reset(unit, nh_index);
    return rv;
}

/*
 * Function:
 *           bcm_td2_vxlan_egress_get
 * Purpose:
 *           Get VXLAN Egress NextHop 
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_td2_vxlan_egress_get(int unit, bcm_l3_egress_t *egr, int nh_index)
{
    int int_l3_flag;
    int_l3_flag = BCM_XGS3_L3_ENT_FLAG(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                                       nh_index);
    if (int_l3_flag == _BCM_L3_VXLAN_ONLY) {
         egr->flags |= BCM_L3_VXLAN_ONLY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *           _bcm_td2_vxlan_higig_ppd_select
 * Purpose:
 *           Select PPD type for Higig header
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 *           IN :  flags
 * Returns:
 *           BCM_E_XXX
 */
int
_bcm_td2_vxlan_higig_ppd_select(int unit, int nh_index, uint32 flags)
{
    egr_l3_next_hop_entry_t egr_nh;
    int entry_type = 0;
    int d = 0; /*vxlan decoupled mode*/

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        d = 1;
    }
#endif    
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                  nh_index, &egr_nh));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        entry_type = soc_mem_field32_get(unit,
                        EGR_L3_NEXT_HOPm, &egr_nh, DATA_TYPEf);
    } else
#endif
    {
        entry_type = soc_mem_field32_get(unit,
                        EGR_L3_NEXT_HOPm, &egr_nh, ENTRY_TYPEf);
    }

    if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) {
        if (flags & BCM_VXLAN_PORT_ENCAP_LOCAL) {
            if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                L3__HG_L2_TUNNEL_ENCAP_ENABLEf)) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3__HG_L2_TUNNEL_ENCAP_ENABLEf, 1);
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3__HG_HDR_SELf, 0);
        } else {
            if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                L3__HG_L2_TUNNEL_ENCAP_ENABLEf)) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3__HG_L2_TUNNEL_ENCAP_ENABLEf, 0);
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3__HG_HDR_SELf, 1);
        }
    } else if (entry_type == BCM_VXLAN_TAG_VIEW(d)) {
        if (flags & BCM_VXLAN_PORT_ENCAP_LOCAL) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                BCM_VXLAN_TAG_FIELD(d, HG_HDR_SELf), 0);
        } else {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                BCM_VXLAN_TAG_FIELD(d, HG_HDR_SELf), 1);
        }
    } else if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) {
        if (flags & BCM_VXLAN_PORT_ENCAP_LOCAL) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_HDR_SELf, 0);
        } else {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_HDR_SELf, 1);
        }
    }

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, &egr_nh));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_ecmp_higig_ppd_select
 * Purpose:
 *      Modify vxlan ecmp member object's higig ppd select
 * Parameters:
 *      unit - Device Number
 *      ecmp_index - ECMP index
 *      flags - vxlan vp flags
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_ecmp_higig_ppd_select(int unit, int ecmp_index, uint32 flags)
{
    int i = 0, ecmp_member_count = 0;
    int alloc_size, index;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_if_t *ecmp_member_array = NULL;
    int rv = BCM_E_NONE;

    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_index)) {
        return BCM_E_PARAM;
    }
    ecmp_info.ecmp_intf = ecmp_index;
    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
    if (NULL == ecmp_member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(ecmp_member_array, 0, alloc_size);

    rv = bcm_esw_l3_egress_ecmp_get(unit, &ecmp_info, BCM_XGS3_L3_ECMP_MAX(unit),
                              ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    while(i < ecmp_member_count) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                    ecmp_member_array[i])) {
            index = (ecmp_member_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit));

        } else {
            rv = BCM_E_PARAM;
            goto clean_up;
        }
        rv = _bcm_td2_vxlan_higig_ppd_select(unit, index, flags);

        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }

        i++;
    }

clean_up:
    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_ecmp_nexthop_entry_modify
 * Purpose:
 *      Modify vxlan ecmp member object's Egress next hop entry
 * Parameters:
 *      unit - Device Number
 *      ecmp_nh_index - ECMP index
 *      drop - local drop
 *      egr_nh_info - egr nh info
 *      new_entry_type - New Entry type
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_ecmp_nexthop_entry_modify(int unit, int ecmp_index, int drop,
        _bcm_td2_vxlan_nh_info_t egr_nh_info, int new_entry_type)
{
    int i = 0, ecmp_member_count = 0;
    int alloc_size, index;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_if_t *ecmp_member_array = NULL;
    int rv = BCM_E_NONE;

    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_index)) {
        return BCM_E_PARAM;
    }
    ecmp_info.ecmp_intf = ecmp_index;
    alloc_size = sizeof(bcm_if_t)*BCM_XGS3_L3_ECMP_MAX(unit);
    ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
    if (NULL == ecmp_member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(ecmp_member_array, 0, alloc_size);

    rv = bcm_esw_l3_egress_ecmp_get(unit, &ecmp_info, BCM_XGS3_L3_ECMP_MAX(unit),
                                  ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    while(i < ecmp_member_count) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                    ecmp_member_array[i])) {
            index = (ecmp_member_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit));

        } else {
            rv = BCM_E_PARAM;
            goto clean_up;
        }
        rv = _bcm_td2_vxlan_nexthop_entry_modify(unit, index, drop,
                                     &egr_nh_info, new_entry_type);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }

        i++;
    }
clean_up:
    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_port_nh_add
 * Purpose:
 *      Add VXLAN Next Hop entry
 * Parameters:
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_td2_vxlan_port_nh_add(int unit, bcm_vxlan_port_t *vxlan_port, int vp,
                            bcm_vpn_t vpn, int drop)
{
    egr_l3_next_hop_entry_t egr_nh;
    _bcm_td2_vxlan_nh_info_t egr_nh_info;
    int rv=BCM_E_NONE;
    int action_present, action_not_present, tpid_index = -1;
    int old_tpid_idx = -1;
    uint32 mpath_flag=0;
    int vp_nh_ecmp_index=-1;
    int ref_count = 0;
#ifdef BCM_RIOT_SUPPORT
    int nh_ecmp_index = -1;
    ing_dvp_table_entry_t dvp;
#endif /* BCM_RIOT_SUPPORT */
    int network_group=0;
    int d = 0; /*vxlan decoupled mode*/
    
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            d = 1;
        }
#endif
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

    network_group = vxlan_port->network_group_id;
    rv = _bcm_validate_splithorizon_network_group(unit,
            vxlan_port->flags & BCM_VXLAN_PORT_NETWORK, &network_group);
    BCM_IF_ERROR_RETURN(rv);
    egr_nh_info.dvp_network_group_id = network_group;

#ifdef BCM_RIOT_SUPPORT
    if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
        SOC_IF_ERROR_RETURN(
            READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                  NEXT_HOP_INDEXf);
    }
#endif /* BCM_RIOT_SUPPORT */

    /* 
     * Get egress next-hop index from egress object and
     * increment egress object reference count. 
     */
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
        rv = bcm_xgs3_get_nh_from_egress_object(unit, vxlan_port->egress_if,
                                                &mpath_flag, 1, &vp_nh_ecmp_index);
        BCM_IF_ERROR_RETURN(rv);

        /* Read the existing egress next_hop entry */
        if (mpath_flag == 0) {
           if (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) {
               int l3_flag = BCM_XGS3_L3_ENT_FLAG(
                   BCM_XGS3_L3_TBL_PTR(unit, next_hop), vp_nh_ecmp_index);
               if (l3_flag != _BCM_L3_VXLAN_ONLY) {
                   rv = BCM_E_CONFIG;
                   goto cleanup;
               }
           }

           rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                          vp_nh_ecmp_index, &egr_nh);
           _BCM_VXLAN_CLEANUP(rv)
        }
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        else {
            /*
             * When Multi-level ECMP is enabled, ECMP group pointed
             * to by DVP must be an underlay group.
             */
            if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
#if defined(BCM_FIREBOLT6_SUPPORT)
                if (soc_feature(unit, soc_feature_fb6_ecmp_group_partitioning) &&
                        BCMI_L3_ECMP_GROUP_OVERLAY_IN_UPPER_RANGE(unit)) {
                    if (vp_nh_ecmp_index >= BCMI_L3_ECMP_OVERLAY_ENTRIES(unit)) {
                        return BCM_E_PARAM;
                    }
                } else
#endif
                    if (vp_nh_ecmp_index < BCMI_L3_ECMP_OVERLAY_ENTRIES(unit)) {
                        return BCM_E_PARAM;
                    }
            }
        }
#endif
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) {

        /* Program DVP entry - Egress */
        rv = _bcm_td2_vxlan_egress_dvp_set(unit, vp, drop, vxlan_port);
        _BCM_VXLAN_CLEANUP(rv)

        /* Program DVP entry  - Ingress  */
        rv = _bcm_td2_vxlan_ingress_dvp_set(unit, vp, mpath_flag, vp_nh_ecmp_index, vxlan_port);
        _BCM_VXLAN_CLEANUP(rv)
    
        rv = _bcm_td2_vxlan_ingress_dvp_2_set(unit, vp, mpath_flag, vp_nh_ecmp_index, vxlan_port);
        _BCM_VXLAN_CLEANUP(rv)
        
        if ((vxlan_port->flags & BCM_VXLAN_PORT_MULTICAST) &&
            (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE))) {
            /* Egress into Network - Tunnel */
            egr_nh_info.dvp_is_network = 1; /* Enable */
            if (mpath_flag == 0) {                
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

                rv = _bcm_td2_vxlan_nexthop_entry_modify(unit, vp_nh_ecmp_index, drop, 
                             &egr_nh_info, _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW);
                _BCM_VXLAN_CLEANUP(rv)
            } else {
                /*ecmp*/
                rv = _bcm_td2_vxlan_ecmp_nexthop_entry_modify(unit, vxlan_port->egress_if, drop, 
                                 egr_nh_info, _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW);
                _BCM_VXLAN_CLEANUP(rv)
            }
        }

        /* Don't select shared network multicast VP's HG header here*/
        if (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
            if (mpath_flag == 0) {
                rv = _bcm_td2_vxlan_higig_ppd_select(unit, vp_nh_ecmp_index, vxlan_port->flags);
                _BCM_VXLAN_CLEANUP(rv)
            } else {
                rv = _bcm_td2_vxlan_ecmp_higig_ppd_select(unit, vxlan_port->egress_if,
                                  vxlan_port->flags);
                _BCM_VXLAN_CLEANUP(rv) 
            }
        }
    } else {
        /* Program DVP entry - Egress */
        rv = _bcm_td2_vxlan_egress_access_dvp_set(unit, vp, drop, vxlan_port);
        _BCM_VXLAN_CLEANUP(rv)

        /* Egress into Access - Non Tunnel */
        egr_nh_info.dvp_is_network = 0; /* Disable */
        if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            action_present = 
                 soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
            action_not_present = 
                 soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
            if ((action_not_present == 0x1) || (action_present == 0x1) ||
                (action_present == 0x4) || (action_present == 0x7)) {
                /* If old tpid exist, delete it */
                old_tpid_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                          &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf);
                BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx));
            }
        }

        egr_nh_info.entry_type = BCM_VXLAN_TAG_VIEW(d);
        /* Configure SD_TAG setting */
        rv = _bcm_td2_vxlan_sd_tag_set(unit, NULL, vxlan_port, &egr_nh_info, NULL, &tpid_index);
        _BCM_VXLAN_CLEANUP(rv)
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit) && mpath_flag) {
            rv = _bcm_td2_vxlan_ecmp_nexthop_entry_modify(unit, vxlan_port->egress_if, drop,
                                 egr_nh_info, BCM_VXLAN_TAG_VIEW(d));
            _BCM_VXLAN_CLEANUP(rv)

            rv = _bcm_td2_vxlan_ecmp_higig_ppd_select(unit, vxlan_port->egress_if, vxlan_port->flags);
            _BCM_VXLAN_CLEANUP(rv)
        } else
#endif
        {
            /* Configure EGR Next Hop entry -- SD_TAG view */
            rv = _bcm_td2_vxlan_nexthop_entry_modify(unit, vp_nh_ecmp_index, drop,
                                 &egr_nh_info, BCM_VXLAN_TAG_VIEW(d));
            _BCM_VXLAN_CLEANUP(rv)
            rv = _bcm_td2_vxlan_higig_ppd_select(unit, vp_nh_ecmp_index, vxlan_port->flags);
            _BCM_VXLAN_CLEANUP(rv)
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_MULTICAST)) {
            /* Program DVP entry  with ECMP / Next Hop Index */
            rv = _bcm_td2_vxlan_ingress_dvp_set(unit, vp, mpath_flag, vp_nh_ecmp_index, vxlan_port);
            _BCM_VXLAN_CLEANUP(rv)

            rv = _bcm_td2_vxlan_ingress_dvp_2_set(unit, vp, mpath_flag, vp_nh_ecmp_index, vxlan_port);
            _BCM_VXLAN_CLEANUP(rv)

        }
    }

#ifdef BCM_RIOT_SUPPORT
    if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            rv = bcmi_l3_nh_assoc_ol_ul_link_replace(unit, nh_ecmp_index, 
                vp_nh_ecmp_index);
    }
#endif /* BCM_RIOT_SUPPORT */
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
 *      _bcm_td2_vxlan_port_nh_delete
 * Purpose:
 *      Delete VXLAN Next Hop entry
 * Parameters:
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_td2_vxlan_port_nh_delete(int unit, bcm_vpn_t vpn, int vp)
{
    int rv=BCM_E_NONE;
    int nh_ecmp_index=-1;
    ing_dvp_table_entry_t dvp;
    uint32  vp_type=0;
    uint32  flags=0;
    int  ref_count=0;
    int ecmp =-1;
    int ecmp_index_check_skip = 0;

    BCM_IF_ERROR_RETURN(
        READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    vp_type = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf);
    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
#if defined (BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit) && (!soc_feature(unit, soc_feature_vp_lag))) {
        ecmp_index_check_skip = 1;
    }
#endif

    if (ecmp) {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
        flags = BCM_L3_MULTIPATH;
        /* Decrement reference count */
        BCM_IF_ERROR_RETURN(
             bcm_xgs3_get_ref_count_from_nhi(unit, flags, &ref_count, nh_ecmp_index));
        if (ref_count == 1) {
            if (nh_ecmp_index || ecmp_index_check_skip) {
                if (vp_type == _BCM_VXLAN_INGRESS_DEST_VP_TYPE) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_td2_vxlan_ecmp_port_egress_nexthop_reset
                                (unit, (nh_ecmp_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit)),
                                                                      vp_type, vp, vpn));
                }
#if defined(BCM_TRIDENT3_SUPPORT)
                else if ((vp_type == _BCM_VXLAN_DEST_VP_TYPE_ACCESS) && SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_td3_vxlan_ecmp_acess_port_egress_nexthop_reset
                                (unit, vp_type, vp, vpn));
                }
#endif
            }
        }
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
                     _bcm_td2_vxlan_port_egress_nexthop_reset
                                (unit, nh_ecmp_index, vp_type, vp, vpn));
              }
         }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_ecmp_member_egress_get_first
 * Purpose:
 *      Modify vxlan ecmp member object's Egress next hop entry
 * Parameters:
 *      unit - Device Number
 *      ecmp_index - ECMP index
 *      index - poiter to first next hop index
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_ecmp_member_egress_get_first(int unit, int ecmp_index, int* index)
{
    int i = 0, ecmp_member_count = 0;
    int alloc_size;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_if_t *ecmp_member_array = NULL;
    int rv = BCM_E_NONE;

    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_index)) {
        return BCM_E_PARAM;
    }
    ecmp_info.ecmp_intf = ecmp_index;
    alloc_size = sizeof(bcm_if_t)*BCM_XGS3_L3_ECMP_MAX(unit);
    ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
    if (NULL == ecmp_member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(ecmp_member_array, 0, alloc_size);

    rv = bcm_esw_l3_egress_ecmp_get(unit, &ecmp_info, BCM_XGS3_L3_ECMP_MAX(unit),
                                  ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                ecmp_member_array[i])) {
        *index = (ecmp_member_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit));
    } else {
        rv = BCM_E_PARAM;
        goto clean_up;
    }
clean_up:
    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_port_nh_get
 * Purpose:
 *      Get VXLAN Next Hop entry
 * Parameters:
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_td2_vxlan_port_nh_get(int unit, bcm_vpn_t vpn,  int vp, bcm_vxlan_port_t *vxlan_port)
{
    egr_l3_next_hop_entry_t egr_nh;
    ing_dvp_table_entry_t dvp;
    int nh_ecmp_index=0;
    uint32 ecmp=0;
    int rv = BCM_E_NONE;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf) == 
                              _BCM_VXLAN_INGRESS_DEST_VP_TYPE) {
        /* Egress into VXLAN tunnel, find the tunnel_if */
        vxlan_port->flags |= BCM_VXLAN_PORT_EGRESS_TUNNEL;
        BCM_IF_ERROR_RETURN(
             READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
        ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
        if (ecmp) {
            nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
            vxlan_port->egress_if  =  nh_ecmp_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
            /*get ecmp's first member nexthop index*/
            BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_ecmp_member_egress_get_first(unit, vxlan_port->egress_if, 
                &nh_ecmp_index));
        } else {
            nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
            /* Extract next hop index from unipath egress object. */
            vxlan_port->egress_if  =  nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
        }

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
            MEM_BLOCK_ANY, nh_ecmp_index, &egr_nh));

    } else if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                                VP_TYPEf) == _BCM_VXLAN_DEST_VP_TYPE_ACCESS) {
        /* Egress into Access-side, find the tunnel_if */
        BCM_IF_ERROR_RETURN(
            READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
        /* Extract next hop index from unipath egress object. */
        vxlan_port->egress_if  =  nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit);

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_ecmp_index, &egr_nh));
         if (BCM_SUCCESS(rv)) {
              (void) _bcm_td2_vxlan_sd_tag_get( unit, NULL, vxlan_port, &egr_nh,
                         NULL, 0);
         }
    } else {
        return BCM_E_NOT_FOUND;
    }

    rv = bcm_td2_vxlan_nexthop_get(unit, &egr_nh, vxlan_port);

    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_vnid_entry_reset
 * Purpose:
 *      Reset VXLAN Match VNID Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  Vnid
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_match_vnid_entry_reset(int unit, uint32 vnid,
                                                bcm_vlan_t ovid)
{
    soc_mem_t mmem = MPLS_ENTRYm;
    uint32 ment[SOC_MAX_MEM_WORDS];
    int d = 0; /*vxlan decoupled mode*/

    sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        mmem = VLAN_XLATE_1_DOUBLEm;
        d =1;
        soc_mem_field32_set(unit, mmem, &ment, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mmem, &ment, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mmem, &ment,
            DATA_TYPEf, _BCM_VXLAN_DATA_TYPE_VNID_VFI_FLEX);        
        soc_mem_field32_set(unit, mmem, &ment, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_VNID_VFI_FLEX);
    } else
#endif
    {
        soc_mem_field32_set(unit, mmem, &ment, VALIDf, 1);        
        soc_mem_field32_set(unit, mmem, &ment, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_VNID_VFI);
    }

    soc_mem_field32_set(unit, mmem, &ment, (d ? VXLAN_FLEX__VNIDf : VXLAN_VN_ID__VN_IDf), vnid);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
        soc_mem_field32_set(unit, mmem, &ment, VXLAN_VN_ID__OVIDf, ovid);
    }
#endif
    
    BCM_IF_ERROR_RETURN(soc_mem_delete(unit, mmem, MEM_BLOCK_ALL, &ment));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_tunnel_entry_update
 * Purpose:
 *      Update Match VXLAN Tunnel Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  vxlan_entry
 *  OUT :  vxlan_entry

 */

STATIC int
_bcm_td2_vxlan_match_tunnel_entry_update(int unit,
                                             uint32 *ment, 
                                             uint32 *new_ment,
                                             uint32 *return_ment)
{
    uint32 value=0, key_type=0;
    soc_mem_t mem = MPLS_ENTRYm;

    /* Check if Key_Type identical */
    key_type = soc_mem_field32_get(unit, mem, ment, KEY_TYPEf);
    value = soc_mem_field32_get(unit, mem, return_ment, KEY_TYPEf);
    if (key_type != value) {
         return BCM_E_PARAM;
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, new_ment, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, new_ment, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mem, new_ment,
            DATA_TYPEf, key_type);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, new_ment, VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, new_ment, KEY_TYPEf, key_type);
    if (key_type == _BCM_VXLAN_KEY_TYPE_TUNNEL_VNID_VFI) {
        value = soc_mem_field32_get(unit,mem, ment, VXLAN_VN_ID__SIPf);
        soc_mem_field32_set(unit, mem, new_ment, VXLAN_VN_ID__SIPf, value);
        value = soc_mem_field32_get(unit,mem, ment, VXLAN_VN_ID__VFIf);
        soc_mem_field32_set(unit, mem, new_ment, VXLAN_VN_ID__VFIf, value);
        value = soc_mem_field32_get(unit,mem, ment, VXLAN_VN_ID__VN_IDf);
        soc_mem_field32_set(unit, mem, new_ment, VXLAN_VN_ID__VN_IDf, value);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
            value = soc_mem_field32_get(unit,mem, ment, VXLAN_VN_ID__OVIDf);
            soc_mem_field32_set(unit, mem, new_ment, VXLAN_VN_ID__OVIDf, value);
        }
#endif        
    } else {
        value = soc_mem_field32_get(unit,mem, ment, VXLAN_SIP__SIPf);
        soc_mem_field32_set(unit, mem, new_ment, VXLAN_SIP__SIPf, value);
        value = soc_mem_field32_get(unit,mem, ment, VXLAN_SIP__SVPf);
        soc_mem_field32_set(unit, mem, new_ment, VXLAN_SIP__SVPf, value);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
            value = soc_mem_field32_get(unit,mem, ment, VXLAN_SIP__OVIDf);
            soc_mem_field32_set(unit, mem, new_ment, VXLAN_SIP__OVIDf, value);
        }
#endif        
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2_vxlan_match_tunnel_entry_set
 * Purpose:
 *      Set VXLAN Match Tunnel Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  mpls_entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_match_tunnel_entry_set(int unit, uint32 *ment, uint32 *return_ment)
{
    int rv = BCM_E_UNAVAIL;
    int index = 0;
    int svp = 0;
    uint32 key_type = 0;
    uint32 tmp_ment[SOC_MAX_MEM_WORDS]={0};

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        ment, return_ment, 0);

    if (rv == SOC_E_NONE) {
        BCM_IF_ERROR_RETURN(
            _bcm_td2_vxlan_match_tunnel_entry_update(unit, ment, tmp_ment, return_ment));

        key_type = soc_mem_field32_get(unit, MPLS_ENTRYm, return_ment, KEY_TYPEf);
        if (key_type == _BCM_VXLAN_KEY_TYPE_TUNNEL) {
            svp = soc_mem_field32_get(unit, MPLS_ENTRYm, return_ment, VXLAN_SIP__SVPf);
            if (_bcm_vp_used_get(unit, svp, _bcmVpTypeVpLag)) {
                soc_mem_field32_set(unit, MPLS_ENTRYm, tmp_ment, VXLAN_SIP__SVPf, svp);
            }
        }
        rv = soc_mem_write(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, index, tmp_ment);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
        rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, ment);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_tunnel_entry_delete
 * Purpose:
 *      Delete VXLAN Match Tunnel Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  mpls_entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_match_tunnel_entry_delete(int unit, uint32 *ment)
{
    int rv = BCM_E_UNAVAIL;
    uint32 value=0, key_type=0;
    soc_mem_t mem = MPLS_ENTRYm;
    uint32 tmp_ment[SOC_MAX_MEM_WORDS]={0};

    /* Check if Key_Type identical */
    key_type = soc_mem_field32_get(unit, mem, &ment, KEY_TYPEf);
    soc_mem_field32_set(unit, mem, &tmp_ment, KEY_TYPEf, key_type);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, &tmp_ment, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, &tmp_ment, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, mem, &tmp_ment,
            DATA_TYPEf, key_type);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, &tmp_ment, VALIDf, 1);
    }
    if (key_type == _BCM_VXLAN_KEY_TYPE_TUNNEL_VNID_VFI) {
        value = soc_mem_field32_get(unit, mem, &ment, VXLAN_VN_ID__SIPf);
        soc_mem_field32_set(unit, mem, &tmp_ment, VXLAN_VN_ID__SIPf, value);
        value = soc_mem_field32_get(unit, mem, &ment, VXLAN_VN_ID__VN_IDf);
        soc_mem_field32_set(unit, mem, &tmp_ment, VXLAN_VN_ID__VN_IDf, value);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
            value = soc_mem_field32_get(unit, mem, &ment, VXLAN_VN_ID__OVIDf);
            soc_mem_field32_set(unit, mem, &tmp_ment, VXLAN_VN_ID__OVIDf, value);
        }
#endif
    } else {
        value = soc_mem_field32_get(unit, mem, &ment, VXLAN_VN_ID__SIPf);
        soc_mem_field32_set(unit, mem, &tmp_ment, VXLAN_VN_ID__SIPf, value);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
            value = soc_mem_field32_get(unit, mem, &ment, VXLAN_SIP__OVIDf);
            soc_mem_field32_set(unit, mem, &tmp_ment, VXLAN_SIP__OVIDf, value);
        }
#endif
    }

    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, tmp_ment);
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_vp_find
 * Purpose:
 *      Find network VP has the same match
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      vp1 - (IN) Network VP.
 *      vp2 - (OUT) Network VP.
 * Returns:
 *      TRUE - Found
 *      FALSE - Not Found
 */
int
_bcm_td2_vxlan_match_vp_find(int unit, int vp1, int *vp2)
{
    int vp = 0;
    int num_vp = 0;
    uint32 tunnel_index1 = 0;
    uint32 tunnel_index2 = 0;
    uint32 tunnel_sip1 = 0;
    uint32 tunnel_sip2 = 0;
    _bcm_vxlan_match_port_info_t *mkey1 = NULL;
    _bcm_vxlan_match_port_info_t *mkey2 = NULL;
    bcm_vpn_t vpn = -1;
    bcm_vxlan_port_t vxlan_port;
    int rv = BCM_E_NONE;

    mkey1 = &(VXLAN_INFO(unit)->match_key[vp1]);
    tunnel_index1 = mkey1->match_tunnel_index;
    tunnel_sip1 = VXLAN_INFO(unit)->vxlan_tunnel_term[tunnel_index1].sip;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (vp = 0; vp < num_vp; vp++) {
        if (vp == vp1) {
            continue;
        }

        mkey2 = &(VXLAN_INFO(unit)->match_key[vp]);
        if (mkey2->flags == _BCM_VXLAN_PORT_MATCH_TYPE_VNID) {
            tunnel_index2 = mkey2->match_tunnel_index;
            tunnel_sip2 = VXLAN_INFO(unit)->vxlan_tunnel_term[tunnel_index2].sip;

            if (tunnel_sip1 == tunnel_sip2) {
                bcm_vxlan_port_t_init(&vxlan_port);
                rv = _bcm_td2_vxlan_port_get(unit, vpn, vp, &vxlan_port);
                /* Skip since match was only recorded in S/W and was not
                 * written into H/W for multicast port.
                 */
                if (BCM_SUCCESS(rv) &&
                    (vxlan_port.flags & BCM_VXLAN_PORT_MULTICAST)) {
                    continue;
                }
                *vp2 = vp;
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_tunnel_entry_reset
 * Purpose:
 *      Reset VXLAN Match Tunnel Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  mpls_entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_match_tunnel_entry_reset(int unit, int vp, uint32 *ment, uint32 *return_ment)
{
    int rv = BCM_E_UNAVAIL;
    int index = 0;
    int svp = 0;
    int d = 0;
    soc_mem_t mem = MPLS_ENTRYm;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        mem = MPLS_ENTRY_SINGLEm;
        d = 1;
    }
#endif
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                        ment, return_ment, 0);
    if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE)) {
         return rv;
    }

    if (rv == SOC_E_NONE) {
        svp = soc_mem_field32_get(unit, mem, return_ment,
                BCM_VXLAN_SIP_FIELD(d, SVPf));
        if (svp != vp) {
            return BCM_E_NONE;
        }

        if (_bcm_td2_vxlan_match_vp_find(unit, vp, &svp)) {
            soc_mem_field32_set(unit, mem, return_ment,
                BCM_VXLAN_SIP_FIELD(d, SVPf), svp);
        } else {
            sal_memset(return_ment, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
        }
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, return_ment);
    }

    if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE)) {
       return rv;
    } else {
       return BCM_E_NONE;
    }
}

/*
 * Function:
 *        _bcm_td2_vxlan_match_vxlate_extd_entry_set
 * Purpose:
 *       Set VXLAN Match Vxlate Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  vxlan_port
 *  IN :  vlan_xlate_extd_entry_t
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_match_egr_vxlate_entry_set(int unit, bcm_vpn_t vpn, 
                                                         bcm_vxlan_port_t *vxlan_port, 
                                                         uint32 *vent, 
                                                         uint32 *return_vent)
{
    int rv = BCM_E_NONE;
    int index = -1;
    soc_mem_t vxlate_mem;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
    }

    rv = soc_mem_search(unit, vxlate_mem,
                        MEM_BLOCK_ANY, &index, vent, return_vent, 0);
    if (BCM_E_NONE == rv) {
        BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                          vxlate_mem, MEM_BLOCK_ALL, index,
                                          vent));
    } else if (BCM_E_NOT_FOUND == rv) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(soc_mem_insert(unit,
                                          vxlate_mem, MEM_BLOCK_ALL, vent));
    } else {
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_egr_vxlate_entry_delete
 * Purpose:
 *      Delete VXLAN Match Tunnel Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  egr_vlan_xlate_entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_match_egr_vxlate_entry_delete(int unit, uint32 *ment)
{
    int rv = BCM_E_UNAVAIL;
    uint32 value = 0, entry_type = 0;
    soc_mem_t vxlate_mem; 
    uint32 tmp_ment[SOC_MAX_MEM_WORDS] = {0};
    
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        vxlate_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
    }
    /* Check if Key_Type identical */
    entry_type = soc_mem_field32_get(unit, vxlate_mem, ment, ENTRY_TYPEf);
    soc_mem_field32_set(unit, vxlate_mem, tmp_ment, ENTRY_TYPEf, entry_type);

    soc_mem_field32_set(unit, vxlate_mem, tmp_ment, VALIDf, 1);
    if (entry_type == _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP) {
        value = soc_mem_field32_get(unit, vxlate_mem, ment, VXLAN_VFI__VFIf);
        soc_mem_field32_set(unit, vxlate_mem, tmp_ment, VXLAN_VFI__VFIf, value);
        value = soc_mem_field32_get(unit, vxlate_mem, ment, VXLAN_VFI__DVPf);
        soc_mem_field32_set(unit, vxlate_mem, tmp_ment, VXLAN_VFI__DVPf, value);
    
    } else {
        value = soc_mem_field32_get(unit, vxlate_mem, ment, VXLAN_VFI__VFIf);
        soc_mem_field32_set(unit, vxlate_mem, tmp_ment, VXLAN_VFI__VFIf, value);       
    }

    rv = soc_mem_delete(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, tmp_ment);
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_vxlate_entry_update
 * Purpose:
 *      Update VXLAN Match Vlan_xlate  Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  vxlan_port
 *  IN :  vlan_xlate_entry_t
 *  OUT :  vlan_xlate_entry_t
 */
STATIC int
_bcm_td2_vxlan_match_vxlate_entry_update(int unit, bcm_vpn_t vpn,
                                         bcm_vxlan_port_t *vxlan_port,
                                         uint32 *vent, uint32 *return_ent)
{
    uint32  key_type=0, value=0;
    soc_mem_t mem;
    int vp = _SHR_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        mem = VLAN_XLATEm;
    }

    if ((vent == NULL) || (return_ent == NULL)) {
        return BCM_E_PARAM;
    }

    /* Check if Key_Type identical */
    key_type = soc_mem_field32_get(unit, mem, vent, KEY_TYPEf);
    value = soc_mem_field32_get(unit, mem, return_ent, KEY_TYPEf);
    if (key_type != value) {
         return BCM_E_PARAM;
    }

    soc_mem_field32_set(unit, mem, return_ent, SOURCE_TYPEf, 1);
    value = soc_mem_field32_get(unit, mem, return_ent, XLATE__MPLS_ACTIONf);
    /* SVP_VALID and L3_IIF_VALID overlay high and low bits of MPLS_ACTION. */
    if (SOC_MEM_FIELD_VALID(unit, mem, L3_IIF_VALIDf)) {
        value |= 0x1;
    } else {
        value = 0x1;
    }
    /* Retain original Keys -- Update data only */
    soc_mem_field32_set(unit, mem, return_ent, XLATE__MPLS_ACTIONf, value);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS) {
            return BCM_E_UNAVAIL;
        }
        soc_mem_field32_set(unit, mem, return_ent, XLATE__DISABLE_VLAN_CHECKSf,
                            0);
        soc_mem_field32_set(unit, mem, return_ent, XLATE__VLAN_ACTION_VALIDf,
                            1);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, return_ent, XLATE__DISABLE_VLAN_CHECKSf,
                            1);
        if (!(vxlan_port->flags & BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS)) {
            soc_mem_field32_set(unit, mem, return_ent,
                                XLATE__VLAN_ACTION_VALIDf, 1);
        } else {
            soc_mem_field32_set(unit, mem, return_ent,
                                XLATE__VLAN_ACTION_VALIDf, 0);
        }
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vp_sharing)) {
        if (vpn != BCM_VXLAN_VPN_INVALID) {
            int vfi = 0;
            _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
            soc_mem_field32_set(unit, mem, return_ent, XLATE__VFIf, vfi);
        }
    }
#endif
    soc_mem_field32_set(unit, mem, return_ent, XLATE__SOURCE_VPf, vp);

    return BCM_E_NONE;
}

/*
 * Function:
 *        _bcm_td2_vxlan_match_vxlate_extd_entry_set
 * Purpose:
 *       Set VXLAN Match Vxlate Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  vxlan_port
 *  IN :  vlan_xlate_extd_entry_t
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_match_vxlate_entry_set(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t *vxlan_port, uint32 *vent)
{
    int rv = BCM_E_NONE;
    int index = -1;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    bcm_port_t gport_id = -1;
    soc_mem_t mem;
    uint32 return_vent[SOC_MAX_MEM_WORDS];
    int vp = _SHR_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else
#endif
    {
        mem = VLAN_XLATEm;
        soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, vxlan_port->match_port,
                                    &mod_out, &port_out, &trunk_id, &gport_id));
    if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
        soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
        soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
    } else {
        soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_out);
    }

    soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    soc_mem_field32_set(unit, mem, vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
    soc_mem_field32_set(unit, mem, vent, XLATE__SOURCE_VPf, vp);

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS){
            return BCM_E_UNAVAIL;
        }
        soc_mem_field32_set(unit, mem, vent, XLATE__DISABLE_VLAN_CHECKSf, 0);
        soc_mem_field32_set(unit, mem, vent, XLATE__VLAN_ACTION_VALIDf, 1);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
        if (!(vxlan_port->flags & BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS)){
            soc_mem_field32_set(unit, mem, vent, XLATE__VLAN_ACTION_VALIDf, 1);
        }
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vp_sharing)) {
        if (vpn != BCM_VXLAN_VPN_INVALID) {
            int vfi = 0;
            _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
            soc_mem_field32_set(unit, mem, vent, XLATE__VFIf, vfi);
        }
    }
#endif

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vent, return_vent, 0);
    if (BCM_E_NONE == rv) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_vxlan_match_vxlate_entry_update(unit, vpn,
                vxlan_port, vent, return_vent));
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, return_vent));
        } else {
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, vent));
        }
    } else if (BCM_E_NOT_FOUND == rv) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vent));
    } else {
        return rv;
    }

    if (vxlan_info->match_key[vp].flags != _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
        if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
            vxlan_info->match_key[vp].trunk_id = trunk_id;
            vxlan_info->match_key[vp].modid = -1;
        } else {
            vxlan_info->match_key[vp].port = port_out;
            vxlan_info->match_key[vp].modid = mod_out;
            vxlan_info->match_key[vp].trunk_id = -1;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_vxlan_match_clear
 * Purpose:
 *      Clear VXLAN Match Software State
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      void
 */

void
bcm_td2_vxlan_match_clear (int unit, int vp)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);

    vxlan_info->match_key[vp].flags = 0;
    vxlan_info->match_key[vp].match_vlan = 0;
    vxlan_info->match_key[vp].match_inner_vlan = 0;
    vxlan_info->match_key[vp].trunk_id = -1;
    vxlan_info->match_key[vp].port = 0;
    vxlan_info->match_key[vp].modid = -1;
    vxlan_info->match_key[vp].match_tunnel_index = 0;
    vxlan_info->match_key[vp].flags2 = 0;
    return;
}

/*
 * Function:
 *      _bcm_td2_source_trunk_svp_get
 * Purpose:
 *      get source trunk port svp
 * Parameters:
 *      unit    - (IN) Device Number
 *      modid   - (IN) module ID
 *      port   - (IN) Trunk member port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_source_trunk_svp_get(int unit, bcm_module_t modid, bcm_port_t port)
{
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/
    int     src_trk_idx = -1;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, modid,
                port, &src_trk_idx));

    /* Read source trunk map table. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                        src_trk_idx, &trunk_map_entry));

    /* get SVP */
    return soc_mem_field32_get(unit, mem, &trunk_map_entry, SOURCE_VPf);
}

/*
 * Function:
 *      _bcm_td2_vxlan_trunk_table_set
 * Purpose:
 *      Configure VXLAN Trunk port entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      port   - (IN) Trunk member port
 *      tgid - (IN) Trunk group Id
 *      vp   - (IN) Virtual port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_trunk_table_set(int unit, bcm_port_t port, bcm_trunk_t tgid, int vp)
{
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/
    bcm_module_t my_modid;
    int     src_trk_idx = -1;
    int     port_type;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;
    int     rv = BCM_E_NONE;

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

    soc_mem_lock(unit, mem);

    /* Read source trunk map table. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                        src_trk_idx, &trunk_map_entry));

    /* Set trunk group id. */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_mem_field_valid(unit, mem, SRC_TGIDf)) {
        soc_mem_field32_set(unit, mem, &trunk_map_entry, SRC_TGIDf, tgid);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, &trunk_map_entry, TGIDf, tgid);
    }
    /* Set port is part of Trunk group */
    soc_mem_field32_set(unit, mem, &trunk_map_entry, PORT_TYPEf, port_type);

    /* Enable SVP Mode */
    if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
        soc_mem_field32_set(unit, mem, &trunk_map_entry, SVP_VALIDf, 0x1);
    }

    /* Set SVP */
    soc_mem_field32_set(unit, mem, &trunk_map_entry, SOURCE_VPf, vp);

    /* Write entry to hw. */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, src_trk_idx, &trunk_map_entry);

    soc_mem_unlock(unit, mem);

    return rv; 
}


/*
 * Function:
 *      _bcm_td2_vxlan_trunk_table_reset
 * Purpose:
 *      Reset VXLAN Trunk port entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      port   - (IN) Trunk member port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_trunk_table_reset(int unit, bcm_port_t port)
{
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/
    bcm_module_t my_modid;
    int    src_trk_idx = -1;
    int rv = BCM_E_NONE;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                        port, &src_trk_idx));

    soc_mem_lock(unit, mem);

    /* Read source trunk map table. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     src_trk_idx, &trunk_map_entry));

    /* Enable SVP Mode */
    if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
        soc_mem_field32_set(unit, mem, &trunk_map_entry, SVP_VALIDf, 0);
    }

    /* Set SVP */
    soc_mem_field32_set(unit, mem, &trunk_map_entry, SOURCE_VPf, 0);

    /* Write entry to hw. */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, src_trk_idx, &trunk_map_entry);

    soc_mem_unlock(unit, mem);

    return rv; 
}

/*
 * Function:
 *      bcm_td2_vxlan_match_trunk_add
 * Purpose:
 *      Assign SVP of  VXLAN Trunk port
 * Parameters:
 *      unit    - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      vp  - (IN) Source Virtual Port
 *      is_replace - (IN) flag indicates if replace
 *      untag_op - (IN)  untagged VLAN operation
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
 bcm_td2_vxlan_match_trunk_add(int unit, bcm_trunk_t tgid, int vp,
                               int is_replace, int untag_op)
{
    int port_idx=0;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int rv = BCM_E_NONE;
    int temp = 0;
    int my_modid = 0;

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                SOC_MAX_NUM_PORTS, local_port_out, &local_member_count));

    if (local_member_count == 0) {
        return BCM_E_NONE;
    }

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    temp = _bcm_td2_source_trunk_svp_get(unit, my_modid, local_port_out[0]);
    if (temp != 0) {
        if ((temp == vp) && (is_replace)) {
            /*continue*/
        } else {
            return BCM_E_INTERNAL;
        }
    }

    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        rv = _bcm_td2_vxlan_trunk_table_set(unit, local_port_out[port_idx], tgid, vp);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
        rv = _bcm_esw_port_tab_set(
                 unit, local_port_out[port_idx],
                 _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0x1); /* L2_SVP */
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }

        if (untag_op) {
            rv = _bcm_td2_vxlan_port_untagged_profile_set(unit, local_port_out[port_idx]);
            if (BCM_FAILURE(rv)) {
                goto trunk_cleanup;
            }
        }
    }

    return BCM_E_NONE;

 trunk_cleanup:
    for (; port_idx >= 0; port_idx--) {
        (void) _bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                     _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0);
        (void) _bcm_td2_vxlan_trunk_table_reset(unit, local_port_out[port_idx]);
    }
    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_match_trunk_delete
 * Purpose:
 *      Remove SVP of VXLAN Trunk port
 * Parameters:
 *      unit    - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      vp  - (IN) Source Virtual Port
 *      untag_op  - (IN) untagged VLAN opeartion
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
bcm_td2_vxlan_match_trunk_delete(int unit, bcm_trunk_t tgid, int vp, int untag_op)
{
    int port_idx=0;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                SOC_MAX_NUM_PORTS, local_port_out, &local_member_count));

    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        rv = _bcm_td2_vxlan_trunk_table_reset(unit, local_port_out[port_idx]);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
        rv = _bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                   _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }

        if (untag_op) {
            rv = _bcm_td2_vxlan_port_untagged_profile_reset(unit,
                     local_port_out[port_idx]);
            if (BCM_FAILURE(rv)) {
                goto trunk_cleanup;
            }
        }
    }

    return BCM_E_NONE;

 trunk_cleanup:
    for (;port_idx >= 0; port_idx--) {
        (void)_bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                    _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0x1);
        (void) _bcm_td2_vxlan_trunk_table_set(unit, local_port_out[port_idx], tgid, vp);
    }
    return rv;
}

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *        _bcm_td3_vxlan_match_vxlate_entry_set
 * Purpose:
 *       Set VXLAN Match Vxlate Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  vxlan_port
 *  IN :  vlan_xlate_extd_entry_t
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_vxlan_match_vxlate_entry_set(int unit, bcm_vpn_t vpn, 
                                                         bcm_vxlan_port_t *vxlan_port, 
                                                         uint32 *vent, 
                                                         uint32 *return_vent)
{
    int rv = BCM_E_NONE;
    int index = -1;
    soc_mem_t vxlate_mem;

    vxlate_mem = VLAN_XLATE_1_DOUBLEm;

    rv = soc_mem_search(unit, vxlate_mem,
                        MEM_BLOCK_ANY, &index, vent, return_vent, 0);
    if (BCM_E_NONE == rv) {
        BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                          vxlate_mem, MEM_BLOCK_ALL, index,
                                          vent));
    } else if (BCM_E_NOT_FOUND == rv) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(soc_mem_insert(unit,
                                          vxlate_mem, MEM_BLOCK_ALL, vent));
    } else {
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *        _bcm_td3_vxlan_match_vxlate_entry_set
 * Purpose:
 *       Set VXLAN Match Vxlate Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  vxlan_port
 *  IN :  vlan_xlate_extd_entry_t
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_vxlan_match_tunnel_entry_set(int unit, uint32 *ment, uint32 *return_ment)
{
    int rv = BCM_E_NONE;
    int index = -1;
    int svp = 0;
    uint32 key_type = 0;
    soc_mem_t vxlate_mem;

    vxlate_mem = MPLS_ENTRY_SINGLEm;

    rv = soc_mem_search(unit, vxlate_mem,
                        MEM_BLOCK_ANY, &index, ment, return_ment, 0);
    if (BCM_E_NONE == rv) {
        key_type = soc_mem_field32_get(unit, vxlate_mem, return_ment, KEY_TYPEf);
        if (key_type == _BCM_VXLAN_KEY_TYPE_TUNNEL) {
            svp = soc_mem_field32_get(
                unit, vxlate_mem, return_ment, VXLAN_FLEX_IPV4_SIP__SVPf);
            if (_bcm_vp_used_get(unit, svp, _bcmVpTypeVpLag)) {
                soc_mem_field32_set(
                    unit, vxlate_mem, ment, VXLAN_FLEX_IPV4_SIP__SVPf, svp);
            }
        }
        BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                          vxlate_mem, MEM_BLOCK_ALL, index,
                                          ment));
    } else if (BCM_E_NOT_FOUND == rv) {
        BCM_IF_ERROR_RETURN(soc_mem_insert(unit,
                                          vxlate_mem, MEM_BLOCK_ALL, ment));
    } else {
        return rv;
    }
    return BCM_E_NONE;
}

#endif

/*
 * Function:
 *      _bcm_td2_vxlan_match_add
 * Purpose:
 *      Assign match criteria of an VXLAN port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vxlan_port - (IN) vxlan port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_match_add(int unit, bcm_vxlan_port_t *vxlan_port, int vp, bcm_vpn_t vpn)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
    int rv = BCM_E_NONE;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS];
    int    src_trk_idx=0;  /*Source Trunk table index.*/
    int    gport_id=-1;
    uint8  vxlan_vpn_type = 0;

    /* reset SOURCE_TRUNK_MAP_TABLE and PORT_TAB tables before PORT replace */
    if ((vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) &&
        (vxlan_info->match_key[vp].flags == _BCM_VXLAN_PORT_MATCH_TYPE_PORT ||
         vxlan_info->match_key[vp].flags == _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK)) {
        bcm_vxlan_port_t curr_vxlan_port;
        bcm_vxlan_port_t_init(&curr_vxlan_port);
        curr_vxlan_port.vxlan_port_id = vxlan_port->vxlan_port_id;
        curr_vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_PORT;

        if (vxlan_info->match_key[vp].trunk_id != -1) {
            bcm_trunk_t trunk_id = vxlan_info->match_key[vp].trunk_id;
            BCM_GPORT_TRUNK_SET(curr_vxlan_port.match_port, trunk_id);
        }
        else {
            bcm_module_t mod_in=0, mod_out=0;
            bcm_port_t port_in=0, port_out=0;
            int src_trk_idx = vxlan_info->match_key[vp].index;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_src_modid_port_get(unit, src_trk_idx, &mod_in, &port_in));
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
            curr_vxlan_port.match_port =
                _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
        }
        curr_vxlan_port.flags2 = (vxlan_info->match_key[vp].flags2 &
             _BCM_VXLAN_PORT_MATCH_NO_UNTAG_OP) ?
                  BCM_VXLAN_PORT_FLAGS2_UT_VLAN_ACTION_NO_CLEAR : 0;

        BCM_IF_ERROR_RETURN(
            _bcm_td2_vxlan_match_delete(unit, vp, curr_vxlan_port, 0));
    }

    if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_NONE) {
        vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_NONE;
        return BCM_E_NONE;
    }

    if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_SHARE) {
        /* there is no criteria to be configurated for shared vp,
           only updating software state to indicate this is a shared vp */
        vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_SHARED;
        return BCM_E_NONE;
    }

    if ((vxlan_port->flags2 & BCM_VXLAN_PORT_FLAGS2_UT_VLAN_ACTION_NO_CLEAR) &&
        (vxlan_port->criteria != BCM_VXLAN_PORT_MATCH_PORT)) {
        return BCM_E_PARAM;
    }

    sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        mem = VLAN_XLATEm;
    }

    if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_PORT_VLAN ) {
        if (!BCM_VLAN_VALID(vxlan_port->match_vlan)) {
             return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf, vxlan_port->match_vlan);

        rv = _bcm_td2_vxlan_match_vxlate_entry_set(unit, vpn, vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);

        if (vxlan_info->match_key[vp].flags != _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_VLAN;
            vxlan_info->match_key[vp].match_vlan = vxlan_port->match_vlan;
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            bcm_td2_vxlan_port_match_count_adjust(unit, vp, 1);
        }
    } else if (vxlan_port->criteria ==
                            BCM_VXLAN_PORT_MATCH_PORT_INNER_VLAN) {
        if (!BCM_VLAN_VALID(vxlan_port->match_inner_vlan)) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf, vxlan_port->match_inner_vlan);

        rv = _bcm_td2_vxlan_match_vxlate_entry_set(unit, vpn, vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);

        if (vxlan_info->match_key[vp].flags != _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN;
            vxlan_info->match_key[vp].match_inner_vlan = vxlan_port->match_inner_vlan;
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            bcm_td2_vxlan_port_match_count_adjust(unit, vp, 1);
        }
    } else if (vxlan_port->criteria == 
                            BCM_VXLAN_PORT_MATCH_PORT_VLAN_STACKED) {
        if (!BCM_VLAN_VALID(vxlan_port->match_vlan) ||
                !BCM_VLAN_VALID(vxlan_port->match_inner_vlan)) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf, vxlan_port->match_vlan);
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf, vxlan_port->match_inner_vlan);

        rv = _bcm_td2_vxlan_match_vxlate_entry_set(unit, vpn, vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);

        if (vxlan_info->match_key[vp].flags != _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED;
            vxlan_info->match_key[vp].match_vlan = vxlan_port->match_vlan;
            vxlan_info->match_key[vp].match_inner_vlan = vxlan_port->match_inner_vlan;
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            bcm_td2_vxlan_port_match_count_adjust(unit, vp, 1);
        }
    } else if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_VLAN_PRI) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
        }
#endif
        /* match_vlan : Bits 12-15 contains VLAN_PRI + CFI, vlan=BCM_E_NONE */
        soc_mem_field32_set(unit, mem, vent, OTAGf, vxlan_port->match_vlan);

        rv = _bcm_td2_vxlan_match_vxlate_entry_set(unit, vpn, vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);

        if (vxlan_info->match_key[vp].flags != _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI;
            vxlan_info->match_key[vp].match_vlan = vxlan_port->match_vlan;
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            bcm_td2_vxlan_port_match_count_adjust(unit, vp, 1);
        }
    } else if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_PORT) {
        if (vxlan_info->match_key[vp].flags ==
                     _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, vxlan_port->match_port,
                                        &mod_out, &port_out, &trunk_id, &gport_id));

        if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
            BCM_IF_ERROR_RETURN(bcm_td2_vxlan_match_trunk_add(unit, trunk_id,
                                vp, vxlan_port->flags & BCM_VXLAN_PORT_REPLACE,
                !(vxlan_port->flags2 & BCM_VXLAN_PORT_FLAGS2_UT_VLAN_ACTION_NO_CLEAR)));

            if (vxlan_info->match_key[vp].flags !=
                     _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
                vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK;
                vxlan_info->match_key[vp].trunk_id = trunk_id;
                vxlan_info->match_key[vp].flags2 = (vxlan_port->flags2 &
                    BCM_VXLAN_PORT_FLAGS2_UT_VLAN_ACTION_NO_CLEAR) ?
                        _BCM_VXLAN_PORT_MATCH_NO_UNTAG_OP : 0;

            }
        } else {
            int is_local;
            int temp = 0;

            temp = _bcm_td2_source_trunk_svp_get(unit, mod_out, port_out);
            if (temp != 0) {
                if ((temp == vp) && (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
                    /*continue*/
                } else {
                    return BCM_E_INTERNAL;
                }
            }

            BCM_IF_ERROR_RETURN
                ( _bcm_esw_modid_is_local(unit, mod_out, &is_local));

            /* Get index to source trunk map table */
            BCM_IF_ERROR_RETURN(
                   _bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                     port_out, &src_trk_idx));

            /* Enable SVP Mode */
            if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, SVP_VALIDf) ) {
                rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        src_trk_idx, SVP_VALIDf, 0x1);
                BCM_IF_ERROR_RETURN(rv);
            }
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                        src_trk_idx, SOURCE_VPf, vp);
            BCM_IF_ERROR_RETURN(rv);

            if (is_local) {
                rv = _bcm_esw_port_tab_set(
                         unit, vxlan_port->match_port, _BCM_CPU_TABS_NONE,
                         PORT_OPERATIONf, 0x1); /* L2_SVP */
                BCM_IF_ERROR_RETURN(rv);

                if (!(vxlan_port->flags2 &
                          BCM_VXLAN_PORT_FLAGS2_UT_VLAN_ACTION_NO_CLEAR)) {
                    /* Set TAG_ACTION_PROFILE_PTR */
                    rv = _bcm_td2_vxlan_port_untagged_profile_set(
                             unit, vxlan_port->match_port);
                    BCM_IF_ERROR_RETURN(rv);
                } else {
                    vxlan_info->match_key[vp].flags2 =
                        _BCM_VXLAN_PORT_MATCH_NO_UNTAG_OP;
                }
            }
            /*
             * The vp has been created based on trunk.
             * When a new port is added into the trunk, a match will be
             * added for the vp through bcm_port_match_add.
             */
            if ((vxlan_info->match_key[vp].flags != _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK) &&
                (vxlan_info->match_key[vp].flags != _BCM_VXLAN_PORT_MATCH_TYPE_SHARED)) {
                vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_PORT;
                vxlan_info->match_key[vp].index = src_trk_idx;
            }
        }
    } else if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_VN_ID) {
        uint32 ment_svp[SOC_MAX_MEM_WORDS] = {0};
        uint32 ment_vnid[SOC_MAX_MEM_WORDS] = {0};
        uint32 ment_evt[SOC_MAX_MEM_WORDS] = {0};
        uint32 orignal_ment_svp[SOC_MAX_MEM_WORDS] = {0};
        uint32 orignal_ment_vnid[SOC_MAX_MEM_WORDS] = {0};
        uint32 orignal_ment_egr_vt[SOC_MAX_MEM_WORDS] = {0};
        int tunnel_idx=-1;
        uint32 tunnel_sip;
        soc_mem_t vxlate_mem;
        int valid = 0;
        int d = 0; /*vxlan decoupled mode*/

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
            d = 1;
        } else
#endif
        {
            vxlate_mem = EGR_VLAN_XLATEm;
        }

        if (!BCM_GPORT_IS_TUNNEL(vxlan_port->match_tunnel_id)) {
            return BCM_E_BADID;
        }
        
        BCM_IF_ERROR_RETURN(
            _bcm_td2_vxlan_vpn_is_tunnel_based_vnid(unit, vpn, &vxlan_vpn_type));
        if ((vxlan_vpn_type == _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID) && 
            vxlan_port->vnid > 0xffffff) {
            return BCM_E_PARAM;
        }

        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(vxlan_port->match_tunnel_id);
        _BCM_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);
        vxlan_info->match_key[vp].match_tunnel_index = tunnel_idx;
        vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_VNID;

        if (vxlan_port->flags & BCM_VXLAN_PORT_MULTICAST) {
              return BCM_E_NONE;
        }
        mem = MPLS_ENTRYm;
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            mem = MPLS_ENTRY_SINGLEm;
            d = 1;
            soc_mem_field32_set(unit, mem, ment_svp, BASE_VALIDf, 1);
            soc_mem_field32_set(unit, mem, ment_svp,
                DATA_TYPEf, _BCM_VXLAN_FLEX_DATA_TYPE_IPV4_SIP);
        } else
#endif
        {
            soc_mem_field32_set(unit, mem, ment_svp, VALIDf, 1);
        }

        tunnel_sip = vxlan_info->vxlan_tunnel_term[tunnel_idx].sip;
        soc_mem_field32_set(unit, mem, ment_svp, BCM_VXLAN_SIP_FIELD(d, SIPf), tunnel_sip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
            VLAN_CHK_ID(unit, vxlan_port->match_vlan);
            vxlan_info->match_key[vp].match_vlan = vxlan_port->match_vlan;
            soc_mem_field32_set(unit, mem, ment_svp,
                VXLAN_SIP__OVIDf, vxlan_port->match_vlan);
        }
#endif
        soc_mem_field32_set(unit, mem, ment_svp, BCM_VXLAN_SIP_FIELD(d, SVPf), vp);
        soc_mem_field32_set(unit, mem, ment_svp, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_TUNNEL);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            rv = _bcm_td3_vxlan_match_tunnel_entry_set(unit, ment_svp, orignal_ment_svp);
        } else
#endif
        {
            rv = _bcm_td2_vxlan_match_tunnel_entry_set(unit, ment_svp, orignal_ment_svp);
        }
        BCM_IF_ERROR_RETURN(rv);
        if (vxlan_vpn_type) {
            int vfi_index;
            int tpid_index = -1;
            _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
            /* set egr_vlan_xlate table */
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                soc_mem_field32_set(unit, vxlate_mem, ment_evt, BASE_VALIDf, 1);
                soc_mem_field32_set(unit, vxlate_mem, ment_evt,
                                    DATA_TYPEf, _BCM_VXLAN_DATA_TYPE_LOOKUP_VFI);
                soc_mem_field32_set(unit, vxlate_mem, ment_evt,
                                KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP);
            } else
#endif
            { 
                soc_mem_field32_set(unit, vxlate_mem, ment_evt, ENTRY_TYPEf,
                                    _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP);
                soc_mem_field32_set(unit, vxlate_mem, ment_evt, VALIDf, 0x1);
            }
            soc_mem_field32_set(unit, vxlate_mem, ment_evt, BCM_VXLAN_VFI_FIELD(d, VFIf), vfi_index);
            soc_mem_field32_set(unit, vxlate_mem, ment_evt, BCM_VXLAN_VFI_FIELD(d, DVPf), vp);
            soc_mem_field32_set(unit, vxlate_mem, ment_evt, (d ? VXLAN_VFI_FLEX__VNIDf : VXLAN_VFI__VN_IDf),
                                 vxlan_port->vnid);
            /* Configure EGR_VLAN_XLATE - SD_TAG setting */
            if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
                int action_present, action_not_present;
                uint32 old_vxlate_entry[SOC_MAX_MEM_WORDS] = {0};

                rv = _bcm_td2_vxlan_egr_xlate_entry_get(unit, vfi_index, vp, old_vxlate_entry);
                if (BCM_FAILURE(rv)) {
                    goto match_add_clean1;
                }
#if defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    int profile_idx = -1;
                    profile_idx = soc_mem_field32_get(unit, vxlate_mem, old_vxlate_entry,
                                                                 VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf);
                    (void)_bcm_td3_sd_tag_action_get(unit, profile_idx, &action_present,
                                   &action_not_present);
                } else
#endif
                {
                    /*If old tpid exist, delete it*/
                    action_present = soc_mem_field32_get(unit, vxlate_mem, old_vxlate_entry,
                                                                 VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf);
                    action_not_present = soc_mem_field32_get(unit, vxlate_mem, old_vxlate_entry,
                                                             VXLAN_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf);
                }
                if (1 == action_present || 4 == action_present ||
                    7 == action_present || 1 == action_not_present) {
                    tpid_index = soc_mem_field32_get(unit,
                                     vxlate_mem, old_vxlate_entry, 
                                     d ? VXLAN_VFI_FLEX__OUTER_TPID_INDEXf : VXLAN_VFI__SD_TAG_TPID_INDEXf);
                    rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
                    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                        goto match_add_clean1;
                    }            
                }
            }

            if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TAGGED) {
                rv = _bcm_td2_vxlan_sd_tag_set(unit, NULL, vxlan_port, NULL, ment_evt, &tpid_index);
                if (BCM_FAILURE(rv)) {
                    goto match_add_clean1;
                }
            }
            rv = _bcm_td2_vxlan_match_egr_vxlate_entry_set(unit, vpn, vxlan_port, ment_evt, orignal_ment_egr_vt);
            if (BCM_FAILURE(rv)) {
                goto match_add_clean1;
            }
            
            /* set mpls_entry table */
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                _bcm_td3_vxlan_vxlate_entry_t ent;

                mem = VLAN_XLATE_1_DOUBLEm;
                sal_memset(&ent, 0, sizeof(_bcm_td3_vxlan_vxlate_entry_t));
                ent.dtype = _BCM_VXLAN_DATA_TYPE_VNID_VFI_FLEX;
                ent.ktype = _BCM_VXLAN_KEY_TYPE_VNID_VFI_SIP_FLEX;
                ent.vfi = vfi_index;
                ent.vlan = vxlan_info->vxlan_vpn_vlan[vfi_index];
                _bcm_td3_vxlan_vxlate_entry_assemble(unit, ent, ment_vnid);
            } else
#endif
            {
                soc_mem_field32_set(unit, mem, ment_vnid, VALIDf, 1);
                soc_mem_field32_set(unit, mem, ment_vnid, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_TUNNEL_VNID_VFI);
                soc_mem_field32_set(unit, mem, ment_vnid, VXLAN_VN_ID__VFIf, vfi_index);
            }
            soc_mem_field32_set(unit, mem, ment_vnid, BCM_VXLAN_VNID_FIELD(d, SIPf), tunnel_sip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                VLAN_CHK_ID(unit, vxlan_port->match_vlan);
                vxlan_info->match_key[vp].match_vlan = vxlan_port->match_vlan;
                soc_mem_field32_set(unit, mem, ment_vnid, VXLAN_VN_ID__OVIDf,
                                            vxlan_port->match_vlan);
            }
#endif
            soc_mem_field32_set(unit, mem, ment_vnid, (d ? VXLAN_FLEX__VNIDf : VXLAN_VN_ID__VN_IDf), vxlan_port->vnid);

#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
               rv = _bcm_td3_vxlan_match_vxlate_entry_set(unit, vpn, vxlan_port, ment_vnid, orignal_ment_vnid);
            } else
#endif
            {
                rv = _bcm_td2_vxlan_match_tunnel_entry_set(unit, ment_vnid, orignal_ment_vnid);
            }    
            if (BCM_FAILURE(rv)) {
                goto match_add_clean2;
            }
            vxlan_info->vxlan_vpn_info[vfi_index].vnid = vxlan_port->vnid;
            vxlan_info->vxlan_vpn_info[vfi_index].sip = tunnel_sip;
            vxlan_info->match_key[vp].vfi = vfi_index;
            return rv;
match_add_clean2:
            if (soc_feature(unit, soc_feature_base_valid)) {
                if ((soc_mem_field32_get(unit, vxlate_mem, orignal_ment_egr_vt, BASE_VALID_0f) == 3) &&
                    (soc_mem_field32_get(unit, vxlate_mem, orignal_ment_egr_vt, BASE_VALID_1f) == 7)) {
                     valid = 1;
                }
            } else {
                valid = soc_mem_field32_get(unit, vxlate_mem, orignal_ment_egr_vt, VALIDf);
            }
            if (valid) {
                rv = _bcm_td2_vxlan_match_egr_vxlate_entry_set(unit, vpn, vxlan_port, 
                                                               orignal_ment_egr_vt, 
                                                               ment_evt);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                rv = _bcm_td2_vxlan_match_egr_vxlate_entry_delete(unit, ment_evt);
                BCM_IF_ERROR_RETURN(rv);
            }
match_add_clean1:
            if (soc_feature(unit, soc_feature_base_valid)) {
                if ((soc_mem_field32_get(unit, vxlate_mem, orignal_ment_egr_vt, BASE_VALID_0f) == 3) &&
                    (soc_mem_field32_get(unit, vxlate_mem, orignal_ment_egr_vt, BASE_VALID_1f) == 7)) {
                     valid = 1;
                }     
            } else {
                valid = soc_mem_field32_get(unit, vxlate_mem, orignal_ment_egr_vt, VALIDf);
            } 
            if (valid) {
                rv = _bcm_td2_vxlan_match_tunnel_entry_set(unit, orignal_ment_svp, ment_svp);
                BCM_IF_ERROR_RETURN(rv);                   
            } else {
                rv = _bcm_td2_vxlan_match_tunnel_entry_delete(unit, ment_svp);
                BCM_IF_ERROR_RETURN(rv);
            }
            if (tpid_index != -1) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            }

            return rv;    
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_delete
 * Purpose:
 *      Delete match criteria of an VXLAN port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vxlan_port - (IN) VXLAN port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_match_delete(int unit,  int vp , bcm_vxlan_port_t vxlan_port, bcm_vpn_t vpn)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
    int rv=BCM_E_NONE;
    soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS] = {0};
    bcm_trunk_t trunk_id;
    int    src_trk_idx=0;   /*Source Trunk table index.*/
    int    mod_id_idx=0;   /* Module_Id */
    int port=0, tunnel_index = -1;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    int    gport_id =-1;
    uint32 ment_svp[SOC_MAX_MEM_WORDS] = {0};
    uint32 ment_vnid[SOC_MAX_MEM_WORDS] = {0};
    uint32 ment_evt[SOC_MAX_MEM_WORDS] = {0};
    uint32 orignal_ment_svp[SOC_MAX_MEM_WORDS] = {0};
    uint32 orignal_ment_vnid[SOC_MAX_MEM_WORDS] = {0};
    uint32 orignal_ment_evt[SOC_MAX_MEM_WORDS] = {0};
    uint8 vxlan_vpn_type = 0;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else
#endif
    {
        mem = VLAN_XLATEm;

        soc_mem_field32_set(unit, mem, vent, VALIDf, 1);
    }

    if  (vxlan_info->match_key[vp].flags == _BCM_VXLAN_PORT_MATCH_TYPE_VLAN) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent,
            XLATE__OVIDf, vxlan_info->match_key[vp].match_vlan);

        if (vxlan_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent,
                XLATE__MODULE_IDf, vxlan_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent,
                XLATE__PORT_NUMf, vxlan_info->match_key[vp].port);

        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent,
                XLATE__TGIDf, vxlan_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        bcm_td2_vxlan_port_match_count_adjust(unit, vp, -1);
    } else if  (vxlan_info->match_key[vp].flags == 
                     _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent,
            XLATE__IVIDf, vxlan_info->match_key[vp].match_inner_vlan);
        if (vxlan_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent,
                XLATE__MODULE_IDf, vxlan_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent,
                XLATE__PORT_NUMf, vxlan_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                        vxlan_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, vent);
        BCM_IF_ERROR_RETURN(rv);

        bcm_td2_vxlan_port_match_count_adjust(unit, vp, -1);
    }else if (vxlan_info->match_key[vp].flags == 
                    _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent,
            KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent,
            XLATE__OVIDf, vxlan_info->match_key[vp].match_vlan);
        soc_mem_field32_set(unit, mem, vent,
            XLATE__IVIDf, vxlan_info->match_key[vp].match_inner_vlan);
        if (vxlan_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent,
                XLATE__MODULE_IDf, vxlan_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent,
                XLATE__PORT_NUMf, vxlan_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent,
                XLATE__TGIDf, vxlan_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, vent);
        BCM_IF_ERROR_RETURN(rv);

        bcm_td2_vxlan_port_match_count_adjust(unit, vp, -1);
    } else if   (vxlan_info->match_key[vp].flags ==
                                               _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
        }
#endif
        soc_mem_field32_set(unit, mem, vent,
            XLATE__OTAGf, vxlan_info->match_key[vp].match_vlan);
        if (vxlan_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent,
                XLATE__MODULE_IDf, vxlan_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent,
                XLATE__PORT_NUMf, vxlan_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent,
                XLATE__TGIDf, vxlan_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        bcm_td2_vxlan_port_match_count_adjust(unit, vp, -1);
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    else if (vxlan_info->match_key[vp].flags == 
                    _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {

        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);

        if (BCM_VXLAN_PORT_MATCH_PORT_VLAN == vxlan_port.criteria) {
            soc_mem_field32_set(unit, mem, vent,
                KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (mem == VLAN_XLATE_1_DOUBLEm) {
                soc_mem_field32_set(unit, mem, vent,
                    DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
            }
#endif
            soc_mem_field32_set(unit, mem, vent,
                XLATE__OVIDf, vxlan_port.match_vlan);
        } else if (BCM_VXLAN_PORT_MATCH_PORT_VLAN_STACKED == vxlan_port.criteria) {
            soc_mem_field32_set(unit, mem, vent,
                KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (mem == VLAN_XLATE_1_DOUBLEm) {
                soc_mem_field32_set(unit, mem, vent,
                    DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
            }
#endif
            soc_mem_field32_set(unit, mem, vent,
                XLATE__OVIDf, vxlan_port.match_vlan);
            soc_mem_field32_set(unit, mem, vent,
                XLATE__IVIDf, vxlan_port.match_inner_vlan);
        } else if (BCM_VXLAN_PORT_MATCH_PORT_INNER_VLAN == vxlan_port.criteria) {
            soc_mem_field32_set(unit, mem, vent,
                KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (mem == VLAN_XLATE_1_DOUBLEm) {
                soc_mem_field32_set(unit, mem, vent,
                    DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
            }
#endif
            soc_mem_field32_set(unit, mem, vent,
                XLATE__IVIDf, vxlan_port.match_inner_vlan);
        } else if (BCM_VXLAN_PORT_MATCH_VLAN_PRI == vxlan_port.criteria) {
            soc_mem_field32_set(unit, mem, vent,
                KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (mem == VLAN_XLATE_1_DOUBLEm) {
                soc_mem_field32_set(unit, mem, vent,
                    DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
            }
#endif
            soc_mem_field32_set(unit, mem, vent,
                XLATE__OTAGf, vxlan_port.match_vlan);
        } else {
            return BCM_E_UNAVAIL;
        }
        
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, vxlan_port.match_port, &mod_out,
                        &port_out, &trunk_id, &gport_id));
        
        if (BCM_GPORT_IS_TRUNK(vxlan_port.match_port)) {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_out);
        }

        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, vent);
        BCM_IF_ERROR_RETURN(rv);

        bcm_td2_vxlan_port_match_count_adjust(unit, vp, -1);
    }
#endif
    else if ((vxlan_info->match_key[vp].flags == _BCM_VXLAN_PORT_MATCH_TYPE_PORT)
             || ((vxlan_info->match_key[vp].flags == _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK)
                 && (!BCM_GPORT_IS_TRUNK(vxlan_port.match_port)))) {
        int is_local;

        if (vxlan_info->match_key[vp].flags == _BCM_VXLAN_PORT_MATCH_TYPE_PORT) {
            src_trk_idx = vxlan_info->match_key[vp].index;
        } else {
            /*
             * The vp has been created based on trunk.
             * When a new port is deleted from the trunk, a match will be deleted
             * for the vp through bcm_port_match_delete.
             */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, vxlan_port.match_port, &mod_out,
                                       &port_out, &trunk_id, &gport_id));
            /* Get index to source trunk map table */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_src_mod_port_table_index_get(
                    unit, mod_out, port_out, &src_trk_idx));
        }
        rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                    src_trk_idx, SOURCE_VPf, 0);
        BCM_IF_ERROR_RETURN(rv);

        /* Disable SVP Mode */
        if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, SVP_VALIDf) ) {
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        src_trk_idx, SVP_VALIDf, 0x0);
            BCM_IF_ERROR_RETURN(rv);
        }

        BCM_IF_ERROR_RETURN(
            _bcm_esw_src_modid_port_get(unit, src_trk_idx, &mod_id_idx, &port));
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_id_idx, &is_local));

        if (is_local) {
            rv = _bcm_esw_port_tab_set(unit, vxlan_port.match_port,
                                       _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0);
            BCM_IF_ERROR_RETURN(rv);

            if (!(vxlan_port.flags2 &
                      BCM_VXLAN_PORT_FLAGS2_UT_VLAN_ACTION_NO_CLEAR)) {
                /* Reset TAG_ACTION_PROFILE_PTR */
                rv = _bcm_td2_vxlan_port_untagged_profile_reset(
                         unit, vxlan_port.match_port);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    } else if  (vxlan_info->match_key[vp].flags == 
                  _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK) {
         trunk_id = vxlan_info->match_key[vp].trunk_id;
         rv = bcm_td2_vxlan_match_trunk_delete(unit, trunk_id, vp,
                  !(vxlan_port.flags2 &
                        BCM_VXLAN_PORT_FLAGS2_UT_VLAN_ACTION_NO_CLEAR));
         BCM_IF_ERROR_RETURN(rv);
    } else if (vxlan_info->match_key[vp].flags == 
                  _BCM_VXLAN_PORT_MATCH_TYPE_VNID) {
        int d = 0; /*vxlan decoupled mode*/

        if (vxlan_port.flags & BCM_VXLAN_PORT_MULTICAST) {
            return BCM_E_NONE;
        }

        mem = MPLS_ENTRYm;
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_tunnel_based_vnid(unit, vpn, &vxlan_vpn_type));

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            mem = MPLS_ENTRY_SINGLEm;
            d = 1;
            soc_mem_field32_set(unit, mem, ment_svp, BASE_VALIDf, 1);
            soc_mem_field32_set(unit, mem, ment_svp,
                DATA_TYPEf, _BCM_VXLAN_FLEX_DATA_TYPE_IPV4_SIP);
        } else
#endif
        {
            soc_mem_field32_set(unit, mem, ment_svp, VALIDf, 1);
        }

        tunnel_index = vxlan_info->match_key[vp].match_tunnel_index;
        soc_mem_field32_set(unit, mem, ment_svp, BCM_VXLAN_SIP_FIELD(d, SIPf),
                vxlan_info->vxlan_tunnel_term[tunnel_index].sip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
            soc_mem_field32_set(unit, mem, ment_svp,
                VXLAN_SIP__OVIDf, vxlan_info->match_key[vp].match_vlan);
        }
#endif
        soc_mem_field32_set(unit, mem, ment_svp, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_TUNNEL);

        BCM_IF_ERROR_RETURN(
            _bcm_td2_vxlan_match_tunnel_entry_reset(unit, vp, ment_svp, 
                                                    orignal_ment_svp));  

        if (vxlan_vpn_type) {            
            int vfi_index;
            int index;
            soc_mem_t vxlate_mem;
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                d = 1;
                vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
            } else
#endif
            {     
               vxlate_mem = EGR_VLAN_XLATEm;
            }   
            
            _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
            /* set egr_vlan_xlate table */
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, vxlate_mem, ment_evt, BASE_VALIDf, 1);
                soc_mem_field32_set(unit, vxlate_mem, ment_evt,
                                    DATA_TYPEf, _BCM_VXLAN_DATA_TYPE_LOOKUP_VFI);
                soc_mem_field32_set(unit, vxlate_mem, ment_evt,
                                KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP);
            } else
#endif
            {
                soc_mem_field32_set(unit, vxlate_mem, ment_evt, ENTRY_TYPEf,
                                    _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP);
                soc_mem_field32_set(unit, vxlate_mem, ment_evt, VALIDf, 0x1);
            }
            soc_mem_field32_set(unit, vxlate_mem, ment_evt, BCM_VXLAN_VFI_FIELD(d, VFIf), vfi_index);
            soc_mem_field32_set(unit, vxlate_mem, ment_evt, BCM_VXLAN_VFI_FIELD(d, DVPf), vp);
                    
           rv = soc_mem_search(unit, vxlate_mem, MEM_BLOCK_ANY, &index,
                                ment_evt, orignal_ment_evt, 0);
            if (BCM_FAILURE(rv)) {
                goto match_delete_clean1;
            }            
            sal_memset(ment_evt, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
            rv = soc_mem_write(unit, vxlate_mem, MEM_BLOCK_ALL, index, ment_evt);
            if (BCM_FAILURE(rv)) {
                goto match_delete_clean1;

            }

            /* set mpls_entry table */
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem = VLAN_XLATE_1_DOUBLEm;
                soc_mem_field32_set(unit, mem, ment_vnid, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, ment_vnid, BASE_VALID_1f, 7);
                soc_mem_field32_set(unit, mem, ment_vnid,
                    DATA_TYPEf, _BCM_VXLAN_DATA_TYPE_VNID_VFI_FLEX);
                soc_mem_field32_set(unit, mem, &ment_vnid, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_VNID_VFI_SIP_FLEX);
            } else
#endif
            {
                soc_mem_field32_set(unit, mem, ment_vnid, VALIDf, 1);
                soc_mem_field32_set(unit, mem, ment_vnid, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_TUNNEL_VNID_VFI);
            }            
            soc_mem_field32_set(unit, mem, ment_vnid, BCM_VXLAN_VNID_FIELD(d, SIPf),
                                        vxlan_info->vxlan_tunnel_term[tunnel_index].sip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                soc_mem_field32_set(unit, mem, ment_vnid, VXLAN_VN_ID__OVIDf,
                                            vxlan_port.match_vlan);
            }
#endif
            soc_mem_field32_set(unit, mem, ment_vnid, (d ? VXLAN_FLEX__VNIDf : VXLAN_VN_ID__VN_IDf), vxlan_port.vnid);
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                                ment_vnid, orignal_ment_vnid, 0);
            if (BCM_FAILURE(rv)) {
                goto match_delete_clean2;
            }

            sal_memset(ment_vnid, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, ment_vnid);
            if (BCM_FAILURE(rv)) {
                goto match_delete_clean2;                
            }
            vxlan_info->vxlan_vpn_info[vfi_index].vnid = 0;
            vxlan_info->vxlan_vpn_info[vfi_index].sip = 0;
            vxlan_info->match_key[vp].vfi = 0;
        }
        if (!vxlan_info->vxlan_tunnel_term[tunnel_index].dip) {
            vxlan_info->vxlan_tunnel_term[tunnel_index].sip = 0; 
        }
        return rv;
match_delete_clean2:
        rv = _bcm_td2_vxlan_match_egr_vxlate_entry_set(unit, vpn, &vxlan_port, 
                                                       orignal_ment_evt, 
                                                       ment_evt);
        BCM_IF_ERROR_RETURN(rv);
match_delete_clean1:   
        rv = _bcm_td2_vxlan_match_tunnel_entry_set(unit, orignal_ment_svp, ment_svp);
        BCM_IF_ERROR_RETURN(rv);
        return rv;
    }
 
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_get
 * Purpose:
 *      Obtain match information of an VXLAN port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vxlan_port - (OUT) VXLAN port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_match_get(int unit, bcm_vxlan_port_t *vxlan_port, int vp, bcm_vpn_t vpn)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
    int rv = BCM_E_NONE;
    bcm_module_t mod_in=0, mod_out=0;
    bcm_port_t port_in=0, port_out=0;
    bcm_trunk_t trunk_id=0;
    int    src_trk_idx=0;    /*Source Trunk table index.*/
    int    tunnel_idx = -1;
    uint8  vxlan_vpn_type;
    if  (vxlan_info->match_key[vp].flags & _BCM_VXLAN_PORT_MATCH_TYPE_VLAN) {

         vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT_VLAN;
         vxlan_port->match_vlan = vxlan_info->match_key[vp].match_vlan;

        if (vxlan_info->match_key[vp].trunk_id != -1) {
             trunk_id = vxlan_info->match_key[vp].trunk_id;
             BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
        } else {
             port_in = vxlan_info->match_key[vp].port;
             mod_in = vxlan_info->match_key[vp].modid;
             rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
             vxlan_port->match_port =
                 _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
        }
    } else if (vxlan_info->match_key[vp].flags &  
                 _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN) {
         vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT_INNER_VLAN;
         vxlan_port->match_inner_vlan = vxlan_info->match_key[vp].match_inner_vlan;

        if (vxlan_info->match_key[vp].trunk_id != -1) {
             trunk_id = vxlan_info->match_key[vp].trunk_id;
             BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
        } else {
             port_in = vxlan_info->match_key[vp].port;
             mod_in = vxlan_info->match_key[vp].modid;
             rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
             vxlan_port->match_port =
                 _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
        }
    }else if (vxlan_info->match_key[vp].flags &
                    _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED) {

         vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT_VLAN_STACKED;
         vxlan_port->match_vlan = vxlan_info->match_key[vp].match_vlan;
         vxlan_port->match_inner_vlan = vxlan_info->match_key[vp].match_inner_vlan;

         if (vxlan_info->match_key[vp].trunk_id != -1) {
              trunk_id = vxlan_info->match_key[vp].trunk_id;
              BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
         } else {
              port_in = vxlan_info->match_key[vp].port;
              mod_in = vxlan_info->match_key[vp].modid;
              rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                             mod_in, port_in, &mod_out, &port_out);
              vxlan_port->match_port =
                  _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
         }
    } else if  (vxlan_info->match_key[vp].flags & _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI) {

         vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_VLAN_PRI;
         vxlan_port->match_vlan = vxlan_info->match_key[vp].match_vlan;

        if (vxlan_info->match_key[vp].trunk_id != -1) {
             trunk_id = vxlan_info->match_key[vp].trunk_id;
             BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
        } else {
             port_in = vxlan_info->match_key[vp].port;
             mod_in = vxlan_info->match_key[vp].modid;
             rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
             vxlan_port->match_port =
                 _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
        }
    } else if (vxlan_info->match_key[vp].flags &
                   _BCM_VXLAN_PORT_MATCH_TYPE_PORT) {

         src_trk_idx = vxlan_info->match_key[vp].index;
         BCM_IF_ERROR_RETURN(_bcm_esw_src_modid_port_get( unit, src_trk_idx,
                                                          &mod_in, &port_in));

         vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT;
         rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                               mod_in, port_in, &mod_out, &port_out);
         vxlan_port->match_port =
             _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
         vxlan_port->flags2 = (vxlan_info->match_key[vp].flags2 &
              _BCM_VXLAN_PORT_MATCH_NO_UNTAG_OP) ?
                   BCM_VXLAN_PORT_FLAGS2_UT_VLAN_ACTION_NO_CLEAR : 0;
    }else if (vxlan_info->match_key[vp].flags &
                  _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK) {

         trunk_id = vxlan_info->match_key[vp].trunk_id;
         vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT;
         BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
         vxlan_port->flags2 = (vxlan_info->match_key[vp].flags2 &
             _BCM_VXLAN_PORT_MATCH_NO_UNTAG_OP) ?
                 BCM_VXLAN_PORT_FLAGS2_UT_VLAN_ACTION_NO_CLEAR : 0;
    } else if ((vxlan_info->match_key[vp].flags &
                          _BCM_VXLAN_PORT_MATCH_TYPE_VNID)) {
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_VN_ID;

        tunnel_idx = vxlan_info->match_key[vp].match_tunnel_index;
        BCM_GPORT_TUNNEL_ID_SET(vxlan_port->match_tunnel_id, tunnel_idx);
        vxlan_port->match_vlan = vxlan_info->match_key[vp].match_vlan;
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_tunnel_based_vnid(unit, vpn, &vxlan_vpn_type));
        if (vxlan_vpn_type) {
            int vfi_idx;
            _BCM_VXLAN_VPN_GET(vfi_idx, _BCM_VPN_TYPE_VFI, vpn);
            vxlan_port->vnid = vxlan_info->vxlan_vpn_info[vfi_idx].vnid;
        }
    }else if (vxlan_info->match_key[vp].flags & _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_SHARE;
    } else {
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_NONE;
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_eline_vp_map_set
 * Purpose:
 *      Set VXLAN ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_vxlan_eline_vp_map_set(int unit, int vfi_index, int vp1, int vp2)
{
    vfi_entry_t vfi_entry;
    int rv=BCM_E_NONE;
    int num_vp=0;
    int vp1_invalid = 0;
    int vp2_invalid = 0;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
         if ((vp1 > 0) && (vp1 < num_vp)) {
              soc_VFIm_field32_set(unit, &vfi_entry, VP_0f, vp1);
         } else {
              vp1_invalid = 1;
         }
         if ((vp2 > 0) && (vp2 < num_vp)) {
              soc_VFIm_field32_set(unit, &vfi_entry, VP_1f, vp2);
         } else {
              vp2_invalid = 1;
         }
         if (vp1_invalid && vp2_invalid) {
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
 *      _bcm_td2_vxlan_eline_vp_map_get
 * Purpose:
 *      Get VXLAN ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_eline_vp_map_get(int unit, int vfi_index, int *vp1, int *vp2)
{
    vfi_entry_t vfi_entry;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVxlan)) {
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
 *      _bcm_td2_vxlan_eline_vp_map_clear
 * Purpose:
 *      Clear VXLAN ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_eline_vp_map_clear(int unit, int vfi_index, int vp1, int vp2)
{
    vfi_entry_t vfi_entry;
    int rv=BCM_E_NONE;
    int num_vp=0;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        if ((vp1 > 0) && (vp1 < num_vp)) {
            soc_VFIm_field32_set(unit, &vfi_entry, VP_0f, 0);
        }
        if ((vp2 > 0) && (vp2 < num_vp)) {
            soc_VFIm_field32_set(unit, &vfi_entry, VP_1f, 0);
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
 *      _bcm_td2_vxlan_eline_vp_configure
 * Purpose:
 *      Configure VXLAN ELINE virtual port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_eline_vp_configure (int unit, int vfi_index, int active_vp, 
                  source_vp_entry_t *svp, uint32 tpid_enable, bcm_vxlan_port_t  *vxlan_port)
{
    int rv = BCM_E_NONE;
    int network_group=0;
    source_vp_2_entry_t svp_2_entry;

    /* Set SOURCE_VP */
    soc_SOURCE_VPm_field32_set(unit, svp, CLASS_IDf,
                                vxlan_port->if_class);

    /* Default Split Horizon Group Id
     * 0 - For Access Port;1 - For Network Port*/
    network_group = vxlan_port->network_group_id;
    rv = _bcm_validate_splithorizon_network_group(unit,
            vxlan_port->flags & BCM_VXLAN_PORT_NETWORK, &network_group);
    BCM_IF_ERROR_RETURN(rv);
    if (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                 network_group);
        } else {
            if (vxlan_port->flags2 & BCM_VXLAN_PORT_FLAGS2_NO_SPLIT_HORIZON_SOURCE) {
                soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf, 0);
            } else {
                soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf, 1);
            }
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, svp, TPID_SOURCEf, _BCM_VXLAN_TPID_SVP_BASED);
        }
    } else {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                        network_group);
        } else {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf, 0);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, svp, TPID_SOURCEf, _BCM_VXLAN_TPID_SVP_BASED);
            /*  Configure IPARS Parser to signal to IMPLS Parser - Applicable only for HG2 PPD2 packets
                  to reconstruct TPID state based on HG header information - Only for VXLAN Access ports */
            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            soc_SOURCE_VP_2m_field32_set(unit, &svp_2_entry, PARSE_USING_SGLP_TPIDf, 1);
            BCM_IF_ERROR_RETURN(WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, active_vp, &svp_2_entry));
        }
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TAGGED) {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 1);
#if defined(BCM_TRIDENT3_SUPPORT)
         if (SOC_IS_TRIDENT3X(unit)) {
             uint32 svp1[SOC_MAX_MEM_WORDS];

             sal_memset(svp1, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
             soc_SVP_ATTRS_1m_field_set(unit, &svp1, TPID_ENABLEf, &tpid_enable);
             BCM_IF_ERROR_RETURN(
                WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ALL, active_vp, &svp1));
         } else
#endif
         {
             soc_SOURCE_VPm_field32_set(unit, svp, TPID_ENABLEf, tpid_enable);
         }
    } else {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 0);
    }

    soc_SOURCE_VPm_field32_set(unit, svp, DISABLE_VLAN_CHECKSf, 1);
    soc_SOURCE_VPm_field32_set(unit, svp, 
                           ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VFI); /* VFI Type */
    soc_SOURCE_VPm_field32_set(unit, svp, VFIf, vfi_index);

    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, svp);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        if ((vfi_index != _BCM_VXLAN_VFI_INVALID) &&
            (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK)) {
            if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit, SOURCE_VPm,
                active_vp, ENTRY_TYPEf, CANCUN_APP__SOURCE_VP__No_Control_Word,
                _BCM_VXLAN_SOURCE_VP_TYPE_VFI)) {
                LOG_ERROR(BSL_LS_BCM_VXLAN,
                   (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
            }
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_access_niv_pe_set
 * Purpose:
 *      Set VXLAN Access virtual port which is of NIV/PE type
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp      -  (IN) Access Virtual Port
 *      vfi      -  (IN) Virtial forwarding instance
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_access_niv_pe_set (int unit, int vp, int vfi)
{
    source_vp_entry_t svp;
    int  rv = BCM_E_PARAM; 

        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            return rv;
        }

        if (vfi == _BCM_VXLAN_VFI_INVALID) {
            soc_SOURCE_VPm_field32_set(unit, &svp, 
                                       ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_INVALID);
        } else {
            /* Initialize VP parameters */
            soc_SOURCE_VPm_field32_set(unit, &svp, 
                                       ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VFI);
        }     
        soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi);
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_access_niv_pe_reset
 * Purpose:
 *      Reset VXLAN Access virtual port which is of NIV/PE type
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp      -  (IN) Access Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_access_niv_pe_reset (int unit, int vp)
{
    source_vp_entry_t svp;

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

    /* Initialize VP parameters */
    soc_SOURCE_VPm_field32_set(unit, &svp,
        ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VLAN);
    soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, 0);
    BCM_IF_ERROR_RETURN(WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2_vxlan_vp_based_vnid_add
 * Purpose:
 *      Add VXLAN VP-based VFI-VNID mapping
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      vxlan_port - (IN) VXLAN port information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_vp_based_vnid_add(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t *vxlan_port)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
    int vp, vfi;
    _bcm_vp_info_t vp_info;
    uint32 tunnel_sip;
    int32 tunnel_idx;
    soc_mem_t vxlate_mem;
    soc_mem_t mmem = MPLS_ENTRYm;
    int valid = 0;
    int tpid_index = -1;
    int d = 0; /*vxlan decoupled mode*/
    uint32 ment_evt[SOC_MAX_MEM_WORDS] = {0};
    uint32 ment_vnid[SOC_MAX_MEM_WORDS] = {0};
    uint32 original_ment_evt[SOC_MAX_MEM_WORDS] = {0};
    uint32 original_ment_vnid[SOC_MAX_MEM_WORDS] = {0};
    uint8 vxlan_vpn_type;
    int rv;
    source_vp_entry_t svp;
    bcm_vxlan_port_t  vxlan_port_r;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
        d = 1;
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
        }

    if (vxlan_port == NULL) {
        return BCM_E_PARAM;
            }

                BCM_IF_ERROR_RETURN (
        _bcm_td2_vxlan_vpn_is_tunnel_based_vnid(unit, vpn, &vxlan_vpn_type));
    if (vxlan_vpn_type == _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID) {
        /* could not co-exist */
        return BCM_E_PARAM;
            }

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
    _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);


    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
    if (vp == -1) {
                   return BCM_E_PARAM;
                }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                     return BCM_E_NOT_FOUND;
                }
    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (!(vp_info.flags & _BCM_VP_INFO_NETWORK_PORT)) {
        return BCM_E_PARAM;
        }

    bcm_vxlan_port_t_init(&vxlan_port_r);
    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_port_get(unit, vpn, vp, &vxlan_port_r));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
        if (!(vxlan_port_r.flags & BCM_VXLAN_PORT_MULTICAST)) {
            /* match_vlan will be used in MPLS_ENTRY table */
            if (vxlan_port->match_vlan != vxlan_info->match_key[vp].match_vlan) {
                                     return BCM_E_PARAM;
                                }
                                }
                                }
#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT) */

    BCM_IF_ERROR_RETURN (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
    if (vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf)) {
        return BCM_E_PARAM;
    }

    /* set egr_vlan_xlate table */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        soc_mem_field32_set(unit, vxlate_mem, ment_evt, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, vxlate_mem, ment_evt,
                            DATA_TYPEf, _BCM_VXLAN_DATA_TYPE_LOOKUP_VFI);
        soc_mem_field32_set(unit, vxlate_mem, ment_evt,
                        KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP);
    } else
#endif
    {
        soc_mem_field32_set(unit, vxlate_mem, ment_evt, ENTRY_TYPEf,
                            _BCM_VXLAN_KEY_TYPE_LOOKUP_VFI_DVP);
        soc_mem_field32_set(unit, vxlate_mem, ment_evt, VALIDf, 0x1);
                                }
    soc_mem_field32_set(unit, vxlate_mem, ment_evt, BCM_VXLAN_VFI_FIELD(d, DVPf), vp);
    soc_mem_field32_set(unit, vxlate_mem, ment_evt, BCM_VXLAN_VFI_FIELD(d, VFIf), vfi);
    soc_mem_field32_set(unit, vxlate_mem, ment_evt, (d ? VXLAN_VFI_FLEX__VNIDf : VXLAN_VFI__VN_IDf),
                            vxlan_port->vnid);

    if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TAGGED) {
        rv = _bcm_td2_vxlan_sd_tag_set(unit, NULL, vxlan_port, NULL, ment_evt, &tpid_index);
        if (BCM_FAILURE(rv)) {
                                      return rv;
                                  }
                             }

    rv = _bcm_td2_vxlan_match_egr_vxlate_entry_set(unit, vpn, vxlan_port, ment_evt, original_ment_evt);
    if (BCM_FAILURE(rv)) {
        goto _rollback;
    }

    if (vxlan_port_r.flags & BCM_VXLAN_PORT_MULTICAST) {
        vxlan_info->vfi_vnid_map_count[vp]++;
         return BCM_E_NONE;
    }

    /* set mpls_entry table */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        _bcm_td3_vxlan_vxlate_entry_t ent;
        mmem = VLAN_XLATE_1_DOUBLEm;
        sal_memset(&ent, 0, sizeof(_bcm_td3_vxlan_vxlate_entry_t));
        ent.dtype = _BCM_VXLAN_DATA_TYPE_VNID_VFI_FLEX;
        ent.ktype = _BCM_VXLAN_KEY_TYPE_VNID_VFI_SIP_FLEX;
        ent.vfi = vfi;
        ent.vlan = vxlan_info->vxlan_vpn_vlan[vfi];
        _bcm_td3_vxlan_vxlate_entry_assemble(unit, ent, ment_vnid);
    } else
#endif
    {
        soc_mem_field32_set(unit, mmem, ment_vnid, VALIDf, 1);
        soc_mem_field32_set(unit, mmem, ment_vnid, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_TUNNEL_VNID_VFI);
        soc_mem_field32_set(unit, mmem, ment_vnid, VXLAN_VN_ID__VFIf, vfi);
    }
    tunnel_idx = vxlan_info->match_key[vp].match_tunnel_index;
    tunnel_sip = vxlan_info->vxlan_tunnel_term[tunnel_idx].sip;
    soc_mem_field32_set(unit, mmem, ment_vnid, BCM_VXLAN_VNID_FIELD(d, SIPf), tunnel_sip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
        soc_mem_field32_set(unit, mmem, ment_vnid, VXLAN_VN_ID__OVIDf,
                                    vxlan_port->match_vlan);
    }
#endif
    soc_mem_field32_set(unit, mmem, ment_vnid, (d ? VXLAN_FLEX__VNIDf : VXLAN_VN_ID__VN_IDf), vxlan_port->vnid);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        rv = _bcm_td3_vxlan_match_vxlate_entry_set(unit, vpn, vxlan_port, ment_vnid, original_ment_vnid);
    } else
#endif
    {
        rv = _bcm_td2_vxlan_match_tunnel_entry_set(unit, ment_vnid, original_ment_vnid);
    }
    if (BCM_FAILURE(rv)) {
        goto _rollback;
    }

    vxlan_info->vfi_vnid_map_count[vp]++;

    return rv;

_rollback:
    if (tpid_index != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        if ((soc_mem_field32_get(unit, vxlate_mem, original_ment_evt, BASE_VALID_0f) == 3) &&
            (soc_mem_field32_get(unit, vxlate_mem, original_ment_evt, BASE_VALID_1f) == 7)) {
             valid = 1;
        } else {
            valid = 0;
        }
    } else {
        valid = soc_mem_field32_get(unit, vxlate_mem, original_ment_evt, VALIDf);
    }
    if (valid) {
        (void) _bcm_td2_vxlan_match_egr_vxlate_entry_set(unit, vpn, vxlan_port,
                                                       original_ment_evt,
                                                       ment_evt);
    } else {
       (void) _bcm_td2_vxlan_match_egr_vxlate_entry_delete(unit, ment_evt);
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        if ((soc_mem_field32_get(unit, vxlate_mem, original_ment_vnid, BASE_VALID_0f) == 3) &&
            (soc_mem_field32_get(unit, vxlate_mem, original_ment_vnid, BASE_VALID_1f) == 7)) {
             valid = 1;
        } else {
            valid = 0;
        }
    } else {
        valid = soc_mem_field32_get(unit, vxlate_mem, original_ment_vnid, VALIDf);
    }
    if (valid) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            (void) _bcm_td3_vxlan_match_vxlate_entry_set(unit, vpn, vxlan_port, original_ment_vnid, ment_vnid);
        } else
#endif
        {
            (void) _bcm_td2_vxlan_match_tunnel_entry_set(unit, original_ment_vnid, ment_vnid);
        }
    } else {
        (void) _bcm_td2_vxlan_match_tunnel_entry_delete(unit, ment_vnid);
    }

    return rv;
}


/*
 * Function:
 *      _bcm_td2_vxlan_vp_based_vnid_delete
 * Purpose:
 *      Delete VXLAN VP-based VFI-VNID mapping
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      vxlan_port - (IN) VXLAN port information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_vp_based_vnid_delete(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t *vxlan_port)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
    int vp, vfi;
    _bcm_vp_info_t vp_info;
    uint32 tunnel_sip;
    int32 tunnel_idx;
    soc_mem_t vxlate_mem;
    soc_mem_t mmem = MPLS_ENTRYm;
    int tpid_index = -1;
    int d = 0; /*vxlan decoupled mode*/
    uint32 ment_vnid[SOC_MAX_MEM_WORDS] = {0};
    uint32 original_ment_vnid[SOC_MAX_MEM_WORDS] = {0};
    uint8 vxlan_vpn_type;
    int rv;
    int action_present, action_not_present;
    uint32 old_vxlate_entry[SOC_MAX_MEM_WORDS] = {0};
    int index;
    uint32 vnid;


#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
        d = 1;
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
    }

    if (vxlan_port == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2_vxlan_vpn_is_tunnel_based_vnid(unit, vpn, &vxlan_vpn_type));
    if (vxlan_vpn_type == _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
    _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }
    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (!(vp_info.flags & _BCM_VP_INFO_NETWORK_PORT)) {
        return BCM_E_PARAM;
    }


    rv = _bcm_td2_vxlan_egr_xlate_entry_get(unit, vfi, vp, old_vxlate_entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        int profile_idx = -1;
        profile_idx = soc_mem_field32_get(unit, vxlate_mem, old_vxlate_entry,
                                                     VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf);
        (void)_bcm_td3_sd_tag_action_get(unit, profile_idx, &action_present,
                       &action_not_present);
    } else
#endif
    {
        /*If tpid exists, delete it*/
        action_present = soc_mem_field32_get(unit, vxlate_mem, old_vxlate_entry,
                                                     VXLAN_VFI__SD_TAG_ACTION_IF_PRESENTf);
        action_not_present = soc_mem_field32_get(unit, vxlate_mem, old_vxlate_entry,
                                                 VXLAN_VFI__SD_TAG_ACTION_IF_NOT_PRESENTf);
    }

    if (1 == action_present || 4 == action_present ||
        7 == action_present || 1 == action_not_present) {
        tpid_index = soc_mem_field32_get(unit,
                         vxlate_mem, old_vxlate_entry,
                         d ? VXLAN_VFI_FLEX__OUTER_TPID_INDEXf : VXLAN_VFI__SD_TAG_TPID_INDEXf);
        rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    vnid = soc_mem_field32_get(unit, vxlate_mem, old_vxlate_entry, (d ? VXLAN_VFI_FLEX__VNIDf : VXLAN_VFI__VN_IDf));

    rv = _bcm_td2_vxlan_match_egr_vxlate_entry_delete(unit, old_vxlate_entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (!(vxlan_port->flags & BCM_VXLAN_PORT_MULTICAST)) {
        /* set mpls_entry table */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            _bcm_td3_vxlan_vxlate_entry_t ent;
            mmem = VLAN_XLATE_1_DOUBLEm;
            sal_memset(&ent, 0, sizeof(_bcm_td3_vxlan_vxlate_entry_t));
            ent.dtype = _BCM_VXLAN_DATA_TYPE_VNID_VFI_FLEX;
            ent.ktype = _BCM_VXLAN_KEY_TYPE_VNID_VFI_SIP_FLEX;
            ent.vfi = vfi;
            ent.vlan = vxlan_info->vxlan_vpn_vlan[vfi];
            _bcm_td3_vxlan_vxlate_entry_assemble(unit, ent, ment_vnid);
        } else
#endif
        {
            soc_mem_field32_set(unit, mmem, ment_vnid, VALIDf, 1);
            soc_mem_field32_set(unit, mmem, ment_vnid, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_TUNNEL_VNID_VFI);
            soc_mem_field32_set(unit, mmem, ment_vnid, VXLAN_VN_ID__VFIf, vfi);
        }
        tunnel_idx = vxlan_info->match_key[vp].match_tunnel_index;
        tunnel_sip = vxlan_info->vxlan_tunnel_term[tunnel_idx].sip;
        soc_mem_field32_set(unit, mmem, ment_vnid, BCM_VXLAN_VNID_FIELD(d, SIPf), tunnel_sip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
            soc_mem_field32_set(unit, mmem, ment_vnid, VXLAN_VN_ID__OVIDf,
                                    vxlan_info->match_key[vp].match_vlan);
        }
#endif
        soc_mem_field32_set(unit, mmem, ment_vnid, (d ? VXLAN_FLEX__VNIDf : VXLAN_VN_ID__VN_IDf),
                                vnid);

        rv = soc_mem_search(unit, mmem, MEM_BLOCK_ANY, &index, ment_vnid, original_ment_vnid, 0);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        sal_memset(ment_vnid, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
        rv = soc_mem_write(unit, mmem, MEM_BLOCK_ALL, index, ment_vnid);
    }

    vxlan_info->vfi_vnid_map_count[vp]--;

    return rv;
}


/*
 * Function:
 *      _bcm_td2_vxlan_vp_based_vnid_get
 * Purpose:
 *      Get VXLAN VP-based VFI-VNID mapping
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      vxlan_port - (IN/out) VXLAN port information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_vp_based_vnid_get(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t *vxlan_port)
{
    int vp, vfi;
    _bcm_vp_info_t vp_info;
    soc_mem_t vxlate_mem;
    int d = 0; /*vxlan decoupled mode*/
    uint8 vxlan_vpn_type;
    int rv = BCM_E_NONE;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS] = {0};

    if (vxlan_port == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        vxlate_mem = EGR_VLAN_XLATE_1_SINGLEm;
        d = 1;
    } else
#endif
    {
        vxlate_mem = EGR_VLAN_XLATEm;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2_vxlan_vpn_is_tunnel_based_vnid(unit, vpn, &vxlan_vpn_type));
    if (vxlan_vpn_type == _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID) {
        /* should not use this API */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
    _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }
    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (!(vp_info.flags & _BCM_VP_INFO_NETWORK_PORT)) {
        return BCM_E_BADID;
    }

    rv = _bcm_td2_vxlan_egr_xlate_entry_get(unit, vfi, vp, vxlate_entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Get SDTAG settings */
    (void)_bcm_td2_vxlan_sd_tag_get(unit, NULL, vxlan_port, NULL, vxlate_entry, 1);
    vxlan_port->vnid = soc_mem_field32_get(unit, vxlate_mem, vxlate_entry,
                           (d ? VXLAN_VFI_FLEX__VNIDf : VXLAN_VFI__VN_IDf));

    vxlan_port->flags |= BCM_VXLAN_PORT_VPN_BASED;

    return rv;
}


/*
 * Function:
 *      _bcm_td2_vxlan_vp_based_vnid_count
 * Purpose:
 *      Get number of VXLAN VP-based VFI-VNID mapping rules
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp      - (IN) VXLAN port ID
 *      count   - (out) number of VFI-VNID mapping rules
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_vp_based_vnid_count(int unit, int vp, uint32 *count)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);

    if (count == NULL) {
        return BCM_E_PARAM;
    }

    if (vp == -1) {
        return BCM_E_PARAM;
    }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }

    *count = vxlan_info->vfi_vnid_map_count[vp];

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2_vxlan_vp_based_vnid_delete_all
 * Purpose:
 *      Delete all VXLAN VP-based VFI-VNID mappings
 * Parameters:
 *      unit    - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_vp_based_vnid_delete_all(int unit)
{
    int rv = BCM_E_NONE;
    uint32 num_vp;
    uint32 vp;
    uint32 count;
    uint32 vfi, num_vfi;
    bcm_vpn_t vpn;
    bcm_vxlan_port_t vxlan_port;

    num_vp  = soc_mem_index_count(unit, SOURCE_VPm);
    num_vfi = soc_mem_index_count(unit, VFIm);

    SHR_BIT_ITER(VIRTUAL_INFO(unit)->vxlan_vp_bitmap, num_vp, vp) {
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vp_based_vnid_count(unit, vp, &count));
        for (vfi = 0; (vfi < num_vfi) && count; vfi++) {
            if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVxlan)) {
                _BCM_VXLAN_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, vfi);
                bcm_vxlan_port_t_init(&vxlan_port);
                BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port.vxlan_port_id, vp);
                rv = _bcm_td2_vxlan_vp_based_vnid_get(unit, vpn, &vxlan_port);
                if (BCM_SUCCESS(rv)) {
                    bcm_vxlan_port_t_init(&vxlan_port);
                    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_port_get(unit, vpn, vp, &vxlan_port));
                    rv = _bcm_td2_vxlan_vp_based_vnid_delete(unit, vpn, &vxlan_port);
                    if (BCM_SUCCESS(rv)) {
                        count--;
                    } else {
                        return rv;
                    }
                }
            }
        }

    }

    return rv;
}


/*
 * Function:
 *      _bcm_td2_vxlan_vplag_vp_update
 * Purpose:
 *      Update vplag vp according to VXLAN port
 * Parameters:
 *      unit   - (IN) Device Number
 *      gport  - (IN) VXLAN port
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_vxlan_vplag_vp_update(int unit, bcm_gport_t gport)
{
    int rv = 0;
    int vp = 0;
    int vplag_vp = 0;
    uint32 vfi = 0;
    uint32 network_port = 0;
    uint32 network_group = 0;
    uint32 tpid_enable = 0;
    uint32 sd_tag_mode = 0;
    source_vp_entry_t svp_entry;
    source_vp_entry_t svp_entry1;

    rv = bcm_td2_vxlan_port_source_vp_lag_get(unit, gport, &vplag_vp);
    if (BCM_SUCCESS(rv)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
        BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
        BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vplag_vp, &svp_entry1));

        vfi = soc_SOURCE_VPm_field32_get(unit, &svp_entry, VFIf);
        sd_tag_mode = soc_SOURCE_VPm_field32_get(unit, &svp_entry, SD_TAG_MODEf);
        soc_SOURCE_VPm_field32_set(unit, &svp_entry1, VFIf, vfi);
        soc_SOURCE_VPm_field32_set(unit, &svp_entry1, SD_TAG_MODEf, sd_tag_mode);

        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            network_group = soc_SOURCE_VPm_field32_get(unit, &svp_entry, NETWORK_GROUPf);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry1, NETWORK_GROUPf, network_group);
        } else {
            network_port = soc_SOURCE_VPm_field32_get(unit, &svp_entry, NETWORK_PORTf);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry1, NETWORK_PORTf, network_port);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 svp1[SOC_MAX_MEM_WORDS];
            uint32 svp2[SOC_MAX_MEM_WORDS];
            BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, svp1));
            BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vplag_vp, svp2));
            soc_SVP_ATTRS_1m_field_get(unit, &svp1, TPID_ENABLEf, &tpid_enable);
            soc_SVP_ATTRS_1m_field_set(unit, &svp2, TPID_ENABLEf, &tpid_enable);
            BCM_IF_ERROR_RETURN(WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ALL, vplag_vp, &svp2));
        } else
#endif
        {
            tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp_entry, TPID_ENABLEf);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry1, TPID_ENABLEf, tpid_enable);
        }
        BCM_IF_ERROR_RETURN(WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vplag_vp, &svp_entry1));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2_vxlan_eline_port_add
 * Purpose:
 *      Add VXLAN ELINE port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      vxlan_port - (IN/OUT) VXLAN port information (OUT : vxlan_port_id)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_eline_port_add(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t  *vxlan_port)
{
    int active_vp = 0; /* default VP */
    source_vp_entry_t svp1, svp2;
    uint8 vp_valid_flag = 0;
    uint32 old_tpid_enable = 0;
    int i;
    uint32 tpid_enable = 0;
    uint32 tpid_enable_length = 0;
    int tpid_index = -1;
    int customer_drop=0;
    int  rv = BCM_E_PARAM;
    int num_vp=0;
    int vp1=-1, vp2=-1, vfi_index= -1;
    _bcm_vp_info_t vp_info;
    source_vp_entry_t active_svp;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 active_svp1[SOC_MAX_MEM_WORDS];
#endif

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeVxlan;

        if ( vpn != BCM_VXLAN_VPN_INVALID) {
            _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VXLAN_VPN_TYPE_ELINE,  vpn);
             if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVxlan)) {
                return BCM_E_NOT_FOUND;
             }
        } else {
             vfi_index = _BCM_VXLAN_VFI_INVALID;
        }

        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        if ( vpn != BCM_VXLAN_VPN_INVALID) {

             /* ---- Read in current table values for VP1 and VP2 ----- */
             (void) _bcm_td2_vxlan_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);

            if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeVxlan)) {
                BCM_IF_ERROR_RETURN (
                   READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp1, &svp1));
                if (soc_SOURCE_VPm_field32_get(unit, &svp1, ENTRY_TYPEf) == 0x1) {
                    vp_valid_flag |= 0x1;  /* -- VP1 Valid ----- */
                }
            }

            if (_bcm_vp_used_get(unit, vp2, _bcmVpTypeVxlan)) {
                BCM_IF_ERROR_RETURN (
                   READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp2, &svp2));
                if (soc_SOURCE_VPm_field32_get(unit, &svp2, ENTRY_TYPEf) == 0x1) {
                    vp_valid_flag |= 0x2;        /* -- VP2 Valid ----- */
                }
            }

            if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
                active_vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
                if (active_vp == -1) {
                   return BCM_E_PARAM;
                }
                if (!_bcm_vp_used_get(unit, active_vp, _bcmVpTypeVxlan)) {
                     return BCM_E_NOT_FOUND;
                }
                /* Decrement old-port's nexthop count */
                rv = _bcm_td2_vxlan_port_nh_cnt_dec(unit, active_vp);
                if (rv < 0) {
                    return rv;
                }
                /* Decrement old-port's VP count */
                rv = _bcm_td2_vxlan_port_cnt_update(unit, vxlan_port->vxlan_port_id,
                                                    active_vp, FALSE);
                if (rv < 0) {
                    return rv;
                }

                BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, active_vp,
                                                    &active_svp));
#if defined(BCM_TRIDENT3_SUPPORT)
                if (SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY,
                                                          active_vp,
                                                          &active_svp1));
                }
#endif
                if (soc_SOURCE_VPm_field32_get(unit, &active_svp, SD_TAG_MODEf)) {
                    /* SD-tag mode, save the old TPID enable bits */
#if defined(BCM_TRIDENT3_SUPPORT)
                    if (SOC_IS_TRIDENT3X(unit)) {
                        old_tpid_enable = soc_SVP_ATTRS_1m_field32_get(unit, &active_svp1,
                                                                       TPID_ENABLEf);
                        tpid_enable_length = soc_mem_field_length(unit,
                                                 SVP_ATTRS_1m, TPID_ENABLEf);
                    } else
#endif
                    {
                        old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &active_svp,
                                                                     TPID_ENABLEf);
                        tpid_enable_length = soc_mem_field_length(unit,
                                                 SOURCE_VPm, TPID_ENABLEf);
                    }
                }
            }
        }

        switch (vp_valid_flag) {

              case 0x0: /* No VP is valid */
                             if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
                                  return BCM_E_NOT_FOUND;
                             }

                             if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID) {
                                vp1 = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
                                if (vp1 == -1) {
                                     return BCM_E_PARAM;
                                }

                                if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeVxlan)) {
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
                                 * EGR_MPLS_VC_AND_SWAP_LABEL_TABLE and
                                 * EGR_DVP_ATTRIBUTE_1 are overlaid tables on
                                 * some chips. If there is conflict between
                                 * these two tables, the entries for
                                 * EGR_DVP_ATTRIBUTE_1 own higher priority.
                                 */
                                if (soc_feature(unit, soc_feature_mpls) &&
                                    soc_feature(unit, soc_feature_vc_and_swap_table_overlaid)) {
                                    rv = _bcm_tr_mpls_vc_and_swap_table_entry_adjust(unit, vp1);
                                    if (rv < 0) {
                                        (void)_bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp1);
                                        return rv;
                                    }
                                }
#endif

                                /* Allocate a new VP index for VP2 */
                                vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
                                 rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp2);
                                  if (rv < 0) {
                                     (void) _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp1);
                                      return rv;
                                  }
                             }

                             active_vp = vp1;
                             vp_valid_flag = 1;
                             sal_memset(&svp1, 0, sizeof(source_vp_entry_t));
                             sal_memset(&svp2, 0, sizeof(source_vp_entry_t));
                             (void) _bcm_td2_vxlan_eline_vp_map_set(unit, vfi_index, vp1, vp2);
                             break;


         case 0x1:    /* Only VP1 is valid */   
                             if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
                                  if (active_vp != vp1) {
                                       return BCM_E_NOT_FOUND;
                                  }
                             } else if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID) {
                                vp2 = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
                                if (vp2 == -1) {
                                     return BCM_E_PARAM;
                                }

                                if (_bcm_vp_used_get(unit, vp2, _bcmVpTypeVxlan)) {
                                     return BCM_E_EXISTS;
                                }
                                if (vp2 >= num_vp) {
                                     return (BCM_E_BADID);
                                }
                                vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
                                BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp2, vp_info));

                                active_vp = vp2;
                                vp_valid_flag = 3;
                                sal_memset(&svp2, 0, sizeof(source_vp_entry_t));
                             } else {
                                  active_vp = vp2;
                                  vp_valid_flag = 3;
                                  sal_memset(&svp2, 0, sizeof(source_vp_entry_t));
                             }

                             (void) _bcm_td2_vxlan_eline_vp_map_set(unit, vfi_index, vp1, vp2);
                             break;



         case 0x2: /* Only VP2 is valid */
                             if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
                                  if (active_vp != vp2) {
                                       return BCM_E_NOT_FOUND;
                                  }
                             } else if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID) {
                                vp1 = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
                                if (vp1 == -1) {
                                     return BCM_E_PARAM;
                                }

                                if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeVxlan)) {
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

                             (void) _bcm_td2_vxlan_eline_vp_map_set(unit, vfi_index, vp1, vp2);
                             break;


              
         case 0x3: /* VP1 and VP2 are valid */
                              if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
                                   return BCM_E_FULL;
                              }
                             break;
       }

       if (active_vp == -1) {
           return BCM_E_CONFIG;
       }

        /* Set VXLAN port ID */
        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, active_vp);
        }
       /* ======== Configure Next-Hop Properties ========== */
       customer_drop = (vxlan_port->flags & BCM_VXLAN_PORT_DROP) ? 1 : 0;
       rv = _bcm_td2_vxlan_port_nh_add(unit, vxlan_port, active_vp, vpn, customer_drop);
       if (rv < 0) {
            if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
                (void) _bcm_vp_free(unit, _bcmVfiTypeVxlan, 1, active_vp);
            }
            return rv;
       }

       /* ======== Configure Service-Tag Properties =========== */
       if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TAGGED) {
           rv = _bcm_fb2_outer_tpid_entry_add(unit,
                    vxlan_port->egress_service_tpid, &tpid_index);
           if (rv < 0) {
               goto vxlan_cleanup;
           }
           tpid_enable = (1 << tpid_index);
       }

       /* ======== Configure SVP Property ========== */
       if (active_vp == vp1) {
           rv  = _bcm_td2_vxlan_eline_vp_configure (unit, vfi_index, active_vp, &svp1, 
                                                    tpid_enable, vxlan_port);
       } else if (active_vp == vp2) {
           rv  = _bcm_td2_vxlan_eline_vp_configure (unit, vfi_index, active_vp, &svp2, 
                                                    tpid_enable, vxlan_port);
       }
       if (rv < 0) {
            goto vxlan_cleanup;
       }

        rv = _bcm_td2_vxlan_match_add(unit, vxlan_port, active_vp, vpn);
        if (rv < 0) {
            goto vxlan_cleanup;
        }

        /* Increment new-port's VP count */
        rv = _bcm_td2_vxlan_port_cnt_update(unit, vxlan_port->vxlan_port_id, active_vp, TRUE);
        if (rv < 0) {
            goto vxlan_cleanup;
        }

        /* Set VXLAN port ID */
        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, active_vp);
        }

        if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            if (old_tpid_enable) {
                for (i = 0; i < tpid_enable_length; i++) {
                    if (old_tpid_enable & (1 << i)) {
                        rv = _bcm_fb2_outer_tpid_entry_delete(unit, i);
                        if (rv < 0) {
                            goto vxlan_cleanup;
                        }
                    }
                }
            }
        }

vxlan_cleanup:
        if (rv < 0) {
            if (tpid_enable) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            }
            /* Free the VP's */
            if (vp_valid_flag) {
                if (vp1 != -1) {
                    (void) _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp1);
                }
                if (vp2 != -1) {
                    (void) _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp2);
                }
            }
        }

        return rv;
}

/*
 * Function:
 *     _bcm_td2_vxlan_elan_port_add
 * Purpose:
 *      Add VXLAN ELAN port to a VPN
 * Parameters:
 *   unit - (IN) Device Number
 *   vpn - (IN) VPN instance ID
 *   vxlan_port - (IN/OUT) vxlan port information (OUT : vxlan_port_id)
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_elan_port_add(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t *vxlan_port)
{
    int vp, num_vp, vfi=0;
    source_vp_entry_t svp;
    source_vp_2_entry_t svp2;
    uint32 old_tpid_enable = 0;
    uint32 tpid_enable = 0;
    int i, tpid_enable_length = 0;
    int tpid_index=-1;
    int drop=0, rv = BCM_E_PARAM;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;
    int network_group=0;
    uint8 vpn_type = 0;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 svp1[SOC_MAX_MEM_WORDS];
#endif

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        _BCM_VXLAN_VPN_GET(vfi, _BCM_VXLAN_VPN_TYPE_ELAN,  vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVxlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        vfi = _BCM_VXLAN_VFI_INVALID;
    }

    /*Valid vnid is provided only for tunnel based VPN*/
    BCM_IF_ERROR_RETURN(
            _bcm_td2_vxlan_vpn_is_tunnel_based_vnid(unit, vpn, &vpn_type));
    if ((vpn_type != _BCM_VXLAN_VPN_TYPE_TUNNEL_BASED_VNID) &&
        vxlan_port->vnid <= 0xffffff) {
        return BCM_E_PARAM;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeVxlan;
    if (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) {
        vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
    }
    if ((vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_SHARE) ||
        (vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
        vp_info.flags |= _BCM_VP_INFO_SHARED_PORT;
    }

   /* ======== Allocate/Get Virtual_Port =============== */
    if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
        if (BCM_GPORT_IS_NIV_PORT(vxlan_port->vxlan_port_id) ||
            BCM_GPORT_IS_EXTENDER_PORT(vxlan_port->vxlan_port_id)) {
            if (BCM_GPORT_IS_NIV_PORT(vxlan_port->vxlan_port_id)) {
               vp = BCM_GPORT_NIV_PORT_ID_GET((vxlan_port->vxlan_port_id));
            } else if (BCM_GPORT_IS_EXTENDER_PORT(vxlan_port->vxlan_port_id)) {
               vp = BCM_GPORT_EXTENDER_PORT_ID_GET((vxlan_port->vxlan_port_id));
            }
            rv = _bcm_td2_vxlan_access_niv_pe_set (unit, vp, vfi);
            if (BCM_SUCCESS(rv)) {
               /* Set VXLAN port ID */
               BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, vp);
               rv = _bcm_vp_used_set(unit, vp, vp_info);
            }
            return rv;
        } else if (BCM_GPORT_IS_TRUNK(vxlan_port->vxlan_port_id)) {
            bcm_trunk_member_t trunk_member;
            int                port_count = 0;
            bcm_trunk_t        tid = 0;
            int                tid_is_vp_lag = FALSE;
            tid = BCM_GPORT_TRUNK_GET(vxlan_port->vxlan_port_id);
            BCM_IF_ERROR_RETURN(
                _bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
            if (tid_is_vp_lag) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2_vp_lag_get(unit, tid, NULL, 1,
                                       &trunk_member, &port_count));
                if (BCM_GPORT_IS_NIV_PORT(trunk_member.gport) ||
                    BCM_GPORT_IS_EXTENDER_PORT(trunk_member.gport)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tid, &vp));
                    rv = _bcm_td2_vxlan_access_niv_pe_set(unit, vp, vfi);
                    if (BCM_SUCCESS(rv)) {
                       rv = _bcm_vp_used_set(unit, vp, vp_info);
                    }
                    return rv;
                }
            }
        }

        vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }

        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            return BCM_E_NOT_FOUND;
        }

        BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, &svp1));
        }
#endif
        if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
            /* SD-tag mode, save the old TPID enable bits */
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                old_tpid_enable = soc_SVP_ATTRS_1m_field32_get(unit, &svp1,
                                                               TPID_ENABLEf);
                tpid_enable_length = soc_mem_field_length(unit,
                                        SVP_ATTRS_1m, TPID_ENABLEf);
            } else
#endif
            {
                old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp,
                                                             TPID_ENABLEf);
                tpid_enable_length = soc_mem_field_length(unit,
                                        SOURCE_VPm, TPID_ENABLEf);
            }
        }

        BCM_IF_ERROR_RETURN(READ_SOURCE_VP_2m(unit, MEM_BLOCK_ANY, vp, &svp2));
        /* Decrement old-port's nexthop count */
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_port_nh_cnt_dec(unit, vp));
        /* Decrement old-port's VP count */
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_port_cnt_update(
                unit, vxlan_port->vxlan_port_id, vp, FALSE));
    } else if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID ) {
       if (!BCM_GPORT_IS_VXLAN_PORT(vxlan_port->vxlan_port_id)) {
            return (BCM_E_BADID);
        }

        vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }
        /* Vp index zero is reserved in function _bcm_virtual_init because of 
         * HW restriction. 
         */
        if (vp >= num_vp || vp < 1) {
            return BCM_E_BADID;
        }
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
            return BCM_E_EXISTS;
        }

        BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));

#if defined(BCM_TRIUMPH3_SUPPORT)
        /*
         * EGR_MPLS_VC_AND_SWAP_LABEL_TABLE and EGR_DVP_ATTRIBUTE_1 are overlaid
         * tables on some chips. If there is conflict between these two tables,
         * the entries for EGR_DVP_ATTRIBUTE_1 own higher priority.
         */
        if (soc_feature(unit, soc_feature_mpls) &&
            soc_feature(unit, soc_feature_vc_and_swap_table_overlaid)) {
            rv = _bcm_tr_mpls_vc_and_swap_table_entry_adjust(unit, vp);
            if (rv < 0) {
                (void)_bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp);
                return rv;
            }
        }
#endif

        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
#if defined(BCM_TRIDENT3_SUPPORT)
        sal_memset(svp1, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#endif
        sal_memset(&svp2, 0, sizeof(source_vp_2_entry_t));
    } else {
        /* allocate a new VP index */
        BCM_IF_ERROR_RETURN(_bcm_vp_alloc
            (unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp));
#if defined(BCM_TRIUMPH3_SUPPORT)
        /*
         * EGR_MPLS_VC_AND_SWAP_LABEL_TABLE and EGR_DVP_ATTRIBUTE_1 are overlaid
         * tables on some chips. If there is conflict between these two tables,
         * the entries for EGR_DVP_ATTRIBUTE_1 own higher priority.
         */
        if (soc_feature(unit, soc_feature_mpls) &&
            soc_feature(unit, soc_feature_vc_and_swap_table_overlaid)) {
            rv = _bcm_tr_mpls_vc_and_swap_table_entry_adjust(unit, vp);
            if (rv < 0) {
                (void)_bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp);
                return rv;
            }
        }
#endif
        BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, vp);

        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
#if defined(BCM_TRIDENT3_SUPPORT)
        sal_memset(svp1, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#endif
        sal_memset(&svp2, 0, sizeof(source_vp_2_entry_t));
    }

    /* ======== Configure Next-Hop Properties ========== */
    drop = (vxlan_port->flags & BCM_VXLAN_PORT_DROP) ? 1 : 0;
    rv = _bcm_td2_vxlan_port_nh_add(unit, vxlan_port, vp, vpn, drop);
    _BCM_VXLAN_CLEANUP(rv)

    /* ======== Configure Service-Tag Properties =========== */
    if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TAGGED) {
        rv = _bcm_fb2_outer_tpid_entry_add(unit,
                 vxlan_port->egress_service_tpid, &tpid_index);
        _BCM_VXLAN_CLEANUP(rv)

        tpid_enable = (1 << tpid_index);
        soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 1);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            soc_SVP_ATTRS_1m_field_set(unit, &svp1, TPID_ENABLEf, &tpid_enable);
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, &svp, TPID_ENABLEf, tpid_enable);
        }
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 0);
    }

    /* ======== Configure SVP/DVP Properties ========== */
    soc_SOURCE_VPm_field32_set(unit, &svp, CLASS_IDf, vxlan_port->if_class);
    soc_SOURCE_VPm_field32_set(unit, &svp, ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VFI);

    /* Default Split Horizon Group Id
     * 0 - For Access Port;1 - For Network Port*/
    network_group = vxlan_port->network_group_id;
    rv = _bcm_validate_splithorizon_network_group(unit,
            vxlan_port->flags & BCM_VXLAN_PORT_NETWORK, &network_group);
    BCM_IF_ERROR_RETURN(rv);
    if (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {  
            soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                        network_group);
        } else {
            if (vxlan_port->flags2 & BCM_VXLAN_PORT_FLAGS2_NO_SPLIT_HORIZON_SOURCE) {
                soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 0);
            } else {
                soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 1);
            }
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, &svp, TPID_SOURCEf, _BCM_VXLAN_TPID_SVP_BASED);
        }
    } else {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
         soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                 network_group);
        } else {
            soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 0);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, &svp, TPID_SOURCEf, _BCM_VXLAN_TPID_SGLP_BASED);
            /*  Configure IPARS Parser to signal to IMPLS Parser - Applicable only for HG2 PPD2 packets
                 to reconstruct TPID state based on HG header information - Only for VXLAN Access ports */
            soc_SOURCE_VP_2m_field32_set(unit, &svp2, PARSE_USING_SGLP_TPIDf, 1);
        }
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi);

    /* Keep CML in the replace operation. It may be set by bcm_td2_vxlan_port_learn_set before */
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
        rv = _bcm_vp_default_cml_mode_get (unit, &cml_default_enable,
                                    &cml_default_new, &cml_default_move);
        _BCM_VXLAN_CLEANUP(rv)

        if (cml_default_enable) {
            /* Set the CML to default values */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml_default_new);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, 0x8);
        }
    }
    if (soc_mem_field_valid(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
        if (!soc_feature(unit, soc_feature_vlan_vfi_membership)) {
            soc_SOURCE_VPm_field32_set(unit, &svp, DISABLE_VLAN_CHECKSf, 1);
        }
    }

    _BCM_VXLAN_CLEANUP(WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        _BCM_VXLAN_CLEANUP(WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ALL, vp, &svp1));
    }
#endif
    _BCM_VXLAN_CLEANUP(WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp2));

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) {
            if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit, SOURCE_VPm,
                vp, ENTRY_TYPEf, CANCUN_APP__SOURCE_VP__No_Control_Word,
                _BCM_VXLAN_SOURCE_VP_TYPE_VFI)) {
                LOG_ERROR(BSL_LS_BCM_VXLAN,
                   (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
            }
        }
    }
#endif

    /* Configure VP match criteria */
    rv = _bcm_td2_vxlan_match_add(unit, vxlan_port, vp, vpn);
    _BCM_VXLAN_CLEANUP(rv)

    /* Increment new-port's VP count */
    rv = _bcm_td2_vxlan_port_cnt_update(unit, vxlan_port->vxlan_port_id, vp, TRUE);
    _BCM_VXLAN_CLEANUP(rv)

    if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
        rv = _bcm_td2_vxlan_vplag_vp_update(unit, vxlan_port->vxlan_port_id);
        _BCM_VXLAN_CLEANUP(rv)

        if (old_tpid_enable) {
            for (i = 0; i < tpid_enable_length; i++) {
                if (old_tpid_enable & (1 << i)) {
                    rv = _bcm_fb2_outer_tpid_entry_delete(unit, i);
                    _BCM_VXLAN_CLEANUP(rv)
                }
            }
        }
    }

    return BCM_E_NONE;

cleanup:
    if (tpid_enable) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
        (void) _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp);
        _bcm_td2_vxlan_port_nh_delete(unit, vpn, vp);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_eline_port_delete
 * Purpose:
 *      Delete VXLAN ELINE port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      vxlan_port_id - (IN) vxlan port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_eline_port_delete(int unit, bcm_vpn_t vpn, int active_vp)
{
    source_vp_entry_t svp;
    source_vp_2_entry_t svp_2_entry;
    int network_port_flag=0;
    int vp1=0, vp2=0, vfi_index= -1;
    int rv = BCM_E_UNAVAIL;
    bcm_gport_t  vxlan_port_id;
    bcm_vxlan_port_t vxlan_port;
    _bcm_vp_info_t vp_info;
    uint32 old_tpid_enable = 0;
    int i, tpid_enable_length = 0;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 svp1[SOC_MAX_MEM_WORDS];
#endif

    if ( vpn != BCM_VXLAN_VPN_INVALID) {
         _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VXLAN_VPN_TYPE_ELINE,  vpn);
         if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVxlan)) {
            return BCM_E_NOT_FOUND;
         }
    } else {
         vfi_index = _BCM_VXLAN_VFI_INVALID;
    }

    if (!_bcm_vp_used_get(unit, active_vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }

    /* Set VXLAN port ID */
    BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port_id, active_vp);

    /* Delete the next-hop info */
    rv = _bcm_td2_vxlan_port_nh_delete(unit, vpn, active_vp);
    if ( rv < 0 ) {
        if (rv != BCM_E_NOT_FOUND) {
            return rv;
        } else {
                rv = 0;
        }
    }

    /* ---- Read in current table values for VP1 and VP2 ----- */
    (void) _bcm_td2_vxlan_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.vxlan_port_id = vxlan_port_id;
    BCM_IF_ERROR_RETURN(bcm_td2_vxlan_port_get(unit, vpn, &vxlan_port));
    rv = _bcm_td2_vxlan_match_delete(unit, active_vp, vxlan_port, vpn);
    if ( rv < 0 ) {
        if (rv != BCM_E_NOT_FOUND) {
             return rv;
        } else {
             rv = BCM_E_NONE;
        }
    }

    /* If the other port is valid, point it to itself */
    if (active_vp == vp1) {
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_td2_vxlan_eline_vp_map_clear (unit, vfi_index, active_vp, 0);
        }
    } else if (active_vp == vp2) {
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_td2_vxlan_eline_vp_map_clear (unit, vfi_index, 0, active_vp);
        }
    }

    if (rv >= 0) {

        /* Check for Network-Port */
        BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, active_vp, &vp_info));
        if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
            network_port_flag = TRUE;
        }

        if (!network_port_flag) {
            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            BCM_IF_ERROR_RETURN
                (WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, active_vp, &svp_2_entry));
        }

        BCM_IF_ERROR_RETURN
            (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, active_vp, &svp));

        if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit,
                                    MEM_BLOCK_ANY, active_vp, &svp1));
                old_tpid_enable = soc_SVP_ATTRS_1m_field32_get(unit, &svp1,
                                                               TPID_ENABLEf);
                tpid_enable_length = soc_mem_field_length(unit,
                                         SVP_ATTRS_1m, TPID_ENABLEf);
            } else
#endif
            {
                old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp,
                                                             TPID_ENABLEf);
                tpid_enable_length = soc_mem_field_length(unit,
                                         SOURCE_VPm, TPID_ENABLEf);
            }

            if (old_tpid_enable) {
                for (i = 0; i < tpid_enable_length; i++) {
                    if (old_tpid_enable & (1 << i)) {
                        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit, i));
                    }
                }
            }
        }

        /* Invalidate the VP being deleted */
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, &svp);
        if (BCM_SUCCESS(rv)) {
             rv = _bcm_td2_vxlan_egress_dvp_reset(unit, active_vp);
             if (rv < 0) {
                 return rv;
             }
             rv = _bcm_td2_vxlan_ingress_dvp_reset(unit, active_vp);
        }
    }

    if (rv < 0) {
        return rv;
    }

    /* Decrement port's VP count */
    rv = _bcm_td2_vxlan_port_cnt_update(unit, vxlan_port_id, active_vp, FALSE);
    if (rv < 0) {
        return rv;
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, active_vp);
    return rv;

}

/*
 * Function:
 *      _bcm_td2_vxlan_elan_port_delete
 * Purpose:
 *      Delete VXLAN ELAN port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      vxlan_port_id - (IN) vxlan port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_elan_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    source_vp_entry_t svp;
    source_vp_2_entry_t svp_2_entry;
    int network_port_flag=0;
    int rv = BCM_E_NONE;
    int vfi_index= -1;
    bcm_gport_t vxlan_port_id;
    bcm_vxlan_port_t  vxlan_port;
    _bcm_vp_info_t vp_info;
    int i, tpid_enable_length = 0;
    uint32 old_tpid_enable = 0;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 svp1[SOC_MAX_MEM_WORDS];
#endif

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }

    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        network_port_flag = TRUE;
    }

    /* Set VXLAN port ID */
    BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port_id, vp);
    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.vxlan_port_id = vxlan_port_id;
    BCM_IF_ERROR_RETURN(bcm_td2_vxlan_port_get(unit, vpn, &vxlan_port));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if ((!network_port_flag) && (vp_info.flags & _BCM_VP_INFO_SHARED_PORT)) {
        if (VXLAN_INFO(unit)->match_key[vp].match_count > 0) {
            BCM_IF_ERROR_RETURN(bcm_td2p_share_vp_delete(unit, vpn, vp));
        }
        if (vpn != BCM_VXLAN_VPN_INVALID) {
            return BCM_E_NONE;
        }
    } else
#endif
    {
        rv = _bcm_td2_vxlan_match_delete(unit, vp, vxlan_port, vpn);
        if (rv < 0 && rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }

    (void)bcm_td2_vxlan_match_clear(unit, vp);

    if ( vpn != BCM_VXLAN_VPN_INVALID) {
        if (!network_port_flag) { /* Check VPN only for Access VP */
            _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VXLAN_VPN_TYPE_ELAN,  vpn);
            if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVxlan)) {
                return BCM_E_NOT_FOUND;
            }
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) ||
                _bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                (void) _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp);
                return _bcm_td2_vxlan_access_niv_pe_reset (unit, vp);
            }
        }
    }

    /* Decrement port's VP count */
    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_port_cnt_update(unit, vxlan_port_id, vp, FALSE));

    /* Delete the next-hop info */
    rv = _bcm_td2_vxlan_port_nh_delete(unit, vpn, vp);
    if (rv < 0 && rv != BCM_E_NOT_FOUND) {
        return rv;
    }

    /* Clear the SVP and DVP table entries */
    BCM_IF_ERROR_RETURN
        (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

    if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit,
                                MEM_BLOCK_ANY, vp, &svp1));
            old_tpid_enable = soc_SVP_ATTRS_1m_field32_get(unit, &svp1,
                                                           TPID_ENABLEf);
            tpid_enable_length = soc_mem_field_length(unit,
                                     SVP_ATTRS_1m, TPID_ENABLEf);
        } else
#endif
        {
            old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp,
                                                         TPID_ENABLEf);
            tpid_enable_length = soc_mem_field_length(unit,
                                     SOURCE_VPm, TPID_ENABLEf);
        }

        if (old_tpid_enable) {
            for (i = 0; i < tpid_enable_length; i++) {
                if (old_tpid_enable & (1 << i)) {
                    BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit, i));
                }
            }
        }
    }

    /* Clear the SVP table entries */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    BCM_IF_ERROR_RETURN(WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp));

    if (!network_port_flag) {
        sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
        BCM_IF_ERROR_RETURN
            (WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry));
    }

    /* Clear the DVP table entries */
    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_egress_dvp_reset(unit, vp));
    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_ingress_dvp_reset(unit, vp));

    /* Free the VP */
    BCM_IF_ERROR_RETURN(_bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_port_vlan_get
 * Purpose:
 *      Obtain vlan information
 * Parameters:
 *      unit           - (IN)   Device Number
 *      vp             - (IN)   Source Virtual Port
 *      vxlan_port     - (OUT)  VXLAN port information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_port_vlan_get(int unit, int vp, bcm_vxlan_port_t *vxlan_port)
{
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);
    int index = 0;
    soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS];

    sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        mem = VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else
#endif
    {
        mem = VLAN_XLATEm;

        soc_mem_field32_set(unit, mem, vent, VALIDf, 1);
    }

    if (_BCM_VXLAN_PORT_MATCH_TYPE_VLAN == vxlan_info->match_key[vp].flags) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                    vxlan_info->match_key[vp].match_vlan);
    } else if (_BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN ==
        vxlan_info->match_key[vp].flags) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                    vxlan_info->match_key[vp].match_inner_vlan);
    } else if (_BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED ==
        vxlan_info->match_key[vp].flags) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent,
            KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent,
            XLATE__OVIDf, vxlan_info->match_key[vp].match_vlan);
        soc_mem_field32_set(unit, mem, vent,
            XLATE__IVIDf, vxlan_info->match_key[vp].match_inner_vlan);
    } else if (_BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI ==
        vxlan_info->match_key[vp].flags) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent,
            KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent,
                DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
        }
#endif
        soc_mem_field32_set(unit, mem, vent,
            XLATE__OTAGf, vxlan_info->match_key[vp].match_vlan);
    } else {
        return BCM_E_NONE;
    }

    if (vxlan_info->match_key[vp].modid != -1) {
        soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                    vxlan_info->match_key[vp].modid);
        soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                    vxlan_info->match_key[vp].port);
    } else {
        soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
        soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                    vxlan_info->match_key[vp].trunk_id);
    }

    BCM_IF_ERROR_RETURN(soc_mem_search(unit,
        mem, MEM_BLOCK_ANY, &index, vent, vent, 0));
    if (!soc_mem_field32_get(unit, mem, vent, XLATE__VLAN_ACTION_VALIDf)) {
        vxlan_port->flags |= BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_port_get
 * Purpose:
 *      Get VXLAN port information from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      vxlan_port_id - (IN) vxlan port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_port_get(int unit, bcm_vpn_t vpn, int vp, bcm_vxlan_port_t  *vxlan_port)
{
    int i, rv = BCM_E_NONE;
    uint32 tpid_enable = 0;
    source_vp_entry_t svp;
    int split_horizon_port_flag=0;
    _bcm_vp_info_t vp_info;
    int network_port = 0;

    /* Initialize the structure */
    bcm_vxlan_port_t_init(vxlan_port);
    BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, vp);
    BCM_IF_ERROR_RETURN (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        split_horizon_port_flag = TRUE;
    }

    if ( vpn != BCM_VXLAN_VPN_INVALID) {
         if (!split_horizon_port_flag) { /* Check VPN only for Access VP */
             if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) ||
                  _bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                   return BCM_E_NONE;
             }
         }
    }

    if (split_horizon_port_flag) {
        vxlan_port->flags |= BCM_VXLAN_PORT_NETWORK;
        vxlan_port->flags |= BCM_VXLAN_PORT_EGRESS_TUNNEL;
        if(vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
            vxlan_port->flags |= BCM_VXLAN_PORT_SHARE;
            /* Shared VXLAN port is used only for multicast now*/
            vxlan_port->flags |= BCM_VXLAN_PORT_MULTICAST;
        }
    }

    /* Get the match parameters */
    rv = _bcm_td2_vxlan_match_get(unit, vxlan_port, vp, vpn);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS flag */
    if (!soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN(
            _bcm_td2_vxlan_port_vlan_get(unit, vp, vxlan_port));
    }

    /* Get the next-hop parameters , for shared VXLAN port, don't get next hop here*/
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
        rv = _bcm_td2_vxlan_port_nh_get(unit, vpn, vp, vxlan_port);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = _bcm_td2_vxlan_ingress_dvp_get(unit, vp, vxlan_port);
    BCM_IF_ERROR_RETURN(rv);

    /* Get Tunnel index */
    rv = _bcm_td2_vxlan_egress_dvp_get(unit, vp, vxlan_port);
    BCM_IF_ERROR_RETURN(rv);

    /* Fill in SVP parameters */
    vxlan_port->if_class = soc_SOURCE_VPm_field32_get(unit, &svp, CLASS_IDf);

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
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_TAGGED;
            for (i = 0; i < 4; i++) {
                if (tpid_enable & (1 << i)) {
                    _bcm_fb2_outer_tpid_entry_get(unit, &vxlan_port->egress_service_tpid, i);
                }
            }
        }
    }

    if (soc_mem_field_valid(unit, SOURCE_VPm, NETWORK_PORTf) &&
        split_horizon_port_flag) {
        network_port = soc_SOURCE_VPm_field32_get(unit, &svp, NETWORK_PORTf);
        if (network_port == 0) {
            vxlan_port->flags2 |= BCM_VXLAN_PORT_FLAGS2_NO_SPLIT_HORIZON_SOURCE;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_add
 * Purpose:
 *      Add VXLAN port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      vxlan_port - (IN/OUT) vxlan_port information (OUT : vxlan_port_id)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_vxlan_default_port_add(int unit, bcm_vxlan_port_t  *vxlan_port)
{
    source_vp_entry_t svp;
    int rv = BCM_E_PARAM, vp=0, num_vp=0;
    int network_group=0;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeVxlan;
    if(vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) { 
        vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

   /* ======== Allocate/Get Virtual_Port =============== */
    if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }

        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            return BCM_E_NOT_FOUND;
        }
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            return rv;
        }

        /* Decrement old-port's nexthop count */
        rv = _bcm_td2_vxlan_port_nh_cnt_dec(unit, vp);
        if (rv < 0) {
            return rv;
        }

        /* Decrement old-port's VP count */
        rv = _bcm_td2_vxlan_port_cnt_update(unit, vxlan_port->vxlan_port_id, vp, FALSE);
        if (rv < 0) {
            return rv;
        }

    } else if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID ) {
       if (!BCM_GPORT_IS_VXLAN_PORT(vxlan_port->vxlan_port_id)) {
            return (BCM_E_BADID);
        }

        vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }
        /* Vp index zero is reserved in function _bcm_virtual_init because of 
         * HW restriction. 
         */
        if (vp >= num_vp || vp < 1) {
            return BCM_E_BADID;
        } 
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) { 
            return BCM_E_EXISTS;
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
                               vxlan_port->if_class);
    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        network_group = vxlan_port->network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
                vxlan_port->flags & BCM_VXLAN_PORT_NETWORK, &network_group);
        BCM_IF_ERROR_RETURN(rv);
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                network_group);
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 1);
    }

    soc_SOURCE_VPm_field32_set(unit, &svp, 
                                      ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VFI);

    /* Keep CML in the replace operation. It may be set by bcm_td2_vxlan_port_learn_set before */
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
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
    }
    if (soc_mem_field_valid(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
        if (!soc_feature(unit, soc_feature_vlan_vfi_membership)) {
            soc_SOURCE_VPm_field32_set(unit, &svp, DISABLE_VLAN_CHECKSf, 1);
        }
    }

    /* Configure VXLAN_DEFAULT_NETWORK_SVPr.SVP */
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit, SOURCE_VPm,
            vp, ENTRY_TYPEf, CANCUN_APP__SOURCE_VP__No_Control_Word,
            _BCM_VXLAN_SOURCE_VP_TYPE_VFI)) {
            LOG_ERROR(BSL_LS_BCM_VXLAN,
               (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
        }
    }
#endif

    BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, vp);

    /* Increment new-port's VP count */
    rv = _bcm_td2_vxlan_port_cnt_update(unit, vxlan_port->vxlan_port_id, vp, TRUE);
    BCM_IF_ERROR_RETURN(rv);

    return (soc_reg_field32_modify(unit, VXLAN_DEFAULT_NETWORK_SVPr, 
             REG_PORT_ANY, SVPf, vp));

}

/*
 * Function:
 *      _bcm_td2_vxlan_default_port_delete
 * Purpose:
 *      Delete VXLAN Default port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      vxlan_port_id - (IN) vxlan port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_vxlan_default_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    source_vp_entry_t svp;
    int rv = BCM_E_UNAVAIL;
    int vfi_index= -1;
    bcm_gport_t vxlan_port_id;

    if ( vpn != BCM_VXLAN_VPN_INVALID) {
         _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VXLAN_VPN_TYPE_ELAN,  vpn);
         if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVxlan)) {
            return BCM_E_NOT_FOUND;
         }
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }

    /* Set VXLAN port ID */
    BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port_id, vp);

    /* Clear the SVP and DVP table entries */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    if (rv < 0) {
        return rv;
    }

    /* Decrement port's VP count */
    rv = _bcm_td2_vxlan_port_cnt_update(unit, vxlan_port_id, vp, FALSE);
    if (rv < 0) {
        return rv;
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp);

    /* Reset VXLAN_DEFAULT_NETWORK_SVP */
    BCM_IF_ERROR_RETURN(soc_reg_field32_modify
        (unit, VXLAN_DEFAULT_NETWORK_SVPr, REG_PORT_ANY, SVPf, 0));

    return rv;

}

/*
 * Function:
 *      _bcm_td2_vxlan_default_port_get
 * Purpose:
 *      Get VXLAN Default port information
 * Parameters:
 *      unit       - (IN) Device Number
 *      vp         - (IN) Source Virtual Port
 *      vxlan_port - (IN/OUT) vxlan port information
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_vxlan_default_port_get(int unit, int vp, bcm_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_NONE;
    source_vp_entry_t svp;
    _bcm_vp_info_t vp_info;

    bcm_vxlan_port_t_init(vxlan_port);
    BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, vp);

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
    vxlan_port->if_class = soc_SOURCE_VPm_field32_get(unit, &svp, CLASS_IDf);

    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        vxlan_port->flags |= BCM_VXLAN_PORT_NETWORK;
    }

    vxlan_port->flags |= BCM_VXLAN_PORT_DEFAULT;

    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_add
 * Purpose:
 *      Add VXLAN port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      vxlan_port - (IN/OUT) vxlan_port information (OUT : vxlan_port_id)
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2_vxlan_port_add(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t  *vxlan_port)
{
    int mode=0, rv = BCM_E_PARAM;
    uint8 isEline = 0;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if (vxlan_port->if_class > 0xfff) {
        return BCM_E_PARAM;
    }

    if (vxlan_port->mtu > 0x3fff) {
        return BCM_E_PARAM;
    }

    if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_SHARE) {
        if (soc_feature(unit, soc_feature_vp_sharing)) {
            if (vpn != BCM_VXLAN_VPN_INVALID) {
                return BCM_E_PARAM;
            }
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_SHARE) {
        if (!((vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) &&
                (vxlan_port->flags & BCM_VXLAN_PORT_MULTICAST))) {
            return BCM_E_PARAM;
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if ((vxlan_port->flags & BCM_VXLAN_PORT_DEFAULT) &&
              !SOC_REG_IS_VALID(unit, VXLAN_DEFAULT_NETWORK_SVPr)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_DEFAULT) {
        rv = _bcm_td2_vxlan_default_port_add(unit, vxlan_port);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (BCM_SUCCESS(rv) && SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
            rv = soc_reg_field32_modify
                  (unit, DEFAULT_SVP_ENABLEr, REG_PORT_ANY,
                                VXLAN_DEFAULT_SVP_ENABLEf, 1);
            /* this register is removed in cch after cancun 5.1.8. It is
             * no longer needed. The cancun access routine will return
             * not found if it doesn't exist in cch. Simply ignore
             * this error.
             */
            if (rv == SOC_E_NOT_FOUND) {
                rv = SOC_E_NONE;
            }
        }
#endif
        return rv;
    }

    if (isEline) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_VPN_BASED) {
            /* not support for this flag */
            return BCM_E_PARAM;
        }
        rv = _bcm_td2_vxlan_eline_port_add(unit, vpn, vxlan_port);
    } else {
        if (vxlan_port->flags & BCM_VXLAN_PORT_VPN_BASED) {
            /* this flag is reused to indicate adding VP-based VFI-VIND mapping */
            rv = _bcm_td2_vxlan_vp_based_vnid_add(unit, vpn, vxlan_port);
        } else {
        rv = _bcm_td2_vxlan_elan_port_add(unit, vpn, vxlan_port);
    }
    }

    return rv;
}


/*
 * Function:
 *      bcm_td2_vxlan_port_delete
 * Purpose:
 *      Delete VXLAN port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      vxlan_port_id - (IN) vxlan port information
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2_vxlan_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t vxlan_port_id)
{
    int vp=0;
    int rv = BCM_E_UNAVAIL;
    uint32 reg_val=0;
    uint8 isEline=0;
    uint32 stat_counter_id;
    int num_ctr = 0;

    if (BCM_GPORT_IS_TRUNK(vxlan_port_id)) {
        bcm_trunk_member_t trunk_member;
        int                port_count = 0;
        bcm_trunk_t        tid = 0;
        int                tid_is_vp_lag = FALSE;
        if (vpn != BCM_VXLAN_VPN_INVALID) {
            BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
        }
        tid = BCM_GPORT_TRUNK_GET(vxlan_port_id);
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
        if (tid_is_vp_lag) {
            BCM_IF_ERROR_RETURN(
                bcm_td2_vp_lag_get(unit, tid, NULL, 1,
                                   &trunk_member, &port_count));
            if (BCM_GPORT_IS_NIV_PORT(trunk_member.gport) ||
                BCM_GPORT_IS_EXTENDER_PORT(trunk_member.gport)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tid, &vp));
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                    return BCM_E_NOT_FOUND;
                }
                BCM_IF_ERROR_RETURN(_bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp));
                return _bcm_td2_vxlan_access_niv_pe_reset(unit, vp);
            }
        }
    }

    /* Check for Valid Vpn */
    if (vpn != BCM_VXLAN_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
    }

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vp_is_eline(unit, vp, &isEline));

    /* Check for VXLAN default port */
    if (SOC_REG_IS_VALID(unit, VXLAN_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_VXLAN_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        if (vp == soc_reg_field_get(unit,
                    VXLAN_DEFAULT_NETWORK_SVPr, reg_val, SVPf)) {
            rv =  _bcm_td2_vxlan_default_port_delete(unit, vpn, vp);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (BCM_SUCCESS(rv) && SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
                rv = soc_reg_field32_modify
                      (unit, DEFAULT_SVP_ENABLEr, REG_PORT_ANY, VXLAN_DEFAULT_SVP_ENABLEf, 0);
                if (rv == SOC_E_NOT_FOUND) {
                    rv = SOC_E_NONE;
                }
            }
#endif
            return rv;
        }
    }
    
    if (isEline == 0x1 ) {
       rv = _bcm_td2_vxlan_eline_port_delete(unit, vpn, vp);
    } else if (isEline == 0x0 ) {
        uint32 count = 0;
        source_vp_entry_t svp;
        int vfi = 0;
        int vfi_r;
        _bcm_vp_info_t vp_info;

        BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));

        if ((vp_info.flags & _BCM_VP_INFO_NETWORK_PORT)) {
            rv = _bcm_td2_vxlan_vp_based_vnid_count(unit, vp, &count);

            /*
             * if no VFI-VNID mappings attached, then delete vxlan port directly,
             * this is to backward-compatible with previous API behavior.
 */
            if (count != 0) {
                if (vpn == BCM_VXLAN_VPN_INVALID) {
                    return BCM_E_BUSY;
                }

                BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
                vfi_r = soc_SOURCE_VPm_field32_get(unit, &svp, VFIf);
                _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);

                if (vfi != vfi_r) {
                    /* user is trying to delete VFI-VNID mappings */
                    bcm_vxlan_port_t vxlan_port_r;

                    bcm_vxlan_port_t_init(&vxlan_port_r);
                    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_port_get(unit, vpn, vp, &vxlan_port_r));
                    rv = _bcm_td2_vxlan_vp_based_vnid_delete(unit, vpn, &vxlan_port_r);
                    return rv;
                } else {
                    return BCM_E_BUSY;
                }
            }
        }

       rv = _bcm_td2_vxlan_elan_port_delete(unit, vpn, vp);
    }

    if (rv != BCM_E_NONE) {
        return rv;
    }

    if (bcm_esw_vxlan_stat_id_get(unit, vxlan_port_id, BCM_VXLAN_VPN_INVALID,
                   bcmVxlanOutPackets, &stat_counter_id) == BCM_E_NONE) {
        num_ctr++;
    }
    if (bcm_esw_vxlan_stat_id_get(unit, vxlan_port_id, BCM_VXLAN_VPN_INVALID,
                   bcmVxlanInPackets, &stat_counter_id) == BCM_E_NONE) {
        num_ctr++;
    }

    if (num_ctr != 0) {
        BCM_IF_ERROR_RETURN(
           bcm_esw_vxlan_stat_detach(unit, vxlan_port_id, BCM_VXLAN_VPN_INVALID));
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_delete_all
 * Purpose:
 *      Delete all VXLAN ports from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vxlan_port_delete_all(int unit, bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    int vfi_index=0;
    int vp1 = 0, vp2 = 0;
    uint8 isEline = 0;
    bcm_gport_t vxlan_port_id;
    uint32 reg_val=0;
    uint32 vp=0;

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    } else {
        vfi_index = _BCM_VXLAN_VFI_INVALID;
    }

    if (isEline == 0x1 ) {
         /* ---- Read in current table values for VP1 and VP2 ----- */
         (void) _bcm_td2_vxlan_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);
         if (vp1 != 0) {
              rv = _bcm_td2_vxlan_eline_port_delete(unit, vpn, vp1);
              BCM_IF_ERROR_RETURN(rv);
         }
         if (vp2 != 0) {
              rv = _bcm_td2_vxlan_eline_port_delete(unit, vpn, vp2);
              BCM_IF_ERROR_RETURN(rv);
         }
    } else if (isEline == 0x0 ) {
        uint32 num_vp = 0, entry_type = 0;
        source_vp_entry_t svp;
        if (SOC_REG_IS_VALID(unit, VXLAN_DEFAULT_NETWORK_SVPr)) {
            BCM_IF_ERROR_RETURN(READ_VXLAN_DEFAULT_NETWORK_SVPr(unit, &reg_val));
            vp = soc_reg_field_get(unit, VXLAN_DEFAULT_NETWORK_SVPr, reg_val, SVPf);
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_default_port_delete(unit, vpn, vp));
#if defined(BCM_TRIDENT3_SUPPORT)
                if (SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
                    rv = soc_reg_field32_modify (unit, DEFAULT_SVP_ENABLEr,
                             REG_PORT_ANY, VXLAN_DEFAULT_SVP_ENABLEf, 0);
                    if (SOC_FAILURE(rv)) {
                        if (rv == SOC_E_NOT_FOUND) {
                            rv = SOC_E_NONE;
                        } else {
                            return rv;
                        }
                    }
                }
#endif
            }
        }
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        SHR_BIT_ITER(VIRTUAL_INFO(unit)->vxlan_vp_bitmap, num_vp, vp) {

            BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
            entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);
            if ((vfi_index == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf) &&
                    entry_type == _BCM_VXLAN_SOURCE_VP_TYPE_VFI)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                    || SHR_BITGET(VIRTUAL_INFO(unit)->vp_shared_vp_bitmap, vp)
#endif
                    ) {
                if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVpLag)) {
                    bcm_trunk_t        tid = 0;
                    bcm_trunk_member_t trunk_member;
                    int                port_count = 0;
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_trunk_vp_lag_vp_to_tid(unit, vp, &tid));
                    BCM_IF_ERROR_RETURN(
                        bcm_td2_vp_lag_get(unit, tid, NULL, 1,
                                           &trunk_member, &port_count));
                    if (BCM_GPORT_IS_NIV_PORT(trunk_member.gport) ||
                        BCM_GPORT_IS_EXTENDER_PORT(trunk_member.gport)) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp));
                        BCM_IF_ERROR_RETURN(
                            _bcm_td2_vxlan_access_niv_pe_reset(unit, vp));
                        continue;
                    }
                }
                   /* Set VXLAN port ID */
                BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port_id, vp);
                BCM_IF_ERROR_RETURN(bcm_td2_vxlan_port_delete(unit, vpn, vxlan_port_id));
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_get
 * Purpose:
 *      Get an VXLAN port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      vxlan_port  - (IN/OUT) VXLAN port information
 */
int
bcm_td2_vxlan_port_get(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t *vxlan_port)
{
    int vp=0;
    int rv = BCM_E_NONE;
    uint32 reg_val = 0;
    int default_vp = -1;
    if (BCM_GPORT_IS_TRUNK(vxlan_port->vxlan_port_id)) {
        bcm_trunk_member_t trunk_member;
        int                port_count = 0;
        bcm_trunk_t        tid = 0;
        int                tid_is_vp_lag = FALSE;
        if (vpn != BCM_VXLAN_VPN_INVALID) {
            BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
        }
        tid = BCM_GPORT_TRUNK_GET(vxlan_port->vxlan_port_id);
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
        if (tid_is_vp_lag) {
            BCM_IF_ERROR_RETURN(
                bcm_td2_vp_lag_get(unit, tid, NULL, 1,
                                   &trunk_member, &port_count));
            if (BCM_GPORT_IS_NIV_PORT(trunk_member.gport) ||
                BCM_GPORT_IS_EXTENDER_PORT(trunk_member.gport)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tid, &vp));
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                    return BCM_E_NOT_FOUND;
                }
                return BCM_E_NONE;
            }
        }
    }

    if (SOC_REG_IS_VALID(unit, VXLAN_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_VXLAN_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        default_vp = soc_reg_field_get(unit,
                            VXLAN_DEFAULT_NETWORK_SVPr, reg_val, SVPf);
    }

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
    }

    if (vp == default_vp) {
        rv = _bcm_td2_vxlan_default_port_get(unit, vp, vxlan_port);
    } else {
        if (vxlan_port->flags & BCM_VXLAN_PORT_VPN_BASED) {
            rv = _bcm_td2_vxlan_vp_based_vnid_get(unit, vpn, vxlan_port);
        } else {
        rv = _bcm_td2_vxlan_port_get(unit, vpn, vp, vxlan_port);
    }
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_get_all
 * Purpose:
 *      Get all VXLAN ports from a VPN
 * Parameters:
 *      unit     - (IN) Device Number
 *      vpn      - (IN) VPN instance ID
 *      port_max   - (IN) Maximum number of interfaces in array
 *      port_array - (OUT) Array of VXLAN ports
 *      port_count - (OUT) Number of interfaces returned in array
 */
int
bcm_td2_vxlan_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                         bcm_vxlan_port_t *port_array, int *port_count)
{
    int vp, rv = BCM_E_NONE;
    int vfi_index=-1;
    int vp1 = 0, vp2 = 0;
    uint8 isEline = 0;
    SHR_BITDCL *share_vp_bitmap = NULL;

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_eline(unit, vpn, &isEline));
    } else {
        vfi_index = _BCM_VXLAN_VFI_INVALID;
    }

    *port_count = 0;

    if (isEline == 0x1 ) {
        /* ---- Read in current table values for VP1 and VP2 ----- */
        (void) _bcm_td2_vxlan_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);
        vp = vp1;
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            if (0 == port_max) {
                (*port_count)++;

            } else if (*port_count < port_max) {
                rv = _bcm_td2_vxlan_port_get(unit, vpn, vp, 
                                           &port_array[*port_count]);
                if (rv < 0) {
                    return rv;
                }
                (*port_count)++;
            }
        }

        vp = vp2;
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            if (0 == port_max) {
                (*port_count)++;

            } else if (*port_count < port_max) {
                rv = _bcm_td2_vxlan_port_get(unit, vpn, vp, 
                                           &port_array[*port_count]);
                if (rv < 0) {
                    return rv;
                }
                (*port_count)++;
            }
        }
    } else if (isEline == 0x0 ) {
        uint32 entry_type = 0;
        source_vp_entry_t svp;
        int num_vp = soc_mem_index_count(unit, SOURCE_VPm);

        share_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "sharr_vp_bitmap");
        if (share_vp_bitmap == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(share_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vp_sharing)) {
            rv = bcm_td2p_share_vp_get(unit, vpn, share_vp_bitmap);
            _BCM_VXLAN_CLEANUP(rv)
        }
#endif

        SHR_BIT_ITER(VIRTUAL_INFO(unit)->vxlan_vp_bitmap, num_vp, vp) {

            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
            _BCM_VXLAN_CLEANUP(rv)
            entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);

            if ((vfi_index == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf) &&
                    entry_type == _BCM_VXLAN_SOURCE_VP_TYPE_VFI)
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

               rv = _bcm_td2_vxlan_port_get(unit,
                        vpn, vp, &port_array[*port_count]);
               _BCM_VXLAN_CLEANUP(rv)

               (*port_count)++;
            }
        }
        sal_free(share_vp_bitmap);
    }

    return BCM_E_NONE;
cleanup:
    sal_free(share_vp_bitmap);
    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_learn_set
 * Purpose:
 *      Set the CML bits for a vxlan port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vxlan_port_learn_set(int unit, bcm_gport_t vxlan_port_id, 
                                  uint32 flags)
{
    int vp, cml = 0, rv = BCM_E_NONE, entry_type;
    source_vp_entry_t svp;

    rv = _bcm_vxlan_check_init(unit);

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
    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port_id);

    MEM_LOCK(unit, SOURCE_VPm);
    /* Be sure the entry is used and is set for VxLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    /* coverity[negative_returns] */
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return rv;
    }

    /* Ensure that the entry_type is 1 for VxLAN*/
    entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);
    if (entry_type != 1){
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }

    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml);
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml);
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    MEM_UNLOCK(unit, SOURCE_VPm);
    return rv;
}

/*
 * Function:    
 *      bcm_td2_vxlan_port_learn_get
 * Purpose:
 *      Get the CML bits for a VxLAN port
 * Returns: 
 *      BCM_E_XXX
 */     
int     
bcm_td2_vxlan_port_learn_get(int unit, bcm_gport_t vxlan_port_id,
                                  uint32 *flags)
{
    int rv, vp, cml = 0, entry_type;
    source_vp_entry_t svp;
    
    rv = _bcm_vxlan_check_init(unit);

    if (rv != BCM_E_NONE){
        return rv;
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port_id);
    if (vp == -1) {
       return BCM_E_PARAM;
    }

    /* Be sure the entry is used and is set for VxLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        return rv;
    }


    /* Ensure that the entry_type is 1 for VxLAN*/
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
 *      bcm_td2_vxlan_trunk_member_add
 * Purpose:
 *      Activate Trunk Port-membership from 
 *      EGR_PORT_TO_NHI_MAPPING
 * Parameters:
 *     unit  - (IN) Device Number
 *     tgid - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be added
 *     trunk_member_array - (IN) Trunk member gports to be added
*/
int
bcm_td2_vxlan_trunk_member_add(int unit, bcm_trunk_t trunk_id, 
                int trunk_member_count, bcm_port_t *trunk_member_array) 
{ 
    int idx = 0;
    int rv = BCM_E_NONE;
    uint32 reg_val = 0;
    int nh_index = 0, old_nh_index = -1;
    bcm_l3_egress_t egr;
    int gport = 0;
    _bcm_port_info_t *port_info;
    int local_member = 0;
    uint32 port_flags;

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
    BCM_IF_ERROR_RETURN(bcm_td2_vxlan_egress_get(unit, &egr, nh_index));

    if (egr.flags & BCM_L3_VXLAN_ONLY) {
        for (idx = 0; idx < trunk_member_count; idx++) {
            rv = bcm_esw_port_local_get(unit, trunk_member_array[idx], &local_member);
            if (SOC_REG_IS_VALID(unit, EGR_PORT_TO_NHI_MAPPINGr)) {
                /* Only support trunk local ports */
                if(BCM_FAILURE(rv)) {
                    continue; 
                }
                BCM_IF_ERROR_RETURN(
                        READ_EGR_PORT_TO_NHI_MAPPINGr(unit, local_member, &reg_val));
                old_nh_index = soc_reg_field_get(unit, EGR_PORT_TO_NHI_MAPPINGr, reg_val, NEXT_HOP_INDEXf);
                if (!old_nh_index) {
                    /* Set EGR_PORT_TO_NHI_MAPPING */
                    BCM_IF_ERROR_RETURN(
                        soc_reg_field32_modify(unit, EGR_PORT_TO_NHI_MAPPINGr,
                            local_member, NEXT_HOP_INDEXf, nh_index));
                }
            } else {
                /* Support trunk remote gports */
                gport = trunk_member_array[idx];
                BCM_IF_ERROR_RETURN(
                    _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index));
                
                if (!old_nh_index) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_trx_gport_to_nhi_set(unit, gport, nh_index));
                }
            }

            /* Only count the number of VPs on local physical ports */
            if(BCM_SUCCESS(rv)) {
                _bcm_port_info_access(unit, local_member, &port_info);
                port_info->vp_count++;
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_get(
                        unit, local_member, &port_flags));
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_set(
                        unit, local_member, port_flags));
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_vxlan_trunk_member_delete
 * Purpose:
 *      Clear Trunk Port-membership from 
 *      EGR_PORT_TO_NHI_MAPPING and EGR_DGPP_TO_NHI
 * Parameters:
 *     unit  - (IN) Device Number
 *     tgid - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be deleted
 *     trunk_member_array - (IN) Trunk member gports to be deleted
*/
int
bcm_td2_vxlan_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
                int trunk_member_count, bcm_port_t *trunk_member_array) 
{ 
    int idx = 0;
    int rv = BCM_E_NONE, rv2 = BCM_E_NONE;
    uint32 reg_val = 0;
    int old_nh_index = 0;
    int nh_index = 0;
    bcm_l3_egress_t egr;
    int gport = 0;
    _bcm_port_info_t *port_info;
    int local_member = 0;
    uint32 port_flags;

    rv = _bcm_trunk_id_validate(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        return BCM_E_PORT;
    }

    /* Obtain nh_index for trunk_id */
    rv = _bcm_xgs3_trunk_nh_store_get(unit, trunk_id, &nh_index);
    if (nh_index == 0) {
        for (idx = 0; idx < trunk_member_count; idx++) {
            rv2 = bcm_esw_port_local_get(unit, trunk_member_array[idx], &local_member);
            /* Only support trunk local ports for trunk match */
            if(BCM_FAILURE(rv2)) {
                continue;
            }
            rv += _bcm_td2_vxlan_trunk_table_reset(unit, local_member);
        }
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        bcm_l3_egress_t_init(&egr);
        BCM_IF_ERROR_RETURN(bcm_td2_vxlan_egress_get(unit, &egr, nh_index));

        if (egr.flags & BCM_L3_VXLAN_ONLY) {
            for (idx = 0; idx < trunk_member_count; idx++) {
                rv2 = bcm_esw_port_local_get(unit, trunk_member_array[idx], &local_member);
                if (SOC_REG_IS_VALID(unit, EGR_PORT_TO_NHI_MAPPINGr)) {
                    /* Only support trunk local ports */
                    if(BCM_FAILURE(rv2)) {
                        continue;
                    }
                    BCM_IF_ERROR_RETURN(
                        READ_EGR_PORT_TO_NHI_MAPPINGr(unit, local_member, &reg_val));
                    old_nh_index = soc_reg_field_get(unit, EGR_PORT_TO_NHI_MAPPINGr, reg_val, NEXT_HOP_INDEXf);
                    if (0 != old_nh_index) {
                        /* Reset EGR_PORT_TO_NHI_MAPPING */
                        rv = soc_reg_field32_modify(unit, EGR_PORT_TO_NHI_MAPPINGr,
                                local_member, NEXT_HOP_INDEXf, 0x0);
                        if (BCM_FAILURE(rv)) {
                            return rv;
                        }
                    }
                } else {
                    /* Support trunk remote gports */
                    gport = trunk_member_array[idx];
                    BCM_IF_ERROR_RETURN(
                        _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index));
                    
                    if (0 != old_nh_index) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_trx_gport_to_nhi_set(unit, gport, 0));
                    }
                }

                /* Only count the number of VPs on local physical ports */
                if(BCM_SUCCESS(rv2)) {
                    _bcm_port_info_access(unit, local_member, &port_info);
                    port_info->vp_count--;
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_port_vlan_member_get(
                            unit, local_member, &port_flags));
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_port_vlan_member_set(
                            unit, local_member, port_flags));
                }
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_match_vp_replace
 * Purpose:
 *      Replace VP value in VXLAN VP's match entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp - (IN) VXLAN VP whose match entry is being replaced.
 *      new_vp - (IN) New VP value.
 *      old_vp - (OUT) Old VP value.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_vxlan_match_vp_replace(int unit, int vp, int new_vp, int *old_vp)
{
    int                rv = BCM_E_NONE;
    soc_mem_t          mem;
    int                key_type = bcmVlanTranslateKeyInvalid;
    uint32             svp_valid, tunnel_index, tunnel_sip;
    int                entry_index;
    int                port, my_modid, stm_idx, i;
    int                local_port[SOC_MAX_NUM_PORTS];
    int                local_port_count = 0;
    uint32 vent[SOC_MAX_MEM_WORDS];
    source_trunk_map_table_entry_t stm_entry;
    uint32 ment[SOC_MAX_MEM_WORDS];
    _bcm_vxlan_match_port_info_t *mkey = NULL;
    int d = 0;

    mkey = &(VXLAN_INFO(unit)->match_key[vp]);

#define _BCM_VLXLT_KTYPE_ASSIGN(_ktype, _k) \
    do { \
        if (_ktype == bcmVlanTranslateKeyInvalid) { \
            _ktype = _k; \
        } \
    } while(0)

    switch(mkey->flags) {
        case _BCM_VXLAN_PORT_MATCH_TYPE_VLAN:
            _BCM_VLXLT_KTYPE_ASSIGN(key_type, bcmVlanTranslateKeyPortOuter);
            /* Fall through */
        case _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN:
            _BCM_VLXLT_KTYPE_ASSIGN(key_type, bcmVlanTranslateKeyPortInner);
            /* Fall through */
        case _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED:
            _BCM_VLXLT_KTYPE_ASSIGN(key_type, bcmVlanTranslateKeyPortDouble);
            /* Fall through */
        case _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI:
            _BCM_VLXLT_KTYPE_ASSIGN(key_type, bcmVlanTranslateKeyPortOuterPri);
            /* Construct lookup key */
            sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                mem = VLAN_XLATE_1_DOUBLEm;

                soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
            } else
#endif
            {
                mem = VLAN_XLATEm;

                soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
            }

            if (mkey->modid != -1) {
                BCM_GPORT_MODPORT_SET(port, mkey->modid, mkey->port);
            } else {
                BCM_GPORT_TRUNK_SET(port, mkey->trunk_id);
            }

            BCM_IF_ERROR_RETURN(
                _bcm_trx_vlan_translate_entry_assemble(unit, &vent,
                                                       port, key_type,
                                                       mkey->match_inner_vlan,
                                                       mkey->match_vlan));
            soc_mem_lock(unit, mem);
            /* Lookup existing entry */
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                                &entry_index, &vent, &vent, 0);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            /* Replace entry's VP value */
            *old_vp = soc_mem_field32_get(unit, mem, vent, SOURCE_VPf);
            soc_mem_field32_set(unit, mem, vent, SOURCE_VPf, new_vp);

            /* Insert new entry */
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, &vent);
            soc_mem_unlock(unit, mem);
            if (rv == SOC_E_EXISTS) {
                rv = BCM_E_NONE;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        case _BCM_VXLAN_PORT_MATCH_TYPE_PORT:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            soc_mem_lock(unit, mem);

            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, 
                              mkey->index, &stm_entry);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            /* Resolve SVP Mode */
            if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                svp_valid = soc_mem_field32_get(unit, mem, 
                                                &stm_entry, SVP_VALIDf);
                if (svp_valid == 0) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_INTERNAL;
                }
            }
            *old_vp = soc_mem_field32_get(unit, mem, 
                                          &stm_entry, SOURCE_VPf);
            rv = soc_mem_field32_modify(unit, mem, mkey->index, 
                                        SOURCE_VPf, new_vp);

            soc_mem_unlock(unit, mem);
            break;
        case _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            /* Get module id for unit. */
            BCM_IF_ERROR_RETURN(
                bcm_esw_stk_my_modid_get(unit, &my_modid));

            BCM_IF_ERROR_RETURN(
                _bcm_esw_trunk_local_members_get(unit, mkey->trunk_id,
                                                 SOC_MAX_NUM_PORTS,
                                                 local_port,
                                                 &local_port_count));
            soc_mem_lock(unit, mem);

            for (i = 0; i < local_port_count; i++) {
                /* Get index to source trunk map table */
                rv = _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                           local_port[i],
                                                           &stm_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }

                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, 
                                  stm_idx, &stm_entry);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }

                /* Resolve SVP Mode */
                if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                    svp_valid = soc_mem_field32_get(unit, mem, 
                                                    &stm_entry, SVP_VALIDf);
                    if (svp_valid == 0) {
                        soc_mem_unlock(unit, mem);
                        return BCM_E_INTERNAL;
                    }
                }
                *old_vp = soc_mem_field32_get(unit, mem, 
                                              &stm_entry, SOURCE_VPf);

                rv = soc_mem_field32_modify(unit, mem, stm_idx, 
                                            SOURCE_VPf, new_vp);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }
            }

            soc_mem_unlock(unit, mem);
            break;
        case _BCM_VXLAN_PORT_MATCH_TYPE_VNID:
            tunnel_index = mkey->match_tunnel_index;
            tunnel_sip = VXLAN_INFO(unit)->vxlan_tunnel_term[tunnel_index].sip;

            sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            mem = MPLS_ENTRYm;
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem = MPLS_ENTRY_SINGLEm;
                d = 1;
                soc_mem_field32_set(unit, mem, &ment, BASE_VALIDf, 1);
                soc_mem_field32_set(unit, mem, &ment,
                    DATA_TYPEf, _BCM_VXLAN_FLEX_DATA_TYPE_IPV4_SIP);
            } else
#endif
            {
                soc_mem_field32_set(unit, mem, &ment, VALIDf, 1);
            }

            soc_mem_lock(unit, mem);
            soc_mem_field32_set(unit, mem, &ment, BCM_VXLAN_SIP_FIELD(d, SIPf), tunnel_sip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                soc_mem_field32_set(unit, mem, &ment, VXLAN_SIP__OVIDf,
                     VXLAN_INFO(unit)->vxlan_tunnel_term[tunnel_index].vlan);
            }
#endif
            soc_mem_field32_set(unit, mem, &ment,
                KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_TUNNEL);
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                                &entry_index, &ment, &ment, 0);
            
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_PARAM;
            }

            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            
            /* Replace entry's VP value */
            *old_vp = soc_mem_field32_get(unit, mem, &ment, BCM_VXLAN_SIP_FIELD(d, SVPf));
            soc_mem_field32_set(unit, mem, &ment, BCM_VXLAN_SIP_FIELD(d, SVPf), new_vp);
            /* Insert new entry */
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, ment);

            soc_mem_unlock(unit, mem);

            if (rv == SOC_E_EXISTS) {
                rv = BCM_E_NONE;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        case _BCM_VXLAN_PORT_MATCH_TYPE_NONE:
            *old_vp = vp;
            break;
        default:
            return BCM_E_PARAM;
    }

#undef _BCM_VLXLT_KTYPE_ASSIGN

    return rv;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a VXLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VXLAN virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_vxlan_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int vp, old_vp;

    if (!BCM_GPORT_IS_VXLAN_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_PARAM;
    }

    /* Set source VP LAG by replacing the SVP field in VXLAN VP's
     * match entry with the VP value representing the VP LAG.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_vxlan_match_vp_replace(unit, vp, vp_lag_vp, &old_vp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a VXLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VXLAN virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_vxlan_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int vp, old_vp;

    if (!BCM_GPORT_IS_VXLAN_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_PARAM;
    }

    /* Clear source VP LAG by replacing the SVP field in VXLAN VP's
     * match entry with the VP value.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_vxlan_match_vp_replace(unit, vp, vp, &old_vp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_vxlan_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a VXLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VXLAN virtual port GPORT ID. 
 *      vp_lag_vp - (OUT) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_vxlan_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp)
{
    int                rv = BCM_E_NONE;
    soc_mem_t          mem;
    int                key_type = bcmVlanTranslateKeyInvalid;
    bcm_gport_t        vp;
    uint32             svp_valid, tunnel_index, tunnel_sip;
    int                entry_index;
    int                port, my_modid, stm_idx;
    int                local_port[SOC_MAX_NUM_PORTS];
    int                local_port_count = 0;
    uint32 vent[SOC_MAX_MEM_WORDS];
    uint32 ment[SOC_MAX_MEM_WORDS];
    source_trunk_map_table_entry_t stm_entry;
    _bcm_vxlan_match_port_info_t *mkey = NULL;
    int d = 0;

    if (!BCM_GPORT_IS_VXLAN_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        return BCM_E_PARAM;
    }

    mkey = &(VXLAN_INFO(unit)->match_key[vp]);

#define _BCM_VLXLT_KTYPE_ASSIGN(_ktype, _k) \
        do { \
            if (_ktype == bcmVlanTranslateKeyInvalid) { \
                _ktype = _k; \
            } \
        } while(0)
    
    switch(mkey->flags) {
        case _BCM_VXLAN_PORT_MATCH_TYPE_VLAN:
            _BCM_VLXLT_KTYPE_ASSIGN(key_type, bcmVlanTranslateKeyPortOuter);
            /* Fall through */
        case _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN:
            _BCM_VLXLT_KTYPE_ASSIGN(key_type, bcmVlanTranslateKeyPortInner);
            /* Fall through */
        case _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED:
            _BCM_VLXLT_KTYPE_ASSIGN(key_type, bcmVlanTranslateKeyPortDouble);
            /* Fall through */
        case _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI:
            _BCM_VLXLT_KTYPE_ASSIGN(key_type, bcmVlanTranslateKeyPortOuterPri);

            sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                mem = VLAN_XLATE_1_DOUBLEm;

                soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
            } else
#endif
            {
                mem = VLAN_XLATEm;

                soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
            }

            if (mkey->modid != -1) {
                BCM_GPORT_MODPORT_SET(port, mkey->modid, mkey->port);
            } else {
                BCM_GPORT_TRUNK_SET(port, mkey->trunk_id);
            }
            /* Construct lookup key */
            BCM_IF_ERROR_RETURN(
                _bcm_trx_vlan_translate_entry_assemble(unit, vent,
                                                       port, key_type,
                                                       mkey->match_inner_vlan,
                                                       mkey->match_vlan));
            soc_mem_lock(unit, mem);
            /* Lookup existing entry */
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                                &entry_index, vent, vent, 0);
            soc_mem_unlock(unit, mem);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem, vent, SOURCE_VPf);
            break;
        case _BCM_VXLAN_PORT_MATCH_TYPE_PORT:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            soc_mem_lock(unit, mem);

            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, 
                              mkey->index, &stm_entry);
            soc_mem_unlock(unit, mem);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* Resolve SVP Mode */
            if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                svp_valid = soc_mem_field32_get(unit, mem, 
                                                &stm_entry, SVP_VALIDf);
                if (svp_valid == 0) {
                    return BCM_E_INTERNAL;
                }
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem, 
                                             &stm_entry, SOURCE_VPf);
            break;
        case _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            /* Get module id for unit. */
            BCM_IF_ERROR_RETURN(
                bcm_esw_stk_my_modid_get(unit, &my_modid));

            BCM_IF_ERROR_RETURN(
                _bcm_esw_trunk_local_members_get(unit, mkey->trunk_id,
                                                 SOC_MAX_NUM_PORTS,
                                                 local_port,
                                                 &local_port_count));

            /* Get index to source trunk map table,
             * Only get it from the first member in LAG
             */
            rv = _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                       local_port[0],
                                                       &stm_idx);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            soc_mem_lock(unit, mem);
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, 
                              stm_idx, &stm_entry);
            soc_mem_unlock(unit, mem);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Resolve SVP Mode */
            if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                svp_valid = soc_mem_field32_get(unit, mem, 
                                                &stm_entry, SVP_VALIDf);
                if (svp_valid == 0) {
                    return BCM_E_INTERNAL;
                }
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem, 
                                             &stm_entry, SOURCE_VPf);
            break;
        case _BCM_VXLAN_PORT_MATCH_TYPE_VNID:
            tunnel_index = mkey->match_tunnel_index;
            tunnel_sip = VXLAN_INFO(unit)->vxlan_tunnel_term[tunnel_index].sip;

            sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            mem = MPLS_ENTRYm;
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem = MPLS_ENTRY_SINGLEm;
                d = 1;
                soc_mem_field32_set(unit, mem, ment, BASE_VALIDf, 1);
                soc_mem_field32_set(unit, mem, &ment,
                    DATA_TYPEf, _BCM_VXLAN_FLEX_DATA_TYPE_IPV4_SIP);
            } else
#endif
            {
                soc_mem_field32_set(unit, mem, &ment, VALIDf, 1);
            }

            soc_mem_field32_set(unit, mem, &ment, BCM_VXLAN_SIP_FIELD(d, SIPf), tunnel_sip);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                soc_mem_field32_set(unit, mem, &ment, VXLAN_SIP__OVIDf,
                    VXLAN_INFO(unit)->vxlan_tunnel_term[tunnel_index].vlan);
            }
#endif
            soc_mem_field32_set(unit, mem, &ment,
                KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_TUNNEL);
            soc_mem_lock(unit, mem);
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &entry_index, &ment, &ment, 0);
            soc_mem_unlock(unit, mem);
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_PARAM;
            }

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem, &ment, BCM_VXLAN_SIP_FIELD(d, SVPf));
            break;
        default:
            return BCM_E_PARAM;
    }

#undef _BCM_VLXLT_KTYPE_ASSIGN

    if (!_bcm_vp_used_get(unit, *vp_lag_vp, _bcmVpTypeVpLag)) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

int
_bcm_td2_vxlan_svp_field_set(int unit, bcm_gport_t vp,
    soc_field_t field, int value)
{
    int rv = BCM_E_NONE;
    source_vp_entry_t svp;

    rv = bcm_td2_vxlan_lock(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        bcm_td2_vxlan_unlock(unit);
        return rv;
    }
    if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, field)) {
        soc_SOURCE_VPm_field32_set(unit, &svp, field, value);
    }
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    bcm_td2_vxlan_unlock(unit);
    return rv;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_td2_vxlan_sw_dump
 * Purpose:
 *     Displays VXLAN information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_td2_vxlan_sw_dump(int unit)
{
    int i, num_vp;
    _bcm_td2_vxlan_bookkeeping_t *vxlan_info = VXLAN_INFO(unit);

    LOG_CLI((BSL_META_U(unit,
                        "Tunnel Initiator Endpoints:\n")));
    for (i = 0; i < _BCM_MAX_NUM_VXLAN_TUNNEL; i++) {
        if (VXLAN_INFO(unit)->vxlan_tunnel_init[i].dip != 0 &&
            VXLAN_INFO(unit)->vxlan_tunnel_init[i].sip != 0 ) {
            LOG_CLI((BSL_META_U(unit,
                                "Tunnel idx:%d, sip:%x, dip:%x, vlan:%x \n"), i,
                     vxlan_info->vxlan_tunnel_init[i].sip, 
                     vxlan_info->vxlan_tunnel_init[i].dip,
                     vxlan_info->vxlan_tunnel_init[i].vlan));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nTunnel Terminator Endpoints:\n")));
    for (i = 0; i < _BCM_MAX_NUM_VXLAN_TUNNEL; i++) {
        if (VXLAN_INFO(unit)->vxlan_tunnel_term[i].dip != 0 &&
            VXLAN_INFO(unit)->vxlan_tunnel_term[i].sip != 0 ) {
            LOG_CLI((BSL_META_U(unit,
                                "Tunnel idx:%d, sip:%x, dip:%x, vlan:%x\n"), i,
                     vxlan_info->vxlan_tunnel_term[i].sip, 
                     vxlan_info->vxlan_tunnel_term[i].dip,
                     vxlan_info->vxlan_tunnel_term[i].vlan));
        }
    }
    
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    LOG_CLI((BSL_META_U(unit,
                        "\n  Match Info    : \n")));
    for (i = 0; i < num_vp; i++) {
        if ((vxlan_info->match_key[i].trunk_id == 0 || 
             vxlan_info->match_key[i].trunk_id == -1) && 
            (vxlan_info->match_key[i].modid == 0 || 
             vxlan_info->match_key[i].modid == -1) &&
            (vxlan_info->match_key[i].port == 0) &&
            (vxlan_info->match_key[i].flags == 0)) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n  VXLAN port vp = %d\n"), i));
        LOG_CLI((BSL_META_U(unit,
                            "Flags = %x\n"), vxlan_info->match_key[i].flags));
        LOG_CLI((BSL_META_U(unit,
                            "Index = %x\n"), vxlan_info->match_key[i].index));
        LOG_CLI((BSL_META_U(unit,
                            "TGID = %d\n"), vxlan_info->match_key[i].trunk_id));
        LOG_CLI((BSL_META_U(unit,
                            "Modid = %d\n"), vxlan_info->match_key[i].modid));
        LOG_CLI((BSL_META_U(unit,
                            "Port = %d\n"), vxlan_info->match_key[i].port));
        LOG_CLI((BSL_META_U(unit,
                            "Match VLAN = %d\n"), 
                 vxlan_info->match_key[i].match_vlan));
        LOG_CLI((BSL_META_U(unit,
                            "Match Inner VLAN = %d\n"), 
                 vxlan_info->match_key[i].match_inner_vlan));
        LOG_CLI((BSL_META_U(unit,
                            "Match tunnel Index = %x\n"), 
                 vxlan_info->match_key[i].match_tunnel_index));
        LOG_CLI((BSL_META_U(unit,
                            "Match count = %x\n"), 
                 vxlan_info->match_key[i].match_count));
        LOG_CLI((BSL_META_U(unit,
                            "Related VFI = %d\n"), 
                 vxlan_info->match_key[i].vfi));         
        LOG_CLI((BSL_META_U(unit,
                            "Flags2 = %x\n"),
                 vxlan_info->match_key[i].flags2));
    }
    
    LOG_CLI((BSL_META_U(unit,
                        "\nVxlan VPN INFO:\n")));
    for (i = 0; i < soc_mem_index_count(unit, VFIm); i++) {
        if (vxlan_info->vxlan_vpn_info[i].vnid != 0 || 
            vxlan_info->vxlan_vpn_info[i].vxlan_vpn_type != 0 || 
            vxlan_info->vxlan_vpn_info[i].sip != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "\nVxlan VPN VFI %d:\n"), i));            
            LOG_CLI((BSL_META_U(unit,
                                "VXLAN VPN TYPE = %d \n"), vxlan_info->vxlan_vpn_info[i].vxlan_vpn_type));
            LOG_CLI((BSL_META_U(unit,
                                "VXLAN VPN SIP = %x \n"), vxlan_info->vxlan_vpn_info[i].sip));
            LOG_CLI((BSL_META_U(unit,
                                "VXLAN VPN VNID = %d \n"), vxlan_info->vxlan_vpn_info[i].vnid));
        }
    }

    for (i = 0; i < num_vp; i++) {
        if (vxlan_info->vfi_vnid_map_count[i]) {
            LOG_CLI((BSL_META_U(unit,
                                "\nVxlan VP %d: VP-based VFI-VNID mappings count = %d\n"), i,
                                vxlan_info->vfi_vnid_map_count[i]));
        }
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *      _bcm_td2_vxlan_dip_entry_idx_get
 * Purpose:
 *      Get TD2 vxlan_dip_entry index by VxLAN DIP.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      vxlan_dip - (IN)VxLAN DIP.
 *      index     - (OUT)vxlan_dip_entry index.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_td2_vxlan_dip_entry_idx_get(int unit, 
                                 bcm_ip_t vxlan_dip,
                                 int *index)
{
    soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS];
    int d = 0; /*vxlan decoupled mode*/

    sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

#if defined(BCM_TRIDENT3_SUPPORT)
     if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        bcm_tunnel_terminator_t tnl_info;

        mem = MPLS_ENTRY_SINGLEm;
        d = 1;
        bcm_tunnel_terminator_t_init(&tnl_info);
        _bcm_td2_vxlan_tunnel_terminate_entry_key_set(unit, &tnl_info,
                       vent, 0, 1);
    } else
#endif
    {
        mem = VLAN_XLATEm;

        soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
    }

    soc_mem_field32_set(unit, mem, vent, BCM_VXLAN_DIP_FIELD(d,DIPf), vxlan_dip);

    return soc_mem_search(unit, mem, MEM_BLOCK_ANY, index, vent, vent, 0);
}

/*
 * Function:
 *      _bcm_esw_vxlan_dip_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given ingress interface.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip     - (IN) vxlan dip
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

static 
bcm_error_t _bcm_td2_vxlan_dip_stat_get_table_info(
            int                        unit,
            bcm_ip_t                   vxlan_dip,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{

    int index;
    soc_mem_t mem;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        return BCM_E_UNAVAIL;
    }
#endif

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_vxlan_dip_entry_idx_get(unit, vxlan_dip, &index)); 

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        mem = MPLS_ENTRY_SINGLEm;
    } else
#endif
    {
        mem = VLAN_XLATEm;
    }

    if (vxlan_dip != 0) { 
        table_info[*num_of_tables].table = mem;
        table_info[*num_of_tables].index = index;
        table_info[*num_of_tables].direction = bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
    }

    if (*num_of_tables == 0) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_dip_stat_counter_get
 * Description:
 *      Get counter statistic values for specific vxlan dip
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      vxlan_dip        - (IN) vxlan dip information
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
bcm_error_t 
_bcm_td2_vxlan_dip_stat_counter_get(
            int unit,
            int sync_mode,
            bcm_ip_t vxlan_dip,
            bcm_vxlan_dip_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmVxlanDipInPackets) ||
        (stat == bcmVxlanDipInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         /* direction = bcmStatFlexDirectionEgress; */
         return BCM_E_PARAM;
    }

    if ((num_entries > 0) && 
        ((NULL == counter_indexes) || (NULL == counter_values))) {
        return BCM_E_PARAM;
    }

    if (stat == bcmVxlanDipInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1; 
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_dip_stat_get_table_info(
                   unit, vxlan_dip,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
               /*ctr_offset_info.offset_index = counter_indexes[index_count];*/
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
}

/*
 * Function:
 *      _bcm_td2_vxlan_dip_stat_multi_get
 *
 * Description:
 *  Get Multiple vxlan dip counter value for specified IPMC group index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip     - (IN) vxlan dip
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t  _bcm_td2_vxlan_dip_stat_multi_get(
                            int                   unit, 
                            bcm_ip_t              vxlan_dip,
                            int                   nstat, 
                            bcm_vxlan_dip_stat_t  *stat_arr,
                            uint64                *value_arr)
{
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};
    int              sync_mode = 0; 

    if ((nstat > 0) && ((NULL == stat_arr) || (NULL == value_arr))) {
        return BCM_E_PARAM;
    }

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat; idx++) {
         BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_dip_stat_counter_get( 
                             unit, sync_mode, vxlan_dip, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmVxlanDipInPackets)) {
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
}

/*
 * Function:
 *      _bcm_td2_vxlan_dip_stat_multi_get32
 *
 * Description:
 *  Get 32bit vxlan dip counter value for specified IPMC group index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip     - (IN) vxlan dip
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t  _bcm_td2_vxlan_dip_stat_multi_get32(
                            int                  unit, 
                            bcm_ip_t             vxlan_dip,
                            int                  nstat, 
                            bcm_vxlan_dip_stat_t *stat_arr,
                            uint32               *value_arr)
{
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};
    int              sync_mode = 0; 

    if ((nstat > 0) && ((NULL == stat_arr) || (NULL == value_arr))) {
        return BCM_E_PARAM;
    }

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat; idx++) {
         BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_dip_stat_counter_get( 
                             unit, sync_mode, vxlan_dip, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmVxlanDipInPackets)) {
                value_arr[idx] = counter_values.packets;
         } else {
             value_arr[idx] = COMPILER_64_LO(counter_values.bytes);
         }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_dip_stat_counter_set
 * Description:
 *      Set counter statistic values for specific vxlan dip
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip     - (IN) vxlan dip
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
bcm_error_t 
_bcm_td2_vxlan_dip_stat_counter_set(
            int unit,
            bcm_ip_t vxlan_dip,
            bcm_vxlan_dip_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    
    if ((stat == bcmVxlanDipInPackets) ||
        (stat == bcmVxlanDipInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         /* direction = bcmStatFlexDirectionEgress; */
         return BCM_E_PARAM;
    }

    if ((num_entries > 0) &&
        ((NULL == counter_indexes) || (NULL == counter_values))) {
        return BCM_E_PARAM;
    }

    if (stat == bcmVxlanDipInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1; 
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_dip_stat_get_table_info(
                   unit, vxlan_dip, &num_of_tables, &table_info[0]));

    for (table_count=0; table_count < num_of_tables; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries; index_count++) {
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
}

/*
 * Function:
 *      _bcm_esw_vxlan_dip_stat_counter_get
 * Description:
 *      Get counter statistic values for specific vxlan dip
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) if 1 sync sw count value with hw count
 *                              before retieving count else get
 *                              sw accumulated count
 *      vxlan_dip      - (IN) vxlan dip
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
bcm_error_t
_bcm_esw_vxlan_dip_stat_counter_get(
                               int                unit,
                               int                sync_mode,
                               bcm_ip_t           vxlan_dip,
                               bcm_vxlan_dip_stat_t  stat,
                               uint32             num_entries,
                               uint32             *counter_indexes,
                               bcm_stat_value_t   *counter_values)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) { 
        return _bcm_td2_vxlan_dip_stat_counter_get(unit, sync_mode, vxlan_dip, stat, 
                    num_entries, counter_indexes, counter_values);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}


/*
 * Function:
 *     _bcm_td2_vxlan_dip_stat_multi_set
 *
 * Description:
 *  Set vxlan dip counter value for specified IPMC group index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip    - (IN) vxlan dip
 *      stat             - (IN) IPMC group counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t  _bcm_td2_vxlan_dip_stat_multi_set(
                            int                   unit, 
                            bcm_ip_t              vxlan_dip,
                            int                   nstat, 
                            bcm_vxlan_dip_stat_t  *stat_arr,
                            uint64                *value_arr)
{
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};

    if ((nstat > 0) && ((NULL == stat_arr) || (NULL == value_arr))) {
        return BCM_E_PARAM;
    }

    /* Iterate f all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat; idx++) {
         if ((stat_arr[idx] == bcmVxlanDipInPackets)) {
              counter_values.packets = COMPILER_64_LO(value_arr[idx]);
         } else {
              COMPILER_64_SET(counter_values.bytes,
                              COMPILER_64_HI(value_arr[idx]),
                              COMPILER_64_LO(value_arr[idx]));
         }
          BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_dip_stat_counter_set( 
                             unit, vxlan_dip, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
    }
    return rv;

}





/*
 * Function:
 *      _bcm_td2_vxlan_dip_stat_multi_set32
 *
 * Description:
 *  Set 32biv xlan dip counter value for specified IPMC group index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *     vxlan_dip     - (IN)vxlan dip
 *      stat             - (IN) IPMC group counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

 bcm_error_t  _bcm_td2_vxlan_dip_stat_multi_set32(
                            int                   unit, 
                            bcm_ip_t              vxlan_dip,
                            int                   nstat, 
                            bcm_vxlan_dip_stat_t  *stat_arr,
                            uint32                *value_arr)
{
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};

    if ((nstat > 0)  && ((NULL == stat_arr) || (NULL == value_arr))) {
        return BCM_E_PARAM;
    }

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat; idx++) {
         if ((stat_arr[idx] == bcmVxlanDipInPackets)) {
             counter_values.packets = value_arr[idx];
         } else {
             COMPILER_64_SET(counter_values.bytes,0,value_arr[idx]);
         }

         BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_dip_stat_counter_set( 
                             unit, vxlan_dip, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_vxlan_dip_stat_id_get
 * Description:
 *   Retrieve associated stat counter for given VXLAN DIP
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip      - (IN) vxlan ip
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t 
_bcm_td2_vxlan_dip_stat_id_get(
            int unit,
            bcm_ip_t vxlan_dip,
            bcm_vxlan_dip_stat_t stat,
            uint32 *stat_counter_id)
{
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if ((stat == bcmVxlanDipInPackets) ||
        (stat == bcmVxlanDipInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         /* direction = bcmStatFlexDirectionEgress; */
         return BCM_E_PARAM;
    }

    if (NULL == stat_counter_id) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_dip_stat_get_table_info(
                        unit,vxlan_dip,&num_of_tables,&table_info[0]));
    for (index=0; index < num_of_tables; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_td2_vxlan_dip_stat_detach
 * Description:
 *      Detach counters entries to the given vxlan dip
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip     - (IN) vxlan dip
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t
_bcm_td2_vxlan_dip_stat_detach(
            int unit,
            bcm_ip_t vxlan_dip)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_dip_stat_get_table_info(
                   unit, vxlan_dip,&num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables; count++) {
      if (table_info[count].direction == bcmStatFlexDirectionIngress) {
           rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
           }
      } else {
         return BCM_E_INTERNAL;
      }
    }
    
    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}

/*
 * Function:
 *      _bcm_td2_vxlan_dip_stat_attach
 * Description:
 *      Attach counters entries to the given vxlan dip
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vxlan_dip      - (IN) vxlan dip 
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 */
bcm_error_t
_bcm_td2_vxlan_dip_stat_attach(
            int unit,
            bcm_ip_t vxlan_dip,
            uint32 stat_counter_id)
{
    soc_mem_t                 table[4]={0};
    uint32                    flex_table_index=0;
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
                        unit,object,4,&actual_num_tables,&table[0],&direction));

    BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_dip_stat_get_table_info(
                   unit, vxlan_dip,&num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables; count++) {
         for (flex_table_index=0; 
              flex_table_index < actual_num_tables ; 
              flex_table_index++) {
              if ((table_info[count].direction == direction) &&
                  (table_info[count].table == table[flex_table_index]) ) {
                  if (direction == bcmStatFlexDirectionIngress) {
                      return _bcm_esw_stat_flex_attach_ingress_table_counters(
                             unit,
                             table_info[count].table,
                             table_info[count].index,
                             offset_mode,
                             base_index,
                             pool_number);
                  } else {
                      return BCM_E_INTERNAL;
                  }
              }
         }
    }
    return BCM_E_NOT_FOUND;
}
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

