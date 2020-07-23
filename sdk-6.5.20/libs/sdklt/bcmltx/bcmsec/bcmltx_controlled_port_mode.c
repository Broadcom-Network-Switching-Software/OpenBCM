/*! \file bcmltx_controlled_port_mode.c
 *
 * SEC Table operational status.
 *
 * This file contains secured data transforms.
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
#include <bcmltx/bcmsec/bcmltx_controlled_port_mode.h>
#include <bcmtm/bcmtm_utils.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_SEC

int
bcmltx_controlled_port_mode_transform (int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    uint32_t mode, enable, forward;
    SHR_FUNC_ENTER(unit);

    mode = (uint32_t)(in->field[0]->data);

    switch (mode) {
        case 0:
            enable = 0; forward = 0;
            break;
        case 1:
            enable = 1; forward = 0;
            break;
        case 2:
            enable = 1; forward = 1;
            break;
        default:
            enable = 0; forward = 0;
            break;
    }

    /* Secured data packet enable. */
    out->field[0]->data = enable;
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

    /* Secured forward enable. */
    out->field[1]->data = forward;
    out->field[1]->idx = 0;
    out->field[1]->id = arg->rfield[1];

    SHR_FUNC_EXIT();
}

int
bcmltx_controlled_port_mode_rev_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    uint32_t mode, enable, forward;
    SHR_FUNC_ENTER(unit);

    enable = (uint32_t)(in->field[0]->data);
    forward = (uint32_t)(in->field[1]->data);

    if (enable == 1) {
        mode = forward ? 2 : 1;
    } else {
        mode = 0;
    }

    /* Controlled port mode. */
    out->field[0]->data = mode;
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

    SHR_FUNC_EXIT();
}
