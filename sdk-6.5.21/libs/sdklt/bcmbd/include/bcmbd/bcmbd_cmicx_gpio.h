/*! \file bcmbd_cmicx_gpio.h
 *
 * Default CMICx GPIO driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_GPIO_H
#define BCMBD_CMICX_GPIO_H

#include <bcmbd/bcmbd_gpio_internal.h>

/*! Number of GPIO pins. */
#define BCMBD_CMICX_NUM_GPIO 32

/*!
 * \brief Install CMICx GPIO driver.
 *
 * Initialize GPIO driver with the default CMICx GPIO driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_gpio_drv_init(int unit);

/*!
 * \brief Uninstall CMICx GPIO driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_gpio_drv_cleanup(int unit);

#endif /* BCMBD_CMICX_GPIO_H */
