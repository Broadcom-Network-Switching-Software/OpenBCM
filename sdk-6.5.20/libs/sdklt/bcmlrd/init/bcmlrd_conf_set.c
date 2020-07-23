/*! \file bcmlrd_conf_set.c
 *
 * DESCRIPTION
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Public functions
 */
int
bcmlrd_conf_set(int unit, const bcmlrd_map_conf_rep_t *in)
{
    int rv = SHR_E_UNIT;

    if (unit >= 0 && unit < COUNTOF(bcmlrd_unit_conf)) {
        bcmlrd_unit_conf[unit] = in;
        rv = SHR_E_NONE;
    }

    return rv;
}
