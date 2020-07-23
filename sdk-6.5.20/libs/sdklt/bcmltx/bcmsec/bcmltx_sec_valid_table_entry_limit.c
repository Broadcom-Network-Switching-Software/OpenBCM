/*! \file bcmltx_sec_valid_table_entry_limit.c
 *
 * Table entry limit handler SEC valid LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltx/bcmsec/bcmltx_sec_valid_table_entry_limit.h>
#include <shr/shr_debug.h>
#include <bcmsec/bcmsec_drv.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_SEC
/*******************************************************************************
 * Public functions
 */
int
bcmltx_sec_valid_table_entry_limit_get(int unit,
                            uint32_t trans_id,
                            bcmltd_sid_t sid,
                            const bcmltd_table_entry_limit_arg_t *table_arg,
                            bcmltd_table_entry_limit_t *table_data,
                            const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    if (!bcmsec_is_sec_supported(unit)) {
        table_data->entry_limit = 0;
    }
    SHR_FUNC_EXIT();
}
