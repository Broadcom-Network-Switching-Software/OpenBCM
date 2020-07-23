/*! \file bcmltx_ctr_group_stats_drop_cnt.c
 *
 * Ingress and egress drop count event transform utilities.
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
#include <bcmltx/bcmctr/bcmltx_ctr_group_stats.h>
#include <bcmltd/bcmltd_bitop.h>

#define BSL_LOG_MODULE  BSL_LS_BCMLTX_CTR

int
bcmltx_ctr_group_stats_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)
{
    uint32_t pipe_num, idx;

    SHR_FUNC_ENTER(unit);

    if (!arg) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (in->count != arg->fields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (out->count < arg->rfields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    out->count = 0;
    pipe_num = arg->rfields;

    for (idx = 0; idx < pipe_num; idx++) {
        out->field[out->count]->data = 0;
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_ctr_group_stats_rev_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    uint32_t pipe_num, idx;

    SHR_FUNC_ENTER(unit);

    if (!arg) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (out->count != arg->rfields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (in->count < arg->fields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    out->count = 0;
    pipe_num = arg->fields;

    out->field[out->count]->data = 0;
    for (idx = 0; idx < pipe_num; idx++) {
        out->field[out->count]->data += in->field[idx]->data;
    }
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}
