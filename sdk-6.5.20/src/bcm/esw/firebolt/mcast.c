/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mcast.c
 * Purpose:     Tracks and manages L2 Multicast tables.
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/l2x.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#include <bcm/error.h>
#include <bcm/l2.h>

#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/triumph3.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif
#include <bcm_int/common/multicast.h>

typedef struct {
    int         size;
    int         *used;
    soc_mem_t   l2mem;
    soc_mem_t   mcmem;
} _bcm_xgs3_mcast_t;

static _bcm_xgs3_mcast_t xgs3_l2mc_info[BCM_MAX_NUM_UNITS];

#define L2MC_INFO(unit)          (&xgs3_l2mc_info[unit])
#define L2MC_SIZE(unit)          L2MC_INFO(unit)->size
#define L2MC_USED(unit)          L2MC_INFO(unit)->used
#define L2MC_L2MEM(unit)         L2MC_INFO(unit)->l2mem
#define L2MC_MCMEM(unit)         L2MC_INFO(unit)->mcmem
#define L2MC_USED_SET(unit, n)   L2MC_USED(unit)[n] += 1
#define L2MC_USED_CLR(unit, n)   L2MC_USED(unit)[n] -= 1
#define L2MC_USED_ISSET(unit, n) (L2MC_USED(unit)[n] > 0)

#define L2MC_INIT(unit) \
        if (L2MC_USED(unit) == NULL) { return BCM_E_INIT; }
#define L2MC_ID(unit, id) \
        if (id < 0 || id >= L2MC_SIZE(unit)) { return BCM_E_PARAM; }

#define L2MC_LOCK(unit)         do {            \
        soc_mem_lock(unit, L2MC_MCMEM(unit));   \
    } while(0)
#define L2MC_UNLOCK(unit)       do {            \
        soc_mem_unlock(unit, L2MC_MCMEM(unit)); \
    } while(0)
#define L2MC_ID_GET(l2mc_index) \
    (_BCM_MULTICAST_IS_SET(l2mc_index) &&    \
        _BCM_MULTICAST_IS_L2(l2mc_index)) ?  \
            _BCM_MULTICAST_ID_GET(l2mc_index) : l2mc_index

/*
 * Function:
 *	_bcm_xgs3_l2_addr_delete_mcast
 * Description
 *	Delete L2 multicast entries
 * Parameters:
 *	unit  - device unit
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 */
int
_bcm_xgs3_l2_addr_delete_mcast(int unit, uint32 flags)
{
    l2x_entry_t *l2xe, *l2xep;
    int		rv, chunksize, chunk, chunk_max, i;
    int		dyn_only, chnk_end, mem_idx_max;
    bcm_mac_t   mac;
    uint32 field_valid = INVALIDf;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
				 L2_MEM_CHUNKS_DEFAULT);

    l2xe = soc_cm_salloc(unit, chunksize * sizeof(*l2xe), "l2del_chunk");
    if (l2xe == NULL) {
	return BCM_E_MEMORY;
    }

    rv = BCM_E_NONE;

    dyn_only = !(flags & BCM_L2_DELETE_STATIC);

    mem_idx_max = soc_mem_index_max(unit, L2Xm);
    for (chunk = soc_mem_index_min(unit, L2Xm); chunk <= mem_idx_max;
         chunk += chunksize) {

        chunk_max = chunk + chunksize - 1;
        if (chunk_max > mem_idx_max) {
            chunk_max = mem_idx_max;
        }

	rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
				chunk, chunk_max, l2xe);
	if (rv < 0) {
	    break;
	}
        chnk_end = (chunk_max - chunk);
	for (i = 0; i <= chnk_end; i++) {

	    l2xep = soc_mem_table_idx_to_pointer(unit, L2Xm,
                                                 l2x_entry_t *, l2xe, i);


        field_valid = VALIDf;
        if (soc_feature(unit, soc_feature_base_valid)) {
            field_valid = BASE_VALIDf;
        }
        if(!soc_L2Xm_field32_get(unit, l2xep, field_valid)) {
            continue;
        }

        soc_L2Xm_mac_addr_get(unit, l2xep, MAC_ADDRf, mac);
        if (!BCM_MAC_IS_MCAST(mac)) {
            continue;
        }

	    if (dyn_only &&
		soc_L2Xm_field32_get(unit, l2xep, STATIC_BITf)) {
		continue;
	    }
	    rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, l2xep);
	    if (rv < 0) {
	        break;
	    }
	}

	if (rv < 0) {
	    break;
	}
    }

    soc_cm_sfree(unit, l2xe);

    return rv;
}

/*
 * Function:
 *      _bcm_xgs3_l2mc_size_get
 * Description:
 *      Get number of L2MC groups.
 */

