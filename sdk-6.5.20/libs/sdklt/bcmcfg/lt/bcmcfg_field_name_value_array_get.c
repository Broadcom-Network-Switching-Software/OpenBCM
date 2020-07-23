/*! \file bcmcfg_field_name_value_array_get.c
 *
 * bcmcfg_field_name_value_array_get() implementation.
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
#include <bcmlrd/bcmlrd_table.h>

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
bcmcfg_field_name_value_array_get(int unit,
                                  const char *table_name,
                                  const char *field_name,
                                  uint32_t start_idx,
                                  uint32_t num_of_elem,
                                  uint64_t *value,
                                  size_t *actual)
{
    bcmltd_sid_t sid;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    /* Get the table and field ID. */
    SHR_IF_ERR_EXIT(
        bcmlrd_table_field_name_to_idx(unit, table_name, field_name,
                                       &sid, &fid));

    /* Get the field value as requested. */
    SHR_IF_ERR_CONT(bcmcfg_table_get_internal(unit,
                                              sid,
                                              fid,
                                              start_idx,
                                              num_of_elem,
                                              value,
                                              actual));

exit:
    SHR_FUNC_EXIT();
}
