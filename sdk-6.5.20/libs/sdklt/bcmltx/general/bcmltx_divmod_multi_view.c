/*! \file bcmltx_divmod_multi_view.c
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
#include <bcmltx/general/bcmltx_divmod_multi_view.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_divmod_multi_view_transform(int unit,
                                   const bcmltd_fields_t *in_key,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    uint64_t sel_val = 0;

    SHR_FUNC_ENTER(unit);

    if ((arg->values != 1) || (arg->value == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"\t transform argument missing\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* INDEX is the first output. */
    out->field[0]->data = in_key->field[0]->data / arg->value[0];
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

    /* For SELECTOR, calculate value from view and index. */
    sel_val = in->field[0]->data * arg->value[0];
    sel_val += in_key->field[0]->data % arg->value[0];
    out->field[1]->data = sel_val;
    out->field[1]->idx = 0;
    out->field[1]->id = arg->rfield[1];

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_divmod_multi_view_rev_transform(int unit,
                                   const bcmltd_fields_t *in_key,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    return 0;
}