int
_bcm_xgs3_l2mc_size_get(int unit, int *size)
{
    *size = L2MC_SIZE(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_l2mc_index_is_set
 * Description:
 *      Check whether and l2mc index is used.
 */

int
_bcm_xgs3_l2mc_index_is_set(int unit, int l2mc_id, int *is_set)
{
    L2MC_ID(unit, l2mc_id);

    *is_set = (0 != L2MC_USED_ISSET(unit, l2mc_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_l2mc_free_index
 * Description:
 *      Find an unused l2mc index and mark it used.
 */

int
_bcm_xgs3_l2mc_free_index(int unit, int *l2mc_id)
{
    int i, rv;

    rv = BCM_E_FULL;
    for (i = 1; i < L2MC_SIZE(unit); i++) {
        if (!L2MC_USED_ISSET(unit, i)) {
            L2MC_USED_SET(unit, i);
            *l2mc_id = i;
            rv = BCM_E_NONE;
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_xgs3_l2mc_id_alloc
 * Description:
 *      allocate the given l2mc entry (increment use count)
 */

int
_bcm_xgs3_l2mc_id_alloc(int unit, int l2mc_id)
{
    L2MC_ID(unit, l2mc_id);

    L2MC_USED_SET(unit, l2mc_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_l2mc_id_free
 * Description:
 *      Marks an l2mc index as free (decrements use count)
 */

int
_bcm_xgs3_l2mc_id_free(int unit, int l2mc_id)
{
    L2MC_ID(unit, l2mc_id);

    L2MC_USED_CLR(unit, l2mc_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_mcast_from_l2
 * Description:
 *      Fill in fields of bcm_mcast_addr_t from bcm_l2_addr_t
 * Parameters:
 *      unit    - (IN) device
 *      mcaddr  - (OUT) mcast addr struct
 *      l2addr  - (IN) l2 addr struct
 */

STATIC void
_bcm_xgs3_mcast_from_l2(int unit,
                        bcm_mcast_addr_t *mcaddr,
                        bcm_l2_addr_t  *l2addr)
{
    int         rv;
    vlan_tab_entry_t vte;
    soc_mem_t   vlan_mem;

    sal_memcpy(mcaddr->mac, l2addr->mac, sizeof(bcm_mac_t));
    mcaddr->vid = l2addr->vid;
    mcaddr->cos_dst = l2addr->cos_dst;
    mcaddr->l2mc_index = l2addr->l2mc_group;

    /* Need to fill in the untagged port bitmap from the VLAN table */
    SOC_PBMP_CLEAR(mcaddr->ubmp);
    vlan_mem = SOC_IS_FBX(unit) ? EGR_VLANm : VLAN_TABm;
    rv = soc_mem_read(unit, vlan_mem, MEM_BLOCK_ANY, mcaddr->vid, &vte);
    if (rv < 0) {
        return;
    }
#if defined (BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        (void)bcm_td3_vlan_table_ut_port_get(unit, mcaddr->vid, &mcaddr->ubmp);
    } else
#endif
    {
        soc_mem_pbmp_field_get(unit, vlan_mem, &vte, UT_BITMAPf, &mcaddr->ubmp);
    }
}

/*
 * Function:
 *      _bcm_xgs3_mcast_to_l2
 * Description:
 *      Fill in fields of bcm_l2_addr_t from bcm_mcast_addr_t
 * Parameters:
 *      unit -  (IN) device
 *      l2addr - (OUT) Pointer to bcm_l2_addr_t
 *      mcaddr - (IN) Pointer to bcm_mcast_addr_t
 */

STATIC void
_bcm_xgs3_mcast_to_l2(int unit,
                      bcm_l2_addr_t *l2addr,
                      bcm_mcast_addr_t *mcaddr)
{
    bcm_l2_addr_t_init(l2addr, mcaddr->mac, mcaddr->vid);
    l2addr->flags = BCM_L2_STATIC | BCM_L2_MCAST | BCM_L2_REPLACE_DYNAMIC;
    l2addr->cos_dst = mcaddr->cos_dst;
    l2addr->l2mc_group = mcaddr->l2mc_index;

}

/*
 * Function:
 *      _bcm_xgs3_mcast_create
 * Description:
 *      Create a muticast entry
 * Parameters:
 *      unit -  (IN) device
 *      mcaddr - (IN) Pointer to bcm_mcast_addr_t
 *      mcindex - (IN) mcast index to use (-1 lets routine choose)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_xgs3_mcast_create(int unit, bcm_mcast_addr_t *mcaddr,
                       int mcindex)
{
    l2mc_entry_t        l2mc, rl2mc;
    bcm_l2_addr_t       l2addr, rl2addr;
    bcm_pbmp_t          pbm, rpbm;
    int                 rv;

    bcm_l2_addr_t_init (&l2addr, mcaddr->mac, mcaddr->vid);
    rv = bcm_esw_l2_addr_get(unit, mcaddr->mac, mcaddr->vid, &l2addr);
    if (rv >= 0) {
        if (!(l2addr.flags & BCM_L2_MCAST)) {
            return BCM_E_EXISTS;
        }
        l2addr.l2mc_group = L2MC_ID_GET(l2addr.l2mc_group);
        if (mcindex >= 0 && mcindex == l2addr.l2mc_group) {
            return BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN(_bcm_xgs3_l2mc_id_free(unit, l2addr.l2mc_group));

    }

    /* Before getting free MC index, make sure this is a valid Port */
    rv = soc_mem_field_pbmp_fit(unit, L2MC_MCMEM(unit), PORT_BITMAPf,
                               (uint32 *)&mcaddr->pbmp);
    if (rv != SOC_E_NONE) {
        return rv;
    }

    if (mcindex < 0) {
        BCM_IF_ERROR_RETURN(_bcm_xgs3_l2mc_free_index(unit, &mcindex));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_xgs3_l2mc_id_alloc(unit, mcindex));
    }

    sal_memset(&l2mc, 0, sizeof(l2mc));
    soc_mem_field32_set(unit, L2MC_MCMEM(unit), &l2mc, VALIDf, 1);
    soc_mem_pbmp_field_set(unit, L2MC_MCMEM(unit), &l2mc, PORT_BITMAPf,
                           &mcaddr->pbmp);
    _bcm_xgs3_mcast_to_l2(unit, &l2addr, mcaddr);
    l2addr.l2mc_group = mcindex;

    if (SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit)) {
        sal_memset(&rl2addr, 0, sizeof(bcm_l2_addr_t));
        rv = bcm_esw_l2_addr_get(unit, (uint8 *)_soc_mac_all_routers,
                    mcaddr->vid, &rl2addr);
        if (rv >= 0) {
            rl2addr.l2mc_group = L2MC_ID_GET(rl2addr.l2mc_group);
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY,
                              rl2addr.l2mc_group, &rl2mc));
            soc_mem_pbmp_field_get(unit, L2MC_MCMEM(unit), &rl2mc,
                                   PORT_BITMAPf, &rpbm);
            soc_mem_pbmp_field_get(unit, L2MC_MCMEM(unit), &l2mc, PORT_BITMAPf,
                                   &pbm);
            BCM_PBMP_OR(pbm, rpbm);
            soc_mem_pbmp_field_set(unit, L2MC_MCMEM(unit), &l2mc, PORT_BITMAPf,
                                   &pbm);
        }
    }

    /* add l2mc entry */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, L2MC_MCMEM(unit), MEM_BLOCK_ALL, mcindex, &l2mc));

    /* add l2 entry */
    rv = bcm_esw_l2_addr_add(unit, &l2addr);
    if (BCM_FAILURE(rv)) {
        _bcm_xgs3_l2mc_id_free(unit, l2addr.l2mc_group);
        if (!L2MC_USED_ISSET(unit, l2addr.l2mc_group)) {
          soc_mem_write(unit, L2MC_MCMEM(unit), MEM_BLOCK_ALL,
                        l2addr.l2mc_group,
                        soc_mem_entry_null(unit, L2MC_MCMEM(unit)));
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_xgs3_mcast_port_add
 * Purpose:
 *      Add new ports to a multicast entry
 * Parameters:
 *      unit    - device
 *      mcaddr  - mcast addr struct with new ports
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_xgs3_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr)
{
    bcm_l2_addr_t       l2addr;
    l2mc_entry_t        l2mc_entry;
    bcm_pbmp_t          pbmp;

    bcm_l2_addr_t_init(&l2addr, mcaddr->mac, mcaddr->vid);
    BCM_IF_ERROR_RETURN
            (bcm_esw_l2_addr_get(unit, mcaddr->mac, mcaddr->vid, &l2addr));

    if (!(l2addr.flags & BCM_L2_MCAST)) {
        return BCM_E_NOT_FOUND;
    }

    l2addr.l2mc_group = L2MC_ID_GET(l2addr.l2mc_group);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY,
                      l2addr.l2mc_group, &l2mc_entry));

    soc_mem_pbmp_field_get(unit, L2MC_MCMEM(unit), &l2mc_entry, PORT_BITMAPf,
                           &pbmp);
    BCM_PBMP_OR(pbmp, mcaddr->pbmp);
    soc_mem_pbmp_field_set(unit, L2MC_MCMEM(unit), &l2mc_entry, PORT_BITMAPf,
                           &pbmp);

    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, L2MC_MCMEM(unit), MEM_BLOCK_ALL,
                       l2addr.l2mc_group, &l2mc_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_mcast_port_delete
 * Purpose:
 *      Remove ports from a multicast entry
 * Parameters:
 *      unit    - device
 *      mcaddr  - mcast addr struct with ports to delete
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_xgs3_mcast_port_delete(int unit,
                            bcm_mcast_addr_t *mcaddr)
{
    bcm_l2_addr_t       l2addr;
    l2mc_entry_t        l2mc_entry;
    bcm_pbmp_t          pbmp;

    bcm_l2_addr_t_init (&l2addr, mcaddr->mac, mcaddr->vid);
    BCM_IF_ERROR_RETURN
            (bcm_esw_l2_addr_get(unit, mcaddr->mac, mcaddr->vid, &l2addr));

    if (!(l2addr.flags & BCM_L2_MCAST)) {
        return BCM_E_NOT_FOUND;
    }

    l2addr.l2mc_group = L2MC_ID_GET(l2addr.l2mc_group);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY,
                      l2addr.l2mc_group, &l2mc_entry));

    soc_mem_pbmp_field_get(unit, L2MC_MCMEM(unit), &l2mc_entry, PORT_BITMAPf,
                           &pbmp);
    BCM_PBMP_REMOVE(pbmp, mcaddr->pbmp);
    soc_mem_pbmp_field_set(unit, L2MC_MCMEM(unit), &l2mc_entry, PORT_BITMAPf,
                           &pbmp);

    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, L2MC_MCMEM(unit), MEM_BLOCK_ALL,
                       l2addr.l2mc_group, &l2mc_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_xgs3_mcast_port_remove
 * Purpose:
 *      Remove ports from existing MC entry.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_xgs3_mcast_port_remove(int unit, bcm_mcast_addr_t *mcaddr)
{
    int rv;

    L2MC_INIT(unit);
    L2MC_LOCK(unit);
    rv = _bcm_xgs3_mcast_port_delete(unit, mcaddr);
    L2MC_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_xgs3_mcast_port_add
 * Purpose:
 *      Add ports to existing MC entry.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_xgs3_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr)
{
    int rv;

    L2MC_INIT(unit);
    L2MC_LOCK(unit);
    rv = _bcm_xgs3_mcast_port_add(unit, mcaddr);
    L2MC_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_xgs3_mcast_addr_add_w_l2mcindex
 * Purpose:
 *      Add a new MC entry to the L2 and L2MC tables, with given MC index.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_xgs3_mcast_addr_add_w_l2mcindex(int unit, bcm_mcast_addr_t *mcaddr)
{
    int rv;

    L2MC_INIT(unit);
    L2MC_LOCK(unit);
    rv = _bcm_xgs3_mcast_create(unit, mcaddr, mcaddr->l2mc_index);
    L2MC_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_xgs3_mcast_addr_add
 * Purpose:
 *      Add new L2 multicast entry
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_xgs3_mcast_addr_add(int unit, bcm_mcast_addr_t *mcaddr)
{
    int rv;

    L2MC_INIT(unit);
    L2MC_LOCK(unit);
    /*    coverity[negative_returns : FALSE]    */
    rv = _bcm_xgs3_mcast_create(unit, mcaddr, -1);
    L2MC_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_xgs3_mcast_addr_remove
 * Purpose:
 *      Remove a multicast entry
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_xgs3_mcast_addr_remove(int unit, sal_mac_addr_t mac, bcm_vlan_t vid)
{
    int                 rv;
    bcm_l2_addr_t       l2addr;

    L2MC_INIT(unit);
    L2MC_LOCK(unit);

    bcm_l2_addr_t_init (&l2addr, mac, vid);
    rv = bcm_esw_l2_addr_get(unit, mac, vid, &l2addr);
    if (rv < 0) {
        L2MC_UNLOCK(unit);
        return rv;
    }

    if (!(l2addr.flags & BCM_L2_MCAST)) {
        L2MC_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    rv = bcm_esw_l2_addr_delete(unit, mac, vid);
    if (rv < 0) {
        L2MC_UNLOCK(unit);
        return rv;
    }

    /* Check added to avoid Segmentation fault in regression tests */
    /* Sometimes l2mc_index may not have a multicast type set when switchcontrol L2McIdxRetType is set to 0*/
    if (_BCM_MULTICAST_IS_L2(l2addr.l2mc_group) || 
        !_BCM_MULTICAST_IS_SET(l2addr.l2mc_group)) {
         l2addr.l2mc_group = L2MC_ID_GET(l2addr.l2mc_group);
         _bcm_xgs3_l2mc_id_free(unit, l2addr.l2mc_group);

         if (!L2MC_USED_ISSET(unit, l2addr.l2mc_group)) {
              rv = soc_mem_write(unit, L2MC_MCMEM(unit), MEM_BLOCK_ALL,
                       l2addr.l2mc_group,
                       soc_mem_entry_null(unit, L2MC_MCMEM(unit)));
         }
    }

    L2MC_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_fb_mcast_addr_remove_w_l2mcindex
 * Purpose:
 *      Remove a multicast entry with l2mc index provided
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_xgs3_mcast_addr_remove_w_l2mcindex(int unit,
                                        bcm_mcast_addr_t *mcaddr)
{
    return bcm_xgs3_mcast_addr_remove(unit, mcaddr->mac, mcaddr->vid);
}

/*
 * Function:
 *      _bcm_xgs3_mcast_index_port_get
 * Purpose:
 *      Get port bit maps for a multicast group by index.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_xgs3_mcast_index_port_get(int unit, int l2mc_index, 
                              bcm_mcast_addr_t *mcaddr)
{
    l2mc_entry_t      l2mc_entry;
    
    L2MC_INIT(unit);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY,
                      L2MC_ID_GET(l2mc_index), &l2mc_entry));

    soc_mem_pbmp_field_get(unit, L2MC_MCMEM(unit), &l2mc_entry, PORT_BITMAPf,
                           &mcaddr->pbmp);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_xgs3_mcast_port_get
 * Purpose:
 *      Get port bit maps for a multicast group.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_xgs3_mcast_port_get(int unit,
                         sal_mac_addr_t mac, bcm_vlan_t vid,
                         bcm_mcast_addr_t *mcaddr)
{
    l2mc_entry_t      l2mc_entry;
    bcm_l2_addr_t     l2addr;

    L2MC_INIT(unit);

    bcm_l2_addr_t_init (&l2addr, mac, vid);
    BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_get(unit, mac, vid, &l2addr));

    l2addr.l2mc_group = L2MC_ID_GET(l2addr.l2mc_group);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY,
                      l2addr.l2mc_group, &l2mc_entry));

    _bcm_xgs3_mcast_from_l2(unit, mcaddr, &l2addr);
    soc_mem_pbmp_field_get(unit, L2MC_MCMEM(unit), &l2mc_entry, PORT_BITMAPf,
                           &mcaddr->pbmp);

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
/*
 * Function:
 *      _bcm_tr3_mcast_l2_table_recover
 * Purpose:
 *      Recover L2MC group usage from L2 table.
 * Returns:
 *      BCM_E_XXX 
 */
STATIC int
_bcm_tr3_mcast_l2_table_recover(int unit)
{
    int             rv = BCM_E_NONE;
    int             buf_size, chunksize;
    uint32          *l2_tbl_chnk;
    soc_mem_t       mem;
    int             chnk_idx, chnk_idx_max, mem_idx_max;
    int             chnk_end, ent_idx;
    uint32          *l2_entry;
    int             key_type;
    bcm_mac_t       mac_addr;
    int             l2mc_idx;
    l2mc_entry_t    mc_entry;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
            L2_MEM_CHUNKS_DEFAULT);
    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_tbl_chnk = soc_cm_salloc(unit, buf_size, "l2 mcast delete");
    if (NULL == l2_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    /* First work on L2_ENTRY_1 */

    mem = L2_ENTRY_1m;
    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
            chnk_idx <= mem_idx_max; 
            chnk_idx += chunksize) {

        sal_memset((void *)l2_tbl_chnk, 0, buf_size);
        chnk_idx_max = ((chnk_idx + chunksize) <= mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (BCM_FAILURE(rv)) {
            break;
        }

        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                    l2_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, l2_entry, VALIDf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry, KEY_TYPEf);
            if (SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE != key_type) {
                continue;
            }

            soc_mem_mac_addr_get(unit, mem, l2_entry, L2__MAC_ADDRf, mac_addr);
            if (!BCM_MAC_IS_MCAST(mac_addr)) {
                continue;
            }

            l2mc_idx = soc_mem_field32_get(unit, mem, l2_entry, L2__L2MC_PTRf);
            rv = soc_mem_read(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY, l2mc_idx,
                    &mc_entry);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (soc_mem_field32_get(unit, L2MC_MCMEM(unit), &mc_entry,
                        VALIDf)) {
                L2MC_USED_SET(unit, l2mc_idx);
            }
        }
    }

    if (BCM_FAILURE(rv)) {
        sal_free(L2MC_USED(unit));
        soc_cm_sfree(unit, l2_tbl_chnk);
        return rv;  
    }

    /* Then work on L2_ENTRY_2 */

    mem = L2_ENTRY_2m;
    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
            chnk_idx <= mem_idx_max; 
            chnk_idx += chunksize) {

        sal_memset((void *)l2_tbl_chnk, 0, buf_size);
        chnk_idx_max = ((chnk_idx + chunksize) <= mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (BCM_FAILURE(rv)) {
            break;
        }

        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                    l2_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, l2_entry, VALID_0f)) {
                continue;
            }
            if (!soc_mem_field32_get(unit, mem, l2_entry, VALID_1f)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry,
                    KEY_TYPE_0f);
            if (SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE != key_type) {
                continue;
            }

            soc_mem_mac_addr_get(unit, mem, l2_entry, L2__MAC_ADDRf, mac_addr);
            if (!BCM_MAC_IS_MCAST(mac_addr)) {
                continue;
            }

            l2mc_idx = soc_mem_field32_get(unit, mem, l2_entry, L2__L2MC_PTRf);
            rv = soc_mem_read(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY, l2mc_idx,
                    &mc_entry);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (soc_mem_field32_get(unit, L2MC_MCMEM(unit), &mc_entry,
                        VALIDf)) {
                L2MC_USED_SET(unit, l2mc_idx);
            }
        }
    }

    if (BCM_FAILURE(rv)) {
        sal_free(L2MC_USED(unit));
        soc_cm_sfree(unit, l2_tbl_chnk);
        return rv;  
    }

    /* Next work on EXT_L2_ENTRY_1 */

    if (!soc_feature(unit, soc_feature_esm_support)) {
        return BCM_E_NONE;
    }

    mem = EXT_L2_ENTRY_1m;
    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
            chnk_idx <= mem_idx_max; 
            chnk_idx += chunksize) {

        sal_memset((void *)l2_tbl_chnk, 0, buf_size);
        chnk_idx_max = ((chnk_idx + chunksize) <= mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (BCM_FAILURE(rv)) {
            break;
        }

        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                    l2_tbl_chnk, ent_idx);

            if (soc_mem_field32_get(unit, mem, l2_entry, FREEf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry,
                    KEY_TYPEf);
            if (0 != key_type) {
                continue;
            }

            soc_mem_mac_addr_get(unit, mem, l2_entry, MAC_ADDRf, mac_addr);
            if (!BCM_MAC_IS_MCAST(mac_addr)) {
                continue;
            }

            l2mc_idx = soc_mem_field32_get(unit, mem, l2_entry, DESTINATIONf);
            rv = soc_mem_read(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY, l2mc_idx,
                    &mc_entry);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (soc_mem_field32_get(unit, L2MC_MCMEM(unit), &mc_entry,
                        VALIDf)) {
                L2MC_USED_SET(unit, l2mc_idx);
            }
        }
    }

    if (BCM_FAILURE(rv)) {
        sal_free(L2MC_USED(unit));
        soc_cm_sfree(unit, l2_tbl_chnk);
        return rv;  
    }

    /* Finally work on EXT_L2_ENTRY_2 */

    mem = EXT_L2_ENTRY_2m;
    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
            chnk_idx <= mem_idx_max; 
            chnk_idx += chunksize) {

        sal_memset((void *)l2_tbl_chnk, 0, buf_size);
        chnk_idx_max = ((chnk_idx + chunksize) <= mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (BCM_FAILURE(rv)) {
            break;
        }

        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                    l2_tbl_chnk, ent_idx);

            if (soc_mem_field32_get(unit, mem, l2_entry, FREEf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry,
                    KEY_TYPEf);
            if (0 != key_type) {
                continue;
            }

            soc_mem_mac_addr_get(unit, mem, l2_entry, MAC_ADDRf, mac_addr);
            if (!BCM_MAC_IS_MCAST(mac_addr)) {
                continue;
            }

            l2mc_idx = soc_mem_field32_get(unit, mem, l2_entry, DESTINATIONf);
            rv = soc_mem_read(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY, l2mc_idx,
                    &mc_entry);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (soc_mem_field32_get(unit, L2MC_MCMEM(unit), &mc_entry,
                        VALIDf)) {
                L2MC_USED_SET(unit, l2mc_idx);
            }
        }
    }

    if (BCM_FAILURE(rv)) {
        sal_free(L2MC_USED(unit));
    }

    soc_cm_sfree(unit, l2_tbl_chnk);
    return rv;        
}
#endif /* BCM_TRIUMPH3_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_xgs3_mcast_detach
 * Purpose:
 *      De-initialize multicast api components
 * Returns:
 *      BCM_E_XXX 
 */
int
_bcm_xgs3_mcast_detach(int unit)
{
    if (L2MC_USED(unit) != NULL) {
        sal_free(L2MC_USED(unit));
        L2MC_USED(unit) = NULL;
    }
    L2MC_SIZE(unit) = 0;
    L2MC_L2MEM(unit) = INVALIDm;
    L2MC_MCMEM(unit) = INVALIDm;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_xgs3_l2_addr_mcast_delete_all 
 * Purpose:
 *      Delete multicast address entries in l2 table
 * Returns:
 *      BCM_E_XXX 
 */
int
bcm_xgs3_l2_addr_mcast_delete_all(int unit)
{
    /* Delete all multicast entries from L2 */
    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_BCMSIM && !SAL_BOOT_XGSSIM) {
#ifdef BCM_TRIDENT_SUPPORT
        if (soc_feature(unit, soc_feature_l2_bulk_control) &&
                !SAL_BOOT_PLISIM) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_ism_memory)) {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_l2_addr_delete_mcast(unit, TRUE));
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
            if (soc_feature(unit, soc_feature_l2_bulk_unified_table)) {
                l2_bulk_entry_t match_mask;
                l2_bulk_entry_t match_data;
                bcm_mac_t mac_mask;
                int field_len;

                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, L2_BULK_CONTROLr,
                                            REG_PORT_ANY, ACTIONf, 1));
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, L2_BULK_CONTROLr,
                                            REG_PORT_ANY, NUM_ENTRIESf,
                                            soc_mem_index_count(unit, L2Xm)));

                sal_memset(&match_mask, 0, sizeof(match_mask));
                sal_memset(&match_data, 0, sizeof(match_data));

                if (soc_feature(unit, soc_feature_base_valid)) {
                    soc_mem_field32_set(unit, L2_BULKm, &match_mask,
                                        BASE_VALIDf, 1);
                    soc_mem_field32_set(unit, L2_BULKm, &match_data,
                                        BASE_VALIDf, 1);
                } else {
                    soc_mem_field32_set(unit, L2_BULKm, &match_mask,
                                        VALIDf, 1);
                    soc_mem_field32_set(unit, L2_BULKm, &match_data,
                                        VALIDf, 1);
                }

                field_len = soc_mem_field_length(unit, L2_BULKm,
                                                 KEY_TYPEf);
                soc_mem_field32_set(unit, L2_BULKm, &match_mask,
                                    KEY_TYPEf, (1 << field_len) - 1);
                /* KEY_TYPE field in data is 0 */

                sal_memset(&mac_mask, 0, sizeof(mac_mask));
                mac_mask[0] = 1; /* bit 40 */
                soc_mem_mac_addr_set(unit, L2_BULKm, &match_mask,
                                     MAC_ADDRf, mac_mask);
                soc_mem_mac_addr_set(unit, L2_BULKm, &match_data,
                                     MAC_ADDRf, mac_mask);

                BCM_IF_ERROR_RETURN
                    (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1,
                                               &match_mask));
                BCM_IF_ERROR_RETURN
                    (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0,
                                               &match_data));
                if (!SAL_BOOT_BCMSIM) {
                    BCM_IF_ERROR_RETURN
                        (soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
                }
            } else
