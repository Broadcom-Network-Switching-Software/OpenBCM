/*! \file bcmltx_mon_gen_index.c
 *
 * Egress drop mask profile LT key combination to
 * LTM track index transform handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/bcmmon/bcmltx_mon_gen_index.h>

/* BSL Module */
#define BSL_LOG_MODULE  BSL_LS_BCMLTX_MON

/*******************************************************************************
 * Public functions
 */
int
bcmltx_mon_gen_index_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if ((in->count != 2)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->values != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->field[0]->data = (in->field[0]->data << arg->value[0]) + in->field[1]->data;
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

    out->count = 1;
exit:
    SHR_FUNC_EXIT();

    return 0;
}

int
bcmltx_mon_gen_index_rev_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    uint32_t track_idx;

    SHR_FUNC_ENTER(unit);

    if ((in->count != 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    track_idx = in->field[0]->data;

    /* First index */
    out->field[0]->data = track_idx >> arg->value[0];
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;

    /* Second index */
    out->field[1]->data = track_idx & (( 1 << arg->value[0]) - 1);
    out->field[1]->id = arg->rfield[1];
    out->field[1]->idx = 0;

    out->count = 2;
exit:
    SHR_FUNC_EXIT();
}
