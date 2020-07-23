/*! \file bcmbd_sw_intr_internal.h
 *
 * BD software interrupt driver internal API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_SW_INTR_INTERNAL_H
#define BCMBD_SW_INTR_INTERNAL_H

#include <bcmbd/bcmbd_sw_intr.h>

/*!
 * \brief Install device-specific software interrupt driver.
 *
 * Install device-specific software interrupt driver into top-level
 * software interrupt API.
 *
 * Use \c sw_intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] sw_intr_drv Software interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_sw_intr_drv_init(int unit, const bcmbd_intr_drv_t *sw_intr_drv);

#endif /* BCMBD_SW_INTR_INTERNAL_H */
