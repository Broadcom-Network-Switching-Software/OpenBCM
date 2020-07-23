/*! \file bcm56880_a0_ptm_uft.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_PTM_UFT_H
#define BCM56880_A0_PTM_UFT_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_uft_internal.h>


/*******************************************************************************
 * Defines
 */



/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Register device specific drivers for UFT
 *
 * \param [in] unit Device unit.
 * \param [out] drv UFT specific driver
 *
 * \return SHR_E_XXX.
 */
extern int
bcm56880_a0_uft_driver_register(int unit, uft_driver_t *drv);

#endif /* BCM56880_A0_PTM_UFT_H */
