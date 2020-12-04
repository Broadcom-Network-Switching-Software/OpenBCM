/*! \file bcmltx_divmod.c
 *
 * This transform calculates quotient and remainder from
 * dividend and divisor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/general/bcmltx_divmod.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_divmod_transform(int unit,
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

    /* Quotient is first output field */
    out->field[0]->data = in->field[0]->data / arg->value[0];
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

    /* Remainder is second output field */
    out->field[1]->data = in->field[0]->data % arg->value[0];
    out->field[1]->idx = 0;
    out->field[1]->id = arg->rfield[1];

exit:
    SHR_FUNC_EXIT();
}

