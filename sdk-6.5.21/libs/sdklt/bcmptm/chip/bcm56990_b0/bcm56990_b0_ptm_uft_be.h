/*! \file bcm56990_b0_ptm_uft_be.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_B0_PTM_UFT_BE_H
#define BCM56990_B0_PTM_UFT_BE_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_cth_uft_be_internal.h>


/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Register device specific drivers for UFT_BE
 *
 * \param [in] unit Device unit.
 * \param [out] drv UFT specific driver
 *
 * \return SHR_E_XXX.
 */
extern int
bcm56990_b0_uft_be_driver_register(int unit, uft_be_driver_t *drv);

#endif /* BCM56990_B0_PTM_UFT_BE_H */
