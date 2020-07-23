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

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ts_imm_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_ts_imm_register(unit, warm));

exit:
    SHR_FUNC_EXIT();
}
