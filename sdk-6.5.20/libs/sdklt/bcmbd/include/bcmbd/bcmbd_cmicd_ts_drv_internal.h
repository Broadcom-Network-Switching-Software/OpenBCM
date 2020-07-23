/*! \file bcmbd_cmicd_ts_drv_internal.h
 *
 * Local driver functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICD_TS_DRV_INTERNAL_H
#define BCMBD_CMICD_TS_DRV_INTERNAL_H

/*!
 * \brief Install CMICd TimeSync driver.
 *
 * Install CMICd TimeSync driver into top-level TimeSync API.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 */
extern int
bcmbd_cmicd_ts_drv_init(int unit);

/*!
 * \brief Cleanup the installed CMICd TimeSync driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 */
extern int
bcmbd_cmicd_ts_drv_cleanup(int unit);

#endif /* BCMBD_CMICD_TS_DRV_INTERNAL_H */
