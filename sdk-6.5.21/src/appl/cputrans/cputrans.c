/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cputrans.c
 * Purpose:     TX and RX packet management routines
 * Requires:    bcm_trans_ptr_t structure
 */

#include <shared/bsl.h>

#include <appl/cputrans/cputrans.h>

#include <sal/core/sync.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

/* Currently used for sal_dma_alloc */
#include <sal/appl/sal.h>

#include <bcm/error.h>
#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm_int/control.h>

#include "t_util.h"

/* MUTEX used by both RX and TX allocation routines */

static sal_mutex_t              ct_lock;
#define CPUTRANS_LOCK           sal_mutex_take(ct_lock, sal_sem_FOREVER)
#define CPUTRANS_UNLOCK         sal_mutex_give(ct_lock)

/****************************************************************
 *
 * TX packet setup and allocation
 *
 * There is one free list of TX packets.  Each TX packet has two
 * data pointers allocated and one header buffer allocated.  If the
 * header buffer is not used, it is placed in the second pointer
 * block and the number of blocks for the packet (blk_count) is
 * set to 1.
 *
 *
 * Static variables:
 *     _tx_trans_ptr        Local transport driver list
 *     _tx_setup_done       Boolean; packets allocated?
 *     _tx_pkt_free_list    Head of TX packet free list
 *     _tx_pkts             Allocation pointer for all TX packets
 *     _tx_pkt_blks         Allocation pointer for all TX data pointer blocks
 *     _tx_header_data      Allocation pointer for all header data
 */

static bcm_trans_ptr_t          _tx_trans_ptr;
#define INIT_DONE               (ct_lock != NULL)
#define CHECK_INIT if (!INIT_DONE) BCM_IF_ERROR_RETURN(_do_init())

static int                      _tx_setup_done;
static bcm_pkt_t               *_tx_pkt_free_list;
static bcm_pkt_t               *_tx_pkts;
static bcm_pkt_blk_t           *_tx_pkt_blks;
static uint8                   *_tx_header_data;

/* This indicates an error which may cause internal corruption has been seen */
volatile int cputrans_error_count;
#define CPUTRANS_ERROR_INCR (++cputrans_error_count)

