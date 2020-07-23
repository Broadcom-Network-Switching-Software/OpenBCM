/*! \file bcmltx_ldh_mon_latency_limit_select.c
 *
 * Latency monitor limit select handler.
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
#include <bcmltx/bcmldh/bcmltx_ldh_mon_latency_limit_select.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_LDH

int
bcmltx_ldh_mon_latency_limit_select_transform(int unit,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out,
                                              const bcmltd_transform_arg_t *arg)
{
    int queue_max;
    int bucket_max;

    SHR_FUNC_ENTER(unit);

    queue_max = arg->value[0];
    bucket_max = arg->value[1];

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n latency select transform enter\n")));

    if (in->count < 3) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    /* _INDEX, calculate the relative offset = q_id * bucket_max + bucket_id */
    out->field[out->count]->data = \
                                   bucket_max * in->field[1]->data + \
                                   in->field[2]->data;
    out->field[out->count]->idx  = 0;
    out->count++;

    /* _TABLE_SEL */
    out->field[out->count]->id = arg->rfield[1];
    out->field[out->count]->data = in->field[0]->data;
    out->field[out->count]->idx  = 0;
    out->count++;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n instance %"PRIu64" offset %"PRIu64" q_max %d "
                            "b_max %d exit\n"),
                            in->field[0]->data,
                            out->field[0]->data,
                            queue_max, bucket_max));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_ldh_mon_latency_limit_select_rev_transform(int unit,
                                                  const bcmltd_fields_t *in,
                                                  bcmltd_fields_t *out,
                                                  const bcmltd_transform_arg_t *arg)
{
    int bucket_max;

    SHR_FUNC_ENTER(unit);

    bucket_max = arg->value[1];

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n rev transform enter\n")));

    if (in->count < 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* _INDEX, queue id */
    out->count = 0;
    out->field[out->count]->id = arg->rfield[1];
    out->field[out->count]->data = in->field[0]->data / bucket_max;
    out->field[out->count]->idx = 0;
    out->count ++;

    /* _INDEX, bucket id */
    out->field[out->count]->id = arg->rfield[2];
    out->field[out->count]->data = in->field[0]->data % bucket_max;
    out->field[out->count]->idx = 0;
    out->count++;

    /* _TABLE_SEL */
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = in->field[1]->data;
    out->field[out->count]->idx = 0;
    out->count++;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n instance %"PRIu64" offset %"PRIu64" "
                            "q_id %"PRIu64" "
                            "b_id %"PRIu64" exit\n"),
                            in->field[1]->data,
                            in->field[0]->data,
                            in->field[0]->data / bucket_max,
                            in->field[0]->data % bucket_max));

exit:
    SHR_FUNC_EXIT();
}
