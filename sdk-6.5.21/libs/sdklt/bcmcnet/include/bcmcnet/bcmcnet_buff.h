/*! \file bcmcnet_buff.h
 *
 * Generic data structure definitions for BCMCNET packet buffer management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCNET_BUFF_H
#define BCMCNET_BUFF_H

/*!
 * \brief Rx buffer.
 */
struct pdma_rx_buf {
    /*! DMA address */
    dma_addr_t dma;

    /*! Source buffer length */
    uint32_t len;

    /*! Source buffer address */
    void *skb;

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

    /*! Source buffer length */
    uint32_t len;

    /*! Source buffer address */
    void *skb;

    /*! Packet buffer point */
    struct pkt_buf *pkb;

    /*! Packet buffer adjustment */
    uint32_t adj;
};

#endif /* BCMCNET_BUFF_H */

