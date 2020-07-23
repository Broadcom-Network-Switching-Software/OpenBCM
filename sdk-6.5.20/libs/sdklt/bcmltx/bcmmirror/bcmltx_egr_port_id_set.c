/*! \file bcmltx_egr_port_id_set.c
 *
 * Mirror Egress Port ID Set Handler.
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
#include <bcmltx/bcmmirror/bcmltx_egr_port_id_set.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

int
bcmltx_egr_port_id_set_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if ((in->count < 1) || (arg->rfields < 1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    out->count=0;
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->data = in->field[0]->data;
    out->field[out->count]->idx  = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}
