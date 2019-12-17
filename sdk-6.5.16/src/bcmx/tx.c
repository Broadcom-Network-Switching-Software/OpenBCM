/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        tx.c
 * Purpose:
 * Requires:
 *
 * Notes:  Does not yet support packet gather at all.
 *    Still issues with data buffer transfer
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <bcm/types.h>
#include <bcm/tx.h>
#include <bcm/pkt.h>
#include <bcm/error.h>
#include <bcm/topo.h>
#include <bcm_int/control.h>

#include <bcmx/tx.h>
#include <bcmx/l2.h>
#include <bcmx/vlan.h>
#include <bcmx/bcmx.h>
#include <bcmx/trunk.h>

#include "bcmx_int.h"

#if defined(BROADCOM_DEBUG)
#endif /* BROADCOM_DEBUG */

#define BCMX_TX_INIT_CHECK    BCMX_READY_CHECK

/*
 * For now, flood on unit 0.  This will support systems
 * where unit 0 is a 567x such as white knight and lancelot.
 */

int _bcmx_tx_unit = 0;
int _bcmx_tx_flood_unit = 0;

/* Actions based on L2 map, etc. */

#define _BCMX_TX_UC           1    /* Unicast based on given LPort */
#define _BCMX_TX_FLOOD        2    /* Flood the packet on its vlan */
#define _BCMX_TX_DISCARD      3    /* Discard the packet */
#define _BCMX_TX_BCM          4    /* Send using bcm_tx */
#define _BCMX_TX_MCAST        5    /* L2 multicast; not implemented */

#define	_BCMX_UNPACK_U16(_buf, _var) \
		_var  = *_buf++ << 8; \
		_var |= *_buf++; 

/****************************************************************
 *
 * Internal static functions
 */

/* Get the VID from a bcm_pkt structure */
STATIC INLINE bcm_vlan_t
pkt_vid_get(bcm_pkt_t *pkt)
{
    int vblk = 0;
    int voffset = 12;
    uint16 vtag;
    uint8 *ptr;

    /* First, find offset of vlan tag */
    if (pkt->pkt_data[0].len < 16) {
        if (pkt->pkt_data[1].len < 10) {
            vblk = 2;
            voffset = 0;
        } else {
            vblk = 1;
            voffset = 6;
        }
    }

    ptr = &pkt->pkt_data[vblk].data[voffset + 2];
    _BCMX_UNPACK_U16(ptr, vtag);

    return BCM_VLAN_CTRL_ID(vtag);
}

/* Get protocol tag from the packet */
STATIC INLINE uint16
pkt_tag_proto_get(bcm_pkt_t *pkt)
{
    int vblk = 0;
    int voffset = 12;
    uint16 vtag;
    uint8 *ptr;

    /* First, find offset of vlan tag */
    if (pkt->pkt_data[0].len < 16) {
        if (pkt->pkt_data[1].len < 10) {
            vblk = 2;
            voffset = 0;
        } else {
            vblk = 1;
            voffset = 6;
        }
    }

    ptr = &pkt->pkt_data[vblk].data[voffset];
    _BCMX_UNPACK_U16(ptr, vtag);

    return vtag;
}

/* Get the VID of the packet if tagged, or default VID */
STATIC INLINE bcm_vlan_t
pkt_vid_resolve(bcm_pkt_t *pkt)
{
    bcm_vlan_t vid = 1;

    if (pkt_tag_proto_get(pkt) == 0x8100) { /* pkt is tagged */
        vid = pkt_vid_get(pkt);
    } else {
        bcmx_vlan_default_get(&vid);
    }

    return vid;
}


/*
 * Check if packet needs L2 lookup and carry out if it does.
 * Returns BCMX_TX_ return values OK, FLOODED, DISCARDED.
 *
 * TO DO:  Implement multicast support.
 */

