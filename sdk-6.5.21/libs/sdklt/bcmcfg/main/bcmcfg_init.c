/*! \file bcmcfg_init.c
 *
 * BCMCFG limited initialization handlers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>

#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>

#include <bcmcfg/bcmcfg.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg_init.h>

#define BSL_LOG_MODULE BSL_LS_BCMCFG_INIT

/*******************************************************************************
 * Public functions
 */

int
bcmcfg_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcfg_table_cached_config_apply(unit, warm, INIT_STAGE_EARLY));

    SHR_IF_ERR_EXIT
        (bcmcfg_read_unit_init(unit, INIT_STAGE_EARLY));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_config(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcfg_table_cached_config_apply(unit, warm, INIT_STAGE_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmcfg_read_unit_init(unit, INIT_STAGE_NORMAL));

    /* Set variant to BASE if variant config is not specified
     * and default variant is unavailable. */
    SHR_IF_ERR_EXIT
        (bcmcfg_lt_config_base_variant_set(unit, warm));

    /* Load map default */
    SHR_IF_ERR_EXIT
        (bcmcfg_lt_config_default_set(unit, warm));

    /* Config LT initialization is done */
    bcmcfg_lt_init_done(unit);

    /* Set variant */
    SHR_IF_ERR_EXIT
        (bcmcfg_variant_load(unit, warm));

    /* Perform conditional map editing */
    SHR_IF_ERR_EXIT
        (bcmcfg_unit_conf_edit(unit, warm));

    /* LT Config range check */
    SHR_IF_ERR_EXIT
        (bcmcfg_lt_config_range_check(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_shutdown(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcfg_config_table_init(unit, 1));

    SHR_IF_ERR_EXIT
        (bcmcfg_read_unit_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

bool
bcmcfg_ready(int unit)
{
    return bcmcfg_lt_init_is_done(unit);
}
