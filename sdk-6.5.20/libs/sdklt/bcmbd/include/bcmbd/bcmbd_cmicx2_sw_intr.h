/*! \file bcmbd_cmicx2_sw_intr.h
 *
 * SW Interrupt definitions for CMICx2.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX2_SW_INTR_H
#define BCMBD_CMICX2_SW_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*! \{
 *
 * \brief CMICx2 software interrupt sources.
 * \anchor CMICX2_SWI_xxx
 *
 * Up to 32 interrupt sources are available.
 * SDK and embedded firmwares must be in sync
 * as to which processor owns which bit.
 */
#define CMICX2_SWI_PCIE          0
#define CMICX2_SWI_CORTEXM0_U0   1
#define CMICX2_SWI_CORTEXM0_U1   2
#define CMICX2_SWI_CORTEXM0_U2   3
#define CMICX2_SWI_CORTEXM0_U3   4
#define CMICX2_SWI_MHOST0        5
#define CMICX2_SWI_MHOST1        6
#define CMICX2_SWI_MHOST2        7
#define CMICX2_SWI_MHOST3        8
/*! \} */

/*! Maximum number of CMICx2 software interrupts. */
#define CMICX2_MAX_SWI_INTRS     9

/*!
 * \brief Install CMICx software interrupt driver.
 *
 * Initialize software interrupt driver with the default
 * CMICx2 software interrupt driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx2_sw_intr_drv_init(int unit);

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
bcmbd_cmicx2_sw_intr_stop(int unit);

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
bcmbd_cmicx2_sw_intr_start(int unit);

#endif /* BCMBD_CMICX2_SW_INTR_H */
