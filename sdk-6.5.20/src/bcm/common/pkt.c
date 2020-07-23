/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/alloc.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>

#include <soc/cm.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <bcm/pkt.h>
#include <bcm_int/control.h>
#if defined(INCLUDE_L3) && defined(BCM_FIREBOLT_SUPPORT)
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/l3.h>
#endif

#define UNIT_VALID(unit) \
{ \
  if (!BCM_UNIT_VALID(unit)) { return BCM_E_UNIT; } \
}

/*
 * Function:
 *      bcm_pkt_byte_index
 * Purpose:
 *      Return a pointer to a location in a packet
 * Parameters:
 *      pkt - The packet of interest
 *      n   - Byte offset in the packet
 *      len - (OUT) number of bytes left in this allocation block
 *      blk - (OUT) if not NULL, gets the pointer to the current block
 *      location - (OUT) pointer to the requested byte
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_pkt_byte_index(bcm_pkt_t *pkt, int n, int *len, int *blk_idx,
                   uint8 **location)
{
    int cnt = 0;
    int blk = 0;
    uint8 *ptr = NULL;
    int offset;

    for (blk = 0; blk < pkt->blk_count; blk++) {
        if (cnt + pkt->pkt_data[blk].len > n) {
            offset = n - cnt;
            ptr = &pkt->pkt_data[blk].data[offset];
            if (len) {
                *len = pkt->pkt_data[blk].len - offset;
            }
            if (blk_idx) {
                *blk_idx = blk;
            }
            break;
        } else {
            cnt += pkt->pkt_data[blk].len;
        }
    }

    *location = ptr;
    return (NULL !=ptr) ? BCM_E_NONE : BCM_E_NOT_FOUND;
}


/*
 * Function:
 *      bcm_pkt_memcpy
 * Purpose:
 *
 * Parameters:
 *      pkt - Packet to copy into
 *      dest_byte - Integer offset into packet
 *      src - Source buffer to copy from
 *      len - Number of bytes to copy
 * Returns:
 *      Number of bytes actually copied
 */

int
bcm_pkt_memcpy(bcm_pkt_t *pkt, int dest_byte, uint8 *src, int len)
{
    int blk;
    uint8 *ptr = NULL;
    int blk_bytes, rv;
    int copied = 0;
    
    rv = bcm_pkt_byte_index(pkt, dest_byte, &blk_bytes, &blk, &ptr);

    if ((BCM_E_NONE == rv) && (ptr)) {
        while (1) {
            if (blk_bytes > 0) {
                if (len - copied <= blk_bytes) { /* Last block */
                    sal_memcpy(ptr, &src[copied], len - copied);
                    copied = len;
                    break;
                }
                sal_memcpy(ptr, &src[copied], blk_bytes);
                copied += blk_bytes;
            }
            if (++blk >= pkt->blk_count) {
                break;
            }
            ptr = pkt->pkt_data[blk].data;
            blk_bytes = pkt->pkt_data[blk].len;
        }
    }

    return copied;
}

/*
 * Function:
 *      bcm_pkt_t_init
 * Purpose:
 *      Initialize packet structure.
 * Parameters:
 *      pkt - Pointer to packet structure.
 * Returns:
 *      NONE
 */
void
bcm_pkt_t_init(bcm_pkt_t *pkt)
{
    if (pkt != NULL) {
        sal_memset(pkt, 0, sizeof (*pkt));
    }
    return;
}

/*
 * Function:
 *      bcm_pkt_blk_t_init
 * Purpose:
 *      Initialize packet block structure.
 * Parameters:
 *      pkt_blk - Pointer to packet block structure.
 * Returns:
 *      NONE
 */
void
bcm_pkt_blk_t_init(bcm_pkt_blk_t *pkt_blk)
{
    if (pkt_blk != NULL) {
        sal_memset(pkt_blk, 0, sizeof (*pkt_blk));
    }
    return;
}

