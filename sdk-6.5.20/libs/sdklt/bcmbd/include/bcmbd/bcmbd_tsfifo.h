/*! \file bcmbd_tsfifo.h
 *
 * BD timesync FIFO driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_TSFIFO_H
#define BCMBD_TSFIFO_H

/*!
 * \brief The entry content of TS FIFO event.
 */
typedef struct bcmbd_tsfifo_capture_info_s {
    /*! Device unit */
    int         unit;
    /*! Event ID */
    uint8_t     evt_id;
    /*! Value should be added to TS0/TS1 to get the accurate timestamp. */
    uint8_t     offset;
    /*! TS0 timestamp value from FIFO */
    uint64_t    raw0;
    /*! TS1 timestamp value from FIFO */
    uint64_t    raw1;
    /*! Compensated TS0 timestamp in picosecond */
    uint64_t      ts0;
    /*! Compensated TS1 timestamp in picosecond */
    uint64_t      ts1;
} bcmbd_tsfifo_capture_info_t;

/*!
 * \brief BCMBD TS FIFO operations capture callback structure.
 *
 * This data structure should be passed to the capture start function. The
 * purpose of this structure is to enable the capture function to perform
 * IO operations that are outside the scope of the SDK. These operations
 * includes in particular the ability to save the captured data into
 * non-volatile memory where it can be read back at a later time.
 */
typedef struct bcmbd_tsfifo_capture_cb_s {
    /*! File descriptor of a file with write permission */
    void *fd;
    /*!
      * Write function to write a number of bytes (nbyte) from the buffer into
      * the file using the file descriptor fd. The function returns the number
      * of bytes that were actually written.
      */
    uint32_t (*write)(void *fd, void *buffer, uint32_t nbyte);
} bcmbd_tsfifo_capture_cb_t;

/*!
 * \brief BCMBD TS FIFO operations dump callback structure.
 *
 * This data structure should be passed to the dump function. The
 * purpose of this structure is to enable the dump function to perform
 * IO operations that are outside the scope of the SDK. These operations
 * includes in particular reading the captured data.
 */
typedef struct bcmbd_tsfifo_dump_cb_s {
    /*! File descriptor of a file to dump with read permission */
    void *fd;
    /*!
      * Read function to read number of bytes (nbyte) from the file into
      * the buffer. The function returns the number of bytes that were
      * actually read.
      */
    uint32_t (*read)(void *fd, void *buffer, uint32_t nbyte);
} bcmbd_tsfifo_dump_cb_t;

/*!
 * \brief Start capturing TS FIFO events.
 *
 * This function starts the capture of all TS FIFO events for a given unit.
 * The capture of the activity will continue until the application calls
 * \ref bcmbd_tsfifo_capture_stop(). After that the application can dump
 * the entire captured entries by calling \ref bcmbd_tsfifo_capture_dump().
 * The application should provide the file descriptor and a write function to
 * be used to store the trace information. If the file descriptor represents
 * a file the application should make sure to truncate older file content.
 * Note that the content of the file will be binary data.
 *
 * \param [in] unit The unit number of the device for which to start
 * capturing.
 * \param [in] app_cb A data structure that provides the IO
 * instrumentation required by the capture functionality. This functionality
 * is implemented by the application.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_BUSY Capturing activity is running for this unit.
 */
extern int
bcmbd_tsfifo_capture_start(int unit, bcmbd_tsfifo_capture_cb_t *app_cb);

/*!
 * \brief Stop capturing TS FIFO events.
 *
 * This function should stop operations capturing on the specified unit.
 * The operations capture should start by calling
 * \ref bcmbd_tsfifo_capture_start().
 * Note that calling \ref bcmbd_tsfifo_capture_start() again after calling
 * this function will erase the entire captured history for this device.
 *
 * \param [in] unit The unit number of the device for which to stop
 * capturing
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 */

extern int
bcmbd_tsfifo_capture_stop(int unit);

/*!
 * \brief Pre-Commit notification for captured information.
 *
 * This callback function can be passed to the function
 * \ref bcmbd_tsfifo_capture_dump to receive the captured information
 * that is going to be dumped.
 */
typedef void (bcmbd_tsfifo_dump_action_f)(
        int unit, bcmbd_tsfifo_capture_info_t *cap_info);

/*!
 * \brief Dump previously captured operations.
 *
 * This function will dump all the timesync events that was previously captured
 * using \ref bcmbd_tsfifo_capture_start() and \ref bcmbd_tsfifo_capture_stop()
 * functions.
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
extern int
bcmbd_tsfifo_capture_dump(int unit,
                          bcmbd_tsfifo_dump_action_f *cb,
                          bcmbd_tsfifo_dump_cb_t *app_cb);

/*!
 * \brief Clean up tsfifo driver.
 *
 * Clean up resource and restore HW configure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_tsfifo_cleanup(int unit);

/*!
 * \brief Initialize tsfifo driver.
 *
 * Initialize resource and HW configure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_tsfifo_init(int unit);
#endif /* BCMBD_TSFIFO_H */
