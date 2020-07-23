/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    virtual.c
 * Purpose: Manages VP / VFI resources
 */
#if defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/l2x.h>
#include <soc/mcm/allenum.h>
#include <sal/compiler.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw_dispatch.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
#include <bcm_int/esw/trident.h>
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_ENDURO_SUPPORT)
#include <bcm_int/esw/enduro.h>
#endif /* BCM_ENDURO_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/stg.h>
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif /* BCM_SABER2_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/flow.h>
#include <bcm_int/esw/trident3.h>
#endif

/*
 * Global virtual book keeping info
 */
_bcm_virtual_bookkeeping_t  _bcm_virtual_bk_info[BCM_MAX_NUM_UNITS] = {{ 0 }};
STATIC sal_mutex_t _virtual_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define VIRTUAL_INFO(_unit_)   (&_bcm_virtual_bk_info[_unit_])

/*
 * VFI table usage bitmap operations
 */
#define _BCM_VIRTUAL_VFI_USED_GET(_u_, _vfi_) \
        ((VIRTUAL_INFO(_u_)->vfi_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vfi_bitmap, (_vfi_)) : 0)
#define _BCM_VIRTUAL_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vfi_bitmap, (_vfi_))
#define _BCM_VIRTUAL_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vfi_bitmap, (_vfi_))

/*
 * Virtual Port usage bitmap operations
 */
#define _BCM_VIRTUAL_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vp_bitmap, (_vp_)) : 0)
#define _BCM_VIRTUAL_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vp_bitmap, (_vp_))
#define _BCM_VIRTUAL_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vp_bitmap, (_vp_))

/*
 * MPLS VFI table usage bitmap operations
 */
#define _BCM_MPLS_VFI_USED_GET(_u_, _vfi_) \
        ((VIRTUAL_INFO(_u_)->mpls_vfi_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->mpls_vfi_bitmap, (_vfi_)) : 0)
#define _BCM_MPLS_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->mpls_vfi_bitmap, (_vfi_))
#define _BCM_MPLS_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->mpls_vfi_bitmap, (_vfi_))

/*
 * MPLS Virtual Port usage bitmap operations
 */
#define _BCM_MPLS_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->mpls_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->mpls_vp_bitmap, (_vp_)) : 0)
#define _BCM_MPLS_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->mpls_vp_bitmap, (_vp_))
#define _BCM_MPLS_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->mpls_vp_bitmap, (_vp_))

/*
 * MIM VFI table usage bitmap operations
 */
#define _BCM_MIM_VFI_USED_GET(_u_, _vfi_) \
        ((VIRTUAL_INFO(_u_)->mim_vfi_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->mim_vfi_bitmap, (_vfi_)) : 0)
#define _BCM_MIM_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->mim_vfi_bitmap, (_vfi_))
#define _BCM_MIM_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->mim_vfi_bitmap, (_vfi_))

/*
 * MIM Virtual Port usage bitmap operations
 */
#define _BCM_MIM_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->mim_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->mim_vp_bitmap, (_vp_)) : 0)
#define _BCM_MIM_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->mim_vp_bitmap, (_vp_))
#define _BCM_MIM_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->mim_vp_bitmap, (_vp_))

/*
 * L2GRE VFI usage bitmap operations
 */
#define _BCM_L2GRE_VFI_USED_GET(_u_, _vfi_) \
        ((VIRTUAL_INFO(_u_)->l2gre_vfi_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->l2gre_vfi_bitmap, (_vfi_)) : 0)
#define _BCM_L2GRE_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->l2gre_vfi_bitmap, (_vfi_))
#define _BCM_L2GRE_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->l2gre_vfi_bitmap, (_vfi_))

/*
 * L2GRE Virtual Port usage bitmap operations
 */
#define _BCM_L2GRE_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->l2gre_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->l2gre_vp_bitmap, (_vp_)) : 0)
#define _BCM_L2GRE_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->l2gre_vp_bitmap, (_vp_))
#define _BCM_L2GRE_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->l2gre_vp_bitmap, (_vp_))


/*
 * VXLAN VFI usage bitmap operations
 */
#define _BCM_VXLAN_VFI_USED_GET(_u_, _vfi_) \
        ((VIRTUAL_INFO(_u_)->vxlan_vfi_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vxlan_vfi_bitmap, (_vfi_)) : 0)
#define _BCM_VXLAN_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vxlan_vfi_bitmap, (_vfi_))
#define _BCM_VXLAN_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vxlan_vfi_bitmap, (_vfi_))

/*
 * VXLAN Virtual Port usage bitmap operations
 */
#define _BCM_VXLAN_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->vxlan_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vxlan_vp_bitmap, (_vp_)) : 0)
#define _BCM_VXLAN_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vxlan_vp_bitmap, (_vp_))
#define _BCM_VXLAN_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vxlan_vp_bitmap, (_vp_))

/*
 * FLOW  VFI usage bitmap operations
 */
#define _BCM_FLOW_VFI_USED_GET(_u_, _vfi_) \
        ((VIRTUAL_INFO(_u_)->flow_vfi_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->flow_vfi_bitmap, (_vfi_)) : 0)
#define _BCM_FLOW_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->flow_vfi_bitmap, (_vfi_))
#define _BCM_FLOW_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->flow_vfi_bitmap, (_vfi_))

/*
 * FLOW Virtual Port usage bitmap operations
 */
#define _BCM_FLOW_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->flow_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->flow_vp_bitmap, (_vp_)) : 0)
#define _BCM_FLOW_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->flow_vp_bitmap, (_vp_))
#define _BCM_FLOW_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->flow_vp_bitmap, (_vp_))

/*
 * VXLAN shared Virtual Port usage bitmap operations
 */
#define _BCM_SHARED_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->vp_shared_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vp_shared_vp_bitmap, (_vp_)) : 0)
#define _BCM_SHARED_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vp_shared_vp_bitmap, (_vp_))
#define _BCM_SHARED_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vp_shared_vp_bitmap, (_vp_))

/*
 * Network Port bit-map for VxLAN/L2GRE/TRILL
 */
#define _BCM_NETWORK_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->vp_network_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vp_network_vp_bitmap, (_vp_)) : 0)
#define _BCM_NETWORK_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vp_network_vp_bitmap, (_vp_))
#define _BCM_NETWORK_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vp_network_vp_bitmap, (_vp_))


/* 
 * Virtual resource lock
 */
#define VIRTUAL_LOCK(unit) \
        sal_mutex_take(_virtual_mutex[unit], sal_mutex_FOREVER); 

#define VIRTUAL_UNLOCK(unit) \
        sal_mutex_give(_virtual_mutex[unit]); 

/*
 * Subport Virtual Port usage bitmap operations
 */
#define _BCM_SUBPORT_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->subport_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->subport_vp_bitmap, (_vp_)) : 0)
#define _BCM_SUBPORT_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->subport_vp_bitmap, (_vp_))
#define _BCM_SUBPORT_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->subport_vp_bitmap, (_vp_))

/*
 * WLAN Virtual Port usage bitmap operations
 */
#define _BCM_WLAN_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->wlan_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->wlan_vp_bitmap, (_vp_)) : 0)
#define _BCM_WLAN_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->wlan_vp_bitmap, (_vp_))
#define _BCM_WLAN_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->wlan_vp_bitmap, (_vp_))

   /*
     * TRILL Virtual Port usage bitmap operations
     */
#define _BCM_TRILL_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->trill_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->trill_vp_bitmap, (_vp_)) : 0)
#define _BCM_TRILL_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->trill_vp_bitmap, (_vp_))
#define _BCM_TRILL_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->trill_vp_bitmap, (_vp_))
/*
 * VLAN Virtual Port usage bitmap operations
 */
#define _BCM_VLAN_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->vlan_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vlan_vp_bitmap, (_vp_)) : 0)
#define _BCM_VLAN_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vlan_vp_bitmap, (_vp_))
#define _BCM_VLAN_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vlan_vp_bitmap, (_vp_))

/*
 * NIV Virtual Port usage bitmap operations
 */
#define _BCM_NIV_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->niv_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->niv_vp_bitmap, (_vp_)) : 0)
#define _BCM_NIV_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->niv_vp_bitmap, (_vp_))
#define _BCM_NIV_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->niv_vp_bitmap, (_vp_))

/*
 * Extender Virtual Port usage bitmap operations
 */
#define _BCM_EXTENDER_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->extender_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->extender_vp_bitmap, (_vp_)) : 0)
#define _BCM_EXTENDER_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->extender_vp_bitmap, (_vp_))
#define _BCM_EXTENDER_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->extender_vp_bitmap, (_vp_))

/*
 * VP LAG Virtual Port usage bitmap operations
 */
#define _BCM_VP_LAG_VP_USED_GET(_u_, _vp_) \
        ((VIRTUAL_INFO(_u_)->vp_lag_vp_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vp_lag_vp_bitmap, (_vp_)) : 0)
#define _BCM_VP_LAG_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vp_lag_vp_bitmap, (_vp_))
#define _BCM_VP_LAG_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vp_lag_vp_bitmap, (_vp_))

/*
 * VLAN VFI usage bitmap operations
 */
#define _BCM_VLAN_VFI_USED_GET(_u_, _vfi_) \
        ((VIRTUAL_INFO(_u_)->vlan_vfi_bitmap) ? \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vlan_vfi_bitmap, (_vfi_)) : 0)
#define _BCM_VLAN_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vlan_vfi_bitmap, (_vfi_))
#define _BCM_VLAN_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vlan_vfi_bitmap, (_vfi_))

/*
 * Virtual Port to VPN operations
 */
#define _BCM_VIRTUAL_VP_VFI_MAP(_u_, _vp_) \
        VIRTUAL_INFO((_u_))->vp_to_vpn[_vp_]

#define VIRTUAL_INIT_CHECK(_u_) \
        if (NULL == _virtual_mutex[_u_]) { return BCM_E_INIT; }

/*
 * Function:
 *      _bcm_virtual_free_resource
 * Purpose:
 *      Free all allocated resources
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_virtual_free_resource(int unit, _bcm_virtual_bookkeeping_t *virtual_info)
{
    if (!virtual_info) {
        return;
    }

    if (_virtual_mutex[unit]) {
        sal_mutex_destroy(_virtual_mutex[unit]);
        _virtual_mutex[unit] = NULL;
    } 

    if (virtual_info->vfi_bitmap) {
        sal_free(virtual_info->vfi_bitmap);
        virtual_info->vfi_bitmap = NULL;
    }

    if (virtual_info->vp_bitmap) {
        /* Before we free the VP bitmap, unregister it from the
         * SOC layer L2X thread. */
        soc_l2x_cml_vp_bitmap_set(unit, NULL);
        sal_free(virtual_info->vp_bitmap);
        virtual_info->vp_bitmap = NULL;
    }

    if (virtual_info->mpls_vfi_bitmap) {
        sal_free(virtual_info->mpls_vfi_bitmap);
        virtual_info->mpls_vfi_bitmap = NULL;
    }

    if (virtual_info->mpls_vp_bitmap) {
        sal_free(virtual_info->mpls_vp_bitmap);
        virtual_info->mpls_vp_bitmap = NULL;
    }

    if (virtual_info->mim_vfi_bitmap) {
        sal_free(virtual_info->mim_vfi_bitmap);
        virtual_info->mim_vfi_bitmap = NULL;
    }

    if (virtual_info->mim_vp_bitmap) {
        sal_free(virtual_info->mim_vp_bitmap);
        virtual_info->mim_vp_bitmap = NULL;
    }

    if (virtual_info->subport_vp_bitmap) {
        sal_free(virtual_info->subport_vp_bitmap);
        virtual_info->subport_vp_bitmap = NULL;
    }

    if (virtual_info->wlan_vp_bitmap) {
        sal_free(virtual_info->wlan_vp_bitmap);
        virtual_info->wlan_vp_bitmap = NULL;
    }

    if (virtual_info->trill_vp_bitmap) {
        sal_free(virtual_info->trill_vp_bitmap);
        virtual_info->trill_vp_bitmap = NULL;
    }

    if (virtual_info->vlan_vp_bitmap) {
        sal_free(virtual_info->vlan_vp_bitmap);
        virtual_info->vlan_vp_bitmap = NULL;
    }

    if (virtual_info->niv_vp_bitmap) {
        sal_free(virtual_info->niv_vp_bitmap);
        virtual_info->niv_vp_bitmap = NULL;
    }

    if (virtual_info->l2gre_vfi_bitmap) {
        sal_free(virtual_info->l2gre_vfi_bitmap);
        virtual_info->l2gre_vfi_bitmap = NULL;
    }

    if (virtual_info->l2gre_vp_bitmap) {
        sal_free(virtual_info->l2gre_vp_bitmap);
        virtual_info->l2gre_vp_bitmap = NULL;
    }

    if (virtual_info->vxlan_vfi_bitmap) {
        sal_free(virtual_info->vxlan_vfi_bitmap);
        virtual_info->vxlan_vfi_bitmap = NULL;
    }

    if (virtual_info->vxlan_vp_bitmap) {
        sal_free(virtual_info->vxlan_vp_bitmap);
        virtual_info->vxlan_vp_bitmap = NULL;
    }

    if (virtual_info->extender_vp_bitmap) {
        sal_free(virtual_info->extender_vp_bitmap);
        virtual_info->extender_vp_bitmap = NULL;
    }

    if (virtual_info->vp_lag_vp_bitmap) {
        sal_free(virtual_info->vp_lag_vp_bitmap);
        virtual_info->vp_lag_vp_bitmap = NULL;
    }
    if (virtual_info->vp_shared_vp_bitmap) {
        sal_free(virtual_info->vp_shared_vp_bitmap);
        virtual_info->vp_shared_vp_bitmap = NULL;
    }
    if (virtual_info->vp_network_vp_bitmap) {
        sal_free(virtual_info->vp_network_vp_bitmap);
        virtual_info->vp_network_vp_bitmap = NULL;
    }
    if (virtual_info->vlan_vfi_bitmap) {
        sal_free(virtual_info->vlan_vfi_bitmap);
        virtual_info->vlan_vfi_bitmap = NULL;
    }

    if (virtual_info->flow_vfi_bitmap) {
        sal_free(virtual_info->flow_vfi_bitmap);
        virtual_info->flow_vfi_bitmap = NULL;
    }

    if (virtual_info->flow_vp_bitmap) {
        sal_free(virtual_info->flow_vp_bitmap);
        virtual_info->flow_vp_bitmap = NULL;
    }

    if (virtual_info->vp_to_vpn) {
        sal_free(virtual_info->vp_to_vpn);
        virtual_info->vp_to_vpn = NULL;
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
#define BCM_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_WB_VERSION_1_9                SOC_SCACHE_VERSION(1,9)
#define BCM_WB_VERSION_1_10               SOC_SCACHE_VERSION(1,10)
#define BCM_WB_VERSION_1_11               SOC_SCACHE_VERSION(1,11)
#define BCM_WB_VERSION_1_12               SOC_SCACHE_VERSION(1,12)
#define BCM_WB_VERSION_1_13               SOC_SCACHE_VERSION(1,13)
#define BCM_WB_VERSION_1_14               SOC_SCACHE_VERSION(1,14)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_14

/*
 * Function:
 *      _bcm_esw_virtual_sync
 * Purpose:
 *      - Copy the s/w state of 'Virtual' module to external
 *        memory.
 *      - Not called during Warm Boot.
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_virtual_sync(int unit)
{
    int                 stable_size, alloc_sz = 0;
    int                 num_vp, num_vfi;
    uint8               *virtual_bitmap;
    soc_scache_handle_t scache_handle;  /* SCache reference number        */
#if defined(BCM_SABER2_SUPPORT)
    uint32     source_vp_sb2_5626x = 0xFED5ABC7;
#endif /* BCM_SABER2_SUPPORT */

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((0 == stable_size) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    if ((!SOC_MEM_IS_VALID(unit, VFIm)) ||
        (!SOC_MEM_IS_VALID(unit, SOURCE_VPm))) {
        /* ===============================================       */
        /* Either VFI not present or SOURCE_VP not present.      */
        /* Possible with some chips(hurricane) so not continuing */
        /* ===============================================       */

        /* if (SOC_IS_HURRICANE(unit)) */
        return BCM_E_NONE;
    }

    num_vfi = soc_mem_index_count(unit, VFIm);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    alloc_sz += SHR_BITALLOCSIZE(num_vp);
    alloc_sz += SHR_BITALLOCSIZE(num_vfi);
    /* mpls vp */
    alloc_sz += SHR_BITALLOCSIZE(num_vp);
    /* mpls vfi */
    alloc_sz += SHR_BITALLOCSIZE(num_vfi);
    /* mim vp */
    alloc_sz += SHR_BITALLOCSIZE(num_vp);
    /* mim vfi */
    alloc_sz += SHR_BITALLOCSIZE(num_vfi);
    /* subport vp */
    alloc_sz += SHR_BITALLOCSIZE(num_vp);
    /* vlan vp */
    if (soc_feature(unit, soc_feature_vlan_vp)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
    }
    /* niv vp */
    if (soc_feature(unit, soc_feature_niv)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
    }
    /* trill vp */
    if (soc_feature(unit, soc_feature_trill)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
    }
    /* l2gre vp */
    if (soc_feature(unit, soc_feature_l2gre)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
        /* l2gre vfi */
        alloc_sz += SHR_BITALLOCSIZE(num_vfi);
    }
    /* extender vp */
    if (soc_feature(unit, soc_feature_port_extension)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
    }
    /* vp lag vp */
    if (soc_feature(unit, soc_feature_vp_lag)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
    }

    /* vxlan vfi & vp */
    if (soc_feature(unit, soc_feature_vxlan)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vfi); 
        alloc_sz += SHR_BITALLOCSIZE(num_vp); 
    } 

    /* Shared vp */
    if (soc_feature(unit, soc_feature_vp_sharing)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vp); 
    } 

    /* Network vp bitmap */
    alloc_sz += SHR_BITALLOCSIZE(num_vp); 

    /*vlan vfi*/
    if (soc_feature(unit, soc_feature_vlan_vfi)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vfi);
    }

    /* flow vfi & vp */
    if (soc_feature(unit, soc_feature_flex_flow)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vfi);
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
    }

    /* Use the same bitmap as shared access VP, if it was allocated,
     * DO NOT allocate it again.
     */
    if ((!soc_feature(unit, soc_feature_vp_sharing)) &&
        soc_feature(unit, soc_feature_nexthop_share_dvp)) {
        alloc_sz += SHR_BITALLOCSIZE(num_vp); 
    }

    /* Maximum virtual port supported on saber2 5626x devices are 4K
     * This fix is related to scaling down of vpn from 8K to 4K
     */
