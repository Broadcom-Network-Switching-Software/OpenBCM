/*! \file bcmcfg_field_array_set.c
 *
 * bcmcfg_field_array_set() implementation.
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

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_TABLE

/*******************************************************************************
 * Public functions
 */

int
bcmcfg_field_array_set(int unit,
                       bcmltd_sid_t sid,
                       bcmltd_fid_t fid,
                       uint32_t start_idx,
                       uint32_t num_of_elem,
                       uint64_t *value)
{
    uint32_t last = start_idx + num_of_elem;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    for (idx = start_idx; idx < last; idx++) {
        SHR_IF_ERR_CONT(
            bcmcfg_table_set_cached_config_value_by_id(unit,
                                                       sid,
                                                       fid,
                                                       idx,
                                                       value[idx]));
        if (SHR_FUNC_ERR()) {
            break;
        }
    }

    SHR_FUNC_EXIT();
}
