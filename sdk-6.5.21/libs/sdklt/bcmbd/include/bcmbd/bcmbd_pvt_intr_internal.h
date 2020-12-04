/*! \file bcmbd_pvt_intr_internal.h
 *
 * BD pvt interrupt driver internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_PVT_INTR_INTERNAL_H
#define BCMBD_PVT_INTR_INTERNAL_H

#include <bcmbd/bcmbd_pvt_intr.h>

/*!
 * \brief Install device-specific pvt interrupt driver.
 *
 * Install device-specific pvt interrupt driver into top-level pvt
 * interrupt API.
 *
 * Use \c intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_drv pvt interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_pvt_intr_drv_init(int unit, bcmbd_intr_drv_t *intr_drv);

#endif /* BCMBD_PVT_INTR_INTERNAL_H */
