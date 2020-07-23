/*! \file bcmltx_dlb_group_stats_id.c
 *
 * DLB_ECMP/LAG_GROUP_STATS transform utilities for DLB_ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmdrd_config.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltx/bcmdlb/bcmltx_dlb_group_stats_id.h>
#include <bcmltd/bcmltd_bitop.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

int
bcmltx_dlb_group_stats_id_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->data = arg->value[0];
    out->count++;

exit:
    SHR_FUNC_EXIT();
}
