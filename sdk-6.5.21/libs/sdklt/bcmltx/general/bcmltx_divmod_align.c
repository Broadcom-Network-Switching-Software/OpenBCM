/*! \file bcmltx_divmod_align.c
 *
 * This transform calculates quotient from dividend and divisor.
 * It will return a parameter error if the remainder is non-zero.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/general/bcmltx_divmod_align.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_divmod_align_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if ((arg->values != 1) || (arg->value == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"\t transform argument missing\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* The provided dividend must be a multiple of the divisor. */
    if ((in->field[0]->data % arg->value[0]) != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,"\t %d is not a multiple of %d\n"),
                     (uint32_t) (in->field[0]->data & 0xffffffff),
                     arg->value[0]));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Quotient is the output field */
    out->field[0]->data = in->field[0]->data / arg->value[0];
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

exit:
    SHR_FUNC_EXIT();
}

