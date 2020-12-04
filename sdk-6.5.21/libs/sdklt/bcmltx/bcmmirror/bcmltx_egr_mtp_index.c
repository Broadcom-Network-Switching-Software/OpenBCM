/*! \file bcmltx_egr_mtp_index.c
 *
 * Mirror Egress MTP Index Set Handler.
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
#include <bcmltx/bcmmirror/bcmltx_egr_mtp_index.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

int
bcmltx_egr_mtp_index_rev_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    int mirror_id, member, shift;

    SHR_FUNC_ENTER(unit);

    if ((in->count < 1) || (arg->rfields < 2) || (arg->values < 1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    shift     = arg->value[0];
    mirror_id = (in->field[0]->data) >> shift;
    member    = in->field[0]->data & ((1 << shift) - 1);

    out->count = 0;

    /* MIRROR_INSTANCE_ID */
    out->field[out->count]->data = mirror_id;
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

    if (shift != 0) {
        out->field[out->count]->data = member;
    } else {
        /* MIRROR_MEMBER_ID is 0 all the time for trunk not supported device */
        out->field[out->count]->data = 0;
    }
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_egr_mtp_index_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    int mirror_id, member, shift;

    SHR_FUNC_ENTER(unit);

    if ((in->count < 2) || (arg->rfields < 1) || (arg->values < 1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    shift     = arg->value[0];
    mirror_id = in->field[0]->data;
    member    = in->field[1]->data;

    out->field[0]->data = (mirror_id << shift) + member;
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->idx  = 0;
    out->count = 1;

 exit:
    SHR_FUNC_EXIT();
}
