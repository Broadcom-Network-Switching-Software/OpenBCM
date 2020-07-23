/*! \file bcmbd_ipoll.h
 *
 * Definitions for poll IRQ support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_IPOLL_H
#define BCMBD_IPOLL_H

#include <bcmbd/bcmbd.h>

/*!
 * \brief Initialize polled IRQ mode.
 *
 * This function will consult the system configuration and enable
 * polled IRQ mode according to the current settings.
 *
 * In polled IRQ mode, hardware interrupts will remain disabled, and
 * instead the primary SDK interrupt handler will be invoked at
 * regular intervals.
 *
 * By default polled IRQ mode is disabled.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Unrecognized device type.
 */
extern int
bcmbd_ipoll_init(int unit);

/*!
 * \brief Get enabled state of polled IRQ mode.
 *
 * This function can be called following \ref bcmbd_ipoll_init in
 * order to determine whether the system runs in polled IRQ mode or
 * not.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Unit is using polled IRQ mode.
 * \retval false Unit is using hardware interrupts.
 */
extern bool
bcmbd_ipoll_enabled(int unit);

#endif /* BCMBD_IPOLL_H */
