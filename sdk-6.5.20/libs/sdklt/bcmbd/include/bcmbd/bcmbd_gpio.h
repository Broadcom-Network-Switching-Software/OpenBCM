/*! \file bcmbd_gpio.h
 *
 * GPIO Base Driver (BD) APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_GPIO_H
#define BCMBD_GPIO_H

#include <bcmdrd/bcmdrd_types.h>

/*! Types of GPIO directions. */
typedef enum bcmbd_gpio_mode_e {

    /*! Input mode. */
    BCMBD_GPIO_MODE_IN,

    /*! Output mode. */
    BCMBD_GPIO_MODE_OUT

} bcmbd_gpio_mode_t;

/*!
 * \brief Configure GPIO mode.
 *
 * This function is used to configure the mode of the given GPIO pin.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio GPIO pin number.
 * \param [in] mode GPIO mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
extern int
bcmbd_gpio_mode_set(int unit, uint32_t gpio, bcmbd_gpio_mode_t mode);

/*!
 * \brief Get GPIO mode.
 *
 * This function is used to get the mode of the given GPIO pin.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio GPIO pin number.
 * \param [out] mode GPIO mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
extern int
bcmbd_gpio_mode_get(int unit, uint32_t gpio, bcmbd_gpio_mode_t *mode);

/*!
 * \brief Set GPIO value.
 *
 * This function is used to set the value of the given GPIO pin.
 *
 * If \c gpio is configured as input, then this function has no effect.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio GPIO pin number.
 * \param [in] val GPIO value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
extern int
bcmbd_gpio_set(int unit, uint32_t gpio, bool val);

/*!
 * \brief Get GPIO value.
 *
 * This function is used to get the value of the given GPIO pin.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio GPIO pin number.
 * \param [out] val GPIO value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
extern int
bcmbd_gpio_get(int unit, uint32_t gpio, bool *val);

#endif /* BCMBD_GPIO_H */
