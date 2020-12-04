/*! \file bcmpkt_buf.c
 *
 * Interfaces for packet buffer management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#endif
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_internal.h>
#include "bcmpkt_buf_internal.h"

#define BSL_LOG_MODULE          BSL_LS_BCMPKT_PACKET

/* Minimum allocation size. */
#define BCMPKT_ALLOC_LEN_MIN    64

/*
 * Minimum packet size (not including CRC).
 *
 * This is a global setting which should be left at its default value
 * under normal operation. The value may be lowered to allow debug
 * packets pass the various sanity checks in the packet I/O driver.
 */
static uint32_t pktsize_min = BCMPKT_FRAME_SIZE_MIN;

int
bcmpkt_alloc(int unit, uint32_t len, uint32_t flags, bcmpkt_packet_t **packet)
{
    bcmpkt_packet_t *pkt = NULL;
    uint32_t extr_size;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    /* Overwrite pointer is not allowed. */
    if (*packet) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META_U(unit,
                              "Allocate buffer for not NULL pointer\n")));
        return SHR_E_EXISTS;
    }

    if (len < BCMPKT_ALLOC_LEN_MIN) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META_U(unit, "Len (%d) is too small\n"), len));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    pkt = bcmpkt_ppool_alloc();
    SHR_NULL_CHECK(pkt, SHR_E_MEMORY);

    sal_memset(pkt, 0, sizeof(*pkt));

    extr_size = (flags & BCMPKT_BUF_F_TX) ? BCMPKT_TX_HDR_RSV : 0;
    SHR_IF_ERR_EXIT
        (bcmpkt_data_buf_alloc(unit, len + extr_size, &pkt->data_buf));

    bcmpkt_reserve(pkt->data_buf, extr_size);
    bcmpkt_pmd_format(pkt);
    pkt->unit = unit;
    *packet = pkt;

exit:
    if (SHR_FUNC_ERR() && pkt != NULL) {
        sal_free(pkt);
    }
    SHR_FUNC_EXIT();
}

int
bcmpkt_free(int unit, bcmpkt_packet_t *packet)
{
    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    if (packet->data_buf != NULL) {
        SHR_IF_ERR_EXIT
            (bcmpkt_data_buf_free(packet->unit, packet->data_buf));
    }

    sal_memset(packet, 0, sizeof(*packet));

    SHR_IF_ERR_EXIT
        (bcmpkt_ppool_free(packet));

exit:
    SHR_FUNC_EXIT();
}

uint8_t *
bcmpkt_reserve(bcmpkt_data_buf_t *dbuf, uint32_t len)
{
    int tailroom;

    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return NULL;
    }
    assert(dbuf->data >= dbuf->head && dbuf->len >= dbuf->data_len);

    tailroom = dbuf->len - dbuf->data_len - (dbuf->data - dbuf->head);
    assert(tailroom >= 0);

    /* Reserve function works for blank packet (len=0) only */
    if (dbuf->data_len == 0 && (uint32_t)tailroom >= len) {
        dbuf->data += len;
        return dbuf->data;
    } else {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("Reserve space failed\n")));
        return NULL;
    }
}

uint8_t *
bcmpkt_push(bcmpkt_data_buf_t *dbuf, uint32_t len)
{
    int headroom;

    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return NULL;
    }

    headroom = dbuf->data - dbuf->head;
    assert(headroom >= 0);
    if ((uint32_t)headroom >= len) {
        dbuf->data -= len;
        dbuf->data_len  += len;
        return dbuf->data;
    }
    LOG_ERROR(BSL_LS_BCMPKT_PACKET,
              (BSL_META("Not enough space: Request - %dB, headroom - %dB\n"),
               len, headroom));
    return NULL;
}

uint8_t *
bcmpkt_put(bcmpkt_data_buf_t *dbuf, uint32_t len)
{
    int tailroom;

    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return NULL;
    }

    assert(dbuf->data >= dbuf->head &&
           dbuf->len >= dbuf->data_len);

    tailroom = dbuf->len - dbuf->data_len - (dbuf->data - dbuf->head);
    assert(tailroom >= 0);
    if ((uint32_t)tailroom >= len) {
        uint8_t *addr = dbuf->data + dbuf->data_len;
        dbuf->data_len  += len;
        return addr;
    }

    LOG_ERROR(BSL_LS_BCMPKT_PACKET,
              (BSL_META("Not enough space: Request - %dB, tailroom - %dB\n"),
               len, tailroom));
    return NULL;
}

uint8_t *
bcmpkt_pull(bcmpkt_data_buf_t *dbuf, uint32_t len)
{

    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return NULL;
    }

    if (dbuf->data_len >= len) {
        dbuf->data += len;
        dbuf->data_len  -= len;
        return dbuf->data;
    }

    LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("Invalid length\n")));

    return NULL;
}

int
bcmpkt_trim(bcmpkt_data_buf_t *dbuf, uint32_t len)
{
    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return SHR_E_PARAM;
    }

    if (dbuf->data_len > len) {
        dbuf->data_len = len;
    }

    return SHR_E_NONE;
}

uint32_t
bcmpkt_headroom(bcmpkt_data_buf_t *dbuf)
{
    int headroom;
    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return 0;
    }

    headroom = dbuf->data - dbuf->head;
    assert(headroom >= 0);

    return (uint32_t)headroom;
}

uint32_t
bcmpkt_tailroom(bcmpkt_data_buf_t *dbuf)
{
    int tailroom;

    if (dbuf == NULL) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return 0;
    }

    assert(dbuf->data >= dbuf->head &&
           dbuf->len >= dbuf->data_len);

    tailroom = dbuf->len - dbuf->data_len - (dbuf->data - dbuf->head);
    assert(tailroom >= 0);

    if (tailroom < 0) {
        tailroom = 0;
    }
    return (uint32_t)tailroom;
}

int
bcmpkt_data_buf_alloc(int unit, uint32_t size, bcmpkt_data_buf_t **dbuf)
{
    bcmpkt_data_buf_t *buf;
    uint32_t req_size;
    uint32_t buf_size = 0;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    if (*dbuf) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "The **data_buf content should be NULL\n")));
    }

    if (size < BCMPKT_ALLOC_LEN_MIN) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META_U(unit, "Size (%d) is too small\n"), size));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Add buffer descriptor space.
     * A descriptor is embedded in the front of each buffer to provide
     * convenience for buffer operation. (e.g. add/remove header)
     */
    req_size = size + sizeof(bcmpkt_data_buf_t);
    /* Overflow check. */
    if (size > req_size) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META_U(unit, "Size (%d) is too large\n"), size));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    buf = (bcmpkt_data_buf_t *)bcmpkt_bpool_alloc(unit, req_size, &buf_size);
    SHR_NULL_CHECK(buf, SHR_E_MEMORY);

    /* Format buffer descriptor. */
    buf->head = (uint8_t *)buf + sizeof(bcmpkt_data_buf_t);
    buf->len = buf_size - sizeof(bcmpkt_data_buf_t);
    buf->data = buf->head;
    buf->data_len = 0;
    buf->ref_count = 1;
    *dbuf = buf;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_data_buf_free(int unit, bcmpkt_data_buf_t *dbuf)
{
    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_free(unit, (void *)dbuf));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_packet_claim(int unit, bcmpkt_packet_t *pkt, bcmpkt_packet_t **new_pkt)
{
    bcmpkt_packet_t *npkt = NULL;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(new_pkt, SHR_E_PARAM);
    if (*new_pkt) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "The **new_pkt content should be NULL\n")));
    }

    npkt = bcmpkt_ppool_alloc();
    SHR_NULL_CHECK(npkt, SHR_E_MEMORY);

    sal_memset(npkt, 0, sizeof(bcmpkt_packet_t));

    if (unit == pkt->unit) {
        npkt->data_buf = pkt->data_buf;
        /* Detach data buffer from pkt. */
        pkt->data_buf = NULL;
    } else {
        SHR_IF_ERR_EXIT
            (bcmpkt_data_buf_copy(unit, pkt->data_buf, &npkt->data_buf));
    }

    /* Copy packet structure information and metadata. */
    bcmpkt_pmd_format(npkt);
    sal_memcpy(npkt->pmd.data, pkt->pmd.data, sizeof(npkt->pmd.data));
    npkt->unit = unit;
    npkt->flags = pkt->flags;
    npkt->type = pkt->type;
    *new_pkt = npkt;

exit:
    if (SHR_FUNC_ERR() && npkt != NULL) {
        bcmpkt_ppool_free(npkt);
    }
    SHR_FUNC_EXIT();
}

int
bcmpkt_packet_clone(int unit, bcmpkt_packet_t *pkt, bcmpkt_packet_t **new_pkt)
{
    bcmpkt_packet_t *npkt = NULL;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(new_pkt, SHR_E_PARAM);
    if (*new_pkt) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "The **new_pkt content should be NULL\n")));
    }

    npkt = bcmpkt_ppool_alloc();
    SHR_NULL_CHECK(npkt, SHR_E_MEMORY);

    sal_memset(npkt, 0, sizeof(bcmpkt_packet_t));

    /* Reference the buffer. */
    npkt->data_buf = pkt->data_buf;
    SHR_NULL_CHECK(npkt->data_buf, SHR_E_FAIL);
    /* If ref_count has been clear, return failure. */
    if ((++(npkt->data_buf->ref_count)) <= 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Copy packet structure information and metadata. */
    bcmpkt_pmd_format(npkt);
    sal_memcpy(npkt->pmd.data, pkt->pmd.data, sizeof(npkt->pmd.data));
    npkt->unit = pkt->unit;
    npkt->flags = pkt->flags;
    npkt->type = pkt->type;
    *new_pkt = npkt;

exit:
    if (SHR_FUNC_ERR() && npkt != NULL) {
        sal_free(npkt);
    }
    SHR_FUNC_EXIT();
}

int
bcmpkt_data_buf_copy(int unit, bcmpkt_data_buf_t *dbuf,
                     bcmpkt_data_buf_t **new_dbuf)
{
    bcmpkt_data_buf_t *buf = NULL;
    uint32_t headroom;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(new_dbuf, SHR_E_PARAM);

    if (*new_dbuf) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "The **new_buf content should be NULL\n")));
    }

    headroom = bcmpkt_headroom(dbuf);
    SHR_IF_ERR_EXIT
        (bcmpkt_data_buf_alloc(unit, dbuf->data_len + headroom, &buf));
    assert(bcmpkt_reserve(buf, headroom));
    assert(bcmpkt_put(buf, dbuf->data_len));
    sal_memcpy(buf->data, dbuf->data, dbuf->data_len);
    buf->ref_count = 1;
    *new_dbuf = buf;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_get(bcmpkt_packet_t *packet, uint32_t **rxpmd)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->pmd.rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);

    *rxpmd = packet->pmd.rxpmd;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_txpmd_get(bcmpkt_packet_t *packet, uint32_t **txpmd)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->pmd.txpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(txpmd, SHR_E_PARAM);

    *txpmd = packet->pmd.txpmd;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_lbhdr_get(bcmpkt_packet_t *packet, uint32_t **lbhdr)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->pmd.lbhdr, SHR_E_PARAM);
    SHR_NULL_CHECK(lbhdr, SHR_E_PARAM);

    *lbhdr = packet->pmd.lbhdr;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_higig_get(bcmpkt_packet_t *packet, uint32_t **hg_hdr)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->pmd.higig, SHR_E_PARAM);
    SHR_NULL_CHECK(hg_hdr, SHR_E_PARAM);

    *hg_hdr = packet->pmd.higig;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_pmd_format(bcmpkt_packet_t *packet)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    packet->pmd.rxpmd = packet->pmd.data;
    packet->pmd.txpmd = packet->pmd.rxpmd + BCMPKT_RCPU_RXPMD_SIZE / 4;
    packet->pmd.higig = packet->pmd.txpmd + BCMPKT_RCPU_TX_MH_SIZE / 4;
    packet->pmd.lbhdr = packet->pmd.higig + BCMPKT_RCPU_TX_MH_SIZE / 4;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_framesize_min_set(uint32_t size)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (size < 1 || size > BCMPKT_FRAME_SIZE_MIN) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META("The minimum packet size (%d) is out of "
                       "valid range [1, %d]\n"),
              size, BCMPKT_FRAME_SIZE_MIN));
    }

    pktsize_min = size;

exit:
    SHR_FUNC_EXIT();
}

uint32_t
bcmpkt_framesize_min_get(void)
{
    return pktsize_min;
}

int
bcmpkt_data_dump(shr_pb_t *pb, const uint8_t *data, int size)
{
    int idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    if (size > 256) {
        size = 256;
    }

    for (idx = 0; idx < size; idx++) {
        if ((idx & 0xf) == 0) {
            shr_pb_printf(pb, "%04x: ", idx);
        }
        if ((idx & 0xf) == 8) {
            shr_pb_printf(pb, "- ");
        }
        shr_pb_printf(pb, "%02x ", data[idx]);
        if ((idx & 0xf) == 0xf) {
            shr_pb_printf(pb, "\n");
        }
    }
    if ((idx & 0xf) != 0) {
        shr_pb_printf(pb, "\n");
    }
exit:
    SHR_FUNC_EXIT();
}

/* Debug. */
void
bcmpkt_data_buf_dump(bcmpkt_data_buf_t *dbuf, shr_pb_t *pb)
{
    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META("NULL data buffer pointer.\n")));
        return;
    }
    if (!pb) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META("NULL print buffer pointer.\n")));
        return;
    }

    shr_pb_printf(pb, "head - %p\n", dbuf->head);
    shr_pb_printf(pb, "data - %p\n", dbuf->data);
    shr_pb_printf(pb, "len - %"PRIu32"\n", dbuf->len);
    shr_pb_printf(pb, "data_len - %"PRIu32"\n", dbuf->data_len);
    shr_pb_printf(pb, "refcnt - %d\n", dbuf->ref_count);
    bcmpkt_data_dump(pb, dbuf->data, dbuf->data_len);
}
