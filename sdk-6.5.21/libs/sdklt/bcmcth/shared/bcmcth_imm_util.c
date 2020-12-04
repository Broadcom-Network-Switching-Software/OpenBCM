/*! \file bcmcth_imm_util.c
 *
 * Convenience functions for doing IMM callback read/write via PTM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>

#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmcth/bcmcth_imm_util.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_UTIL

/*******************************************************************************
 * Public functions
 */

int
bcmcth_imm_cb_read(int unit, bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                   int tbl_inst, int tbl_idx, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.tbl_inst = tbl_inst;
    pt_info.index = tbl_idx;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_read(unit, 0, pt_id, &pt_info,
                              NULL, rsp_entry_wsize, lt_id,
                              entry_data, &rsp_ltid, &rsp_flags));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_imm_cb_write(int unit, bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                    int tbl_inst, int tbl_idx, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.tbl_inst = tbl_inst;
    pt_info.index = tbl_idx;
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_write(unit, 0, pt_id, &pt_info, NULL,
                               entry_data, lt_id, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_imm_pipe_ireq_read(
    int unit,
    bcmltd_sid_t lt_id,
    bcmdrd_sid_t pt_id,
    int pipe,
    int index,
    void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    uint64_t            req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index    = index;
    pt_info.tbl_inst = pipe;
    rsp_entry_wsize  = bcmdrd_pt_entry_wsize(unit, pt_id);
    req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(BCMLT_ENT_ATTR_GET_FROM_HW);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_read(unit,
                              req_flags,
                              pt_id,
                              &pt_info,
                              NULL,
                              rsp_entry_wsize,
                              lt_id,
                              entry_data,
                              &rsp_ltid,
                              &rsp_flags));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_imm_pipe_ireq_write(
    int unit,
    bcmltd_sid_t lt_id,
    bcmdrd_sid_t pt_id,
    int pipe,
    int index,
    void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index    = index;
    pt_info.tbl_inst = pipe;
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_write(unit,
                               BCMPTM_REQ_FLAGS_NO_FLAGS,
                               pt_id,
                               &pt_info,
                               NULL,
                               entry_data,
                               lt_id,
                               &rsp_ltid,
                               &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_imm_fields_free(int unit, bcmltd_fields_t *in)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    if (in->field) {
        for (i = 0; i < in->count; i++) {
            if (in->field[i]) {
                shr_fmm_free(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_imm_fields_alloc(int unit, bcmltd_fields_t *in, size_t num_fields)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    /* Allocate fields buffers */
    SHR_ALLOC(in->field,
              sizeof(bcmltd_field_t *) * num_fields,
              "bcmcthMonEflexFields");

    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);

    sal_memset(in->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        in->field[i] = shr_fmm_alloc();
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

    SHR_FUNC_EXIT();

exit:
    bcmcth_imm_fields_free(unit, in);

    SHR_FUNC_EXIT();
}

/*
 * Update value of a field from IMM fields list.
 */
int
bcmcth_imm_field_update(int unit,
                        bcmltd_fields_t *list,
                        uint32_t fid,
                        uint64_t data)
{
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);
    for (i = 0; i < list->count; i++) {
        if (list->field[i]->id == fid) {
            list->field[i]->data = data;
            break;
        }
    }

    if (i == list->count) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Get value of a field from IMM fields list.
 */
int
bcmcth_imm_fields_get(int unit,
                      const bcmltd_fields_t *list,
                      uint32_t fid,
                      uint64_t *data)
{
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);
    for (i = 0; i < list->count; i++) {
        if (list->field[i]->id == fid) {
            *data = list->field[i]->data;
            break;
        }
    }

    if (i == list->count) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}
