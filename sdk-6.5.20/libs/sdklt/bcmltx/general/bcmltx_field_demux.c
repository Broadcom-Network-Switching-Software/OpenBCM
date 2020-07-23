/*! \file bcmltx_field_demux.c
 *
 * Field demux handler
 *
 * Built-in Transform implementation to split or recombine API fields
 * when they do not map directly to PT fields.
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
#include <bcmltx/bcmltx_field_demux.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

int
bcmltx_field_demux_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t field_ix, field_num, rfield_ix, rfield_num;
    uint64_t src_lta_data = 0;
    bcmltd_field_t *src_lta_field;
    const bcmltd_field_list_t *src_list;
    const bcmltd_field_list_t *dst_list;
    const bcmltd_field_desc_t *src_field;
    const bcmltd_field_desc_t *cur_subfield;
    bcmltd_field_t      *cur_lta_field;
    uint32_t src_field_id, src_field_idx, src_minbit, src_maxbit;
    uint32_t bitwidth;
    uint64_t bitmask;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_field_demux_transform \n")));

    if (arg->fields != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* src(logical)  : in (field)  */
    /* dst(physical) : out(rfield) */
    src_list = arg->field_list;
    dst_list = arg->rfield_list;

    src_field_id = arg->field[0];
    src_field_idx = 0;

    field_num = in->count;
    if (field_num == 0) {
        SHR_EXIT();
    }
    /* Search LTA input list for matching field */
    for (field_ix = 0; field_ix < field_num; field_ix++) {
        src_lta_field = in->field[field_ix];
        if ((src_lta_field->id == src_field_id) &&
            (src_lta_field->idx == src_field_idx)) {
            break;
        }
    }

    if (field_ix == field_num) {
        /* Field not found in LTA input list */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    rfield_ix = 0;
    rfield_num = out->count;
    for (field_ix = 0; field_ix < src_list->field_num; field_ix++) {

        src_field = &(src_list->field_array[field_ix]);
        src_field_id  = src_field->field_id;
        src_field_idx = src_field->field_idx;
        src_minbit = src_field->minbit;
        src_maxbit = src_field->maxbit;
        bitwidth = src_maxbit - src_minbit + 1;
        if ((bitwidth <= 0) || (bitwidth > 64)) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (bitwidth == 64) {
            bitmask = ~0ULL;
        } else {
            bitmask = (1ULL << bitwidth) - 1;
        }
        src_lta_data = (src_lta_field->data >> src_minbit) & bitmask;

        if (rfield_ix >= rfield_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        cur_subfield = &(dst_list->field_array[field_ix]);

        cur_lta_field = out->field[rfield_ix];
        cur_lta_field->id = cur_subfield->field_id;
        cur_lta_field->idx = cur_subfield->field_idx;
        cur_lta_field->data = src_lta_data;

        rfield_ix++;
    }

    out->count = rfield_ix;

    SHR_EXIT();
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_field_mux_transform(int unit,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_transform_arg_t *arg)
{
    uint32_t field_ix, field_num, ifield_ix, ifield_num;
    uint64_t src_value = 0;
    bcmltd_field_t *src_lta_field;
    const bcmltd_field_list_t *src_list;
    const bcmltd_field_list_t *dst_list;
    const bcmltd_field_desc_t *src_field;
    const bcmltd_field_desc_t *cur_subfield;
    bcmltd_field_t *cur_lta_field;
    uint32_t src_minbit, src_maxbit;
    uint64_t dst_data;
    uint32_t bitwidth;
    uint64_t bitmask;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_field_mux_transform \n")));

    /* src(logical)  : out(rfield) */
    /* dst(physical) : in (field)  */
    src_list = arg->rfield_list;
    dst_list = arg->field_list;

    if (out->count < 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ifield_num = in->count;
    field_num = dst_list->field_num;

    for (ifield_ix = 0; ifield_ix < ifield_num; ifield_ix++) {

        cur_lta_field = in->field[ifield_ix];

        /* Search subfield list for matching field */
        for (field_ix = 0; field_ix < field_num; field_ix++) {
            cur_subfield = &(dst_list->field_array[field_ix]);
            if ((cur_lta_field->id == cur_subfield->field_id) &&
                (cur_lta_field->idx == cur_subfield->field_idx)) {
                break;
            }
        }

        if (field_ix == field_num) {
            /* Subfield not found in LTA input list */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        dst_data = cur_lta_field->data;

        src_field = &(src_list->field_array[field_ix]);
        src_minbit = src_field->minbit;
        src_maxbit = src_field->maxbit;
        bitwidth = src_maxbit - src_minbit + 1;
        if ((bitwidth <= 0) || (bitwidth > 64)) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (bitwidth == 64) {
            bitmask = ~0ULL;
        } else {
            bitmask = (1ULL << bitwidth) - 1;
        }
        dst_data = ((dst_data & bitmask) << src_minbit);
        src_value |= dst_data;
    }

    /* Copy value to LTA output field list */
    src_lta_field = out->field[0];
    src_lta_field->data = src_value;

    if (arg->rfields != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    src_lta_field->id = arg->rfield[0];
    src_lta_field->idx = 0;

    out->count = 1;

    SHR_EXIT();
 exit:
    SHR_FUNC_EXIT();
}

