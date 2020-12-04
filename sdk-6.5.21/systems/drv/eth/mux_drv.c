/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(VXWORKS) && (VX_VERSION > 54)

#include <stdlib.h>
#include <end.h>
#include <taskLib.h>
#include <muxLib.h>
#include <muxTkLib.h>
#include <netBufLib.h>
#include <errnoLib.h>
#include <time.h>
#include <cacheLib.h>
#include <logLib.h>
#include <intLib.h>

#ifdef VXWORKS_NETWORK_STACK_6_5 
#include <if.h>
#include <string.h>
#endif /* VXWORKS_NETWORK_STACK_6_5 */

#include "mux_drv.h"

#define ET_DRV_TXRX_DEBUG
#define ET_DRV_TX_TEST_XXXX

#define ET_DRV_TX_RX_PK_SZ    2048
#define ET_DRV_TXRX_CLBLKS    256
#define ET_DRV_TXRX_MBLKS     256
#define ET_DRV_TXRX_CLBUFS    256

typedef struct et_drv_txrx_info_s {
    void *          et_drvMuxBindID;
    void *          et_drvNetPoolID;
    M_CL_CONFIG     m_cl_blks;
    CL_DESC         cluster;
    CL_POOL_ID      clpool_id;
    cb_f            rx_cb;
    void *          cookie;
    alloc_f         rx_alloc;
    free_f          rx_free;
    void *          rx_head;
    void *          rx_tail;
    UINT32          queue_rx_pkts;
    UINT32          rx_out_of_mem;
    UINT32          rx_cnt;
    UINT32          tx_cnt;
    UINT32          tx_fail;
} et_drv_txrx_info_t;

LOCAL et_drv_txrx_info_t et_drv_txrx_info[ETH_MAX_UNITS];
#ifdef ET_DRV_TXRX_DEBUG
int  et_drv_txrx_debug = 0;
#endif

#define ET_DRV_TXRX_DEBUG_F   0x00000001
#define ET_DRV_TXRX_DEBUG_P   0x00000002
#define ET_DRV_TXRX_PRINT if (et_drv_txrx_debug & ET_DRV_TXRX_DEBUG_P) logMsg


#ifdef ET_DRV_TXRX_DEBUG
/* debug test functions */
void et_drv_tx_test(int unit, int l);
void et_drv_rx_test(int unit);
void et_drv_tx_netpool_show(int unit);

#include <stdio.h>

/* Debug */
LOCAL void
et_drv_txrx_pp(UINT8 *p, int l)
{
    int i;

    for(i=0; i < l; i++) {
        if ((i % 16) == 0) {
            printf("\n%02x :", i);
        }
        printf("%02x ", *p++);
    }
    printf("\n");
}

/* Debug */
LOCAL void
et_drv_txrx_print_packet(M_BLK_ID pMblk)
{
    unsigned char *p;

    pMblk->mBlkHdr.mFlags &=(~(M_BCAST | M_MCAST));

    printf("pklen=%d mLen=%d mType=%d mNext=%08x mData=%08x\n",
        pMblk->mBlkPktHdr.len,
        pMblk->mBlkHdr.mLen,
        pMblk->mBlkHdr.mType,
        (int)pMblk->mBlkHdr.mNext,
        (int)pMblk->mBlkHdr.mData);

    p = (unsigned char *)pMblk->mBlkHdr.mData;
    et_drv_txrx_pp(p, pMblk->mBlkPktHdr.len);
}

