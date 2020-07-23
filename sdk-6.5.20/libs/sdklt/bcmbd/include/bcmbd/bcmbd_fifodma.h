/*! \file bcmbd_fifodma.h
 *
 * FIFODMA data structures and APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_FIFODMA_H
#define BCMBD_FIFODMA_H

#include <sal/sal_types.h>
#include <bcmdrd_config.h>

/*! Maximum number of FIFODMA devices supported */
#define FIFODMA_DEV_NUM_MAX     BCMDRD_CONFIG_MAX_UNITS

/*!
 * \brief FIFODMA work data.
 */
typedef struct bcmbd_fifodma_data_s {
    /*! Base address of FIFO memory */
    uint32_t start_addr;

    /*! Entry width in 32-bit words */
    uint32_t data_width;

    /*! S-Channel header as DMA opcode */
    uint32_t op_code;

    /*! Number of host buffer entries */
    uint32_t num_entries;

    /*! Threshold of host buffer entries to trigger interrupt */
    uint32_t thresh_entries;

    /*! Clocks @ 25MHz from DMA start before triggering a TIMEOUT interrupt */
    uint32_t timeout_cnt;

    /*! Command spacing count for multiple command pending mode */
    uint32_t cmd_spacing;

    /*! Indicates one of 12 FIFO interfaces to be selected for the channel */
    uint32_t fifo_if;
} bcmbd_fifodma_data_t;

/*!
 * \name Work flags.
 * \anchor FIFODMA_WF_xxx
 */

/*! \{ */
/*! Use interrupt mode */
#define FIFODMA_WF_INT_MODE     (1 << 0)

/*! Ignore early ACK, cmd_spacing indicates the wait clocks to send next commands */
#define FIFODMA_WF_IGN_EARLYACK (1 << 1)

/*! Reconnect one FIFO interface for the channel, fifo_if indicates the new one */
#define FIFODMA_WF_NEW_FIFOIF   (1 << 2)
/*! \} */

/*! Function type for FIFODMA operation callback */
typedef void (*fifodma_cb_f)(int unit, void *data);

/*!
 * \brief FIFODMA work.
 *
 * The work information is passed along FIFODMA start function
 * to carry FIFO memory information, callback, the returned
 * buffer cache, etc.
 */
typedef struct bcmbd_fifodma_work_s {
    /*! Work data */
    bcmbd_fifodma_data_t *data;

    /*! Callback if interrupt mode used */
    fifodma_cb_f cb;

    /*! Callback parameter */
    void *cb_data;

    /*! Host buffer cache */
    void *buf_cache;

    /*! Private data */
    void *priv;

    /*! Work flags (\ref FIFODMA_WF_xxx). */
    uint32_t flags;
} bcmbd_fifodma_work_t;

/*!
 * \brief Attach FIFODMA driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_fifodma_attach(int unit);

/*!
 * \brief Detach FIFODMA driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_fifodma_detach(int unit);

/*!
 * \brief Start FIFODMA on a channel.
 *
 * Common entry point for performing a FIFODMA start operation.
 *
 * The function implementation relies on device-specific driver
 * functions for handling FIFODMA hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] chan FIFODMA channel number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_fifodma_start(int unit, int chan, bcmbd_fifodma_work_t *work);

/*!
 * \brief Stop FIFODMA on a channel.
 *
 * Common entry point for performing a FIFODMA stop operation.
 *
 * The function implementation relies on device-specific driver
 * functions for handling FIFODMA hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] chan FIFODMA channel number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_fifodma_stop(int unit, int chan);

/*!
 * \brief Pop FIFODMA on a channel.
 *
 * Common entry point for performing a FIFODMA pop operation.
 *
 * The function implementation relies on device-specific driver
 * functions for handling FIFODMA hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] chan FIFODMA channel number.
 * \param [in] timeout Timeout value in microsecond.
 *
 * \param [out] ptr Ready entry pointer.
 * \param [out] num Ready entries number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_fifodma_pop(int unit, int chan, uint32_t timeout, uint32_t *ptr, uint32_t *num);

#endif /* BCMBD_FIFODMA_H */