STATIC INLINE int
pkt_addr_resolve(bcm_pkt_t *pkt, uint32 flags, bcmx_lport_t *d_port)
{
    bcm_vlan_t vid;
    bcmx_l2_addr_t l2addr;
    int rv = BCM_E_NONE;

    if (flags & BCMX_TX_F_FLOOD_PKT) {
        return _BCMX_TX_FLOOD;
    }

    /* Check if L2 lookup required */
    if (flags & BCMX_TX_F_L2_LOOKUP) {
        *d_port = BCMX_LPORT_INVALID;

        if ((pkt->pkt_data[0].data[0] & 0x1) == 0) {
            /* Only look up unicast packets */
            vid = pkt_vid_resolve(pkt);

            sal_memset(&l2addr, 0, sizeof(l2addr));
            if (bcmx_l2_addr_get((void *)pkt->pkt_data[0].data,
                                 vid, &l2addr, NULL) == 0) {
                if (l2addr.flags & BCM_L2_TRUNK_MEMBER) {
                    bcmx_trunk_add_info_t t_data;

                    if (bcmx_trunk_get(l2addr.tgid, &t_data) >= 0) {
                        bcmx_lport_t lport;
                        int count, local_tx = 0, unit = 0;

                        if (BCMX_LPLIST_IS_EMPTY(&t_data.ports)) {
                            /*
                             * L2 table is programmed with tgid, but
                             * no trunk members found
                             */
                            rv = BCM_E_INTERNAL;
                        }

                        if(BCM_SUCCESS(rv)) {
                            BCMX_LPLIST_IDX_ITER(&t_data.ports, lport, count) {
                                unit = bcmx_lport_bcm_unit(lport);

                                if (unit < 0) {
                                    rv = BCM_E_PORT;
                                    break;
                                }

                                if ((BCM_SUCCESS(rv)) && (BCM_IS_LOCAL(unit))) {
                                    local_tx = 1;
                                    *d_port = lport;
                                    break;
                                }
                            }
                        }

                        if (BCM_SUCCESS(rv) && (!local_tx)) {
                            *d_port = bcmx_lplist_first(&t_data.ports);
                        }
                    } else {
                        /*
                         * bcmx_trunk_get failed; Non-existent tid probably
                         */
                        rv = BCM_E_INTERNAL;
                    }
                    bcmx_trunk_add_info_t_free(&t_data);
                    if(BCM_FAILURE(rv)) {
                        return rv;
                    }
                } else {
                    /* l2addr.port is overloaded with logical port */
                    *d_port = l2addr.lport;
                }
            }
        } else {
            return BCM_E_UNAVAIL;
        }

        if (*d_port == BCMX_LPORT_INVALID) {
            if (pkt->flags & BCMX_TX_F_DLF_DISCARD) {
                return _BCMX_TX_DISCARD;
            } else {
                return _BCMX_TX_FLOOD;
            }
        }

        return _BCMX_TX_UC;
    }

    return _BCMX_TX_BCM;
}

/*
 * Function:
 *      bcmx_tx
 * Purpose:
 *      Transmit a packet according to flags
 * Parameters:
 *      pkt    - The packet to transmit
 *      flags  - Flags as defined in include/bcmx/tx.h
 * Returns:
 *      BCMX_E_XXX < 0 on error.
 * Notes:
 *      If the L2 map flag is set, an L2 LOOKUP is performed on
 * the packet.  If this fails and the DLF_DISCARD flag is set
 * then the packet is not transmitted.  Otherwise it is flooded
 * to the packet's VLAN.  This is indicated by the return value
 * (see include/bcmx/tx.h).
 *
 *      If the L2 map flag is not set, the packet is sent on the
 * unit given by pkt->unit with the port bitmaps given by
 * pkt->tx_pbmp, etc.
 *
 *      The semantics match those of bcm_tx.  In particular,
 * the packet callback being non-null indicates a user callback
 * should be made and the transmit will be done asynchronously.
 *
 *      The packet's L3 and untagged bitmaps are AND-ed with the
 * tx bitmap.  The caller can use bcmx_tx_pkt_l3_set or
 * bcmx_tx_pkt_untagged_set to set all ports in the bitmap
 */

int
bcmx_tx(bcm_pkt_t *pkt, uint32 flags)
{
    bcmx_lport_t d_port;
    int rv;

    BCMX_TX_INIT_CHECK;

    if (pkt == NULL || pkt->pkt_data == NULL || pkt->blk_count <= 0) {
        return BCM_E_PARAM;
    }

    /* Try to L2 map if needed; if pkt discarded, return */
    d_port = BCMX_LPORT_INVALID;
    switch ((rv = pkt_addr_resolve(pkt, flags, &d_port))) {
    case _BCMX_TX_UC:  /* Okay */
        return bcmx_tx_uc(pkt, d_port, flags);

    case _BCMX_TX_FLOOD:  /* Packet to be flooded */
        return bcmx_tx_flood(pkt, flags);

    case _BCMX_TX_DISCARD:  /* Discarded */
        return BCMX_TX_DISCARDED;

        /* TO DO:  Implement multicast support here */
    case _BCMX_TX_BCM:
        return bcm_tx(pkt->unit, pkt, NULL);

    case _BCMX_TX_MCAST:
        return BCM_E_UNAVAIL;   /* Not yet implemented */

    default:
        /* Error */
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META("bcmx_tx: packet resolve error (%d): %s\n"),
                  rv, bcm_errmsg(rv)));
        break;
    }

    /* Error if we get here */
    return rv;
}


