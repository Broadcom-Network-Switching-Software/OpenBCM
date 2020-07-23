/*! \file bcmltx_egr_encap_ctrl_entry_type.c
 *
 * Egress Encap Control Entry Type Handler.
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
#include <bcmltx/bcmmirror/bcmltx_egr_encap_ctrl_entry_type.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

/* Refer to enum MIRROR_TRUNCATE_ACTION_T.TRUNCATE_AND_ZERO */
#define MIRROR_TRUNCATE_AND_ZERO 2

int
bcmltx_egr_encap_ctrl_entry_type_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    bool encap = 0;
    uint32_t truncate = 0;
    uint32_t encap_id = 0;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        /* ENCAP */
        if (in->field[i]->id == arg->field[0]) {
            encap = in->field[i]->data & 0x1;
        }
        /* TRUNCATE_ACTION */
        if (in->field[i]->id == arg->field[1]) {
            truncate = in->field[i]->data & 0xFFFFFFFF;
        }
        /* MIRROR_ENCAP_ID */
        if (in->field[i]->id == arg->field[2]) {
            encap_id = in->field[i]->data & 0xFFFFFFFF;
        }
    }

    out->count = 0;
    if (encap == 0) {
        /*
         * ENTRY_TYPE must be non-zero reserved value (0xB ~ 0xF) for handling
         * payload wiping non-tunneled packets.
         */
        if (truncate == MIRROR_TRUNCATE_AND_ZERO) {
            /* Program ENTRY_TYPE with 0xF */
            out->field[out->count]->data = 0xF;
            out->field[out->count]->id   = arg->rfield[0];
            out->field[out->count]->idx  = 0;
            out->count++;
        } else {
            out->field[out->count]->data = 0x0;
            out->field[out->count]->id   = arg->rfield[0];
            out->field[out->count]->idx  = 0;
            out->count++;
        }

        /* Program EGR_MIRROR_ENCAP_CONTROL.__INDEX with MIRROR_ENCAP_ID */
        out->field[out->count]->data = encap_id;
        out->field[out->count]->id   = arg->rfield[1];
        out->field[out->count]->idx  = 0;
        out->count++;
    }

    SHR_FUNC_EXIT();
}

