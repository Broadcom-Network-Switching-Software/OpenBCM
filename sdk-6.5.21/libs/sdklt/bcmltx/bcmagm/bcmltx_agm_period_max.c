/*! \file bcmltx_agm_period_max.c
 *
 * Mirror Egress MTP Index Set Handler.
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
#include <bcmltx/bcmagm/bcmltx_agm_period_max.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_AGM

int
bcmltx_agm_period_max_rev_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    int period, offset;

    SHR_FUNC_ENTER(unit);

    if ((in->count < 1) || (arg->rfields < 1) || (arg->values < 1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    offset = arg->value[0];
    period = in->field[0]->data;

    out->count = 0;
    out->field[out->count]->data = period + offset;
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->idx  = 0;
    out->count++;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_agm_period_max_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    int period, offset;

    SHR_FUNC_ENTER(unit);

    if ((in->count < 1) || (arg->rfields < 1) || (arg->values < 1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    offset = arg->value[0];
    period = in->field[0]->data;

    out->field[0]->data = period - offset;
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->idx  = 0;
    out->count = 1;

 exit:
    SHR_FUNC_EXIT();
}

