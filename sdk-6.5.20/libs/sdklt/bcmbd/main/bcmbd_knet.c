/*! \file bcmbd_knet.c
 *
 * KNET mode control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>

#include <bcmbd/bcmbd.h>

int
bcmbd_knet_enable_set(int unit, bool enable)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->knet_enable_set == NULL) {
        return enable ? SHR_E_UNAVAIL : SHR_E_NONE;
    }

    return bd->knet_enable_set(unit, enable);
}

int
bcmbd_knet_enable_get(int unit, bool *enable)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->knet_enable_get == NULL) {
        *enable = false;
        return SHR_E_NONE;
    }

    return bd->knet_enable_get(unit, enable);
}
