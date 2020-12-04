/*! \file bcm56990_a0_di_devicecode.c
 *
 * BCMDI DeviceCode Loader
 *
 * This module contains the implementation of BCMDI DeviceCode loader and
 * extra device initialization not covered by DeviceCode
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmdi/bcmdi_internal.h>

/*******************************************************************************
 * Public Functions
 */
int
bcm56990_a0_di_dvc_drv_attach(int unit, bcmdi_dvc_drv_t *drv)
{
    return SHR_E_NONE;
}