/*
 * Function:
 *      bcmx_tx_uc
 * Purpose:
 *      Transmit a packet out a specific front panel port
 * Parameters:
 *      pkt    - The packet to transmit
 *      d_port - The front panel destination port
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The semantics match those of bcm_tx.  In particular,
 * the packet callback being non-null indicates a user callback
 * should be made and the transmit will be done asynchronously.
 *
 *      The packet's L3 and untagged bitmaps are AND-ed with the
 * tx bitmap.  The caller can use bcmx_tx_pkt_l3_set or
 * bcmx_tx_pkt_untagged_set to set all ports in the bitmap
 */

int
bcmx_tx_uc(bcm_pkt_t *pkt, bcmx_lport_t d_port, uint32 flags)
{
    int          rv = BCM_E_NONE;
    bcm_port_t   port, modport;
    int          unit;
    bcm_module_t modid;

    BCMX_TX_INIT_CHECK;

    if (pkt == NULL || pkt->pkt_data == NULL || pkt->blk_count <= 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(d_port, &unit, &port,
                                 BCMX_DEST_CONVERT_NON_GPORT));
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(d_port, &modid, &modport,
                                  BCMX_DEST_CONVERT_DEFAULT));

    /* First, try to send the packet on a local unit, directing
     * the packet using the module ID.
     */
    if (modid < 0) { /* Bad modid */
        return BCM_E_BADID;
    }

    if (!BCM_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    pkt->dest_mod = modid;
    pkt->dest_port = modport;
    pkt->opcode = BCM_HG_OPCODE_UC;

    if (!BCM_IS_LOCAL(unit) && BCM_IS_LOCAL(_bcmx_tx_unit)) {
        /* Try to send directly; if unable to map, tunnel via bcm_tx. */
        bcm_port_t local_tx_port;

        rv = bcm_topo_port_get(_bcmx_tx_unit, (int)modid, &local_tx_port);
        if (!(flags & BCMX_TX_F_CPU_TUNNEL) && (rv == BCM_E_NONE) &&
            (local_tx_port != -1)) {
            /* Can transmit locally to port; otherwise tunneled */
            unit = _bcmx_tx_unit;
            port = local_tx_port;
        }
    }

    BCM_PBMP_PORT_SET(pkt->tx_pbmp, port);
    pkt->unit = unit;
    return bcm_tx(unit, pkt, NULL);
}


/*
 * Function:
 *      bcmx_tx_lplist
 * Purpose:
 *      Transmit a packet to a list of ports (efficiently)
 * Parameters:
 *      pkt             -- Pointer to packet to send
 *      tx_ports        -- List of ports to which to send; if NULL,
 *                         use untagged port list only
 *      untagged_ports  -- Egress ports which should be untagged
 *      flags           -- For future enhancements (unused)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Packet will be sent tagged unless:
 *           tx_ports is NULL/empty, in which case the packet is sent
 *           out untagged on all ports specified in the untagged_ports list;
 *           OR both tx_ports and untagged_ports params are non-NULL,
 *           in which case the packet is sent out untagged on the ports
 *           specified in untagged_ports.  If both are specified,
 *           untagged_ports must be a subset of tx_ports.
 *
 *      The port list is scanned first and the devices that appear
 *      on the list are extracted.  The code then iterates over
 *      those units and creates a bitmap for each unit.
 *
 *      Async transmit is supported, but it won't really take place
 *      until the last unit transmit is set up.
 */

