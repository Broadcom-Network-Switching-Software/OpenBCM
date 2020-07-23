/*! \file bcmltx_dlb_group_stats_drop_cnt.c
 *
 * DLB_ECMP/LAG_GROUP_STATS transform utilities.
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
#include <bcmltx/bcmdlb/bcmltx_dlb_group_stats.h>
#include <bcmltd/bcmltd_bitop.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

int
bcmltx_dlb_group_stats_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)
{
    uint32_t pipe_num, idx;

    SHR_FUNC_ENTER(unit);

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;
    pipe_num = arg->value[0];

    /* Clear the counter for any UPDATE operation. */
    for (idx = 0; idx < pipe_num; idx++) {
        out->field[out->count]->data = 0;
        out->field[out->count]->id = arg->rfield[out->count];
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_dlb_group_stats_rev_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    uint32_t pipe_num, idx;

    SHR_FUNC_ENTER(unit);

    pipe_num = arg->value[0];

    if (in->count != pipe_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;

    for (idx = 0; idx < pipe_num; idx++) {
        out->field[out->count]->data += in->field[idx]->data;
    }
    out->field[out->count]->id = arg->rfield[out->count];
    out->count++;

exit:
    SHR_FUNC_EXIT();
}
