/*! \file cth_alpm_device.c
 *
 * CTH ALPM Device specific
 *
 * This file contains the main function for CTH ALPM device specific info.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "cth_alpm_device.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_CTHALPM

/*******************************************************************************
 * Private variables
 */

static cth_alpm_device_info_t *cth_alpm_device_info[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static bcmptm_cth_alpm_driver_t *cth_alpm_driver[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Populate device info into data structure
 *
 * \param [in] u Device u.
 * \param [out] device Device info data structure
 *
 * \return SHR_E_XXX.
 */
static int
cth_alpm_device_info_init(int u, int m, cth_alpm_device_info_t *device)
{
    int rv = SHR_E_UNAVAIL;

    if (CTH_ALPM_DRV(u, m) && CTH_ALPM_DRV_EXEC(u, m, device_info_init)) {
        rv = CTH_ALPM_DRV_EXEC(u, m, device_info_init) (u, m, device);
    }
    return rv;
}
/*******************************************************************************
 * Public Functions
 */

const cth_alpm_device_info_t *
bcmptm_cth_alpm_device_info_get(int u, int m)
{
    return CTH_ALPM_DEV(u, m);
}

int
bcmptm_cth_alpm_device_control_validate(int u, int m, uint8_t stage,
                                        bcmptm_cth_alpm_control_t *control)
{
    int rv = SHR_E_UNAVAIL;

    if (CTH_ALPM_DRV(u, m) && CTH_ALPM_DRV_EXEC(u, m, control_validate)) {
        rv = CTH_ALPM_DRV_EXEC(u, m, control_validate) (u, m, stage, control);
    }
    return rv;
}

int
bcmptm_cth_alpm_device_control_encode(int u, int m,
                                      bcmptm_cth_alpm_control_t *control,
                                      cth_alpm_control_pt_t *control_pt)
{
    int rv = SHR_E_UNAVAIL;

    if (CTH_ALPM_DRV(u, m) && CTH_ALPM_DRV_EXEC(u, m, control_encode)) {
        rv = CTH_ALPM_DRV_EXEC(u, m, control_encode) (u, m, control, control_pt);
    }
    return rv;
}

void
bcmptm_cth_alpm_device_cleanup(int u, int m)
{
    SHR_FREE(CTH_ALPM_DEV(u, m));
    SHR_FREE(CTH_ALPM_DRV(u, m));
}

int
bcmptm_cth_alpm_device_init(int u, int m)
{
    int rv;

    SHR_FUNC_ENTER(u);
    SHR_ALLOC(CTH_ALPM_DEV(u, m), sizeof(cth_alpm_device_info_t),
              "bcmptmCthalmpDev");
    SHR_NULL_CHECK(CTH_ALPM_DEV(u, m), SHR_E_MEMORY);
    sal_memset(CTH_ALPM_DEV(u, m), 0, sizeof(cth_alpm_device_info_t));

    SHR_ALLOC(CTH_ALPM_DRV(u, m), sizeof(bcmptm_cth_alpm_driver_t),
              "bcmptmCthalmpDrv");
    SHR_NULL_CHECK(CTH_ALPM_DRV(u, m), SHR_E_MEMORY);
    sal_memset(CTH_ALPM_DRV(u, m), 0, sizeof(bcmptm_cth_alpm_driver_t));

    rv = bcmptm_cth_alpm_driver_register(u, CTH_ALPM_DRV(u, m));
    if (rv == SHR_E_UNAVAIL) {
        LOG_INFO(BSL_LOG_MODULE,
                  (BSL_META_U(u, "CTH ALPM driver missing.\n")));
        SHR_ERR_EXIT(rv);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT(cth_alpm_device_info_init(u, m, CTH_ALPM_DEV(u, m)));
exit:
    SHR_FUNC_EXIT();
}

