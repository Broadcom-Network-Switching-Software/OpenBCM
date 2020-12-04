/*! \file bcmcth_ts_imm.c
 *
 * BCMCTH TimeSync IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmcth/bcmcth_ts_drv.h>
#include <bcmcth/bcmcth_ts_synce_drv.h>
#include <bcmcth/bcmcth_ts_bs.h>
#include <bcmlrd/bcmlrd_map.h>

/*******************************************************************************
 * Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

/*******************************************************************************
 * Private Functions
 */
static int
bcmcth_ts_imm_register(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Register callbacks for SYNCE LT. */
    SHR_IF_ERR_EXIT
        (bcmcth_ts_synce_imm_register(unit));

    /* Register callback for TOD LT. */
    SHR_IF_ERR_EXIT
        (bcmcth_ts_tod_imm_register(unit));

    /* Register callback for BS LT. */
    SHR_IF_ERR_EXIT
        (bcmcth_ts_bs_imm_register(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
bool
bcmcth_ts_imm_mapped(int unit, const bcmlrd_sid_t sid)
{
    int rv;
    const bcmlrd_map_t *map = NULL;

    rv = bcmlrd_map_get(unit, sid, &map);
    if (SHR_SUCCESS(rv) &&
        map &&
        map->group &&
        map->group[0].dest.kind == BCMLRD_MAP_CUSTOM) {
        return TRUE;
    }
    return FALSE;
}

int
bcmcth_ts_imm_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_ts_imm_register(unit, warm));

exit:
    SHR_FUNC_EXIT();
}
