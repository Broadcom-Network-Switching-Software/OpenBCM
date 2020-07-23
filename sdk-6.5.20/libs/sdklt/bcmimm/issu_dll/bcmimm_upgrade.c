/*! \file bcmimm_upgrade.c
 *
 * In-Memory software upgrade functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <shr/shr_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmissu/issu_types.h>

extern void bcmimm_set_issu_event(int unit);

int bcmimm_fl_id_hdl(int unit,
                     shr_sysm_categories_t cat,
                     bcmissu_phases_t phase)
{
    if (cat != SHR_SYSM_CAT_UNIT) {
        return SHR_E_NONE;
    }
    bcmimm_set_issu_event(unit);

    return SHR_E_NONE;
}
