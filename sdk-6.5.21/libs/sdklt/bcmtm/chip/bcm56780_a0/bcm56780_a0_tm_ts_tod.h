/*! \file bcm56780_a0_tm_ts_tod.h
 *
 * File containing Timestamp ToD related defines and internal function for
 * bcm56780_a0_tm.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCM56780_A0_TM_TS_TOD_H
#define BCM56780_A0_TM_TS_TOD_H

/*!
 * \brief  Initialize device type based function pointers for timestamp ToD.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  ts_tod_drv   Timestamp ToD driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcm56780_a0_tm_ts_tod_drv_get(int unit, void * ts_tod_drv);

#endif  /* BCM56780_A0_TM_TS_TOD_H */
