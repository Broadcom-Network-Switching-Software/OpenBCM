
/*! \file bcmbd_cmicx2_tsfifo.h
 *
 * Timestamp FIFO operation driver on CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX2_TSFIFO_H
#define BCMBD_CMICX2_TSFIFO_H

/*!
 * \brief Install CMICx timesync FIFO capture driver.
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
bcmbd_cmicx2_tsfifo_drv_init(int unit);

/*!
 * \brief Clean up CMICx timesync FIFO capture driver.
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
bcmbd_cmicx2_tsfifo_drv_cleanup(int unit);

#endif /* BCMBD_CMICX2_TSFIFO_H */
