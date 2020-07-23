/*! \file bcmcnet_cmicx_pdma_rxtx.c
 *
 * Utility routines for BCMCNET hardware (CMICx) specific Rx/Tx.
 *
 * Here are the CMIC specific Rx/Tx routines including DCBs resource allocation
 * and clean up, DCBs configuration, Rx buffers allocation, Tx buffers release,
 * Rx/Tx packets processing, etc.
 * They are shared among all the modes (UNET, KNET, VNET, HNET) and in both of
 * user space and kernel space.
 *
 * The driver uses a ring of DCBs per DMA channel based on Continuous DMA mode.
 * The beginning is written to register pointing to the physical address of the
 * start of the ring. The ring size is maintained by the driver. A HALT DCB
 * physical address is written to DMA register timely to indicate how many DCBs
 * can be handled by HW.
 *
 * When a packet is received, an interrupt is triggered. The handler will go
 * through the Rx DCB ring to process the current completed DCB and every
 * subsequent DCBs until no one is left. The received packet is processed and
 * passed up to the high level SW. After that, a new buffer is allocated and
 * the DCB is updated for receiving a new packet. A new HALT DCB is selected
 * and its physical address is written to DMA register.
 *
 * When a packet is transmitted, the driver starts where it left off last time
 * in the Tx DCB ring, updates the DCB and writes its physical address to DMA
 * register so as to start DMA. Once the transmitting is finished, the handler
 * is informed to clean up the buffer based on the work mode. In KNET or HNET
 * mode, an interrupt will be triggered. Polling mode is used in CNET or VNET
 * mode, the buffers will be cleaned up when the number of dirty DCBs reaches
 * a pre-defined threshold.
 *
 * In VNET and HNET modes, DCB updating between virtual ring and real ring and
 * a IOCTL based notification mechanism are involved. The hypervisor in kernel
 * emulates the DMA HW behaviors to update DCBs in virtual network and inform
 * the handler something happened. Likewise, the hypervisor updates itself real
 * DCB ring from the virtual ring to start DMA for transmitting a packet once a
 * notification is received from the virtual network.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmcnet/bcmcnet_core.h>
#include <bcmcnet/bcmcnet_dev.h>
#include <bcmcnet/bcmcnet_rxtx.h>
#include <bcmcnet/bcmcnet_buff.h>
#include <bcmcnet/bcmcnet_cmicx.h>

/*!
 * Configure Rx descriptor
 */
static inline void
cmicx_rx_desc_config(struct cmicx_rx_desc *rd, uint64_t addr, uint32_t len)
{
    uint32_t ctrl;

    rd->addr_lo = addr;
    rd->addr_hi = DMA_TO_BUS_HI(addr >> 32);
    rd->status = 0;
    ctrl = rd->ctrl;
    ctrl &= CMICX_DESC_CTRL_REMAIN(0xf);
    ctrl |= CMICX_DESC_CTRL_CNTLD_INTR | CMICX_DESC_CTRL_CHAIN |
            CMICX_DESC_CTRL_LEN(len);
    rd->ctrl = ctrl;
}

/*!
 * Configure Tx descriptor
 */
static inline void
cmicx_tx_desc_config(struct cmicx_tx_desc *td, uint64_t addr, uint32_t len, uint32_t flags)
{
    uint32_t ctrl;

    td->addr_lo = addr;
    td->addr_hi = DMA_TO_BUS_HI(addr >> 32);
    td->status = 0;
    ctrl = td->ctrl;
    ctrl &= CMICX_DESC_CTRL_REMAIN(0xf);
    ctrl |= CMICX_DESC_CTRL_CNTLD_INTR | CMICX_DESC_CTRL_CHAIN |
            CMICX_DESC_CTRL_FLAGS(flags) | CMICX_DESC_CTRL_LEN(len);
    td->ctrl = ctrl;
}

/*!
 * Configure Rx reload descriptor
 */
static inline void
cmicx_rx_rldesc_config(struct cmicx_rx_desc *rd, uint64_t addr)
{
    rd->addr_lo = addr;
    rd->addr_hi = DMA_TO_BUS_HI(addr >> 32);
    rd->status = 0;
    rd->ctrl = CMICX_DESC_CTRL_CNTLD_INTR | CMICX_DESC_CTRL_CHAIN |
               CMICX_DESC_CTRL_RELOAD;
}

/*!
 * Configure Tx reload descriptor
 */
static inline void
cmicx_tx_rldesc_config(struct cmicx_tx_desc *td, uint64_t addr)
{
    td->addr_lo = addr;
    td->addr_hi = DMA_TO_BUS_HI(addr >> 32);
    td->status = 0;
    td->ctrl = CMICX_DESC_CTRL_CNTLD_INTR | CMICX_DESC_CTRL_CHAIN |
               CMICX_DESC_CTRL_RELOAD;
}

/*!
 * Chain Rx descriptor
 */
static inline void
cmicx_rx_desc_chain(struct cmicx_rx_desc *rd, int chain)
{
    if (chain) {
        rd->ctrl |= CMICX_DESC_CTRL_CHAIN;
    } else {
        rd->ctrl &= ~CMICX_DESC_CTRL_CHAIN;
    }
}

