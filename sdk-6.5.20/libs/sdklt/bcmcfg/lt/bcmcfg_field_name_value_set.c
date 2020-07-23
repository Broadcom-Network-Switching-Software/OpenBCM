/*! \file bcmcfg_field_name_value_set.c
 *
 * bcmcfg_field_name_value_set() implementation.
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
bcmcfg_field_name_value_set(int unit,
                            const char *table_name,
                            const char *field_name,
                            uint64_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT(
        bcmcfg_table_set_cached_config_value_by_name(unit,
                                                     table_name,
                                                     field_name,
                                                     0,
                                                     value));
    SHR_FUNC_EXIT();
}
