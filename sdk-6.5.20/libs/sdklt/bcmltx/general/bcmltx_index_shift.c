/*! \file bcmltx_index_shift.c
 *
 * Index shift handler
 *
 * Built-in Transform implementation to shift a LT index which is not
 * equal to the mapped PT index.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/bcmltx_index_shift.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

int
bcmltx_index_shift_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t field_ix, field_num;
    uint32_t src_field_id, dst_field_id;
    uint32_t index_shift;
    bcmltd_field_t *src_lta_field, *cur_lta_field;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_index_shift_transform \n")));

    if (arg->fields != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    src_field_id = arg->field[0];

    if (arg->rfields != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    dst_field_id = arg->rfield[0];

    if (arg->values != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    index_shift = arg->value[0];

    /* Search LTA input list for matching field */
    field_num = in->count;
    src_lta_field = NULL;
    for (field_ix = 0; field_ix < field_num; field_ix++) {
        src_lta_field = in->field[field_ix];
        if ((src_lta_field->id == src_field_id) &&
            (src_lta_field->idx == 0)) {
            break;
        }
    }
    if (src_lta_field == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (field_ix == field_num) {
        /* Field not found in LTA input list */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (out->count < 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    cur_lta_field = out->field[0];
    cur_lta_field->data = src_lta_field->data + index_shift;

    cur_lta_field->id = dst_field_id;
    cur_lta_field->idx = 0;
    out->count = 1;

    SHR_EXIT();
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_index_shift_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t field_ix, field_num;
    uint32_t src_field_id, dst_field_id;
    uint32_t index_shift;
    bcmltd_field_t *src_lta_field, *cur_lta_field;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_index_shift_rev_transform \n")));

    if (arg->fields != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    src_field_id = arg->field[0];

    if (arg->rfields != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    dst_field_id = arg->rfield[0];

    if (arg->values != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    index_shift = arg->value[0];

    /* Search LTA input list for matching field */
    field_num = in->count;
    src_lta_field = NULL;
    for (field_ix = 0; field_ix < field_num; field_ix++) {
        src_lta_field = in->field[field_ix];
        if ((src_lta_field->id == src_field_id) &&
            (src_lta_field->idx == 0)) {
            break;
        }
    }
    if (src_lta_field == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (field_ix == field_num) {
        /* Field not found in LTA input list */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (out->count < 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    cur_lta_field = out->field[0];
    cur_lta_field->data = src_lta_field->data - index_shift;

    cur_lta_field->id = dst_field_id;
    cur_lta_field->idx = 0;
    out->count = 1;

    SHR_EXIT();
 exit:
    SHR_FUNC_EXIT();
}

