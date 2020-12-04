/*! \file bcmbd_cmicd_intr.h
 *
 * Interrupt definitions for CMICd v2.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICD_INTR_H
#define BCMBD_CMICD_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*! \{
 *
 * \brief Interrupt source base offsets.
 *
 * All interrupt sources have a unique value, which is derived from a
 * base offset and a bit number in an interrupt status register.
 */
#define CMICD_IRQ_BASE0                 (0 * 32)
#define CMICD_IRQ_BASE1                 (1 * 32)
#define CMICD_IRQ_BASE2                 (2 * 32)
#define CMICD_IRQ_BASE3                 (3 * 32)
#define CMICD_IRQ_BASE4                 (4 * 32)
#define CMICD_IRQ_BASE5                 (5 * 32)
#define CMICD_IRQ_BASE6                 (6 * 32)
/*! \} */

/*! \{
 *
 * \brief Interrupt sources in the CMIC_CMCx_IRQ_MASKx registers.
 * \anchor CMICD_IRQ_xxx
 *
 * Please refer to the hardware documentation for details.
 */
#define CMICD_IRQ_SBUSDMA_CH1_DONE      (CMICD_IRQ_BASE0 + 0)
#define CMICD_IRQ_SBUSDMA_CH0_DONE      (CMICD_IRQ_BASE0 + 1)
#define CMICD_IRQ_FIFO_CH3_DMA_INTR     (CMICD_IRQ_BASE0 + 2)
#define CMICD_IRQ_FIFO_CH2_DMA_INTR     (CMICD_IRQ_BASE0 + 3)
#define CMICD_IRQ_FIFO_CH1_DMA_INTR     (CMICD_IRQ_BASE0 + 4)
#define CMICD_IRQ_FIFO_CH0_DMA_INTR     (CMICD_IRQ_BASE0 + 5)
#define CMICD_IRQ_SBUSDMA_CH2_DONE      (CMICD_IRQ_BASE0 + 6)
#define CMICD_IRQ_MIIM_OP_DONE          (CMICD_IRQ_BASE0 + 7)
#define CMICD_IRQ_CH3_DESC_DONE         (CMICD_IRQ_BASE0 + 8)
#define CMICD_IRQ_CH3_CHAIN_DONE        (CMICD_IRQ_BASE0 + 9)
#define CMICD_IRQ_CH2_DESC_DONE         (CMICD_IRQ_BASE0 + 10)
#define CMICD_IRQ_CH2_CHAIN_DONE        (CMICD_IRQ_BASE0 + 11)
#define CMICD_IRQ_CH1_DESC_DONE         (CMICD_IRQ_BASE0 + 12)
#define CMICD_IRQ_CH1_CHAIN_DONE        (CMICD_IRQ_BASE0 + 13)
#define CMICD_IRQ_CH0_DESC_DONE         (CMICD_IRQ_BASE0 + 14)
#define CMICD_IRQ_CH0_CHAIN_DONE        (CMICD_IRQ_BASE0 + 15)
#define CMICD_IRQ_CH3_INTR_COAL_INTR    (CMICD_IRQ_BASE0 + 16)
#define CMICD_IRQ_CH2_INTR_COAL_INTR    (CMICD_IRQ_BASE0 + 17)
#define CMICD_IRQ_CH1_INTR_COAL_INTR    (CMICD_IRQ_BASE0 + 18)
#define CMICD_IRQ_CH0_INTR_COAL_INTR    (CMICD_IRQ_BASE0 + 19)
#define CMICD_IRQ_SCHAN_OP_DONE         (CMICD_IRQ_BASE0 + 20)
#define CMICD_IRQ_CCMDMA_DONE           (CMICD_IRQ_BASE0 + 21)
#define CMICD_IRQ_SW_INTR0              (CMICD_IRQ_BASE0 + 22)
#define CMICD_IRQ_SW_INTR1              (CMICD_IRQ_BASE0 + 23)
#define CMICD_IRQ_SW_INTR2              (CMICD_IRQ_BASE0 + 24)
#define CMICD_IRQ_SW_INTR3              (CMICD_IRQ_BASE0 + 25)
#define CMICD_IRQ_SBUSDMA_ECCERR        (CMICD_IRQ_BASE0 + 26)
#define CMICD_IRQ_CH0_DESC_CTRL_INTR    (CMICD_IRQ_BASE0 + 27)
#define CMICD_IRQ_CH1_DESC_CTRL_INTR    (CMICD_IRQ_BASE0 + 28)
#define CMICD_IRQ_CH2_DESC_CTRL_INTR    (CMICD_IRQ_BASE0 + 29)
#define CMICD_IRQ_CH3_DESC_CTRL_INTR    (CMICD_IRQ_BASE0 + 30)

