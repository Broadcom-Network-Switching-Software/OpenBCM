/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software and
 * all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 * LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
 * AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE
 * OF THE SOFTWARE. 
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and
 * to use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 * INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
 * RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF
 * BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE
 * ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL
 * APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED
 * REMEDY.
 */

#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <shared/bsl.h>
#include <bcm_int/control.h>
#include <bcm/error.h>

#include <bcm/txbeacon.h>

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)

/* Version of the SDK-side app code */
#define TXBEACON_SDK_VERSION    0x01000000
#define TXBEACON_UK_MIN_VERSION 0x01000000


/*
 * Function:
 *      _bcm_esw_txbeacon_hton
 * Purpose:
 *      Internal routine to convert txbeacon area endianness
 * Parameters:
 *      host - pointer to host struct
 *      net - pointer to net (uC) struct
 * Returns:
 *      nothing
 * Notes:
 */

void
_bcm_esw_txbeacon_hton(bcm_txbeacon_t *host, bcm_txbeacon_t *net)
{
    net->flags = soc_htonl(host->flags);
    net->len = soc_htons(host->len);
    net->maxlen = soc_htons(host->maxlen);
    net->pkt_id = soc_htonl(host->pkt_id);
    net->port = soc_htons(host->port);
    net->interval = soc_htons(host->interval);
    net->count = soc_htons(host->count);
}

/*
 * Function:
 *      _bcm_esw_txbeacon_ntoh
 * Purpose:
 *      Internal routine to convert txbeacon area endianness
 * Parameters:
 *      host - pointer to host struct
 *      net - pointer to net (uC) struct
 * Returns:
 *      nothing
 * Notes:
 */

void
_bcm_esw_txbeacon_ntoh(bcm_txbeacon_t *net, bcm_txbeacon_t *host)
{
    host->flags = soc_ntohl(net->flags);
    host->len = soc_ntohs(net->len);
    host->maxlen = soc_ntohs(net->maxlen);
    host->pkt_id = soc_ntohl(net->pkt_id);
    host->port = soc_ntohs(net->port);
    host->interval = soc_ntohs(net->interval);
    host->count = soc_ntohs(net->count);
}
#endif

/*
 * Function:
 *      bcm_esw_txbeacon_init
 * Purpose:
 *      Init routine for TX beacon
 * Parameters:
 *      unit - for what unit is this being set up
 *      uC - Microcontroller to handle TX beacons
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_txbeacon_init(int unit, int uC)
{
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    soc_control_t       *soc = SOC_CONTROL(unit);

    if ( (!soc_feature(unit, soc_feature_cmicm)) &&
         (!soc_feature(unit, soc_feature_cmicx)) ) {
        return (BCM_E_UNAVAIL);
    } 

    soc->txbeacon_msg_timeout =
        soc_property_get(unit, spn_UC_MSG_TX_BEACON_TIMEOUT, 200000000);

    /* Send the init msg to the uC and wait for an "APPL ready"
       message */
    if (soc_cmic_uc_appl_init(unit, uC, MOS_MSG_CLASS_PKT_BCN,
                              soc->txbeacon_msg_timeout,
                              TXBEACON_SDK_VERSION,
                              TXBEACON_UK_MIN_VERSION,
                              NULL, NULL) != SOC_E_NONE) {
        return (BCM_E_UNAVAIL);
    }

    soc->txbeacon_uC = uC;
    soc->txbeacon_init = 1;

    LOG_CLI((BSL_META_U(unit,
                        "txbeacon ready\n")));
    
    return (BCM_E_NONE);

#else
    return (BCM_E_UNAVAIL);

#endif 
}

/*
 * Function:
 *      bcm_esw_txbeacon_pkt_setup
 * Purpose:
 *      Add a packet for TX beacon
 * Parameters:
 *      unit - for what unit is this being set up
 *      txbeacon - pkt beacon info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_txbeacon_pkt_setup(
    int unit,
    bcm_txbeacon_t *txbeacon)
{
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)

    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_info_t          *si = &SOC_INFO(unit);
    mos_msg_data_t      send, rcv;
    int rc = BCM_E_NONE;
    char *dma_mem;

    if ( (!soc_feature(unit, soc_feature_cmicm)) &&
         (!soc_feature(unit, soc_feature_cmicx)) ) {
        return (BCM_E_UNAVAIL);
    } 

    if (soc->txbeacon_init == 0) {
        return (BCM_E_INIT);
    }

    if (txbeacon->port >= SOC_MAX_NUM_PORTS) {
        return (BCM_E_PARAM);
    } 

    /* Make a dma-able copy */
    dma_mem = soc_cm_salloc(unit,
                            MOS_MSG_DMA_LEN(sizeof(bcm_txbeacon_t) +
                                            txbeacon->len),
                            "txbeacon copy");
    if (dma_mem == NULL) {
        return (BCM_E_MEMORY);
    }
    _bcm_esw_txbeacon_hton(txbeacon, (bcm_txbeacon_t *) dma_mem);
    sal_memcpy(dma_mem + sizeof(bcm_txbeacon_t), txbeacon->pkt_data, txbeacon->len);
    soc_cm_sflush(unit, dma_mem, sizeof(bcm_txbeacon_t) + txbeacon->len);

    send.s.mclass = MOS_MSG_CLASS_PKT_BCN;
    send.s.subclass = MOS_MSG_SUBCLASS_PKT_BCN_CONFIG;
    /* Cheat and use the len for the queue num */
    send.s.len = soc_htons(si->port_cosq_base[txbeacon->port]);
    send.s.data = soc_htonl(soc_cm_l2p(unit, dma_mem));

    rcv.s.mclass = MOS_MSG_CLASS_PKT_BCN;

    if ((soc_cmic_uc_msg_send_receive(unit, soc->txbeacon_uC,
                                      &send, &rcv,
                                      soc->txbeacon_msg_timeout) !=
         SOC_E_NONE) ||
        (rcv.s.data != 0)) {
        rc = BCM_E_FAIL;
    }

    soc_cm_sfree(unit, dma_mem);

    return(rc);

#else
    return (BCM_E_UNAVAIL);

#endif 
}

/*
 * Function:
 *      bcm_esw_txbeacon_destroy
 * Purpose:
 *      Remove this packet from the list
 * Parameters:
 *      unit - for what unit is this being set up
 *      pkt_id - ID of packet to destroy
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_txbeacon_destroy(
    int unit,
    bcm_txbeacon_pkt_id_t pkt_id) {
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)

    soc_control_t       *soc = SOC_CONTROL(unit);
    mos_msg_data_t      send, rcv;

    if ( (!soc_feature(unit, soc_feature_cmicm)) &&
         (!soc_feature(unit, soc_feature_cmicx)) ) {
        return (BCM_E_UNAVAIL);
    } 

    if (soc->txbeacon_init == 0) {
        return (BCM_E_INIT);
    } 

    send.s.mclass = MOS_MSG_CLASS_PKT_BCN;
    send.s.subclass = MOS_MSG_SUBCLASS_PKT_BCN_DESTROY;
    send.s.len = soc_htons(4);
    send.s.data = soc_htonl(pkt_id);

    if ((soc_cmic_uc_msg_send_receive(unit, soc->txbeacon_uC, &send, &rcv, 
                                      soc->txbeacon_msg_timeout) != SOC_E_NONE) ||
        (rcv.s.data != 0)) {
        return(BCM_E_FAIL);
    }

    return(BCM_E_NONE);

#else
    return (BCM_E_UNAVAIL);

#endif 
}

/*
 * Function:
 *      bcm_esw_txbeacon_pkt_get
 * Purpose:
 *      Lookup packet beacon info structure
 * Parameters:
 *      unit - for what unit is this being set up
 *      txbeacon - pkt beacon info.  Should have the pkt_id, pkt_ptr,
 *                  and maxlen fields set.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Find the packet with the minimum packt ID which is greater
 *      than or equal to the pkt_id in the txbeacon into.  This can
 *      be used in a loop to recover all of the packets by starting
 *      with zero and then incrementing the returned pkt_id until a
 *      zero pkt_id is returned.
 *
 *      The txbeacon info must be filled in with the pkt_id, and a
 *      pkt_pointer.  Up to maxlen bytes are copied into the
 *      pkt_pointer.
 */

int bcm_esw_txbeacon_pkt_get(
    int unit,
    bcm_txbeacon_t *txbeacon) {
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)

    uint8               *dma_mem = NULL;
    soc_control_t       *soc = SOC_CONTROL(unit);
    mos_msg_data_t      send, rcv;
    uint8 *             pkt_data = txbeacon->pkt_data;

    int rc = BCM_E_NONE;
    int maxlen = txbeacon->maxlen;
    int size = 0;

    if ( (!soc_feature(unit, soc_feature_cmicm)) &&
         (!soc_feature(unit, soc_feature_cmicx)) ) {
        return (BCM_E_UNAVAIL);
    } 

    if (soc->txbeacon_init == 0) {
        return (BCM_E_INIT);
    }

    /* Get a dma-able region and copy the pkt data in*/
    size = MOS_MSG_DMA_LEN(sizeof(bcm_txbeacon_t) + maxlen);
    dma_mem = soc_cm_salloc(unit, size, "dma");
    if (dma_mem == NULL) {
        return (BCM_E_MEMORY);
    }

    _bcm_esw_txbeacon_hton(txbeacon, (bcm_txbeacon_t *) dma_mem);
    soc_cm_sflush(unit, dma_mem, sizeof(bcm_txbeacon_t));
    soc_cm_sinval(unit, dma_mem, size);

    send.s.mclass = MOS_MSG_CLASS_PKT_BCN;
    send.s.subclass = MOS_MSG_SUBCLASS_PKT_BCN_GET;
    send.s.len = soc_htons(sizeof(bcm_txbeacon_t));
    send.s.data = soc_htonl(soc_cm_l2p(unit, dma_mem));

    rcv.s.mclass = MOS_MSG_CLASS_PKT_BCN;

    rc = soc_cmic_uc_msg_send_receive(unit, soc->txbeacon_uC, &send, &rcv, 
                                      soc->txbeacon_msg_timeout);
    if (rc == SOC_E_NONE) {
        /* Copy the data back out to the user area */
        _bcm_esw_txbeacon_ntoh((bcm_txbeacon_t *) dma_mem, txbeacon);
        txbeacon->pkt_data = pkt_data;          /* Restore data ptr */
        sal_memcpy(pkt_data, dma_mem + sizeof(bcm_txbeacon_t),
                   (maxlen > txbeacon->len) ? txbeacon->len : maxlen);
    } 

    soc_cm_sfree(unit, dma_mem);
    
    return(rc);

#else
    return (BCM_E_UNAVAIL);

#endif 
}

