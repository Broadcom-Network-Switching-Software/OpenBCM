/*! \file bcmltx_port_array_to_pbmp.c
 *
 * TM table's array to port bitmap field Transform Handler
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
#include <bcmltd/chip/bcmltd_vlan_constants.h>
#include <bcmltx/bcmport/bcmltx_port_array_to_pbmp.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT
#define PBMP_SIZE  (MAX_PORTS/sizeof(uint64_t))


int
bcmltx_port_array_to_pbmp_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    uint32_t array_idx;
    BCMLTD_BITDCL out_pbmp[PBMP_SIZE] = {0};

    SHR_FUNC_ENTER(unit);

    for (array_idx = 0; array_idx < in->count; array_idx++) {
        if (in->field[array_idx]->data) {
            BCMLTD_BIT_SET(out_pbmp, in->field[array_idx]->idx);
        } else {
            BCMLTD_BIT_CLR(out_pbmp, in->field[array_idx]->idx);
        }
    }

    for (array_idx = 0; array_idx < out->count; array_idx++) {
        out->field[array_idx]->id = arg->rfield[0];
        out->field[array_idx]->idx = array_idx;
        out->field[array_idx]->data = out_pbmp[array_idx];
    }
    SHR_FUNC_EXIT();
}

int
bcmltx_port_array_to_pbmp_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint32_t array_idx;
    BCMLTD_BITDCL in_pbmp[PBMP_SIZE] = {0};

    SHR_FUNC_ENTER(unit);

    for (array_idx = 0 ; array_idx < in->count; array_idx++) {
        in_pbmp[array_idx] = in->field[array_idx]->data;
    }

    for (array_idx = 0; array_idx < out->count; array_idx++) {
        out->field[array_idx]->id = arg->rfield[0];
        out->field[array_idx]->data = (bool)BCMLTD_BIT_GET(in_pbmp, array_idx);
        out->field[array_idx]->idx = array_idx;
    }
    SHR_FUNC_EXIT();
}