/*!
 * Chain Tx descriptor
 */
static inline void
cmicx_tx_desc_chain(struct cmicx_tx_desc *td, int chain)
{
    if (chain) {
        td->ctrl |= CMICX_DESC_CTRL_CHAIN;
    } else {
        td->ctrl &= ~CMICX_DESC_CTRL_CHAIN;
    }
}

/*!
 * Set Rx descriptor remain
 */
static inline void
cmicx_rx_desc_remain(struct cmicx_rx_desc *rd, uint32_t rm)
{
    rd->ctrl &= ~CMICX_DESC_CTRL_REMAIN(0xf);
    rd->ctrl |= CMICX_DESC_CTRL_REMAIN(rm);
}

/*!
 * Set Tx descriptor remain
 */
static inline void
cmicx_tx_desc_remain(struct cmicx_tx_desc *td, uint32_t rm)
{
    td->ctrl &= ~CMICX_DESC_CTRL_REMAIN(0xf);
    td->ctrl |= CMICX_DESC_CTRL_REMAIN(rm);
}

/*!
 * Get unused descriptors in a Rx ring
 */
static inline int
cmicx_pdma_rx_ring_unused(struct pdma_rx_queue *rxq)
{
    /* Leave one descriptor unused so as not to halt */
    return rxq->curr > rxq->halt ?
           rxq->curr - rxq->halt - 1 :
           rxq->nb_desc + rxq->curr - rxq->halt - 1;
}

/*!
 * Get unused descriptors in a Tx ring
 */
static inline int
cmicx_pdma_tx_ring_unused(struct pdma_tx_queue *txq)
{
    /* Leave one descriptor unused so as not to halt */
    return txq->dirt > txq->curr ?
           txq->dirt - txq->curr - 1 :
           txq->nb_desc + txq->dirt - txq->curr - 1;
}

/*!
 * Initialize Rx descriptors
 */
static int
cmicx_pdma_rx_desc_init(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    struct pdma_dev *dev = hw->dev;
    struct pdma_buf_mngr *bm = (struct pdma_buf_mngr *)dev->ctrl.buf_mngr;
    struct cmicx_rx_desc *ring = (struct cmicx_rx_desc *)rxq->ring;
    dma_addr_t addr;
    uint32_t di, rm;
    int rv;

    for (di = 0; di < rxq->nb_desc; di++) {
        if (!rxq->pbuf[di].dma) {
            /* Allocate pktbuf for ring entry */
            rv = bm->rx_buf_alloc(dev, rxq, &rxq->pbuf[di]);
            if (SHR_FAILURE(rv)) {
                goto cleanup;
            }
        }
        /* Config receive descriptor ring */
        bm->rx_buf_dma(dev, rxq, &rxq->pbuf[di], &addr);
        cmicx_rx_desc_config(&ring[di], addr, rxq->buf_size);
        rm = (rxq->nb_desc - di) >= CMICX_DESC_REMAIN_MAX ?
             CMICX_DESC_REMAIN_MAX : rxq->nb_desc - di;
        cmicx_rx_desc_remain(&ring[di], rm);
        if (hw->dev->flags & PDMA_CHAIN_MODE && di == rxq->nb_desc - 1) {
            cmicx_rx_desc_chain(&ring[di], 0);
        }
    }
    /* Config the last descriptor in the ring as reload descriptor */
    cmicx_rx_rldesc_config(&ring[di], rxq->ring_addr);

    rxq->curr = 0;
    rxq->halt = rxq->state & PDMA_RX_BATCH_REFILL ? 0 : rxq->nb_desc;

    rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicx_rx_desc) * di;
    hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
    hw->hdls.chan_setup(hw, rxq->chan_id, rxq->ring_addr);

    return SHR_E_NONE;

cleanup:
    for (di = 0; di < rxq->nb_desc; di++) {
        if (rxq->pbuf[di].dma) {
            bm->rx_buf_free(dev, rxq, &rxq->pbuf[di]);
        }
        cmicx_rx_desc_config(&ring[di], 0, 0);
    }

    CNET_PR("RX: Failed to allocate mem\n");

    return SHR_E_MEMORY;
}

/*!
 * Cleanup Rx descriptors
 */
static int
cmicx_pdma_rx_desc_clean(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    struct pdma_dev *dev = hw->dev;
    struct pdma_buf_mngr *bm = (struct pdma_buf_mngr *)dev->ctrl.buf_mngr;
    struct cmicx_rx_desc *ring = (struct cmicx_rx_desc *)rxq->ring;
    uint32_t di;

    /* Go through all the descriptors and free pktbuf */
    for (di = 0; di < rxq->nb_desc; di++) {
        if (rxq->pbuf[di].dma) {
            bm->rx_buf_free(dev, rxq, &rxq->pbuf[di]);
        }
        cmicx_rx_desc_config(&ring[di], 0, 0);
    }

    rxq->curr = 0;
    rxq->halt = 0;

    return SHR_E_NONE;
}

/*!
 * Initialize Tx descriptors
 */