#if defined(BCM_SABER2_SUPPORT)
        if (soc_sb2_5626x_devtype(unit)) {
            alloc_sz += sizeof(source_vp_sb2_5626x);
        }
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    if ((soc_feature(unit, soc_feature_vlan_vp) &&
        soc_feature(unit, soc_feature_vp_lag)) ||
        soc_feature(unit, soc_feature_vlan_vp_sync_and_recover)) {
        int vlan_vp_scache_size = 0;
        BCM_IF_ERROR_RETURN(
            _bcm_td2_vlan_vp_scache_size_get(unit, &vlan_vp_scache_size));

        /* VLAN VP Info */
        alloc_sz += vlan_vp_scache_size;
    }

    if (soc_feature(unit, soc_feature_vp_lag)) {
        int egr_dis_vp_scache_size = 0;
        BCM_IF_ERROR_RETURN(
            _bcm_td2_vp_lag_egr_dis_vp_scache_size_get(unit, &egr_dis_vp_scache_size));
        alloc_sz += egr_dis_vp_scache_size;
    }
#endif

    SOC_SCACHE_HANDLE_SET(scache_handle,
                          unit, BCM_MODULE_VIRTUAL, 0);

    if (stable_size > alloc_sz) {
        SOC_IF_ERROR_RETURN (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                             alloc_sz, &virtual_bitmap, 
                             BCM_WB_DEFAULT_VERSION, NULL));
        
        VIRTUAL_LOCK(unit);
        /* Maximum virtual port supported on saber2 5626x devices are 4K
         * This fix is related to scaling down of vpn from 8K to 4K
         */
#if defined(BCM_SABER2_SUPPORT)
        if (soc_sb2_5626x_devtype(unit)) {
            sal_memcpy(virtual_bitmap, &source_vp_sb2_5626x,
                    sizeof(source_vp_sb2_5626x));
            virtual_bitmap += sizeof(source_vp_sb2_5626x);
        }
#endif /* BCM_SABER2_SUPPORT */
        sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vp_bitmap,
                   SHR_BITALLOCSIZE(num_vp));
        virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vfi_bitmap,
                   SHR_BITALLOCSIZE(num_vfi));
        virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
        sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->mpls_vp_bitmap,
                   SHR_BITALLOCSIZE(num_vp));
        virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->mpls_vfi_bitmap,
                   SHR_BITALLOCSIZE(num_vfi));
        virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
        sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->mim_vp_bitmap,
                   SHR_BITALLOCSIZE(num_vp));
        virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->mim_vfi_bitmap,
                   SHR_BITALLOCSIZE(num_vfi));
        virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
        sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->subport_vp_bitmap,
                   SHR_BITALLOCSIZE(num_vp));
        virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        
        if (soc_feature(unit, soc_feature_vlan_vp)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vlan_vp_bitmap, 
                       SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

        if (soc_feature(unit, soc_feature_niv)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->niv_vp_bitmap,
                           SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

        if (soc_feature(unit, soc_feature_trill)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->trill_vp_bitmap,
                          SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

        if (soc_feature(unit, soc_feature_l2gre)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->l2gre_vp_bitmap,
                          SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

        if (soc_feature(unit, soc_feature_port_extension)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->extender_vp_bitmap,
                           SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

        if (soc_feature(unit, soc_feature_vp_lag)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vp_lag_vp_bitmap,
                           SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

        /* 
         *  BCM_WB_VERSION_1_3
         */

        /* Store VXLAN vfi/vp BITMAP */
        /* 
         *  VXLAN vfi/vp BITMAP points to a struct array with a base type of uint32.
         *  sized to hold the maximum number of VFI / VP
         * 
         *  uint32[(size of (SOURCE_VPm) + 32) / 32]     *vxlan_ip_tnl_bitmap
         */

        if (soc_feature(unit, soc_feature_vxlan)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vxlan_vfi_bitmap,
                           SHR_BITALLOCSIZE(num_vfi));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);

            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vxlan_vp_bitmap,
                           SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

        /* 
         *  BCM_WB_VERSION_1_4
         */

        /* Store shared VP */ 
        if (soc_feature(unit, soc_feature_vp_sharing)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vp_shared_vp_bitmap,
                           SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

        /*
         * BCM_WB_VERSION_1_5
         * vp_network_vp_bitmap
         * Bit-map that specifies if the VP belongs to network-side port
         * Used for VxLAN/L2GRE/TRILL
         */
        sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vp_network_vp_bitmap,
                       SHR_BITALLOCSIZE(num_vp));
        virtual_bitmap += SHR_BITALLOCSIZE(num_vp);

        /*
         *  BCM_WB_VERSION_1_6
         */

        /* Store VLAN VFI */
        if (soc_feature(unit, soc_feature_vlan_vfi)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vlan_vfi_bitmap,
                           SHR_BITALLOCSIZE(num_vfi));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
        }

        /*
         *  BCM_WB_VERSION_1_7
         */
        /* reuse the shared access vp bitmap to save the shared network dvp*/
        if ((!soc_feature(unit, soc_feature_vp_sharing)) &&
            soc_feature(unit, soc_feature_nexthop_share_dvp)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->vp_shared_vp_bitmap,
                           SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

        /* BCM_WB_VERSION_1_10 */
        /* Store Vlan Vp Info */
#ifdef BCM_TRIDENT2_SUPPORT
        if ((soc_feature(unit, soc_feature_vlan_vp) &&
             soc_feature(unit, soc_feature_vp_lag)) ||
             soc_feature(unit, soc_feature_vlan_vp_sync_and_recover)) {
            int rv = BCM_E_NONE;

            rv = _bcm_td2_vlan_vp_sync(unit, &virtual_bitmap);
            if (BCM_FAILURE(rv)) {
                VIRTUAL_UNLOCK(unit);
                return rv;
            }
        }
#endif

        if (soc_feature(unit, soc_feature_l2gre)) {
            sal_memcpy(virtual_bitmap,
                    VIRTUAL_INFO(unit)->l2gre_vfi_bitmap,
                    SHR_BITALLOCSIZE(num_vfi));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
        }

        /* BCM_WB_VERSION_1_13 */
        if (soc_feature(unit, soc_feature_flex_flow)) {
            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->flow_vfi_bitmap,
                           SHR_BITALLOCSIZE(num_vfi));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);

            sal_memcpy(virtual_bitmap, VIRTUAL_INFO(unit)->flow_vp_bitmap,
                           SHR_BITALLOCSIZE(num_vp));
            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
        }

#if defined(BCM_TRIDENT2_SUPPORT)
        /* BCM_EB_VERSION_1_14 */
        if (soc_feature(unit, soc_feature_vp_lag)) {
            int rv = BCM_E_NONE;
            rv = _bcm_td2_vp_lag_egr_dis_vp_sync(unit, &virtual_bitmap);
            if (BCM_FAILURE(rv)) {
                VIRTUAL_UNLOCK(unit);
                return rv;
            }
        }

#endif /* BCM_TRIDENT2_SUPPORT  */
        VIRTUAL_UNLOCK(unit);
 
    }

    return BCM_E_NONE;
}

/* 
 * Function:
 *      _bcm_vp_info_hw_recover
 * Purpose: 
 *     Reconstruct the following VP info from h/w by
 *     traversing the source VP table:
 *     - Network VP bitmap
 *
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_vp_info_hw_recover (int unit)
{
    source_vp_entry_t *svp_entry; 
    int chnk_idx, ent_idx, chnk_idx_max, mem_idx_min, mem_idx_max;
    int buf_size, chunksize, chnk_end;
    uint32 *tbl_chnk;
    soc_mem_t mem = SOURCE_VPm;
    int rv = BCM_E_NONE;
    int is_network_port = FALSE;

    chunksize = 1024;
    buf_size = sizeof(source_vp_entry_t) * chunksize;
    tbl_chnk = soc_cm_salloc(unit, buf_size, "source_vp traverse");
    if (NULL == tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_min = soc_mem_index_min(unit, mem);
    mem_idx_max = soc_mem_index_max(unit, mem);

    for (chnk_idx = mem_idx_min;
         chnk_idx <= mem_idx_max;
         chnk_idx += chunksize) {
        sal_memset((void *)tbl_chnk, 0, buf_size);

        chnk_idx_max =
            ((chnk_idx + chunksize) <= mem_idx_max) ?
            chnk_idx + chunksize - 1: mem_idx_max;

        soc_mem_lock(unit, mem);
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, tbl_chnk);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            soc_cm_sfree(unit, tbl_chnk);
            return rv;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            svp_entry = soc_mem_table_idx_to_pointer(unit, mem, 
                                             source_vp_entry_t *,
                                             tbl_chnk, ent_idx);
            if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
                is_network_port =
                    soc_SOURCE_VPm_field32_get(unit, svp_entry, NETWORK_GROUPf);
            } else {
                is_network_port =
                    soc_SOURCE_VPm_field32_get(unit, svp_entry, NETWORK_PORTf);
            }
            if (is_network_port) {
                _BCM_NETWORK_VP_USED_SET(unit, ent_idx+chnk_idx);
            }
        }
        soc_mem_unlock(unit, mem);
    }

    soc_cm_sfree(unit, tbl_chnk);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vp_vpn_hw_recover
 * Purpose:
 *     Reconstruct the VP to VPN map from h/w by
 *     traversing the source VP table
 *
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_vp_vpn_hw_recover(int unit)
{
    source_vp_entry_t *svp_entry;
    int chnk_idx, ent_idx, chnk_idx_max, mem_idx_min, mem_idx_max;
    int buf_size, chunksize, chnk_end;
    uint32 *tbl_chnk;
    soc_mem_t mem = SOURCE_VPm;
    int rv = BCM_E_NONE;
    uint32 vfi = 0;
    bcm_vpn_t vpn = 0;

    chunksize = 1024;
    buf_size = sizeof(source_vp_entry_t) * chunksize;
    tbl_chnk = soc_cm_salloc(unit, buf_size, "source_vp traverse");
    if (NULL == tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_min = soc_mem_index_min(unit, mem);
    mem_idx_max = soc_mem_index_max(unit, mem);

    for (chnk_idx = mem_idx_min;
         chnk_idx <= mem_idx_max;
         chnk_idx += chunksize) {
        sal_memset((void *)tbl_chnk, 0, buf_size);

        chnk_idx_max =
            ((chnk_idx + chunksize) <= mem_idx_max) ?
            chnk_idx + chunksize - 1: mem_idx_max;

        soc_mem_lock(unit, mem);
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, tbl_chnk);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            soc_cm_sfree(unit, tbl_chnk);
            return rv;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            svp_entry = soc_mem_table_idx_to_pointer(unit, mem,
                                                     source_vp_entry_t *,
                                                     tbl_chnk, ent_idx);
            /* Only record VPN for MPLS VPLS*/
            if (0x1 ==
                soc_SOURCE_VPm_field32_get(unit, svp_entry, ENTRY_TYPEf)) {
                vfi = soc_SOURCE_VPm_field32_get(unit, svp_entry, VFIf);
                if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
                    _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
                    _bcm_vp_vfi_set(unit, ent_idx+chnk_idx, vpn);
                }
            }
        }
        soc_mem_unlock(unit, mem);
    }

    soc_cm_sfree(unit, tbl_chnk);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_virtual_reinit
 * Purpose:
 *      - Top level reinit func for Virtual module.
 *      - Do nothing for Unsynchronized (level 1) and Limited 
 *        scache (level 1.5) based recovery.
 *      - Extended scache (level 2) based recovery:
 *        a. Cold Boot: Allocate external memory required to store 
 *                 module specific s/w state.
 *        b. Warm Boot: Recover the s/w state from external memory.
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_virtual_reinit(int unit) {

    int rv = BCM_E_NONE;
    int32  stable_size = 0;
    uint32 num_vp, num_vfi;
    uint32 alloc_sz = 0;
    uint8  *virtual_bitmap;
    uint16 recovered_ver;
    uint32  virtual_scache_size;
    soc_scache_handle_t scache_handle;  /* SCache reference number */
    int        old_num_vp = 0;
#if defined(BCM_SABER2_SUPPORT)
    uint32     source_vp_sb2_5626x = 0;
#endif /* BCM_SABER2_SUPPORT */

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((0 == stable_size) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
    /* Unsynchronized or Limited scache support based recovery */
        return rv;
    } else { 
    /* Extended scache support available */

        num_vfi = soc_mem_index_count(unit, VFIm);
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VIRTUAL, 0);
    
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
        alloc_sz += SHR_BITALLOCSIZE(num_vfi);
        /* mpls vp */
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
        /* mpls vfi */
        alloc_sz += SHR_BITALLOCSIZE(num_vfi);
        /* mim vp */
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
        /* mim vfi */
        alloc_sz += SHR_BITALLOCSIZE(num_vfi);
        /* subport vp */
        alloc_sz += SHR_BITALLOCSIZE(num_vp);
        /* vlan vp */
        if (soc_feature(unit, soc_feature_vlan_vp)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vp);
        }
        /* niv vp */
        if (soc_feature(unit, soc_feature_niv)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vp);
        }
        /* trill vp */
        if (soc_feature(unit, soc_feature_trill)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vp);
        }
        /* l2gre vp */
        if (soc_feature(unit, soc_feature_l2gre)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vp);
            alloc_sz += SHR_BITALLOCSIZE(num_vfi);
        }
        /* extender vp */
        if (soc_feature(unit, soc_feature_port_extension)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vp);
        }
        /* vp lag vp */
        if (soc_feature(unit, soc_feature_vp_lag)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vp);
        }
        /* vxlan vfi & vp */
        if (soc_feature(unit, soc_feature_vxlan)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vfi); 
            alloc_sz += SHR_BITALLOCSIZE(num_vp); 
        }  

        /* flow vfi  & vp */
        if (soc_feature(unit, soc_feature_flex_flow)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vfi);
            alloc_sz += SHR_BITALLOCSIZE(num_vp);
        }
        
        /* Shared vp */
        if (soc_feature(unit, soc_feature_vp_sharing)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vp); 
        } 
        
        /* Network vp */
        alloc_sz += SHR_BITALLOCSIZE(num_vp); 

        /*vlan vfi*/
        if (soc_feature(unit, soc_feature_vlan_vfi)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vfi);
        }

        /* Use the same bitmap as shared access VP, if it was allocated,
         * DO NOT allocate it again.
         */
        if ((!soc_feature(unit, soc_feature_vp_sharing)) &&
            soc_feature(unit, soc_feature_nexthop_share_dvp)) {
            alloc_sz += SHR_BITALLOCSIZE(num_vp); 
        }

        /* Maximum virtual port supported on saber2 5626x devices are 4K
         * This fix is related to scaling down of vpn from 8K to 4K
         */
