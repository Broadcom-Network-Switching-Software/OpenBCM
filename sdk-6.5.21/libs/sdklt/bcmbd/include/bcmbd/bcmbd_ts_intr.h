/*! \file bcmbd_ts_intr.h
 *
 * BD timesync interrupt driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_TS_INTR_H
#define BCMBD_TS_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*!
 * \brief Assign per-source timesync interrupt handler.
 *
 * Assign an interrupt handler for a specific timesync interrupt
 * source.
 *
 * Refer to \ref bcmbd_intr_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num timesync interrupt source number.
 * \param [in] intr_func Per-source timesync interrupt handler function.
 * \param [in] intr_param Per-source timesync interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid interrupt source.
 */
extern int
bcmbd_ts_intr_func_set(int unit, unsigned int intr_num,
                       bcmbd_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Enable timesync interrupt source.
 *
 * Allow a given timesync interrupt source to generate hardware
 * interrupts.
 *
 * A per-source interrupt handler should be installed before an
 * interrupt is enabled (See \ref bcmbd_ts_intr_func_set).
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_ts_intr_enable(int unit, unsigned int intr_num);

/*!
 * \brief Disable timesync interrupt source.
 *
 * Prevent a given timesync interrupt source from generating hardware
 * interrupts.
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num timesync interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_ts_intr_disable(int unit, unsigned int intr_num);

/*!
 * \brief Clear timesync interrupt source.
 *
 * Clear a given timesysnc interrupt status.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_ts_intr_clear(int unit, unsigned int intr_num);

#endif /* BCMBD_TS_INTR_H */
