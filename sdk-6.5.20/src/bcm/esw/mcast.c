/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Multicast Table Management
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/feature.h>
#include <soc/mem.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#include <bcm/error.h>
#include <bcm/mcast.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/mcast.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/common/multicast.h>
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(INCLUDE_L3)
#include <bcm_int/esw/ipmc.h>
#endif /* BCM_WARM_BOOT_SUPPORT && INCLUDE_L3 */

#include <bcm_int/esw_dispatch.h>

#define MCAST_INIT_RETURN(unit) \
        if (!_bcm_mcast_init[unit]) {return BCM_E_INIT;}

static int _bcm_mcast_init[BCM_MAX_NUM_UNITS];
static int _bcm_mcast_idx_ret_type[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_esw_mcast_detach
 * Purpose:
 *      De-initialize multicast bcm routines.
 * Parameters:
 *      unit - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_mcast_detach(int unit)
{
    int rv;

    rv = mbcm_driver[unit]->mbcm_mcast_detach(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    _bcm_mcast_init[unit] = FALSE;

    return rv;
}

/*
 * Function:
 *      bcm_esw_mcast_init
 * Purpose:
 *      Initialize multicast bcm routines.
 * Parameters:
 *      unit - StrataSwitch unit #
 * Returns:
 *      Mcast table size for XGS, or BCM_E_NONE for Strata
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_init(int unit)
{
    int rv;

    LOG_INFO(BSL_LS_BCM_MCAST,
             (BSL_META_U(unit,
                         "MCAST %d: Init\n"),
              unit));
    if (SOC_IS_SHADOW(unit)) {
        return BCM_E_UNAVAIL;
    }

    rv = mbcm_driver[unit]->mbcm_mcast_init(unit);
    if (rv < 0) {
        return rv;
    }

    _bcm_mcast_init[unit] = TRUE;
    _bcm_mcast_idx_ret_type[unit] = 0;

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_WARM_BOOT(unit) && soc_feature(unit, soc_feature_l3)) {
        int rv2; /* Don't change return value holding mcast table size. */
        uint8 flags;
        rv2 = _bcm_esw_ipmc_repl_wb_flags_get(unit,
                      _BCM_IPMC_WB_L2MC_GROUP_TYPE_MULTICAST, &flags);
        if (BCM_SUCCESS(rv2)) {
            _bcm_mcast_idx_ret_type[unit] = (0 != flags);
        } else if (BCM_E_UNAVAIL != rv2) {
            return rv2;
        } /* Else ignore unavailable error. */
    }
#endif /* BCM_WARM_BOOT_SUPPORT && INCLUDE_L3 */

    return rv;
}

/*
 * Function:
 *      bcm_esw_mcast_port_add
 * Purpose:
 *      Add ports to an existing entry in the multicast table
 * Parameters:
 *      unit - unit number.
 *	mcaddr - Pointer to fully filled-in bcm_mcast_addr_t
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr)
{
    MCAST_INIT_RETURN(unit);

    LOG_INFO(BSL_LS_BCM_MCAST,
             (BSL_META_U(unit,
                         "MCAST %d: port add %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n"),
              unit, mcaddr->mac[0], mcaddr->mac[1], mcaddr->mac[2],
              mcaddr->mac[3], mcaddr->mac[4], mcaddr->mac[5],
              mcaddr->vid));
    LOG_VERBOSE(BSL_LS_BCM_MCAST,
                (BSL_META_U(unit,
                            "        : l2 idx %u, cos dest %d, ports 0x%x, ut 0x%x\n"),
                 mcaddr->l2mc_index, mcaddr->cos_dst,
                 SOC_PBMP_WORD_GET(mcaddr->pbmp, 0),
                 SOC_PBMP_WORD_GET(mcaddr->ubmp, 0)));

    /* Remove inactive stack ports; all from untagged */
    BCM_PBMP_REMOVE(mcaddr->pbmp, SOC_PBMP_STACK_INACTIVE(unit));
    BCM_PBMP_REMOVE(mcaddr->ubmp, SOC_PBMP_STACK_CURRENT(unit));
    return mbcm_driver[unit]->mbcm_mcast_port_add(unit, mcaddr);
}

/*
 * Function:
 *      bcm_esw_mcast_port_remove
 * Purpose:
 *      Remove ports from an existing entry in the multicast table
 * Parameters:
 *      unit - unit number.
 *	mcaddr - Pointer to fully filled-in bcm_mcast_addr_t
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_port_remove(int unit, bcm_mcast_addr_t *mcaddr)
{
    MCAST_INIT_RETURN(unit);

    LOG_INFO(BSL_LS_BCM_MCAST,
             (BSL_META_U(unit,
                         "MCAST %d: port remove %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n"),
              unit, mcaddr->mac[0], mcaddr->mac[1], mcaddr->mac[2],
              mcaddr->mac[3], mcaddr->mac[4], mcaddr->mac[5],
              mcaddr->vid));
    LOG_VERBOSE(BSL_LS_BCM_MCAST,
                (BSL_META_U(unit,
                            "        : l2 idx %u, cos dest %d, ports 0x%x, ut 0x%x\n"),
                 mcaddr->l2mc_index, mcaddr->cos_dst,
                 SOC_PBMP_WORD_GET(mcaddr->pbmp, 0),
                 SOC_PBMP_WORD_GET(mcaddr->ubmp, 0)));

    /* Remove stack ports */
    BCM_PBMP_REMOVE(mcaddr->pbmp, SOC_PBMP_STACK_CURRENT(unit));
    BCM_PBMP_REMOVE(mcaddr->ubmp, SOC_PBMP_STACK_CURRENT(unit));
    return mbcm_driver[unit]->mbcm_mcast_port_remove(unit, mcaddr);
}

/* Add stack ports */

STATIC void
_esw_mcast_addr_update(int unit, bcm_mcast_addr_t *mcaddr)
{
    bcm_pbmp_t active;

    SOC_PBMP_STACK_ACTIVE_GET(unit, active);
    BCM_PBMP_OR(mcaddr->pbmp, active);
    BCM_PBMP_REMOVE(mcaddr->ubmp, SOC_PBMP_STACK_CURRENT(unit));
}

/*
 * Function:
 *      _bcm_esw_mcast_convert_mcindex_m2h
 * Purpose:
 *      Convert a bcm_multicast_t l2mc index to a hardware l2mc index.
 * Parameters:
 *	mcindex - (IN/OUT) Pointer to a multicast index
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM   If bcm_multicast_t is not L2 type 
 */

STATIC int
_bcm_esw_mcast_convert_mcindex_m2h(unsigned int *mcindex)
{
    /* If the input parameter mc_index is a L2 type bcm_multicast_t, 
     * convert it to a hardware l2mc index.
     */
    if (_BCM_MULTICAST_IS_SET(*mcindex)) {
        if (_BCM_MULTICAST_IS_L2(*mcindex)) {
            *mcindex = _BCM_MULTICAST_ID_GET(*mcindex);
        } else {
            return BCM_E_PARAM;
        }
    } 

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_mcast_convert_mcindex_h2m
 * Purpose:
 *      Convert a hardware l2mc index to a bcm_multicast_t l2mc index.
 * Parameters:
 *      unit - unit number.
 *	mcindex - (IN/OUT) Pointer to a multicast index 
 * Returns:
 *      None.
 */

STATIC void
_bcm_esw_mcast_convert_mcindex_h2m(int unit, unsigned int *mcindex)
{
    if (_bcm_mcast_idx_ret_type[unit]) {
        /* Convert hardware l2mc index to L2 type bcm_multicast_t */
        _BCM_MULTICAST_GROUP_SET(*mcindex, _BCM_MULTICAST_TYPE_L2, *mcindex);
    }
}

/*
 * Function:
 *      bcm_esw_mcast_addr_add_w_l2mcindex
 * Purpose:
 *      Add an entry in the multicast table with l2mc index provided
 * Parameters:
 *      unit - unit number.
 *	mcaddr - Pointer to fully filled-in bcm_mcast_addr_t
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_addr_add_w_l2mcindex(int unit, bcm_mcast_addr_t *mcaddr)
{
    int rv;
    int orig_mcindex;

    MCAST_INIT_RETURN(unit);

    LOG_INFO(BSL_LS_BCM_MCAST,
             (BSL_META_U(unit,
                         "MCAST %d: addr add w/ idx. %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n"),
              unit, mcaddr->mac[0], mcaddr->mac[1], mcaddr->mac[2],
              mcaddr->mac[3], mcaddr->mac[4], mcaddr->mac[5],
              mcaddr->vid));
    LOG_VERBOSE(BSL_LS_BCM_MCAST,
                (BSL_META_U(unit,
                            "        : l2 idx %u, cos dest %d, ports 0x%x, ut 0x%x\n"),
                 mcaddr->l2mc_index, mcaddr->cos_dst,
                 SOC_PBMP_WORD_GET(mcaddr->pbmp, 0),
                 SOC_PBMP_WORD_GET(mcaddr->ubmp, 0)));
    _esw_mcast_addr_update(unit, mcaddr);

    orig_mcindex = mcaddr->l2mc_index;
    BCM_IF_ERROR_RETURN(_bcm_esw_mcast_convert_mcindex_m2h(&mcaddr->l2mc_index));

    rv = mbcm_driver[unit]->mbcm_mcast_addr_add_w_l2mcindex(unit, mcaddr);

    /* Restore user-specified index */
    mcaddr->l2mc_index = orig_mcindex;

    return rv;
}


/*
 * Function:
 *      bcm_esw_mcast_addr_add
 * Purpose:
 *      Add an entry in the multicast table
 * Parameters:
 *      unit - StrataSwitch unit number.
 *	mcaddr - Pointer to fully filled-in bcm_mcast_addr_t
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_addr_add(int unit, bcm_mcast_addr_t *mcaddr)
{
    MCAST_INIT_RETURN(unit);

    _esw_mcast_addr_update(unit, mcaddr);
    LOG_INFO(BSL_LS_BCM_MCAST,
             (BSL_META_U(unit,
                         "MCAST %d: addr add %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n"),
              unit, mcaddr->mac[0], mcaddr->mac[1], mcaddr->mac[2],
              mcaddr->mac[3], mcaddr->mac[4], mcaddr->mac[5],
              mcaddr->vid));
    LOG_VERBOSE(BSL_LS_BCM_MCAST,
                (BSL_META_U(unit,
                            "        : l2 idx %u, cos dest %d, ports 0x%x, ut 0x%x\n"),
                 mcaddr->l2mc_index, mcaddr->cos_dst,
                 SOC_PBMP_WORD_GET(mcaddr->pbmp, 0),
                 SOC_PBMP_WORD_GET(mcaddr->ubmp, 0)));

    return mbcm_driver[unit]->mbcm_mcast_addr_add(unit, mcaddr);
}

/*
 * Function:
 *      bcm_esw_mcast_addr_remove_w_l2mcindex
 * Purpose:
 *      Delete an entry from the multicast table and provide l2mcindex in
 *      bcm_mcast_addr_t of the deleted entry
 * Parameters:
 *      unit - unit number.
 *      mcaddr - Pointer to  filled-in bcm_mcast_addr_t with mac addr and vid
 *               l2mc_index is provided as output
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXXX
 */

int
bcm_esw_mcast_addr_remove_w_l2mcindex(int unit, bcm_mcast_addr_t *mcaddr)
{
    int rv;
    int orig_mcindex;

    MCAST_INIT_RETURN(unit);

    LOG_INFO(BSL_LS_BCM_MCAST,
             (BSL_META_U(unit,
                         "MCAST %d: addr rmv w/ idx %d\n"),
              unit, mcaddr->l2mc_index));

    orig_mcindex = mcaddr->l2mc_index;
    BCM_IF_ERROR_RETURN(_bcm_esw_mcast_convert_mcindex_m2h(&mcaddr->l2mc_index));
    
    rv = mbcm_driver[unit]->mbcm_mcast_addr_remove_w_l2mcindex(unit, mcaddr);

    /* Restore user-specified index */
    mcaddr->l2mc_index = orig_mcindex;

    return rv;
}

/*
 * Function:
 *      bcm_esw_mcast_addr_remove
 * Purpose:
 *      Delete an entry from the multicast table
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mac  - mac address
 *      vid  - vlan id
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXXX
 */

int
bcm_esw_mcast_addr_remove(int unit, sal_mac_addr_t mac, bcm_vlan_t vid)
{
    MCAST_INIT_RETURN(unit);

    LOG_INFO(BSL_LS_BCM_MCAST,
             (BSL_META_U(unit,
                         "MCAST %d: addr remove %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n"),
              unit, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vid));

    return mbcm_driver[unit]->mbcm_mcast_addr_remove(unit, mac, vid);
}

/*
 * Function:
 *      bcm_esw_mcast_port_get
 * Purpose:
 *      Get membership and untagged port bit maps for a MC group
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      mac - mac address used for lookup.
 *      vid - vlan id used for lookup.
 *	mcaddr - pointer to bcm_mcast_addr_t that is fully filled in
 * Returns:
 *      BCM_E_NONE		Success (mcaddr filled in)
 *      BCM_E_INTERNAL		Chip access failure
 *	BCM_E_NOT_FOUND	Address not found (mcaddr not filled in)
 */

int
bcm_esw_mcast_port_get(int unit,
		   sal_mac_addr_t mac, bcm_vlan_t vid,
		   bcm_mcast_addr_t *mcaddr)
{
    int rv;

    MCAST_INIT_RETURN(unit);

    rv = mbcm_driver[unit]->mbcm_mcast_port_get(unit, mac, vid, mcaddr);

    _bcm_esw_mcast_convert_mcindex_h2m(unit, &mcaddr->l2mc_index);

    return rv;
}

/*
 * Function:
 *	bcm_esw_mcast_leave
 * Purpose:
 *	This function removes the given port from the group membership
 *	of the given mcast mac address.
 * Parameters:
 *	unit    - device number
 *	mac	- Multicast MAC Address
 *	vlan    - Vlan ID
 *	port    - port to be removed from the given multicast address
 * Returns:
 *	BCM_MCAST_LEAVE_DELETED - If the entry with the given mcast
 *                                        addr is deleted from H/W table
 *	BCM_MCAST_LEAVE_UPDATED - If the mcast address is found in
 *                                        the table and bitmap is updated.
 *	BCM_MCAST_LEAVE_NOTFOUND - If the mcast address is not found in
 *                                         the table
 *	BCM_E_XXX		- an error occurred
 * Notes:
 *	BCM_MCAST_LEAVE_XXX  values are non-negative success codes
 */

int
bcm_esw_mcast_leave(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_port_t port)
{
    int                 rv;
    bcm_mcast_addr_t	mcaddr;
    bcm_mcast_addr_t	rtaddr;

    MCAST_INIT_RETURN(unit);

    bcm_mcast_addr_t_init(&mcaddr, mac, vlan);
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    BCM_PBMP_PORT_SET(mcaddr.pbmp, port);
    BCM_IF_ERROR_RETURN(bcm_esw_mcast_port_remove(unit, &mcaddr));
    BCM_IF_ERROR_RETURN(bcm_esw_mcast_port_get(unit, mac, vlan, &mcaddr));

    if (SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit)) {
        rv = bcm_esw_mcast_port_get(unit, (uint8 *)_soc_mac_all_routers, vlan,
                                    &rtaddr);
        if (rv < 0) {
            bcm_mcast_addr_t_init(&rtaddr, (uint8 *)_soc_mac_all_routers, vlan);
        }
        /* Coverity: rtaddr.pbmp is initialized above by bcm_esw_mcast_port_get */
        /* coverity[uninit_use : FALSE] */
        BCM_PBMP_REMOVE(mcaddr.pbmp, rtaddr.pbmp);
    }

    
    if (BCM_PBMP_IS_NULL(mcaddr.pbmp)) {
	BCM_IF_ERROR_RETURN(bcm_esw_mcast_addr_remove(unit, mac, vlan));
        return BCM_MCAST_LEAVE_DELETED;
    }
    return BCM_MCAST_LEAVE_UPDATED;
}

/*
 * Function:
 *	bcm_esw_mcast_join
 * Purpose:
 *	This function adds the given port to the membership of the
 *	given mcast mac address.
 * Parameters:
 *	unit	- device number
 *	mac	- Multicast MAC Address
 *	vlan    - Vlan ID
 *	port    - port to be added to the mulicast address
 *	mcaddr	- (OUT) If not NULL, returns the updated mcast_addr_t for
 *		  the given multicast address
 *	all_router_pbmp - (OUT) If not NULL, returns the ports associated with
 *			the all routers multicast address
 *
 * Returns:
 *	BCM_MCAST_JOIN_ADDED - If a new entry is created for the
 *                                     given mcast address.
 *	BCM_MCAST_JOIN_UPDATED - If the mcast address is found in
 *                                       the table and bitmap is updated.
 *	BCM_E_XXX		- an error occurred
 * Notes:
 *	BCM_MCAST_JOIN_XXX values are non-negative success codes
 */

int
bcm_esw_mcast_join(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_port_t port,
	       bcm_mcast_addr_t *mcaddr,
	       bcm_pbmp_t *all_router_pbmp)
{
    int                 rv;
    bcm_mcast_addr_t	newmc;

    MCAST_INIT_RETURN(unit);

    /*
     * Fill in all_routers port bitmap
     */
    if (all_router_pbmp != NULL) {
	rv = bcm_esw_mcast_port_get(unit, (uint8 *)_soc_mac_all_routers, vlan,
                                    &newmc);
        if (rv == BCM_E_NOT_FOUND) {
	    BCM_PBMP_CLEAR(*all_router_pbmp);
        } else if (rv < 0) {
            return rv;
        } else {
            /* Coverity: newmc.pbmp is initialized above by bcm_esw_mcast_port_get */
            /* coverity[uninit_use : FALSE] */
	    BCM_PBMP_ASSIGN(*all_router_pbmp, newmc.pbmp);
        }
    }

    /*
     * Now, join the actual multicast group
     */
    bcm_mcast_addr_t_init(&newmc, mac, vlan);
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }
    
    BCM_PBMP_PORT_SET(newmc.pbmp, port);
    rv = bcm_esw_mcast_port_add(unit, &newmc);

    if (rv == BCM_E_NOT_FOUND) {
	BCM_IF_ERROR_RETURN(bcm_esw_mcast_addr_add(unit, &newmc));
        rv = BCM_MCAST_JOIN_ADDED;
    } else if (rv < 0) {
	return rv;
    } else {
	rv = BCM_MCAST_JOIN_UPDATED;
    }
    if (mcaddr != NULL) {
	BCM_IF_ERROR_RETURN(bcm_esw_mcast_port_get(unit, mac, vlan, mcaddr));
    }
    return rv;
}

/*
 * Function:	bcm_mcast_bitmap_max_get
 * Purpose:	provides maximum mcast index that this fabric can handle
 * Parameters:	unit		- device
 *		max_index	- (OUT) returned number of entries
 * Returns:	BCM_E_*
 * Note:
 * 	Fabric only
 */
