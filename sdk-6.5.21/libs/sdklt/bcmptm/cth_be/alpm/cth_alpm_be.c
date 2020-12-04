/*! \file cth_alpm_be.c
 *
 * Backend part of CTH_ALPM
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
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_cth_alpm_be_internal.h>

/*******************************************************************************
 * Defines
 */
#define CTH_ALPM_ALLOC(_ptr, _sz, _str) \
        SHR_ALLOC(_ptr, _sz, _str); \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY)
#define SHR_RETURN      SHR_ERR_EXIT

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_CTHALPM

/*******************************************************************************
 * Private variables
 */

static bcmptm_cth_alpm_control_t *cth_alpm_control[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static bcmptm_cth_alpm_control_t *default_cth_alpm_control[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_cth_alpm_control_reset(int u, int mtop)
{
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(cth_alpm_control[u][mtop], SHR_E_INTERNAL);
    SHR_NULL_CHECK(default_cth_alpm_control[u][mtop], SHR_E_INTERNAL);
    *cth_alpm_control[u][mtop] = *default_cth_alpm_control[u][mtop];
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_control_default_get(int u, int mtop, bcmptm_cth_alpm_control_t *ctrl)
{
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(ctrl, SHR_E_INTERNAL);
    *ctrl = *default_cth_alpm_control[u][mtop];
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_control_default_set(int u, int mtop, bcmptm_cth_alpm_control_t *ctrl)
{
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(ctrl, SHR_E_INTERNAL);
    *default_cth_alpm_control[u][mtop] = *ctrl;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_control_get(int u, int mtop, bcmptm_cth_alpm_control_t *ctrl)
{
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(ctrl, SHR_E_INTERNAL);
    *ctrl = *cth_alpm_control[u][mtop];
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_control_set(int u, int mtop, bcmptm_cth_alpm_control_t *ctrl)
{
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(ctrl, SHR_E_INTERNAL);
    *cth_alpm_control[u][mtop] = *ctrl;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_be_init(int u, bool warm)
{
    int i;

    SHR_FUNC_ENTER(u);
    for (i = 0; i < ALPMS; i++) {
        CTH_ALPM_ALLOC(cth_alpm_control[u][i], sizeof(bcmptm_cth_alpm_control_t),
                       "bcmptmCthalpmCtrls");
        sal_memset(cth_alpm_control[u][i], 0, sizeof(bcmptm_cth_alpm_control_t));

        CTH_ALPM_ALLOC(default_cth_alpm_control[u][i], sizeof(bcmptm_cth_alpm_control_t),
                       "bcmptmCthalpmDefctrls");
        sal_memset(default_cth_alpm_control[u][i], 0, sizeof(bcmptm_cth_alpm_control_t));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_be_cleanup(int u, bool warm)
{
    int i;

    SHR_FUNC_ENTER(u);
    for (i = 0; i < ALPMS; i++) {
        SHR_FREE(cth_alpm_control[u][i]);
        SHR_FREE(default_cth_alpm_control[u][i]);
    }
    SHR_FUNC_EXIT();
}