#endif /* BCM_TOMAHAWK_SUPPORT */
            {
                l2_bulk_match_mask_entry_t match_mask;
                l2_bulk_match_data_entry_t match_data;
                bcm_mac_t mac_mask;
                int field_len;

                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, L2_BULK_CONTROLr,
                                            REG_PORT_ANY, ACTIONf, 1));

                sal_memset(&match_mask, 0, sizeof(match_mask));
                sal_memset(&match_data, 0, sizeof(match_data));

                soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask,
                                    VALIDf, 1);
                soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data,
                                    VALIDf, 1);

                field_len = soc_mem_field_length(unit, L2_BULK_MATCH_MASKm,
                                                 KEY_TYPEf);
                soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask,
                                    KEY_TYPEf, (1 << field_len) - 1);
                /* KEY_TYPE field in data is 0 */

                sal_memset(&mac_mask, 0, sizeof(mac_mask));
                mac_mask[0] = 1; /* bit 40 */
                soc_mem_mac_addr_set(unit, L2_BULK_MATCH_MASKm, &match_mask,
                                     MAC_ADDRf, mac_mask);
                soc_mem_mac_addr_set(unit, L2_BULK_MATCH_DATAm, &match_data,
                                     MAC_ADDRf, mac_mask);

                BCM_IF_ERROR_RETURN
                    (WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0,
                                               &match_mask));
                BCM_IF_ERROR_RETURN
                    (WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0,
                                               &match_data));
                if (!SAL_BOOT_BCMSIM) {
                    BCM_IF_ERROR_RETURN
                        (soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
                }
            }
        } else
