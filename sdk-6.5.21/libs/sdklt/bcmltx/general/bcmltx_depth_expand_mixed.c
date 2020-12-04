/*! \file bcmltx_depth_expand_mixed.c
 *
 * Physical table heterogeneous depth expansion key transform.
 *
 * This allows a depth transform to work across memories of
 * different sizes. The transform_arg values are the sizes
 * used for each memory. This is generally, but not necessarily,
 * the size of the memory.
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
#include <bcmltx/general/bcmltx_depth_expand_mixed.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_depth_expand_mixed_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    uint32_t pidx;
    uint32_t sel;
    uint32_t msize;
    uint32_t mbase;
    uint32_t lidx;
    bool pidx_set;

    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    mbase = 0;
    pidx = 0;
    pidx_set = false;
    lidx = in->field[0]->data;
    for ( sel = 0; sel < arg->values; sel++) {
        msize = arg->value[sel];
        if (lidx >= mbase && lidx <= (mbase+msize-1)) {
            pidx = lidx - mbase;
            pidx_set = true;
            break;
        }
        mbase += msize;
    }

    if (pidx_set != true) {
        /* lidx not found. */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* __INDEX */
    out->field[0]->data = pidx;
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
bcmltx_depth_expand_mixed_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint32_t msize;
    uint32_t mbase;
    uint32_t sel;
    uint32_t tsel;
    uint32_t pidx;

    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pidx = in->field[0]->data;
    tsel = in->field[1]->data;

    if (tsel >= arg->values) {
        /* Unexpected */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Calculate base logical index of selected memory.*/
    mbase = 0;
    for ( sel = 0; sel < tsel; sel++) {
        msize = arg->value[sel];
        mbase += msize;
    }

    /* _ID = mbase + __INDEX */
    out->field[0]->data = mbase + pidx;
    out->field[0]->id = arg->rfield[0];
    out->count = 1;

exit:
    SHR_FUNC_EXIT();
}
