/*! \file ngknet_buff.h
 *
 * Generic data structure definitions for NGKNET packet buffer management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef NGKNET_BUFF_H
#define NGKNET_BUFF_H

/*! Rx buffer align size */
#define PDMA_RXB_ALIGN          32
/*! Rx buffer reserved size */
#define PDMA_RXB_RESV           (PDMA_RXB_ALIGN + PKT_HDR_SIZE)
/*! Rx SKB reserved size */
#define PDMA_SKB_RESV           (PDMA_RXB_RESV + SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))
/*! Rx buffer size */
#define PDMA_RXB_SIZE(len)      (SKB_DATA_ALIGN(len + NET_SKB_PAD) + PDMA_SKB_RESV)
/*! Rx reserved meta size */
#define PDMA_RXB_META           64
/*! Max page buffer size */
#define PDMA_PAGE_BUF_MAX       2048

/*!
 * \brief Rx buffer.
 */
struct pdma_rx_buf {
    /*! DMA address */
    dma_addr_t dma;

    /*! Buffer page */
    struct page *page;

    /*! Buffer page offset */
    unsigned int page_offset;

    /*! Rx SKB */
    struct sk_buff *skb;

    /*! Packet buffer point */
    struct pkt_buf *pkb;

    /*! Packet buffer adjustment */
    uint32_t adj;
};

/*!
 * \brief Tx buffer.
 */
struct pdma_tx_buf {
    /*! DMA address */
    dma_addr_t dma;

    /*! Tx buffer length */
    uint32_t len;

    /*! Tx SKB */
    struct sk_buff *skb;

    /*! Packet buffer point */
    struct pkt_buf *pkb;

    /*! Packet buffer adjustment */
    uint32_t adj;
};

#endif /* NGKNET_BUFF_H */

