/*! \file bcmbd_gpio_intr.h
 *
 * BD GPIO interrupt driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_GPIO_INTR_H
#define BCMBD_GPIO_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*!
 * \brief Assign per-source GPIO interrupt handler.
 *
 * Assign an interrupt handler for a specific GPIO interrupt source.
 *
 * Refer to \ref bcmbd_intr_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 * \param [in] intr_func Per-source GPIO interrupt handler function.
 * \param [in] intr_param Per-source GPIO interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid interrupt source.
 */
extern int
bcmbd_gpio_intr_func_set(int unit, unsigned int intr_num,
                         bcmbd_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Enable GPIO interrupt source.
 *
 * Allow a given GPIO interrupt source to generate hardware interrupts.
 *
 * A per-source interrupt handler should be installed before an
 * interrupt is enabled (See \ref bcmbd_gpio_intr_func_set).
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_gpio_intr_enable(int unit, unsigned int intr_num);

/*!
 * \brief Disable GPIO interrupt source.
 *
 * Prevent a given GPIO interrupt source from generating hardware interrupts.
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_gpio_intr_disable(int unit, unsigned int intr_num);

/*!
 * \brief Clear GPIO interrupt status.
 *
 * Clear a given GPIO interrupt status.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_gpio_intr_clear(int unit, unsigned int intr_num);

#endif /* BCMBD_GPIO_INTR_H */
