/*! \file bcmltx_depth_expand.c
 *
 * Physical table depth expansion key transform.
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
#include <bcmltx/general/bcmltx_depth_expand.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_depth_expand_transform(int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg)
{
    uint32_t idx;
    uint32_t sel;
    uint32_t msize;

    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    msize = arg->value[0];
    idx = in->field[0]->data % msize;
    sel = in->field[0]->data / msize;

    /* __INDEX */
    out->field[0]->data = idx;
    out->field[0]->id = arg->rfield[0];

    /* __TABLE_SEL */
    out->field[1]->data = sel;
    out->field[1]->id = arg->rfield[1];
    out->count = 2;

exit:
    SHR_FUNC_EXIT();

    return 0;
}

int
bcmltx_depth_expand_rev_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    uint32_t msize;

    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    msize = arg->value[0];
    /* _ID = __TABLE_SEL*msize + __INDEX */
    out->field[0]->data = (msize * in->field[1]->data) + in->field[0]->data;
    out->field[0]->id = arg->rfield[0];
    out->count = 1;

exit:
    SHR_FUNC_EXIT();
}


