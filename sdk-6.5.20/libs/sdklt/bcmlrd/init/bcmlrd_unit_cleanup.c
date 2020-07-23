/*! \file bcmlrd_unit_cleanup.c
 *
 * Clean up all LRD unit data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <sal/sal_alloc.h>
#include <bcmdrd/bcmdrd_dev.h>

int
bcmlrd_unit_cleanup(void)
{
    int unit;

    sal_memset(bcmlrd_unit_conf, 0, sizeof(bcmlrd_unit_conf));

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmlrd_unit_pcm_conf[unit] != NULL) {
            sal_free(bcmlrd_unit_pcm_conf[unit]);
            bcmlrd_unit_pcm_conf[unit] = NULL;
        }
    }

    return SHR_E_NONE;
}
