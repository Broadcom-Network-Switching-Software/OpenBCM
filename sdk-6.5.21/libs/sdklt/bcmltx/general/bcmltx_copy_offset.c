/*! \file bcmltx_copy_offset.c
 *
 * Copy a field from input to output and apply an offset to the output
 * field. There is no reverse transform for this operation - it is
 * forward only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/general/bcmltx_copy_offset.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_copy_offset_transform(int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg)
{
    uint32_t i;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    if (in->count != out->count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->values != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    offset = arg->value[0];

    for (i = 0; i < in->count; i++) {
        out->field[i]->data = in->field[i]->data + offset;
        out->field[i]->idx = in->field[i]->idx;
        out->field[i]->id = arg->rfield[i];
    }

exit:
    SHR_FUNC_EXIT();

    return 0;
}
