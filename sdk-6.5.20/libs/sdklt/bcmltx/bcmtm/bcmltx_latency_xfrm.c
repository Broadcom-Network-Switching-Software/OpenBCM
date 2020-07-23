/*! \file bcmltx_latency_xfrm.c
 *
 * TM latency related transform handler
 *
 * This file contains table and field transforms for TM latency related
 * information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmltx/bcmtm/bcmltx_obm_latency_port_tbl_sel.h>
#include <bcmltx/bcmtm/bcmltx_obm_latency_fld_sel.h>

#define BSL_LOG_MODULE  BSL_LS_BCMLTX_TM
/*******************************************************************************
 * Private functions
 */
static int
bcmltx_field_list_set(int unit,
                      bcmltd_fields_t *flist,
                      bcmltd_fid_t fid,
                      uint32_t idx,
                      uint64_t val)
{
    size_t count =  flist->count;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(flist->field[count], SHR_E_INTERNAL);
    flist->field[count]->id = fid;
    flist->field[count]->data = val;
    flist->field[count]->idx = idx;
    flist->count++;
exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public functions
 */
int
bcmltx_obm_latency_port_tbl_sel_transform(int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    int pport, pipe, idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    lport = in->field[0]->data;

    if (!bcmtm_obm_port_validation(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(bcmtm_lport_pipe_get(unit, lport, &pipe))) {
        SHR_EXIT();
    }

    /* local physical port number. */
    pport = (pport - 1) % arg->value[0];

    if (bcmtm_lport_is_mgmt(unit, lport)) {
        /* TBA */
    } else {
        /* Instance */
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, pipe));
        /* Index */
        idx = in->field[1]->data / arg->value[1];
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, idx));
        /* Table select */
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, pport));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_obm_latency_port_tbl_sel_rev_transform(int unit,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out,
                                              const bcmltd_transform_arg_t *arg)
{

    int  lport, pipe, idx, tbl_id;
    bcmtm_pport_t pport;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    pipe = in->field[0]->data;
    idx = in->field[1]->data;
    tbl_id = in->field[2]->data;

    pport = pipe * arg->value[0] + tbl_id;

    SHR_IF_ERR_EXIT
        (bcmtm_pport_lport_get(unit, pport, &lport));

    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, lport));
    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0,
            idx * arg->value[1] + tbl_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_obm_latency_fld_sel_transform(int unit,
                                     const bcmltd_fields_t *in_key,
                                     const bcmltd_fields_t *in_value,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    uint32_t id_to_field_sel;
    uint32_t field_sel;

    SHR_FUNC_ENTER(unit);

    out->count = 0;
    id_to_field_sel = in_key->field[0]->data % arg->value[0];
    field_sel = in_value->field[0]->data % arg->value[0];

    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out,
            arg->rfield_list->field_array[id_to_field_sel].field_id, 0,
            field_sel));

exit:
    SHR_FUNC_EXIT();

}

int
bcmltx_obm_latency_fld_sel_rev_transform(int unit,
                                         const bcmltd_fields_t *in_key,
                                         const bcmltd_fields_t *in_value,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg)
{
    uint32_t id_to_field_sel = 0;

    SHR_FUNC_ENTER(unit);

    if (in_key->field[0]) {
        id_to_field_sel = in_key->field[0]->data % arg->value[0];
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    out->count = 0;

    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0,
            in_value->field[id_to_field_sel]->data));
exit:
    SHR_FUNC_EXIT();
}

