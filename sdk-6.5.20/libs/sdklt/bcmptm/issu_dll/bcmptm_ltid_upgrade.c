/*! \file bcmptm_ltid_upgrade.c
 *
 * ISSU functions to update LTIDs
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
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmissu/issu_types.h>
#include <bcmptm/bcmptm.h>

/*******************************************************************************
 * Defines
 */

/*******************************************************************************
 * Private variables
 */

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_ltid_hdl(int unit,
                shr_sysm_categories_t cat,
                bcmissu_phases_t phase)
{
    if (cat != SHR_SYSM_CAT_UNIT) {
        return SHR_E_NONE;
    }

    return bcmptm_ptcache_ltid_change(unit);
}
