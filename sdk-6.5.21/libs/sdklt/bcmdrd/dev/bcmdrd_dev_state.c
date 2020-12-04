/*! \file bcmdrd_dev_state.c
 *
 * Device state related functions in DRD.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>

/* Device tainted state */
static bool bcmdrd_dev_tainted[BCMDRD_CONFIG_MAX_UNITS];

int
bcmdrd_dev_tainted_set(int unit, bool tainted)
{
    if (!bcmdrd_dev_exists(unit)) {
        return SHR_E_UNIT;
    }

    bcmdrd_dev_tainted[unit] = tainted;
    return SHR_E_NONE;
}

int
bcmdrd_dev_tainted_get(int unit, bool *tainted)
{
    if (!bcmdrd_dev_exists(unit)) {
        return SHR_E_UNIT;
    }

    *tainted = bcmdrd_dev_tainted[unit];
    return SHR_E_NONE;
}