#endif /* BCM_TRIDENT_SUPPORT */
        {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_ism_memory)) {
                if (!SAL_BOOT_PLISIM) {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr3_l2_addr_delete_mcast(unit, FALSE));
                }
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                if (!SAL_BOOT_BCMSIM) {
                BCM_IF_ERROR_RETURN
                    (_bcm_xgs3_l2_addr_delete_mcast(unit, BCM_L2_DELETE_STATIC));
                }
            }
        }
    }

    return BCM_E_NONE;    
}

/*
 * Function:
 *      bcm_xgs3_mcast_init
 * Purpose:
 *      Initialize multicast api components
 * Returns:
 *      BCM_E_XXX on error
 *      number of mcast entries supported on success
 */

int
bcm_xgs3_mcast_init(int unit)
{
    if (L2MC_USED(unit) != NULL) {
        sal_free(L2MC_USED(unit));
    }
    L2MC_SIZE(unit) = 0;

    L2MC_L2MEM(unit) = soc_feature(unit, soc_feature_ism_memory) ?
        L2_ENTRY_1m: L2Xm;
    L2MC_MCMEM(unit) = L2MCm;
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
        int mc_base, mc_size;
        
#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            uint32 mc_ctrl;
            soc_control_t       *soc = SOC_CONTROL(unit);

            /* Recover the mcast size settings */
            if (soc_feature(unit, soc_feature_higig2)) {
                SOC_IF_ERROR_RETURN
                    (READ_MC_CONTROL_1r(unit, &mc_ctrl));
                soc->higig2_bcast_size =
                    soc_reg_field_get(unit, MC_CONTROL_1r, mc_ctrl,
                                      HIGIG2_BC_SIZEf);

                SOC_IF_ERROR_RETURN
                    (READ_MC_CONTROL_2r(unit, &mc_ctrl));
                soc->higig2_mcast_size =
                    soc_reg_field_get(unit, MC_CONTROL_2r, mc_ctrl,
                                      HIGIG2_L2MC_SIZEf);

                SOC_IF_ERROR_RETURN
                    (READ_MC_CONTROL_3r(unit, &mc_ctrl));
                soc->higig2_ipmc_size =
                    soc_reg_field_get(unit, MC_CONTROL_3r, mc_ctrl,
                                      HIGIG2_IPMC_SIZEf);
            }

            if (!SOC_IS_MONTEREY(unit)) {
                if (SOC_REG_FIELD_VALID(unit, MC_CONTROL_5r,
                                        SHARED_TABLE_L2MC_SIZEf) &&
                    SOC_REG_FIELD_VALID(unit, MC_CONTROL_5r,
                                        SHARED_TABLE_IPMC_SIZEf)) {
                    SOC_IF_ERROR_RETURN
                        (READ_MC_CONTROL_5r(unit, &mc_ctrl));
                    soc->mcast_size =
                        soc_reg_field_get(unit, MC_CONTROL_5r, mc_ctrl,
                                          SHARED_TABLE_L2MC_SIZEf);
                    soc->ipmc_size =
                        soc_reg_field_get(unit, MC_CONTROL_5r, mc_ctrl,
                                          SHARED_TABLE_IPMC_SIZEf);
                }
            }

        }