UINT8 et_drv_tx_test_pkbuf[512] = {
 0x00 ,0x01 ,0x02 ,0x03 ,0x04 ,0x05 ,0x06 ,0x07 
,0x08 ,0x09 ,0x0a ,0x0b ,0x0c ,0x0d ,0x0e ,0x0f 
,0x10 ,0x11 ,0x12 ,0x13 ,0x14 ,0x15 ,0x16 ,0x17 
,0x18 ,0x19 ,0x1a ,0x1b ,0x1c ,0x1d ,0x1e ,0x1f 
,0x20 ,0x21 ,0x22 ,0x23 ,0x24 ,0x25 ,0x26 ,0x27 
,0x28 ,0x29 ,0x2a ,0x2b ,0x2c ,0x2d ,0x2e ,0x2f 
,0x30 ,0x31 ,0x32 ,0x33 ,0x34 ,0x35 ,0x36 ,0x37 
,0x38 ,0x39 ,0x3a ,0x3b ,0x3c ,0x3d ,0x3e ,0x3f 
,0x40 ,0x41 ,0x42 ,0x43 ,0x44 ,0x45 ,0x46 ,0x47 
,0x48 ,0x49 ,0x4a ,0x4b ,0x4c ,0x4d ,0x4e ,0x4f 
,0x50 ,0x51 ,0x52 ,0x53 ,0x54 ,0x55 ,0x56 ,0x57 
,0x58 ,0x59 ,0x5a ,0x5b ,0x5c ,0x5d ,0x5e ,0x5f 
,0x60 ,0x61 ,0x62 ,0x63 ,0x64 ,0x65 ,0x66 ,0x67 
,0x68 ,0x69 ,0x6a ,0x6b ,0x6c ,0x6d ,0x6e ,0x6f 
,0x70 ,0x71 ,0x72 ,0x73 ,0x74 ,0x75 ,0x76 ,0x77 
,0x78 ,0x79 ,0x7a ,0x7b ,0x7c ,0x7d ,0x7e ,0x7f 
,0x80 ,0x81 ,0x82 ,0x83 ,0x84 ,0x85 ,0x86 ,0x87 
,0x88 ,0x89 ,0x8a ,0x8b ,0x8c ,0x8d ,0x8e ,0x8f 
,0x90 ,0x91 ,0x92 ,0x93 ,0x94 ,0x95 ,0x96 ,0x97 
,0x98 ,0x99 ,0x9a ,0x9b ,0x9c ,0x9d ,0x9e ,0x9f 
,0xa0 ,0xa1 ,0xa2 ,0xa3 ,0xa4 ,0xa5 ,0xa6 ,0xa7 
,0xa8 ,0xa9 ,0xaa ,0xab ,0xac ,0xad ,0xae ,0xaf 
,0xb0 ,0xb1 ,0xb2 ,0xb3 ,0xb4 ,0xb5 ,0xb6 ,0xb7 
,0xb8 ,0xb9 ,0xba ,0xbb ,0xbc ,0xbd ,0xbe ,0xbf 
,0xc0 ,0xc1 ,0xc2 ,0xc3 ,0xc4 ,0xc5 ,0xc6 ,0xc7 
,0xc8 ,0xc9 ,0xca ,0xcb ,0xcc ,0xcd ,0xce ,0xcf 
,0xd0 ,0xd1 ,0xd2 ,0xd3 ,0xd4 ,0xd5 ,0xd6 ,0xd7 
,0xd8 ,0xd9 ,0xda ,0xdb ,0xdc ,0xdd ,0xde ,0xdf 
,0xe0 ,0xe1 ,0xe2 ,0xe3 ,0xe4 ,0xe5 ,0xe6 ,0xe7 
,0xe8 ,0xe9 ,0xea ,0xeb ,0xec ,0xed ,0xee ,0xef 
,0xf0 ,0xf1 ,0xf2 ,0xf3 ,0xf4 ,0xf5 ,0xf6 ,0xf7 
,0xf8 ,0xf9 ,0xfa ,0xfb ,0xfc ,0xfd ,0xfe ,0xff 
,0x00 ,0x01 ,0x02 ,0x03 ,0x04 ,0x05 ,0x06 ,0x07 
,0x08 ,0x09 ,0x0a ,0x0b ,0x0c ,0x0d ,0x0e ,0x0f 
,0x10 ,0x11 ,0x12 ,0x13 ,0x14 ,0x15 ,0x16 ,0x17 
,0x18 ,0x19 ,0x1a ,0x1b ,0x1c ,0x1d ,0x1e ,0x1f 
,0x20 ,0x21 ,0x22 ,0x23 ,0x24 ,0x25 ,0x26 ,0x27 
,0x28 ,0x29 ,0x2a ,0x2b ,0x2c ,0x2d ,0x2e ,0x2f 
,0x30 ,0x31 ,0x32 ,0x33 ,0x34 ,0x35 ,0x36 ,0x37 
,0x38 ,0x39 ,0x3a ,0x3b ,0x3c ,0x3d ,0x3e ,0x3f 
,0x40 ,0x41 ,0x42 ,0x43 ,0x44 ,0x45 ,0x46 ,0x47 
,0x48 ,0x49 ,0x4a ,0x4b ,0x4c ,0x4d ,0x4e ,0x4f 
,0x50 ,0x51 ,0x52 ,0x53 ,0x54 ,0x55 ,0x56 ,0x57 
,0x58 ,0x59 ,0x5a ,0x5b ,0x5c ,0x5d ,0x5e ,0x5f 
,0x60 ,0x61 ,0x62 ,0x63 ,0x64 ,0x65 ,0x66 ,0x67 
,0x68 ,0x69 ,0x6a ,0x6b ,0x6c ,0x6d ,0x6e ,0x6f 
,0x70 ,0x71 ,0x72 ,0x73 ,0x74 ,0x75 ,0x76 ,0x77 
,0x78 ,0x79 ,0x7a ,0x7b ,0x7c ,0x7d ,0x7e ,0x7f 
,0x80 ,0x81 ,0x82 ,0x83 ,0x84 ,0x85 ,0x86 ,0x87 
,0x88 ,0x89 ,0x8a ,0x8b ,0x8c ,0x8d ,0x8e ,0x8f 
,0x90 ,0x91 ,0x92 ,0x93 ,0x94 ,0x95 ,0x96 ,0x97 
,0x98 ,0x99 ,0x9a ,0x9b ,0x9c ,0x9d ,0x9e ,0x9f 
,0xa0 ,0xa1 ,0xa2 ,0xa3 ,0xa4 ,0xa5 ,0xa6 ,0xa7 
,0xa8 ,0xa9 ,0xaa ,0xab ,0xac ,0xad ,0xae ,0xaf 
,0xb0 ,0xb1 ,0xb2 ,0xb3 ,0xb4 ,0xb5 ,0xb6 ,0xb7 
,0xb8 ,0xb9 ,0xba ,0xbb ,0xbc ,0xbd ,0xbe ,0xbf 
,0xc0 ,0xc1 ,0xc2 ,0xc3 ,0xc4 ,0xc5 ,0xc6 ,0xc7 
,0xc8 ,0xc9 ,0xca ,0xcb ,0xcc ,0xcd ,0xce ,0xcf 
,0xd0 ,0xd1 ,0xd2 ,0xd3 ,0xd4 ,0xd5 ,0xd6 ,0xd7 
,0xd8 ,0xd9 ,0xda ,0xdb ,0xdc ,0xdd ,0xde ,0xdf 
,0xe0 ,0xe1 ,0xe2 ,0xe3 ,0xe4 ,0xe5 ,0xe6 ,0xe7 
,0xe8 ,0xe9 ,0xea ,0xeb ,0xec ,0xed ,0xee ,0xef 
,0xf0 ,0xf1 ,0xf2 ,0xf3 ,0xf4 ,0xf5 ,0xf6 ,0xf7 
,0xf8 ,0xf9 ,0xfa ,0xfb ,0xfc ,0xfd ,0xfe ,0xff
};
#endif