static int
cmicx_pdma_tx_desc_init(struct pdma_hw *hw, struct pdma_tx_queue *txq)
{
    struct pdma_dev *dev = hw->dev;
    struct pdma_buf_mngr *bm = (struct pdma_buf_mngr *)dev->ctrl.buf_mngr;
    struct cmicx_tx_desc *ring = (struct cmicx_tx_desc *)txq->ring;
    uint32_t di, rm;

    for (di = 0; di < txq->nb_desc; di++) {
        if (txq->pbuf[di].dma) {
            bm->tx_buf_free(dev, txq, &txq->pbuf[di]);
        }
        /* Config transmit descriptor ring */
        cmicx_tx_desc_config(&ring[di], 0, 0, 0);
        rm = (txq->nb_desc - di) >= CMICX_DESC_REMAIN_MAX ?
             CMICX_DESC_REMAIN_MAX : txq->nb_desc - di;
        cmicx_tx_desc_remain(&ring[di], rm);
        if (hw->dev->flags & PDMA_CHAIN_MODE) {
            cmicx_tx_desc_chain(&ring[di], 0);
        }
    }
    /* Config the last descriptor in the ring as reload descriptor */
    cmicx_tx_rldesc_config(&ring[di], txq->ring_addr);

    txq->curr = 0;
    txq->dirt = 0;
    txq->halt = 0;

    txq->halt_addr = txq->ring_addr;
    hw->hdls.chan_goto(hw, txq->chan_id, txq->halt_addr);
    hw->hdls.chan_setup(hw, txq->chan_id, txq->ring_addr);

    return SHR_E_NONE;
}

/*!
 * Cleanup Tx descriptors
 */
static int
cmicx_pdma_tx_desc_clean(struct pdma_hw *hw, struct pdma_tx_queue *txq)
{
    struct pdma_dev *dev = hw->dev;
    struct pdma_buf_mngr *bm = (struct pdma_buf_mngr *)dev->ctrl.buf_mngr;
    struct cmicx_tx_desc *ring = (struct cmicx_tx_desc *)txq->ring;
    uint32_t di;

    /* Go through all the descriptors and free pktbuf */
    for (di = 0; di < txq->nb_desc; di++) {
        if (txq->pbuf[di].dma) {
            bm->tx_buf_free(dev, txq, &txq->pbuf[di]);
        }
        cmicx_tx_desc_config(&ring[di], 0, 0, 0);
    }

    txq->curr = 0;
    txq->dirt = 0;
    txq->halt = 0;

    return SHR_E_NONE;
}

/*!
 * Process Rx vring
 */
static int
cmicx_pdma_rx_vring_process(struct pdma_hw *hw, struct pdma_rx_queue *rxq,
                            struct pdma_rx_buf *pbuf)
{
    struct pdma_dev *dev = hw->dev;
    struct cmicx_rx_desc *ring = (struct cmicx_rx_desc *)rxq->ring;
    struct pdma_rx_queue *vrxq = NULL;
    struct cmicx_rx_desc *vring = NULL;
    struct pkt_hdr *pkh = &pbuf->pkb->pkh;
    uint64_t buf_addr;

    vrxq = (struct pdma_rx_queue *)dev->ctrl.vnet_rxq[rxq->queue_id];
    vring = (struct cmicx_rx_desc *)vrxq->ring;
    if (!vring) {
        rxq->stats.dropped++;
        return SHR_E_UNAVAIL;
    }

    if (vring[vrxq->curr].status & CMICX_DESC_STAT_RTX_DONE) {
        dev->xnet_wake(dev);
        return SHR_E_BUSY;
    }

    /* Copy descriptor and packet to vring */
    buf_addr = BUS_TO_DMA_HI(vring[vrxq->curr].addr_hi);
    buf_addr = buf_addr << 32 | vring[vrxq->curr].addr_lo;
    sal_memcpy(dev->sys_p2v(dev, buf_addr), &pbuf->pkb->data,
               pkh->meta_len + pkh->data_len);
    vring[vrxq->curr].status = ring[rxq->curr].status;

    MEMORY_BARRIER;

    /* Notify VNET to process if needed */
    if (!vring[(vrxq->curr + vrxq->nb_desc - 1) % vrxq->nb_desc].status) {
        dev->xnet_wake(dev);
    }
    vrxq->curr = (vrxq->curr + 1) % vrxq->nb_desc;

    return SHR_E_NONE;
}

/*!
 * Refill Rx ring
 */
