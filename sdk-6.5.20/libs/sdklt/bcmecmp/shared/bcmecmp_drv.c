/*! \file bcmecmp_drv.c
 *
 * ECMP base driver data structure.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmecmp/bcmecmp.h>

/*******************************************************************************
* Local definitions
 */
/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

static bcmecmp_drv_t ecmp_drv[BCMDRD_CONFIG_MAX_UNITS];

static bcmecmp_drv_var_t ecmp_drv_var[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Get pointer to ECMP base driver device structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
bcmecmp_drv_t *
bcmecmp_drv_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return &ecmp_drv[unit];
}

/*!
 * \brief Get pointer to ECMP base driver device variant structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device variant structure, or NULL if not found.
 */
bcmecmp_drv_var_t *
bcmecmp_drv_var_get(int unit)
{
    return &ecmp_drv_var[unit];
}

