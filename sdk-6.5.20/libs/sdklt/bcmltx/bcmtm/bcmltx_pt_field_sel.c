/*! \file bcmltx_pt_field_sel.c
 *
 * TM table's LT key and value pair to PT fields Transform Handler.
 *
 * Some PTs are designed to represent 2D table in 1D. Therefore, one of the
 * dimensions is encoded in the fields name such as PGx_SPID. However, the
 * corresponding LTs are usually 2D (2 key fields). Therefore, one of the key
 * fields is used as PT field selection. The transform functions in this files
 * is generic so that they can be applied to all PTs with such fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmltx/bcmtm/bcmltx_pt_field_sel.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_TM


/*******************************************************************************
 * Global definitions
 */
int
bcmltx_pt_field_sel_transform(int unit,
                              const bcmltd_fields_t *in_key,
                              const bcmltd_fields_t *in_value,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    uint32_t pt_field_sel;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in_key, SHR_E_PARAM);
    SHR_NULL_CHECK(in_value, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 1;
    pt_field_sel = in_key->field[0]->data;

    out->field[0]->id = arg->rfield_list->field_array[pt_field_sel].field_id;
    out->field[0]->idx = 0;
    out->field[0]->data = in_value->field[0]->data;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_pt_field_sel_rev_transform(int unit,
                                  const bcmltd_fields_t *in_key,
                                  const bcmltd_fields_t *in_value,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    uint32_t pt_field_sel = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in_key, SHR_E_PARAM);
    SHR_NULL_CHECK(in_value, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in_key->field[0]) {
        pt_field_sel = in_key->field[0]->data;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    out->count = 1;
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->data = in_value->field[pt_field_sel]->data;
    out->field[0]->idx = 0;
exit:
    SHR_FUNC_EXIT();
}
