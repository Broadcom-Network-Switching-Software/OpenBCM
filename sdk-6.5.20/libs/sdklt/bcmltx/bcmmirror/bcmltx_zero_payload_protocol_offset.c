/*! \file bcmltx_zero_payload_protocol_offset.c
 *
 * Mirror zero payload protocol value handler.
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
#include <bcmltx/bcmmirror/bcmltx_zero_payload_protocol_offset.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

int
bcmltx_zero_payload_protocol_offset_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint32_t cnt = 0;

    SHR_FUNC_ENTER(unit);

    out->count = 0;

    /*
     * Program IP_PROTOCOL_0/1/2_OFFSET to IP_PROTOCOL_OFFSET[]
     */
    for (cnt = 0; cnt < in->count; cnt++) {
        out->field[out->count]->data = in->field[cnt]->data;
        out->field[out->count]->id   = arg->rfield[0];
        out->field[out->count]->idx  = cnt;
        out->count++;

    }
    SHR_FUNC_EXIT();
}

int
bcmltx_zero_payload_protocol_offset_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint32_t ary_val = 0;
    uint32_t ary_idx = 0;
    uint32_t cnt = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Program IP_PROTOCOL_OFFSET[] to IP_PROTOCOL_0/1/2_OFFSET
     */
    out->count = 0;
    for (cnt = 0; cnt < in->count; cnt++) {
        /* the array LT field IP_PROTOCOL_OFFSET */
        if (in->field[cnt]->id == arg->field[0]) {
            ary_idx = in->field[cnt]->idx;
            ary_val = in->field[cnt]->data;
            out->field[out->count]->data = ary_val;
            out->field[out->count]->id   = arg->rfield[ary_idx];
            out->field[out->count]->idx  = 0;
            out->count++;
        }
    }
    SHR_FUNC_EXIT();

}

