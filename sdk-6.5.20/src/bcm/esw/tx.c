/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tx.c
 * Purpose:
 * Requires:
 *
 * Notes on internals:
 *    The basic tx function is bcm_tx.  In this case, a chain
 *    corresponds to a single packet.  The done chain interrupt (dv done)
 *    is used to indicate the end of packet for async transmission.
 *
 *    Arrays may be transmitted using bcm_tx_array; similarly, linked lists
 *    can be transmitted with bcm_tx_list.  In both cases, the
 *    tx_dv_info_t structure is associated with the DV.  Currently,
 *    only one call back is supported for a chain of packets.  It
 *    wouldn't be too difficult to have a per packet callback using
 *    the packet's call back member.
 */

/* We need to call top-level APIs on other units */
#include <assert.h>
#include <bcm_int/common/tx.h>
#ifdef BCM_TRIDENT3_SUPPORT
#include <bcm_int/esw/trident3.h>
#endif
#include <soc/drv.h>
/*
 * Function:
 *      bcm_tx_pkt_setup
 * Purpose:
 *      Default packet setup routine for transmit
 * Parameters:
 *      unit         - Unit on which being transmitted
 *      tx_pkt       - Packet to update
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      tx_pkt->tx_pbmp should be set before calling this function
 *      Currently, the main thing this does is force the HG header
 *      on Hercules.
 */

int
bcm_esw_tx_pkt_setup(int unit, bcm_pkt_t *tx_pkt)
{
    return bcm_common_tx_pkt_setup(unit, tx_pkt);
}

/*
 * Function:
 *      bcm_tx_init
 * Purpose:
 *      Initialize BCM TX API
 * Parameters:
 *      unit - transmission unit
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Currently, this just allocates shared memory space for the
 *      CRC append memory.  This memory is zero and used by all packets
 *      that require CRC append (allocate).
 *
 *      This pointer is only allocated once.  If allocation fails,
 *      BCM_E_MEMORY is returned.
 */

int
bcm_esw_tx_init(int unit)
{
    return bcm_common_tx_init(unit);    
}

/*
 * Function:
 *      bcm_esw_tx_cleanup
 * Purpose:
 *      Clean up resources created during TX initialization.
 * Parameters:
 *      unit - transmission unit
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tx_cleanup(int unit)
{

#ifdef BCM_XGS3_SWITCH_SUPPORT    
    /* Terminate global bcmAsyncTX thread */
    bcm_common_xgs3_async_tx_thread_term();
#endif    
    /* Termiante global bcmTX thread */
    return bcm_common_tx_deinit(unit);

}

/*
 * Function:
 *      bcm_tx
 * Purpose:
 *      Wrapper for _bcm_tx to work around a lack of XGS3 support for
 *      transmitting to a bitmap.
 * Parameters:
 *      unit - transmission unit
 *      pkt - The tx packet structure
 *      cookie - Callback cookie when tx done
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      XGS3 devices cannot dispatch packets to multiple ports. To emulate this
 *      functionality, bcm_tx iterates over the requested bitmaps. The callback,
 *      if applicable, shall be called only once.
 */

int
bcm_esw_tx(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int rv;
    
#ifdef BCM_TRIUMPH3_SUPPORT 
    bcm_port_t port_iter;
    bcm_pbmp_t lock_pbmp; 
    
extern void _bcm_esw_ibod_recovery_port_lock(int unit, bcm_port_t port);
extern void _bcm_esw_ibod_recovery_port_unlock(int unit, bcm_port_t port);
    /* to avoid any change in tx_pbmp leaving the port in locked state */ 
    BCM_PBMP_ASSIGN(lock_pbmp, pkt->tx_pbmp); 

    /* Hold the ibod recovery port group lock */
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_PBMP_ITER(lock_pbmp, port_iter) {
            _bcm_esw_ibod_recovery_port_lock(unit, port_iter);
        }
    }
#endif
    rv = bcm_common_tx(unit, pkt, cookie);
