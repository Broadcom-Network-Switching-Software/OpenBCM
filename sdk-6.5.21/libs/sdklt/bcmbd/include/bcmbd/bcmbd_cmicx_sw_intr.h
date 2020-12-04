/*! \file bcmbd_cmicx_sw_intr.h
 *
 * Interrupt definitions for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_SW_INTR_H
#define BCMBD_CMICX_SW_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*! \{
 *
 * \brief CMICx software Interrupt sources.
 * \anchor CMICX_SWI_xxx
 *
 * Please refer to the hardware documentation for details.
 */
#define CMICX_SWI_RCPU          0
#define CMICX_SWI_CORTEXM0_U0   1
#define CMICX_SWI_CORTEXM0_U1   2
#define CMICX_SWI_CORTEXM0_U2   3
#define CMICX_SWI_CORTEXM0_U3   4
#define CMICX_SWI_MHOST0        5
#define CMICX_SWI_MHOST1        6
#define CMICX_SWI_PCIE          7
/*! \} */

/*! Maximum number of CMICx software interrupts. */
#define CMICX_MAX_SWI_INTRS     8

/*!
 * \brief Install CMICx software interrupt driver.
 *
 * Initialize software interrupt driver with the default CMICx
 * software interrupt driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_sw_intr_drv_init(int unit);

/*!
 * \brief Clean up software interrupt handler.
 *
 * Disable hardware interrupt in the primary interrupt handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_sw_intr_stop(int unit);

/*!
 * \brief Initialize software interrupt handler.
 *
 * Install software interrupt handler and enable hardware
 * interrupt in the primary interrupt handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_sw_intr_start(int unit);

#endif /* BCMBD_CMICX_SW_INTR_H */
