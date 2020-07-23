/*! \file bcmptm_bcm56990_a0_cth_alpm.h
 *
 * Chip specific functions for CTH ALPM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_BCM56990_A0_CTH_ALPM_H
#define BCMPTM_BCM56990_A0_CTH_ALPM_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm_internal.h>
#include <sal/sal_types.h>
#include <bcmptm/bcmptm_cth_alpm_internal.h>

/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Register device specific drivers for CTH ALPM
 *
 * \param [in] u Device unit.
 * \param [out] drv ALPM specific driver
 *
 * \return SHR_E_XXX.
 */
extern int
bcm56990_a0_cth_alpm_driver_register(int unit, bcmptm_cth_alpm_driver_t *drv);

#endif /* BCMPTM_BCM56990_A0_CTH_ALPM_H */