/* Netpool create */
LOCAL int
et_drvTxRxNetpoolCreate(int unit)
{
    et_drv_txrx_info[unit].et_drvNetPoolID =  (NET_POOL_ID)calloc(1, sizeof(NET_POOL));

    if (et_drv_txrx_info[unit].et_drvNetPoolID == NULL) {
        return(0);
    }

    et_drv_txrx_info[unit].m_cl_blks.mBlkNum = ET_DRV_TXRX_MBLKS;
    et_drv_txrx_info[unit].m_cl_blks.clBlkNum = ET_DRV_TXRX_CLBLKS;
    et_drv_txrx_info[unit].m_cl_blks.memSize =
            (et_drv_txrx_info[unit].m_cl_blks.mBlkNum * (M_BLK_SZ + sizeof(long))) +
            (et_drv_txrx_info[unit].m_cl_blks.clBlkNum * (CL_BLK_SZ + sizeof(long)));
    et_drv_txrx_info[unit].m_cl_blks.memArea = 
            (char *)memalign (sizeof (long), et_drv_txrx_info[unit].m_cl_blks.memSize);
    if (et_drv_txrx_info[unit].m_cl_blks.memArea == NULL) {
        free(et_drv_txrx_info[unit].et_drvNetPoolID);
        et_drv_txrx_info[unit].et_drvNetPoolID = NULL;
        return(0);
    }

    et_drv_txrx_info[unit].cluster.clNum = ET_DRV_TXRX_CLBUFS;
    et_drv_txrx_info[unit].cluster.clSize = ET_DRV_TX_RX_PK_SZ;
    et_drv_txrx_info[unit].cluster.memSize = et_drv_txrx_info[unit].cluster.clNum *
            (et_drv_txrx_info[unit].cluster.clSize + sizeof(long)) + sizeof(long);
    et_drv_txrx_info[unit].cluster.memArea = cacheDmaMalloc(
                                    et_drv_txrx_info[unit].cluster.memSize);
    if (et_drv_txrx_info[unit].cluster.memArea == NULL) {
        free(et_drv_txrx_info[unit].et_drvNetPoolID);
        et_drv_txrx_info[unit].et_drvNetPoolID = NULL;
        free(et_drv_txrx_info[unit].m_cl_blks.memArea);
        et_drv_txrx_info[unit].m_cl_blks.memArea = NULL;
        return(0);
    }

    if (netPoolInit(et_drv_txrx_info[unit].et_drvNetPoolID,
                &et_drv_txrx_info[unit].m_cl_blks,
                &et_drv_txrx_info[unit].cluster, 1, NULL) != OK) {

        free(et_drv_txrx_info[unit].et_drvNetPoolID);
        et_drv_txrx_info[unit].et_drvNetPoolID = NULL;
        free(et_drv_txrx_info[unit].m_cl_blks.memArea);
        et_drv_txrx_info[unit].m_cl_blks.memArea = NULL;
        free(et_drv_txrx_info[unit].cluster.memArea);
        et_drv_txrx_info[unit].cluster.memArea = NULL;
        return(0);
    }
    et_drv_txrx_info[unit].clpool_id = netClPoolIdGet(et_drv_txrx_info[unit].et_drvNetPoolID,
                                et_drv_txrx_info[unit].cluster.clSize, FALSE);
    return(1);
}

LOCAL int
_et_drvTx(int unit, M_BLK_ID pMblk)
{
#ifdef ET_DRV_TXRX_DEBUG
    if (et_drv_txrx_debug & ET_DRV_TXRX_DEBUG_F) {
        logMsg("Sending a packet %08x\n", (int)pMblk, 2, 3, 4, 5, 6);
        et_drv_txrx_print_packet(pMblk);
    }
#endif

    if (muxSend( et_drv_txrx_info[unit].et_drvMuxBindID, pMblk) == ERROR)
    {
        logMsg("muxTkSend failed \n", 1, 2, 3, 4, 5, 6);
        netMblkClChainFree(pMblk);
        et_drv_txrx_info[unit].tx_fail++;
        return(-1);
    }
    et_drv_txrx_info[unit].tx_cnt++;
    return(0);
}

LOCAL BOOL
et_drvRecv(void *pCookie, long type, M_BLK_ID pMblk, 
           LL_HDR_INFO * pLinkHdrInfo, void *pSpareData)
{
    int   rv = -1, unit;
    UINT8 *buf;

    for (unit = 0 ; unit < ETH_MAX_UNITS; unit++) {
        if ((void *)et_drv_txrx_info[unit].et_drvMuxBindID == pCookie) {
            break;
        }
    }

    if (unit == ETH_MAX_UNITS) {
        return FALSE;
    }

    et_drv_txrx_info[unit].rx_cnt++;

#ifdef ET_DRV_TXRX_DEBUG
    if (et_drv_txrx_debug & ET_DRV_TXRX_DEBUG_F) {
        et_drv_txrx_print_packet(pMblk);
    }
#endif
    if (et_drv_txrx_info[unit].rx_cb != NULL) {
        if (et_drv_txrx_info[unit].rx_alloc != NULL) {
            buf = et_drv_txrx_info[unit].rx_alloc(pMblk->mBlkPktHdr.len);
            if (buf != NULL) { 
                netMblkToBufCopy(pMblk,(char *)buf, NULL);
                rv = et_drv_txrx_info[unit].rx_cb(unit, buf, 
                                                  pMblk->mBlkPktHdr.len,
                                                  et_drv_txrx_info[unit].cookie);
            } else {
                et_drv_txrx_info[unit].rx_out_of_mem++;
            }
        } else {
            rv = et_drv_txrx_info[unit].rx_cb(unit,
                                              pMblk->mBlkHdr.mData, 
                                              pMblk->mBlkPktHdr.len,
                                              et_drv_txrx_info[unit].cookie);
        }
    }
    netMblkClChainFree(pMblk);

    return (rv == 0)? TRUE : FALSE;
}

