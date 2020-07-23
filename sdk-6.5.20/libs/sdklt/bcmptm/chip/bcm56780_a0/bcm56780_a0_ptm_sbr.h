/*! \file bcm56780_a0_ptm_sbr.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_PTM_SBR_H
#define BCM56780_A0_PTM_SBR_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_sbr_internal.h>


/*******************************************************************************
 * Defines
 */



/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Register device specific drivers for SBR
 *
 * \param [in] unit Device unit.
 * \param [out] drv SBR specific driver
 *
 * \retval SHR_E_XXX.
 */
extern int
bcm56780_a0_sbr_driver_register(int unit, sbr_driver_t *drv);

#endif /* BCM56780_A0_PTM_SBR_H */
