/*! \file bcmltx_ldh_mon_port.c
 *
 * Latency monitor ingress/egress port handler.
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
#include <bcmltx/bcmldh/bcmltx_ldh_mon_port.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_LDH

int
bcmltx_ldh_mon_port_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n port transform enter\n")));

    if (in->count < 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (out->count = 0; out->count < in->count; out->count++) {
        out->field[out->count]->data = in->field[out->count]->data;
        out->field[out->count]->id = arg->rfield[out->count];
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_ldh_mon_port_rev_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    uint32_t idx;
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n rev transform enter\n")));

    if (in->count < 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = out->count = 0; idx < in->count; idx++) {
        out->field[idx]->id = arg->rfield[0];
        out->field[idx]->data = in->field[idx]->data;
        out->field[idx]->idx = idx;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}
