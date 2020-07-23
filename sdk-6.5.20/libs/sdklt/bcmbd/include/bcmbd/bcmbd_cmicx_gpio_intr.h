/*! \file bcmbd_cmicx_gpio_intr.h
 *
 * Timestamp Interrupt definitions for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_GPIO_INTR_H
#define BCMBD_CMICX_GPIO_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*!
 * \brief Install CMICx GPIO interrupt driver.
 *
 * Initialize GPIO interrupt driver with the default
 * CMICx GPIO interrupt driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_gpio_intr_drv_init(int unit);

/*!
 * \brief Uninstall CMICx GPIO interrupt driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_gpio_intr_drv_cleanup(int unit);

/*!
 * \brief Clean up GPIO interrupt handler.
 *
 * Disable hardware interrupt in the primary interrupt handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_gpio_intr_stop(int unit);

/*!
 * \brief Initialize GPIO interrupt handler.
 *
 * Install GPIO interrupt handler and enable hardware interrupt in the
 * primary interrupt handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_gpio_intr_start(int unit);

#endif /* BCMBD_CMICX_GPIO_INTR_H */
