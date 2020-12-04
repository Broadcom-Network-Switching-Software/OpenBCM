/*! \file bcmbd_pvt_internal.h
 *
 * PVT internal function definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_PVT_INTERNAL_H
#define BCMBD_PVT_INTERNAL_H

#include <bcmbd/bcmbd_pvt.h>

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
typedef int (*bcmbd_pvt_event_func_set_f)(int unit, bcmbd_pvt_event_cb_f func);

/*!
 * \brief Per-unit get device temperature handler.
 *
 * \param [in] unit     Unit number.
 * \param [in] size     Buffer size.
 * \param [in] equation Specify equation used for data conversion.
 * \param [out] readout Buffer to hold readout data.
 * \param [out] num_rec Number of sensors supported from device.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval SHR_E_PARAM Size and num_rec not match.
 */
typedef int (*bcmbd_pvt_temp_get_f)(int unit,
                                    size_t size,
                                    bcmbd_pvt_equation_t equation,
                                    bcmbd_pvt_temp_rec_t *readout,
                                    size_t *num_rec);

/*! Generic PVT driver object. */
typedef struct bcmbd_pvt_drv_s {

    /*! Handler for event callback function set. */
    bcmbd_pvt_event_func_set_f event_func_set;

    /*! Handler for die temperature get. */
    bcmbd_pvt_temp_get_f temp_get;

} bcmbd_pvt_drv_t;

/*!
 * \brief Install device-specific PVT driver.
 *
 * \param [in] unit Unit number.
 * \param [in] drv PVT driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_pvt_drv_init(int unit, bcmbd_pvt_drv_t *drv);


#endif /* BCMBD_PVT_INTERNAL_H */
