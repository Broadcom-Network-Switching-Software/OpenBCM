/*! \file ngknet_buff.c
 *
 * Utility routines for NGKNET packet buffer management in Linux kernel mode.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmcnet/bcmcnet_core.h>
#include <bcmcnet/bcmcnet_dev.h>
#include <bcmcnet/bcmcnet_rxtx.h>
#include "ngknet_main.h"
#include "ngknet_buff.h"

/*!
 * Allocate coherent memory
 */
static void *
bcmcnet_ring_buf_alloc(struct pdma_dev *dev, uint32_t size, dma_addr_t *dma)
{
    struct ngknet_dev *kdev = (struct ngknet_dev *)dev->priv;

    return dma_alloc_coherent(kdev->dev, size, dma, GFP_KERNEL);
}

/*!
 * Free coherent memory
 */
static void
bcmcnet_ring_buf_free(struct pdma_dev *dev, uint32_t size, void *addr, dma_addr_t dma)
{
    struct ngknet_dev *kdev = (struct ngknet_dev *)dev->priv;

    dma_free_coherent(kdev->dev, size, addr, dma);
}

/*!
 * Allocate Rx buffer
 */
static int
bcmcnet_rx_buf_alloc(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                     struct pdma_rx_buf *pbuf)
{
    struct ngknet_dev *kdev = (struct ngknet_dev *)dev->priv;
    dma_addr_t dma;
    struct page *page;
    struct sk_buff *skb;

    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        page = kal_dev_alloc_page();
        if (unlikely(!page)) {
            return SHR_E_MEMORY;
        }
        dma = dma_map_page(kdev->dev, page, 0, PAGE_SIZE, DMA_FROM_DEVICE);
        if (unlikely(dma_mapping_error(kdev->dev, dma))) {
            __free_page(page);
            return SHR_E_MEMORY;
        }
        pbuf->dma = dma;
        pbuf->page = page;
        pbuf->page_offset = 0;
    } else {
        skb = netdev_alloc_skb(kdev->net_dev, PDMA_RXB_RESV + pbuf->adj + rxq->buf_size);
        if (unlikely(!skb)) {
            return SHR_E_MEMORY;
        }
        skb_reserve(skb, PDMA_RXB_ALIGN - (((unsigned long)skb->data) & (PDMA_RXB_ALIGN - 1)));
        pbuf->skb = skb;
        pbuf->pkb = (struct pkt_buf *)skb->data;
        dma = dma_map_single(kdev->dev, &pbuf->pkb->data + pbuf->adj, rxq->buf_size, DMA_FROM_DEVICE);
        if (unlikely(dma_mapping_error(kdev->dev, dma))) {
            dev_kfree_skb_any(skb);
            return SHR_E_MEMORY;
        }
        pbuf->dma = dma;
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
    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        *addr = pbuf->dma + pbuf->page_offset + PDMA_RXB_RESV + pbuf->adj;
    } else {
        *addr = pbuf->dma;
    }
}

/*!
 * Check Rx buffer
 */
static int
bcmcnet_rx_buf_avail(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                     struct pdma_rx_buf *pbuf)
{
    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        pbuf->skb = NULL;
    }

    return pbuf->dma != 0;
}

/*!
 * Get Rx buffer
 */
static struct pkt_hdr *
bcmcnet_rx_buf_get(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                   struct pdma_rx_buf *pbuf, int len)
{
    struct ngknet_dev *kdev = (struct ngknet_dev *)dev->priv;
    struct sk_buff *skb;

    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        if (pbuf->skb) {
            return &pbuf->pkb->pkh;
        }
        skb = kal_build_skb(page_address(pbuf->page) + pbuf->page_offset,
                            PDMA_SKB_RESV + pbuf->adj + rxq->buf_size);
        if (unlikely(!skb)) {
            return NULL;
        }
        skb_reserve(skb, PDMA_RXB_ALIGN);
        dma_sync_single_range_for_cpu(kdev->dev, pbuf->dma, pbuf->page_offset,
                                      PDMA_PAGE_BUF_MAX, DMA_FROM_DEVICE);
        pbuf->skb = skb;
        pbuf->pkb = (struct pkt_buf *)skb->data;

        /* Try to reuse this page */
        if (unlikely(page_count(pbuf->page) != 1)) {
            dma_unmap_page(kdev->dev, pbuf->dma, PAGE_SIZE, DMA_FROM_DEVICE);
            pbuf->dma = 0;
        } else {
            pbuf->page_offset ^= PDMA_PAGE_BUF_MAX;
            page_ref_inc(pbuf->page);
            dma_sync_single_range_for_device(kdev->dev, pbuf->dma, pbuf->page_offset,
                                             PDMA_PAGE_BUF_MAX, DMA_FROM_DEVICE);
        }
    } else {
        if (!pbuf->dma) {
            return &pbuf->pkb->pkh;
        }
        skb = pbuf->skb;
        dma_unmap_single(kdev->dev, pbuf->dma, rxq->buf_size, DMA_FROM_DEVICE);
        pbuf->dma = 0;
    }

    skb_put(skb, PKT_HDR_SIZE + pbuf->adj + len);

    return &pbuf->pkb->pkh;
}

/*!
 * Put Rx buffer
 */
static int
bcmcnet_rx_buf_put(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                   struct pdma_rx_buf *pbuf, int len)
{
    struct ngknet_dev *kdev = (struct ngknet_dev *)dev->priv;
    dma_addr_t dma;
    struct sk_buff *skb;

    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        dev_kfree_skb_any(pbuf->skb);
    } else {
        skb = pbuf->skb;
        dma = dma_map_single(kdev->dev, &pbuf->pkb->data + pbuf->adj,
                             rxq->buf_size, DMA_FROM_DEVICE);
        if (unlikely(dma_mapping_error(kdev->dev, dma))) {
            dev_kfree_skb_any(skb);
            pbuf->dma = 0;
            return SHR_E_MEMORY;
        }
        pbuf->dma = dma;
        skb_trim(skb, skb->len - (PKT_HDR_SIZE + pbuf->adj + len));
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
    struct ngknet_dev *kdev = (struct ngknet_dev *)dev->priv;

    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        dma_unmap_single(kdev->dev, pbuf->dma, PAGE_SIZE, DMA_FROM_DEVICE);
        __free_page(pbuf->page);
    } else {
        dma_unmap_single(kdev->dev, pbuf->dma, rxq->buf_size, DMA_FROM_DEVICE);
        dev_kfree_skb_any(pbuf->skb);
    }

    pbuf->dma = 0;
    pbuf->page = NULL;
    pbuf->page_offset = 0;
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
    uint32_t len;

    len = dev->rx_ph_size ? rxq->buf_size : rxq->buf_size + PDMA_RXB_META;
    if (PDMA_RXB_SIZE(len) <= PDMA_PAGE_BUF_MAX && PAGE_SIZE < 8192 &&
        kal_support_paged_skb()) {
        return PDMA_BUF_MODE_PAGE;
    }

    return PDMA_BUF_MODE_SKB;
}

/*!
 * Get Tx buffer
 */
static struct pkt_hdr *
bcmcnet_tx_buf_get(struct pdma_dev *dev, struct pdma_tx_queue *txq,
                   struct pdma_tx_buf *pbuf, void *buf)
{
    struct ngknet_dev *kdev = (struct ngknet_dev *)dev->priv;
    struct sk_buff *skb = (struct sk_buff *)buf;
    struct pkt_buf *pkb = (struct pkt_buf *)skb->data;
    dma_addr_t dma;

    pbuf->len = pkb->pkh.data_len + (pbuf->adj ? pkb->pkh.meta_len : 0);
    dma = dma_map_single(kdev->dev, &pkb->data + (pbuf->adj ? 0 : pkb->pkh.meta_len),
                         pbuf->len, DMA_TO_DEVICE);
    if (unlikely(dma_mapping_error(kdev->dev, dma))) {
        dev_kfree_skb_any(skb);
        return NULL;
    }
    pbuf->dma = dma;
    pbuf->skb = skb;
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
    struct ngknet_dev *kdev = (struct ngknet_dev *)dev->priv;

    dma_unmap_single(kdev->dev, pbuf->dma, pbuf->len, DMA_TO_DEVICE);
    dev_kfree_skb_any(pbuf->skb);

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

