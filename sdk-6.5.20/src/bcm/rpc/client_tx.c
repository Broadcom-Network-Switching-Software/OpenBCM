/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        client_tx.c
 * Purpose:     Client implementation of bcm_tx* functions.
 * Requires:
 */

#include <shared/bsl.h>

#include <bcm/tx.h>
#include <bcm/error.h>

#if defined(BROADCOM_DEBUG)
#include <soc/debug.h>
#include <soc/cm.h>
#define TX_DEBUG(stuff)         LOG_ERROR(BSL_LS_SOC_COMMON, stuff)
#else
#define TX_DEBUG(stuff)
#endif  /* BROADCOM_DEBUG */


/*
 * Function:
 *      bcm_client_tx_init
 * Purpose:
 *      Initialize BCM TX API on client
 * Parameters:
 *      unit - transmission unit
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_client_tx_init(int unit)
{
    /* Set up TX tunnel receiver and transmitter */
    /* Currently this is done elsewhere */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_client_tx_cleanup
 * Purpose:
 *      Cleanup resources created by BCM TX init API on client
 * Parameters:
 *      unit - transmission unit
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_client_tx_cleanup(int unit)
{
    /* Currently this is done elsewhere */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_client_tx
 * Purpose:
 *     Transmit a single packet on client
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
bcm_client_tx(int unit, bcm_pkt_t *pkt, void *cookie)
{
    /* Tunnel the packet to the remote CPU */
    bcm_cpu_tunnel_mode_t mode = BCM_CPU_TUNNEL_PACKET; /* default */

    if (pkt->call_back != NULL && cookie != NULL) {
        TX_DEBUG(("bcm_tx ERROR:  "
                     "Cookie non-NULL on async tunnel call\n"));
        return BCM_E_PARAM;
    }

    if (pkt->flags & BCM_TX_BEST_EFFORT) {
        mode = BCM_CPU_TUNNEL_PACKET_BEST_EFFORT;
    } else if (pkt->flags & BCM_TX_RELIABLE) {
        mode = BCM_CPU_TUNNEL_PACKET_RELIABLE;
    }

    return bcm_tx_cpu_tunnel(pkt, unit, 0, BCM_CPU_TUNNEL_F_PBMP, mode);
}

/*
 * Function:
 *      bcm_client_tx_array
 * Purpose:
 *      Transmit an array of packets on client
 * Parameters:
 *      unit - transmission unit
 *      pkt - array of pointers to packets to transmit
 *      count - Number of packets in list
 *      all_done_cb - Callback function (if non-NULL) when all pkts trx'd
 *      cookie - Callback cookie.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Can't tunnel an array to the client, so return error.
 */

int
bcm_client_tx_array(int unit, bcm_pkt_t **pkt, int count,
                    bcm_pkt_cb_f all_done_cb, void *cookie)
{
    TX_DEBUG(("bcm_tx_array ERROR:  Cannot tunnel\n"));
    return BCM_E_PARAM;
}

/*
 * Function:
 *      bcm_client_tx_list
 * Purpose:
 *      Transmit a linked list of packets
 * Parameters:
 *      unit - transmission unit
 *      pkt - Pointer to linked list of packets
 *      all_done_cb - Callback function (if non-NULL) when all pkts trx'd
 *      cookie - Callback cookie.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Can't tunnel a list to the client, so return error.
 */

int
bcm_client_tx_list(int unit, bcm_pkt_t *pkt, bcm_pkt_cb_f all_done_cb,
                   void *cookie)
{
    TX_DEBUG(("bcm_tx_list ERROR:  Cannot tunnel\n"));
    return BCM_E_PARAM;
}

/*
 * Function:
 *      bcm_client_tx_pkt_setup
 * Purpose:
 *      Default packet setup routine for transmit on client
 * Parameters:
 *      unit         - Unit on which being transmitted
 *      tx_pkt       - Packet to update
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_client_tx_pkt_setup(int unit, bcm_pkt_t *tx_pkt)
{
    return BCM_E_NONE;
}
