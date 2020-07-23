/*! \file bcm56990_b0_tm_ebst.h
 *
 * File containing EBST funebstions specific to
 * bcm56990_b0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_B0_TM_EBST_H
#define BCM56990_B0_TM_EBST_H

#include <shr/shr_types.h>

/*!
 * \brief  Device specific EBST driver get.
 *
 * \param [in] unit         Unit number.
 * \param [in] warm         Warmboot.
 *
 * \retval SHR_E_NONE       No error.
 * \retval SHR_E_INTERNAL   Internal error.
 */
extern int
bcm56990_b0_tm_ebst_drv_get(int unit, void *ebst_drv);

/*!
 * \brief  Device specific EBST initialization.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcm56990_b0_tm_ebst_init(int unit, bool warm);

#endif /* BCM56990_B0_TM_EBST_H */
