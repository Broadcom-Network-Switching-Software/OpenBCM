/*! \file bcmbd_ts_intr_internal.h
 *
 * BD timesync interrupt driver internal API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_TS_INTR_INTERNAL_H
#define BCMBD_TS_INTR_INTERNAL_H

#include <bcmbd/bcmbd_ts_intr.h>

/*!
 * \brief Install device-specific timesync interrupt driver.
 *
 * Install device-specific timesync interrupt driver into top-level
 * timesync interrupt API.
 *
 * Use \c ts_intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_intr_drv Timesync interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_ts_intr_drv_init(int unit, const bcmbd_intr_drv_t *ts_intr_drv);

#endif /* BCMBD_TS_INTR_INTERNAL_H */
