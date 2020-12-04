/*! \file bcmltx_lt_pt_map.c
 *
 * TM transform functions for mapping lt fields with pt fields.
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
#include <bcmltx/bcmtm/bcmltx_one_to_one_field_map.h>


/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_TM

int
bcmltx_one_to_one_field_map_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    uint32_t idx = 0, count = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;
    while(idx < in->count) {
        out->field[count]->id = arg->rfield[idx];
        out->field[count]->data = in->field[idx]->data;
        out->field[count]->idx = 0;
        count++;
        idx++;
    }
    out->count = count;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_one_to_one_field_map_rev_transform(int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    uint32_t idx = 0, count = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;
    while(idx < in->count) {
        out->field[count]->id = arg->rfield[idx];
        out->field[count]->data = in->field[idx]->data;
        out->field[count]->idx = 0;
        count++;
        idx++;
    }
    out->count = count;
exit:
    SHR_FUNC_EXIT();
}
