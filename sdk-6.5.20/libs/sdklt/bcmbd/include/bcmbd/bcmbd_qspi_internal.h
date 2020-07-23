/*! \file bcmbd_qspi_internal.h
 *
 * BD QSPI internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_QSPI_INTERNAL_H
#define BCMBD_QSPI_INTERNAL_H

#include <bcmbd/bcmbd_qspi.h>

/*!
 * Initialize QSPI interface.
 *
 * \param [in] unit Unit number.
 * \param [in] conf Parameters for initialization.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_MEMORY Insufficient resources.
 */
typedef int (*bcmbd_qspi_init_f)(int unit, bcmbd_qspi_conf_t *conf);

/*!
 * Deinitialize QSPI interface.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmbd_qspi_cleanup_f)(int unit);

/*!
 * \brief Write data to QSPI device.
 *
 * \param [in] unit Unit number.
 * \param [in] flag Transfer flag. \ref BCMBD_QSPI_TRANS_.
 * \param [in] tx_bytes Bytes to transmit.
 * \param [in] tx Transmit buffer.
 * \param [in] rx_bytes Bytes to receive.
 * \param [out] rx Receive buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation timeout.
 */
typedef int (*bcmbd_qspi_transfer_f)(int unit, uint32_t flag,
                                     size_t tx_bytes, uint8_t *tx,
                                     size_t rx_bytes, uint8_t *rx);

/*!
 * \brief QSPI driver object.
 */
typedef struct bcmbd_qspi_drv_s {

    /*! Initialize QSPI interface. */
    bcmbd_qspi_init_f init;

    /*! Cleanup QSPI interface. */
    bcmbd_qspi_cleanup_f cleanup;

    /*! Transfer QSPI message. */
    bcmbd_qspi_transfer_f transfer;

} bcmbd_qspi_drv_t;


/*!
 * Initialize QSPI driver.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver for the given device.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_qspi_drv_init(int unit, bcmbd_qspi_drv_t *drv);

#endif /* BCMBD_QSPI_INTERNAL_H */