/*
 * Function:
 *      bcm_pkt_clear
 * Purpose:
 *      Clear a packet structure and install its data buffer
 * Parameters:
 *      unit - for what unit is this being set up
 *      pkt - pointer to pointer packet to setup.  (*pkt) may be NULL
 *      blks - Pointer to array of gather blocks for the packet
 *      blk_count - Number of elts in the array
 *      flags - flags for CRC and VLAN tag.  See notes.
 *      pkt_buf (OUT) - pkt, or allocated packet if pkt is NULL
 * Returns:
 *      Pointer to packet or NULL if cannot allocate new packet.
 * Notes:
 *      If pkt is null, allocate a new one with sal_alloc.
 *
 *      Flags can be the bitwise or of:
 *        BCM_TX_CRC_NONE        No action on CRC
 *        BCM_TX_CRC_ALLOC       CRC is not in packet; allocate
 *        BCM_TX_CRC_REGEN       Regenerate the CRC on egress
 *        BCM_TX_CRC_APPEND      Allocate and regenerate (same as alloc)
 *        BCM_PKT_F_NO_VTAG      Packet does not contain vlan tag
 *
 *      In addition, the following may be used to override normal
 *      behavior, but are not generally advertised or documented.
 *        BCM_TX_CRC_FORCE_ERROR Force an error in the CRC (unsupported)
 *        BCM_PKT_F_HGHDR        HiGig header setup for packet DMA
 *        BCM_PKT_F_SLTAG        SL tag setup for packet DMA
 *
 *      The payload length of the packet is set as large as possible
 *      assuming the allocation blocks contain the MAC addresses and
 *      (unless NO_VTAG is specified) the VLAN tag; if CRC append (or alloc)
 *      is indicated, the CRC is assumed NOT to be part of the gather
 *      blocks.
 */

