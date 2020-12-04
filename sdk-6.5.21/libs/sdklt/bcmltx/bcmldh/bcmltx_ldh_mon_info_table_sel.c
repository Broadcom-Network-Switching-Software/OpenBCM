/*! \file bcmltx_ldh_mon_info_table_sel.c
 *
 * Latency monitor info table handler.
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
#include <bcmltx/bcmldh/bcmltx_ldh_mon_info_table_sel.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_LDH

int
bcmltx_ldh_mon_info_table_sel_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n info table select transform enter\n")));

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;
    out->field[out->count]->data = in->field[0]->data;
    out->field[out->count]->id = arg->rfield[0];

    out->count ++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_ldh_mon_info_table_sel_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n info table select rev transform enter\n")));

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = in->field[0]->data;

    out->count ++;

    SHR_FUNC_EXIT();
}
