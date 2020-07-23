/*! \file bcmltm_state_upgrade.c
 *
 * Logical Table Manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>

#include <bcmissu/issu_api.h>
#include <bcmissu/issu_types.h>

#include <bcmltm/bcmltm_ha_internal.h>

int
bcmltm_state_hdl(int unit,
                 shr_sysm_categories_t cat,
                 bcmissu_phases_t phase)
{
    if (cat != SHR_SYSM_CAT_UNIT) {
        return SHR_E_NONE;
    }
    bcmltm_issu_enable(unit);

    return SHR_E_NONE;
}