int
bcmx_tx_lplist(bcm_pkt_t *pkt, bcmx_lplist_t *tx_ports,
               bcmx_lplist_t *untagged_ports, uint32 flags)
{
    bcm_unit_bmp_t tx_units;
    int unit;
    bcmx_lport_t lport;
    bcmx_lport_t cb_lport = BCMX_LPORT_INVALID;
    int i;
    bcm_pbmp_t tx_pbmp, ut_pbmp;
    int rv, final_rv = BCM_E_NONE;
    int unit_iter=0, unit_count = 0;
    bcm_pkt_cb_f pkt_callback = NULL;
    int all_untagged = FALSE;
    int local_unit_found = FALSE;
    int bcm_unit;
    bcm_port_t bcm_port;

    BCMX_TX_INIT_CHECK;

    if (tx_ports == NULL || BCMX_LPLIST_IS_EMPTY(tx_ports)) {
        if (untagged_ports == NULL || BCMX_LPLIST_IS_EMPTY(untagged_ports)) {
            LOG_VERBOSE(BSL_LS_BCMX_COMMON,
                        (BSL_META("bcmx_tx_lplist: No TX ports given\n")));
            return BCM_E_NONE;
        }
        all_untagged = TRUE;
        tx_ports = untagged_ports;
    }

    /* Only set callback on transmit of last packet.  Note that
     * we aren't really sending async until transmitting to the
     * last unit.
     */
    pkt_callback = pkt->call_back;
    pkt->call_back = NULL;

    /* Find which units being transmitted to, and which remote lport
       may require a callback.

       Only the last lport on a remote unit is eligible for a
       callback, and the presence of any local port will cancel such
       eligibility. */
    
    BCM_UNIT_BMP_CLEAR(tx_units);
    BCMX_LPLIST_IDX_ITER(tx_ports, lport, i) {
        unit = bcmx_lport_bcm_unit(lport);
        if (unit < 0) {
            /* Port got disconnected */
            return BCM_E_INTERNAL;
        }

        BCM_UNIT_BMP_ADD(tx_units, unit);

        /* See if there needs to be a callback for a remote port */
        if (!local_unit_found) {
            if (BCM_IS_LOCAL(unit)) {
                /* Local port found, so discontinue looking for a
                   callback port */
                cb_lport = BCMX_LPORT_INVALID;
                local_unit_found = TRUE;
            } else {
                cb_lport = lport;
            }
        }
    }

    if (!(flags & BCMX_TX_F_CPU_TUNNEL)) {
        /*
         * For the logical ports that reside on remote units, let us try and 
         * switch directly if a path is available (using bcmx_tx_uc())
         *
         * Note: the logical ports that are handled here using bcmx_tx_uc()
         * will be excluded from the unit iterator further below (that takes
         * care of logical ports residing on local units only)
         */
        BCMX_LPLIST_IDX_ITER(tx_ports, lport, i) {
            unit = bcmx_lport_bcm_unit(lport);
            if (unit < 0) {
                return BCM_E_INTERNAL;
            }
            if (BCM_IS_LOCAL(unit)) {
                continue;
            }

            if (BCM_UNIT_BMP_TEST(tx_units, unit)) {
                BCM_UNIT_BMP_REMOVE(tx_units, unit);
            }

            if (lport == cb_lport) {
                pkt->call_back = pkt_callback;
            }

            rv = bcmx_tx_uc(pkt, lport, flags);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_BCMX_COMMON,
                          (BSL_META_U(unit,
                                      "BCMX tx_lplist: Failed on unit %d port 0x%x: %s\n"),
                           unit, lport, bcm_errmsg(rv)));
                final_rv = rv;
            }
        }

    }

    /* Count the units for setting up callback if present */
    for (unit = 0; unit < BCM_UNITS_MAX; unit++) {
        if (BCM_UNIT_BMP_TEST(tx_units, unit)) {
            ++unit_count;
        }
    }

    /* Send out packet on units */
    for (unit = 0; unit < BCM_UNITS_MAX; unit++) {
        if (BCM_UNIT_BMP_TEST(tx_units, unit)) {
            if (++unit_iter == unit_count) {
                /* Restore callback for async operation on last pkt */
                pkt->call_back = pkt_callback;
            }

            /* Generate the transmit bitmaps for this device */
            BCM_PBMP_CLEAR(tx_pbmp);
            BCM_PBMP_CLEAR(ut_pbmp);
            BCMX_LPLIST_IDX_ITER(tx_ports, lport, i) {
                if (BCM_SUCCESS
                    (_bcmx_dest_to_unit_port(lport, &bcm_unit, &bcm_port,
                                             BCMX_DEST_CONVERT_NON_GPORT))) {
                    if (bcm_unit == unit) {
                        BCM_PBMP_PORT_ADD(tx_pbmp, bcm_port);
                    }
                }
            }
            if ((!all_untagged) &&
                (untagged_ports != NULL) &&
                (!BCMX_LPLIST_IS_EMPTY(untagged_ports))) {
                BCMX_LPLIST_IDX_ITER(untagged_ports, lport, i) {
                    if (BCM_SUCCESS
                        (_bcmx_dest_to_unit_port(lport,
                                                 &bcm_unit, &bcm_port,
                                                 BCMX_DEST_CONVERT_NON_GPORT))) {
                        if (bcm_unit == unit) {
                            BCM_PBMP_PORT_ADD(ut_pbmp, bcm_port);
                        }
                    }
                }
            }

            BCM_PBMP_ASSIGN(pkt->tx_pbmp, tx_pbmp);
            if (all_untagged) {
                BCM_PBMP_ASSIGN(pkt->tx_upbmp, tx_pbmp);
            } else {
                BCM_PBMP_ASSIGN(pkt->tx_upbmp, ut_pbmp);
            }
            pkt->unit = unit;
            rv = bcm_tx(unit, pkt, NULL);

            if (rv < 0) {
                LOG_ERROR(BSL_LS_BCMX_COMMON,
                          (BSL_META_U(unit,
                                      "BCMX tx_lplist: Failed on unit %d:%s\n"),
                           unit, bcm_errmsg(rv)));
                final_rv = rv;
            }
        }
    }

    return final_rv;

}


