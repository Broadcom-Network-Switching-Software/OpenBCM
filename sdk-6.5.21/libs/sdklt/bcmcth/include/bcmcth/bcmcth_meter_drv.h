/*! \file bcmcth_meter_drv.h
 *
 * BCMCTH Meter top-level APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_DRV_H
#define BCMCTH_METER_DRV_H

/*!
 * \brief Initialize METER driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_meter_drv_init(int unit);

/*!
 * \brief Cleanup METER driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_meter_drv_cleanup(int unit);


#endif /* BCMCTH_METER_DRV_H */
