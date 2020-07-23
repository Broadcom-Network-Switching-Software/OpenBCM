/*! \file bcmlrd_unit_pcm_conf_get.c
 *
 * Return a mapping configuration for the given unit.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_internal.h>

bcmlrd_pcm_conf_rep_t *bcmlrd_unit_pcm_conf[BCMDRD_CONFIG_MAX_UNITS];

/* get a mapping configuration for a unit */
bcmlrd_pcm_conf_rep_t *
bcmlrd_unit_pcm_conf_get(int unit)
{
    bcmlrd_pcm_conf_rep_t *conf = NULL;

    if (unit >= 0 && unit < COUNTOF(bcmlrd_unit_pcm_conf)) {
        conf = bcmlrd_unit_pcm_conf[unit];
    }

    return conf;
}
