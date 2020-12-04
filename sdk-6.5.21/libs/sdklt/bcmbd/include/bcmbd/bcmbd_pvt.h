/*! \file bcmbd_pvt.h
 *
 * PVT APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_PVT_H
#define BCMBD_PVT_H


/*!
 * \brief PVT monitor data to thermal conversion equation.
 */
typedef enum bcmbd_pvt_equation_e {
    /*! PVT conversion equation 1. */
    PVT_EQUATION_1 = 1,

    /*! PVT conversion equation 2. */
    PVT_EQUATION_2 = 2,

    /*! PVT conversion equation 3. */
    PVT_EQUATION_3 = 3,

} bcmbd_pvt_equation_t;

/*!
 * \brief PVT monitor raised alarm event type.
 */
typedef enum bcmbd_pvt_event_e {
    /*! High temperature protection SW interrupt. */
    PVT_EVENT_HIGH_TEMP = 1,

    /*! High temperature protection watchdog HW reset. */
    PVT_EVENT_HW_RESET  = 2,

} bcmbd_pvt_event_t;

/*! Temperature data type. */
typedef double bcmbd_temperature_t;

/*!
 * \brief Data structure to hold temperature record.
 */
typedef struct bcmbd_pvt_temp_rec_s {
    /*! Current temperature. */
    bcmbd_temperature_t current;

    /*! Historical minimum temperature. */
    bcmbd_temperature_t min;

    /*! Historical maximum temperature. */
    bcmbd_temperature_t max;

} bcmbd_pvt_temp_rec_t;

/*!
 * \brief Data structure to hold AVS sensor record.
 */
typedef struct bcmbd_avs_s {
    /*! Current readout. */
    int32_t current;

    /*! Historical minimum readout. */
    int32_t min;

    /*! Historical maximum readout. */
    int32_t max;

} bcmbd_avs_t;

/*!
 * \brief Per-uint PVT event handler.
 *
 * If a per-unit PVT event handler is installed, when PVT high temperature
 * protection happens, PVT interrupt handler will call this event handler
 * in thread context to notify upper layer application.
 *
 * The event handler can get event type, index of the monitor on which event
 * happened, and current temperature from input params. Do not need to clear
 * the interrupt conditon in this event handler, as PVT interrupt handler
 * will do it.
 *
 * \param [in] unit Unit number.
 * \param [in] event PVT event type.
 * \param [in] index PVT monitor index.
 * \param [in] param Additional PVT event context.
 *
 * \return Nothing.
 */
typedef void (*bcmbd_pvt_event_cb_f)(int unit,
                                     bcmbd_pvt_event_t event,
                                     uint32_t index,
                                     uint32_t param);

/*!
 * \brief Assign per-unit PVT event handler.
 *
 * Assign an event callback function for a specific unit.
 *
 * Refer to \ref bcmbd_pvt_event_cb_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] func Per-unit PVT event callback function.
 *
 * \retval SHR_E_NONE Function successfully installed.
 */
extern int
bcmbd_pvt_event_func_set(int unit, bcmbd_pvt_event_cb_f func);

/*!
 * \brief Get thermal data from PVT monitor.
 *
 * Each thermal sensor will return a set of temperature readouts.
 * Number of sensors are device specific.
 *
 * API usage:
 *
 * Step 1: Call API first time with NULL pointer and O size
 *         to query device about number of sensors supported.
 *
 * Step 2: Based on sensor number, allocates buffer accordingly. Call
 *         API second time with actual sensor number and allocated buffer
 *         to collect device readout.
 *
 * \param[in]  unit     Unit number.
 * \param[in]  size     Buffer size.
 * \param[in]  equation Specify conversion equation of read out data.
 * \param[out] readout  Buffer to hold readout temperature data.
 * \param[out] num_rec  Number of sensors supported by device.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval SHR_E_PARAM Size and num_rec not match.
 */
extern int
bcmbd_pvt_temperature_get(int unit,
                          size_t size,
                          bcmbd_pvt_equation_t equation,
                          bcmbd_pvt_temp_rec_t *readout,
                          size_t *num_rec);

#endif /* BCMBD_PVT_H */