#if defined(BCM_SABER2_SUPPORT)
        if (soc_sb2_5626x_devtype(unit)) {
            alloc_sz += sizeof(source_vp_sb2_5626x);
        }
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
        if ((soc_feature(unit, soc_feature_vlan_vp) &&
             soc_feature(unit, soc_feature_vp_lag)) ||
             soc_feature(unit, soc_feature_vlan_vp_sync_and_recover)) {
            int vlan_vp_scache_size = 0;
            BCM_IF_ERROR_RETURN(
                _bcm_td2_vlan_vp_scache_size_get(unit, &vlan_vp_scache_size));

            /* VLAN VP Info */
            alloc_sz += vlan_vp_scache_size;
        }

        if (soc_feature(unit, soc_feature_vp_lag)) {
            int vp_lag_scache_size = 0;;
            BCM_IF_ERROR_RETURN(_bcm_td2_vp_lag_egr_dis_vp_scache_size_get(unit,
                                    &vp_lag_scache_size));
            alloc_sz += vp_lag_scache_size;
        }
#endif

        if (SOC_WARM_BOOT(unit)) {
        /* Warm Boot */
            if (stable_size > alloc_sz) {
                SOC_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle,
                                    FALSE, alloc_sz, &virtual_bitmap, 
                                    BCM_WB_DEFAULT_VERSION, &recovered_ver));
                /* Maximum virtual port supported on saber2 5626x devices are 4K
                 * This fix is related to scaling down of vpn from 8K to 4K
                 */
#if defined(BCM_SABER2_SUPPORT)
                if (soc_sb2_5626x_devtype(unit)) {
                    sal_memcpy(&source_vp_sb2_5626x, virtual_bitmap,
                            sizeof(source_vp_sb2_5626x));
                    if (source_vp_sb2_5626x == 0xFED5ABC7) {
                        virtual_bitmap += sizeof(source_vp_sb2_5626x);
                    } else {
                        old_num_vp = 1;
                    }
                }
#endif /* BCM_SABER2_SUPPORT */
                sal_memcpy(VIRTUAL_INFO(unit)->vp_bitmap, virtual_bitmap,
                          SHR_BITALLOCSIZE(num_vp));
                /* coverity[dead_error_condition : FALSE] */
                if (old_num_vp) {
                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                } else {
                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                }
                sal_memcpy(VIRTUAL_INFO(unit)->vfi_bitmap, virtual_bitmap,
                          SHR_BITALLOCSIZE(num_vfi));
                virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
                sal_memcpy(VIRTUAL_INFO(unit)->mpls_vp_bitmap, virtual_bitmap,
                          SHR_BITALLOCSIZE(num_vp));
                /* coverity[dead_error_condition : FALSE] */
                if (old_num_vp) {
                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                } else {
                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                }
                sal_memcpy(VIRTUAL_INFO(unit)->mpls_vfi_bitmap, virtual_bitmap,
                          SHR_BITALLOCSIZE(num_vfi));
                virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
                sal_memcpy(VIRTUAL_INFO(unit)->mim_vp_bitmap, virtual_bitmap,
                          SHR_BITALLOCSIZE(num_vp));
                /* coverity[dead_error_condition : FALSE] */
                if (old_num_vp) {
                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                } else {
                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                }
                sal_memcpy(VIRTUAL_INFO(unit)->mim_vfi_bitmap, virtual_bitmap,
                          SHR_BITALLOCSIZE(num_vfi));
                virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
                sal_memcpy(VIRTUAL_INFO(unit)->subport_vp_bitmap, virtual_bitmap,
                          SHR_BITALLOCSIZE(num_vp));
                /* coverity[dead_error_condition : FALSE] */
                if (old_num_vp) {
                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                } else {
                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                }

                virtual_scache_size = 0;
                if (recovered_ver >= BCM_WB_VERSION_1_1) {
                    if (soc_feature(unit, soc_feature_vlan_vp)) {
                            sal_memcpy(VIRTUAL_INFO(unit)->vlan_vp_bitmap, virtual_bitmap,
                                      SHR_BITALLOCSIZE(num_vp));
                            /* coverity[dead_error_condition : FALSE] */
                            if (old_num_vp) {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                            } else {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                            }
                    }

                    if (soc_feature(unit, soc_feature_niv)) {
#ifdef BCM_KATANA2_SUPPORT
                        if (recovered_ver < BCM_WB_VERSION_1_9) {
                            if (!SOC_IS_KATANA2(unit)) {
                                sal_memcpy(VIRTUAL_INFO(unit)->niv_vp_bitmap,
                                           virtual_bitmap,
                                           SHR_BITALLOCSIZE(num_vp));
                                /* coverity[dead_error_condition : FALSE] */
                                if (old_num_vp) {
                                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                                } else {
                                    virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                                }
                            }
                        } else
#endif /* BCM_KATANA2_SUPPORT */
                        {
                            sal_memcpy(VIRTUAL_INFO(unit)->niv_vp_bitmap,
                                       virtual_bitmap,
                                       SHR_BITALLOCSIZE(num_vp));
                            /* coverity[dead_error_condition : FALSE] */
                            if (old_num_vp) {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                            } else {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                            }
                        }
                    }

                    if (soc_feature(unit, soc_feature_trill)) {
                            sal_memcpy(VIRTUAL_INFO(unit)->trill_vp_bitmap, virtual_bitmap,
                                      SHR_BITALLOCSIZE(num_vp));
                            /* coverity[dead_error_condition : FALSE] */
                            if (old_num_vp) {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                            } else {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                            }
                    }
                } else {
                    if (soc_feature(unit, soc_feature_vlan_vp)) {
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp);
                        }
                    }
                    if (soc_feature(unit, soc_feature_niv)) {
                        if (!SOC_IS_KATANA2(unit)) {
                            /* coverity[dead_error_condition : FALSE] */
                            if (old_num_vp) {
                                virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                            } else {
                                virtual_scache_size += SHR_BITALLOCSIZE(num_vp); 
                            }
                        }
                    }
                    if (soc_feature(unit, soc_feature_trill)) {
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp);
                        }
                    }
                }

                if (recovered_ver >= BCM_WB_VERSION_1_2) {
                    if (soc_feature(unit, soc_feature_l2gre)) {
                            sal_memcpy(VIRTUAL_INFO(unit)->l2gre_vp_bitmap, virtual_bitmap,
                                       SHR_BITALLOCSIZE(num_vp));
                            /* coverity[dead_error_condition : FALSE] */
                            if (old_num_vp) {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                            } else {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                            }

                            if (recovered_ver == BCM_WB_VERSION_1_11) {
                                sal_memcpy(VIRTUAL_INFO(unit)->l2gre_vfi_bitmap, virtual_bitmap,
                                        SHR_BITALLOCSIZE(num_vfi));
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
                            }
                    }
                    if (soc_feature(unit, soc_feature_port_extension)) {
                            sal_memcpy(VIRTUAL_INFO(unit)->extender_vp_bitmap, virtual_bitmap,
                                      SHR_BITALLOCSIZE(num_vp));
                            /* coverity[dead_error_condition : FALSE] */
                            if (old_num_vp) {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                            } else {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                            }
                    }
                    if (soc_feature(unit, soc_feature_vp_lag)) {
                            sal_memcpy(VIRTUAL_INFO(unit)->vp_lag_vp_bitmap, virtual_bitmap,
                                      SHR_BITALLOCSIZE(num_vp));
                            /* coverity[dead_error_condition : FALSE] */
                            if (old_num_vp) {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                            } else {
                                virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                            }
                    }
                } else {
                    if (soc_feature(unit, soc_feature_l2gre)) {
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp);
                        }
                    }
                    if (soc_feature(unit, soc_feature_port_extension)) {
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp); 
                        }
                    }
                    if (soc_feature(unit, soc_feature_vp_lag)) {
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp); 
                        }
                    }
                }

                if (recovered_ver >= BCM_WB_VERSION_1_3) {
                    if (soc_feature(unit, soc_feature_vxlan)) {
                        if (recovered_ver >= BCM_WB_VERSION_1_8) {
                            sal_memcpy(VIRTUAL_INFO(unit)->vxlan_vfi_bitmap, virtual_bitmap,
                                       SHR_BITALLOCSIZE(num_vfi));
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
                        } else {
                            /* When the recovered version smaller than BCM_WB_VERSION_1_8,
                             * the store was incorrect,  adjust the offset to recover the state */
                            sal_memcpy(VIRTUAL_INFO(unit)->vxlan_vfi_bitmap,
                            virtual_bitmap - (SHR_BITALLOCSIZE(num_vp) - SHR_BITALLOCSIZE(num_vfi)),
                                       SHR_BITALLOCSIZE(num_vfi));
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
                        }
                        sal_memcpy(VIRTUAL_INFO(unit)->vxlan_vp_bitmap, virtual_bitmap,
                                   SHR_BITALLOCSIZE(num_vp));
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                        }

                    }
                } else {
                    if (soc_feature(unit, soc_feature_vxlan)) {
                        virtual_scache_size += SHR_BITALLOCSIZE(num_vfi);
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp); 
                        }
                    }
                } 
                
                if (recovered_ver >= BCM_WB_VERSION_1_4) {
                    if (soc_feature(unit, soc_feature_vp_sharing)) {
                        sal_memcpy(VIRTUAL_INFO(unit)->vp_shared_vp_bitmap, virtual_bitmap,
                                   SHR_BITALLOCSIZE(num_vp));
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                        }
                    }
                } else {
                    if (soc_feature(unit, soc_feature_vp_sharing)) {
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp); 
                        }
                    }
                } 

                if (recovered_ver >= BCM_WB_VERSION_1_5) {
                    sal_memcpy(VIRTUAL_INFO(unit)->vp_network_vp_bitmap, virtual_bitmap,
                            SHR_BITALLOCSIZE(num_vp));
                    /* coverity[dead_error_condition : FALSE] */
                    if (old_num_vp) {
                        virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                    } else {
                        virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                    }
                } else {
                    /* 
                     * When the recovered version doesn't have vp_network_vp_bitmap 
                     * in scache, recover the state from hw by reading 
                     * source_vp table.
                     */
                    BCM_IF_ERROR_RETURN(_bcm_vp_info_hw_recover(unit));
                    /* coverity[dead_error_condition : FALSE] */
                    if (old_num_vp) {
                        virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                    } else {
                        virtual_scache_size += SHR_BITALLOCSIZE(num_vp); 
                    }
                }

                if (recovered_ver >= BCM_WB_VERSION_1_6) {
                    if (soc_feature(unit, soc_feature_vlan_vfi)) {
                        if (recovered_ver >= BCM_WB_VERSION_1_8) {
                            sal_memcpy(VIRTUAL_INFO(unit)->vlan_vfi_bitmap, virtual_bitmap,
                                       SHR_BITALLOCSIZE(num_vfi));
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
                        } else {
                            /* When the recovered version smaller than BCM_WB_VERSION_1_8,
                             * the store was incorrect,  adjust the offset to recover the state */
                            sal_memcpy(VIRTUAL_INFO(unit)->vlan_vfi_bitmap, 
                            virtual_bitmap - (SHR_BITALLOCSIZE(num_vp) - SHR_BITALLOCSIZE(num_vfi)),
                                       SHR_BITALLOCSIZE(num_vfi));
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
                        }
                    }
                } else {
                    if (soc_feature(unit, soc_feature_vlan_vfi)) {
                        virtual_scache_size += SHR_BITALLOCSIZE(num_vfi);
                    }
                } 

                if (recovered_ver >= BCM_WB_VERSION_1_7) {
                    if ((!soc_feature(unit, soc_feature_vp_sharing)) &&
                        soc_feature(unit, soc_feature_nexthop_share_dvp)) {
                        sal_memcpy(VIRTUAL_INFO(unit)->vp_shared_vp_bitmap, virtual_bitmap,
                                   SHR_BITALLOCSIZE(num_vp));
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                        }
                    }
                } else {
                    if ((!soc_feature(unit, soc_feature_vp_sharing)) &&
                        soc_feature(unit, soc_feature_nexthop_share_dvp)) {
                        /* coverity[dead_error_condition : FALSE] */
                        if (old_num_vp) {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp); 
                        }
                    }
                }

