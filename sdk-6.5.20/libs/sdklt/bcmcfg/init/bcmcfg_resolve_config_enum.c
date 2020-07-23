/*! \file bcmcfg_resolve_config_enum.c
 *
 * Resolve all config enum.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmltd/bcmltd_table.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_INIT

/*******************************************************************************
 * Public functions
 */
int
bcmcfg_resolve_config_enum(int unit)
{
    /* DEPRECATED */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}
