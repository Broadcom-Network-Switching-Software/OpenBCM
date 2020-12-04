/*! \file bcmltx_valid_enable.c
 *
 * Set the enable/valid field given a non-zero input.
 *
 * This file contains field transform information for Port.
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
#include <bcmltx/bcmport/bcmltx_valid_enable.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT
#define PBMP_SIZE  (MAX_PORTS/sizeof(uint64_t))


int
bcmltx_valid_enable_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    uint32_t array_idx;

    SHR_FUNC_ENTER(unit);

    out->field[0]->id   = arg->rfield[0];
    out->field[0]->idx  = 0;
    out->field[0]->data = 0;

    for (array_idx = 0; array_idx < in->count; array_idx++) {
        if (in->field[array_idx]->data) {
            out->field[0]->data = 1;
            break;
        }
    }

    SHR_FUNC_EXIT();
}