static int
cmicx_pdma_rx_ring_refill(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    struct pdma_dev *dev = hw->dev;
    struct pdma_buf_mngr *bm = (struct pdma_buf_mngr *)dev->ctrl.buf_mngr;
    struct cmicx_rx_desc *ring = (struct cmicx_rx_desc *)rxq->ring;
    struct pdma_rx_buf *pbuf = NULL;
    int unused = cmicx_pdma_rx_ring_unused(rxq);
    dma_addr_t addr;
    uint32_t halt;
    int rv;

    for (halt = rxq->halt; halt < rxq->halt + unused; halt++) {
        pbuf = &rxq->pbuf[halt % rxq->nb_desc];
        /* Allocate a new pktbuf */
        if (!bm->rx_buf_avail(dev, rxq, pbuf)) {
            rv = bm->rx_buf_alloc(dev, rxq, pbuf);
            if (SHR_FAILURE(rv)) {
                rxq->halt = halt % rxq->nb_desc;
                rxq->stats.nomems++;
                goto fail;
            }
        }
        /* Setup the new descriptor */
        bm->rx_buf_dma(dev, rxq, pbuf, &addr);
        cmicx_rx_desc_config(&ring[halt % rxq->nb_desc], addr, rxq->buf_size);
        if (dev->flags & PDMA_CHAIN_MODE && halt % rxq->nb_desc == rxq->nb_desc - 1) {
            cmicx_rx_desc_chain(&ring[halt % rxq->nb_desc], 0);
        }
    }

    sal_spinlock_lock(rxq->lock);
    rxq->halt = halt % rxq->nb_desc;
    if (!(rxq->state & PDMA_RX_QUEUE_XOFF)) {
        /* Descriptor cherry pick */
        rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicx_rx_desc) * rxq->halt;
        hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
    }
    sal_spinlock_unlock(rxq->lock);

    return SHR_E_NONE;

fail:
    CNET_PR("RX: Failed to allocate mem\n");

    return SHR_E_MEMORY;
}

/*!
 * \brief Clean Rx ring
 *
 * \param [in] hw HW structure point.
 * \param [in] rxq Rx queue structure point.
 * \param [in] budget Polling budget.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
cmicx_pdma_rx_ring_clean(struct pdma_hw *hw, struct pdma_rx_queue *rxq, int budget)
{
    struct pdma_dev *dev = hw->dev;
    struct pdma_buf_mngr *bm = (struct pdma_buf_mngr *)dev->ctrl.buf_mngr;
    struct cmicx_rx_desc *ring = (struct cmicx_rx_desc *)rxq->ring;
    struct pdma_rx_buf *pbuf = NULL;
    struct pkt_hdr *pkh = NULL;
    dma_addr_t addr;
    uint32_t stat, curr;
    int len, done = 0;
    int rv;

    curr = rxq->curr;
    while (CMICX_DESC_STAT_DONE(ring[curr].status)) {
        if (dev->mode == DEV_MODE_VNET && rxq->state & PDMA_RX_QUEUE_XOFF) {
            break;
        }
        if (!(rxq->state & PDMA_RX_BATCH_REFILL) &&
            !(rxq->state & PDMA_RX_QUEUE_XOFF)) {
            /* Descriptor cherry pick */
            rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicx_rx_desc) * curr;
            hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
            rxq->halt = curr;
        }
        if (done == budget) {
            break;
        }

        /* Get the current pktbuf to process */
        pbuf = &rxq->pbuf[curr];
        stat = ring[curr].status;
        len = CMICX_DESC_STAT_LEN(stat);
        pkh = bm->rx_buf_get(dev, rxq, pbuf, len);
        if (!pkh) {
            rxq->stats.nomems++;
            goto fail;
        }

        /* Setup packet header */
        pkh->data_len = len - hw->info.rx_ph_size;
        pkh->meta_len = hw->info.rx_ph_size;
        pkh->queue_id = rxq->queue_id;
        pkh->attrs = CMICX_DESC_STAT_FLAGS(stat);
        ((uint32_t *)(pkh + 1))[pkh->meta_len / sizeof(uint32_t) - 1] = stat;

        /* Send up the packet */
        rv = dev->pkt_recv(dev, rxq->queue_id, (void *)pbuf->skb);
        if (SHR_FAILURE(rv)) {
            if (dev->mode == DEV_MODE_HNET && pkh->attrs & PDMA_RX_TO_VNET) {
                rv = cmicx_pdma_rx_vring_process(hw, rxq, pbuf);
                if (SHR_FAILURE(rv) && rv == SHR_E_BUSY) {
                    rxq->state |= PDMA_RX_QUEUE_BUSY;
                    return done;
                }
            } else {
                rxq->stats.dropped++;
            }
            bm->rx_buf_put(dev, rxq, pbuf, len);
        }

        /* Count the packets/bytes */
        rxq->stats.packets++;
        rxq->stats.bytes += len;

        /* Count the errors if any */
        if (stat & CMICX_DESC_STAT_ERR_MASK) {
            rxq->stats.errors++;
            if (stat & CMICX_DESC_STAT_DATA_ERR) {
                rxq->stats.data_errors++;
            }
            if (stat & CMICX_DESC_STAT_CELL_ERR) {
                rxq->stats.cell_errors++;
            }
        }

        /* Setup the new descriptor */
        if (!(rxq->state & PDMA_RX_BATCH_REFILL)) {
            if (!bm->rx_buf_avail(dev, rxq, pbuf)) {
                rv = bm->rx_buf_alloc(dev, rxq, pbuf);
                if (SHR_FAILURE(rv)) {
                    rxq->stats.nomems++;
                    goto fail;
                }
            }
            bm->rx_buf_dma(dev, rxq, pbuf, &addr);
            cmicx_rx_desc_config(&ring[curr], addr, rxq->buf_size);
            if (dev->flags & PDMA_CHAIN_MODE && curr == rxq->nb_desc - 1) {
                cmicx_rx_desc_chain(&ring[curr], 0);
            }
        } else {
            cmicx_rx_desc_config(&ring[curr], 0, 0);
        }

        /* Notify HNET to process if needed */
        if (dev->mode == DEV_MODE_VNET) {
            MEMORY_BARRIER;
            if (ring[(curr + rxq->nb_desc - 1) % rxq->nb_desc].status) {
                dev->xnet_wake(dev);
            }
        }

        /* Update the indicators */
        if (!(rxq->state & PDMA_RX_BATCH_REFILL) && rxq->halt != curr) {
            sal_spinlock_lock(rxq->lock);
            if (!(rxq->state & PDMA_RX_QUEUE_XOFF)) {
                /* Descriptor cherry pick */
                rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicx_rx_desc) * curr;
                hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
                rxq->halt = curr;
            }
            curr = (curr + 1) % rxq->nb_desc;
            sal_spinlock_unlock(rxq->lock);
        } else {
            curr = (curr + 1) % rxq->nb_desc;
        }
        rxq->curr = curr;
        done++;

        /* Restart DMA if in chain mode */
        if (dev->flags & PDMA_CHAIN_MODE) {
            if (curr == 0 && !(rxq->state & PDMA_RX_QUEUE_XOFF)) {
                hw->hdls.chan_stop(hw, rxq->chan_id);
                hw->hdls.chan_start(hw, rxq->chan_id);
            }
        }
    }

    /* One more poll for chain done in chain mode */
    if (dev->flags & PDMA_CHAIN_MODE) {
        if (curr == rxq->nb_desc - 1 && done) {
            done = budget;
        }
    }

    /* In batching mode, replenish all the unused descriptors */
    if (rxq->state & PDMA_RX_BATCH_REFILL &&
        cmicx_pdma_rx_ring_unused(rxq) >= (int)rxq->free_thresh) {
        cmicx_pdma_rx_ring_refill(hw, rxq);
    }

    /* Notify the other side to process */
    if (dev->mode == DEV_MODE_VNET || dev->mode == DEV_MODE_HNET) {
        if (done) {
            dev->xnet_wake(dev);
        }
    }

    return done;

fail:
    CNET_PR("RX: Failed to allocate mem\n");

    return done;
}

/*!
 * Process Tx vring
 */
static int
cmicx_pdma_tx_vring_process(struct pdma_hw *hw, struct pdma_tx_queue *txq,
                            struct pdma_tx_buf *pbuf)
{
    struct pdma_dev *dev = hw->dev;
    struct cmicx_tx_desc *ring = (struct cmicx_tx_desc *)txq->ring;
    struct pdma_tx_queue *vtxq = NULL;
    struct cmicx_tx_desc *vring = NULL;

    vtxq = (struct pdma_tx_queue *)dev->ctrl.vnet_txq[txq->queue_id];
    vring = (struct cmicx_tx_desc *)vtxq->ring;
    if (!vring) {
        return SHR_E_UNAVAIL;
    }

    /* Update vring descriptor */
    vring[vtxq->dirt].status = ring[txq->dirt].status;
    pbuf->dma = 0;

    MEMORY_BARRIER;

    /* Notify VNET to process if needed */
    if (!vring[(vtxq->dirt + vtxq->nb_desc - 1) % vtxq->nb_desc].status) {
        dev->xnet_wake(dev);
    }
    vtxq->dirt = (vtxq->dirt + 1) % vtxq->nb_desc;

    return SHR_E_NONE;
}

/*!
 * \brief Clean Tx ring
 *
 * \param [in] hw HW structure point.
 * \param [in] txq Tx queue structure point.
 * \param [in] budget Polling budget.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
cmicx_pdma_tx_ring_clean(struct pdma_hw *hw, struct pdma_tx_queue *txq, int budget)
{
    struct pdma_dev *dev = hw->dev;
    struct pdma_buf_mngr *bm = (struct pdma_buf_mngr *)dev->ctrl.buf_mngr;
    struct cmicx_tx_desc *ring = (struct cmicx_tx_desc *)txq->ring;
    uint32_t dirt, curr;
    int done = 0;

    dirt = txq->dirt;
    while (txq->pbuf[dirt].dma) {
        if (!CMICX_DESC_STAT_DONE(ring[dirt].status)) {
            break;
        }
        if (done == budget) {
            break;
        }

        if (dev->mode == DEV_MODE_HNET && !txq->pbuf[dirt].skb) {
            cmicx_pdma_tx_vring_process(hw, txq, &txq->pbuf[dirt]);
        } else {
            /* Free the done pktbuf */
            bm->tx_buf_free(dev, txq, &txq->pbuf[dirt]);
        }

        cmicx_tx_desc_config(&ring[dirt], 0, 0, 0);

        /* Update the indicators */
        dirt = (dirt + 1) % txq->nb_desc;
        txq->dirt = dirt;
        done++;

        /* Restart DMA if in chain mode */
        if (dev->flags & PDMA_CHAIN_MODE) {
            sal_spinlock_lock(txq->lock);
            curr = txq->curr;
            if (dirt == txq->halt && dirt != curr) {
                hw->hdls.chan_stop(hw, txq->chan_id);
                cmicx_tx_desc_chain(&ring[(curr + txq->nb_desc - 1) % txq->nb_desc], 0);
                hw->hdls.chan_setup(hw, txq->chan_id,
                                    txq->ring_addr + sizeof(struct cmicx_tx_desc) * txq->halt);
                hw->hdls.chan_start(hw, txq->chan_id);
                txq->halt = curr;
            }
            sal_spinlock_unlock(txq->lock);
        }
    }

    /* One more poll for chain done in chain mode */
    if (dev->flags & PDMA_CHAIN_MODE) {
        sal_spinlock_lock(txq->lock);
        if (dirt != txq->halt) {
            done = budget;
        }
        sal_spinlock_unlock(txq->lock);
    }

    /* Resume Tx if any */
    sal_spinlock_lock(txq->lock);
    if (cmicx_pdma_tx_ring_unused(txq) && txq->state & PDMA_TX_QUEUE_XOFF) {
        txq->state &= ~PDMA_TX_QUEUE_XOFF;
        sal_spinlock_unlock(txq->lock);
        if (dev->tx_resume) {
            dev->tx_resume(dev, txq->queue_id);
        } else {
            sal_sem_give(txq->sem);
        }
        return done;
    }
    sal_spinlock_unlock(txq->lock);

    return done;
}

/*!
 * Dump Rx ring
 */
static int
cmicx_pdma_rx_ring_dump(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    struct cmicx_rx_desc *ring = (struct cmicx_rx_desc *)rxq->ring;
    struct cmicx_rx_desc *rd;
    uint32_t di;

    CNET_PR("\nRX: queue=%d, chan=%d, curr=%d, halt=%d, halt@%p\n",
            rxq->queue_id, rxq->chan_id, rxq->curr, rxq->halt, (void *)&ring[rxq->halt]);
    CNET_PR("----------------------------------------------------------------\n");
    for (di = 0; di < rxq->nb_desc + 1; di++) {
        rd = &ring[di];
        CNET_PR("DESC[%03d]: (%p)->%08x %08x %08x %08x\n",
                di, (void *)(unsigned long)(rxq->ring_addr + di * CMICX_PDMA_DCB_SIZE),
                rd->addr_lo, rd->addr_hi, rd->ctrl, rd->status);
    }

    return SHR_E_NONE;
}

/*!
 * Dump Tx ring
 */
static int
cmicx_pdma_tx_ring_dump(struct pdma_hw *hw, struct pdma_tx_queue *txq)
{
    struct cmicx_tx_desc *ring = (struct cmicx_tx_desc *)txq->ring;
    struct cmicx_tx_desc *td;
    uint32_t di;

    CNET_PR("\nTX: queue=%d, chan=%d, curr=%d, dirt=%d, halt@%p\n",
            txq->queue_id, txq->chan_id, txq->curr, txq->dirt, (void *)&ring[txq->curr]);
    CNET_PR("----------------------------------------------------------------\n");
    for (di = 0; di < txq->nb_desc + 1; di++) {
        td = &ring[di];
        CNET_PR("DESC[%03d]: (%p)->%08x %08x %08x %08x\n",
                di, (void *)(unsigned long)(txq->ring_addr + di * CMICX_PDMA_DCB_SIZE),
                td->addr_lo, td->addr_hi, td->ctrl, td->status);
    }

    return SHR_E_NONE;
}

/*!
 * Fetch Tx vring
 */
static int
cmicx_pdma_tx_vring_fetch(struct pdma_hw *hw, struct pdma_tx_queue *txq,
                          struct pdma_tx_buf *pbuf)
{
    struct pdma_dev *dev = hw->dev;
    struct cmicx_tx_desc *ring = (struct cmicx_tx_desc *)txq->ring;
    struct pdma_tx_queue *vtxq = NULL;
    struct cmicx_tx_desc *vring = NULL;

    vtxq = (struct pdma_tx_queue *)dev->ctrl.vnet_txq[txq->queue_id];
    vring = (struct cmicx_tx_desc *)vtxq->ring;
    if (!vring || !CMICX_DESC_CTRL_LEN(vring[vtxq->curr].ctrl)) {
        return SHR_E_UNAVAIL;
    }

    /* Fetch vring descriptor */
    sal_memcpy(&ring[txq->curr], &vring[vtxq->curr], sizeof(struct cmicx_tx_desc));
    vring[vtxq->curr].ctrl &= ~CMICX_DESC_CTRL_LEN(-1);

    MEMORY_BARRIER;

    pbuf->dma = vring[vtxq->curr].addr_lo;
    pbuf->len = CMICX_DESC_CTRL_LEN(ring[txq->curr].ctrl);
    vtxq->curr = (vtxq->curr + 1) % vtxq->nb_desc;

    return SHR_E_NONE;
}

/*!
 * Check Tx ring
 */
static inline int
cmicx_pdma_tx_ring_check(struct pdma_hw *hw, struct pdma_tx_queue *txq)
{
    if (cmicx_pdma_tx_ring_unused(txq)) {
        return SHR_E_NONE;
    }

    sal_spinlock_lock(txq->lock);
    if (!cmicx_pdma_tx_ring_unused(txq)) {
        txq->state |= PDMA_TX_QUEUE_XOFF;
        txq->stats.xoffs++;
        sal_spinlock_unlock(txq->lock);
        if (hw->dev->tx_suspend) {
            hw->dev->tx_suspend(hw->dev, txq->queue_id);
        }
        return SHR_E_BUSY;
    }
    sal_spinlock_unlock(txq->lock);

    return SHR_E_NONE;
}

/*!
 * \brief Start packet transmission
 *
 * \param [in] hw HW structure point.
 * \param [in] txq Tx queue structure point.
 * \param [in] buf Tx packet buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
cmicx_pdma_pkt_xmit(struct pdma_hw *hw, struct pdma_tx_queue *txq, void *buf)
{
    struct pdma_dev *dev = hw->dev;
    struct pdma_buf_mngr *bm = (struct pdma_buf_mngr *)dev->ctrl.buf_mngr;
    struct cmicx_tx_desc *ring = (struct cmicx_tx_desc *)txq->ring;
    struct pdma_tx_buf *pbuf = NULL;
    struct pkt_hdr *pkh = NULL;
    dma_addr_t addr;
    uint32_t curr, flags = 0;
    int retry = 1000000;
    int rv;

    if (!(txq->state & PDMA_TX_QUEUE_ACTIVE)) {
        return SHR_E_UNAVAIL;
    }

    if (dev->tx_suspend) {
        sal_spinlock_lock(txq->mutex);
    } else {
        sal_sem_take(txq->sem, SAL_SEM_FOREVER);
    }

    /* Suspend Tx if no resource */
    if (dev->tx_suspend) {
        rv = cmicx_pdma_tx_ring_check(hw, txq);
        if (SHR_FAILURE(rv)) {
            sal_spinlock_unlock(txq->mutex);
            return rv;
        }
    }

    /* Setup the new descriptor */
    curr = txq->curr;
    pbuf = &txq->pbuf[curr];
    if (dev->mode == DEV_MODE_HNET && !buf) {
        rv = cmicx_pdma_tx_vring_fetch(hw, txq, pbuf);
        if (SHR_FAILURE(rv)) {
            sal_spinlock_unlock(txq->mutex);
            return SHR_E_EMPTY;
        }
    } else {
        pbuf->adj = 1;
        pkh = bm->tx_buf_get(dev, txq, pbuf, buf);
        if (!pkh) {
            txq->stats.dropped++;
            if (dev->tx_suspend) {
                sal_spinlock_unlock(txq->mutex);
            } else {
                sal_sem_give(txq->sem);
            }
            return SHR_E_NONE;
        }
        bm->tx_buf_dma(dev, txq, pbuf, &addr);
        flags |= pkh->attrs & PDMA_TX_HIGIG_PKT ? CMICX_DESC_TX_HIGIG_PKT : 0;
        flags |= pkh->attrs & PDMA_TX_PURGE_PKT ? CMICX_DESC_TX_PURGE_PKT : 0;
        cmicx_tx_desc_config(&ring[curr], addr, pbuf->len, flags);
    }

    /* Notify HNET to process if needed */
    if (dev->mode == DEV_MODE_VNET) {
        MEMORY_BARRIER;
        if (!CMICX_DESC_CTRL_LEN(ring[(curr + txq->nb_desc - 1) % txq->nb_desc].ctrl)) {
            dev->xnet_wake(dev);
        }
    }

    /* Update the indicators */
    curr = (curr + 1) % txq->nb_desc;
    txq->curr = curr;

    /* Start DMA if in chain mode */
    if (dev->flags & PDMA_CHAIN_MODE) {
        if (txq->state & PDMA_TX_QUEUE_POLL) {
            do {
                rv = cmicx_pdma_tx_ring_clean(hw, txq, txq->nb_desc - 1);
                if (rv != (int)txq->nb_desc - 1) {
                    break;
                }
                sal_usleep(1);
            } while (retry--);
            if (retry <= 0) {
                CNET_PR("Last Tx could not be done in given time\n");
            }
        }
        sal_spinlock_lock(txq->lock);
        if (txq->dirt == txq->halt && txq->dirt != curr) {
            hw->hdls.chan_stop(hw, txq->chan_id);
            cmicx_tx_desc_chain(&ring[(curr + txq->nb_desc - 1) % txq->nb_desc], 0);
            hw->hdls.chan_setup(hw, txq->chan_id,
                                txq->ring_addr + sizeof(struct cmicx_tx_desc) * txq->halt);
            hw->hdls.chan_start(hw, txq->chan_id);
            txq->halt = curr;
        }
        sal_spinlock_unlock(txq->lock);
    }

    /* Kick off DMA */
    txq->halt_addr = txq->ring_addr + sizeof(struct cmicx_tx_desc) * curr;
    hw->hdls.chan_goto(hw, txq->chan_id, txq->halt_addr);

    /* Count the packets/bytes */
    txq->stats.packets++;
    txq->stats.bytes += pbuf->len;

    /* Clean up ring if in polling mode */
    if (txq->state & PDMA_TX_QUEUE_POLL &&
        cmicx_pdma_tx_ring_unused(txq) <= (int)txq->free_thresh) {
        cmicx_pdma_tx_ring_clean(hw, txq, txq->nb_desc - txq->free_thresh);
    }

    /* Suspend Tx if no resource */
    rv = cmicx_pdma_tx_ring_check(hw, txq);
    if (SHR_FAILURE(rv)) {
        if (dev->mode == DEV_MODE_VNET) {
            dev->xnet_wake(dev);
        }

        /* In polling mode, must wait till the ring is available */
        if (txq->state & PDMA_TX_QUEUE_POLL) {
            do {
                cmicx_pdma_tx_ring_clean(hw, txq, txq->free_thresh);
                if (!(txq->state & PDMA_TX_QUEUE_XOFF)) {
                    break;
                }
                sal_usleep(1);
            } while (retry--);
            if (retry <= 0) {
                CNET_PR("Tx resource could not be released in given time\n");
            }
        }

        if (!dev->tx_suspend) {
            return SHR_E_NONE;
        }
    }

    if (dev->tx_suspend) {
        sal_spinlock_unlock(txq->mutex);
    } else {
        sal_sem_give(txq->sem);
    }

    return SHR_E_NONE;
}

/*!
 * Suspend Rx queue
 */
static int
cmicx_pdma_rx_suspend(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    sal_spinlock_lock(rxq->lock);
    rxq->state |= PDMA_RX_QUEUE_XOFF;
    if (hw->dev->flags & PDMA_CHAIN_MODE) {
        hw->hdls.chan_stop(hw, rxq->chan_id);
    }
    sal_spinlock_unlock(rxq->lock);

    return SHR_E_NONE;
}

/*!
 * Resume Rx queue
 */
static int
cmicx_pdma_rx_resume(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    sal_spinlock_lock(rxq->lock);
    if (!(rxq->state & PDMA_RX_QUEUE_XOFF)) {
        sal_spinlock_unlock(rxq->lock);
        return SHR_E_NONE;
    }
    if (rxq->state & PDMA_RX_BATCH_REFILL) {
        rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicx_rx_desc) * rxq->halt;
        hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
    } else if (rxq->halt == rxq->curr || (rxq->halt == rxq->nb_desc && rxq->curr == 0)) {
        rxq->halt = (rxq->curr + 1) % rxq->nb_desc;
        rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicx_rx_desc) * rxq->halt;
        hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
    }
    if (hw->dev->flags & PDMA_CHAIN_MODE) {
        rxq->curr = 0;
        hw->hdls.chan_start(hw, rxq->chan_id);
    }
    rxq->state &= ~PDMA_RX_QUEUE_XOFF;
    sal_spinlock_unlock(rxq->lock);

    return SHR_E_NONE;
}

/*!
 * Initialize function pointers
 */
int
bcmcnet_cmicx_pdma_desc_ops_init(struct pdma_hw *hw)
{
    if (!hw) {
        return SHR_E_PARAM;
    }

    hw->dops.rx_desc_init = cmicx_pdma_rx_desc_init;
    hw->dops.rx_desc_clean = cmicx_pdma_rx_desc_clean;
    hw->dops.rx_ring_clean = cmicx_pdma_rx_ring_clean;
    hw->dops.rx_ring_dump = cmicx_pdma_rx_ring_dump;
    hw->dops.rx_suspend = cmicx_pdma_rx_suspend;
    hw->dops.rx_resume = cmicx_pdma_rx_resume;
    hw->dops.tx_desc_init = cmicx_pdma_tx_desc_init;
    hw->dops.tx_desc_clean = cmicx_pdma_tx_desc_clean;
    hw->dops.tx_ring_clean = cmicx_pdma_tx_ring_clean;
    hw->dops.tx_ring_dump = cmicx_pdma_tx_ring_dump;
    hw->dops.pkt_xmit = cmicx_pdma_pkt_xmit;

    return SHR_E_NONE;
}

/*!
 * Attach device driver
 */
int
bcmcnet_cmicx_pdma_driver_attach(struct pdma_dev *dev)
{
    struct pdma_hw *hw = NULL;

    /* Allocate memory for HW data */
    hw = sal_alloc(sizeof(*hw), "bcmcnetPdmaHw");
    if (!hw) {
        return SHR_E_MEMORY;
    }
    sal_memset(hw, 0, sizeof(*hw));
    hw->unit = dev->unit;
    hw->dev = dev;
    dev->ctrl.hw = hw;

    bcmcnet_cmicx_pdma_hw_hdls_init(hw);
    bcmcnet_cmicx_pdma_desc_ops_init(hw);

    return SHR_E_NONE;
}

/*!
 * Detach device driver
 */
int
bcmcnet_cmicx_pdma_driver_detach(struct pdma_dev *dev)
{
    if (dev->ctrl.hw) {
        sal_free(dev->ctrl.hw);
    }
    dev->ctrl.hw = NULL;

    return SHR_E_NONE;
}

