/*! \file bcmltx_fld_val_xfrm.c
 *
 * TM field value transform handler
 *
 * This file contains field value transforms for TM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmltx/bcmtm/bcmltx_obm_thd_granularity.h>

#define BSL_LOG_MODULE  BSL_LS_BCMLTX_TM

#define MAX_OBM_THD_NUM     (8)

#define ROUND_UP            (0)
#define ROUND_DOWN          (1)

/*******************************************************************************
 * Public functions
 */
int
bcmltx_obm_thd_granularity_transform(int unit,
                                     const bcmltd_fields_t *in_key,
                                     const bcmltd_fields_t *in_value,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    uint32_t fld_val[MAX_OBM_THD_NUM];
    uint32_t fld_num = 0;
    uint32_t i, offset = 0;
    bcmtm_lport_t lport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in_key, SHR_E_PARAM);
    SHR_NULL_CHECK(in_value, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in_value->count % 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else {
        fld_num = in_value->count / 2;
        if (fld_num > MAX_OBM_THD_NUM) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (arg->values < (2 + fld_num)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->value[1] == ROUND_UP) {
        offset = arg->value[0] - 1;
    }

    lport = in_key->field[0]->data;

    for (i = 0; i < fld_num; i++) {
        fld_val[i] = in_value->field[i]->data;
        bcmtm_obm_thd_set(unit, lport, arg->value[i+2], fld_val[i]);
    }

    out->count = 0;
    for (i = 0; i < fld_num; i++) {
        out->field[out->count]->data =
            (fld_val[i] + offset) / arg->value[0];
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->count++;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_obm_thd_granularity_rev_transform(int unit,
                                         const bcmltd_fields_t *in_key,
                                         const bcmltd_fields_t *in_value,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg)
{
    uint32_t fld_val[MAX_OBM_THD_NUM];
    uint32_t i;
    bcmtm_lport_t lport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in_key, SHR_E_PARAM);
    SHR_NULL_CHECK(in_value, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in_value->count > MAX_OBM_THD_NUM ) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->values < (2 + in_value->count)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    lport = in_key->field[0]->data;

    for (i = 0; i < in_value->count; i++) {
        bcmtm_obm_thd_get(unit, lport, arg->value[i+2], &fld_val[i]);
    }

    out->count = 0;
    for (i = 0; i < in_value->count; i++) {
        out->field[out->count]->data = fld_val[i];
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->count++;
    }
    for (i = 0; i < in_value->count; i++) {
        out->field[out->count]->data = in_value->field[i]->data * arg->value[0];
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->count++;
    }
exit:
    SHR_FUNC_EXIT();
}

