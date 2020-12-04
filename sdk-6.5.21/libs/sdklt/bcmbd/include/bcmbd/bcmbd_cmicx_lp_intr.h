/*! \file bcmbd_cmicx_lp_intr.h
 *
 * Low-priotiry interrupt definitions for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_LP_INTR_H
#define BCMBD_CMICX_LP_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*! \{
 *
 * \brief Low-priority interrupt source base offsets.
 *
 * All interrupt sources have a unique value, which is derived from a
 * base offset and a bit number in an interrupt status register.
 *
 * The low-priority interrupt sources are chip-specific.
 */
#define LP_INTR_BASE0                   (0 * 32)
#define LP_INTR_BASE1                   (1 * 32)
#define LP_INTR_BASE2                   (2 * 32)
#define LP_INTR_BASE3                   (3 * 32)
/*! \} */

/*!
 * \brief Low-priority interrupt handler function.
 *
 * This function must be installed as the handler for the low-priority
 * interrupts at the primary interrupt handler. Normally this is done
 * viA \ref bcmbd_cmicx_lp_intr_start.
 *
 * \param [in] unit Unit number.
 * \param [in] param Interrupt handler context from client.
 */
void
bcmbd_cmicx_lp_intr(int unit, uint32_t param);

/*!
 * \brief Assign low-priority interrupt handler.
 *
 * Assign an interrupt handler for a specific low-priority interrupt
 * source.
 *
 * Refer to \ref bcmbd_intr_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] lp_intr_num Low-priority interrupt source number.
 * \param [in] intr_func Per-source interrupt handler function.
 * \param [in] intr_param Per-source interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid interrupt source.
 */
extern int
bcmbd_cmicx_lp_intr_func_set(int unit, unsigned int lp_intr_num,
                             bcmbd_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Enable low-priority interrupt source.
 *
 * Allow a given low-priority interrupt source to generate hardware
 * interrupts.
 *
 * A per-source interrupt handler should be installed before an
 * interrupt is enabled (\ref bcmbd_cmicx_lp_intr_func_set).
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] lp_intr_num Low-priority interrupt source number.
 *
 * \return Nothing.
 */
extern void
bcmbd_cmicx_lp_intr_enable(int unit, int lp_intr_num);

/*!
 * \brief Disable low-priority interrupt source.
 *
 * Prevent a given low-priority interrupt source from generating
 * hardware interrupts.
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] lp_intr_num Low-priority interrupt source number.
 *
 * \return Nothing.
 */
extern void
bcmbd_cmicx_lp_intr_disable(int unit, int lp_intr_num);

/*!
 * \brief Disable low-priority interrupt from interrupt handler context.
 *
 * Prevent a given low-priority interrupt source from generating
 * hardware interrupts.
 *
 * Should only be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] lp_intr_num Low-priority interrupt source number.
 *
 * \return Nothing.
 */
extern void
bcmbd_cmicx_lp_intr_disable_nosync(int unit, int lp_intr_num);

/*!
 * \brief Clear all low-priority interrupt register configurations.
 *
 * Remove all low-priority interrupt register configurations created
 * using \ref bcmbd_cmicx_lp_intr_reg_set.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_lp_intr_regs_clr(int unit);

/*!
 * \brief Add low-priority interrupt register configuration.
 *
 * Add an entry to the low-priority interrupt register set table.
 *
 * This function is normally not called directly. Use \ref
 * bcmbd_cmicx_lp_intr_regs_init instead.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_base Interrupt source base for these registers.
 * \param [in] status_offs Interrupt status register offset.
 * \param [in] mask_offs Interrupt mask register offset.
 *
 * \retval 0 No errors
 */
extern int
bcmbd_cmicx_lp_intr_reg_set(int unit, unsigned int intr_base,
                            uint32_t status_offs, uint32_t mask_offs);

/*!
 * \brief Configure low-priority interrupt status and mask registers.
 *
 * This function will configure all the required the low-priority
 * interrupt status and mask registers for a device using \ref
 * bcmbd_cmicx_lp_intr_reg_set.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_lp_intr_regs_init(int unit);

/*!
 * \brief Initialize low-priority interrupt sub-system.
 *
 * This is a convenience function that initializes low-priority
 * interrupt register controls and other software structures. No
 * hardware access is performed by this function.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_lp_intr_init(int unit);

/*!
 * \brief Clean up low-priority interrupt handler.
 *
 * Disable hardware interrupt in the primary interrupt handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_lp_intr_stop(int unit);

/*!
 * \brief Initialize low-priority interrupt handler.
 *
 * Install low-priority interrupt handler and enable hardware
 * interrupt in the primary interrupt handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_lp_intr_start(int unit);

#endif /* BCMBD_CMICX_LP_INTR_H */
