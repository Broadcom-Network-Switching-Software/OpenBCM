/*! \file bcmltx_table_sel.c
 *
 * This transform selects the out tables based on input values.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/general/bcmltx_table_sel.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_table_sel_transform(int unit,
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
        out->field[i]->id = arg->rfield[i];
        out->field[i]->idx = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_table_sel_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    return bcmltx_table_sel_transform(unit, in, out, arg);
}

