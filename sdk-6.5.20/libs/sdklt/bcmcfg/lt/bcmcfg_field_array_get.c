/*! \file bcmcfg_field_array_get.c
 *
 * bcmcfg_field_array_get() implementation.
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
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

int
bcmcfg_field_array_get(int unit,
                       bcmltd_sid_t sid,
                       bcmltd_fid_t fid,
                       uint32_t start_idx,
                       uint32_t num_of_elem,
                       uint64_t *value,
                       size_t *actual)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_CONT(bcmcfg_table_get_internal(unit,
                                              sid,
                                              fid,
                                              start_idx,
                                              num_of_elem,
                                              value,
                                              actual));
    SHR_FUNC_EXIT();
}
