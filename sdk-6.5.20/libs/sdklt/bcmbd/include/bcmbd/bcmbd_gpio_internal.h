/*! \file bcmbd_gpio_internal.h
 *
 * BD GPIO internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_GPIO_INTERNAL_H
#define BCMBD_GPIO_INTERNAL_H

#include <bcmbd/bcmbd_gpio.h>

/*!
 * \brief See \ref bcmbd_gpio_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_MEMORY Insufficient resources.
 */
typedef int (*bcmbd_gpio_init_f)(int unit);

/*!
 * \brief See \ref bcmbd_gpio_cleanup.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmbd_gpio_cleanup_f)(int unit);

/*!
 * \brief See \ref bcmbd_gpio_mode_set.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio GPIO number.
 * \param [in] mode GPIO mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
typedef int (*bcmbd_gpio_mode_set_f)(int unit, uint32_t gpio,
                                     bcmbd_gpio_mode_t mode);

/*!
 * \brief See \ref bcmbd_gpio_mode_get.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio GPIO number.
 * \param [out] mode GPIO mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
typedef int (*bcmbd_gpio_mode_get_f)(int unit, uint32_t gpio,
                                     bcmbd_gpio_mode_t *mode);

/*!
 * \brief See \ref bcmbd_gpio_set.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio GPIO number.
 * \param [in] val GPIO value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
typedef int (*bcmbd_gpio_set_f)(int unit, uint32_t gpio, bool val);

/*!
 * \brief See \ref bcmbd_gpio_get.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio GPIO number.
 * \param [out] val GPIO value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
typedef int (*bcmbd_gpio_get_f)(int unit, uint32_t gpio, bool *val);

/*!
 * \brief GPIO driver object.
 */
typedef struct bcmbd_gpio_drv_s {

    /*! Initialize GPIO driver. */
    bcmbd_gpio_init_f init;

    /*! Clean up GPIO driver. */
    bcmbd_gpio_cleanup_f cleanup;

    /*! Set GPIO mode. */
    bcmbd_gpio_mode_set_f mode_set;

    /*! Get GPIO mode. */
    bcmbd_gpio_mode_get_f mode_get;

    /*! Set GPIO value. */
    bcmbd_gpio_set_f set;

    /*! Get GPIO value. */
    bcmbd_gpio_get_f get;

} bcmbd_gpio_drv_t;

/*!
 * \brief Assign device-specific GPIO driver.
 *
 * \param [in] unit Unit number.
 * \param [in] gpio_drv Driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_gpio_drv_init(int unit, const bcmbd_gpio_drv_t *gpio_drv);

/*!
 * \brief Initialize GPIO driver.
 *
 * Allocates locks and other resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_MEMORY Insufficient resources.
 */
extern int
bcmbd_gpio_init(int unit);

/*!
 * \brief Clean up GPIO driver.
 *
 * Free resources allocated by \ref bcmbd_gpio_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_gpio_cleanup(int unit);

#endif /* BCMBD_GPIO_INTERNAL_H */
