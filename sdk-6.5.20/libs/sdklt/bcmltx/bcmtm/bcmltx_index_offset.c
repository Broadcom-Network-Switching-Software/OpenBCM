/*! \file bcmltx_index_offset.c
 *
 * TM Index offset transform Handler
 *
 * This file contains field transform information for TM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltd/bcmltd_handler.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmtm/bcmltx_index_offset.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_TM

int
bcmltx_index_offset_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;
    if (arg->values) {
        /* __INDEX */
        out->field[out->count]->data = in->field[0]->data + arg->value[0];
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->count++;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_index_offset_rev_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    if (arg->values) {
        /*! __INDEX */
        out->field[0]->id = arg->rfield[0];
        out->field[0]->idx = 0;
        out->field[0]->data = in->field[0]->data - arg->value[0];
        out->count++;
    }
exit:
    SHR_FUNC_EXIT();
}
