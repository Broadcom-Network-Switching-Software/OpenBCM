/*! \file bcm56780_a0_tm_multicast.h
 *
 * File containing Multicast PT parameters for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_TM_MULTICAST_H
#define BCM56780_A0_TM_MULTICAST_H

#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmltd/bcmltd_types.h>

#define TD4_X9_NHOPS_PER_REPL_INDEX_SPARSE_MODE  6
#define TD4_X9_NHOPS_PER_REPL_INDEX_DENSE_MODE  64
#define TD4_X9_MAX_MC_PKT_REPL 1024
#define TD4_X9_MAX_REPL_LIST_COUNT 147456

/*!
 * \brief Multicast chip driver get.
 *
 * \param [in] unit Unit number.
 * \param [out] mc_drv Multicast driver.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcm56780_a0_tm_mc_drv_get(int unit, void *mc_drv);

#endif /* BCM56780_A0_TM_MULTICAST_H */