/*
 * Function:
 *      bcm_esw_txbeacon_start
 * Purpose:
 *      Start beacon for this packet or all
 * Parameters:
 *      unit - for what unit is this being set up
 *      pkt_id - ID of packet to start or zero for all
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_txbeacon_start(
     int unit,
     bcm_txbeacon_pkt_id_t pkt_id) {
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)

    mos_msg_data_t      send, rcv;

    soc_control_t       *soc = SOC_CONTROL(unit);
    
    if ( (!soc_feature(unit, soc_feature_cmicm)) &&
         (!soc_feature(unit, soc_feature_cmicx)) ) {
        return (BCM_E_UNAVAIL);
    } 

    if (soc->txbeacon_init == 0) {
        return BCM_E_INIT;
    } 

    send.s.mclass = MOS_MSG_CLASS_PKT_BCN;
    send.s.subclass = MOS_MSG_SUBCLASS_PKT_BCN_START;
    send.s.len = soc_htons(4);
    send.s.data = soc_htonl(pkt_id);

    rcv.s.mclass = MOS_MSG_CLASS_PKT_BCN;

    if ((soc_cmic_uc_msg_send_receive(unit, soc->txbeacon_uC, &send, &rcv, 
                                      soc->txbeacon_msg_timeout) != SOC_E_NONE) ||
        (rcv.s.data != 0)) {
        return(BCM_E_FAIL);
    }

    return(BCM_E_NONE);

#else
    return (BCM_E_UNAVAIL);

#endif 
}

/*
 * Function:
 *      bcm_esw_txbeacon_stop
 * Purpose:
 *      Stop the packet beacon
 * Parameters:
 *      unit - for what unit is this being set up
 *      pkt_id - ID of packet to stop or zero for all
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_txbeacon_stop(
    int unit,
    bcm_txbeacon_pkt_id_t pkt_id) {
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)

    mos_msg_data_t      send, rcv;

    soc_control_t       *soc = SOC_CONTROL(unit);
    
    if ( (!soc_feature(unit, soc_feature_cmicm)) &&
         (!soc_feature(unit, soc_feature_cmicx)) ) {
        return (BCM_E_UNAVAIL);
    } 

    if (soc->txbeacon_init == 0) {
        return BCM_E_INIT;
    } 

    send.s.mclass = MOS_MSG_CLASS_PKT_BCN;
    send.s.subclass = MOS_MSG_SUBCLASS_PKT_BCN_STOP;
    send.s.len = soc_htons(4);
    send.s.data = soc_htonl(pkt_id);

    rcv.s.mclass = MOS_MSG_CLASS_PKT_BCN;

    if ((soc_cmic_uc_msg_send_receive(unit, soc->txbeacon_uC, &send, &rcv, 
                                      soc->txbeacon_msg_timeout) != SOC_E_NONE) ||
        (rcv.s.data != 0)) {
        return(BCM_E_FAIL);
    }

    return(BCM_E_NONE);

#else
    return (BCM_E_UNAVAIL);

#endif 
}

/*
 * Function:
 *      bcm_esw_txbeacon_traverse
 * Purpose:
 *      Traverse the tx beacon packets
 * Parameters:
 *      unit - for what unit is this being set up
 *      txbeacon_traverse_cb
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_txbeacon_traverse(
    int unit,
    bcm_txbeacon_traverse_cb_t trav_fn,
    void *user_data)
{
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)

    soc_control_t       *soc = SOC_CONTROL(unit);
    mos_msg_data_t      send, rcv;
    bcm_txbeacon_pkt_id_t pkt_id = 0;
    int rc = BCM_E_NONE;
    bcm_txbeacon_t *txb;
    int size;
    bcm_txbeacon_t *pkt;

    if ( (!soc_feature(unit, soc_feature_cmicm)) &&
         (!soc_feature(unit, soc_feature_cmicx)) ) {
        return (BCM_E_UNAVAIL);
    } 

    if (soc->txbeacon_init == 0) {
        return (BCM_E_INIT);
    }

    /* Allocate a dma-able area just big enough for the beacon info */
    txb = soc_cm_salloc(unit, MOS_MSG_DMA_LEN(sizeof(bcm_txbeacon_t)),
                        "traverse info");
    if (txb == NULL) {
        return(BCM_E_MEMORY);
    }
    
    while (1) {

        /* Set up the info-only area */
        txb->pkt_id = soc_htonl(pkt_id);
        txb->maxlen = 0;
        txb->pkt_data = NULL;
        soc_cm_sflush(unit, txb, sizeof(bcm_txbeacon_t));
        soc_cm_sinval(unit, txb, sizeof(bcm_txbeacon_t));
        
        send.s.mclass = MOS_MSG_CLASS_PKT_BCN;
        send.s.subclass = MOS_MSG_SUBCLASS_PKT_BCN_GET;
        send.s.len = soc_htons(sizeof(bcm_txbeacon_t));
        send.s.data = soc_htonl(soc_cm_l2p(unit, txb));
    
        if ((soc_cmic_uc_msg_send_receive(unit, soc->txbeacon_uC, &send, &rcv, 
                                          soc->txbeacon_msg_timeout) != SOC_E_NONE) ||
            (rcv.s.data != 0)) {
            break;
        } 

        /* Copy actual packet ID out */
        pkt_id = soc_htonl(txb->pkt_id);

        /* Allocate an area big enough for everything */
        size = MOS_MSG_DMA_LEN(sizeof(bcm_txbeacon_t) + soc_htons(txb->maxlen));
        pkt = soc_cm_salloc(unit, size, "traverse packet");
        if (pkt == NULL) {
            rc = BCM_E_MEMORY;
            break;
        }


        pkt->pkt_id = soc_htonl(pkt_id);
        pkt->maxlen = soc_htons(txb->maxlen);
        pkt->pkt_data = INT_TO_PTR(soc_cm_l2p(unit, ((char *) pkt) + sizeof(bcm_txbeacon_t)));
        soc_cm_sflush(unit, pkt, size);
        soc_cm_sinval(unit, pkt, size);


        /* Resend the request - this time with the len big enough for
           the whole packet */
        send.s.len = soc_htons(sizeof(bcm_txbeacon_t));
        send.s.data = soc_htonl(soc_cm_l2p(unit, pkt));
    
        if ((soc_cmic_uc_msg_send_receive(unit, soc->txbeacon_uC, &send, &rcv, 
                                          soc->txbeacon_msg_timeout) != SOC_E_NONE) ||
            (rcv.s.data != 0)) {
            rc = BCM_E_FAIL;
            soc_cm_sfree(unit, pkt);
            break;
        }

        /* Set the packet point to the virtual addr and call the loopback function */
        _bcm_esw_txbeacon_ntoh(pkt, pkt);
        pkt->pkt_data =  (((uint8 *) pkt) + sizeof(bcm_txbeacon_t));

        trav_fn(unit, pkt, user_data);
        
        soc_cm_sfree(unit, pkt);                /* Free the big one */

        pkt_id++;
    }

    soc_cm_sfree(unit, txb);

    return rc;

#else
    return (BCM_E_UNAVAIL);

#endif 
}

