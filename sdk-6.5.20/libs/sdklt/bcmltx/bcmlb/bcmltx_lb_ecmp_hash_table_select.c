/*! \file bcmltx_lb_ecmp_hash_table_select.c
 *
 * DESCRIPTION
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
#include <bcmltx/bcmlb/bcmltx_lb_ecmp_hash_table_select.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_lb_ecmp_hash_table_select_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint32_t idx;
    uint32_t sel;

    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    idx = in->field[0]->data;
    sel = in->field[1]->data ? 1 : 0;

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
bcmltx_lb_ecmp_hash_table_select_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* __INDEX */
    out->field[0]->data = in->field[0]->data;
    out->field[0]->id = arg->rfield[0];

    /* __TABLE_SEL */
    out->field[1]->data = in->field[1]->data;
    out->field[1]->id = arg->rfield[1];
    out->count = 2;

exit:
    SHR_FUNC_EXIT();
}