/* Do local initalization */
STATIC int
_do_init(void)
{
    ct_lock = sal_mutex_create("ct_tx_lock");
    if (ct_lock == NULL) {
        return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}


/* Group allocate and free macros */

#define _ALLOC_CHECK(id, bytes, _cleanup, _str)         \
    do {                                                \
        (id) = sal_alloc((bytes), (_str));              \
        if ((id) == NULL) {                             \
            (_cleanup)();                               \
            CPUTRANS_UNLOCK;                            \
            return BCM_E_MEMORY;                        \
        }                                               \
        sal_memset((id), 0, bytes);                     \
    } while (0)

#define _FREE_CHECK(id)                                 \
    do {                                                \
        if ((id) != NULL) {                             \
            sal_free(id);                               \
            (id) = NULL;                                \
        }                                               \
    } while (0)

/*
 * Function:
 *      cputrans_tx_setup_done
 * Purpose:
 *      Indicate if setup has been done for cputrans subsys
 * Returns:
 *      Boolean:  True if setup has been done
 */

int
cputrans_tx_setup_done(void)
{
    return _tx_setup_done;
}

#define _TX_RESERVE_MAX   BCM_COS_MAX
static int tx_reserve[_TX_RESERVE_MAX + 1] = CPUTRANS_TX_RESERVE_DEFAULT;
static int tx_pkts_avail;
static int tx_pool_size;

/*
 * Function:
 *      cputrans_tx_pkt_setup
 * Purpose:
 *      Setup the CPU transport tx packet pool
 * Parameters:
 *      pkt_pool_size    - Number of packets to use in pool;
 *      trans_ptr        - Pointer to transport driver (for salloc/sfree).
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If pkt_pool_size <= 0, use default value.
 */

int
cputrans_tx_pkt_setup(int pkt_pool_size, bcm_trans_ptr_t *trans_ptr)
{
    int bytes;
    int i;
    bcm_pkt_t *pkt;

    CHECK_INIT;
    if (_tx_setup_done) {
        return BCM_E_BUSY;
    }

    if (trans_ptr->tp_data_alloc == NULL ||
                  trans_ptr->tp_data_free == NULL) {
        return BCM_E_PARAM;
    }
    sal_memcpy(&_tx_trans_ptr, trans_ptr, sizeof(bcm_trans_ptr_t));

    if (pkt_pool_size <= 0) {
        pkt_pool_size = CPUTRANS_PKT_POOL_SIZE_DEFAULT;
    }
    tx_pkts_avail = pkt_pool_size;
    tx_pool_size = pkt_pool_size;

    /* Allocate static data for TX queues */
    bytes = pkt_pool_size * CPUTRANS_HEADER_BYTES;
    _tx_header_data = sal_dma_alloc(bytes, "CT_HDR");
    if (_tx_header_data == NULL) {
        cputrans_tx_pkt_cleanup();
        return BCM_E_MEMORY;
    }

    /* Allocate tx pkts then data block ptrs, two per packet */
    bytes = pkt_pool_size * sizeof(bcm_pkt_t);
    _ALLOC_CHECK(_tx_pkts, bytes, cputrans_tx_pkt_cleanup, "ct_tx_pkts");

    bytes = pkt_pool_size * 2 * sizeof(bcm_pkt_blk_t);
    _ALLOC_CHECK(_tx_pkt_blks, bytes, cputrans_tx_pkt_cleanup, "ct_pkt_blks");

    /* Give each tx packet a header buffer and link to freelist */
    for (i = 0; i < pkt_pool_size; i++) {
        _tx_pkts[i].next = &_tx_pkts[i + 1];
        pkt = &_tx_pkts[i];
        pkt->pkt_data = &_tx_pkt_blks[2 * i];
        pkt->pkt_data[0].data = &_tx_header_data[i * CPUTRANS_HEADER_BYTES];
        pkt->pkt_data[0].len = CPUTRANS_HEADER_BYTES;
        pkt->flags = BCM_TX_CRC_APPEND;
    }
    _tx_pkts[i - 1].next = NULL;
    _tx_pkt_free_list = _tx_pkts;

    _tx_setup_done = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      cputrans_tx_pkt_cleanup
 * Purpose:
 *      Cleanup and deallocate static packet data for CPU transport
 */

void
cputrans_tx_pkt_cleanup(void)
{
    if (!INIT_DONE) {
        return;
    }

    CPUTRANS_LOCK;

    if (_tx_header_data != NULL) {
        sal_dma_free(_tx_header_data);
        _tx_header_data = NULL;
    }

    _FREE_CHECK(_tx_pkts);
    _FREE_CHECK(_tx_pkt_blks);
    _tx_setup_done = FALSE;

    CPUTRANS_UNLOCK;
}

/* Grab (extra_pkt_count + 1) packets from free list; return NULL if fails */
static INLINE bcm_pkt_t *
grab_packets(int extra_pkt_count, int cos, uint32 ct_flags)
{
    int _i;
    bcm_pkt_t *_pkt;
    bcm_pkt_t *first_pkt;

    if (cos > _TX_RESERVE_MAX) {
        return NULL;
    }

    CPUTRANS_LOCK;
    if (_tx_pkt_free_list == NULL) {
        CPUTRANS_UNLOCK;
        return NULL;
    }
    if (cos >= 0) {
        if (tx_pkts_avail - (extra_pkt_count + 1) < tx_reserve[cos]) {
            CPUTRANS_UNLOCK;
            return NULL;
        }
    }
    _pkt = first_pkt = _tx_pkt_free_list;
    _pkt->flags = BCM_TX_CRC_APPEND;
    for (_i = 0; _i < extra_pkt_count; _i++) {
        if (_pkt->next == NULL) {
            CPUTRANS_UNLOCK;
            return NULL;
        }
        _pkt = _pkt->next;
        _pkt->flags = BCM_TX_CRC_APPEND;
    }
    /* pkt now points to last pkt in alloc chain. */
    _tx_pkt_free_list = _pkt->next;
    _pkt->next = NULL;
    first_pkt->_last_pkt = _pkt;
    tx_pkts_avail -= (extra_pkt_count + 1);

    if (ct_flags & CPUTRANS_CRC_REGEN) {
        _pkt->flags = BCM_TX_CRC_REGEN;
    } /* else, leave as append */

    CPUTRANS_UNLOCK;

    return first_pkt;
}

/* See notes for cputrans_tx_alloc_empty_set below */

/*
 * Function:
 *      cputrans_tx_alloc_limit_set/get
 * Purpose:
 *      Set the level at which the tx pkt alloc pool appears empty to a COS
 * Parameters:
 *      cos         - The COS whose low marker should be set
 *      reserve     - Keep this many pkts available in pool; see Notes.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When an allocation request is made and the COS override flag
 *      is set in the ct_flags, the COS is extracted from the flags
 *      and a "pkt reserve" is checked.  This represents the number of
 *      packets that must remain in the queue after the allocation.  If
 *      not enough packets remain, the allocation will fail and return NULL.
 *
 *      Normally, reserve is set to 0 for the highest COS.  Allocations
 *      can be disabled for a COS by setting reserve >= pkt_pool_size
 *      (though this is not recommended).
 */

int
cputrans_tx_alloc_limit_set(int cos, int reserve)
{
    if (cos < 0 || cos > _TX_RESERVE_MAX) {
        return BCM_E_PARAM;
    }

    tx_reserve[cos] = reserve;

    return BCM_E_NONE;
}

int
cputrans_tx_alloc_limit_get(int cos, int *reserve)
{
    if (cos < 0 || cos > _TX_RESERVE_MAX) {
        return BCM_E_PARAM;
    }

    *reserve = tx_reserve[cos];

    return BCM_E_NONE;
}

/*
 * Function:
 *      cputrans_tx_pkt_alloc
 * Purpose:
 *      Allocate a packet from the CPU trans pool
 * Parameters:
 *      pkt_buf     - Pointer to data to link to packet
 *      len         - Length of packet buffer
 *      ct_flags    - Flags for pkt:  Mainly
 *                    NO_HEADER_ALLOC:  The header space is in pkt_buf
 *                    CPUTRANS_COS_OVERRIDE: cos to check for packet allocation
 * Returns:
 *      Pointer to packet or NULL if failed.
 * Notes:
 *      Whether or not the header is in a separate block is indicated
 *      by the pkt->blk_count value.
 *         blk_count == 1 => Header is at start of pkt_buf in pkt_data[0]
 *         blk_count == 2 => Header is pkt_data[0], pkt_buf in pkt_data[1]
 */

bcm_pkt_t *
cputrans_tx_pkt_alloc(uint8 *pkt_buf, int len, uint32 ct_flags)
{
    bcm_pkt_t *pkt;
    int pkt_blk;
    int cos = -1;

    if (!_tx_setup_done) {
        return NULL;
    }

    if (ct_flags & CPUTRANS_COS_OVERRIDE) {
        cos = CPUTRANS_COS_GET(ct_flags);
    }
    /* Will return NULL if error */
    pkt = grab_packets(0, cos, ct_flags);
    if (pkt == NULL) {
        return NULL;
    }

    if (ct_flags & CPUTRANS_NO_HEADER_ALLOC) {
        /* Save header buffer in data 1 */
        sal_memcpy(&pkt->pkt_data[1], &pkt->pkt_data[0],
                   sizeof(bcm_pkt_blk_t));
        pkt_blk = 0;
        pkt->blk_count = 1;
    } else {  /* Header in first block */
        pkt_blk = 1;
        pkt->blk_count = 2;
    }

    /* Put data pointers in proper place. */
    pkt->pkt_data[pkt_blk].data = pkt_buf;
    pkt->pkt_data[pkt_blk].len = len;
    /* Mark as a single packet, not in a linked list */
    pkt->next = NULL;

    return pkt;
}

/*
 * Function:
 *      cputrans_tx_pkt_list_alloc
 * Purpose:
 *      Allocate a list of packets and segment the packet buffer
 * Parameters:
 *      pkt_buf     - Pointer to data to link to packet
 *      len         - Length of packet buffer
 *      seg_len     - Number of bytes in each segment, not including header
 *      ct_flags    - Flags for pkt:  Mainly
 *                    NO_HEADER_ALLOC:  The header space is in pkt_buf
 *                    CPUTRANS_COS_OVERRIDE: cos to check for packet allocation
 *      num_pkt     - (OUT) How many packets allocated
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The last packet pointer is stored in _last_pkt of the first
 *      packet.  The "next" pointers should not be
 *      altered in the list.  CPUTRANS headers are always provided
 *      for the second, third... packets.  The NO_HEADER_ALLOC flag
 *      only applies to the first buffer.
 *
 *      The MTU for the channel must be at least seg_len + header_bytes.
 *
 *      If NO_HEADER_ALLOC is set, then the length of the first chunk
 *      of data taken from pkt_buf is (seg_len + header_bytes).  This way,
 *      there are always seg_len bytes of payload per packet.
 */

bcm_pkt_t *
cputrans_tx_pkt_list_alloc(uint8 *pkt_buf, int len, int seg_len,
                           uint32 ct_flags, int *num_segs)
{
    bcm_pkt_t *first_pkt;
    bcm_pkt_t *pkt;
    int extra_pkt_count;
    int offset;
    int i;
    int pkt_blk = 0;
    int len_fld_val = 0x601;  /* What goes in the pkt's length fld */
    int cos = -1;

    if (!_tx_setup_done) {
        return NULL;
    }

    extra_pkt_count = (len - 1) / seg_len;   /* Packets needed beyond first */

    if (ct_flags & CPUTRANS_COS_OVERRIDE) {
        cos = CPUTRANS_COS_GET(ct_flags);
    }

    /* Will return NULL if error */
    first_pkt = grab_packets(extra_pkt_count, cos, ct_flags);
    if (first_pkt == NULL) {
        return NULL;
    }

    if (num_segs != NULL) {
        *num_segs = extra_pkt_count + 1;
    }

    offset = seg_len;   /* Where second packet starts in payload */
    if (ct_flags & CPUTRANS_NO_HEADER_ALLOC) {
        /* Save header buffer in data 1 */
        sal_memcpy(&first_pkt->pkt_data[1], &first_pkt->pkt_data[0],
                   sizeof(bcm_pkt_blk_t));
        pkt_blk = 0;
        first_pkt->blk_count = 1;
        offset += CPUTRANS_HEADER_BYTES;
    } else {  /* Header in first block */
        pkt_blk = 1;
        first_pkt->blk_count = 2;
    }

    /* Put data pointers in proper place. */
    first_pkt->pkt_data[pkt_blk].data = pkt_buf;
    first_pkt->call_back = NULL;
    if (offset >= len) {  /* Only one segment needed */
        first_pkt->pkt_data[pkt_blk].len = len;
        PACK_SHORT(&first_pkt->pkt_data[0].data[CPUTRANS_LEN_OFS],
                   len_fld_val);
    } else {              /* Extra packets */
        first_pkt->pkt_data[pkt_blk].len = offset;
        PACK_SHORT(&first_pkt->pkt_data[0].data[CPUTRANS_LEN_OFS],
                   len_fld_val);
        /* Segment the rest of the packet */
        pkt = first_pkt->next;
        for (i = 0; i < extra_pkt_count - 1; i++) { /* Not first or last */
            pkt->pkt_data[1].data = &pkt_buf[offset];
            pkt->pkt_data[1].len = seg_len;
            PACK_SHORT(&first_pkt->pkt_data[0].data[CPUTRANS_LEN_OFS],
                       len_fld_val);
            pkt->blk_count = 2;
            offset += seg_len;
            pkt = pkt->next;
            pkt->call_back = NULL;
        }

        /* Take care of last pkt (pointed to by pkt) */
        pkt->pkt_data[1].data = &pkt_buf[offset];
        pkt->pkt_data[1].len = len - offset;
        PACK_SHORT(&first_pkt->pkt_data[0].data[CPUTRANS_LEN_OFS],
                   len_fld_val);
        pkt->blk_count = 2;
        pkt->call_back = NULL;
    }
    first_pkt->alloc_ptr = pkt_buf;
    if (first_pkt->next == first_pkt) {
        LOG_INFO(BSL_LS_TKS_CTPKT,
                 (BSL_META("CT pkt list alloc: Internal corruption in pointers\n")));
        CPUTRANS_ERROR_INCR;
    }

    return first_pkt;
}


/*
 * Function:
 *      cputrans_tx_pkt_list_free
 * Purpose:
 *      Free a list of packets allocated by cputrans_tx_pkt_list_alloc
 * Parameters:
 *      pkt         - Pointer to start of list
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Will work for a single packet as well, as long as _last_pkt
 *      points to self.
 */

void
cputrans_tx_pkt_list_free(bcm_pkt_t *pkt)
{
    bcm_pkt_t *last_pkt, *t_pkt;
    int count = 0;

    if (pkt == NULL) {
        LOG_INFO(BSL_LS_TKS_CTPKT,
                 (BSL_META("CT pkt list free: Packet NULL\n")));
        return;
    }
    if (pkt->next == pkt) {
        LOG_INFO(BSL_LS_TKS_CTPKT,
                 (BSL_META("CT pkt list free: Circular list\n")));
        pkt->next = NULL;
        pkt->_last_pkt = pkt;
        CPUTRANS_ERROR_INCR;
    }
    if (pkt->_last_pkt == NULL) {
        LOG_INFO(BSL_LS_TKS_CTPKT,
                 (BSL_META("CT pkt list free: No last pkt\n")));
        pkt->_last_pkt = pkt;
        CPUTRANS_ERROR_INCR;
    }
    /* Check if header is saved in pkt block 1 */
    if (pkt->blk_count == 1) {
        sal_memcpy(&pkt->pkt_data[0], &pkt->pkt_data[1],
                   sizeof(bcm_pkt_blk_t));
    }

    for (t_pkt = pkt; t_pkt != NULL; t_pkt = t_pkt->next) {
        if (count++ > tx_pool_size) {
            LOG_INFO(BSL_LS_TKS_CTPKT,
                     (BSL_META("CT pkt list free: List too long\n")));
            CPUTRANS_ERROR_INCR;
            return;
        }
    }

    last_pkt = pkt->_last_pkt;
    CPUTRANS_LOCK;
    last_pkt->next = _tx_pkt_free_list;
    _tx_pkt_free_list = pkt;
    tx_pkts_avail += count;
    CPUTRANS_UNLOCK;
}


/****************************************************************
 *
 * RX Packet Setup and Allocation
 *
 * RX packets are set up to hold multiple packet blocks for
 * reassembly.  RX packets are not given data pointers; it
 * is expected that the data will be gotten from the RX call
 * or allocated by the application using these packets.
 *
 * On setup, multiple packets are allocated with block numbers
 * at powers of two.  When rx_pkt_alloc is called, the smallest block
 * number large enough to satisfy the allocation and with free packets
 * is used.
 *
 * CPUTRANS uses pkt->cookie2 for RX packets to store the index
 * used, so pkt->cookie2 must NOT be used above this layer.
 *
 * CPUTRANS RX does not require a transport pointer; it only
 * uses sal_alloc and sal_free.
 *
 * CPUTRANS RX packets have an extra pointer allocate at the
 * beginning of the first block's packet buffer.  This is used
 * by ATP/BET and should not be used by applications.
 *
 * Static variables:
 *     _rx_setup_done       Boolean; packets allocated?
 *     _rx_lists            How many categories of packets set up
 *     _rx_free_lists       Pointer to array of free list head ptrs
 *     _rx_alloc_pkts       Allocation pointer for all RX packets
 *     _rx_pkt_blks         Allocation pointer for all data pointer blocks
 */

static int                      _rx_setup_done;

static int                      _rx_lists;
static bcm_pkt_t              **_rx_free_lists;
static bcm_pkt_t               *_rx_alloc_pkts;
static bcm_pkt_blk_t           *_rx_pkt_blks;

/*
 * Function:
 *      cputrans_rx_setup_done
 * Purpose:
 *      Indicate if setup has been done for RX part of cputrans subsys
 * Returns:
 *      Boolean:  True if setup has been done
 */

int
cputrans_rx_setup_done(void)
{
    return _rx_setup_done;
}


/*
 * Function:
 *      cputrans_rx_pkt_setup
 * Purpose:
 *      Pre-allocate and set up RX packets
 * Parameters:
 *      list_count   - The number of lists to provide; if < 0, uses
 *                     default values.
 *      blk_cnts     - Pointer to an array with list_count entries. See notes.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Each entry is the number of packets to allocate with 2^n
 *      packet block pointers.  The number of blocks needed by a long
 *      packet is about 2 * (pkt_len/seg_len + 1).  With seg_len = 1400
 *      and pkt_len = 64K, this is about 94 blocks, so list count should
 *      generally be <= 8.
 *
 *      Currently, if a packet is requested with too many blocks,
 *      it fails (no on-the-fly allocation.)
 */

int
cputrans_rx_pkt_setup(int list_count, int *blk_cnts)
{
    int tot_pkts = 0;   /* How many packets to allocate, total */
    int tot_blks = 0;   /* How many pkt_blk structs to allocate, total */
    int bytes;
    int pkt_cnt = 0;    /* How many pkts filled in so far */
    bcm_pkt_t *pkt;
    int blk_offset = 0; /* How many blocks used up so far */
    int i, j;
    int blks;
    static int deflt_blk_cnts[] = CPUTRANS_RX_BLK_CNTS_DEFAULT;
    int deflt_count = CPUTRANS_RX_LIST_COUNT_DEFAULT;

    if (_rx_setup_done) {
        return BCM_E_BUSY;
    }

    if (list_count <= 0 || blk_cnts == NULL) {
        list_count = deflt_count;
        blk_cnts = deflt_blk_cnts;
    }
    _rx_lists = list_count;

    for (i = 0; i < list_count; i++) {
        tot_pkts += blk_cnts[i];
        tot_blks += blk_cnts[i] * (1 << i);
    }

    bytes = tot_pkts * sizeof(bcm_pkt_t);
    _ALLOC_CHECK(_rx_alloc_pkts, bytes, cputrans_rx_pkt_cleanup, "CT_RX");

    bytes = list_count * sizeof(bcm_pkt_t *);
    _ALLOC_CHECK(_rx_free_lists, bytes, cputrans_rx_pkt_cleanup, "CT_RX");

    bytes = tot_blks * sizeof(bcm_pkt_blk_t);
    _ALLOC_CHECK(_rx_pkt_blks, bytes, cputrans_rx_pkt_cleanup, "CT_RX");

    /* Set up free lists and packet block pointers from the allocated data */
    pkt = &_rx_alloc_pkts[0];   /* Keep compiler happy */
    for (i = 0; i < list_count; i++) {
        blks = 1 << i;  /* Number of blocks this round */
        if (blk_cnts[i] > 0) {
            _rx_free_lists[i] = &_rx_alloc_pkts[pkt_cnt];
            for (j = 0; j < blk_cnts[i]; j++) {
                pkt = &_rx_alloc_pkts[pkt_cnt + j];
                pkt->blk_count = blks;
                pkt->pkt_data = &_rx_pkt_blks[blk_offset];
                pkt->cookie2 = INT_TO_PTR(i);  /* Record which free list */
                blk_offset += blks;
                pkt->next = &_rx_alloc_pkts[pkt_cnt + j + 1];
            }
            pkt->next = NULL;  /* Terminate list */
            pkt_cnt += blk_cnts[i];
        }
    }

    _rx_setup_done = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      cputrans_rx_pkt_alloc
 * Purpose:
 *      Allocate a packet struct with sufficient RX block pointers.
 * Parameters:
 *      num_blks      - The number of blocks needed in the pkt_blk struct
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Currently, will not allocate beyond the number
 *      set up in cputrans_rx_pkt_setup.  Nor will it allocate if there
 *      are no free buffers available.
 *
 *      Packets allocated with this routine must be freed by a call
 *      to cputrans_rx_pkt_free.
 */

bcm_pkt_t *
cputrans_rx_pkt_alloc(int num_blks)
{
    int i;
    int blks = 1;
    bcm_pkt_t *pkt = NULL;

    if (!_rx_setup_done) {
        return NULL;
    }

    CPUTRANS_LOCK;
    for (i = 0; i < _rx_lists; i++) {
        if (num_blks <= blks && _rx_free_lists[i] != NULL) {
            pkt = _rx_free_lists[i];
            _rx_free_lists[i] = pkt->next;
            break;
        }
        blks <<= 1;
    }
    CPUTRANS_UNLOCK;

    if (pkt != NULL) {
        pkt->blk_count = num_blks;
    }
    return pkt;
}


/*
 * Function:
 *      cputrans_rx_pkt_free
 * Purpose:
 *      Free an RX packet allocated by cputrans_rx_pkt_alloc
 * Parameters:
 *      pkt          - pointer to packet to free
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Should only be called on packets allocated by cputrans_rx_pkt_alloc.
 */

void
cputrans_rx_pkt_free(bcm_pkt_t *pkt)
{
    int idx;

    if (pkt == NULL) {
        LOG_INFO(BSL_LS_TKS_CTPKT,
                 (BSL_META("CT free: Packet NULL\n")));
        return;
    }

    if (!_rx_setup_done) {
        LOG_INFO(BSL_LS_TKS_CTPKT,
                 (BSL_META("CT free: Not initialized\n")));
        return;
    }

    idx = PTR_TO_INT(pkt->cookie2);
    if (idx >= _rx_lists) {
        LOG_INFO(BSL_LS_TKS_CTPKT,
                 (BSL_META("CT free: bad CT index: %d > %d\n"),
                  idx, _rx_lists));
        return;
    }

    CPUTRANS_LOCK;
    pkt->next = _rx_free_lists[idx];
    _rx_free_lists[idx] = pkt;
    CPUTRANS_UNLOCK;
}


/*
 * Function:
 *      cputrans_rx_pkt_cleanup
 * Purpose:
 *      Deallocate RX packets for CPUTRANS
 */

void
cputrans_rx_pkt_cleanup(void)
{
    if (!INIT_DONE) { /* Create lock */
        if (_do_init() < 0) {
            return;
        }
    }

    CPUTRANS_LOCK;
    _FREE_CHECK(_rx_free_lists);
    _FREE_CHECK(_rx_alloc_pkts);
    _FREE_CHECK(_rx_pkt_blks);
    _rx_lists = 0;
    _rx_setup_done = FALSE;
    CPUTRANS_UNLOCK;
}


#undef _FREE_CHECK
#undef _ALLOC_CHECK

#define CT_TRANS_MAX 10
static bcm_trans_ptr_t *ct_trans[CT_TRANS_MAX];
static int ct_trans_count;

int
cputrans_trans_count(void)
{
    return ct_trans_count;
}

int
cputrans_trans_add(bcm_trans_ptr_t *trans)
{
    int i;

    for (i = 0; i < ct_trans_count; i++) {
        if (ct_trans[i] == trans) {
            return BCM_E_NONE;
        }
    }

    if (ct_trans_count + 1 > CT_TRANS_MAX) {
        return BCM_E_RESOURCE;
    }
    ct_trans[ct_trans_count++] = trans;

    return BCM_E_NONE;
}

int
cputrans_trans_remove(bcm_trans_ptr_t *trans)
{
    int i, j;

    for (i = 0; i < ct_trans_count; i++) {
        if (ct_trans[i] == trans) {
            for (j = i; j < ct_trans_count - 1; j++) {
                ct_trans[j] = ct_trans[j + 1];
            }
            ct_trans_count--;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NONE;
}

/*
 * RX register on all known transports
 *     rx_unit_register    -- on the specified unit
 *     rx_bmp_register     -- on the specified bitmap of units (unit < 32)
 *     rx_register         -- on all local units
 *
 * For rx_register and rx_bmp_register, if no units are found or
 * specified, calls on unit -1.
 */

int
cputrans_rx_unit_register(int unit,
                          const char *name,
                          bcm_rx_cb_f callback,
                          uint8 priority,
                          void *cookie,
                          uint32 flags)
{
    int rv = BCM_E_NONE;
    int tmp_rv;
    int i;

    for (i = 0; i < ct_trans_count; i++) {
        if (ct_trans[i]->tp_rx_reg != NULL) {
            tmp_rv = ct_trans[i]->tp_rx_reg(unit, name, callback,
                                            priority, cookie, flags);
            if (tmp_rv < 0) {
                rv = tmp_rv;
            }
        }
    }

    return rv;
}

int
cputrans_rx_register(const char *name,
                     bcm_rx_cb_f callback,
                     uint8 priority,
                     void *cookie,
                     uint32 flags)
{
    int rv = BCM_E_NONE;
    int tmp_rv;
    int found = FALSE;
    int unit;

    for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
        if (BCM_UNIT_VALID(unit) && BCM_IS_LOCAL(unit)) {
            found = TRUE;
            tmp_rv = cputrans_rx_unit_register(unit, name, callback,
                                               priority, cookie, flags);
            if (tmp_rv < 0) {
                LOG_VERBOSE(BSL_LS_TKS_CTPKT,
                            (BSL_META("CPU Trans reg failed %d, unit %d: %s\n"),
                             rv, unit, bcm_errmsg(rv)));
                rv = tmp_rv;
            }
        }
    }

    if (!found) { /* Call transport pointers on unit -1 */
        tmp_rv = cputrans_rx_unit_register(-1, name, callback, priority,
                                           cookie, flags);
        if (tmp_rv < 0) {
            LOG_VERBOSE(BSL_LS_TKS_CTPKT,
                        (BSL_META("CPU Trans reg failed %d, dflt unit: %s\n"),
                         rv, bcm_errmsg(rv)));
            rv = tmp_rv;
        }
    }

    return rv;
}

int
cputrans_rx_bmp_register(uint32 unit_bmp,
                         const char *name,
                         bcm_rx_cb_f callback,
                         uint8 priority,
                         void *cookie,
                         uint32 flags)
{
    int rv = BCM_E_NONE;
    int tmp_rv;
    int unit;

    if (unit_bmp == 0) {
        rv = cputrans_rx_unit_register(-1, name, callback, priority,
                                       cookie, flags);
    } else {
        for (unit = 0; unit < 32; unit++) {
            if (BCM_UNIT_VALID(unit) && BCM_IS_LOCAL(unit) &&
                    ((1 << unit) & unit_bmp)) {
                tmp_rv = cputrans_rx_unit_register(unit, name,
                                                   callback, priority,
                                                   cookie, flags);
                if (tmp_rv < 0) {
                    rv = tmp_rv;
                }
            }
        }
    }

    return rv;
}


/*
 * RX unregister on all known transports
 */

int
cputrans_rx_unit_unregister(int unit, bcm_rx_cb_f callback, uint8 priority)
{
    int rv = BCM_E_NONE;
    int tmp_rv;
    int i;

    for (i = 0; i < ct_trans_count; i++) {
        if (ct_trans[i]->tp_rx_unreg != NULL) {
            tmp_rv = ct_trans[i]->
                tp_rx_unreg(unit, callback, priority);
            if (tmp_rv < 0) {
                rv = tmp_rv;
            }
        }
    }

    return rv;
}

int
cputrans_rx_unregister(bcm_rx_cb_f callback, uint8 priority)
{
    int rv = BCM_E_NONE;
    int tmp_rv;
    int unit;
    int found = FALSE;

    for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
        if (BCM_UNIT_VALID(unit) && BCM_IS_LOCAL(unit)) {
            found = TRUE;
            tmp_rv = cputrans_rx_unit_unregister(unit, callback, priority);
            if (tmp_rv < 0) {
                rv = tmp_rv;
            }
        }
    }

    if (!found) { /* Call transport pointers on unit -1 */
        tmp_rv = cputrans_rx_unit_unregister(-1, callback, priority);
        if (tmp_rv < 0) {
            rv = tmp_rv;
        }
    }

    return rv;
}

int
cputrans_rx_bmp_unregister(uint32 unit_bmp,
                            bcm_rx_cb_f callback,
                            uint8 priority)
{
    int rv = BCM_E_NONE;
    int tmp_rv;
    int unit;

    if (unit_bmp == 0) {
        rv = cputrans_rx_unit_unregister(-1, callback, priority);
    } else {
        for (unit = 0; unit < 32; unit++) {
            if (BCM_UNIT_VALID(unit) && BCM_IS_LOCAL(unit) &&
                    ((1 << unit) & unit_bmp)) {
                tmp_rv = cputrans_rx_unit_unregister(unit, callback, priority);
                if (tmp_rv < 0) {
                    rv = tmp_rv;
                }
            }
        }
    }

    return rv;
}


/*
 * Function:
 *      cputrans_error_count_get
 * Purpose:
 *      Get and optionally clear the error count in this layer
 * Parameters:
 *      clear     - Boolean: If set, will clear the error counter
 * Returns:
 *      BCM_E_XXX
 */

int
cputrans_error_count_get(int clear)
{
    int cur_cnt;

    cur_cnt = cputrans_error_count;
    if (clear) {
        cputrans_error_count = 0;
    }

    return cur_cnt;
}
