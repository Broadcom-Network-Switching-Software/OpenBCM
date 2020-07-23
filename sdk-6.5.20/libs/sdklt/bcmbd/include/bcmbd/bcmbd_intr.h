/*! \file bcmbd_intr.h
 *
 * Common interrupt handler definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_INTR_H
#define BCMBD_INTR_H

#include <bcmbd/bcmbd.h>

/*!
 * \brief Per-source interrupt handler.
 *
 * A per-source interrupt handler must be defined for each of enabled
 * interrupt sources (corresponding to the individual bits in the
 * interrupt status registers).
 *
 * When a per-source interrupt handler is installed, an associated
 * context value is passed in as well, and this context value will be
 * passed to the interrupt handler, when an interrupt occurs.
 *
 * A typical use case for the context value is to allow the interrupt
 * handler to separate interrupt sources from each other, when the
 * same handler is assigned to multiple interrupt sources of the same
 * type.
 *
 * For example, if same handler serves multiple DMA channels of the
 * same type, the context value can indicate the DMA channel number.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_param Additional interrupt context.
 *
 * \return Nothing.
 */
typedef void (*bcmbd_intr_f)(int unit, uint32_t intr_param);

/*!
 * \brief Assign per-source interrupt handler.
 *
 * Assign an interrupt handler for a specific interrupt source.
 *
 * Refer to \ref bcmbd_intr_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 * \param [in] intr_func Per-source interrupt handler function.
 * \param [in] intr_param Per-source interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid interrupt source.
 */
typedef int (*bcmbd_intr_func_set_f)(int unit, unsigned int intr_num,
                                     bcmbd_intr_f func, uint32_t param);

/*!
 * \brief Enable interrupt source.
 *
 * Allow a given interrupt source to generate hardware interrupts.
 *
 * A per-source interrupt handler should be installed before an
 * interrupt is enabled (See \ref bcmbd_intr_func_set_f).
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
typedef void (*bcmbd_intr_enable_f)(int unit, unsigned int intr_num);

/*!
 * \brief Disable interrupt source.
 *
 * Prevent a given interrupt source from generating hardware
 * interrupts.
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
typedef void (*bcmbd_intr_disable_f)(int unit, unsigned int intr_num);

/*!
 * \brief Clear interrupt source.
 *
 * Some interrupts must be cleared explicitly in the interrupt status
 * register after the interrupt condition has been resolved.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
typedef void (*bcmbd_intr_clear_f)(int unit, unsigned int intr_num);

/*!
 * \brief Generic interrupt driver object.
 *
 * This driver object is mainly intended for secod-level interrupt
 * handlers where a sub-system uses a single hardware interrupt source
 * for multiple types of events.
 */
typedef struct bcmbd_intr_drv_s {

    /*! Install interrupt handler for a specific source/event. */
    bcmbd_intr_func_set_f intr_func_set;

    /*! Enable interrupts for a specific source/event. */
    bcmbd_intr_enable_f intr_enable;

    /*! Disable interrupts for a specific source/event. */
    bcmbd_intr_disable_f intr_disable;

    /*! Clear interrupt for a specific source/event. */
    bcmbd_intr_clear_f intr_clear;

} bcmbd_intr_drv_t;

#endif /* BCMBD_INTR_H */