#ifdef BCM_TRIDENT2_SUPPORT
                if ((soc_feature(unit, soc_feature_vlan_vp) &&
                     soc_feature(unit, soc_feature_vp_lag)) ||
                     soc_feature(unit, soc_feature_vlan_vp_sync_and_recover)) {
                    if (recovered_ver >= BCM_WB_VERSION_1_10) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_td2_vlan_vp_recover(unit, &virtual_bitmap));
                    } else {
                        int vlan_vp_size = 0;
                        BCM_IF_ERROR_RETURN(
                            _bcm_td2_vlan_vp_scache_size_get(unit, &vlan_vp_size));

                        virtual_scache_size += vlan_vp_size;
                    }
                }
#endif

                /*Due to SDK-93047 changes downgrade is broken.Always add new changes
                 * in the last in scache layout*/
                if (recovered_ver >= BCM_WB_VERSION_1_12) {
                    if (soc_feature(unit, soc_feature_l2gre)) {
                        sal_memcpy(VIRTUAL_INFO(unit)->l2gre_vfi_bitmap,
                                virtual_bitmap,
                                SHR_BITALLOCSIZE(num_vfi));
                        virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);
                    }
                } else {
                    virtual_scache_size += SHR_BITALLOCSIZE(num_vfi);
                }

                if (recovered_ver >= BCM_WB_VERSION_1_13) {
                    if (soc_feature(unit, soc_feature_flex_flow)) {
                        sal_memcpy(VIRTUAL_INFO(unit)->flow_vfi_bitmap, virtual_bitmap,
                                SHR_BITALLOCSIZE(num_vfi));
                        virtual_bitmap += SHR_BITALLOCSIZE(num_vfi);

                        sal_memcpy(VIRTUAL_INFO(unit)->flow_vp_bitmap, virtual_bitmap,
                                   SHR_BITALLOCSIZE(num_vp));
                        if (old_num_vp) {
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_bitmap += SHR_BITALLOCSIZE(num_vp);
                        }
                    }
                } else {
                    if (soc_feature(unit, soc_feature_flex_flow)) {
                        virtual_scache_size += SHR_BITALLOCSIZE(num_vfi);
                        if (old_num_vp) {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
                        } else {
                            virtual_scache_size += SHR_BITALLOCSIZE(num_vp); 
                        }
                    }
                }

#if defined(BCM_TRIDENT2_SUPPORT)
                if (soc_feature(unit, soc_feature_vp_lag)) {
                    if (recovered_ver >= BCM_WB_VERSION_1_14) {
                        /* recover member VPs with BCM_TRUNK_MEMBER_EGRESS_DISABLE flag */
                        BCM_IF_ERROR_RETURN(_bcm_td2_vp_lag_egr_dis_vp_scache_recover(unit,
                                                &virtual_bitmap));
                    } else {
                        int vp_lag_scache_size = 0;
                        BCM_IF_ERROR_RETURN(_bcm_td2_vp_lag_egr_dis_vp_scache_size_get(unit,
                                                &vp_lag_scache_size));
                        virtual_scache_size += vp_lag_scache_size;
                    }
                }
#endif /* BCM_TRIDENT2_SUPPORT */

                if (virtual_scache_size > 0) {
                    SOC_IF_ERROR_RETURN
                        (soc_scache_realloc(unit, scache_handle, virtual_scache_size));
                }

            } else {
                rv = BCM_E_INTERNAL; /* stable_size < alloc_size */
            }

            /* VP to VPN */
            BCM_IF_ERROR_RETURN(_bcm_vp_vpn_hw_recover(unit));
        } else {
        /* Cold Boot */
            if (stable_size > alloc_sz) {
                rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                             alloc_sz, &virtual_bitmap, 
                                             BCM_WB_DEFAULT_VERSION, NULL);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    return rv;
                }
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_vlan_vp_count_recover
 * Purpose:
 *      Recover the number of VLAN virtual ports used by each trunk
 *      in the unit.
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_vlan_vp_count_recover(int unit)
{
    uint32 num_vp, vp;
    ing_dvp_table_entry_t dvp_entry;
    ing_l3_next_hop_entry_t ing_nh_entry;
    uint32 nh_index = -1;
    bcm_l3_egress_t nh;

    bcm_l3_egress_t_init(&nh);

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    SHR_BIT_ITER(VIRTUAL_INFO(unit)->vlan_vp_bitmap, num_vp, vp) {
        BCM_IF_ERROR_RETURN
            (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));

        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
            NEXT_HOP_INDEXf);

        BCM_IF_ERROR_RETURN
            (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index,
            &ing_nh_entry));

        /* determine if it points to a trunk */
#ifdef BCM_RIOT_SUPPORT
        if (soc_feature(unit, soc_feature_ing_l3_next_hop_encoded_dest))
        {
            uint32 nh_dest = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                &ing_nh_entry, DESTINATIONf);
            bcmi_get_port_from_destination(unit, nh_dest, &nh);
        } else
#endif /* BCM_RIOT_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_generic_dest)) {
            uint32 nh_dest = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                &ing_nh_entry, DESTINATIONf);
            bcmi_td3_get_port_from_destination(unit, nh_dest, &nh);
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        if (soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh_entry, Tf))
        {
            nh.flags |= BCM_L3_TGID;
            nh.port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                &ing_nh_entry, TGIDf);
        }
        if (nh.flags & BCM_L3_TGID) {
            trunk_private_t *t_info;
            t_info = _bcm_esw_trunk_info_get(unit, nh.port);
            t_info->vlan_vp_count++;
        }
    }
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_vp_info_init
 * Purpose:
 *      Internal function to initialize _bcm_vp_info_t structure
 * Parameters:
 *      _bcm_vp_info_t (IN/OUT) variable
 * Returns:
 *      void
 */
void
_bcm_vp_info_init (_bcm_vp_info_t *vp_info)
{
    if (vp_info != NULL) {
        sal_memset(vp_info, 0, sizeof (*vp_info));
    }
    return;
}

int _bcm_vlan_xlate_dummy_vp_init(int unit, int index)
{
    source_vp_entry_t svp;
    int min_vp, max_vp;

    min_vp = soc_mem_index_min(unit, SOURCE_VPm);
    max_vp = soc_mem_index_max(unit, SOURCE_VPm);

    if ((index < min_vp) || (index > max_vp)) {
        return BCM_E_PARAM;
    }

    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    soc_SOURCE_VPm_field32_set(unit, &svp, ENTRY_TYPEf, 7);
    soc_SOURCE_VPm_field32_set(unit, &svp, IPV4L3_ENABLEf, 1);
    soc_SOURCE_VPm_field32_set(unit, &svp, IPV6L3_ENABLEf, 1);
    soc_SOURCE_VPm_field32_set(unit, &svp, ENABLE_IFILTERf, 0);
    BCM_IF_ERROR_RETURN(WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, index, &svp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_virtual_init
 * Purpose:
 *      Internal function for initializing virtual resource management
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_virtual_init(int unit, soc_mem_t vp_mem, soc_mem_t vfi_mem)
{
    int num_vfi, num_vp, num_wlan_vp=0, vp, rv = BCM_E_NONE;
    _bcm_virtual_bookkeeping_t *virtual_info = VIRTUAL_INFO(unit);
    _bcm_vp_info_t vp_info;
    int num_bytes_vfi_table, idx, profile_idx;
    uint8 *vfi_mem_buffer;
    vfi_entry_t *vfi_entry;


    num_vfi = soc_mem_index_count(unit, vfi_mem);
    num_vp = soc_mem_index_count(unit, vp_mem);
    if (soc_feature(unit, soc_feature_wlan)) {
        if (SOC_MEM_IS_VALID(unit, WLAN_SVP_TABLEm)) {
            num_wlan_vp = soc_mem_index_count(unit, WLAN_SVP_TABLEm);
        } else {
            num_wlan_vp = soc_mem_index_count(unit, SOURCE_VPm);
        }
    }
    /* 
     * Allocate resources 
     */
    if (NULL == _virtual_mutex[unit]) {
        _virtual_mutex[unit] = sal_mutex_create("virtual mutex");
        if (_virtual_mutex[unit] == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->vfi_bitmap) {
        virtual_info->vfi_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vfi), "vfi_bitmap");
        if (virtual_info->vfi_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->vp_bitmap) {
        virtual_info->vp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vp), "vp_bitmap");
        if (virtual_info->vp_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->mpls_vfi_bitmap) {
        virtual_info->mpls_vfi_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vfi), "mpls_vfi_bitmap");
        if (virtual_info->mpls_vfi_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->mpls_vp_bitmap) {
        virtual_info->mpls_vp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vp), "mpls_vp_bitmap");
        if (virtual_info->mpls_vp_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->mim_vfi_bitmap) {
        virtual_info->mim_vfi_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vfi), "mim_vfi_bitmap");
        if (virtual_info->mim_vfi_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->mim_vp_bitmap) {
        virtual_info->mim_vp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vp), "mim_vp_bitmap");
        if (virtual_info->mim_vp_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->subport_vp_bitmap) {
        virtual_info->subport_vp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vp), "subport_vp_bitmap");
        if (virtual_info->subport_vp_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (soc_feature(unit, soc_feature_wlan)) {
        if (NULL == virtual_info->wlan_vp_bitmap) {
            virtual_info->wlan_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_wlan_vp), "wlan_vp_bitmap");
            if (virtual_info->wlan_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (soc_feature(unit, soc_feature_trill)) {
        if (NULL == virtual_info->trill_vp_bitmap) {
            virtual_info->trill_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vp), "trill_vp_bitmap");
            if (virtual_info->trill_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (soc_feature(unit, soc_feature_vlan_vp)) {
        if (NULL == virtual_info->vlan_vp_bitmap) {
            virtual_info->vlan_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vp), "vlan_vp_bitmap");
            if (virtual_info->vlan_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (soc_feature(unit, soc_feature_niv)) {
        if (NULL == virtual_info->niv_vp_bitmap) {
            virtual_info->niv_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vp), "niv_vp_bitmap");
            if (virtual_info->niv_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (soc_feature(unit, soc_feature_l2gre)) {
        if (NULL == virtual_info->l2gre_vfi_bitmap) {
            virtual_info->l2gre_vfi_bitmap =
                   sal_alloc(SHR_BITALLOCSIZE(num_vfi), "l2gre_vfi_bitmap");
            if (virtual_info->l2gre_vfi_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }
    
    if (soc_feature(unit, soc_feature_l2gre)) {
        if (NULL == virtual_info->l2gre_vp_bitmap) {
            virtual_info->l2gre_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vp), "l2gre_vp_bitmap");
            if (virtual_info->l2gre_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (soc_feature(unit, soc_feature_vxlan)) {
        if (NULL == virtual_info->vxlan_vfi_bitmap) {
            virtual_info->vxlan_vfi_bitmap =
                   sal_alloc(SHR_BITALLOCSIZE(num_vfi), "vxlan_vfi_bitmap");
            if (virtual_info->vxlan_vfi_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }
    
    if (soc_feature(unit, soc_feature_vxlan)) {
        if (NULL == virtual_info->vxlan_vp_bitmap) {
            virtual_info->vxlan_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vp), "vxlan_vp_bitmap");
            if (virtual_info->vxlan_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (soc_feature(unit, soc_feature_port_extension)) {
        if (NULL == virtual_info->extender_vp_bitmap) {
            virtual_info->extender_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vp), "extender_vp_bitmap");
            if (virtual_info->extender_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (soc_feature(unit, soc_feature_vp_lag)) {
        if (NULL == virtual_info->vp_lag_vp_bitmap) {
            virtual_info->vp_lag_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vp), "vp_lag_vp_bitmap");
            if (virtual_info->vp_lag_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (soc_feature(unit, soc_feature_vp_sharing) || 
        soc_feature(unit, soc_feature_nexthop_share_dvp)) {
        if (NULL == virtual_info->vp_shared_vp_bitmap) {
            virtual_info->vp_shared_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vp), "vp_shared_vp_bitmap");
            if (virtual_info->vp_shared_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (NULL == virtual_info->vp_network_vp_bitmap) {
        virtual_info->vp_network_vp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vp), "vp_network_vp_bitmap");
        if (virtual_info->vp_network_vp_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (soc_feature(unit, soc_feature_vlan_vp)) {
        if (NULL == virtual_info->vlan_vfi_bitmap) {
            virtual_info->vlan_vfi_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vfi), "vlan_vfi_bitmap");
            if (virtual_info->vlan_vfi_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (soc_feature(unit, soc_feature_flex_flow)) {
        if (NULL == virtual_info->flow_vfi_bitmap) {
            virtual_info->flow_vfi_bitmap =
                   sal_alloc(SHR_BITALLOCSIZE(num_vfi), "flow_vfi_bitmap");
            if (virtual_info->flow_vfi_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }

        if (NULL == virtual_info->flow_vp_bitmap) {
            virtual_info->flow_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_vp), "flow_vp_bitmap");
            if (virtual_info->flow_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }

    if (NULL == virtual_info->vp_to_vpn) {
        virtual_info->vp_to_vpn =
            sal_alloc(num_vp * sizeof(uint16), "vp_to_vpn_map");
        if (virtual_info->vp_to_vpn == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    /*
     * Initialize 
     */ 
    sal_memset(virtual_info->vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
    sal_memset(virtual_info->vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    sal_memset(virtual_info->mpls_vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
    sal_memset(virtual_info->mpls_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    sal_memset(virtual_info->mim_vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
    sal_memset(virtual_info->mim_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    sal_memset(virtual_info->subport_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    if (soc_feature(unit, soc_feature_wlan)) {
        sal_memset(virtual_info->wlan_vp_bitmap, 0, SHR_BITALLOCSIZE(num_wlan_vp));
    }	
    if (soc_feature(unit, soc_feature_trill)) {
        sal_memset(virtual_info->trill_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    }
    if (soc_feature(unit, soc_feature_vlan_vp)) {
        sal_memset(virtual_info->vlan_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    }
    if (soc_feature(unit, soc_feature_niv)) {
        sal_memset(virtual_info->niv_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    }
    if (soc_feature(unit, soc_feature_l2gre)) {
        sal_memset(virtual_info->l2gre_vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
        sal_memset(virtual_info->l2gre_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    }
    if (soc_feature(unit, soc_feature_vxlan)) {
        sal_memset(virtual_info->vxlan_vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
        sal_memset(virtual_info->vxlan_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    }
    if (soc_feature(unit, soc_feature_flex_flow)) {
        sal_memset(virtual_info->flow_vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
        sal_memset(virtual_info->flow_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    }
    if (soc_feature(unit, soc_feature_port_extension)) {
        sal_memset(virtual_info->extender_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    }
    if (soc_feature(unit, soc_feature_vp_lag)) {
        sal_memset(virtual_info->vp_lag_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    }
    if (soc_feature(unit, soc_feature_vp_sharing) || 
        soc_feature(unit, soc_feature_nexthop_share_dvp)) {
        sal_memset(virtual_info->vp_shared_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    }

    sal_memset(virtual_info->vp_network_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    if (soc_feature(unit, soc_feature_vlan_vfi)) {
        sal_memset(virtual_info->vlan_vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
    }

    sal_memset(virtual_info->vp_to_vpn, 0, num_vp * sizeof(uint16));

    /*enable pruning on split horizon group 1 by default*/
    if (soc_feature(unit, soc_feature_reserve_shg_network_port) &&
        !(soc_property_get(unit, spn_USE_ALL_SPLITHORIZON_GROUPS, 0))) {
        bcm_switch_network_group_config_t network_shg_config;
        sal_memset(&network_shg_config, 0,
                sizeof(bcm_switch_network_group_config_t));
        network_shg_config.dest_network_group_id = 1;
        network_shg_config.config_flags =
            (BCM_SWITCH_NETWORK_GROUP_INGRESS_PRUNE_ENABLE |
             BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE);
        _bcm_switch_network_group_pruning(unit, 1, &network_shg_config);
    }

    /* HW retriction - mark VP index zero as used */
    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeAny;
    rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp);
    if (vp != 0) {
        rv = BCM_E_INTERNAL;
    }
    if (BCM_FAILURE(rv)) {
        _bcm_virtual_free_resource(unit, virtual_info);
        return rv;
    }
    /* HW restriction - mark wlan VP index zero as used */
    /* wlan uses different table for vp management */
    if (soc_feature(unit, soc_feature_wlan)) {
        vp_info.vp_type = _bcmVpTypeWlan;
        rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp);
        if (vp != 0) {
            rv = BCM_E_INTERNAL;
        }
        if (BCM_FAILURE(rv)) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return rv;
        }
    }

    /* Hw restriction - mark SVP 1 used for unknown SVP support on TR2 B0 */
    if (soc_feature(unit, soc_feature_mim_reserve_default_port) &&
        soc_property_get(unit, spn_RESERVE_MIM_DEFAULT_SVP, 0)) {
        /* reserve svp 1 for mim */
        vp_info.vp_type = _bcmVpTypeMim;
        rv = _bcm_vp_alloc(unit, 1, (num_vp - 1), 1, SOURCE_VPm,
                            vp_info, &vp);
        if (vp != _BCM_MIM_DEFAULT_PORT) {
            rv = BCM_E_INTERNAL;
        }
        if(BCM_FAILURE(rv)) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return rv;
        }
    }

#ifdef BCM_TRIDENT2_SUPPORT
    /* mark SVP 2 used as dummy VP for VLAN_XLATE*/
    if (soc_feature(unit, soc_feature_vlan_xlate_iif_with_dummy_vp) &&
        soc_property_get(unit, spn_VLAN_ACTION_DUMMY_VP_RESERVED, 0)) {
        /* reserve svp 2 */
        vp_info.vp_type = _bcmVpTypeAny;
        rv = _bcm_vp_alloc(unit, _BCM_VLAN_XLATE_DUMMY_PORT, (num_vp - 1), 1, SOURCE_VPm,
                           vp_info, &vp);
        if (vp != _BCM_VLAN_XLATE_DUMMY_PORT) {
            rv = BCM_E_INTERNAL;
        }
        if(BCM_FAILURE(rv)) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return rv;
        }

        if (!SOC_WARM_BOOT(unit)) {
            rv = _bcm_vlan_xlate_dummy_vp_init(unit, _BCM_VLAN_XLATE_DUMMY_PORT);
            if(BCM_FAILURE(rv)) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return rv;
            }
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    /* Now that the VP bitmap is initialized, register it with the
     * SOC layer L2X thread to use during freeze/thaw. */
    soc_l2x_cml_vp_bitmap_set(unit, virtual_info->vp_bitmap);

    /* Update reference count for protocol pkt control profile */
    if (soc_mem_field_valid(unit, vfi_mem, PROTOCOL_PKT_INDEXf)) {
        num_bytes_vfi_table = SOC_MEM_TABLE_BYTES(unit, vfi_mem);
        vfi_mem_buffer = (uint8 *) soc_cm_salloc(unit, num_bytes_vfi_table,
                                                 "VFI table buffer");
        if (NULL == vfi_mem_buffer) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
        sal_memset(vfi_mem_buffer, 0, num_bytes_vfi_table);
        rv = soc_mem_read_range(unit, vfi_mem, MEM_BLOCK_ANY,
                                soc_mem_index_min(unit, vfi_mem),
                                soc_mem_index_max(unit, vfi_mem),
                                vfi_mem_buffer);
        if (SOC_E_NONE != rv) {
            soc_cm_sfree(unit, vfi_mem_buffer);
            _bcm_virtual_free_resource(unit, virtual_info);
            return rv;
        }
        for (idx = 0; idx < num_vfi; idx++) {
            vfi_entry = soc_mem_table_idx_to_pointer(unit, vfi_mem,
                                                     vfi_entry_t *,
                                                     vfi_mem_buffer, idx);
            profile_idx = soc_mem_field32_get(unit, vfi_mem, vfi_entry,
                                              PROTOCOL_PKT_INDEXf);
            rv = _bcm_prot_pkt_ctrl_reference(unit, profile_idx);
            if (BCM_E_NONE != rv) {
                soc_cm_sfree(unit, vfi_mem_buffer);
                _bcm_virtual_free_resource(unit, virtual_info);
                return rv;
            }
        }
        soc_cm_sfree(unit, vfi_mem_buffer);
        vfi_mem_buffer = NULL;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_virtual_reinit(unit);
    if (rv != BCM_E_NONE) {
        _bcm_virtual_free_resource(unit, virtual_info);
        return rv;
    }

    if (SOC_WARM_BOOT(unit)) {
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_vlan_vp)) {
            /* Warm boot recovery of VLAN module's VLAN_VP_INFO is postponed to
             * here since it depends on the warm boot recovery of this module's
             * virtual_info.
             */
#ifdef BCM_ENDURO_SUPPORT
            if (SOC_IS_ENDURO(unit)) {
                rv = bcm_enduro_vlan_virtual_reinit(unit);
            } else
#endif /* BCM_ENDURO_SUPPORT */
            {
                rv = bcm_tr2_vlan_virtual_reinit(unit);
            } 
            if (BCM_FAILURE(rv)) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return rv;
            }
            rv = _bcm_vlan_vp_count_recover(unit);
            if (BCM_FAILURE(rv)) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return rv;
            }
        }
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit, soc_feature_vp_lag)) {
            /* Warm boot recovery of trunk module's VP LAG info is postponed to
             * here since it depends on the warm boot recovery of this module's
             * VP LAG VP bitmap.
             */
            rv = bcm_td2_vp_lag_reinit(unit);
            if (BCM_FAILURE(rv)) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return rv;
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */

    }

#endif /* BCM_WARM_BOOT_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_virtual_deinit
 * Purpose:
 *      Necessary deinit process such as resource release.
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_virtual_deinit(int unit)
{
    _bcm_virtual_free_resource(unit, VIRTUAL_INFO(unit));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vp_alloc
 * Purpose:
 *      Internal function for allocating a group of VPs
 * Parameters:
 *      unit    -  (IN) Device number.
 *      start   -  (IN) First VP index to allocate from 
 *      end     -  (IN) Last VP index to allocate from 
 *      count   -  (IN) How many consecutive VPs to allocate
 *      vp_mem  -  (IN) HW specific VP memory
 *      info    -  (IN) VP type information
 *      base_vp -  (OUT) Base VP index
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vp_alloc(int unit, int start, int end, int count, soc_mem_t vp_mem, 
              _bcm_vp_info_t vp_info, int *base_vp)
{
    _bcm_vp_type_e type = vp_info.vp_type;
    int num_vp = soc_mem_index_count(unit, vp_mem);
    int i, j, vp;
    int found = 0;
    int reverse = 0;

    VIRTUAL_INIT_CHECK(unit);

    if ((end >= num_vp) || (start >= num_vp)) {
        return BCM_E_PARAM;
    }
    if (start > end) {
        reverse = 1;
    }

    /* vp_bitmap referenced by _BCM_VIRTUAL_VP_XXX macros, tracks
     * the usage of SOURCE_VP table because all vp types except wlan use the
     * same hardware table SOURCE_VP for vp assignment.  
     * Since Wlan vp uses a different table exclusively for vp assignment, 
     * the wlan_vp_bitmap alone should be sufficient to track wlan vp usage. 
     */
    VIRTUAL_LOCK(unit);
    if (type == _bcmVpTypeWlan) {
        if (!soc_feature(unit, soc_feature_wlan)) {
            VIRTUAL_UNLOCK(unit);
            return BCM_E_PORT;
        }

        if (reverse) {
            for (i = start; i >= end; i -= count) {
                for (j = 0; j < count; j++) {
                    if (_BCM_WLAN_VP_USED_GET(unit, i - j)) {
                        break;
                    }
                }
                if (j == count) {
                    break;
                }
            }
            if (i >= end) {
                *base_vp = i;
                for (j = 0; j < count; j++) {
                    _BCM_WLAN_VP_USED_SET(unit, i - j);
                }
                VIRTUAL_UNLOCK(unit);
                return BCM_E_NONE;
            }
        } else {
            for (i = start; i <= end; i += count) {
                for (j = 0; j < count; j++) {
                    if (_BCM_WLAN_VP_USED_GET(unit, i + j)) {
                        break;
                    }
                }
                if (j == count) {
                    break;
                }
            }
            if (i <= end) {
                *base_vp = i;
                for (j = 0; j < count; j++) {
                    _BCM_WLAN_VP_USED_SET(unit, i + j);
                }
                VIRTUAL_UNLOCK(unit);
                return BCM_E_NONE;
            }
        }
        VIRTUAL_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    /* handle all other vp types except wlan */
    if (reverse) {
        for (i = start; i >= end; i -= count) {
            for (j = 0; j < count; j++) {
                if (_BCM_VIRTUAL_VP_USED_GET(unit, i - j)) {
                    break;
                }
            }
            if (j == count) {
                break;
            }
        }
        if (i >= end) {
            found = 1;
        }
    } else {
        for (i = start; i <= end; i += count) {
            for (j = 0; j < count; j++) {
                if (_BCM_VIRTUAL_VP_USED_GET(unit, i + j)) {
                    break;
                }
            }
            if (j == count) {
                break;
            }
        }
        if (i <= end) {
            found = 1;
        }
    }
    if (found) {
        *base_vp = i;
        for (j = 0; j < count; j++) {
            if (reverse) {
                vp = i - j;
            } else {
                vp = i + j;
            }
            _BCM_VIRTUAL_VP_USED_SET(unit, vp);
            switch (type) {
            case _bcmVpTypeMpls:
                _BCM_MPLS_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeMim:
                _BCM_MIM_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeSubport:
                _BCM_SUBPORT_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeTrill:
                if (!soc_feature(unit, soc_feature_trill)) {
                    VIRTUAL_UNLOCK(unit);
                    return BCM_E_PORT;
                }
                _BCM_TRILL_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeVlan:
                if (!soc_feature(unit, soc_feature_vlan_vp)) {
                    VIRTUAL_UNLOCK(unit);
                    return BCM_E_PORT;
                }
                _BCM_VLAN_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeNiv:
                if (!soc_feature(unit, soc_feature_niv)) {
                    VIRTUAL_UNLOCK(unit);
                    return BCM_E_PORT;
                }
                _BCM_NIV_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeL2Gre:
                if (!soc_feature(unit, soc_feature_l2gre)) {
                    VIRTUAL_UNLOCK(unit);
                    return BCM_E_PORT;
                }
                _BCM_L2GRE_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeVxlan:
                if (!soc_feature(unit, soc_feature_vxlan)) {
                    VIRTUAL_UNLOCK(unit);
                    return BCM_E_PORT;
                }
                _BCM_VXLAN_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeFlow:
                if (!soc_feature(unit, soc_feature_flex_flow)) {
                    VIRTUAL_UNLOCK(unit);
                    return BCM_E_PORT;
                }
                _BCM_FLOW_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeExtender:
                if (!soc_feature(unit, soc_feature_port_extension)) {
                    VIRTUAL_UNLOCK(unit);
                    return BCM_E_PORT;
                }
                _BCM_EXTENDER_VP_USED_SET(unit, vp);
                break;
            case _bcmVpTypeVpLag:
                if (!soc_feature(unit, soc_feature_vp_lag)) {
                    VIRTUAL_UNLOCK(unit);
                    return BCM_E_PORT;
                }
                _BCM_VP_LAG_VP_USED_SET(unit, vp);
                break;
            default:
                break;
            }
            if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
                _BCM_NETWORK_VP_USED_SET(unit, vp);
            }
            if (vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
                if (!(soc_feature(unit, soc_feature_vp_sharing) ||
                          soc_feature(unit, soc_feature_nexthop_share_dvp))) {
                    VIRTUAL_UNLOCK(unit);
                    return BCM_E_UNAVAIL;
                }
                _BCM_SHARED_VP_USED_SET(unit, vp);
            }
        }
        VIRTUAL_UNLOCK(unit);
        return BCM_E_NONE;
    }
    VIRTUAL_UNLOCK(unit);
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_vp_free
 * Purpose:
 *      Internal function for freeing a group of VPs
 * Parameters:
 *      unit    -  (IN) Device number
 *      type    -  (IN) VP type
 *      count   -  (IN) How many consecutive VPs to free
 *      base_vp -  (IN) Base VP index
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vp_free(int unit, _bcm_vp_type_e type, int count, int base_vp)
{
    int i;
    VIRTUAL_INIT_CHECK(unit);
    VIRTUAL_LOCK(unit);
    for (i = 0; i < count; i++) {
        if (type != _bcmVpTypeWlan) {
            _BCM_VIRTUAL_VP_USED_CLR(unit, base_vp + i);
        }
        switch (type) {
        case _bcmVpTypeMpls:
            _BCM_MPLS_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeMim:
            _BCM_MIM_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeSubport:
            _BCM_SUBPORT_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeWlan:
            if (!soc_feature(unit, soc_feature_wlan)) {
                VIRTUAL_UNLOCK(unit);
                return BCM_E_PORT;
            }
            _BCM_WLAN_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeTrill:
            if (!soc_feature(unit, soc_feature_trill)) {
                VIRTUAL_UNLOCK(unit);
                return BCM_E_PORT;
            }
            _BCM_TRILL_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeVlan:
            if (!soc_feature(unit, soc_feature_vlan_vp)) {
                VIRTUAL_UNLOCK(unit);
                return BCM_E_PORT;
            }
            _BCM_VLAN_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeNiv:
            if (!soc_feature(unit, soc_feature_niv)) {
                VIRTUAL_UNLOCK(unit);
                return BCM_E_PORT;
            }
            _BCM_NIV_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeL2Gre:
            if (!soc_feature(unit, soc_feature_l2gre)) {
                VIRTUAL_UNLOCK(unit);
                return BCM_E_PORT;
            }
            _BCM_L2GRE_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeVxlan:
            if (!soc_feature(unit, soc_feature_vxlan)) {
                VIRTUAL_UNLOCK(unit);
                return BCM_E_PORT;
            }
            _BCM_VXLAN_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeFlow:
            if (!soc_feature(unit, soc_feature_flex_flow)) {
                VIRTUAL_UNLOCK(unit);
                return BCM_E_PORT;
            }
            _BCM_FLOW_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeExtender:
            if (!soc_feature(unit, soc_feature_port_extension)) {
                VIRTUAL_UNLOCK(unit);
                return BCM_E_PORT;
            }
            _BCM_EXTENDER_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeVpLag:
            if (!soc_feature(unit, soc_feature_vp_lag)) {
                VIRTUAL_UNLOCK(unit);
                return BCM_E_PORT;
            }
            _BCM_VP_LAG_VP_USED_CLR(unit, base_vp + i);
            break;
        default:
            break;
        }
        _BCM_NETWORK_VP_USED_CLR(unit, base_vp+i);

        if (soc_feature(unit, soc_feature_vp_sharing) ||
                soc_feature(unit, soc_feature_nexthop_share_dvp)) {
            _BCM_SHARED_VP_USED_CLR(unit, base_vp+i);
        }
    }
    VIRTUAL_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vp_used_set
 * Purpose:
 *      Mark the VP as used
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vp      -  (IN) VP
 *      type   -  (IN) VP type
 * Returns:
 *      Boolean
 */
int
_bcm_vp_used_set(int unit, int vp, _bcm_vp_info_t vp_info)
{
    int rv=BCM_E_NONE;
    _bcm_vp_type_e type = vp_info.vp_type;

    VIRTUAL_INIT_CHECK(unit);
    VIRTUAL_LOCK(unit);

    if (type != _bcmVpTypeWlan) {
        _BCM_VIRTUAL_VP_USED_SET(unit, vp);
    }
    switch (type) {
         case _bcmVpTypeMpls:
              _BCM_MPLS_VP_USED_SET(unit, vp);
             break;
         case _bcmVpTypeMim:
              _BCM_MIM_VP_USED_SET(unit, vp);
             break;
         case _bcmVpTypeSubport:
              _BCM_SUBPORT_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeWlan:
              if (!soc_feature(unit, soc_feature_wlan)) {
                  VIRTUAL_UNLOCK(unit);
                  return BCM_E_PORT;
              }
              _BCM_WLAN_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeTrill:
              if (!soc_feature(unit, soc_feature_trill)) {
                  _BCM_VIRTUAL_VP_USED_CLR(unit, vp);
                  VIRTUAL_UNLOCK(unit);
                  return BCM_E_PORT;
              }
              _BCM_TRILL_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeVlan:
              if (!soc_feature(unit, soc_feature_vlan_vp)) {
                  _BCM_VIRTUAL_VP_USED_CLR(unit, vp);
                  VIRTUAL_UNLOCK(unit);
                  return BCM_E_PORT;
              }
              _BCM_VLAN_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeNiv:
              if (!soc_feature(unit, soc_feature_niv)) {
                  _BCM_VIRTUAL_VP_USED_CLR(unit, vp);
                  VIRTUAL_UNLOCK(unit);
                  return BCM_E_PORT;
              }
              _BCM_NIV_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeL2Gre:
              if (!soc_feature(unit, soc_feature_l2gre)) {
                  _BCM_VIRTUAL_VP_USED_CLR(unit, vp);
                  VIRTUAL_UNLOCK(unit);
                  return BCM_E_PORT;
              }
              _BCM_L2GRE_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeVxlan:
              if (!soc_feature(unit, soc_feature_vxlan)) {
                  _BCM_VIRTUAL_VP_USED_CLR(unit, vp);
                  VIRTUAL_UNLOCK(unit);
                  return BCM_E_PORT;
              }
              _BCM_VXLAN_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeExtender:
              if (!soc_feature(unit, soc_feature_port_extension)) {
                  _BCM_VIRTUAL_VP_USED_CLR(unit, vp);
                  VIRTUAL_UNLOCK(unit);
                  return BCM_E_PORT;
              }
              _BCM_EXTENDER_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeVpLag:
              if (!soc_feature(unit, soc_feature_vp_lag)) {
                  _BCM_VIRTUAL_VP_USED_CLR(unit, vp);
                  VIRTUAL_UNLOCK(unit);
                  return BCM_E_PORT;
              }
              _BCM_VP_LAG_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeFlow:
              if (!soc_feature(unit, soc_feature_flex_flow)) {
                  _BCM_VIRTUAL_VP_USED_CLR(unit, vp);
                  VIRTUAL_UNLOCK(unit);
                  return BCM_E_PORT;
              }
              _BCM_FLOW_VP_USED_SET(unit, vp);
              break;
         default:
              break;
    }

    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        _BCM_NETWORK_VP_USED_SET(unit, vp);
    } else {
        _BCM_NETWORK_VP_USED_CLR(unit, vp);
    }

    if (vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
        if (!(soc_feature(unit, soc_feature_vp_sharing) ||
                  soc_feature(unit, soc_feature_nexthop_share_dvp))) {
            VIRTUAL_UNLOCK(unit);
            return BCM_E_UNAVAIL;
        }
        _BCM_SHARED_VP_USED_SET(unit, vp);
    }

    VIRTUAL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_vp_info_get
 * Purpose:
 *      Get the VP Type of a VP, given its Id
 * Parameters:
 *      unit     -  (IN) Device number.
 *      vp       -  (IN) VP
 *      vp_type  -  (OUT) VP type
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_vp_info_get(int unit, int vp, _bcm_vp_info_t *vp_info)
{
    _bcm_vp_type_e vpt;
    _bcm_vp_info_init(vp_info);

    for (vpt = _bcmVpTypeMpls; vpt < _bcmVpTypeAny; vpt++) {
        if (_bcm_vp_used_get(unit, vp, vpt)) {
            vp_info->vp_type = vpt;
            if (_BCM_NETWORK_VP_USED_GET(unit, vp)) {
                vp_info->flags |= _BCM_VP_INFO_NETWORK_PORT;
            }
            if (soc_feature(unit, soc_feature_vp_sharing) ||
                    soc_feature(unit, soc_feature_nexthop_share_dvp)) {
                if (_BCM_SHARED_VP_USED_GET(unit, vp)) {
                    vp_info->flags |= _BCM_VP_INFO_SHARED_PORT;
                }
            }
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_vp_encode_gport
 * Purpose:
 *      Given a VP, encode its gport type
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vp      -  (IN) VP
 *      type   -  (IN) VP type
 * Returns:
 *      Boolean
 */
int 
_bcm_vp_encode_gport(int unit, int vp, int *gport)
{
    _bcm_vp_info_t vp_info;

    if(_bcm_vp_info_get(unit, vp, &vp_info) == BCM_E_NONE) {
        if(vp_info.vp_type == _bcmVpTypeMpls)
            BCM_GPORT_MPLS_PORT_ID_SET(*gport, vp);
        else if(vp_info.vp_type == _bcmVpTypeMim)
            BCM_GPORT_MIM_PORT_ID_SET(*gport, vp);
        else if(vp_info.vp_type == _bcmVpTypeSubport)
            BCM_GPORT_SUBPORT_GROUP_SET(*gport, vp);  
        else if(vp_info.vp_type == _bcmVpTypeWlan)
            BCM_GPORT_WLAN_PORT_ID_SET(*gport, vp);
        else if(vp_info.vp_type == _bcmVpTypeTrill)
            BCM_GPORT_TRILL_PORT_ID_SET(*gport, vp);
        else if(vp_info.vp_type == _bcmVpTypeVlan)
            BCM_GPORT_VLAN_PORT_ID_SET(*gport, vp);
        else if(vp_info.vp_type == _bcmVpTypeNiv)
            BCM_GPORT_NIV_PORT_ID_SET(*gport, vp);
        else if(vp_info.vp_type == _bcmVpTypeL2Gre)
            BCM_GPORT_L2GRE_PORT_ID_SET(*gport, vp);
        else if(vp_info.vp_type == _bcmVpTypeVxlan)
            BCM_GPORT_VXLAN_PORT_ID_SET(*gport, vp);
        else if(vp_info.vp_type == _bcmVpTypeExtender)
            BCM_GPORT_EXTENDER_PORT_ID_SET(*gport, vp);
        else if(vp_info.vp_type == _bcmVpTypeFlow)
            BCM_GPORT_FLOW_PORT_ID_SET(*gport, vp);
        else
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vp_used_get
 * Purpose:
 *      Check whether a VP is used or not
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vp      -  (IN) VP
 * Returns:
 *      Boolean
 */
int
_bcm_vp_used_get(int unit, int vp, _bcm_vp_type_e type)
{
    int rv = BCM_E_NONE;
    int vp_chg = 0;

    
    switch (type) {
    case _bcmVpTypeMpls:
        if (vp < 0) {
            return rv;
        }
        if (SOC_MEM_IS_VALID(unit, SOURCE_VPm)) {
            if (_BCM_MPLS_GPORT_FAILOVER_VPLESS_GET(vp)) {
                vp_chg = _BCM_MPLS_GPORT_FAILOVER_VPLESS_CLEAR(vp);
            } else {
                vp_chg = vp;
            }
            if (vp_chg > soc_mem_index_count(unit, SOURCE_VPm)) {
                return rv;
            }
            rv = _BCM_MPLS_VP_USED_GET(unit, vp_chg);
        }
        break;
    case _bcmVpTypeMim:
        if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
            return rv;
        }
        rv = _BCM_MIM_VP_USED_GET(unit, vp);
        break;
    case _bcmVpTypeSubport:
        if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
            return rv;
        }
        rv = _BCM_SUBPORT_VP_USED_GET(unit, vp);
        break;
    case _bcmVpTypeWlan:
        if (soc_feature(unit, soc_feature_wlan)) {
            if (SOC_MEM_IS_VALID(unit, WLAN_SVP_TABLEm)) {
                if (vp > soc_mem_index_count(unit, WLAN_SVP_TABLEm)) {
                    return rv;
                }
            } else if (SOC_MEM_IS_VALID(unit,SOURCE_VP_ATTRIBUTES_2m)) {
                if (vp > soc_mem_index_count(unit, SOURCE_VP_ATTRIBUTES_2m)) {
                    return rv;
                }
            } else {
                return rv;
            }
            rv = _BCM_WLAN_VP_USED_GET(unit, vp);
        }
        break;
    case _bcmVpTypeTrill:
        if (soc_feature(unit, soc_feature_trill)) {
            if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
                return rv;
            }
            rv = _BCM_TRILL_VP_USED_GET(unit, vp);
        }
        break;
    case _bcmVpTypeVlan:
        if (soc_feature(unit, soc_feature_vlan_vp)) {
            if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
                return rv;
            }
            rv = _BCM_VLAN_VP_USED_GET(unit, vp);
        }
        break;
    case _bcmVpTypeNiv:
        if (soc_feature(unit, soc_feature_niv)) {
            if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
                return rv;
            }
            rv = _BCM_NIV_VP_USED_GET(unit, vp);
        }
        break;
    case _bcmVpTypeL2Gre:
        if (soc_feature(unit, soc_feature_l2gre)) {
            if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
                return rv;
            }
            rv = _BCM_L2GRE_VP_USED_GET(unit, vp);
        }
        break;
    case _bcmVpTypeVxlan:
        if (soc_feature(unit, soc_feature_vxlan)) {
            if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
                return rv;
            }
            rv = _BCM_VXLAN_VP_USED_GET(unit, vp);
        }
        break;
    case _bcmVpTypeExtender:
        if (soc_feature(unit, soc_feature_port_extension)) {
            if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
                return rv;
            }
            rv = _BCM_EXTENDER_VP_USED_GET(unit, vp);
        }
        break;
    case _bcmVpTypeVpLag:
        if (soc_feature(unit, soc_feature_vp_lag)) {
            if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
                return rv;
            }
            rv = _BCM_VP_LAG_VP_USED_GET(unit, vp);
        }
        break;
    case _bcmVpTypeFlow:
        if (soc_feature(unit, soc_feature_flex_flow)) {
            if (vp > soc_mem_index_count(unit, SOURCE_VPm)) {
                return rv;
            }
            rv = _BCM_FLOW_VP_USED_GET(unit, vp);
        }
        break;
    default:
        rv = _BCM_VIRTUAL_VP_USED_GET(unit, vp);
        break;
    }
    return rv;
}


int
_bcm_vp_gport_dest_fill(int unit, int vp, _bcm_gport_dest_t *gport_dest)
{
    _bcm_vp_info_t vp_info;

    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));

    switch (vp_info.vp_type) {
    case _bcmVpTypeMpls:
        gport_dest->gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
        gport_dest->mpls_id = vp;
        break;
    case _bcmVpTypeMim:
        gport_dest->gport_type = _SHR_GPORT_TYPE_MIM_PORT;
        gport_dest->mim_id = vp;
        break;
    case _bcmVpTypeSubport:
        gport_dest->gport_type = _SHR_GPORT_TYPE_SUBPORT_PORT;
        gport_dest->subport_id = vp;
        break;
    case _bcmVpTypeWlan:
        gport_dest->gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
        gport_dest->wlan_id = vp;
        break;
    case _bcmVpTypeTrill:
        gport_dest->gport_type = _SHR_GPORT_TYPE_TRILL_PORT;
        gport_dest->trill_id = vp;
        break;
    case _bcmVpTypeVlan:
        gport_dest->gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
        gport_dest->vlan_vp_id = vp;
        break;
    case _bcmVpTypeNiv:
        gport_dest->gport_type = _SHR_GPORT_TYPE_NIV_PORT;
        gport_dest->niv_id = vp;
        break;
    case _bcmVpTypeL2Gre:
        gport_dest->gport_type = _SHR_GPORT_TYPE_L2GRE_PORT;
        gport_dest->l2gre_id = vp;
        break;
    case _bcmVpTypeVxlan:
        gport_dest->gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
        gport_dest->vxlan_id = vp;
        break;
    case _bcmVpTypeExtender:
        gport_dest->gport_type = _SHR_GPORT_TYPE_EXTENDER_PORT;
        gport_dest->extender_id = vp;
        break;
    case _bcmVpTypeVpLag:
        gport_dest->gport_type = _SHR_GPORT_TYPE_VP_GROUP;
        break;
    default:
        gport_dest->gport_type = _SHR_GPORT_INVALID;
        break;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_vp_default_cml_mode_get
 * Purpose:
 *      Get Gport default cml_enable
 * Parameters:
 *      unit    -  (IN) Device number.
 *      cml_default_enable  -  (OUT) default_cml_mode_enable
 *      cml_new  - (OUT) default cml_new
 *      cml_move - (OUT) default cml_move
 * Returns:
 *      Boolean
 */
int
_bcm_vp_default_cml_mode_get (int unit, int *cml_default_enable, int *cml_new, int *cml_move)
{
   int vp = 0;
   int rv = BCM_E_NONE;
   source_vp_entry_t svp;

    /* HW retriction - VP index zero is reserved */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN
          (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

    if (soc_SOURCE_VPm_field32_get(unit, &svp, CLASS_IDf)) {
         *cml_new = soc_SOURCE_VPm_field32_get(unit, &svp, CML_FLAGS_NEWf);
         *cml_default_enable = 0x1;
    } else {
         *cml_new = 0x8; /* Default Value of cml */
    }

    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dv, dest_type = SOC_MEM_FIF_DEST_INVALID;
        dv = soc_mem_field32_dest_get(unit, SOURCE_VPm, &svp, DESTINATIONf, &dest_type);
        if (dest_type != SOC_MEM_FIF_DEST_DVP) {
            dv = 0;
        }
        if (dv) {
            *cml_move = soc_SOURCE_VPm_field32_get(unit, &svp, CML_FLAGS_MOVEf);
            *cml_default_enable = 0x1;
        } else {
            *cml_move = 0x8;
        }
    } else {
        if (soc_SOURCE_VPm_field32_get(unit, &svp, DVPf)) {
            *cml_move = soc_SOURCE_VPm_field32_get(unit, &svp, CML_FLAGS_MOVEf);
            *cml_default_enable = 0x1;
        } else {
            *cml_move = 0x8; /* Default Value of cml */
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_vfi_init
 * Purpose:
 *      initialize VFI hardware tables
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vfi     -  (IN) VFI HW table index
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vfi_init(int unit, int vfi)
{
    vfi_entry_t vfi_entry;
    egr_vfi_entry_t egr_vfi_entry;

    sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
    sal_memset(&egr_vfi_entry, 0, sizeof(egr_vfi_entry_t));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        /* Initialize VFI with default stg group and membership profile */
        soc_EGR_VFIm_field32_set(unit, &egr_vfi_entry, MEMBERSHIP_PROFILE_PTRf, 1);
        soc_EGR_VFIm_field32_set(unit, &egr_vfi_entry, EN_EFILTERf, 1);
    }
#endif

    BCM_IF_ERROR_RETURN(WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry));
    if (soc_mem_is_valid(unit, EGR_VFIm)) {
        BCM_IF_ERROR_RETURN(WRITE_EGR_VFIm(unit, MEM_BLOCK_ALL, vfi, &egr_vfi_entry));
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        bcm_vpn_t vpn;
        bcm_stg_t stg;

        _BCM_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, vfi);
        BCM_IF_ERROR_RETURN(bcm_esw_stg_default_get(unit, &stg));
        BCM_IF_ERROR_RETURN(bcm_esw_stg_vlan_add(unit, stg, vpn));
    }
#endif

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_vfi_alloc
 * Purpose:
 *      Internal function for allocating a VFI
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vfi_mem -  (IN) HW specific VFI memory
 *      type    -  (IN) VFI type
 *      vfi     -  (OUT) Base VP index
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vfi_alloc(int unit, soc_mem_t vfi_mem, _bcm_vfi_type_e type, int *vfi)
{
    int rv, i, num_vfi;
    num_vfi = soc_mem_index_count(unit, vfi_mem);
    VIRTUAL_LOCK(unit);
    if (soc_feature(unit, soc_feature_vfi_zero_invalid)) {
        i = 1;  /* entry 0 is not usable */
    } else {
        i = 0;
    }
    for (; i < num_vfi; i++) {
        if (!_BCM_VIRTUAL_VFI_USED_GET(unit, i)) {
            break;
        }
    }
    if (i == num_vfi) {
        VIRTUAL_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }
    *vfi = i;
    _BCM_VIRTUAL_VFI_USED_SET(unit, i);
    switch (type) {
    case _bcmVfiTypeMpls:
        _BCM_MPLS_VFI_USED_SET(unit, i);
        break;
    case _bcmVfiTypeMim:
        _BCM_MIM_VFI_USED_SET(unit, i);
        break;
    case _bcmVfiTypeL2Gre:
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_l2gre)) {
            _BCM_L2GRE_VFI_USED_SET(unit, i);
        }
#endif
        break;
    case _bcmVfiTypeVxlan:
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan)) {
           _BCM_VXLAN_VFI_USED_SET(unit, i);
        }
#endif
        break;
    case _bcmVfiTypeVlan:
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi)) {
            _BCM_VLAN_VFI_USED_SET(unit, i);
        }
#endif
        break;
    case _bcmVfiTypeFlow:
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flow)) {
           _BCM_FLOW_VFI_USED_SET(unit, i);
        }
#endif
        break;
    default:
        break;
    }

    rv = _bcm_vfi_init(unit, i);
    VIRTUAL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vfi_alloc_with_id
 * Purpose:
 *      Internal function for allocating a VFI with a given ID
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vfi_mem -  (IN) HW specific VFI memory
 *      type    -  (IN) VFI type
 *      vfi     -  (IN) VFI index
 * Returns:
 *      BCM_X_XXX
 */
 
int
_bcm_vfi_alloc_with_id(int unit, soc_mem_t vfi_mem, _bcm_vfi_type_e type, int vfi)
{
    int rv, num_vfi;
    num_vfi = soc_mem_index_count(unit, vfi_mem);

    /* Check Valid range of VFI */
    if ( vfi < 0 || vfi >= num_vfi ) {
         return BCM_E_RESOURCE;
    }
    if (soc_feature(unit, soc_feature_vfi_zero_invalid)) {
        if (vfi == 0) { /* entry 0 is not usable */
            return BCM_E_PARAM;
        }
    }
   
    VIRTUAL_LOCK(unit);
    if (_BCM_VIRTUAL_VFI_USED_GET(unit, vfi)) {
        VIRTUAL_UNLOCK(unit);
        return BCM_E_EXISTS;
    }

    _BCM_VIRTUAL_VFI_USED_SET(unit, vfi);

    switch (type) {
    case _bcmVfiTypeMpls:
        _BCM_MPLS_VFI_USED_SET(unit, vfi);
        break;
    case _bcmVfiTypeMim:
        _BCM_MIM_VFI_USED_SET(unit, vfi);
        break;
    case _bcmVfiTypeL2Gre:
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_l2gre)) {
            _BCM_L2GRE_VFI_USED_SET(unit, vfi);
        }
#endif
        break;
    case _bcmVfiTypeVxlan:
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan)) {
            _BCM_VXLAN_VFI_USED_SET(unit, vfi);
        }
#endif
        break;
    case _bcmVfiTypeVlan:
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi)) {
            _BCM_VLAN_VFI_USED_SET(unit, vfi);
        }
#endif
        break;
    case _bcmVfiTypeFlow:
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flow)) {
            _BCM_FLOW_VFI_USED_SET(unit, vfi);
        }
#endif
        break;
    default:
        break;
    }

    rv = _bcm_vfi_init(unit, vfi);
    VIRTUAL_UNLOCK(unit);

    return rv;
}


/*
 * Function:
 *      _bcm_vfi_free
 * Purpose:
 *      Internal function for freeing a VFI
 * Parameters:
 *      unit    -  (IN) Device number.
 *      type    -  (IN) VFI type
 *      base_vfi - (IN) VFI index
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vfi_free(int unit, _bcm_vfi_type_e type, int vfi)
{
    vfi_entry_t vfi_entry;
    egr_vfi_entry_t egr_vfi_entry;
    int rv = BCM_E_NONE;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        bcm_vpn_t vpn;
        bcm_stg_t stg;

        /* Remove VPN from its spanning tree group */
        _BCM_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, vfi);
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_stg_get(unit, vpn, &stg));
        BCM_IF_ERROR_RETURN(_bcm_stg_vlan_destroy(unit, stg, vpn));
    }

    /* Delete VFI PROFILE */
    if (soc_feature(unit, soc_feature_vfi_profile)) {
        uint32 index = 0;

        BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry));
        index = soc_VFIm_field32_get(unit, &vfi_entry, VFI_PROFILE_PTRf);
        if (0 != index) {
            BCM_IF_ERROR_RETURN(_bcm_vfi_profile_entry_delete(unit, index));
        }
    }
#endif

    VIRTUAL_LOCK(unit);
    _BCM_VIRTUAL_VFI_USED_CLR(unit, vfi);
    switch (type) {
    case _bcmVfiTypeMpls:
        _BCM_MPLS_VFI_USED_CLR(unit, vfi);
        break;
    case _bcmVfiTypeMim:
        _BCM_MIM_VFI_USED_CLR(unit, vfi);
        break;
    case _bcmVfiTypeL2Gre:
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_l2gre)) {
            _BCM_L2GRE_VFI_USED_CLR(unit, vfi);
        }
#endif
        break;
    case _bcmVfiTypeVxlan:
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan)) {
            _BCM_VXLAN_VFI_USED_CLR(unit, vfi);
        }
#endif
        break;
    case _bcmVfiTypeVlan:
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi)) {
            _BCM_VLAN_VFI_USED_CLR(unit, vfi);
        }
#endif
        break;
    case _bcmVfiTypeFlow:
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flow)) {
            _BCM_FLOW_VFI_USED_CLR(unit, vfi);
        }
#endif
        break;
    default:
        break;
    }

    sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);
    if (SOC_FAILURE(rv)) {
        VIRTUAL_UNLOCK(unit);
        return rv;
    }
    sal_memset(&egr_vfi_entry, 0, sizeof(egr_vfi_entry_t));
    rv = WRITE_EGR_VFIm(unit, MEM_BLOCK_ALL, vfi, &egr_vfi_entry);
    VIRTUAL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_vfi_used_get
 * Purpose:
 *      Check whether a VFI is used or not 
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vfi     -  (IN) VFI
 *      type    -  (IN) VFI type
 * Returns:
 *      Boolean
 */
int
_bcm_vfi_used_get(int unit, int vfi, _bcm_vfi_type_e type)
{
    int rv=0;
/* Removed Lock-Unlock - fn used in Interrupt context */
    switch (type) {
    case _bcmVfiTypeMpls:
        rv = _BCM_MPLS_VFI_USED_GET(unit, vfi);
        break;
    case _bcmVfiTypeMim:
        rv = _BCM_MIM_VFI_USED_GET(unit, vfi);
        break;
    case _bcmVfiTypeL2Gre:
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_l2gre)) {
            rv = _BCM_L2GRE_VFI_USED_GET(unit, vfi);
        }
#endif
        break;
    case _bcmVfiTypeVxlan:
#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan)) {
            rv = _BCM_VXLAN_VFI_USED_GET(unit, vfi);
        }
#endif
        break;
    case _bcmVfiTypeVlan:
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi)) {
            rv = _BCM_VLAN_VFI_USED_GET(unit, vfi);
        }
#endif
        break;
    case _bcmVfiTypeFlow:
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flow)) {
            rv = _BCM_FLOW_VFI_USED_GET(unit, vfi);
        }
#endif
        break;
    default:
        rv = _BCM_VIRTUAL_VFI_USED_GET(unit, vfi);
        break;
    }
    return rv;
}

#ifdef BCM_TRIUMPH2_SUPPORT 
/*
 * Function:
 *      _bcm_vfi_flex_stat_index_set
 * Purpose:
 *      Associate a flexible stat with a VFI
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vfi     -  (IN) VFI
 *      fs_idx  -  (IN) Flexible stat index
 * Returns:
 *      Boolean
 */
int
_bcm_vfi_flex_stat_index_set(int unit, int vfi, _bcm_vfi_type_e type,
                             int fs_idx,uint32 flags)
{
    int rv,rv1;
    vfi_entry_t vfi_entry;
    egr_vfi_entry_t egr_vfi_entry;

    rv = BCM_E_NONE;
    rv1 = BCM_E_NONE;
    VIRTUAL_LOCK(unit);
    if (_bcm_vfi_used_get(unit, vfi, type)) {
        if (flags & _BCM_FLEX_STAT_HW_INGRESS) {
            rv = soc_mem_read(unit, VFIm, MEM_BLOCK_ANY, vfi, &vfi_entry);
            if (BCM_SUCCESS(rv)) {
                if (soc_mem_field_valid(unit, VFIm, USE_SERVICE_CTR_IDXf)) {
                    soc_mem_field32_set(unit, VFIm, &vfi_entry,
                                    USE_SERVICE_CTR_IDXf, fs_idx > 0 ? 1 : 0);
                }
                soc_mem_field32_set(unit, VFIm, &vfi_entry, SERVICE_CTR_IDXf,
                                    fs_idx);
                rv = soc_mem_write(unit, VFIm, MEM_BLOCK_ANY, vfi, &vfi_entry);
            }
        }
        if (flags & _BCM_FLEX_STAT_HW_EGRESS) {
            rv1 = soc_mem_read(unit, EGR_VFIm, MEM_BLOCK_ANY, vfi,
                              &egr_vfi_entry);
            if (BCM_SUCCESS(rv1)) {
                if (soc_mem_field_valid(unit, EGR_VFIm,
                                        USE_SERVICE_CTR_IDXf)) {
                    soc_mem_field32_set(unit, EGR_VFIm, &egr_vfi_entry,
                                        USE_SERVICE_CTR_IDXf,
                                        fs_idx > 0 ? 1 : 0);
                }
                soc_mem_field32_set(unit, EGR_VFIm, &egr_vfi_entry,
                                    SERVICE_CTR_IDXf, fs_idx);
                rv1 = soc_mem_write(unit, EGR_VFIm, MEM_BLOCK_ANY, vfi,
                                   &egr_vfi_entry);
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    VIRTUAL_UNLOCK(unit);
    if (BCM_SUCCESS(rv1)) {
        return rv;
    } else {
        return rv1;
    }
}
#endif  /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      _bcm_vp_ing_dvp_config
 * Purpose:
 *      Configures ING_DVP and ING_DVP_2 tables
 * Parameters:
 *   unit -  (IN) Device number.
 *   op   -  (IN) Operation to perform on the ING_DVP_* tables
 *     _bcmVpIngDvpConfigClear:  Clear the tables.
 *     _bcmVpIngDvpConfigSet  :  Set the tables with inputs, rest to zeros.
 *     _bcmVpIngDvpConfigUpdate: Update the tables with inputs, rest unmodified
 *   vp   -          (IN) Virtual Port ID.
 *   vp_type   -  (IN) Virtual Port type.
 *   intf     -       (IN)  EGR L3 nexthops, DVP EGR L3 nexthops or
 *                      ECMP group objects
 *   network_port -  (IN) indicate network port or not.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *     ING_DVP_2_TABLE also should be updated for IFP redirect to work.
 *     For Td2p/Td2/TH, this table should be programed the same as ING_DVP_TABLE.
 *     This function can be called by passing only the changed fields with 
 *     _bcmVpIngDvpConfigUpdate. Any params set as invalid value should not be touched.
 */
int
_bcm_vp_ing_dvp_config(int unit, _bcm_vp_ing_dvp_config_op_t op, int vp,
                                int vp_type, bcm_if_t intf, int network_port)
{
    int rv = BCM_E_NONE;
    int ecmp = -1;
    int nh_ecmp_index = -1;
    soc_field_t nw_field = NETWORK_PORTf;
    ing_dvp_table_entry_t dvp_entry;
#if defined (BCM_TRIDENT_SUPPORT)
    ing_dvp_2_table_entry_t dvp_2_entry;
#endif /* BCM_TRIDENT_SUPPORT */

    if ((soc_feature(unit, soc_feature_multiple_split_horizon_group))) {
        nw_field = NETWORK_GROUPf;
    }

    if (op == _bcmVpIngDvpConfigUpdate) {
        /* read the entry from hardware */
        rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry);
        BCM_IF_ERROR_RETURN(rv);

#if defined (BCM_TRIDENT_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, ING_DVP_2_TABLEm)) {
            rv = READ_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_2_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
#endif /* BCM_TRIDENT_SUPPORT */
    } else {

        sal_memset(&dvp_entry, 0, sizeof(ing_dvp_table_entry_t));
#if defined (BCM_TRIDENT_SUPPORT)
        sal_memset(&dvp_2_entry, 0, sizeof(ing_dvp_2_table_entry_t));
#endif /* BCM_TRIDENT_SUPPORT */

    }
    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf) || 
        BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {

        /* Extract next hop index from unipath egress object. */
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
            nh_ecmp_index = intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            nh_ecmp_index = intf - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }
        ecmp = 0;
    } else if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, intf)) {
        nh_ecmp_index = intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        ecmp = 1;        
#ifdef BCM_MULTI_LEVEL_ECMP_SUPPORT
        /*
         * When Multi-level ECMP is enabled, ECMP group pointed
         * to by DVP must be an underlay group.
         */
#if defined(BCM_FIREBOLT6_SUPPORT)
        if (soc_feature(unit, soc_feature_fb6_ecmp_group_partitioning) &&
                BCMI_L3_ECMP_GROUP_OVERLAY_IN_UPPER_RANGE(unit)) {
            if (nh_ecmp_index >= BCMI_L3_ECMP_OVERLAY_ENTRIES(unit)) {
                return BCM_E_PARAM;
            }
        } else
#endif
        if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
            if (nh_ecmp_index < BCMI_L3_ECMP_OVERLAY_ENTRIES(unit)) {
                return BCM_E_PARAM;
            }
        }
#endif
    }
    /* Configure DVP table */
    if (op != _bcmVpIngDvpConfigClear) {
        if ((ecmp == 0) && (nh_ecmp_index > 0)) {
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry,
                                           ECMP_PTRf, 0);
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry,
                                           NEXT_HOP_INDEXf, nh_ecmp_index);
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry,
                                           ECMPf, 0);
#if defined (BCM_TRIDENT_SUPPORT)
            if (SOC_MEM_IS_VALID(unit, ING_DVP_2_TABLEm)) {
                soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp_2_entry,
                                                 ECMP_PTRf, 0);
                soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp_2_entry,
                                                 NEXT_HOP_INDEXf, nh_ecmp_index);
                soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp_2_entry,
                                                 ECMPf, 0);
            }
#endif /* BCM_TRIDENT_SUPPORT */
        } 
        if ((ecmp == 1) && (nh_ecmp_index >= 0)) {
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry,
                                           NEXT_HOP_INDEXf, 0);
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry,
                                           ECMPf, 1);
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry,
                                           ECMP_PTRf, nh_ecmp_index);
#if defined (BCM_TRIDENT_SUPPORT)
            if (SOC_MEM_IS_VALID(unit, ING_DVP_2_TABLEm)) {
                soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp_2_entry,
                                                 NEXT_HOP_INDEXf, 0);
                soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp_2_entry,
                                                 ECMPf, 1);
                soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp_2_entry,
                                                 ECMP_PTRf, nh_ecmp_index);
            }
#endif /* BCM_TRIDENT_SUPPORT */
        }

        if ((vp_type >= _bcmVpIngDvpVpTypeMPLS_MIM_NIV) && 
            (vp_type < _bcmVpIngDvpVpTypeCount)) {
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry,
                                           VP_TYPEf, vp_type);
#if defined (BCM_TRIDENT_SUPPORT)
            if (SOC_MEM_IS_VALID(unit, ING_DVP_2_TABLEm)) {
                soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp_2_entry,
                                                 VP_TYPEf, vp_type);
            }
#endif /* BCM_TRIDENT_SUPPORT */
        }
        /*Do not allow (-1) as network group id*/
        if (network_port != ING_DVP_CONFIG_INVALID_PORT_TYPE) {
            if (soc_mem_field_valid(unit, ING_DVP_TABLEm, nw_field)) {
                soc_ING_DVP_TABLEm_field32_set(unit,
                    &dvp_entry, nw_field, network_port);
            }
#if defined (BCM_TRIDENT_SUPPORT)
            if (SOC_MEM_IS_VALID(unit, ING_DVP_2_TABLEm)) {
                soc_ING_DVP_2_TABLEm_field32_set(unit,
                    &dvp_2_entry, nw_field, network_port);
            }
#endif /* BCM_TRIDENT_SUPPORT */
        }
    }

    rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry);
    BCM_IF_ERROR_RETURN(rv);

#if defined (BCM_TRIDENT_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, ING_DVP_2_TABLEm)) {
        rv = WRITE_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_2_entry);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_TRIDENT_SUPPORT */


    return rv;
}

int
_bcm_vp_ing_dvp_to_ing_dvp2_2(int unit,
    ing_dvp_table_entry_t *dvp, int vp, int network_group_id)
{
    ing_dvp_2_table_entry_t dvp2;
    uint8 vp_type;
    uint8 ecmp;
    uint8 nw_port, nw_group;
    uint8 en_vplag_res;
    uint32 nh_index;

    if (!soc_feature(unit, soc_feature_ifp_redirect_to_dvp)) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(READ_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ANY,
                vp, &dvp2));

    vp_type = soc_ING_DVP_TABLEm_field32_get(unit, dvp, 
            VP_TYPEf);
    soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp2, VP_TYPEf, vp_type);

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            nw_group = network_group_id;
        } else
#endif
        {
            nw_group = soc_ING_DVP_TABLEm_field32_get(unit, dvp,
                NETWORK_GROUPf);
        }
        soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp2, NETWORK_GROUPf, nw_group);
    } else {
        nw_port = soc_ING_DVP_TABLEm_field32_get(unit, dvp, NETWORK_PORTf);
        soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp2, NETWORK_PORTf, nw_port);
    }

    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, dvp, 
            ECMPf);
    soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp2, ECMPf, ecmp);

    if (soc_feature(unit, soc_feature_vp_lag)) {
        en_vplag_res = soc_ING_DVP_TABLEm_field32_get(unit, dvp, 
                ENABLE_VPLAG_RESOLUTIONf);
        soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp2, ENABLE_VPLAG_RESOLUTIONf,
                en_vplag_res);
    }

    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, dvp, 
            NEXT_HOP_INDEXf);
    soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp2, NEXT_HOP_INDEXf, nh_index);

    BCM_IF_ERROR_RETURN(WRITE_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ANY,
                vp, &dvp2));

    return BCM_E_NONE;
}

int
_bcm_vp_ing_dvp_to_ing_dvp2(int unit, ing_dvp_table_entry_t *dvp,
    int vp)
{
    return _bcm_vp_ing_dvp_to_ing_dvp2_2(unit, dvp, vp , 0);
}

int
_bcm_validate_splithorizon_network_group(int unit,int is_nw_port,
    int *network_group)
{
    int group=0;
    group=*network_group;

    /*If spn_USE_ALL_SPLITHORIZON_GROUPS config property is enabled customer
      can use split horizon groups from zero to max supported otherwise
      0 and 1 are reserved.And based on soc_feature_reserve_shg_network_port
      BCM5656x family of devices the split horizon group index 1 is
       reserved by default for network ports*/

    /*Do not allow out of range group ids*/
    if (!_BCM_SWITCH_NETWORK_GROUP_ID_VALID (unit, group)) {
        return BCM_E_PARAM;
    }

    if(soc_feature(unit,
                soc_feature_multiple_split_horizon_group)) {

        if(!soc_property_get(unit, spn_USE_ALL_SPLITHORIZON_GROUPS, 0)) {
            if (soc_feature(unit,
                        soc_feature_reserve_shg_network_port)) {
                /*Default value:
                 *0 - for access ports
                 *1 - for network ports*/
                if (_BCM_SWITCH_RESERVED_NETWORK_GROUP_IDS(unit, group)) {
                    if (is_nw_port) {
                        *network_group = 1;
                    } else {
                        *network_group = 0;
                    }
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_switch_network_group_pruning
 * Parameters:
 *    unit                   (IN)device number
 *    source_network_group_id (IN) source split horizon group ID
 *    config                 (IN) network group config information
 * Purpose:
 *    This API is used to get the enable/disable status of ingress pruning, egress pruning and
 *    ingress IPMC group remap for a given pair of
 *    source split horizon group and destination split horizon group.
 *    Katana2 (BCM5645x) family of devices support 8 split horizon group.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_switch_network_group_pruning(int unit,
        bcm_switch_network_group_t source_network_group_id,
        bcm_switch_network_group_config_t *config)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    soc_field_t field;
    bcm_pbmp_t prune_pbmp;
    ing_network_prune_control_entry_t ing_entry;
    egr_network_prune_control_entry_t egr_entry;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    BCM_PBMP_CLEAR(prune_pbmp);
    mem = ING_NETWORK_PRUNE_CONTROLm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, COPYNO_ALL,
                    source_network_group_id, &ing_entry));

        field = REMAP_IPMC_GROUPf;
        if (SOC_MEM_FIELD_VALID(unit, mem, field)) {
            soc_mem_field32_set(unit, mem, &ing_entry, field,
                    (config->config_flags &
                     BCM_SWITCH_NETWORK_GROUP_MCAST_REMAP_ENABLE) ? 1 : 0);
        }

        field = PRUNE_ENABLE_BITMAPf;
        if (SOC_MEM_FIELD_VALID(unit, mem, field)) {
            soc_mem_pbmp_field_get(unit, mem, &ing_entry, field, &prune_pbmp);
            if (config->config_flags &
                    BCM_SWITCH_NETWORK_GROUP_INGRESS_PRUNE_ENABLE) {
                BCM_PBMP_PORT_ADD(prune_pbmp, config->dest_network_group_id);
            } else {
                BCM_PBMP_PORT_REMOVE(prune_pbmp, config->dest_network_group_id);
            }
            soc_mem_pbmp_field_set(unit, mem, &ing_entry, field, &prune_pbmp);
        }

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, COPYNO_ALL,
                    source_network_group_id, &ing_entry));
    }

    BCM_PBMP_CLEAR(prune_pbmp);
    mem = EGR_NETWORK_PRUNE_CONTROLm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, COPYNO_ALL,
                    source_network_group_id, &egr_entry));

        field = PRUNE_ENABLE_BITMAPf;
        if (SOC_MEM_FIELD_VALID(unit, mem, field)) {
            soc_mem_pbmp_field_get(unit, mem, &egr_entry, field, &prune_pbmp);
            if (config->config_flags &
                    BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE) {
                BCM_PBMP_PORT_ADD(prune_pbmp, config->dest_network_group_id);
            } else {
                BCM_PBMP_PORT_REMOVE(prune_pbmp, config->dest_network_group_id);
            }
            soc_mem_pbmp_field_set(unit, mem, &egr_entry, field, &prune_pbmp);
        }

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, COPYNO_ALL,
                    source_network_group_id, &egr_entry));

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit,soc_feature_flex_flow) &&
            soc_property_get(unit, spn_IP6_VXLAN_MSHG_ENABLE, 0)) {
            rv = bcmi_esw_flow_mshg_prune_egress_set(unit,
                source_network_group_id, config->dest_network_group_id,
                config->config_flags &
                BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE);
        }
#endif
    }

    return rv;
}

#ifdef BCM_RIOT_SUPPORT

/*
 * Function:
 *      _bcm_vp_is_vfi_type
 * Purpose: 
 *      Finds out if GPORT is VFI type virtual port.
 * Parameters:
 *      unit  - SOC device unit number
 *      gport - gport provided 
 * Returns: 
 *      TRUE/FALSE              
 */

int
_bcm_vp_is_vfi_type(int unit, bcm_gport_t gport)
{
    int gport_type = 0;

    gport_type = BCMI_GPORT_TYPE_GET(gport);

    switch(gport_type) {
        case BCM_GPORT_VXLAN_PORT:
        case BCM_GPORT_L2GRE_PORT:
        case BCM_GPORT_MPLS_PORT:
        case BCM_GPORT_MIM_PORT:
        case BCM_GPORT_FLOW_PORT:
            return 1;
        default:
             return 0;
    }
}

/*
 * Function:
 *      _bcm_vp_vfi_type_vp_get
 * Purpose:
 *      Checks and gets a valid vp.
 * Parameters:
 *      unit - SOC device unit number
 *      gport - (IN)gport provided 
 *      vp    - (Out) Physical port decoded from gport
 * Returns:
 *      TRUE/FALSE              
 */
int
_bcm_vp_vfi_type_vp_get(int unit, bcm_gport_t gport, int *vp)
{
    int gport_type = 0;

    gport_type = BCMI_GPORT_TYPE_GET(gport);

    switch(gport_type) {
        case BCM_GPORT_VXLAN_PORT:
            *vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
            break;
        case BCM_GPORT_L2GRE_PORT:
            *vp = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
            break;
        case BCM_GPORT_MPLS_PORT:
            *vp = BCM_GPORT_MPLS_PORT_ID_GET(gport);
            break;
        case BCM_GPORT_MIM_PORT:
            *vp = BCM_GPORT_MIM_PORT_ID_GET(gport);
            break;
        case BCM_GPORT_FLOW_PORT:
            *vp = BCM_GPORT_FLOW_PORT_ID_GET(gport);
            break;
        default:
             return 0;
    }
    return 1;
}

/*
 * Function:
 *      _bcm_vp_lag_member_is_vfi_type
 * Purpose:
 *      Finds out if member of VP-LAG is VFI type virtual port.
 * Parameters:
 *      unit  - SOC device unit number
 *      gport - gport provided
 * Returns:
 *      TRUE/FALSE
 */
int
_bcm_vp_lag_member_is_vfi_type(int unit, bcm_gport_t gport)
{
    int is_vfi_type = 0;

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        if (BCM_GPORT_IS_TRUNK(gport)) {
            bcm_trunk_t tid = BCM_TRUNK_INVALID;
            int tid_is_vp_lag = 0;
            int rv = BCM_E_NONE;

            tid = BCM_GPORT_TRUNK_GET(gport);
            if (BCM_TRUNK_INVALID != tid) {
                rv = _bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag);
                if (BCM_SUCCESS (rv) &&  tid_is_vp_lag) {
                       (void)_bcm_td2_vp_lag_member_is_vfi_type(unit, tid, &is_vfi_type);
                }
            }
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return is_vfi_type;
}

#endif /* BCM_RIOT_SUPPORT */

void
_bcm_vp_vfi_set(int unit, int vp, int vfi)
{
    _BCM_VIRTUAL_VP_VFI_MAP(unit, vp) = vfi;
}

int
_bcm_vp_vfi_get(int unit, int vp)
{
    int vpn = _BCM_VIRTUAL_VP_VFI_MAP(unit, vp);
    int vfi = 0;

    if(_BCM_MPLS_VPN_IS_VPLS(vpn)) {
        vfi = vpn;
    }
    return vfi;
}

#else /* INCLUDE_L3 */
typedef int _bcm_esw_trx_virtual_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */
