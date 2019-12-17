/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:	bcmx/mcast.c
 * Purpose:	BCMX L2 Multicasting APIs
 */

#include <sal/core/libc.h>

#include <bcm/types.h>

#include <bcm_int/control.h>

#include <bcmx/mcast.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

#define BCMX_MCAST_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_MCAST_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MCAST_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MCAST_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MCAST_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


/*
 * Function:
 *      bcmx_mcast_addr_t_init
 */

void
bcmx_mcast_addr_t_init(bcmx_mcast_addr_t *mcaddr,
		       bcm_mac_t mac,
		       bcm_vlan_t vid)
{
    sal_memset(mcaddr, 0, sizeof(*mcaddr));
    sal_memcpy(mcaddr->mac, mac, sizeof(mcaddr->mac));
    mcaddr->vid = vid;
    bcmx_lplist_init(&mcaddr->ports, 0, 0);
    bcmx_lplist_init(&mcaddr->untag_ports, 0, 0);
}

void
bcmx_mcast_addr_t_free(bcmx_mcast_addr_t * mcaddr)
{
    bcmx_lplist_free(&mcaddr->ports);
    bcmx_lplist_free(&mcaddr->untag_ports);
}


/*
 * Function:
 *      bcmx_mcast_init
 */

int
bcmx_mcast_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_MCAST_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mcast_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_MCAST_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_mcast_addr_add
 * Returns:
 *      BCM_E_CONFIG if conflict is found in existing chips
 */

