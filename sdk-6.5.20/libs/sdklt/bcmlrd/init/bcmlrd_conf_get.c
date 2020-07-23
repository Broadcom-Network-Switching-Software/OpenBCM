/*! \file bcmlrd_conf_get.c
 *
 * High level API to return a mapping configuration for the given
 * unit.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_conf_get(int unit, const bcmlrd_map_conf_rep_t **out)
{
    const bcmlrd_map_conf_rep_t *conf = bcmlrd_unit_conf_get(unit);
    int rv = SHR_E_UNIT;

    if (conf) {
        *out = conf;
        rv = SHR_E_NONE;
    }

    return rv;
}
