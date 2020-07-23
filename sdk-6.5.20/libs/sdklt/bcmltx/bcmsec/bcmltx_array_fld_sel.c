/*! \file bcmltx_array_fld_sel.c
 *
 * SEC table's LT key and value pair to PT fields Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmltx/bcmsec/bcmltx_array_fld_sel.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMSEC_LTX


/*******************************************************************************
 * Global definitions
 */
int
bcmltx_array_fld_sel_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    for (i = 0; i < out->count; i++) {
        out->field[i]->data = in->field[i]->data;
        out->field[i]->idx = 0;
        out->field[i]->id = arg->rfield[i];
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_array_fld_sel_rev_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    for (i = 0; i < in->count; i++) {
        out->field[i]->data = in->field[i]->data;
        out->field[i]->idx = i;
        out->field[i]->id = arg->rfield[0];
    }

exit:
    SHR_FUNC_EXIT();
}
