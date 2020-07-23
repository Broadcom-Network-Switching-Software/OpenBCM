/*! \file bcmltx_index_tile_mode_table_entry_limit.c
 *
 * Table entry limit handler for index tile mode tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/general/bcmltx_index_tile_mode_table_entry_limit.h>
#include <bcmptm/bcmptm_itx.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_index_tile_mode_table_entry_limit_get(int unit,
                            uint32_t trans_id,
                            bcmltd_sid_t sid,
                            const bcmltd_table_entry_limit_arg_t *table_arg,
                            bcmltd_table_entry_limit_t *table_data,
                            const bcmltd_generic_arg_t *arg)
{
    /* Wrapper for ITX implementation */
    return bcmptm_itx_entry_limit_get(unit, trans_id, sid,
                                      table_arg, table_data, arg);
}
