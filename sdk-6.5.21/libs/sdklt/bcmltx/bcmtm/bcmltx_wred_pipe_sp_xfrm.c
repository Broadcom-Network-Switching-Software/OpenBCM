/*! \file bcmltx_wred_pipe_sp_xfrm.c
 *
 * APIs for wred pipe service pool transform functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmtm/bcmltx_wred_pipe_sp_idx.h>
#include <bcmltx/bcmtm/bcmltx_wred_pipe_sp_field.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_TM

/*******************************************************************************
 * Public functions
 */
int
bcmltx_wred_pipe_sp_idx_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* Index */
    out->field[out->count]->data = in->field[1]->data + arg->value[0];
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

    /* Table select */
    out->field[out->count]->data = in->field[0]->data;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_wred_pipe_sp_idx_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* PIPE */
    out->field[out->count]->data = in->field[1]->data;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

    /* TM_WRED_PIPE_SERVICE_POOL_ID */
    out->field[out->count]->data = in->field[0]->data - arg->value[0];
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_wred_pipe_sp_field_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    uint32_t count = 0, id = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;
    while (count  < in->count) {
        switch(in->field[count]->id) {
            case TM_WRED_PIPE_SERVICE_POOLt_CURRENT_Q_SIZEf:
                id = arg->rfield[0];
                break;
            case TM_WRED_PIPE_SERVICE_POOLt_WEIGHTf:
                id = arg->rfield[1];
                break;
            case TM_WRED_PIPE_SERVICE_POOLt_WREDf:
                id = arg->rfield[2];
                break;
            case TM_WRED_PIPE_SERVICE_POOLt_TM_WRED_TIME_PROFILE_IDf:
                id = arg->rfield[3];
                break;
            case TM_WRED_PIPE_SERVICE_POOLt_TM_WRED_DROP_CURVE_SET_PROFILE_IDf:
                id = arg->rfield[4];
                break;
            default:
                break;
        }
        out->field[count]->data = in->field[count]->data;
        out->field[count]->id = id;
        out->field[count]->idx = 0;
        count++;
    }
    out->count = count;
exit:
    SHR_FUNC_EXIT();
}


int
bcmltx_wred_pipe_sp_field_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint32_t count = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;
    while (count  < in->count) {
        out->field[count]->data = in->field[count]->data;
        out->field[count]->id = arg->rfield[count];
        out->field[count]->idx = 0;
        count++;
    }
    out->count = count;
exit:
    SHR_FUNC_EXIT();
}
