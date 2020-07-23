/*! \file bcmcnet_buff.c
 *
 * Utility routines for BCMCNET packet buffer management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmcnet/bcmcnet_main.h>
#include <bcmcnet/bcmcnet_core.h>
#include <bcmcnet/bcmcnet_dev.h>
#include <bcmcnet/bcmcnet_rxtx.h>
#include <bcmcnet/bcmcnet_buff.h>

/*!
 * Allocate coherent memory
 */
static void *
bcmcnet_ring_buf_alloc(struct pdma_dev *dev, uint32_t size, dma_addr_t *dma)
{
    void *mem;
    uint64_t dma_addr;

    mem = bcmdrd_hal_dma_alloc(dev->unit, size, "bcmcnetRxTxRing", &dma_addr);
    if (!mem) {
        return NULL;
    }
    *dma = (dma_addr_t)dma_addr;

    return mem;
}

/*!
 * Free coherent memory
 */
static void
bcmcnet_ring_buf_free(struct pdma_dev *dev, uint32_t size, void *mem, dma_addr_t dma)
{
    bcmdrd_hal_dma_free(dev->unit, size, mem, (uint64_t)dma);
}

/*!
 * Allocate Rx buffer
 */
static int
bcmcnet_rx_buf_alloc(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                     struct pdma_rx_buf *pbuf)
{
    bcmpkt_data_buf_t *buf = NULL;
    struct pkt_buf *pkb = NULL;
    int rv;

    pbuf->len = PKT_HDR_SIZE + pbuf->adj + rxq->buf_size;
    rv = bcmpkt_data_buf_alloc(dev->unit, pbuf->len, &buf);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    pkb = (struct pkt_buf *)buf->data;
    pbuf->skb = buf;
    pbuf->pkb = pkb;
    pbuf->dma = (dma_addr_t)bcmdrd_hal_dma_map(dev->unit, &pkb->data + pbuf->adj,
                                               rxq->buf_size, TRUE);
    if (!pbuf->dma) {
        bcmpkt_data_buf_free(dev->unit, pbuf->skb);
        return SHR_E_MEMORY;
    }

    return SHR_E_NONE;
}

/*!
 * Get Rx buffer DMA address
 */
static void
bcmcnet_rx_buf_dma(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                   struct pdma_rx_buf *pbuf, dma_addr_t *addr)
{
    *addr = pbuf->dma;
}

/*!
 * Check Rx buffer
 */
static int
bcmcnet_rx_buf_avail(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                     struct pdma_rx_buf *pbuf)
{
    return pbuf->dma != 0;
}

/*!
 * Get Rx buffer
 */
static struct pkt_hdr *
bcmcnet_rx_buf_get(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                   struct pdma_rx_buf *pbuf, int len)
{
    bcmpkt_data_buf_t *buf;

    if (!pbuf->dma) {
        return &pbuf->pkb->pkh;
    }

    bcmdrd_hal_dma_unmap(dev->unit, pbuf->dma, rxq->buf_size, TRUE);
    pbuf->dma = 0;
    buf = (bcmpkt_data_buf_t *)pbuf->skb;
    buf->data_len = PKT_HDR_SIZE + pbuf->adj + len;

    return &pbuf->pkb->pkh;
}

/*!
 * Put Rx buffer
 */
static int
bcmcnet_rx_buf_put(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                   struct pdma_rx_buf *pbuf, int len)
{
    pbuf->dma = (dma_addr_t)bcmdrd_hal_dma_map(dev->unit, &pbuf->pkb->data + pbuf->adj,
                                               rxq->buf_size, TRUE);
    if (!pbuf->dma) {
        bcmpkt_data_buf_free(dev->unit, pbuf->skb);
        return SHR_E_MEMORY;
    }

    return SHR_E_NONE;
}

/*!
 * Free Rx buffer
 */
static void
bcmcnet_rx_buf_free(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                    struct pdma_rx_buf *pbuf)
{
    bcmdrd_hal_dma_unmap(dev->unit, pbuf->dma, rxq->buf_size, TRUE);
    bcmpkt_data_buf_free(dev->unit, pbuf->skb);

    pbuf->dma = 0;
    pbuf->len = 0;
    pbuf->skb = NULL;
    pbuf->pkb = NULL;
    pbuf->adj = 0;
}

/*!
 * Get Rx buffer mode
 */
static enum buf_mode
bcmcnet_rx_buf_mode(struct pdma_dev *dev, struct pdma_rx_queue *rxq)
{
    return PDMA_BUF_MODE_PRIV;
}

/*!
 * Get Tx buffer
 */
static struct pkt_hdr *
bcmcnet_tx_buf_get(struct pdma_dev *dev, struct pdma_tx_queue *txq,
                   struct pdma_tx_buf *pbuf, void *buf)
{
    struct pkt_buf *pkb = (struct pkt_buf *)((bcmpkt_data_buf_t *)buf)->data;

    pbuf->len = pkb->pkh.data_len + (pbuf->adj ? pkb->pkh.meta_len : 0);
    pbuf->dma = (dma_addr_t)bcmdrd_hal_dma_map(dev->unit,
                                               &pkb->data + (pbuf->adj ? 0 : pkb->pkh.meta_len),
                                               pbuf->len,
                                               TRUE);
    if (!pbuf->dma) {
        bcmpkt_data_buf_free(dev->unit, buf);
        return NULL;
    }
    pbuf->skb = buf;
    pbuf->pkb = pkb;

    return &pkb->pkh;
}

/*!
 * Get Tx buffer DMA address
 */
static void
bcmcnet_tx_buf_dma(struct pdma_dev *dev, struct pdma_tx_queue *txq,
                   struct pdma_tx_buf *pbuf, dma_addr_t *addr)
{
    *addr = pbuf->dma;
}

/*!
 * Free Tx buffer
 */
static void
bcmcnet_tx_buf_free(struct pdma_dev *dev, struct pdma_tx_queue *txq,
                    struct pdma_tx_buf *pbuf)
{
    bcmdrd_hal_dma_unmap(dev->unit, pbuf->dma, pbuf->len, TRUE);
    bcmpkt_data_buf_free(dev->unit, pbuf->skb);

    pbuf->dma = 0;
    pbuf->len = 0;
    pbuf->skb = NULL;
    pbuf->pkb = NULL;
    pbuf->adj = 0;
}

static const struct pdma_buf_mngr buf_mngr = {
    .ring_buf_alloc     = bcmcnet_ring_buf_alloc,
    .ring_buf_free      = bcmcnet_ring_buf_free,
    .rx_buf_alloc       = bcmcnet_rx_buf_alloc,
    .rx_buf_dma         = bcmcnet_rx_buf_dma,
    .rx_buf_avail       = bcmcnet_rx_buf_avail,
    .rx_buf_get         = bcmcnet_rx_buf_get,
    .rx_buf_put         = bcmcnet_rx_buf_put,
    .rx_buf_free        = bcmcnet_rx_buf_free,
    .rx_buf_mode        = bcmcnet_rx_buf_mode,
    .tx_buf_get         = bcmcnet_tx_buf_get,
    .tx_buf_dma         = bcmcnet_tx_buf_dma,
    .tx_buf_free        = bcmcnet_tx_buf_free,
};

/*!
 * Open a device
 */
void
bcmcnet_buf_mngr_init(struct pdma_dev *dev)
{
    dev->ctrl.buf_mngr = (struct pdma_buf_mngr *)&buf_mngr;
}

