/*! \file bcmltx_thd_mask_fld_sel.c
 *
 * SEC Table operational status.
 *
 * This file contains field transform for operational state.
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
#include <bcmltx/bcmsec/bcmltx_thd_mask_fld_sel.h>
#include <bcmtm/bcmtm_utils.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_SEC

int
bcmltx_thd_mask_fld_sel_transform (int unit,
                                   const bcmltd_fields_t *in_key,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    uint32_t id, len;
    SHR_FUNC_ENTER(unit);

    id = (uint32_t)(in_key->field[0]->data);

    len = (uint32_t)(in->field[0]->data);


    out->field[id]->data = len;
    out->field[id]->idx = 0;
    out->field[id]->id = arg->rfield[id];

    SHR_FUNC_EXIT();
}

int
bcmltx_thd_mask_fld_sel_rev_transform(int unit,
                                      const bcmltd_fields_t *in_key,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    uint32_t id, len;
    SHR_FUNC_ENTER(unit);

    id = (uint32_t)(in_key->field[0]->data);

    len = (uint32_t)(in->field[id]->data);


    out->field[0]->data = len;
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

    SHR_FUNC_EXIT();
}