#if 0
LOCAL BOOL
et_drvTxShutdownRtn(void *netCallbackId)
{
    return (1);
}

LOCAL BOOL
et_drvTxRestartRtn(void *netCallbackId)
{
    return (1);
}

LOCAL void
et_drvErrorRtn(void *netCallbackId, END_ERR * pError)
{
    return;
}
#endif

/* Publicly exported functions */

int et_drvTxRxInit(char * dev, int unit)
{
    if (et_drv_txrx_info[unit].et_drvMuxBindID != NULL) {
        return -1;
    }

    et_drv_txrx_info[unit].queue_rx_pkts = 0;

    et_drvTxRxNetpoolCreate(unit);

    /* Bind et_drvTxRx Network service to the END driver. */
    et_drv_txrx_info[unit].et_drvMuxBindID = (void *) muxBind(
                        dev, unit,
                        et_drvRecv, NULL,
                        NULL, NULL,
                        MUX_PROTO_SNARF, "ET_DRV TX/RX", NULL);

    if (!et_drv_txrx_info[unit].et_drvMuxBindID)
    {
        logMsg("muxTkBind Failed (%08x).\n", errnoGet(), 2, 3, 4, 5, 6);
        return -1;
    }

    /* Promiscuous mode */
    muxIoctl( et_drv_txrx_info[unit].et_drvMuxBindID, EIOCSFLAGS, (void *)IFF_PROMISC);

    return 0;
}

int 
et_drv_mac_addr_get(int unit, unsigned char *mac)
{
    return muxIoctl( et_drv_txrx_info[unit].et_drvMuxBindID, EIOCGADDR, (void *)mac);
}

int
et_drvTx(int unit, UINT8 *b, int l, void *cookie)
{
    M_BLK_ID pMblk;

    pMblk = netTupleGet(et_drv_txrx_info[unit].et_drvNetPoolID, l,
                        M_DONTWAIT, MT_DATA,FALSE);
    if (pMblk == NULL) {
        et_drv_txrx_info[unit].tx_fail++;
        return(0);
    }
    /* cache invalidate */
    /* CACHE_INVALIDATE(pMblk->m_data, l); */
    pMblk->mBlkHdr.mFlags  |= M_PKTHDR;
    pMblk->mBlkHdr.mLen     = l;
    pMblk->mBlkPktHdr.len   = l;

    memcpy(pMblk->m_data, b, l);
    return (_et_drvTx(unit, pMblk));
}

int
et_drvMIIRxHandlerRegister(int unit, cb_f cf, alloc_f af, free_f ff, void *cookie)
{
    if (et_drv_txrx_info[unit].rx_cb == NULL) {
        et_drv_txrx_info[unit].rx_alloc = af;
        et_drv_txrx_info[unit].rx_free = ff;
        et_drv_txrx_info[unit].rx_cb = cf;
        et_drv_txrx_info[unit].cookie = cookie;
        return(0);
    } else {
        return(1);
    }
}
int
et_drvMIIRxHandlerUnRegister(int unit, cb_f f)
{
    if (et_drv_txrx_info[unit].rx_cb == f) {
        et_drv_txrx_info[unit].rx_cb = NULL;
        return(0);
    } else {
        return(1);
    }
}

void et_drv_rx_drain_pkts(int unit)
{
    M_BLK_ID pMblk, cMblk;
    int s;

    s = intLock();
    pMblk = et_drv_txrx_info[unit].rx_head;
    et_drv_txrx_info[unit].rx_tail = et_drv_txrx_info[unit].rx_head = NULL;
    while(pMblk) {
        cMblk = pMblk;
        netMblkClChainFree(cMblk);
        pMblk = pMblk->mBlkHdr.mNextPkt;
    }
    intUnlock(s);
}

