/*! \file bcmltx_mod_id_set.c
 *
 * Mirror ModID Transform Handler.
 * transform for ModID which converts logical array MODID[] to
 * to a physical ModID0~ModIDx.
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
#include <bcmltx/bcmmirror/bcmltx_mod_id_set.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

int
bcmltx_mod_id_set_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    size_t cnt = 0, idx;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mod_id_set_rev_transform \n")));

    out->count = 0;
    for (cnt = 0; cnt < in->count; cnt++) {
        /*
         * Supposed that the PT fields order in map file is always same as
         * the order in LT MODID[].
         */
        for (idx = 0; idx < arg->fields; idx++) {
            if (in->field[cnt]->id == arg->field[idx]) {
                out->field[out->count]->id   = arg->rfield[0];
                out->field[out->count]->data = in->field[cnt]->data;
                out->field[out->count]->idx  = idx;
                out->count++;
            }
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmltx_mod_id_set_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    size_t cnt;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mod_id_set_transform \n")));

    out->count = 0;
    for (cnt = 0; cnt < in->count; cnt++) {
        /*
         * The input array will be the list of the LT fields, and we only
         * handle MIRROR_ING_INSTANCEt_MODIDf/MIRROR_EGR_INSTANCEt_MODIDf.
         */
        if (in->field[cnt]->id == MIRROR_ING_INSTANCEt_MODIDf ||
            in->field[cnt]->id == MIRROR_EGR_INSTANCEt_MODIDf) {
            /*
             * Supposed that the PT fields order in map file is always same as
             * the order in LT MODID[].
             */
            out->field[out->count]->idx  = 0;
            out->field[out->count]->id   = arg->rfield[in->field[cnt]->idx];
            out->field[out->count]->data = in->field[cnt]->data;
            out->count++;
        }
    }

    SHR_FUNC_EXIT();
}

