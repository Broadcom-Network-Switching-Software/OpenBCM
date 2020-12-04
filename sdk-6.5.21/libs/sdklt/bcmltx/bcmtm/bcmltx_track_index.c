/*! \file bcmltx_track_index.c
 *
 * TM table's LT key combination to LTM track index transform handler.
 *
 * Some LTs have multiple keys, and each key combination should be associated to
 * one logical entry. In such cases, a transform function should be provided to
 * let LTM track the entries as it is a 1D indexed table.
 * The transform functions in this files is generic so that they can be applied
 * to all PTs with such fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmtm/bcmltx_track_index.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_TM

/*******************************************************************************
 * Public functions
 */
int
bcmltx_track_index_transform(int unit,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_transform_arg_t *arg)
{
    uint32_t idx = 0;
    uint32_t track_idx;
    size_t width;
    bcmltd_sid_t ltid;
    const bcmlrd_field_data_t *field_data;

    SHR_FUNC_ENTER(unit);

    ltid = arg->comp_data->sid;
    /* __TRACK_INDEX */
    track_idx = in->field[0]->data;
    for (idx = 1; idx < in->count; idx++) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_get(unit, ltid, in->field[idx]->id, &field_data));
        width = field_data->width;
        track_idx = (track_idx << width) + in->field[idx]->data;
    }
    out->field[0]->data = track_idx;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_track_index_rev_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    int idx = 0;
    size_t width;
    uint32_t track_idx;
    bcmltd_sid_t ltid;
    const bcmlrd_field_data_t *field_data;

    SHR_FUNC_ENTER(unit);

    ltid = arg->comp_data->sid;
    track_idx = in->field[0]->data;
    for (idx =  out->count - 1; idx > 0; idx--) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_get(unit, ltid, arg->rfield[idx], &field_data));
        width = field_data->width;

        out->field[idx]->data = track_idx & (( 1 << width) - 1);
        out->field[idx]->id = arg->rfield[idx];
        out->field[idx]->idx = 0;
        track_idx >>= width;
    }
    /* First index */
    out->field[idx]->data = track_idx;
    out->field[idx]->id = arg->rfield[idx];
    out->field[idx]->idx = 0;
exit:
    SHR_FUNC_EXIT();
}
