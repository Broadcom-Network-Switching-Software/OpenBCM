/*! \file bcmltx_array_to_bmp.c
 *
 * TM table's array to bitmap field Transform Handler
 *
 * This file contains field transform information for TM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmtm/bcmltx_array_to_bmp.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_TM

int
bcmltx_array_to_bmp_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    uint32_t array_idx;
    BCMLTD_BITDCL out_bmp[1] = {0};

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_array_to_bmp_transform \n")));

    for (array_idx = 0; array_idx < in->count; array_idx++) {
        if (in->field[array_idx]->data) {
            BCMLTD_BIT_SET(out_bmp, in->field[array_idx]->idx);
        }
    }
    out->count = 1;
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->data = out_bmp[0];

    SHR_FUNC_EXIT();
}


int
bcmltx_array_to_bmp_rev_transform(int unit,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg)
{
    uint32_t array_idx;
    BCMLTD_BITDCL in_bmp[1] = {0};

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_array_to_bmp_rev_transform \n")));

    for (array_idx = 0; array_idx < out->count; array_idx++) {
        out->field[array_idx]->id   = arg->rfield[0];
        in_bmp[0] = in->field[0]->data;
        out->field[array_idx]->data = (bool)BCMLTD_BIT_GET(in_bmp, array_idx);
        out->field[array_idx]->idx  = array_idx;
    }

    SHR_FUNC_EXIT();
}
