/*! \file bcmbd_cmicx2_lp_intr.h
 *
 * Low-priotiry interrupt definitions for CMICx v2.
 *
 * For any CMICx LP interrupt API not listed here, the default (v1)
 * CMICx API should be used.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX2_LP_INTR_H
#define BCMBD_CMICX2_LP_INTR_H

#include <bcmbd/bcmbd_intr.h>

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
bcmbd_cmicx2_lp_intr_init(int unit);

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
bcmbd_cmicx2_lp_intr_stop(int unit);

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
bcmbd_cmicx2_lp_intr_start(int unit);

#endif /* BCMBD_CMICX2_LP_INTR_H */