#define CMICD_IRQ_I2C_INTR              (CMICD_IRQ_BASE1 + 0)
#define CMICD_IRQ_TIMESYNC_INTR         (CMICD_IRQ_BASE1 + 2)
#define CMICD_IRQ_LINK_STAT_MOD         (CMICD_IRQ_BASE1 + 3)
#define CMICD_IRQ_PAUSE_STAT_MOD        (CMICD_IRQ_BASE1 + 4)
#define CMICD_IRQ_SPI_INTERRUPT         (CMICD_IRQ_BASE1 + 5)
#define CMICD_IRQ_COMMON_SCHAN_DONE     (CMICD_IRQ_BASE1 + 8)
#define CMICD_IRQ_COMMON_MIIM_OP_DONE   (CMICD_IRQ_BASE1 + 9)
#define CMICD_IRQ_GPIO_INTR             (CMICD_IRQ_BASE1 + 10)
#define CMICD_IRQ_TIM0_INTR1            (CMICD_IRQ_BASE1 + 23)
#define CMICD_IRQ_TIM0_INTR2            (CMICD_IRQ_BASE1 + 24)
#define CMICD_IRQ_TIM1_INTR1            (CMICD_IRQ_BASE1 + 25)
#define CMICD_IRQ_TIM1_INTR2            (CMICD_IRQ_BASE1 + 26)
#define CMICD_IRQ_SER_INTR              (CMICD_IRQ_BASE1 + 29)

/*
 * Remaining interrupts are chip-specific parity interrupts and SBUS
 * slave interrupts.
 */

/*! \} */

/*!
 * \brief Assign per-source interrupt handler.
 *
 * Assign an interrupt handler for a specific interrupt source.
 *
 * Refer to \ref bcmbd_intr_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] cmc CMIC CMC number.
 * \param [in] intr_num Interrupt source number (\ref CMICD_IRQ_xxx).
 * \param [in] intr_func Per-source interrupt handler function.
 * \param [in] intr_param Per-source interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid CMC or interrupt source.
 */
extern int
bcmbd_cmicd_intr_func_set(int unit, int cmc, unsigned int intr_num,
                          bcmbd_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Enable interrupt source.
 *
 * Allow a given interrupt source to generate hardware interrupts.
 *
 * A per-source interrupt handler should be installed before an
 * interrupt is enabled (\ref bcmbd_cmicd_intr_func_set).
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] cmc CMIC CMC number.
 * \param [in] intr_num Interrupt source number (\ref CMICD_IRQ_xxx).
 *
 * \return Nothing.
 */
extern void
bcmbd_cmicd_intr_enable(int unit, int cmc, int intr_num);

/*!
 * \brief Disable interrupt source.
 *
 * Prevent a given interrupt source from generating hardware
 * interrupts.
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] cmc CMIC CMC number.
 * \param [in] intr_num Interrupt source number (\ref CMICD_IRQ_xxx).
 *
 * \return Nothing.
 */
extern void
bcmbd_cmicd_intr_disable(int unit, int cmc, int intr_num);

