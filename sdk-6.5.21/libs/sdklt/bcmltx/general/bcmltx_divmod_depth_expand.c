/*! \file bcmltx_divmod_depth_expand.c
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
#include <bcmltx/general/bcmltx_divmod_depth_expand.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_divmod_depth_expand_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    uint32_t tbl_idx;
    uint32_t entry_idx;
    uint32_t msize;
    uint32_t div_val;
    uint32_t sel;
    uint32_t tbl_sel;

    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    msize = arg->value[0];
    div_val = arg->value[1];

    tbl_idx = in->field[0]->data % msize;
    entry_idx = tbl_idx / div_val;
    tbl_sel = in->field[0]->data / msize;
    sel = tbl_idx % div_val;

    /* __INDEX */
    out->field[0]->data = entry_idx;
    out->field[0]->id = arg->rfield[0];

    /* __TABLE_SEL */
    out->field[1]->data = tbl_sel;
    out->field[1]->id = arg->rfield[1];

    /* __SELECTOR */
    out->field[2]->data = sel;
    out->field[2]->id = arg->rfield[2];

    out->count = 3;

exit:
    SHR_FUNC_EXIT();

    return 0;
}