#endif /* BCM_WARM_BOOT_SUPPORT */

        SOC_IF_ERROR_RETURN
            (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));

        L2MC_SIZE(unit) = mc_size;
    }
#endif

    if (L2MC_SIZE(unit) <= 0) {
        L2MC_SIZE(unit) = soc_mem_index_count(unit, L2MC_MCMEM(unit));
    }

    L2MC_USED(unit) = sal_alloc(sizeof(int) * L2MC_SIZE(unit), "L2MC");
    if (L2MC_USED(unit) == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(L2MC_USED(unit), 0, sizeof(int) * L2MC_SIZE(unit));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        l2x_entry_t  *l2_entry, *l2_table;
        l2mc_entry_t *l2mc_entry, *l2mc_table;
        int index, index_min, index_max, l2mc_tbl_sz;
        sal_mac_addr_t mcmac;

        /*
         * Go through L2 table to get MC use counts
         */
#ifdef BCM_TRIUMPH3_SUPPORT
        if (soc_feature(unit, soc_feature_ism_memory)) {
            _bcm_tr3_mcast_l2_table_recover(unit);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            int chunk_entries, chunk_bytes;
            int chunk_idx_min, chunk_idx_max;
            int entry_size = 0; /* Size of L2X entry in bytes */

            chunk_entries = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                         L2_MEM_CHUNKS_DEFAULT);

            if(SOC_IS_TRIDENT3(unit)){
                entry_size = sizeof(l2x_entry_t);
#if defined(BCM_HURRICANE4_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
            } else if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIRELIGHT(unit)) {
                entry_size = soc_mem_entry_words(unit, L2MC_L2MEM(unit)) * 4;
#endif /* BCM_HURRICANE4_SUPPORT || BCM_FIRELIGHT_SUPPORT */
            } else {
                entry_size = soc_mem_entry_bytes(unit, L2MC_L2MEM(unit));
            }

            chunk_bytes = entry_size * chunk_entries;
            l2_table = soc_cm_salloc(unit, chunk_bytes, "l2 tbl dma");
            if (l2_table == NULL) {
                sal_free(L2MC_USED(unit));
                return BCM_E_MEMORY;
            }

            for (chunk_idx_min = soc_mem_index_min(unit, L2MC_L2MEM(unit));
                 chunk_idx_min <= soc_mem_index_max(unit, L2MC_L2MEM(unit));
                 chunk_idx_min += chunk_entries) {

                /* Read a chunk of L2 table */
                sal_memset(l2_table, 0, chunk_bytes);
                chunk_idx_max = chunk_idx_min + chunk_entries - 1;
                if (chunk_idx_max > soc_mem_index_max(unit, L2MC_L2MEM(unit))) {
                    chunk_idx_max = soc_mem_index_max(unit, L2MC_L2MEM(unit));
                }

                if (soc_mem_read_range(unit, L2MC_L2MEM(unit), MEM_BLOCK_ANY,
                            chunk_idx_min, chunk_idx_max, l2_table) < 0) {
                    sal_free(L2MC_USED(unit));
                    soc_cm_sfree(unit, l2_table);
                    return SOC_E_INTERNAL;
                }

                for (index = chunk_idx_min; index <= chunk_idx_max; index++) {
                    int l2mc_idx;
                    l2mc_entry_t mc_entry;
                    soc_field_t  field_valid;

                    l2_entry =
                        soc_mem_table_idx_to_pointer(unit, L2MC_L2MEM(unit),
                                                     l2x_entry_t *, l2_table,
                                                     index - chunk_idx_min);
                    field_valid = VALIDf;
                    if (soc_feature(unit, soc_feature_base_valid)) {
                        field_valid = BASE_VALIDf;
                    }
                    if (!soc_mem_field32_get(unit, L2MC_L2MEM(unit),
                                l2_entry, field_valid)) {
                        continue;
                    }

#ifdef BCM_TRX_SUPPORT
                    /* We're already traversing the L2 memory here, so
                     * check for non-bridge items */
                    if ((FALSE == SOC_CONTROL(unit)->l2x_ppa_bypass) &&
                            soc_feature(unit, soc_feature_ppa_bypass) &&
                            (soc_mem_field32_get(unit, L2MC_L2MEM(unit),
                                                 l2_entry, KEY_TYPEf) !=
                             TR_L2_HASH_KEY_TYPE_BRIDGE)) {
                        SOC_CONTROL(unit)->l2x_ppa_bypass = TRUE;
                    }

                    if (soc_mem_field_valid(unit, 
                                            L2MC_L2MEM(unit), KEY_TYPEf) &&
                            (soc_mem_field32_get(unit, L2MC_L2MEM(unit),
                                                 l2_entry, KEY_TYPEf) !=
                             TR_L2_HASH_KEY_TYPE_BRIDGE)) {
                        /* The field L2MC_PTRf is only valid for key
                         * type BRIDGE */
                        continue;
                    }
#endif /* BCM_TRX_SUPPORT */

                    soc_L2Xm_mac_addr_get(unit, l2_entry, MAC_ADDRf, mcmac);
                    if (!BCM_MAC_IS_MCAST(mcmac)) {
                        continue;
                    }

                    if (soc_feature(unit, soc_feature_generic_dest)) {
                        uint32 dest_type;
                        l2mc_idx = soc_mem_field32_dest_get(unit,
                                    L2MC_L2MEM(unit), l2_entry,
                                    DESTINATIONf, &dest_type);
                        if (dest_type != SOC_MEM_FIF_DEST_L2MC) {
                            l2mc_idx = 0;
                        }
                    } else {
                        l2mc_idx = soc_mem_field32_get(unit, L2MC_L2MEM(unit),
                                                       l2_entry, L2MC_PTRf);
                    }
                    sal_memset(&mc_entry, 0, sizeof(l2mc_entry_t));
                    if (soc_mem_read(unit, L2MC_MCMEM(unit),  MEM_BLOCK_ANY,
                                l2mc_idx, &mc_entry) < 0) {
                        sal_free(L2MC_USED(unit));
                        soc_cm_sfree(unit, l2_table);
                        return SOC_E_INTERNAL;
                    }

                    if (!soc_mem_field32_get(unit, L2MC_MCMEM(unit),
                                &mc_entry, VALIDf)) {
                        continue;
                    }
                    L2MC_USED_SET(unit, l2mc_idx);
                }

            }
            soc_cm_sfree(unit, l2_table);
        }

        /*
         * But then L2 table may not have indices populated for
         * all valid L2MC entries..
         * Go through L2MC table to search for existing entries
         */
        index_min = soc_mem_index_min(unit, L2MC_MCMEM(unit));
        index_max = index_min + L2MC_SIZE(unit) - 1;

        l2mc_tbl_sz = sizeof(l2mc_entry_t) * L2MC_SIZE(unit);
        l2mc_table = soc_cm_salloc(unit, l2mc_tbl_sz, "l2mc tbl dma");
        if (l2mc_table == NULL) {
            sal_free(L2MC_USED(unit));
            return BCM_E_MEMORY;
        }

        /* coverity[overflow_sink : FALSE] */
        memset((void *)l2mc_table, 0, l2mc_tbl_sz);
        if (soc_mem_read_range(unit, L2MC_MCMEM(unit), MEM_BLOCK_ANY,
                    index_min, index_max, l2mc_table) < 0) {
            sal_free(L2MC_USED(unit));
            soc_cm_sfree(unit, l2mc_table);
            return SOC_E_INTERNAL;
        }

        for (index = index_min; index <= index_max; index++) {

            l2mc_entry = soc_mem_table_idx_to_pointer(unit, L2MC_MCMEM(unit),
                    l2mc_entry_t *, l2mc_table, index);

            if (!soc_mem_field32_get(unit, L2MC_MCMEM(unit),
                        l2mc_entry, VALIDf)) {
                continue;
            }

            if (!L2MC_USED_ISSET(unit, index)) {
                L2MC_USED_SET(unit, index);
            }
        }

        soc_cm_sfree(unit, l2mc_table);

        return L2MC_SIZE(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (!SAL_BOOT_BCMSIM && !SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM) {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, L2MC_MCMEM(unit), MEM_BLOCK_ALL, FALSE));
    }

    BCM_IF_ERROR_RETURN(bcm_xgs3_l2_addr_mcast_delete_all(unit)); 

    return L2MC_SIZE(unit);
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_xgs3_mcast_sw_dump
 * Purpose:
 *     Displays XGS3 mcast information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_xgs3_mcast_sw_dump(int unit)
{
    _bcm_xgs3_mcast_t  *info;
    int                *used;
    int                i, j;

    info = &xgs3_l2mc_info[unit];

    LOG_CLI((BSL_META_U(unit,
                        "\n  XGS3 MCAST -\n")));
    LOG_CLI((BSL_META_U(unit,
                        "    Size   : %d\n"), info->size));
    LOG_CLI((BSL_META_U(unit,
                        "    L2 Mem : %d\n"), info->l2mem));
    LOG_CLI((BSL_META_U(unit,
                        "    MC Mem : %d\n"), info->mcmem));

    LOG_CLI((BSL_META_U(unit,
                        "    Used (index:value) :")));
    used = info->used;
    if (used != NULL) {
        for (i = 0, j = 0; i < info->size; i++) {
            /* If zero, skip print */
            if (used[i] == 0) {
                continue;
            }
            if (!(j % 4)) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "  %4d:%-5d"), i, used[i]));
            j++;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */
