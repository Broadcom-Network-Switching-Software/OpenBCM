/*! \file bcmbd_pvt_intr.h
 *
 * PVT interrupt driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_PVT_INTR_H
#define BCMBD_PVT_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*!
 * \brief Assign PVT interrupt handler.
 *
 * Assign an interrupt handler for PVT interrupt source.
 *
 * Refer to \ref bcmbd_intr_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num  interrupt source number.
 * \param [in] intr_func interrupt handler function.
 * \param [in] intr_param interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid interrupt source.
 */
extern int
bcmbd_pvt_intr_func_set(int unit, unsigned int intr_num,
                        bcmbd_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Enable PVT interrupt source.
 *
 * Allow PVT interrupt source to generate hardware interrupts.
 *
 * A per-source interrupt handler should be installed before an
 * interrupt is enabled (See \ref bcmbd_pvt_intr_func_set).
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_pvt_intr_enable(int unit, unsigned int intr_num);

/*!
 * \brief Disable PVT interrupt source.
 *
 * Prevent PVT interrupt source from generating hardware
 * interrupts.
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num IPEP interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_pvt_intr_disable(int unit, unsigned int intr_num);

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
extern int
bcmbd_pvt_intr_clear(int unit, unsigned int intr_num);

/*!
 * \brief Start PVT interrupt operation.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num  interrupt source number.
 * \param [in] intr_func interrupt handler function.
 * \param [in] intr_param interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid interrupt source.
 */
extern int
bcmbd_pvt_intr_start(int unit, unsigned int intr_num,
                     bcmbd_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Stop PVT interrupt operation.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_pvt_intr_stop(int unit, unsigned int intr_num);

#endif /* BCMBD_PVT_INTR_H */
