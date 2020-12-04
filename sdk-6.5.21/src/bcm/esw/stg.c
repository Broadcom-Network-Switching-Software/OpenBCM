/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        stg.c
 * Purpose:     Spanning tree group support
 *
 * Multiple spanning trees (MST) is supported in
 * BCM5690, etc.  STGs are created and VLANs may be added to each STG.
 *
 * Per-VLAN spanning tree (PVST) is supported in BCM5632.  This module
 * supports PVST on BCM5632 by having 4k virtual STGs and allowing only
 * a single VLAN per STG.  Before the application can create a second
 * STG, it must remove all but one VLAN from the default STG.
 */

#include <shared/bsl.h>

#include <shared/bitop.h>

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/ptable.h>

#include <bcm/error.h>
#include <bcm/stg.h>

#include <bcm_int/common/lock.h>
#include <bcm_int/control.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stg.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#include <bcm_int/esw_dispatch.h>
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
#include <bcm_int/esw/trident2plus.h>
#endif

/*
 * The STG info structure is protected by BCM_LOCK.
 * The hardware PTABLE and hardware STG table are protected by
 * memory locks in the lower level.
 */

typedef struct bcm_stg_info_s {
    int		init;		/* TRUE if STG module has been initialized */
    soc_mem_t	stg_mem;	/* STG table memory, INVALIDm if none */
    bcm_stg_t	stg_min;	/* STG table min index */
    bcm_stg_t	stg_max;	/* STG table max index */
    bcm_stg_t	stg_defl;	/* Default STG */
    SHR_BITDCL	*stg_bitmap;	/* Bitmap of allocated STGs */
    int		stg_count;	/* Number STGs allocated */

    /*
     * STG map - keep a linked list of VLANs in each STG.
     */
    bcm_vlan_t	*vlan_first;	/* Indexed by STG (also links free list) */
    bcm_vlan_t	*vlan_next;	/* Indexed by VLAN ID */
} bcm_stg_info_t;

static bcm_stg_info_t stg_info[BCM_MAX_NUM_UNITS];

#define CHECK_INIT(unit, si) do {				\
	    if (!SOC_UNIT_VALID(unit)) return BCM_E_UNIT;	\
	    if (si->init == 0) return BCM_E_INIT;		\
	    if (si->init < 0) return si->init;			\
	} while (0);


#define CHECK_STG(si, stg)   					\
        if ((stg) < 0 || (stg) > si->stg_max) 	\
	    return BCM_E_BADID

/*
 * Allocation bitmap macros
 */
#define STG_BITMAP_TST(si, stg)		SHR_BITGET(si->stg_bitmap, stg)
#define STG_BITMAP_SET(si, stg)		SHR_BITSET(si->stg_bitmap, stg)
#define STG_BITMAP_CLR(si, stg)		SHR_BITCLR(si->stg_bitmap, stg)

/*
 * STG-to-VLAN Reverse Map
 *
 *   The Spanning Tree Group map is a data structure that forms one
 *   linked list per spanning tree group to keep track of all the VLANs
 *   in each Spanning Tree Group.  Otherwise, to answer the question
 *   'what VLANs are in STG x' would require searching all 4k entries of
 *   the hardware VLAN table.
 *
 *   vlan_first[stg_num] is the number of the first VLAN in STG stg_num.
 *
 *   vlan_next[vlan_id] has one entry per VLAN ID and contains the number
 *   of the next VLAN in the STG that contains vlan_id.
 */

/*
 * Function:
 *	_bcm_vlan_vfi_index_get(internal)
 * Purpose:
 *	Transform vlan/vpn to vlan/vfi index
 */
STATIC uint32
_bcm_stg_vlan_vfi_get(int vlan_vpn)
{
    uint32 vlan_vfi = -1;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (_BCM_VPN_VFI_IS_SET(vlan_vpn)) {
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan_vpn);
        vlan_vfi += BCM_VLAN_COUNT;
    } else
#endif
    {
        vlan_vfi = vlan_vpn;
    }

    return vlan_vfi;
}

/*
 * Function:
 *	_bcm_stg_vlan_vpn_get(internal)
 * Purpose:
 *	Transform vlan/vfi index to vlan/vpn
 */
STATIC uint32
_bcm_stg_vlan_vpn_get(int vlan_vfi)
{
    uint32 vlan_vpn = -1;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (vlan_vfi > BCM_VLAN_MAX) {
        vlan_vpn = vlan_vfi - BCM_VLAN_COUNT;
        _BCM_VPN_SET(vlan_vpn, _BCM_VPN_TYPE_VFI, vlan_vpn);
    } else
#endif
    {
        vlan_vpn = vlan_vfi;
    }

    return vlan_vpn;
}

/*
 * Function:
 *	_bcm_stg_vlan_vfi_count_get(internal)
 * Purpose:
 *	Get vlan and vfi count
 */
STATIC uint32
_bcm_stg_vlan_vfi_count_get(int unit)
{
    int count = BCM_VLAN_COUNT;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        count += soc_mem_index_count(unit, VFIm);
    }
#endif

    return count;
}

#ifdef BROADCOM_DEBUG

/*
 * Function:
 *	_bcm_stg_map_check (internal)
 * Purpose:
 *	Return TRUE if STG contains VLAN
 */
int
_bcm_stg_map_check(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    bcm_vlan_t		v = si->vlan_first[stg];
    int vlan_vfi = _bcm_stg_vlan_vfi_get(vid);
    int vlan_vfi_count = _bcm_stg_vlan_vfi_count_get(unit);

    while (v != vlan_vfi_count) {
        if (v == vlan_vfi) {
            return TRUE;
        } else {
            v = si->vlan_next[v];
        }
    }

    return FALSE;
}

