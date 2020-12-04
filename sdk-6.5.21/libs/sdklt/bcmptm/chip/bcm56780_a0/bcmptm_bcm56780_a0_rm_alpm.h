/*! \file bcmptm_bcm56780_a0_rm_alpm.h
 *
 * Chip specific functions for PTRM ALPM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_BCM56780_A0_RM_ALPM_H
#define BCMPTM_BCM56780_A0_RM_ALPM_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm_internal.h>
#include <sal/sal_types.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>


/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Register device specific drivers for RM ALPM
 *
 * \param [in] u Device u.
 * \param [out] drv ALPM specific driver
 *
 * \return SHR_E_XXX.
 */
extern int
bcm56780_a0_rm_alpm_driver_register(int unit, bcmptm_rm_alpm_driver_t *drv);

#endif /* BCMPTM_BCM56780_A0_RM_ALPM_H */