/*
 * Function:
 *      bcmx_tx_flood
 * Purpose:
 *      Flood a packet on its VLAN
 * Parameters:
 *      pkt    - The packet to transmit
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The CRC field in pkt->flags must be set before calling.
 */

int
bcmx_tx_flood(bcm_pkt_t *pkt, uint32 flags)
{
    int rv;
    bcm_port_config_t config;

    COMPILER_REFERENCE(flags);

    BCMX_TX_INIT_CHECK;

    /* To do:  detect tunnel and forward to each CPU in system */

    if (pkt == NULL || pkt->pkt_data == NULL || pkt->blk_count <= 0) {
        return BCM_E_PARAM;
    }

    rv = bcm_port_config_get(_bcmx_tx_flood_unit, &config);
    if (rv < 0) {
        return rv;
    }

    BCM_PBMP_ASSIGN(pkt->tx_pbmp, config.port);
    pkt->unit = _bcmx_tx_flood_unit;
    pkt->opcode = BCM_HG_OPCODE_BC;
    rv = bcm_tx(_bcmx_tx_flood_unit, pkt, NULL);

    if (rv < 0) {
        return rv;
    } else {
        return BCMX_TX_FLOODED;
    }
}


/*
 * Function:
 *      bcmx_tx_pkt_l3_set
 * Purpose:
 *      Set l3 characteristic for a bcm pkt sent thru bcmx
 * Parameters:
 *      pkt         - the pkt to affect
 *      l3          - boolean; should pkt be sent as l3
 * Returns:
 *      BCM_E_XXX
 */

int
bcmx_tx_pkt_l3_set(bcm_pkt_t *pkt, int l3)
{
    bcm_pbmp_t tmp_pbm;

    if (pkt) {
        if (l3) {
            BCM_PBMP_CLEAR(tmp_pbm);
            BCM_PBMP_NEGATE(pkt->tx_l3pbmp, tmp_pbm);
        } else {
            BCM_PBMP_CLEAR(pkt->tx_l3pbmp);
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmx_tx_pkt_untagged_set
 * Purpose:
 *      Set untagged characteristic for a bcm pkt sent thru bcmx
 * Parameters:
 *      pkt         - the pkt to affect
 *      untagged    - boolean; should pkt be tagged
 * Returns:
 *      BCM_E_XXX
 */

int
bcmx_tx_pkt_untagged_set(bcm_pkt_t *pkt, int untagged)
{
    bcm_pbmp_t tmp_pbm;

    if (pkt) {
        if (untagged) {
            BCM_PBMP_CLEAR(tmp_pbm);
            BCM_PBMP_NEGATE(pkt->tx_upbmp, tmp_pbm);
        } else {
            BCM_PBMP_CLEAR(pkt->tx_upbmp);
        }
    }

    return BCM_E_NONE;
}



/****************************************************************
 *
 * The following calls are DEPRECATED.
 */

/*
 * Function:
 *      bcmx_tx_port_list   DEPRECATED:  Use bcmx_tx_lplist.
 * Purpose:
 *      Transmit a packet to a list of ports
 * Parameters:
 *      lplist    - lport list of destinations
 *      pkt       - pointer to packet to send
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Returns last error seen if one occurs and continues attempting
 *      to send.
 *
 *      Not sophisticated yet; just sends out to each port individually.
 *      This could generate the bitmaps per unit and send them out
 *      once per unit.
 */

int
bcmx_tx_port_list(bcmx_lplist_t *lplist, bcm_pkt_t *pkt)
{
    bcmx_lport_t lport;
    int count;
    int rv = BCM_E_NONE;
    int rv_keep = BCM_E_NONE;

    BCMX_TX_INIT_CHECK;

    if (pkt->call_back != NULL) {
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META("WARNING: bcmx_tx_port_list: async not supported\n")));
    }
    BCMX_LPLIST_ITER(*lplist, lport, count) {
        rv = bcmx_tx_uc(pkt, lport, 0);
        if (rv < 0) {
            rv_keep = rv;
        }
    }

    return rv_keep;
}