int
bcmx_mcast_addr_add(bcmx_mcast_addr_t *mcaddr)
{
    int                 rv = BCM_E_UNAVAIL, tmp_rv;
    int                 i, bcm_unit, mcindex, port;
    bcm_mcast_addr_t	bcm_mca, bcm_mca2;
    bcmx_lport_t	lport;
    bcm_pbmp_t		pbmp;
    bcm_unit_bmp_t      unit_bmp;
    int                 lport_unit;
    bcm_port_t          lport_port;
    int                 mcindex_conflict = FALSE;

    BCMX_MCAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mcaddr);

    /*
     * First, look to see if the entry exists in any chip.  Set
     * mcindex if so.  Record chips which should not have entry
     * added.
     */
    mcindex = -1;
    BCM_UNIT_BMP_CLEAR(unit_bmp);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv =
            bcm_mcast_port_get(bcm_unit, mcaddr->mac, mcaddr->vid, &bcm_mca);
        if (tmp_rv == BCM_E_NONE) { /* Found a chip holding entry */
            BCM_UNIT_BMP_ADD(unit_bmp, bcm_unit);
            if (mcindex < 0) {
                mcindex = bcm_mca.l2mc_index;
            } else if (mcindex != bcm_mca.l2mc_index) {
                /*
                 * Indicate to caller that current mc indexes disagree
                 * between different chips.
                 */
                mcindex_conflict = TRUE;
            }
        }

    }

    bcm_mcast_addr_t_init(&bcm_mca, mcaddr->mac, mcaddr->vid);
    bcm_mca.cos_dst = mcaddr->cos_dst;
    bcm_mca.l2mc_index = mcindex;

    /*
     * Add the mcast entry to any non-fabric switches that
     * do not already have it
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        if (BCM_UNIT_BMP_TEST(unit_bmp, bcm_unit)) { /* Already present */
            continue;
        }
        BCMX_LPLIST_TO_PBMP(mcaddr->ports, bcm_unit, bcm_mca.pbmp);
        BCMX_LPLIST_TO_PBMP(mcaddr->untag_ports, bcm_unit, bcm_mca.ubmp);
        /* add stack and higig ports */
        BCMX_FOREACH_QUALIFIED_LPORT(lport,
                                     BCMX_PORT_F_HG|
                                     BCMX_PORT_F_STACK_INT|
                                     BCMX_PORT_F_STACK_EXT) {
            if (BCM_SUCCESS(_bcmx_dest_to_unit_port(lport,
                                                    &lport_unit, &lport_port,
                                                    BCMX_DEST_CONVERT_NON_GPORT))) {
                if (lport_unit == bcm_unit) {
                    BCM_PBMP_PORT_ADD(bcm_mca.pbmp, lport_port);
                }
            }
        }
        if (mcindex < 0) {	/* get allocated mcast index */
            tmp_rv = bcm_mcast_addr_add(bcm_unit, &bcm_mca);
            if (tmp_rv >= 0) {
                bcm_mca2.l2mc_index = -1;
                (void)bcm_mcast_port_get(bcm_unit, mcaddr->mac, mcaddr->vid,
                                         &bcm_mca2);
                mcindex = bcm_mca2.l2mc_index;
                bcm_mca.l2mc_index = mcindex;
            }
        } else {
            tmp_rv = bcm_mcast_addr_add_w_l2mcindex(bcm_unit, &bcm_mca);
        }
        BCM_IF_ERROR_RETURN(BCMX_MCAST_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    /*
     * Add the mcast entry to fabric switches
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (!BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        BCM_PBMP_CLEAR(pbmp);
        BCMX_FOREACH_QUALIFIED_LPORT(lport,
                                     BCMX_PORT_F_HG|
                                     BCMX_PORT_F_STACK_INT|
                                     BCMX_PORT_F_STACK_EXT) {
            if (BCM_SUCCESS(_bcmx_dest_to_unit_port(lport,
                                                    &lport_unit, &lport_port,
                                                    BCMX_DEST_CONVERT_NON_GPORT))) {
                if (lport_unit == bcm_unit) {
                    BCM_PBMP_PORT_ADD(pbmp, lport_port);
                }
            }
        }
        BCM_PBMP_ITER(pbmp, port) {
            tmp_rv = bcm_mcast_bitmap_set(bcm_unit, mcindex, port, pbmp);
            BCM_IF_ERROR_RETURN(BCMX_MCAST_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    if (BCM_SUCCESS(rv) && mcindex_conflict) {
        rv = BCM_E_CONFIG;
    }

    return rv;
}


/*
 * Function:
 *      bcmx_mcast_addr_remove
 */

int
bcmx_mcast_addr_remove(bcm_mac_t mac, bcm_vlan_t vid)
{
    int             rv = BCM_E_UNAVAIL, tmp_rv;
    int			i, bcm_unit, mcindex;
    bcm_mcast_addr_t	bcm_mca;
    bcmx_lport_t	lport;
    bcm_pbmp_t		empty;
    bcm_port_t      bcm_port;

    BCMX_MCAST_INIT_CHECK;

    mcindex = -1;
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        if (mcindex < 0) {	/* get mcast index if available */
            tmp_rv = bcm_mcast_port_get(bcm_unit, mac, vid, &bcm_mca);
            if (tmp_rv >= 0) {
                mcindex = bcm_mca.l2mc_index;
            }
        }
        tmp_rv = bcm_mcast_addr_remove(bcm_unit, mac, vid);
        BCM_IF_ERROR_RETURN(BCMX_MCAST_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    if (mcindex < 0) {
        return rv;
    }

    /*
     * Delete mcast index from fabric
     */
    BCM_PBMP_CLEAR(empty);
    BCMX_FOREACH_QUALIFIED_LPORT(lport,
				 BCMX_PORT_F_HG|
				 BCMX_PORT_F_STACK_INT|
				 BCMX_PORT_F_STACK_EXT) {
        if (BCM_SUCCESS(_bcmx_dest_to_unit_port(lport,
                                                &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT))) {
            if (!BCM_IS_FABRIC(bcm_unit)) {
                continue;
            }
            tmp_rv = bcm_mcast_bitmap_set(bcm_unit, mcindex, bcm_port,
                                          empty);
            BCM_IF_ERROR_RETURN
                (BCMX_MCAST_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_mcast_port_get
 */

int
bcmx_mcast_port_get(bcm_mac_t mac,
		    bcm_vlan_t vid,
		    bcmx_mcast_addr_t *mcaddr)
{
    int rv = BCM_E_UNAVAIL, tmp_rv, i, bcm_unit;
    bcm_mcast_addr_t	bcm_mca;

    BCMX_MCAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mcaddr);

    bcmx_mcast_addr_t_init(mcaddr, mac, vid);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mcast_port_get(bcm_unit, mac, vid, &bcm_mca);

        if (BCMX_MCAST_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                mcaddr->cos_dst = bcm_mca.cos_dst;
                mcaddr->l2mc_index = bcm_mca.l2mc_index;
                BCMX_LPLIST_PBMP_ADD(&mcaddr->ports, bcm_unit, bcm_mca.pbmp);
                BCMX_LPLIST_PBMP_ADD(&mcaddr->untag_ports, bcm_unit, bcm_mca.ubmp);
            } else {
                break;
            }
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_mcast_join
 */

int
bcmx_mcast_join(bcm_mac_t mac,
		bcm_vlan_t vid,
		bcmx_lport_t port,
		bcmx_mcast_addr_t *mcaddr,
		bcmx_lplist_t *allrtr)
{
    int			     rv = BCM_E_UNAVAIL;
    int              bcm_unit;
    bcm_mcast_addr_t bcm_mca;
    bcm_pbmp_t		pbmp;
    bcm_port_t      bcm_port;

    BCMX_MCAST_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    rv = bcm_mcast_join(bcm_unit, mac, vid,
                        bcm_port,
                        mcaddr ? &bcm_mca : NULL,
                        allrtr ? &pbmp : NULL);
    if (BCM_SUCCESS(rv)) {
        if (mcaddr) {
            bcmx_mcast_addr_t_init(mcaddr, mac, vid);
            mcaddr->cos_dst = bcm_mca.cos_dst;
            mcaddr->l2mc_index = bcm_mca.l2mc_index;
            BCMX_LPLIST_PBMP_ADD(&mcaddr->ports, bcm_unit, bcm_mca.pbmp);
            BCMX_LPLIST_PBMP_ADD(&mcaddr->untag_ports, bcm_unit, bcm_mca.ubmp);
        }
        if (allrtr) {
            BCMX_LPLIST_PBMP_ADD(allrtr, bcm_unit, pbmp);
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_mcast_leave
 */

int
bcmx_mcast_leave(bcm_mac_t mac,
		 bcm_vlan_t vid,
		 bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MCAST_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mcast_leave(bcm_unit, mac, vid, bcm_port);
}


/*
 * Function:
 *      bcmx_mcast_port_remove
 */

int
bcmx_mcast_port_remove(bcmx_mcast_addr_t *mcaddr)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_mcast_addr_t bcm_mca;

    BCMX_MCAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mcaddr);

    bcm_mcast_addr_t_init(&bcm_mca, mcaddr->mac, mcaddr->vid);
    bcm_mca.cos_dst = mcaddr->cos_dst;
    bcm_mca.l2mc_index = mcaddr->l2mc_index;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(mcaddr->ports, bcm_unit, bcm_mca.pbmp);
        BCMX_LPLIST_TO_PBMP(mcaddr->untag_ports, bcm_unit, bcm_mca.ubmp);
        if (BCM_PBMP_IS_NULL(bcm_mca.pbmp)) {
            continue;
        }
        tmp_rv = bcm_mcast_port_remove(bcm_unit, &bcm_mca);
        BCM_IF_ERROR_RETURN
            (BCMX_MCAST_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_mcast_port_add
 */

int
bcmx_mcast_port_add(bcmx_mcast_addr_t *mcaddr)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_mcast_addr_t bcm_mca;

    BCMX_MCAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mcaddr);

    bcm_mcast_addr_t_init(&bcm_mca, mcaddr->mac, mcaddr->vid);
    bcm_mca.cos_dst = mcaddr->cos_dst;
    bcm_mca.l2mc_index = mcaddr->l2mc_index;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(mcaddr->ports, bcm_unit, bcm_mca.pbmp);
        BCMX_LPLIST_TO_PBMP(mcaddr->untag_ports, bcm_unit, bcm_mca.ubmp);
        if (BCM_PBMP_IS_NULL(bcm_mca.pbmp)) {
            continue;
        }
        tmp_rv = bcm_mcast_port_add(bcm_unit, &bcm_mca);
        BCM_IF_ERROR_RETURN(BCMX_MCAST_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}