/*!
 * \brief Disable interrupt source from interrupt handler context.
 *
 * Prevent a given interrupt source from generating hardware
 * interrupts.
 *
 * Should only be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] cmc CMIC CMC number.
 * \param [in] intr_num Interrupt source number (\ref CMICD_IRQ_xxx).
 *
 * \return Nothing.
 */
extern void
bcmbd_cmicd_intr_disable_deferred(int unit, int cmc, int intr_num);

/*!
 * \brief Clear all interrupt register configurations.
 *
 * Remove all interrupt register configurations created using \ref
 * bcmbd_cmicd_intr_reg_set.
 *
 * \param [in] unit Unit number.
 * \param [in] cmc CMIC CMC number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicd_intr_regs_clr(int unit, int cmc);

/*!
 * \brief Add interrupt register configuration.
 *
 * Add an entry to the interrupt register set table.
 *
 * This function is normally not called directly. Use \ref
 * bcmbd_cmicd_intr_regs_init instead.
 *
 * \param [in] unit Unit number.
 * \param [in] cmc CMIC CMC number.
 * \param [in] intr_base Interrupt source base for these registers.
 * \param [in] status_offs Interrupt status register offset.
 * \param [in] mask_offs Interrupt mask register offset.
 * \param [in] kmask Interrupt mask bits owned by a kernel driver.
 *
 * \retval 0 No errors
 */
extern int
bcmbd_cmicd_intr_reg_set(int unit, int cmc, unsigned int intr_base,
                         uint32_t status_offs, uint32_t mask_offs,
                         uint32_t kmask);

/*!
 * \brief Configure interrupt status and mask registers.
 *
 * This function will configure all the required the interrupt status
 * and mask registers for a device using \ref
 * bcmbd_cmicd_intr_reg_set.
 *
 * The initialization will take into account which CPU (external or
 * internal) the SDK is running on, as this will affect which
 * interrupt mask registers should be used.
 *
 * Since some devices only implement a subset of the interrupt status
 * registers, the \c num_regs parameter can be used to limit the
 * number of register sets installed.
 *
 * \param [in] unit Unit number.
 * \param [in] cmc CMIC CMC number.
 * \param [in] num_regs Number of register set to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid CMC.
 */
extern int
bcmbd_cmicd_intr_regs_init(int unit, int cmc, unsigned int num_regs);

/*!
 * \brief Disconnect CMICd ISR from system interrupt handler.
 *
 * This function will disconnect the CMICd ISR (interrupt service
 * routine) from the system interrupt handler via the
 * application-provided functions in the DRD library.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid DRD interface encountered.
 */
extern int
bcmbd_cmicd_intr_disconnect(int unit);

/*!
 * \brief Connect CMICd ISR to system interrupt handler.
 *
 * This function will connect the CMICd ISR (interrupt service
 * routine) to the system interrupt handler via the
 * application-provided functions in the DRD library.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid DRD interface encountered.
 */
extern int
bcmbd_cmicd_intr_connect(int unit);

/*!
 * \brief Enable or disable KNET interrupt mode.
 *
 * Low-level packet I/O (interrupts and DMA) can be handled either in
 * user space or kernel space (if applicable to the operating system).
 *
 * This function will configure interrupt handling according to the
 * selected mode of operation (KNET or non-KNET).
 *
 * \param [in] unit Unit number.
 * \param [in] enable Set to true for KNET mode.
 *
 * \retval SHR_E_NONE Operating mode successfully updated.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
extern int
bcmbd_cmicd_intr_knet_enable_set(int unit, bool enable);

/*!
 * \brief Get KNET interrupt mode enable status.
 *
 * Retrieve the mode that was currently selected using \ref
 * bcmbd_cmicd_intr_knet_enable_set.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Set to true if KNET mode is enabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
extern int
bcmbd_cmicd_intr_knet_enable_get(int unit, bool *enabled);

#endif /* BCMBD_CMICD_INTR_H */