int
bcm_pkt_clear(int unit, bcm_pkt_t *pkt, bcm_pkt_blk_t *blks, int blk_count,
              uint32 flags, bcm_pkt_t **pkt_buf)
{
    int rv, i;
    int bytes = 0;
    int local_alloc = FALSE;

    UNIT_VALID(unit);

    if (pkt == NULL) {  /* Allocate new packet */
        local_alloc = TRUE;
        if ((pkt = sal_alloc(sizeof(bcm_pkt_t), "pkt_setup")) == NULL) {
            *pkt_buf = NULL;
            return BCM_E_MEMORY;
        }
    }
    sal_memset(pkt, 0, sizeof(bcm_pkt_t));
    pkt->unit = unit;

    if (blk_count == 0) {
        /*
         * Permit allocation of an empty packet structure.
         * A single buffer can be added alter using the
         * BCM_PKT_ONE_BUF_SETUP macro.
         */
        bcm_pkt_flags_init(unit, pkt, flags);
    } else {
        for (i = 0; i < blk_count; i++) {
            bytes += blks[i].len;
        }

        if (bytes < BCM_PKT_ALLOC_MIN) {
            *pkt_buf = NULL;
            if (local_alloc) {
                sal_free(pkt);
            }
            return BCM_E_MEMORY;
        }

        pkt->pkt_data = blks;
        pkt->blk_count = blk_count;

        rv = bcm_pkt_flags_len_setup(unit, pkt, bytes, -1, flags);
        if (BCM_FAILURE(rv)) {
            *pkt_buf = NULL;
            if (local_alloc) {
                sal_free(pkt);
            }
            return rv;
        }
    }

    *pkt_buf = pkt;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_pkt_flags_len_setup
 * Purpose:
 *      Install data in a packet without clearing fields
 * Parameters:
 *      unit - for what unit is this being set up
 *      pkt - the packet to setup
 *      buf - buffer to use; must be non-null
 *      alloc_bytes - buffer length (allocated)
 *      payload_len - Length of payload.  See notes
 *      flags - See above.
 * Returns:
 *      >= 0:  Payload length
 *      < 0:   BCM_E_XXX
 * Notes:
 *      If payload_len < 0, this means make payload as large as possible.
 *
 *      The payload_len is checked because space for the VLAN tag,
 *      HiGig header and SL tag is always allocated.
 *
 *      The payload length includes everything in the packet EXCEPT:
 *          dest and src MAC;
 *          VLAN tag (4 bytes)
 *          CRC
 *          SL Tag
 *          HiGig header
 *      The txrx length includes all of the above.
 *      The packet length value depends on the format indications.
 *      It will exclude the VLAN tag if indicated in flags; it will
 *      exclude the CRC if CRC_ALLOC is indicated.
 *
 *      Valid values for CRC are:
 *         BCM_TX_CRC_NONE, BCM_TX_CRC_ALLOC, BCM_TX_CRC_REGEN,
 *         BCM_TX_CRC_APPEND.  These maybe combined with
 *         BCM_TX_CRC_FORCE_ERROR,
 */

int
bcm_pkt_flags_len_setup(int unit, bcm_pkt_t *pkt, int alloc_bytes,
                        int payload_len, uint32 flags)
{
    UNIT_VALID(unit);

    if (payload_len < 0) {
        payload_len = alloc_bytes - BCM_PKT_ALLOC_MIN;
    } else if (payload_len > alloc_bytes - BCM_PKT_ALLOC_MIN) {
        return BCM_E_PARAM;
    }

    bcm_pkt_flags_init(unit, pkt, flags);

    return payload_len;
}


int
bcm_pkt_flags_init(int unit, bcm_pkt_t *pkt, uint32 flags)
{
    int rv;

    BCM_UNIT_BUSY(unit);
    if (BCM_UNIT_VALID(unit)) {
        pkt->flags = flags;
        pkt->unit = unit;   /* Set unit as well */

        if (BCM_IS_LOCAL(unit)) {
            if (SOC_UNIT_VALID(unit) && SOC_IS_XGS12_FABRIC(unit)) {
                pkt->flags |= BCM_PKT_F_HGHDR;
            }
            /* Was SL flags handling, but SL is no longer supported */
        }
        rv = BCM_E_NONE;
        
    } else {
        rv = BCM_E_UNIT;
    }
    BCM_UNIT_IDLE(unit);
    
    return rv;
}





/****************************************************************
 *
 * Default allocate and free routines.  Uses soc_cm_ shared
 * memory routines.  These use single data buffers and do
 * runtime allocation and free.
 */

#define DEFAULT_ALLOCATOR_UNIT 0
#define NO_ALLOCATOR_UNIT -1

/*
  Return a local unit number (or NO_ALLOCATOR_UNIT) that can allocate
  a buffer for the given unit.

  For local units, the unit returned is the unit passed. For remote
  units, return DEFAULT_ALLOCATOR_UNIT if DEFAULT_ALLOCATOR_UNIT unit
  itself is valid and local, otherwise return NO_ALLOCATOR_UNIT (which
  usually means tunneling to a remote unit when there are no local
  units, which is a valid configuration).

*/

static int
_bcm_pkt_allocator_unit(int unit)
{
    /* allocation unit */
    if (BCM_IS_REMOTE(unit)) {
        unit = (BCM_UNIT_VALID(DEFAULT_ALLOCATOR_UNIT) &&
                BCM_IS_LOCAL(DEFAULT_ALLOCATOR_UNIT)) ?
            DEFAULT_ALLOCATOR_UNIT : NO_ALLOCATOR_UNIT;
    }

    return unit;
}
 
/* could be bcm_pkt_blk_t_alloc */

static int
_bcm_pkt_data_alloc(int unit, int size, bcm_pkt_blk_t *pkt_data)
{
    int	aunit;
    int rv = BCM_E_MEMORY;

    /* allocation unit, which may not be the same as unit */
    aunit = _bcm_pkt_allocator_unit(unit);
#if defined (BCM_LTSW_SUPPORT)
    if (SOC_IS_LTSW(unit)) {
        aunit = NO_ALLOCATOR_UNIT;
    }
#endif /* BCM_LTSW_SUPPORT */

    BCM_UNIT_BUSY(aunit);
    /* If aunit == NO_ALLOCATOR_UNIT, use a heap allocator, because
       there are no DMAable devices available. */
    pkt_data->data = (aunit == NO_ALLOCATOR_UNIT) ?
        sal_alloc(size, "pkt alloc data") :
        soc_cm_salloc(aunit, size, "pkt alloc data");

    if (pkt_data->data) {
        pkt_data->len = size;
        rv = BCM_E_NONE;
    }
    BCM_UNIT_IDLE(aunit);

    return rv; 
}

static int
_bcm_pkt_data_free(int unit, bcm_pkt_blk_t *pkt_data)
{
    int	aunit;

    /* allocation unit, which may not be the same as unit */
    aunit = _bcm_pkt_allocator_unit(unit);
#if defined(BCM_LTSW_SUPPORT)
    if (SOC_IS_LTSW(unit)) {
        aunit = NO_ALLOCATOR_UNIT;
    }
#endif /* BCM_LTSW_SUPPORT */
    BCM_UNIT_BUSY(aunit);
    if (pkt_data->data != NULL) {
        if (aunit == NO_ALLOCATOR_UNIT) {
            sal_free(pkt_data->data);
        } else {
            soc_cm_sfree(aunit, pkt_data->data);
        }
    }
    pkt_data->data = NULL;
    pkt_data->len = 0;
    BCM_UNIT_IDLE(aunit);

    return BCM_E_NONE; 
}



/*
 * Function:
 *      bcm_pkt_alloc
 * Purpose:
 *      Basic, single block packet allocation using sal and soc functions
 * Parameters:
 *      unit - Strata unit
 *      size - Bytes to allocate
 *      flags - See pkt.h for more about these flags
 * Returns:
 *      Pointer to packet if successful or NULL if not
 * Notes:
 *	Flags are copied into the packet, and so may indicate if CRC/VLAN
 *	should be stripped.
 *
 *      If the unit allocator returns NO_ALLOCATOR_UNIT, then assume
 *      that a DMA allocator is not required, and use a regular memory
 *      allocator.
 */

int
bcm_pkt_alloc(int unit, int size, uint32 flags, bcm_pkt_t **pkt_buf)
{
    bcm_pkt_t *pkt;
    int rv = BCM_E_INTERNAL;
    int retry_counter;

    UNIT_VALID(unit);

    pkt = sal_alloc(sizeof(bcm_pkt_t), "bcm_pkt_alloc");
    if (!pkt) {
        *pkt_buf = NULL;
        return BCM_E_MEMORY;
    }
    sal_memset(pkt, 0, sizeof(bcm_pkt_t));
    pkt->pkt_data = &pkt->_pkt_data;
    pkt->blk_count = 1;

    for (retry_counter = 1; retry_counter <= 500; retry_counter++) {
        if (BCM_SUCCESS(rv = _bcm_pkt_data_alloc(unit, size, pkt->pkt_data))) {
            break;
        }
        LOG_INFO(BSL_LS_BCM_TX,
                 (BSL_META_U
                  (unit, "[%d] Failed to allocate packet buffer, retrying ...\n"),
                  retry_counter));
        sal_usleep(retry_counter < 100 ? 1000: 20000);
    }

    if (BCM_FAILURE(rv)) {
        sal_free(pkt);
        *pkt_buf = NULL;
        return rv;
    }
    bcm_pkt_flags_init(unit, pkt, flags);

    *pkt_buf = pkt;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_pkt_free
 * Purpose:
 *      Basic, free routine using sal and soc functions
 * Parameters:
 *      unit - Strata unit
 *      size - Bytes to allocate
 *      flags - See pkt.h for more about these flags
 * Returns:
 * Notes:
 *      Works with bcm_pkt_alloc.  Checks for multiple blocks,
 *      so could be used with other routines that use sal_alloc
 *      and soc_cm_salloc.
 */

int
bcm_pkt_free(int unit, bcm_pkt_t *pkt)
{
    int i;
#if defined(BCM_CMICX_SUPPORT)
    soc_stat_t    *stat = &SOC_CONTROL(unit)->stat;
#endif

    UNIT_VALID(unit);

    if (pkt) {
        for (i = 0; i < pkt->blk_count; i++) {
            if (pkt->pkt_data[i].data) {
                _bcm_pkt_data_free(unit, &pkt->pkt_data[i]);
            }
        }
#if defined(BCM_CMICX_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicx)) {
#if defined(BCM_CMICX_TX_PREALLOC_SUPPORT)
            stat->dma_tbyt -= sizeof(pkt->_higig);
#else
            if (pkt->tx_header) {
                stat->dma_tbyt -= sizeof(pkt->_higig);
                soc_cm_sfree(unit, pkt->tx_header);
                pkt->tx_header = NULL;
            }
#endif
        }
#endif

        sal_free(pkt);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_pkt_blk_alloc
 * Purpose:
 *      Allocate an array of packets
 * Parameters:
 *      unit - Strata device number
 *      count - How many packets to alloc
 *      size - Bytes in each packet
 *      flags - flags to use for packet alloc
 * Returns:
 *      Pointer to allocated structure or NULL if fails
 */

int
bcm_pkt_blk_alloc(int unit, int count, int size, uint32 flags, 
                  bcm_pkt_t ***packet_array)
{
    bcm_pkt_t          **p_array;
    int i, j;

    UNIT_VALID(unit);

    if (!(p_array = sal_alloc(count * sizeof(bcm_pkt_t *), "pkt_blk"))) {
        *packet_array = NULL;
        return (BCM_E_MEMORY);
    }

    for (i = 0; i < count; i++) {
        if (BCM_FAILURE(bcm_pkt_alloc(unit, size, flags, p_array + i))) {

            for (j = 0; j < i; j++) {
                bcm_pkt_free(unit, p_array[j]);
            }
            sal_free(p_array);
            *packet_array = NULL;
            return (BCM_E_MEMORY);
        }
    }

    *packet_array = p_array;
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_pkt_blk_free
 * Purpose:
 *      Free an array of packets allocated with bcm_pkt_blk_alloc
 * Parameters:
 *      unit - Strata device number
 *      pkts - pointer to array of packets
 *      count - How many packets to alloc
 * Returns:
 */

int 
bcm_pkt_blk_free(int unit, bcm_pkt_t **pkts, int count)
{
    int i;

    UNIT_VALID(unit);

    if (pkts) {
        for (i = 0; i < count; i++) {
            if (pkts[i]) {
                bcm_pkt_free(unit, pkts[i]);
            }
        }
        sal_free(pkts);
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_pkt_rx_alloc
 * Purpose:
 *      Allocate a packet using RX alloc
 * Parameters:
 *      len    - length of data buffer
 * Returns:
 *      Pointer to packet or NULL if memory error
 * Notes:
 *      Sets up the packet to have a single buffer.
 */

int
bcm_pkt_rx_alloc(int unit, int len, bcm_pkt_t **pkt_buf)
{
    bcm_pkt_t *pkt;
    uint8 *buf;
    int rv;

    UNIT_VALID(unit);

    buf = NULL;
    pkt = sal_alloc(sizeof(bcm_pkt_t), "pkt_rx_alloc");
    if (pkt == NULL) {
        *pkt_buf = NULL;
        return BCM_E_MEMORY;
    }
    sal_memset(pkt, 0, sizeof(bcm_pkt_t));

    if (len > 0) {
        rv = bcm_rx_alloc(unit, len, 0, (void*)&buf);
        if (rv != BCM_E_NONE) {
            sal_free(pkt);
            return rv;
        }
        pkt->_pkt_data.data = buf;
        pkt->_pkt_data.len = len;
        pkt->pkt_len = len;
        pkt->pkt_data = &pkt->_pkt_data;
        pkt->blk_count = 1;
    }

    /* Set up flags here if possible */

    *pkt_buf = pkt;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_pkt_rx_free
 * Purpose:
 *      Free a packet allocated using bcm_pkt_rx_alloc
 * Parameters:
 *      pkt    - packet to free
 * Returns:
 * Notes:
 *      Checks for null pkt and buffer
 */

int
bcm_pkt_rx_free(int unit, bcm_pkt_t *pkt)
{
    UNIT_VALID(unit);

    if (pkt) {
        if (pkt->_pkt_data.data) {
            bcm_rx_free(unit, pkt->_pkt_data.data);
        }
        sal_free(pkt);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_rx_reasons_t_init
 * Purpose:
 *      Initialize RX reasons structure.
 * Parameters:
 *      reasons - (INOUT) Pointer to the structure to be initialized.
 * Returns:
 *      NONE
 */
void
bcm_rx_reasons_t_init(bcm_rx_reasons_t *reasons)
{
    if (reasons != NULL) {
        sal_memset(reasons, 0, sizeof (*reasons));
    }
    return;
}



int
bcm_pkt_ecmp_grp_set(int unit, bcm_pkt_t *pkt, bcm_if_t ecmp_grp)
{
#if defined(INCLUDE_L3) && defined(BCM_FIREBOLT_SUPPORT)
    if (ecmp_grp < BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit)) {
        return BCM_E_PARAM;
    }

    pkt->txprocmh_ecmp_group_index =
         ecmp_grp - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

    return BCM_E_NONE;
#else
    return BCM_E_CONFIG;
#endif
}
int
bcm_pkt_ecmp_member_set(int unit, bcm_pkt_t *pkt, bcm_if_t ecmp_grp,
                        bcm_if_t ecmp_member)
{
#if defined(INCLUDE_L3) && defined(BCM_FIREBOLT_SUPPORT)
    int ecmp_group_idx = ecmp_grp - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    int ecmp_count = 0;
    int *ecmp_mem, max_group_size, i;
    int rv;

    if (ecmp_grp < BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit)) {
        return BCM_E_PARAM;
    }

    if (ecmp_member < BCM_XGS3_EGRESS_IDX_MIN(unit)) {
        return BCM_E_PARAM;
    }

    ecmp_mem = sal_alloc(4096 * sizeof(bcm_if_t), "intf array");

    if (ecmp_mem == NULL) {
        return BCM_E_MEMORY;
    }

    pkt->txprocmh_ecmp_group_index =
        ecmp_grp - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

    rv = _bcm_xgs3_ecmp_max_grp_size_get(unit, ecmp_group_idx, &max_group_size);
    if (BCM_FAILURE(rv)) {
        sal_free(ecmp_mem);
        return rv;
    }

    rv = bcm_xgs3_l3_egress_multipath_get(unit, ecmp_grp, 
                                        max_group_size, ecmp_mem, &ecmp_count);

    for( i = 0; i < ecmp_count; ++i) {
        if (ecmp_mem[i] == ecmp_member) {
            pkt->txprocmh_ecmp_member_index = i;
            sal_free(ecmp_mem);
            return BCM_E_NONE;
        }
    }
    sal_free(ecmp_mem);
    return BCM_E_PARAM;
#else
    return BCM_E_CONFIG;
#endif
}

int
bcm_pkt_nexthop_set(int unit, bcm_pkt_t *pkt, bcm_if_t nexthop_id)
{

#if defined(INCLUDE_L3) && defined(BCM_FIREBOLT_SUPPORT)
    if (nexthop_id < BCM_XGS3_EGRESS_IDX_MIN(unit)) {
        return BCM_E_PARAM;
    }

    pkt->txprocmh_destination = nexthop_id - BCM_XGS3_EGRESS_IDX_MIN(unit);
    return BCM_E_NONE;
#else
    return BCM_E_CONFIG;
#endif
}

