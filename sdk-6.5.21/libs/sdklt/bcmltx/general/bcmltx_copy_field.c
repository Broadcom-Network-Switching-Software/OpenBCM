/*! \file bcmltx_copy_field.c
 *
 * This transform simply copies fields from input to the output.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/general/bcmltx_copy_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_copy_field_transform(int unit,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (in->count != out->count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < in->count; i++) {
        out->field[i]->data = in->field[i]->data;
        out->field[i]->idx = in->field[i]->idx;
        out->field[i]->id = arg->rfield[i];
    }

exit:
    SHR_FUNC_EXIT();

    return 0;
}

int
bcmltx_copy_field_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    return bcmltx_copy_field_transform(unit, in, out, arg);
}


