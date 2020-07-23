/*! \file bcmltx_array_to_multi_fields_set.c
 *
 * Mirror IPv4 Array Set Handler.
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
#include <bcmltx/bcmmirror/bcmltx_array_to_multi_fields_set.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

int
bcmltx_array_to_multi_fields_set_rev_transform(int unit,
                                           const bcmltd_fields_t *in,
                                           bcmltd_fields_t *out,
                                           const bcmltd_transform_arg_t *arg)

{
    uint32_t cnt;

    SHR_FUNC_ENTER(unit);

    out->count = 0;
    for (cnt = 0; cnt < in->count; cnt++) {
        out->field[out->count]->data = in->field[cnt]->data;
        out->field[out->count]->id   = arg->rfield[0];
        out->field[out->count]->idx  = cnt;
        out->count++;
    }
    SHR_FUNC_EXIT();
}

int
bcmltx_array_to_multi_fields_set_transform(int unit,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg)
{
    uint32_t array_idx;
    uint32_t tmp_ipv4;

    SHR_FUNC_ENTER(unit);

    out->count = 0;
    for (array_idx = 0; array_idx < in->count; array_idx ++) {
        tmp_ipv4 = in->field[array_idx]->data;
        out->field[out->count]->id   = arg->rfield[array_idx];
        out->field[out->count]->data = tmp_ipv4;
        out->field[out->count]->idx  = 0;
        out->count++;
    }

    SHR_FUNC_EXIT();
}
