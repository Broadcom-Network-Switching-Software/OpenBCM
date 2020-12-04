/*! \file bcmltx_dlb_trunk_table_entry_limit.c
 *
 * Table entry limit handler for DLB tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmdrd_config.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltx/bcmdlb/bcmltx_dlb_trunk_table_entry_limit.h>
#include <bcmltd/bcmltd_bitop.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL
#define MAX_PT_COUNT 8

/*******************************************************************************
 * Public functions
 */
int
bcmltx_dlb_trunk_table_entry_limit_get(int unit,
                                       uint32_t trans_id,
                                       bcmltd_sid_t sid,
                                       const bcmltd_table_entry_limit_arg_t *table_arg,
                                       bcmltd_table_entry_limit_t *table_data,
                                       const bcmltd_generic_arg_t *arg)
{
    int rv;
    uint32_t num_pt;
    uint32_t actual_num_pt = 0;
    bcmdrd_sid_t ptid_list[MAX_PT_COUNT];
    uint32_t ix;

    SHR_FUNC_ENTER(unit);
    table_data->entry_limit = table_arg->entry_maximum;

    rv = bcmlrd_table_pt_list_num_get(unit,
                                      sid,
                                      &num_pt);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    if (num_pt > MAX_PT_COUNT) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    rv = bcmlrd_table_pt_list_get(unit,
                                  sid,
                                  num_pt,
                                  ptid_list,
                                  &actual_num_pt);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    if (actual_num_pt > MAX_PT_COUNT) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (ix = 0; ix < actual_num_pt; ix++) {
        if (!bcmdrd_pt_is_valid(unit, ptid_list[ix])) {
            table_data->entry_limit = 0;
            break;
        }
    }

exit:

    SHR_FUNC_EXIT();
}
