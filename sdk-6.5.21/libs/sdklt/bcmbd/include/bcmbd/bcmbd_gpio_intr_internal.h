/*! \file bcmbd_gpio_intr_internal.h
 *
 * BD GPIO interrupt driver internal API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_GPIO_INTR_INTERNAL_H
#define BCMBD_GPIO_INTR_INTERNAL_H

#include <bcmbd/bcmbd_gpio_intr.h>

/*!
 * \brief Install device-specific GPIO interrupt driver.
 *
 * Install device-specific GPIO interrupt driver into top-level
 * GPIO interrupt API.
 *
 * Use \c gpio_intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio_intr_drv GPIO interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_gpio_intr_drv_init(int unit, const bcmbd_intr_drv_t *gpio_intr_drv);

#endif /* BCMBD_GPIO_INTR_INTERNAL_H */