void et_drv_rx_q_pkts(int unit, M_BLK_ID pMblk, int dummy)
{
    M_BLK_ID qMblk;
    M_BLK_ID newMblk, newId;
    int s;

    newMblk = netMblkGet(et_drv_txrx_info[unit].et_drvNetPoolID,
                         M_DONTWAIT, MT_DATA);
    if (newMblk == NULL) {
        printf("et_drv_rx_q_pkts : Failed to allocate mbuf\n");
        et_drv_txrx_info[unit].tx_fail++;
        return;
    }
    newId = netMblkDup(pMblk, newMblk);
    if (newId == NULL) {
        printf("dup failed \n");
        return;
    }

    s = intLock();
    if (et_drv_txrx_info[unit].rx_tail == NULL ) {
        et_drv_txrx_info[unit].rx_tail = newId;
        et_drv_txrx_info[unit].rx_head = newId;
    } else {
        qMblk = et_drv_txrx_info[unit].rx_tail;
        qMblk->mBlkHdr.mNextPkt = newId;
        et_drv_txrx_info[unit].rx_tail = newId;
        newId->mBlkHdr.mNextPkt = NULL;
    }
    intUnlock(s);
    
    ET_DRV_TXRX_PRINT("Queued a packet %08x\n", (int)pMblk, 2, 3, 4, 5, 6);

    /* Indicate that a packet is available */
    /* semTake(et_drv_txrx_info.rx_sem, WAIT_FOREVER); */
}

void et_drv_mii_rx(int unit, UINT8 *b, int *l)
{
    M_BLK_ID pMblk;
    int s;
    int n = 10;

    *l = 0;

    while(*l == 0) {
        pMblk = et_drv_txrx_info[unit].rx_head;
        if (pMblk != NULL) {
            s = intLock();
            et_drv_txrx_info[unit].rx_head = pMblk->mBlkHdr.mNextPkt;
            if (et_drv_txrx_info[unit].rx_head == NULL) {
                et_drv_txrx_info[unit].rx_tail = NULL;
            }
            intUnlock(s);

            *l = pMblk->mBlkPktHdr.len;
            netMblkToBufCopy(pMblk, (char *)b, NULL);
            netMblkClChainFree(pMblk);
            ET_DRV_TXRX_PRINT("DeQueued a packet %08x len = %d\n",
                            (int)pMblk, *l, 3, 4, 5, 6);
        } else {
            /* Wait for a packet */
            /* semTake(et_drv_txrx_info.rx_sem); */
            taskDelay(1);
            if (!(n--)) break;
        }
    }
}


void et_drv_start(int unit)
{
    ET_DRV_TXRX_PRINT("et_drv_start\n", 1, 2, 3, 4, 5, 6);
    et_drv_txrx_info[unit].queue_rx_pkts = 1;
    et_drvMIIRxHandlerRegister(unit, (cb_f) et_drv_rx_q_pkts, NULL, NULL, NULL);
}

void et_drv_stop(int unit)
{
    ET_DRV_TXRX_PRINT("et_drv_stop\n", 1, 2, 3, 4, 5, 6);
    if (et_drvMIIRxHandlerUnRegister(unit, (cb_f) et_drv_rx_q_pkts)) {
        et_drv_txrx_info[unit].queue_rx_pkts = 0;
        et_drv_rx_drain_pkts(unit);
    }
}


#ifdef ET_DRV_TXRX_DEBUG
/* debug */

LOCAL void
et_drv_rx_test_cb(char *buf, int len)
{
    logMsg("et_drv_rx_test_cb:",1,2,3,4,5,6);
    et_drv_txrx_pp((UINT8 *)buf, len);
    free(buf);
}

extern void netPoolShow(NET_POOL_ID);
void
et_drv_tx_netpool_show(int unit)
{
    netPoolShow(et_drv_txrx_info[unit].et_drvNetPoolID);
}

void
et_drv_rx_test(int unit)
{
    et_drvMIIRxHandlerRegister(unit, (cb_f) et_drv_rx_test_cb, (alloc_f)malloc, free, NULL);
}

void
et_drv_tx_test(int unit, int l)
{
    if (l > 512) {
        l = 64;
    }
    if (!et_drvTx(unit, et_drv_tx_test_pkbuf, l, NULL)) {
        logMsg("et_drv TX failed",1,2,3,4,5,6);
    }
}
#endif

#endif /* VXWORKS && VX_VERSION > 54 */

int mux_drv_not_empty;