int
bcm_esw_mcast_bitmap_max_get(int unit, int *max_index)
{
#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
	*max_index = soc_mem_index_count(unit, MEM_MCm);
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int	mc_base, mc_size;

	SOC_IF_ERROR_RETURN
	    (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
	*max_index = mc_size;
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    *max_index = 0;
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_mcast_bitmap_set
 * Purpose:
 *     Set the L2 MC forwarding port bitmap
 * Parameters:
 *     unit     - SOC unit #
 *     index    - the MC index carried in HiGig header
 *     in_port  - ingress port number
 *     pbmp     - IPMC forwarding port bitmap
 * Returns:
 *     BCM_E_XXX
 * Note:
 *	Fabric only
 */
int
bcm_esw_mcast_bitmap_set(int unit, int index, bcm_port_t in_port, bcm_pbmp_t pbmp)
{
#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
	mem_mc_entry_t      entry;
	int                 blk, rv;

    if (BCM_GPORT_IS_SET(in_port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, in_port, &in_port));
    }
	if (!SOC_PORT_VALID(unit, in_port)) {
	    return BCM_E_PORT;
	}
	if (index < 0 || index > soc_mem_index_max(unit, MEM_MCm)) {
	    return BCM_E_PARAM;
	}

	/* Make sure packet never goes back */
	BCM_PBMP_PORT_REMOVE(pbmp, in_port);

	blk = SOC_PORT_BLOCK(unit, in_port);

	sal_memset(&entry, 0, sizeof(entry));

	soc_mem_lock(unit, MEM_MCm);
	rv = READ_MEM_MCm(unit, blk, index, &entry);
	if (rv >= 0) {
	    soc_MEM_MCm_field32_set(unit, &entry, MCBITMAPf,
				    SOC_PBMP_WORD_GET(pbmp, 0));
	    rv = WRITE_MEM_MCm(unit, blk, index, &entry);
	}
	soc_mem_unlock(unit, MEM_MCm);

	return rv;
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int		rv, mc_base, mc_size;
	l2mc_entry_t	l2mc;

	COMPILER_REFERENCE(in_port);

        BCM_IF_ERROR_RETURN(_bcm_esw_mcast_convert_mcindex_m2h((unsigned int*)&index));

	SOC_IF_ERROR_RETURN
	    (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
	if (index < 0 || index > mc_size) {
	    return BCM_E_PARAM;
	}
	index += mc_base;

	soc_mem_lock(unit, L2MCm);
        rv = READ_L2MCm(unit, MEM_BLOCK_ANY, index, &l2mc);
	if (rv >= 0) {
            soc_mem_pbmp_field_set(unit, L2MCm, &l2mc, PORT_BITMAPf, &pbmp);
	    soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
	    rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, index, &l2mc);
	}
	soc_mem_unlock(unit, L2MCm);
	return rv;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_mcast_bitmap_get
 * Purpose:
 *     Get the L2 MC forwarding port bitmap
 * Parameters:
 *     unit    - SOC unit #
 *     index   - the L2 MC index carried in HiGig header
 *     in_port - ingress port number
 *     pbmp    - IPMC forwarding port bitmap
 * Returns:
 *     BCM_E_xxxx
 * Note:
 * 	Fabric only
 */
int
bcm_esw_mcast_bitmap_get(int unit, int index, bcm_port_t in_port,
		     bcm_pbmp_t *pbmp)
{
#ifdef	BCM_XGS12_FABRIC_SUPPORT

    if (SOC_IS_XGS12_FABRIC(unit)) {
	mem_mc_entry_t	entry;
	int		blk;
	uint32		pword;

    if (BCM_GPORT_IS_SET(in_port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, in_port, &in_port));
    }
	if (!SOC_PORT_VALID(unit, in_port)) {
	    return BCM_E_PORT;
	}
	if (index < 0 || index > soc_mem_index_max(unit, MEM_MCm)) {
	    return BCM_E_PARAM;
	}

	blk = SOC_PORT_BLOCK(unit, in_port);
	SOC_IF_ERROR_RETURN(READ_MEM_MCm(unit, blk, index, &entry));
	pword = soc_MEM_MCm_field32_get(unit, &entry, MCBITMAPf);
	SOC_PBMP_CLEAR(*pbmp);
	SOC_PBMP_WORD_SET(*pbmp, 0, pword);
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int		mc_base, mc_size;
	l2mc_entry_t	l2mc;

	COMPILER_REFERENCE(in_port);

        BCM_IF_ERROR_RETURN(_bcm_esw_mcast_convert_mcindex_m2h((unsigned int*)&index));

	SOC_IF_ERROR_RETURN
	    (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
	if (index < 0 || index > mc_size) {
	    return BCM_E_PARAM;
	}
	index += mc_base;

        SOC_IF_ERROR_RETURN(READ_L2MCm(unit, MEM_BLOCK_ANY, index, &l2mc));
        soc_mem_pbmp_field_get(unit, L2MCm, &l2mc, PORT_BITMAPf, pbmp);
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_mcast_bitmap_del
 * Purpose:
 *     Remove L2 MC forwarding port bitmap
 * Parameters:
 *     unit     - SOC unit #
 *     index    - the L2 MC index carried in HiGig header
 *     in_port  - ingress port number
 *     pbmp     - IPMC forwarding port bitmap
 * Returns:
 *     BCM_E_xxxx
 * Note:
 * 	Fabric only
 */
int
bcm_esw_mcast_bitmap_del(int unit, int index, bcm_port_t in_port, bcm_pbmp_t pbmp)
{
#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
	mem_mc_entry_t      entry;
	uint32              pword;
	int                 rv, blk;

    if (BCM_GPORT_IS_SET(in_port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, in_port, &in_port));
    }
	if (!SOC_PORT_VALID(unit, in_port)) {
	    return BCM_E_PORT;
	}
	if (index < 0 || index > soc_mem_index_max(unit, MEM_MCm)) {
	    return BCM_E_PARAM;
	}

	blk = SOC_PORT_BLOCK(unit, in_port);

	soc_mem_lock(unit, MEM_MCm);

	rv = READ_MEM_MCm(unit, blk, index, &entry);
	if (rv >= 0) {
	    pword = soc_MEM_MCm_field32_get(unit, &entry, MCBITMAPf);
	    pword &= ~SOC_PBMP_WORD_GET(pbmp, 0);
	    soc_MEM_MCm_field32_set(unit, &entry, MCBITMAPf, pword);
	    rv = WRITE_MEM_MCm(unit, blk, index, &entry);
	}

	soc_mem_unlock(unit, MEM_MCm);

	return rv;
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int		rv, mc_base, mc_size;
	l2mc_entry_t	l2mc;
        bcm_pbmp_t      tmp_pbmp;

	COMPILER_REFERENCE(in_port);

        BCM_IF_ERROR_RETURN(_bcm_esw_mcast_convert_mcindex_m2h((unsigned int*)&index));

	SOC_IF_ERROR_RETURN
	    (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
	if (index < 0 || index > mc_size) {
	    return BCM_E_PARAM;
	}
	index += mc_base;

	soc_mem_lock(unit, L2MCm);
        rv = READ_L2MCm(unit, MEM_BLOCK_ANY, index, &l2mc);
	if (rv >= 0) {
            soc_mem_pbmp_field_get(unit, L2MCm, &l2mc, PORT_BITMAPf,
                                   &tmp_pbmp);
            SOC_PBMP_REMOVE(tmp_pbmp, pbmp);
            soc_mem_pbmp_field_set(unit, L2MCm, &l2mc, PORT_BITMAPf,
                                   &tmp_pbmp);
	    soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
	    rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, index, &l2mc);
	}
	soc_mem_unlock(unit, L2MCm);
	return rv;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

STATIC int
_xgs12_mcast_stk_update(int unit, bcm_pbmp_t add_ports,
                      bcm_pbmp_t remove_ports)
{
    int rv = BCM_E_NONE;  /* Ignore if unavailable */

#if defined(BCM_XGS12_SWITCH_SUPPORT)
    int i;
    bcm_pbmp_t new_ports, old_ports;
    l2x_mc_entry_t l2mc;
    int changed = 0;

    soc_mem_lock(unit, L2X_MCm);
    for (i = soc_mem_index_min(unit, L2X_MCm);
             i <= soc_mem_index_max(unit, L2X_MCm);
             i++) {

        rv = soc_mem_read(unit, L2X_MCm, MEM_BLOCK_ANY, i, &l2mc);
        if (rv < 0) {
            break;
        }
        if (soc_L2X_MCm_field32_get(unit, &l2mc, VALIDf) == 0) {
            continue;
        }
        _bcm_l2mcentry_pbmp_get(unit, &l2mc, &old_ports);
        BCM_PBMP_ASSIGN(new_ports, old_ports);
        BCM_PBMP_OR(new_ports, add_ports);
        BCM_PBMP_REMOVE(new_ports, remove_ports);
        if (BCM_PBMP_NEQ(new_ports, old_ports)) {
            ++changed;
            _bcm_l2mcentry_pbmp_set(unit, &l2mc, new_ports);
            rv = soc_mem_write(unit, L2X_MCm, MEM_BLOCK_ALL, i, &l2mc);
            if (rv < 0) {
                break;
            }
        }
    }
    soc_mem_unlock(unit, L2X_MCm);

    if (changed) {
        LOG_VERBOSE(BSL_LS_BCM_MCAST,
                    (BSL_META_U(unit,
                                "L2MC %d: xgs stk update changed %d entries\n"),
                     unit, changed));
    }
#endif

    return rv;
}

STATIC int
_xgs3_fb_mcast_stk_update(int unit, bcm_pbmp_t add_ports,
                       bcm_pbmp_t remove_ports)
{
    int rv = BCM_E_NONE;  /* Ignore if unavailable */

#if defined(BCM_FIREBOLT_SUPPORT)
    int i;
    bcm_pbmp_t new_ports, old_ports;
    l2mc_entry_t l2mc;
    int changed = 0;

    soc_mem_lock(unit, L2MCm);
    for (i = soc_mem_index_min(unit, L2MCm);
             i <= soc_mem_index_max(unit, L2MCm);
             i++) {

        rv = soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, i, &l2mc);
        if (rv < 0) {
            break;
        }
        if (soc_L2MCm_field32_get(unit, &l2mc, VALIDf) == 0) {
            continue;
        }
        soc_mem_pbmp_field_get(unit, L2MCm, &l2mc, PORT_BITMAPf, &old_ports);
        BCM_PBMP_ASSIGN(new_ports, old_ports);
        BCM_PBMP_OR(new_ports, add_ports);
        BCM_PBMP_REMOVE(new_ports, remove_ports);
        if (BCM_PBMP_NEQ(new_ports, old_ports)) {
            ++changed;
            soc_mem_pbmp_field_set(unit, L2MCm, &l2mc, PORT_BITMAPf,
                                   &new_ports);
            rv = soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, i, &l2mc);
            if (rv < 0) {
                break;
            }
        }
    }
    soc_mem_unlock(unit, L2MCm);

    if (changed) {
        LOG_VERBOSE(BSL_LS_BCM_MCAST,
                    (BSL_META_U(unit,
                                "L2MC %d: xgs stk update changed %d entries\n"),
                     unit, changed));
    }
#endif

    return rv;
}

/*
 * Function:
 *      _bcm_esw_mcast_stk_update
 * Purpose:
 *      Update port bitmaps after stack change
 * Parameters:
 *      
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */


int
_bcm_esw_mcast_stk_update(int unit, uint32 flags)
{
    bcm_pbmp_t add_ports, remove_ports;
    int rv = BCM_E_NONE;

    /* Calculate ports that should and should not be in bitmap */
    SOC_PBMP_ASSIGN(add_ports, SOC_PBMP_STACK_CURRENT(unit));
    SOC_PBMP_REMOVE(add_ports, SOC_PBMP_STACK_INACTIVE(unit));

    /* Remove ports no longer stacking, or explicitly inactive */
    SOC_PBMP_ASSIGN(remove_ports, SOC_PBMP_STACK_PREVIOUS(unit));
    SOC_PBMP_REMOVE(remove_ports, SOC_PBMP_STACK_CURRENT(unit));
    SOC_PBMP_OR(remove_ports, SOC_PBMP_STACK_INACTIVE(unit));

    if (SOC_IS_XGS12_SWITCH(unit)) {
        rv = _xgs12_mcast_stk_update(unit, add_ports, remove_ports);
    } else if (SOC_IS_XGS3_SWITCH(unit)) {
        rv = _xgs3_fb_mcast_stk_update(unit, add_ports, remove_ports);
    } else { /* May update fabric in the future, but not needed yet */
        rv = BCM_E_NONE; /* Ignore if unavailable */
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_mcast_cleanup(int unit)
{
    int rv = BCM_E_NONE;
  	 
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_FB_FX_HX(unit)) {
        rv = _bcm_xgs3_mcast_detach(unit);
    }
#endif
    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_mcast_sw_dump
 * Purpose:
 *     Displays mcast information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_mcast_sw_dump(int unit)
{

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information MCAST - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "    Init : %d\n"), _bcm_mcast_init[unit]));
    LOG_CLI((BSL_META_U(unit,
                        "    Multicast Index Return Type : %d\n"), _bcm_mcast_idx_ret_type[unit]));

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        _bcm_xgs3_mcast_sw_dump(unit);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *      _bcm_esw_mcast_idx_ret_type_set
 * Purpose:
 *      Set multicast index return type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      arg - 0 means hardware multicast index, 1 means bcm_multicast_t
 * Returns:
 *      BCM_E_NONE 
 *      BCM_E_UNAVAIL
 */

int
_bcm_esw_mcast_idx_ret_type_set(int unit, int arg)
{
    if (SOC_IS_XGS3_SWITCH(unit)) {
        MCAST_INIT_RETURN(unit);
        _bcm_mcast_idx_ret_type[unit] = arg;

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(INCLUDE_L3)
        /* Record this value in HW for Warm Boot recovery. */
        BCM_IF_ERROR_NOT_UNAVAIL_RETURN
            (_bcm_esw_ipmc_repl_wb_flags_set(unit,
                  (arg ? _BCM_IPMC_WB_L2MC_GROUP_TYPE_MULTICAST : 0),
                         _BCM_IPMC_WB_L2MC_GROUP_TYPE_MULTICAST));
#endif /* BCM_WARM_BOOT_SUPPORT && INCLUDE_L3 */
        
        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_mcast_idx_ret_type_get
 * Purpose:
 *      Get multicast index return type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      arg - Pointer to where retrieved value will be written
 * Returns:
 *      BCM_E_NONE 
 *      BCM_E_UNAVAIL
 */

int
_bcm_esw_mcast_idx_ret_type_get(int unit, int *arg)
{
    if (SOC_IS_XGS3_SWITCH(unit)) {
        MCAST_INIT_RETURN(unit);
        *arg = _bcm_mcast_idx_ret_type[unit];
        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