#endif	/* BROADCOM_DEBUG */

/*
 * Function:
 *	_bcm_stg_map_add (internal)
 * Purpose:
 *	Add VLAN to STG linked list
 */

STATIC void
_bcm_stg_map_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int vlan_vfi = _bcm_stg_vlan_vfi_get(vid);

    si->vlan_next[vlan_vfi] = si->vlan_first[stg];
    si->vlan_first[stg] = vlan_vfi;
}

/*
 * Function:
 *	_bcm_stg_map_delete (internal)
 * Purpose:
 *	Remove VLAN from STG linked list; return boolean TRUE if found
 */

STATIC void
_bcm_stg_map_delete(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    bcm_vlan_t		*vp;
    int vlan_vfi = _bcm_stg_vlan_vfi_get(vid);
    int vlan_vfi_count = _bcm_stg_vlan_vfi_count_get(unit);

    vp = &si->vlan_first[stg];
    while (*vp != vlan_vfi_count) {
        if (*vp == vlan_vfi) {
            *vp = si->vlan_next[*vp];
        } else {
            vp = &si->vlan_next[*vp];
        }
    }
}

#if defined (BCM_EASY_RELOAD_SUPPORT)
/*
 * Function:
 *    _bcm_stg_map_get (internal)
 * Purpose:
 *    Given a VLAN with vid, return the STG
 */

STATIC bcm_stg_t
_bcm_stg_map_get(int unit, bcm_vlan_t vid)
{
    bcm_stg_t           stg = BCM_STG_DEFAULT;
    bcm_stg_info_t    *si = &stg_info[unit];
    bcm_vlan_t                *vp;
    int vlan_vfi = _bcm_stg_vlan_vfi_get(vid);
    int vlan_vfi_count = _bcm_stg_vlan_vfi_count_get(unit);

    for (stg = si->stg_min; stg <= si->stg_max; stg++) {
        vp = &si->vlan_first[stg];
        while (*vp != vlan_vfi_count) {
            if (*vp == vlan_vfi) {
                return stg;
            } else {
                vp = &si->vlan_next[*vp];
            }
        }
    }
    return BCM_STG_DEFAULT;
}
#endif
 
/*
 * Function:
 *	_bcm_stg_pvp_translate (internal)
 * Purpose:
 *	Translate from hardware PVP_xxx state to BCM_STG_STP_xxx state
 * Parameters: 
 *   unit       - (IN) Bcm device number. 
 *   pvp_state  - (IN) HW STP state encoding. 
 *   bcm_state  - (OUT) API STP state. 
 * Return: 
 *   BCM_E_XXX  
 */

int
_bcm_stg_pvp_translate(int unit, int pvp_state, int *bcm_state)
{
    COMPILER_REFERENCE(unit);

    if (NULL == bcm_state) {
        return (BCM_E_PARAM);
    }

    switch (pvp_state) {
      case PVP_STP_FORWARDING:
          *bcm_state = BCM_STG_STP_FORWARD;
          break;
      case PVP_STP_BLOCKING:
          *bcm_state = BCM_STG_STP_BLOCK;
          break;
      case PVP_STP_LEARNING:
          *bcm_state = BCM_STG_STP_LEARN;
          break;
      case PVP_STP_DISABLED:
          *bcm_state = BCM_STG_STP_DISABLE;
          break;
      default:
          return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *	_bcm_stg_stp_translate (internal)
 * Purpose:
 *	 Translate from BCM_STG_STP_xxx state to hardware PVP_xxx state
 * Parameters: 
 *   unit       - (IN) Bcm device number. 
 *   bcm_state  - (IN) API STP state. 
 *   pvp_state  - (OUT) HW STP state encoding. 
 * Return: 
 *   BCM_E_XXX  
 */

int
_bcm_stg_stp_translate(int unit, int bcm_state, int *pvp_state)
{
    COMPILER_REFERENCE(unit);
    
    if (NULL == pvp_state) {
        return (BCM_E_PARAM);
    }

    switch (bcm_state) {
      case BCM_STG_STP_FORWARD:
          *pvp_state = PVP_STP_FORWARDING;
          break;
      case BCM_STG_STP_LISTEN:
      case BCM_STG_STP_BLOCK:
          *pvp_state = PVP_STP_BLOCKING;
          break;
      case BCM_STG_STP_LEARN:
          *pvp_state = PVP_STP_LEARNING;
          break;
      case BCM_STG_STP_DISABLE:
          *pvp_state = PVP_STP_DISABLED;
          break;
      default:
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *	_bcm_stg_vid_compar
 * Purpose:
 *	Internal utility routine for sorting on VLAN ID.
 */

STATIC int
_bcm_stg_vid_compar(void *a, void *b)
{
    uint16	a16, b16;

    a16 = *(uint16 *)a;
    b16 = *(uint16 *)b;

    if (a16 < b16) {
        return -1;
    }

    if (a16 > b16) {
        return 1;
    }

    return 0;
}

/*
 * Function:
 *	bcm_stg_vlan_list
 * Purpose:
 *	Return a list of VLANs in a specified STG.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *	stg - STG ID to list
 *      list - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero VLANs returned.
 *      count - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero VLANs returned.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *	The caller is responsible for freeing the memory that is
 *	returned, using bcm_stg_vlan_list_destroy().
 */

int
bcm_esw_stg_vlan_list(int unit, bcm_stg_t stg, bcm_vlan_t **list, int *count)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    bcm_vlan_t		v;
    int			n;
    int vlan_vfi_count = _bcm_stg_vlan_vfi_count_get(unit);

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    BCM_LOCK(unit);
    if (!STG_BITMAP_TST(si, stg)) {
        BCM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    *list = NULL;
    *count = 0;

    /* Traverse list once just to get an allocation count */
    if (si->stg_mem == INVALIDm) {	/* Strata1 */
        (*count) = 4096;
    } else {
        v = si->vlan_first[stg];

        while (v != vlan_vfi_count) {
            (*count)++;
            v = si->vlan_next[v];
        }

        if (*count == 0) {
            BCM_UNLOCK(unit);
            return BCM_E_NONE; /* Return empty list */
        }
    }

    *list = sal_alloc(*count * sizeof (bcm_vlan_t), "bcm_stg_vlan_list");
    if (*list == NULL) {
        BCM_UNLOCK(unit);
        return BCM_E_MEMORY;
    }

    /* Traverse list a second time to record the VLANs */
    if (si->stg_mem == INVALIDm) {	/* Strata1 */
        for (n = 0; n < 4096; n++) {
            (*list)[n] = n;
        }
    } else {
        v = si->vlan_first[stg];
        n = 0;

        while (v != vlan_vfi_count) {
            (*list)[n++] = _bcm_stg_vlan_vpn_get(v);
            v = si->vlan_next[v];
        }
    }

    BCM_UNLOCK(unit);

    _shr_sort(*list, *count, sizeof (bcm_vlan_t), _bcm_stg_vid_compar);

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_stg_vlan_list_destroy
 * Purpose:
 *	Destroy a list returned by bcm_stg_vlan_list.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      list - Pointer to VLAN array to be destroyed.
 *      count - Number of entries in array.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_stg_vlan_list_destroy(int unit, bcm_vlan_t *list, int count)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    if (list != NULL) {
	sal_free(list);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_stg_default_get
 * Purpose:
 *	Returns the default STG for the chip, usually 0 or 1 depending
 *	on the architecture.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *	stg_ptr - (OUT) STG ID for default.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_stg_default_get(int unit, bcm_stg_t *stg_ptr)
{
    bcm_stg_info_t	*si = &stg_info[unit];

    CHECK_INIT(unit, si);

    *stg_ptr = si->stg_defl;

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_stg_default_set
 * Purpose:
 *	Changes the default STG for the chip.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *	stg - STG ID to become default.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *	The specified STG must already exist.
 */

int
bcm_esw_stg_default_set(int unit, bcm_stg_t stg)
{
    bcm_stg_info_t	*si = &stg_info[unit];

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    BCM_LOCK(unit);

    if (!STG_BITMAP_TST(si, stg)) {
	BCM_UNLOCK(unit);
	return BCM_E_NOT_FOUND;
    }

    si->stg_defl = stg;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    BCM_UNLOCK(unit);

    return BCM_E_NONE;
}

/* Function:
 *      _bcm_stg_mstp_vlan_update
 * Purpose:
 *      For systems with MSTP mask, update the mask info for the VLAN.
 */
STATIC int
_bcm_stg_mstp_vlan_update(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    int                 stp_state;
    bcm_port_t          port;
    bcm_pbmp_t          stp_pbmp;

    BCM_PBMP_CLEAR(stp_pbmp);
    BCM_PBMP_ASSIGN(stp_pbmp, PBMP_E_ALL(unit));

#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update (unit, &stp_pbmp);
    }
#endif


    PBMP_ITER(stp_pbmp, port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_stg_stp_get(unit, stg, port, &stp_state));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_stg_vlan_add
 * Purpose:
 *	Main part of bcm_stg_vlan_add; assumes locks already done.
 */

STATIC int
_bcm_stg_vlan_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    bcm_stg_t		stg_cur;

    /* STG must already exist */
    if (!STG_BITMAP_TST(si, stg)) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_RELOADING(unit)) {
#if defined (BCM_EASY_RELOAD_SUPPORT)
        stg_cur = _bcm_stg_map_get(unit, vid);
#endif
    } else {
         BCM_IF_ERROR_RETURN
              (mbcm_driver[unit]->mbcm_vlan_stg_get(unit, vid, &stg_cur));
    }

    /*
     * If this is being called from bcm_vlan_create(), the map will not
     * contain the VLAN but this will not hurt.
     */

    if (si->stg_mem != INVALIDm) {	/* !Strata1 */
        _bcm_stg_map_delete(unit, stg_cur, vid);
    }

    BCM_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_vlan_stg_set(unit, vid, stg));

    if (si->stg_mem != INVALIDm) {	/* !Strata1 */
        _bcm_stg_map_add(unit, stg, vid);
    }

    if (soc_feature(unit, soc_feature_mstp_mask)) {
        BCM_IF_ERROR_RETURN
            (_bcm_stg_mstp_vlan_update(unit, stg, vid));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_stg_vlan_add
 * Purpose:
 *	Add a VLAN to a spanning tree group.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *	stg - STG ID to use
 *      vid - VLAN id to be added to STG
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *	Spanning tree group ID must have already been created.
 *	The VLAN is removed from the STG it is currently in.
 */

int
bcm_esw_stg_vlan_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int			rv;

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    /* VLAN/VFI check */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_vfi_membership) &&
        _BCM_VPN_VFI_IS_SET(vid) && SOC_MEM_IS_VALID(unit, VFIm)) {
        bcm_vpn_t vpn_min = 0;
        int num_vfi = soc_mem_index_count(unit, VFIm);

        _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
        if ((vid > (vpn_min+num_vfi-1)) || (vid < vpn_min)) {
            return (BCM_E_PARAM);
        }
    } else
#endif
    {
        VLAN_CHK_ID(unit, vid);
    }

    BCM_LOCK(unit);
    rv = _bcm_stg_vlan_add(unit, stg, vid);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	_bcm_stg_vlan_remove
 * Purpose:
 *	Main part of bcm_stg_vlan_remove; assumes lock already done.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_stg_vlan_remove(int unit, bcm_stg_t stg, bcm_vlan_t vid, int destroy)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int			stg_cur;

    /* STG must already exist */

    if (!STG_BITMAP_TST(si, stg)) {
	return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_vlan_stg_get(unit, vid, &stg_cur));

    if (stg != stg_cur) {
        return BCM_E_NOT_FOUND;	/* Not found in specified STG */
    }

    BCM_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_vlan_stg_set(unit, vid, si->stg_defl));

    if (si->stg_mem != INVALIDm) {	/* !Strata1 */
        _bcm_stg_map_delete(unit, stg, vid);
    }

    if (!destroy) {
        _bcm_stg_map_add(unit, si->stg_defl, vid);
    }

    if (soc_feature(unit, soc_feature_mstp_mask)) {
        BCM_IF_ERROR_RETURN
            (_bcm_stg_mstp_vlan_update(unit, si->stg_defl, vid));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_stg_vlan_destroy
 * Purpose:
 *	Remove a VLAN from a spanning tree group.
 *	The VLAN is NOT placed in the default spanning tree group.
 *      Intended for use when the VLAN is destroyed.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *	stg - STG ID to use
 *      vid - VLAN id to be removed from STG
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_stg_vlan_destroy(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int			rv;

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    /* VLAN/VFI check */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (_BCM_VPN_VFI_IS_SET(vid) && SOC_MEM_IS_VALID(unit, VFIm)) {
        bcm_vpn_t vpn_min = 0;
        int num_vfi = soc_mem_index_count(unit, VFIm);

        _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
        if ((vid > (vpn_min+num_vfi-1)) || (vid < vpn_min)) {
            return (BCM_E_PARAM);
        }
    } else
#endif
    {
        VLAN_CHK_ID(unit, vid);
    }

    BCM_LOCK(unit);

    rv = _bcm_stg_vlan_remove(unit, stg, vid, TRUE);

    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_stg_vlan_remove
 * Purpose:
 *	Remove a VLAN from a spanning tree group.
 *	The VLAN is placed in the default spanning tree group.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *	stg - STG ID to use
 *      vid - VLAN id to be removed from STG
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_stg_vlan_remove(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int			rv;

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    /* VLAN/VFI check */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_vfi_membership) &&
        _BCM_VPN_VFI_IS_SET(vid) && SOC_MEM_IS_VALID(unit, VFIm)) {
        bcm_vpn_t vpn_min = 0;
        int num_vfi = soc_mem_index_count(unit, VFIm);

        _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
        if ((vid > (vpn_min+num_vfi-1)) || (vid < vpn_min)) {
            return (BCM_E_PARAM);
        }
    } else
#endif
    {
        VLAN_CHK_ID(unit, vid);
    }

    BCM_LOCK(unit);
    rv = _bcm_stg_vlan_remove(unit, stg, vid, FALSE);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_stg_vlan_remove_all
 * Purpose:
 *	Remove all VLAN from a spanning tree group.
 *	The VLANs are placed in the default spanning tree group.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *	stg - STG ID to clear
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_stg_vlan_remove_all(int unit, bcm_stg_t stg)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int rv = BCM_E_NONE;
    bcm_vlan_t		vid, vlan_vpn;
    int vlan_vfi_count = _bcm_stg_vlan_vfi_count_get(unit);

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    BCM_LOCK(unit);

    if (stg == si->stg_defl) {		/* Null operation */
        BCM_UNLOCK(unit);
        return BCM_E_NONE;
    }

    if (!STG_BITMAP_TST(si, stg)) {	/* STG must already exist */
        BCM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    while ((vid = si->vlan_first[stg]) != vlan_vfi_count) {
        vlan_vpn = _bcm_stg_vlan_vpn_get(vid);

        if ((rv = _bcm_stg_vlan_remove(unit, stg, vlan_vpn, FALSE)) < 0) {
            BCM_UNLOCK(unit);
            return rv;
        }

        if ((rv = _bcm_stg_vlan_add(unit, si->stg_defl, vlan_vpn)) < 0) {
            BCM_UNLOCK(unit);
            return rv;
        }
    }

    BCM_UNLOCK(unit);

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      _bcm_esw_stg_sync
 * Purpose:
 *      Record STG module persisitent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_stg_sync(int unit)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int                 rv = BCM_E_NONE;
    int			alloc_size;
    soc_scache_handle_t scache_handle;
    uint8               *stg_scache;

    CHECK_INIT(unit, si);

    alloc_size = SHR_BITALLOCSIZE(si->stg_max + 1);
    alloc_size += sizeof(si->stg_defl);
    SOC_SCACHE_HANDLE_SET(scache_handle,
                          unit, BCM_MODULE_STG, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 alloc_size,
                                 &stg_scache, BCM_WB_DEFAULT_VERSION, NULL));

    /* Save bitmap of defined STGs*/
    sal_memcpy(stg_scache, si->stg_bitmap,
               SHR_BITALLOCSIZE(si->stg_max + 1));
    /* Save default STG */
    sal_memcpy(stg_scache + SHR_BITALLOCSIZE(si->stg_max + 1),
                   &(si->stg_defl), sizeof(si->stg_defl));

    return rv;
}

/*
 * Function:
 *	_bcm_stg_reload (internal)
 * Description:
 *      Reload the STG structures from the current hardware configuration.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_stg_reload(int unit)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    bcm_stg_t		stg;
    int			vid, rv, alloc_size;
    soc_scache_handle_t scache_handle;
    uint8               *stg_scache;
    bcm_stg_t           stg_defl;
    int                 vlan_vpn = 0;
    int                 vlan_vfi_count;

    /*
     * Go through the VLAN table to find out which STG is created,
     * and which VLANs belong to a particular STG.
     */

    if (si->stg_mem == INVALIDm) {	/* Strata1 */
        STG_BITMAP_SET(si, BCM_STG_DEFAULT);
        si->stg_count++;
    } else {
        alloc_size = SHR_BITALLOCSIZE(si->stg_max + 1);
        alloc_size += sizeof(si->stg_defl);
        SOC_SCACHE_HANDLE_SET(scache_handle,
                              unit, BCM_MODULE_STG, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 alloc_size,
                                 &stg_scache, BCM_WB_DEFAULT_VERSION, NULL);

        if (BCM_SUCCESS(rv)) {
            /* Get the saved bitmap of defined STGs*/
            sal_memcpy(si->stg_bitmap, stg_scache,
                       SHR_BITALLOCSIZE(si->stg_max + 1));
            sal_memcpy(&stg_defl,
                       stg_scache + SHR_BITALLOCSIZE(si->stg_max + 1),
                       sizeof(stg_defl));

            if (STG_BITMAP_TST(si, stg_defl)) {
                /* If saved default STG exists, change the SW default
                 * to match */
                si->stg_defl = stg_defl;
            }

            /* Count the STGs to prepare for VLAN recovery below */
            for (stg = si->stg_min; stg <= si->stg_max; stg++) {
                if (STG_BITMAP_TST(si, stg)) {
                    si->stg_count++;
                }
            }
        } else if (BCM_E_NOT_FOUND != rv) {
            return rv;
        } else {
            /*
             * For level 1 Warm Boot
             * XGS switches have a special STG 0 entry that is used
             * only for tagged packets with invalid VLAN.
             * STG is marked valid so the bcm_stg_stp_set/get APIs can be used
             * to manage entry 0, but this is generally for internal purposes.
             */

            if (SOC_IS_XGS_SWITCH(unit)) {
                STG_BITMAP_SET(si, 0);
            }
        }

        vlan_vfi_count = _bcm_stg_vlan_vfi_count_get(unit);
        for (vid = 1; vid < vlan_vfi_count; vid++) {
            vlan_vpn = _bcm_stg_vlan_vpn_get(vid);

            if (mbcm_driver[unit]->mbcm_vlan_stg_get(unit, vlan_vpn, &stg) != 0) {
                continue;
            }

            if ((stg < si->stg_min) || (stg > si->stg_max)) {
                /* Out of range */
                continue;
            }

            if (!STG_BITMAP_TST(si, stg)) {
                STG_BITMAP_SET(si, stg);
                si->stg_count++;
            }

            /* Add the VLAN to this STG's VLAN list */
            _bcm_stg_map_add(unit, stg, vlan_vpn);
        }
    }

    si->init = 1;	/* > 0 */

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *	bcm_stg_init
 * Description:
 *      Initialize the STG module according to Initial System Configuration.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_stg_init(int unit)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int			alloc_size, idx;
    int vlan_vfi_count;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (SOC_IS_SHADOW(unit)) {
        si->init = BCM_E_UNAVAIL;
        return BCM_E_NONE;
    }

    if (SOC_IS_XGS12_FABRIC(unit)) {
        si->init = BCM_E_UNAVAIL;
        return BCM_E_NONE;
    }

    if (soc_feature(unit, soc_feature_stg_xgs)) {
        si->stg_mem = STG_TABm;
    } else {
        si->stg_mem = INVALIDm;
    }

    /*
     * Strata 2 and XGS switches have a special STG 0 entry that is used
     * only for tagged packets with invalid VLAN.
     */

    si->stg_min = 1;

    if (si->stg_mem == INVALIDm) {
        si->stg_max = 1;
    } else {
        si->stg_max = soc_mem_index_max(unit, si->stg_mem);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        int                 rv;
        soc_scache_handle_t scache_handle;
        uint8               *stg_scache;

        alloc_size = SHR_BITALLOCSIZE(si->stg_max + 1);
        alloc_size += sizeof(si->stg_defl);

        SOC_SCACHE_HANDLE_SET(scache_handle,
                              unit, BCM_MODULE_STG, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
                                     TRUE,
                                     alloc_size,
                                     &stg_scache, BCM_WB_DEFAULT_VERSION, NULL);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    alloc_size = SHR_BITALLOCSIZE(si->stg_max + 1);
    if (si->stg_bitmap == NULL) {
        si->stg_bitmap = sal_alloc(alloc_size, "STG-bitmap");
    }

    if (si->vlan_first == NULL) {
        si->vlan_first = sal_alloc(
            (si->stg_max + 1) * sizeof (bcm_vlan_t), "STG-vfirst");
    }

    vlan_vfi_count = _bcm_stg_vlan_vfi_count_get(unit);
    if (si->vlan_next == NULL) {
        si->vlan_next = sal_alloc(
            vlan_vfi_count * sizeof(bcm_vlan_t), "STG-vnext");
    }

    if (si->stg_bitmap == NULL ||
        si->vlan_first == NULL ||
        si->vlan_next == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(si->stg_bitmap, 0, alloc_size);
    for (idx = 0; idx < (si->stg_max + 1); idx++) {
        si->vlan_first[idx] = vlan_vfi_count;
    }
    for (idx = 0; idx < vlan_vfi_count; idx++) {
        si->vlan_next[idx] = vlan_vfi_count;
    }

    si->stg_count = 0;

    /*
     * For practical ease of use, the initial default STG is always 1
     * since that ID is valid on all chips.
     */

    si->stg_defl = BCM_STG_DEFAULT;

    si->init = 1;	/* > 0 */

    /*
     * If reloading, recover software structures from hardware
     * configuration.
     */

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return _bcm_stg_reload(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /*
     * XGS switches have a special STG 0 entry that is used
     * only for tagged packets with invalid VLAN.  XGS hardware does not
     * automatically initialize it to all 1s.
     * STG is marked valid so the bcm_stg_stp_set/get APIs can be used
     * to manage entry 0, but this is generally for internal purposes.
     */

    if (SOC_IS_XGS_SWITCH(unit))
    {
        BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_stg_stp_init(unit, 0));
        STG_BITMAP_SET(si, 0);
    }

    /*
     * Create default STG and add all VLANs to it.  Calling this routine
     * is safe because it does not reference other BCM driver modules.
     */

    BCM_IF_ERROR_RETURN(bcm_esw_stg_create_id(unit, si->stg_defl));

    _bcm_stg_map_add(unit, si->stg_defl, BCM_VLAN_DEFAULT);

    return BCM_E_NONE;
}

int _bcm_esw_stg_destroy_all(int unit)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    bcm_stg_t		stg;

    CHECK_INIT(unit, si);
    BCM_LOCK(unit);

    for (stg = si->stg_min; stg <= si->stg_max; stg++)
    {
        if (STG_BITMAP_TST(si, stg))
        {
            bcm_esw_stg_destroy(unit, stg);
        }
    }

    BCM_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_stg_clear
 * Description:
 *      Destroy all STGs
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_stg_clear(int unit)
{
    _bcm_esw_stg_destroy_all(unit);

    return bcm_esw_stg_init(unit);
}

int bcm_esw_stg_detach(int unit)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int rv;

    if ((si->init == 0) || (si->init < 0)) {
        return BCM_E_NONE;
    }

    BCM_LOCK(unit);

    rv = _bcm_esw_stg_destroy_all(unit);

    if (BCM_SUCCESS(rv))
    {
        sal_free(si->vlan_next);
        sal_free(si->vlan_first);
        sal_free(si->stg_bitmap);

        si->vlan_next = NULL;
        si->vlan_first = NULL;
        si->stg_bitmap = NULL;
    }

    BCM_UNLOCK(unit);
    si->init = 0;

    return rv;
}

/*
 * Function:
 *	bcm_stg_create_id
 * Description:
 *      Create a STG, using a specified ID.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      stg - STG to create
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      In the new STG, all ports are in the DISABLED state.
 */

int
bcm_esw_stg_create_id(int unit, bcm_stg_t stg)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int			rv = BCM_E_NONE;

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    BCM_LOCK(unit);

    if (STG_BITMAP_TST(si, stg)) {
        BCM_UNLOCK(unit);
        return BCM_E_EXISTS;
    }

    /* Write an entry with all ports DISABLED */

    if ((rv = mbcm_driver[unit]->mbcm_stg_stp_init(unit, stg)) < 0) {
        BCM_UNLOCK(unit);
        return rv;
    }

    STG_BITMAP_SET(si, stg);
    si->stg_count++;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_stg_create
 * Description:
 *      Create a STG, picking an unused ID and returning it.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      stg_ptr - (OUT) the STG ID.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_stg_create(int unit, bcm_stg_t *stg_ptr)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    bcm_stg_t		stg;
    int			rv;

    CHECK_INIT(unit, si);

    BCM_LOCK(unit);

    for (stg = si->stg_min; stg <= si->stg_max; stg++) {
        if (!STG_BITMAP_TST(si, stg)) {
            break;
        }
    }

    if (stg > si->stg_max) {
        BCM_UNLOCK(unit);
        return BCM_E_FULL;
    }

    rv = bcm_esw_stg_create_id(unit, stg);

    BCM_UNLOCK(unit);

    *stg_ptr = stg;

    return rv;
}

/*
 * Function:
 *	bcm_stg_destroy
 * Description:
 *      Destroy an STG.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      stg - The STG ID to be destroyed.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The default STG may not be destroyed.
 */

int
bcm_esw_stg_destroy(int unit, bcm_stg_t stg)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int			rv;

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    BCM_LOCK(unit);

    if (stg == si->stg_defl) {
        BCM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    if (stg < si->stg_min) {
        /* Do not destroy STG 0 */
        BCM_UNLOCK(unit);
        return BCM_E_BADID;
    }

    /* The next call already checks if STG exists */

    rv = bcm_esw_stg_vlan_remove_all(unit, stg);

    if (rv < 0) {
        BCM_UNLOCK(unit);
        return rv;
    }

    STG_BITMAP_CLR(si, stg);
    si->stg_count--;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    BCM_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_stg_list
 * Purpose:
 *	Return a list of defined STGs
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      list - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero STGs returned.
 *      count - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero STGs returned.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *	The caller is responsible for freeing the memory that is
 *	returned, using bcm_stg_list_destroy().
 */

int
bcm_esw_stg_list(int unit, bcm_stg_t **list, int *count)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    bcm_stg_t		stg;
    int			n;

    CHECK_INIT(unit, si);

    BCM_LOCK(unit);

    if (si->stg_count == 0) {
        BCM_UNLOCK(unit);
        *count = 0;
        *list = NULL;
        return BCM_E_NONE;		/* Empty list */
    }

    *count = si->stg_count;
    *list = sal_alloc(si->stg_count * sizeof (bcm_stg_t), "bcm_stg_list");

    if (*list == NULL) {
        BCM_UNLOCK(unit);
        return BCM_E_MEMORY;
    }

    n = 0;
    for (stg = si->stg_min; stg <= si->stg_max; stg++) {
        if (STG_BITMAP_TST(si, stg)) {
            assert(n < *count);
            (*list)[n++] = stg;
        }
    }

    BCM_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_stg_list_destroy
 * Purpose:
 *	Destroy a list returned by bcm_stg_list.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      list - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero STGs returned.
 *      count - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero STGs returned.
 * Returns:
 *      BCM_E_NONE
 */

int
bcm_esw_stg_list_destroy(int unit, bcm_stg_t *list, int count)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    if (list != NULL) {
        sal_free(list);
    }

    return BCM_E_NONE;
}

/* Function:
 *      _bcm_stg_mstp_stp_update
 * Purpose:
 *      For systems with MSTP mask, update for an STP state change.
 */
STATIC int
_bcm_stg_mstp_stp_update(int unit, bcm_stg_t stg, bcm_port_t port,
                          int stp_state)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    bcm_vlan_t		v = si->vlan_first[stg];
    int vlan_vfi_count = _bcm_stg_vlan_vfi_count_get(unit);

    while (v != vlan_vfi_count) {
        v = si->vlan_next[v];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_stg_stp_set
 * Purpose:
 *      Set the Spanning tree state for a port in specified STG.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *      stg - STG ID.
 *      port - StrataSwitch port number.
 *      stp_state - Spanning Tree State of port.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_stg_stp_set(int unit, bcm_stg_t stg, bcm_port_t port, int stp_state)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int         local_port;
    int			rv = BCM_E_NONE;
    int         hw_stp_state;

    if (SOC_IS_XGS12_FABRIC(unit)) {
        return BCM_E_NONE;
    }

#ifdef BCM_SHADOW_SUPPORT
    /* No STG STP support in shadow device */
    if (SOC_IS_SHADOW(unit)) {
        return BCM_E_NONE;
    }
#endif /* BCM_SHADOW_SUPPORT */

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &local_port));

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    if ((SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
        && IS_LB_PORT(unit, local_port)) {
    } else
#endif
#ifdef BCM_KATANA2_SUPPORT
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
          BCM_PBMP_MEMBER (SOC_INFO(unit).linkphy_pp_port_pbm, local_port)) ||
         (soc_feature(unit, soc_feature_subtag_coe) &&
          BCM_PBMP_MEMBER (SOC_INFO(unit).subtag_pp_port_pbm, local_port)) ||
         (soc_feature(unit, soc_feature_general_cascade) &&
          BCM_PBMP_MEMBER (SOC_INFO(unit).general_pp_port_pbm, local_port))) {
    } else
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership) &&
        !BCM_PORT_IS_LOCAL_PHYSICAL(port)) {
    } else
