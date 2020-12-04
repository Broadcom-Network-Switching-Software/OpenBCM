/*! \file bcmcnet_internal.h
 *
 * BCMCNET internal data structure and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCNET_INTERNAL_H
#define BCMCNET_INTERNAL_H

#include <bcmcnet/bcmcnet_dev.h>

/*! CMICD name */
#define CMICD_DEV_NAME      "cmicd"

/*! CMICX name */
#define CMICX_DEV_NAME      "cmicx"

/*!
 * \brief Allocate descriptor ring buffer.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] dma DMA address of ring buffer.
 *
 * \retval Pointer to DMA buffer or NULL if an error occurred.
 */
typedef void *(*ring_buf_alloc_f)(struct pdma_dev *dev, uint32_t, dma_addr_t *dma);

/*!
 * \brief Free descriptor ring buffer.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] size Size of DMA buffer.
 * \param [in] mem Pointer to DMA buffer.
 * \param [in] dma DMA address of ring buffer.
 */
typedef void (*ring_buf_free_f)(struct pdma_dev *dev, uint32_t size, void *mem,
                                dma_addr_t dma);

/*!
 * \brief Allocate Rx packet buffer.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] rxq Pointer to Rx queue struture.
 * \param [in] pbuf Pointer to packet buffer structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Allocation failed.
 */
typedef int (*rx_buf_alloc_f)(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                              struct pdma_rx_buf *pbuf);

/*!
 * \brief Get Rx packet buffer DMA address.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] rxq Pointer to Rx queue struture.
 * \param [in] pbuf Pointer to packet buffer structure.
 * \param [in] dma DMA address of packet buffer.
 */
typedef void (*rx_buf_dma_f)(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                             struct pdma_rx_buf *pbuf, dma_addr_t *dma);

/*!
 * \brief Check Rx packet buffer validity.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] rxq Pointer to Rx queue struture.
 * \param [in] pbuf Pointer to packet buffer structure.
 *
 * \retval Ture Buffer is available or FALSE.
 */
typedef int (*rx_buf_avail_f)(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                              struct pdma_rx_buf *pbuf);

/*!
 * \brief Get Rx packet buffer.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] rxq Pointer to Rx queue struture.
 * \param [in] pbuf Pointer to packet buffer structure.
 * \param [in] len Packet length.
 *
 * \retval Pointer to packet header structure or NULL if failed.
 */
typedef struct pkt_hdr *(*rx_buf_get_f)(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                                        struct pdma_rx_buf *pbuf, int len);

/*!
 * \brief Put Rx packet buffer.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] rxq Pointer to Rx queue struture.
 * \param [in] pbuf Pointer to packet buffer structure.
 * \param [in] len Packet length.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Allocation failed.
 */
typedef int (*rx_buf_put_f)(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                            struct pdma_rx_buf *pbuf, int len);

/*!
 * \brief Free Rx packet buffer.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] rxq Pointer to Rx queue struture.
 * \param [in] pbuf Pointer to packet buffer structure.
 */
typedef void (*rx_buf_free_f)(struct pdma_dev *dev, struct pdma_rx_queue *rxq,
                              struct pdma_rx_buf *pbuf);

/*!
 * \brief Get Rx packet buffer mode.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] rxq Pointer to Rx queue struture.
 *
 * \retval Buffer mode.
 */
typedef enum buf_mode (*rx_buf_mode_f)(struct pdma_dev *dev, struct pdma_rx_queue *rxq);

/*!
 * \brief Get Tx packet buffer.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] txq Pointer to Rx queue struture.
 * \param [in] pbuf Pointer to packet buffer structure.
 * \param [in] buf Packet buffer.
 *
 * \retval Pointer to packet header structure or NULL if failed.
 */
typedef struct pkt_hdr *(*tx_buf_get_f)(struct pdma_dev *dev, struct pdma_tx_queue *txq,
                                        struct pdma_tx_buf *pbuf, void *buf);

/*!
 * \brief Get Tx packet buffer DMA address.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] txq Pointer to Rx queue struture.
 * \param [in] pbuf Pointer to packet buffer structure.
 * \param [in] dma DMA address of packet buffer.
 */
typedef void (*tx_buf_dma_f)(struct pdma_dev *dev, struct pdma_tx_queue *txq,
                             struct pdma_tx_buf *pbuf, dma_addr_t *dma);

/*!
 * \brief Free Tx packet buffer.
 *
 * \param [in] dev Pointer to Packet DMA device.
 * \param [in] txq Pointer to Rx queue struture.
 * \param [in] pbuf Pointer to packet buffer structure.
 */
typedef void (*tx_buf_free_f)(struct pdma_dev *dev, struct pdma_tx_queue *txq,
                              struct pdma_tx_buf *pbuf);

/*!
 * \brief Buffer manager.
 */
struct pdma_buf_mngr {
    /*! Allocate descriptor ring buffer */
    ring_buf_alloc_f ring_buf_alloc;

    /*! Free descriptor ring buffer */
    ring_buf_free_f ring_buf_free;

    /*! Allocate Rx packet buffer */
    rx_buf_alloc_f rx_buf_alloc;

    /*! Get Rx packet buffer DMA address */
    rx_buf_dma_f rx_buf_dma;

    /*! Check Rx packet buffer validity */
    rx_buf_avail_f rx_buf_avail;

    /*! Get Rx packet buffer */
    rx_buf_get_f rx_buf_get;

    /*! Put Rx packet buffer */
    rx_buf_put_f rx_buf_put;

    /*! Free Rx packet buffer */
    rx_buf_free_f rx_buf_free;

    /*! Get Rx packet buffer mode */
    rx_buf_mode_f rx_buf_mode;

    /*! Get Tx packet buffer */
    tx_buf_get_f tx_buf_get;

    /*! Get Tx packet buffer DMA address */
    tx_buf_dma_f tx_buf_dma;

    /*! Free Tx packet buffer */
    tx_buf_free_f tx_buf_free;
};

/*!
 * \brief Wait for the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failed.
 */
typedef int (*bcmcnet_vnet_wait_f)(int unit);

/*!
 * \brief Wake up the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failed.
 */
typedef int (*bcmcnet_hnet_wake_f)(int unit);

/*!
 * \brief Dock to the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 * \param [in] vsync Sync data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failed.
 */
typedef int (*bcmcnet_vnet_dock_f)(int unit, vnet_sync_t *vsync);

/*!
 * \brief Undock from the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failed.
 */
typedef int (*bcmcnet_vnet_undock_f)(int unit);

/*!
 * \brief VNET operations.
 */
typedef struct bcmcnet_vnet_ops_s {
    /*!
     * VNET wait for HNET.
     * VNET calls this to wait for any notification from HNET.
     */
    bcmcnet_vnet_wait_f vnet_wait;

    /*!
     * VNET wake up HNET.
     * VNET calls this to notify HNET that Tx/Rx is ready.
     */
    bcmcnet_hnet_wake_f hnet_wake;

    /*!
     * VNET dock to HNET.
     * This is called to notify HNET that VNET is ready to work and synchronize
     * vrings information to HNET.
     */
    bcmcnet_vnet_dock_f vnet_dock;

    /*!
     * VNET undock from HNET.
     * This is called to notify HNET that VNET is ready to leave.
     */
    bcmcnet_vnet_undock_f vnet_undock;
} bcmcnet_vnet_ops_t;

/*!
 * \brief Initialize buffer manager.
 *
 * \param [in] dev Device structure pointer.
 */
extern void
bcmcnet_buf_mngr_init(struct pdma_dev *dev);

/*!
 * \brief Register VNET operations.
 *
 * \param [in] unit Device number.
 * \param [in] vnet_ops VNET operations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 */
extern int
bcmcnet_vnet_ops_register(int unit, bcmcnet_vnet_ops_t *vnet_ops);

#endif /* BCMCNET_INTERNAL_H */

