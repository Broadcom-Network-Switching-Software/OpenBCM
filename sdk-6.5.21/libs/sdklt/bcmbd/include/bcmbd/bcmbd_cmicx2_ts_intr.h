/*! \file bcmbd_cmicx2_ts_intr.h
 *
 * Timestamp Interrupt definitions for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX2_TS_INTR_H
#define BCMBD_CMICX2_TS_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*! \{
 *
 * \brief CMICx timesync interrupt sources.
 * \anchor CMICX_TS_xxx
 */
#define CMICX_TS_FIFO1_NOT_EMPTY            0
#define CMICX_TS_FIFO1_OVERFLOW             1
#define CMICX_TS_CPU_FIFO1_ECC_ERR_STATUS   2
#define CMICX_TS_FIFO2_NOT_EMPTY            3
#define CMICX_TS_FIFO2_OVERFLOW             4
#define CMICX_TS_CPU_FIFO2_ECC_ERR_STATUS   5
#define CMICX_TS_TS0_CNT_OFFSET_UPDATED     6
#define CMICX_TS_TS1_CNT_OFFSET_UPDATED     7
#define CMICX_TS_CAPTURE_OVRD               8
#define CMICX_TS_TIMER_INT                  9
/*! \} */

/*! Maximum number of CMICx ts interrupts. */
#define CMICX_MAX_TS_INTRS     10

/*!
 * \brief Install CMICx timesync interrupt driver.
 *
 * Initialize timesync interrupt driver with the default
 * CMICx timesync interrupt driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx2_ts_intr_drv_init(int unit);

/*!
 * \brief Clean up CMICx timesync interrupt driver.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx2_ts_intr_drv_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx2_ts_intr_drv_cleanup(int unit);

/*!
 * \brief Clean up timesync interrupt handler.
 *
 * Disable hardware interrupt in the primary interrupt handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx2_ts_intr_stop(int unit);

/*!
 * \brief Initialize timesync interrupt handler.
 *
 * Install timesync interrupt handler and enable hardware
 * interrupt in the primary interrupt handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx2_ts_intr_start(int unit);

#endif /* BCMBD_CMICX_TS_INTR_H */