#endif
    if (!IS_PORT(unit, local_port)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (!SOC_IS_TRIDENT2PLUS(unit) || !IS_CPU_PORT(unit, local_port))
#endif
        return BCM_E_PORT;
    }

    /* Verify passed stp state. */
    BCM_IF_ERROR_RETURN(_bcm_stg_stp_translate(unit, stp_state, &hw_stp_state));

    BCM_LOCK(unit);

    if (!STG_BITMAP_TST(si, stg)) {
        rv = BCM_E_NOT_FOUND;
        goto cleanup;
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (!BCM_PORT_IS_LOCAL_PHYSICAL(port) &&
        soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        int vp_group_found = 0;

        if (BCM_GPORT_IS_VP_GROUP(port)) {
            if (bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
                rv = bcm_td2p_vp_group_stp_state_set(unit,
                            stg, port, FALSE, hw_stp_state);
                if (rv < 0) {
                    goto cleanup;
                }

                vp_group_found++;
            }

            if (bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
                rv = bcm_td2p_vp_group_stp_state_set(
                        unit, stg, port, TRUE, hw_stp_state);
                if (rv < 0) {
                    goto cleanup;
                }

                vp_group_found++;
            }
        } else {
            uint32 filter_flags;

            rv = bcm_esw_port_vlan_member_get(unit, port, &filter_flags);
            if (rv < 0) {
                goto cleanup;
            }

            if (!(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)
                && (filter_flags & BCM_PORT_VLAN_MEMBER_INGRESS)) {
                rv = bcm_td2p_vp_group_stg_move(unit,
                        stg, port, hw_stp_state, FALSE);
                if (rv < 0) {
                    goto cleanup;
                }

                vp_group_found++;
            }

            if (!(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)
                    && (filter_flags & BCM_PORT_VLAN_MEMBER_EGRESS)) {
                rv = bcm_td2p_vp_group_stg_move(unit,
                        stg, port, hw_stp_state, TRUE);
                if (rv < 0) {
                    goto cleanup;
                }

                vp_group_found++;
            }
        }

        if (!vp_group_found) {
            rv = BCM_E_NOT_FOUND;
            goto cleanup;
        }
    } else 
#endif
    {
        rv = mbcm_driver[unit]->mbcm_stg_stp_set(unit, stg, local_port, stp_state);
    }

    if (rv >= 0 && soc_feature(unit, soc_feature_mstp_mask) &&
        IS_E_PORT(unit, local_port)) {
       rv = _bcm_stg_mstp_stp_update(unit, stg, local_port, stp_state);
    }

cleanup:
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_stg_stp_get
 * Purpose:
 *      Get the Spanning tree state for a port in specified STG.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      stg - STG ID.
 *      port - StrataSwitch port number.
 *      stp_state - (Out) Pointer to where Spanning Tree State is stored.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
bcm_esw_stg_stp_get(int unit, bcm_stg_t stg, bcm_port_t port, int *stp_state)
{
    bcm_stg_info_t	*si = &stg_info[unit];
    int			rv = BCM_E_NONE;
    int         local_port;

    if (SOC_IS_XGS12_FABRIC(unit)) {
        *stp_state = BCM_STG_STP_FORWARD;
        return BCM_E_NONE;
    }

    CHECK_INIT(unit, si);
    CHECK_STG(si, stg);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &local_port));

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    if ((SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit))
        && IS_LB_PORT(unit, local_port)) {
    } else
