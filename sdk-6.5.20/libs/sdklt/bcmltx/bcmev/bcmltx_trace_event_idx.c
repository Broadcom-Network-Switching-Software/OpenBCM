/*! \file bcmltx_trace_event_idx.c
 *
 * IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.__TABLE_SEL Transform Handler
 *
 * This file contains field transform information for
 * IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.__TABLE_SEL.
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
#include <bcmltx/bcmev/bcmltx_trace_event_idx.h>
#include <bcmdrd/bcmdrd_dev.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_EV

int
bcmltx_trace_event_idx_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)
{
    int event_idx;

    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if ((in->count != 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    event_idx = in->field[0]->data;

    LOG_VERBOSE(BSL_LS_BCMLTX_EV,
                (BSL_META_U(unit,
                            "event index = %d\n"), event_idx));

    out->count = 0;

    /* ouput __TABLE_SEL */
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->data = event_idx;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_trace_event_idx_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if ((in->count != 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;

    /* ouput TRACE_EVENT_ID */
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->data = in->field[out->count]->data;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}
