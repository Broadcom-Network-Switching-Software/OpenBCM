/*! \file bcm56780_a0_sec_interrupt.h
 *
 * File containing BCMSEC interrupt related changes for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_SEC_INTERRUPT_H
#define BCM56780_A0_SEC_INTERRUPT_H

/*!
 * \brief SEC Interrupt init function
 *
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot.
 *
 * \retval SHR_E_NONE
 * \retval SHR_E_PARAM Parameter error
 */
extern int
bcm56780_a0_sec_intr_init(int unit, bool warm);
#endif /* BCM56780_A0_SEC_INTERRUPT_H */