#ifdef BCM_TRIUMPH3_SUPPORT
    /* Release the ibod recovery port group lock */
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_PBMP_ITER(lock_pbmp, port_iter) {
            _bcm_esw_ibod_recovery_port_unlock(unit, port_iter);
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_tx_array
 * Purpose:
 *      Transmit an array of packets
 * Parameters:
 *      unit - transmission unit
 *      pkt - array of pointers to packets to transmit
 *      count - Number of packets in list
 *      all_done_cb - Callback function (if non-NULL) when all pkts tx'd
 *      cookie - Callback cookie.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If all_done_cb is non-NULL, the packets are sent asynchronously
 *      (the routine returns before all the pkts are sent)
 *
 *      The "packet callback" will be set according to the value
 *      in each packet's "call_back" member, so that must be initialized
 *      for all packets in the chain.
 *
 *      If any packet requires a callback, the packet-done callback is
 *      enabled for all packets in the chain.
 *
 *      This routine does not support tunnelling to a remote CPU for
 *      forwarding.
 *
 *      CURRENTLY:  The TX parameters, src mod, src port, PFM
 *      and internal priority, are determined by the first packet in
 *      the array.  This may change to break the array into subchains
 *      when differences are detected.
 */

int
bcm_esw_tx_array(int unit, bcm_pkt_t **pkt, int count, bcm_pkt_cb_f all_done_cb,
             void *cookie)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit) && soc_feature(unit, soc_feature_higig_over_ethernet)) {
        return bcm_td3_tx_hgoe_array(unit, pkt, count, all_done_cb, cookie);
    }
#endif
    return bcm_common_tx_array(unit, pkt, count, all_done_cb, cookie);
}

/*
 * Function:
 *      bcm_tx_list
 * Purpose:
 *      Transmit a linked list of packets
 * Parameters:
 *      unit - transmission unit
 *      pkt - Pointer to linked list of packets
 *      all_done_cb - Callback function (if non-NULL) when all pkts tx'd
 *      cookie - Callback cookie.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If callback is non-NULL, the packets are sent asynchronously
 *      (the routine returns before all the pkts are sent)
 *
 *      The "packet callback" will be set according to the value
 *      in each packet's "call_back" member, so that must be initialized
 *      for all packets in the chain.
 *
 *      The "next" member of the packet is used for the linked list.
 *      CAREFUL:  The internal _next member is not used for this.
 *
 *      This routine does not support tunnelling to a remote CPU for
 *      forwarding.
 *
 *      The TX parameters, src mod, src port, PFM and internal priority,
 *      are currently determined by the first packet in the list.
 */

int
bcm_esw_tx_list(int unit, bcm_pkt_t *pkt, bcm_pkt_cb_f all_done_cb, void *cookie)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)
           && soc_feature(unit, soc_feature_higig_over_ethernet)) {
        return bcm_td3_tx_hgoe_list(unit, pkt, all_done_cb, cookie);
    }
#endif
    return bcm_common_tx_list(unit, pkt, all_done_cb, cookie);
}


/****************************************************************
 *
 * Map a destination MAC address to port bitmaps.  Uses
 * the dest_mac passed as a parameter.  Sets the
 * bitmaps in the pkt structure.
 */

/*
 * Function:
 *      bcm_tx_pkt_l2_map
 * Purpose:
 *      Resolve the packet's L2 destination and update the necessary
 *      fields of the packet.
 * Parameters:
 *      unit - Transmit unit
 *      pkt - Pointer to pkt being transmitted
 *      dest_mac - use for L2 lookup
 *      vid - use for L2 lookup
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Deprecated, not implemented.
 */

int
bcm_esw_tx_pkt_l2_map(int unit, bcm_pkt_t *pkt, bcm_mac_t dest_mac, int vid)
{
    return bcm_common_tx_pkt_l2_map(unit, pkt, dest_mac, vid);
}


/****************************************************************
 *
 * TX DV info dump routine.
 */

#if defined(BROADCOM_DEBUG)
/*
 * Function:
 *      bcm_tx_show
 * Purpose:
 *      Display info about tx state
 * Parameters:
 *      unit - ignored
 * Returns:
 *      None
 * Notes:
 */

int
bcm_esw_tx_show(int unit)
{
    return bcm_common_tx_show(unit);
}

/*
 * Function:
 *      bcm_tx_dv_dump
 * Purpose:
 *      Display info about a DV that is setup for TX.
 * Parameters:
 *      unit - transmission unit
 *      dv - The DV to show info about
 * Returns:
 *      None
 * Notes:
 *      Mainly, dumps the tx_dv_info_t structure; then calls soc_dma_dump_dv
 */

int
bcm_esw_tx_dv_dump(int unit, void *dv_p)
{
    return bcm_common_tx_dv_dump(unit, dv_p);
}

#endif  /* BROADCOM_DEBUG */

