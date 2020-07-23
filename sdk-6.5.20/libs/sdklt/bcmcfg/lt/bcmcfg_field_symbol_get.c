/*! \file bcmcfg_field_symbol_get.c
 *
 * bcmcfg_field_symbol_get() implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmlrd/bcmlrd_client.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_TABLE

/*******************************************************************************
 * Public functions
 */
int
bcmcfg_field_symbol_get(int unit,
                        bcmltd_sid_t sid,
                        bcmltd_fid_t fid,
                        const char **sym)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmcfg_field_get(unit, sid, fid, &value));

    SHR_IF_ERR_EXIT(
        bcmlrd_field_value_to_symbol(unit, sid, fid,
                                     (uint32_t)value, sym));

exit:
    SHR_FUNC_EXIT();
}