#endif
#ifdef BCM_KATANA2_SUPPORT
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
          BCM_PBMP_MEMBER (SOC_INFO(unit).linkphy_pp_port_pbm, local_port)) ||
         (soc_feature(unit, soc_feature_subtag_coe) &&
          BCM_PBMP_MEMBER (SOC_INFO(unit).subtag_pp_port_pbm, local_port))) {
    } else
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership) &&
        !BCM_PORT_IS_LOCAL_PHYSICAL(port)) {
    } else
#endif
    if (!IS_PORT(unit, local_port)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (!SOC_IS_TRIDENT2PLUS(unit) || !IS_CPU_PORT(unit, local_port))
#endif
        return BCM_E_PORT;
    }

    BCM_LOCK(unit);

    if (!STG_BITMAP_TST(si, stg)) {
        rv = BCM_E_NOT_FOUND;
        goto cleanup;
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (!BCM_PORT_IS_LOCAL_PHYSICAL(port) &&
        soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        int egress;
        int hw_stp_state = PVP_STP_FORWARDING;

        if (BCM_GPORT_IS_VP_GROUP(port)) {
            if (bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
                egress = FALSE;
            } else if (bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
                egress = TRUE;
            } else {
                rv = BCM_E_NOT_FOUND;
                goto cleanup;
            }
        } else {
            uint32 filter_flags;

            rv = bcm_esw_port_vlan_member_get(unit, port, &filter_flags);
            if (rv < 0) {
                goto cleanup;
            }

            if (!(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)
                && (filter_flags & BCM_PORT_VLAN_MEMBER_INGRESS)) {
                egress = FALSE;
            } else if (!(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)
                    && (filter_flags & BCM_PORT_VLAN_MEMBER_EGRESS)) {
                egress = TRUE;
            } else {
                rv = BCM_E_NOT_FOUND;
                goto cleanup;
            }
        }

        rv = bcm_td2p_vp_group_stp_state_get(unit, stg, port, egress, &hw_stp_state);
        if (rv < 0) {
            goto cleanup;
        }

        /* Translate from hardware PVP_xxx state to BCM_STG_STP_xxx state. */
        rv = _bcm_stg_pvp_translate(unit, hw_stp_state, stp_state);
    } else 
#endif
    {
        rv = mbcm_driver[unit]->mbcm_stg_stp_get(unit, stg, local_port, stp_state);
    }

cleanup:
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *     bcm_stg_count_get
 * Purpose:
 *     Get the maximum number of STG groups the chip supports
 * Parameters:
 *     unit - StrataSwitch unit number.
 *     max_stg - max number of STG groups supported by this chip
 * Returns:
 *     BCM_E_xxx
 */

int
bcm_esw_stg_count_get(int unit, int *max_stg)
{
    bcm_stg_info_t *si = &stg_info[unit];

    CHECK_INIT(unit, si);

    *max_stg = si->stg_max;

    return BCM_E_NONE;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_stg_sw_dump
 * Purpose:
 *     Displays STG software structure information.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_stg_sw_dump(int unit)
{
    int             i, count;
    bcm_stg_info_t  *si = &stg_info[unit];
    int             vlan_count = 0;
    bcm_vlan_t      v;
    int vlan_vfi_count = _bcm_stg_vlan_vfi_count_get(unit);

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information STG - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Init       : %4d\n"), si->init));
    LOG_CLI((BSL_META_U(unit,
                        "  Memory     : %d(%4d-%4d)\n"),
             si->stg_mem, si->stg_min, si->stg_max));
    LOG_CLI((BSL_META_U(unit,
                        "  Default STG: %4d\n"), si->stg_defl));
    LOG_CLI((BSL_META_U(unit,
                        "  Count      : %4d\n"), si->stg_count));

    if (si->stg_max >= BCM_VLAN_COUNT) {
        LOG_CLI((BSL_META_U(unit,
                            "More STGs than VLANs!\n")));
        si->stg_max = BCM_VLAN_COUNT - 1;
    }

    /* Print STG list sorted */
    count = 0;
    for (i = si->stg_min; i < si->stg_max; i++) {
        if (0 == STG_BITMAP_TST(si, i)){
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "  STG %4d:   "), i));
        count++;
        vlan_count = 0;
        v = si->vlan_first[i];

        while (v != vlan_vfi_count) {
            if ((vlan_count > 0) && (!(vlan_count % 10))) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n              ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %4d"), _bcm_stg_vlan_vpn_get(v)));
            vlan_count++;
            v = si->vlan_next[v];
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
        if (si->stg_count == count) {
            break;
        }
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */
