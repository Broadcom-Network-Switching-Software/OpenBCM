/*! \file bcmbd_tsfifo_internal.h
 *
 * BD TimeSync FIFO driver internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_TSFIFO_INTERNAL_H
#define BCMBD_TSFIFO_INTERNAL_H

#include <bcmbd/bcmbd_tsfifo.h>

/*!
 * \brief Start capturing TimeSync events to file.
 *
 * \ref bcmbd_tsfifo_capture_start.
 *
 * \param [in] unit The unit number of the device for which to start
 * capturing.
 * \param [in] cb A data structure that provides the IO
 * instrumentation required by the capture functionality. This functionality
 * is implemented by the application.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_BUSY Capturing activity is running for this unit.
 */
typedef int
(*bcmbd_tsfifo_capture_start_f)(int unit, bcmbd_tsfifo_capture_cb_t *cb);

/*!
 * \brief Stop capturing TimeSync events to file.
 *
 * \ref bcmbd_tsfifo_capture_stop.
 *
 * \param [in] unit The unit number of the device for which to stop
 * capturing
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int
(*bcmbd_tsfifo_capture_stop_f)(int unit);

/*!
 * \brief Dump the captured TimeSync events from file.
 *
 * \ref bcmbd_tsfifo_capture_dump.
 *
 * \param [in] unit The unit number of the device.
 * \param [in] cb Callback function to be called by the dump engine for
 * every entry. This function may use other APIs to retrieve and display
 * the content of the entry in a human readable format.
 * See \ref bcmbd_tsfifo_dump_action_f for more information.
 * \param [in] app_cb The app_cb is a data structure that provides the IO
 * instrumentation required by the capture dump function. This functionality
 * is implemented by the application.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid app_cb parameter.
 */
typedef int
(*bcmbd_tsfifo_capture_dump_f)(
        bcmbd_tsfifo_dump_action_f *cb,
        bcmbd_tsfifo_dump_cb_t *app_cb);

/*!
 * \brief See \ref bcmbd_tsfifo_cleanup.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmbd_tsfifo_cleanup_f)(int unit);

/*!
 * \brief See \ref bcmbd_tsfifo_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmbd_tsfifo_init_f)(int unit);

/*!
 * \brief Generic TS FIFO driver object.
 */
typedef struct bcmbd_tsfifo_drv_s {

    /*! Start capturing TimeSync events to file */
    bcmbd_tsfifo_capture_start_f capture_start;

    /*! Stop capturing TimeSync events to file */
    bcmbd_tsfifo_capture_stop_f capture_stop;

    /*! Dump the captured TimeSync events from file */
    bcmbd_tsfifo_capture_dump_f capture_dump;

    /*! Clean up tsfifo driver */
    bcmbd_tsfifo_cleanup_f cleanup;

    /*! Initialize tsfifo driver */
    bcmbd_tsfifo_init_f init;
} bcmbd_tsfifo_drv_t;


/*!
 * \brief Install device-specific timesync TSFIFO driver.
 *
 * Install device-specific timesync TSFIFO driver.
 *
 * Use \c tsfifo_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] tsfifo_drv Timesync TSFIFO driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_tsfifo_drv_init(int unit, const bcmbd_tsfifo_drv_t *tsfifo_drv);
#endif /* BCMBD_TSFIFO_INTERNAL_H */
