/*! \file bcmcth_meter_action_internal.c
 *
 * This file defines HW interface handler functions for
 * FP Meter actions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_fmm.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmcth/bcmcth_meter_util.h>
#include <bcmcth/bcmcth_meter_fp_ing_action.h>
#include <bcmcth/bcmcth_meter_fp_ing_imm.h>
#include <bcmcth/bcmcth_meter_fp_egr_action.h>
#include <bcmcth/bcmcth_meter_fp_egr_imm.h>

#include <bcmcth/bcmcth_meter_action_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Private functions
 */

/*
 * Retrieve a field from the input field list.
 */
static int
meter_field_value_get(int unit,
                      const bcmltd_field_t *list,
                      uint32_t fid,
                      uint32_t *data)
{
    bool  found = false;
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    field = list;
    while (field) {
        if (field->id == fid) {
            *data = field->data;
            found = true;
            break;
        }
        field = field->next;
    }

    if (found == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Validate the action entry with PDD profile configuration.
 */
static int
meter_fp_action_validate (int unit,
                          int stage,
                          uint32_t pool_id,
                          bcmltd_fields_t *old_data,
                          const bcmltd_field_t *new_data,
                          uint8_t *pdd_state,
                          bcmcth_meter_action_pdd_offset_t *offset_info)
{
    uint32_t i = 0, fid = 0, pdd_fid = 0, val = 0;
    const bcmltd_field_t *field;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
    }

    if (drv == NULL || drv->action_lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (*pdd_state == METER_ACTION_PDD_WIDTH_EXCEEDS) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (old_data && old_data->count != 0) {
        for (i = 0; i < old_data->count; i++) {
            fid = old_data->field[i]->id;
            val = old_data->field[i]->data;
            if (val != 0) {
                pdd_fid = drv->action_lt_info[pool_id].fid_info[fid].map_fid;
                if ((pdd_fid != 0) && offset_info[pdd_fid].en == false) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }
    }

    field = new_data;
    while (field) {
        fid = field->id;
        val = field->data;
        if (val != 0) {
            pdd_fid = drv->action_lt_info[pool_id].fid_info[fid].map_fid;
            if ((pdd_fid != 0) && offset_info[pdd_fid].en == false) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
        field = field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Write an entry into the H/W table.
 */
static int
meter_action_hw_entry_write(int unit,
                            uint32_t trans_id,
                            int pipe,
                            uint32_t lt_id,
                            uint32_t pt_id,
                            uint32_t hw_idx,
                            uint8_t *entry_buf)
{
    uint32_t            rsp_flags;
    bcmltd_sid_t        rsp_ltid;
    bcmdrd_sym_info_t   sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = hw_idx;
    pt_dyn_info.tbl_inst = pipe;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_info_get(unit, pt_id, &sinfo));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_id,
                                    &pt_dyn_info,
                                    NULL, NULL,
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)entry_buf,
                                    0,
                                    lt_id,
                                    trans_id,
                                    NULL, NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Prepare the H/W entry based on the offset information
 * corresponding to the PDD profile.
 */
static int
meter_action_hw_entry_prepare(int unit,
                              int stage,
                              uint32_t pool_id,
                              uint32_t pt_id,
                              bcmltd_fields_t *old_data,
                              const bcmltd_field_t *new_data,
                              bcmcth_meter_action_pdd_offset_t *offset_info,
                              uint8_t *buf)
{
    uint32_t    bit_pos = 0, tmp_data[4] = {0};
    uint32_t    i = 0, j = 0, fid = 0, data[4] = {0}, pdd_fid = 0;
    uint32_t    s_bit = 0, e_bit = 0, offset = 0;
    uint32_t    num_cont = 0, cont_width = 0;
    const bcmltd_field_t *field;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
    }

    /* Program h/w entry with old actions, UPDATE case. */
    if (old_data && old_data->count != 0) {
        for (i = 0; i < old_data->count; i++) {
            fid = old_data->field[i]->id;
            data[0] = old_data->field[i]->data;

            pdd_fid = drv->action_lt_info[pool_id].fid_info[fid].map_fid;
            if (pdd_fid == 0) {
                continue;
            }

            s_bit = drv->action_lt_info[pool_id].offset_info[fid].s_bit;
            num_cont = drv->pdd_lt_info->cont_info[pdd_fid].num_conts;

            if (!offset_info[pdd_fid].en) {
                continue;
            }
            bit_pos = 0;
            for (j = 0; j < num_cont; j++) {
                cont_width = drv->pdd_lt_info->cont_info[pdd_fid].cont_width[j];
                offset = offset_info[pdd_fid].offset[j];

                s_bit = (s_bit + offset);
                e_bit = (s_bit + cont_width - 1);

                SHR_BITCOPY_RANGE(tmp_data, 0, data, bit_pos, cont_width);

                bcmdrd_field_set((uint32_t *)buf,
                                 s_bit,
                                 e_bit,
                                 &tmp_data[0]);

                bit_pos += cont_width;
            }
        }
    }

    /* Program H/W entry with new actions.*/
    field = new_data;
    while (field) {
        fid = field->id;
        data[0] = field->data;

        pdd_fid = drv->action_lt_info[pool_id].fid_info[fid].map_fid;
        if (pdd_fid == 0) {
            field = field->next;
            continue;
        }

        s_bit = drv->action_lt_info[pool_id].offset_info[fid].s_bit;
        num_cont = drv->pdd_lt_info->cont_info[pdd_fid].num_conts;

        if (!offset_info[pdd_fid].en) {
            field = field->next;
            continue;
        }
        bit_pos = 0;
        for (j = 0; j < num_cont; j++) {
            cont_width = drv->pdd_lt_info->cont_info[pdd_fid].cont_width[j];
            offset = offset_info[pdd_fid].offset[j];

            s_bit = (s_bit + offset);
            e_bit = (s_bit + cont_width - 1);

            SHR_BITCOPY_RANGE(tmp_data, 0, data, bit_pos, cont_width);

            bcmdrd_field_set((uint32_t *)buf,
                             s_bit,
                             e_bit,
                             &tmp_data[0]);

            bit_pos += cont_width;
        }
        field = field->next;
    }

    SHR_FUNC_EXIT();
}

/*
 * Update the existing LT field list with new input.
 * During an LT UPDATE operation, bcmimm_entry_lookup will
 * return the existing list of LT fields. This function
 * updates that list with the new fields being added by user
 * during UPDATE.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  old_data      Existing field list.
 * \param [in]  data          New field list.
 */
static void
meter_action_update_flds_list(int unit,
                              bcmltd_fields_t *old_data,
                              const bcmltd_field_t *data)
{
    int         rv = 0;
    bool        found = false;
    uint32_t    i = 0, fid = 0, val = 0, out_idx = 0;
    const bcmltd_field_t *field;

    if (!old_data) {
        return;
    }

    /*
     * Update the output fields list based on the current input.
     * If any field has changed, update the field with the new
     * value
     */
    for (i = 0; i < old_data->count; i++) {
        fid = old_data->field[i]->id;
        rv = meter_field_value_get(unit, data, fid, &val);
        if (rv != SHR_E_NOT_FOUND) {
            old_data->field[i]->data = val;
        }
    }
    out_idx = old_data->count;

    /*
     * Parse the new list. If any new fields are being added
     * insert those into "old_data" so that "old_data"
     * has all the latest values of all LT fields.
     */
    field = data;
    while (field) {
        fid = field->id;
        val = field->data;
        found = false;
        for (i = 0; i < old_data->count; i++) {
            if (fid == old_data->field[i]->id) {
                found = true;
                break;
            }
        }
        if (found == false) {
            old_data->field[out_idx]->id = fid;
            old_data->field[out_idx]->data = val;
            out_idx++;
        }
        field = field->next;
    }
    old_data->count = out_idx;
}

/*******************************************************************************
 * Public functions
 */
int
bcmcth_meter_action_entry_insert(int unit,
                                 bcmltd_sid_t sid,
                                 int stage,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmltd_fields_t *out)
{
    int         rv = SHR_E_NONE;
    int         pipe = -1;
    uint32_t    num_profiles = 0;
    uint32_t    pipe_val = 0, pipe_fid = 0;
    uint32_t    i = 0, action_id = 0, pool_id = 0;
    uint32_t    key_val = 0, key_fid = 0;
    uint32_t    pt_id = 0, alloc_sz = 0, ret_fid = 0;
    uint32_t    pdd_fid = 0, pdd_id = 0/*, pdd_ltid = 0*/;
    uint32_t    arr_idx = 0, max_pdd_count = 0;
    uint8_t     *entry_buf = NULL;
    uint8_t     *pdd_state = NULL;
    uint8_t     oper_mode = 0;
    bool        insert = false;
    bcmcth_meter_action_drv_t *drv = NULL;
    bcmcth_meter_action_pdd_pt_info_t   *pdd_pt_info = NULL;
    bcmcth_meter_action_pdd_offset_t    *offset_info = NULL;
    bcmcth_meter_action_pdd_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_oper_mode_get(unit, &oper_mode);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_oper_mode_get(unit, &oper_mode);
    }

    if (drv == NULL || drv->action_lt_info == NULL || drv->pdd_lt_info == NULL
        || drv->action_pt_info == NULL || drv->pdd_pt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (i = 0; i < drv->num_action_lts; i++) {
        if (sid == drv->action_lt_info[i].ltid) {
            pool_id = i;
            break;
        }
    }
    if (i == drv->num_action_lts) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    key_fid = drv->action_lt_info[pool_id].key_fid;
    pdd_fid = drv->action_lt_info[pool_id].pdd_fid;
    ret_fid = drv->action_lt_info[pool_id].oper_fid;
    pipe_fid = drv->action_lt_info[pool_id].pipe_fid;

    /* Parse the input to retrieve key values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &key_val));

    action_id = key_val;
    pdd_pt_info = drv->pdd_pt_info;
    pt_id = drv->action_pt_info->pt_id[pool_id];

    num_profiles = pdd_pt_info->num_profiles;
    if ((oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) && (pipe_fid != 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_field_value_get(unit,
                                   key,
                                   pipe_fid,
                                   &pipe_val));
        pipe = pipe_val;
    } else {
        pipe_val = 0;
        pipe = -1;
    }

    /* Retrieve PDD profile ID. */
    rv = meter_field_value_get(unit, data, pdd_fid, &pdd_id);
    if (rv == SHR_E_NOT_FOUND) {
        pdd_id = 0;
    }

    max_pdd_count = pdd_pt_info->pdd_bmp_size;
    arr_idx = ((pipe_val * num_profiles * max_pdd_count) +
               (pdd_id * max_pdd_count));
    /* Fetch PDD offset information for this PDD profile ID. */
    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_pdd_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_egr_pdd_state_get(unit, &state);
    }
    offset_info = &state->offset_info[arr_idx];
    pdd_state = &state->state[(pipe_val * num_profiles) + pdd_id];

    /*
     * Validate the specified actions based on PDD information.
     */
    rv = meter_fp_action_validate(unit,
                                  stage,
                                  pool_id,
                                  NULL,
                                  data,
                                  pdd_state,
                                  offset_info);
    if (rv == SHR_E_NONE) {
        insert = true;
        if (out) {
            out->count = 1;
            out->field[0]->id = ret_fid;
            out->field[0]->data = METER_ACTION_ENTRY_STATE_VALID;
        }
    } else {
        insert = false;
        if (out) {
            out->count = 1;
            out->field[0]->id = ret_fid;
            if (rv == SHR_E_RESOURCE) {
                out->field[0]->data = METER_ACTION_PDD_WIDTH_EXCEEDS;
            } else {
                out->field[0]->data = METER_ACTION_ENTRY_STATE_INVALID;
            }
        }
    }

    if (insert == false) {
        SHR_EXIT();
    }
    /* Allocate buffer to read/write to HW action table. */
    alloc_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, alloc_sz, "bcmcthMeterActionEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, alloc_sz);

    /*
     * Prepare the H/W entry based on the associated PDD
     * profile.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_action_hw_entry_prepare(unit,
                                      stage,
                                      pool_id,
                                      pt_id,
                                      NULL,
                                      data,
                                      offset_info,
                                      entry_buf));

    /*
     * Write back the entry into H/W table.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_action_hw_entry_write(unit,
                                    trans_id,
                                    pipe,
                                    sid,
                                    pt_id,
                                    action_id,
                                    entry_buf));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_action_entry_update(int unit,
                                 bcmltd_sid_t sid,
                                 int stage,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmltd_fields_t *out)
{
    int         rv = SHR_E_NONE;
    int         pipe = -1;
    bool        per_pipe = false;
    uint32_t    pipe_val = 0, pipe_fid = 0;
    uint32_t    num_profiles = 0;
    uint32_t    i = 0, action_id = 0, pool_id = 0;
    uint32_t    key_fid = 0, key_val = 0;
    uint32_t    pt_id = 0, alloc_sz = 0, ret_fid = 0;
    uint32_t    pdd_fid = 0, pdd_id = 0;
    uint32_t    arr_idx = 0, max_pdd_count = 0;
    uint8_t     *entry_buf = NULL;
    uint8_t     *pdd_state = NULL;
    uint8_t     oper_mode = 0;
    bool        update = false;
    bcmltd_fields_t  in_flds = {0}, out_flds = {0};
    shr_famm_hdl_t   hdl = NULL;
    bcmcth_meter_action_drv_t *drv = NULL;
    bcmcth_meter_action_pdd_pt_info_t   *pdd_pt_info = NULL;
    bcmcth_meter_action_pdd_offset_t    *offset_info = NULL;
    bcmcth_meter_action_pdd_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_oper_mode_get(unit, &oper_mode);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_oper_mode_get(unit, &oper_mode);
    }

    if (drv == NULL || drv->action_lt_info == NULL || drv->pdd_lt_info == NULL
        || drv->action_pt_info == NULL || drv->pdd_pt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (i = 0; i < drv->num_action_lts; i++) {
        if (sid == drv->action_lt_info[i].ltid) {
            pool_id = i;
            break;
        }
    }
    if (i == drv->num_action_lts) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    key_fid = drv->action_lt_info[pool_id].key_fid;
    pdd_fid = drv->action_lt_info[pool_id].pdd_fid;
    ret_fid = drv->action_lt_info[pool_id].oper_fid;
    pipe_fid = drv->action_lt_info[pool_id].pipe_fid;

    /* Parse the input to retrieve key values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &key_val));

    action_id = key_val;
    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_pdd_fld_arr_hdl_get(unit, &hdl);
    } else {
        bcmcth_meter_fp_egr_pdd_fld_arr_hdl_get(unit, &hdl);
    }

    pdd_pt_info = drv->pdd_pt_info;
    pt_id = drv->action_pt_info->pt_id[pool_id];

    num_profiles = pdd_pt_info->num_profiles;
    if ((oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) && (pipe_fid != 0)) {
        per_pipe = true;
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_field_value_get(unit,
                                   key,
                                   pipe_fid,
                                   &pipe_val));
        pipe = pipe_val;
    } else {
        pipe_val = 0;
        pipe = -1;
    }

    /*
     * Do an IMM lookup to retrieve the
     * previous fields in this LT entry.
     */
    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(hdl, 2);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = 64;
    out_flds.field = shr_famm_alloc(hdl, 64);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    in_flds.field[0]->id = key_fid;
    in_flds.field[0]->data = key_val;
    if (per_pipe == true) {
        in_flds.count = 2;
        in_flds.field[1]->id = pipe_fid;
        in_flds.field[1]->data = pipe_val;
    }

    if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) == SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    meter_action_update_flds_list(unit, &out_flds, data);

    /* Retrieve PDD profile ID. */
    for (i = 0; i < out_flds.count; i++) {
        if (out_flds.field[i]->id == pdd_fid) {
            pdd_id = out_flds.field[i]->data;
            break;
        }
    }

    /* PDD profile ID field not found, default = 0. */
    if (i == out_flds.count) {
        pdd_id = 0;
    }

    max_pdd_count = pdd_pt_info->pdd_bmp_size;
    arr_idx = ((pipe_val * num_profiles * max_pdd_count) +
               (pdd_id * max_pdd_count));
    /* Fetch PDD offset information for this PDD profile ID. */
    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_pdd_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_egr_pdd_state_get(unit, &state);
    }
    offset_info = &state->offset_info[arr_idx];
    pdd_state = &state->state[(num_profiles * pipe_val) + pdd_id];

    /*
     * Validate the specified actions based on PDD information.
     */
    rv = meter_fp_action_validate(unit,
                                  stage,
                                  pool_id,
                                  &out_flds,
                                  NULL,
                                  pdd_state,
                                  offset_info);
    if (rv == SHR_E_NONE) {
        update = true;
        if (out) {
            out->count = 1;
            out->field[0]->id = ret_fid;
            out->field[0]->data = METER_ACTION_ENTRY_STATE_VALID;
        }
    } else {
        update = false;
        if (out) {
            out->count = 1;
            out->field[0]->id = ret_fid;
            if (rv == SHR_E_RESOURCE) {
                out->field[0]->data = METER_ACTION_PDD_WIDTH_EXCEEDS;
            } else {
                out->field[0]->data = METER_ACTION_ENTRY_STATE_INVALID;
            }
        }
    }

    /* Allocate buffer to read/write to HW action table. */
    alloc_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, alloc_sz, "bcmcthMeterActionEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, alloc_sz);

    /*
     * Prepare the H/W entry based on the associated PDD
     * profile.
     */
    if (update == true) {
        SHR_IF_ERR_VERBOSE_EXIT(
            meter_action_hw_entry_prepare(unit,
                                          stage,
                                          pool_id,
                                          pt_id,
                                          &out_flds,
                                          NULL,
                                          offset_info,
                                          entry_buf));

    }

    /*
     * Write back the entry into H/W table.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_action_hw_entry_write(unit,
                                    trans_id,
                                    pipe,
                                    sid,
                                    pt_id,
                                    action_id,
                                    entry_buf));

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 2);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, 64);
    }
    SHR_FREE(entry_buf);

    SHR_FUNC_EXIT();
}

int
bcmcth_meter_action_entry_delete(int unit,
                                 bcmltd_sid_t sid,
                                 int stage,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data)
{
    int      pipe = -1;
    uint32_t pipe_val = 0, pipe_fid = 0;
    uint32_t i = 0;
    uint8_t *entry_buf = NULL;
    uint8_t  oper_mode = 0;
    uint32_t pt_id = 0, alloc_sz = 0, action_id = 0;
    uint32_t pool_id = 0, key_fid = 0, key_val = 0;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_oper_mode_get(unit, &oper_mode);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_oper_mode_get(unit, &oper_mode);
    }
    if (drv == NULL || drv->action_lt_info == NULL || drv->pdd_lt_info == NULL
        || drv->action_pt_info == NULL || drv->pdd_pt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (i = 0; i < drv->num_action_lts; i++) {
        if (sid == drv->action_lt_info[i].ltid) {
            pool_id = i;
            break;
        }
    }
    if (i == drv->num_action_lts) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    key_fid = drv->action_lt_info[pool_id].key_fid;
    pipe_fid = drv->action_lt_info[pool_id].pipe_fid;

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &key_val));

    action_id = key_val;

    if ((oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) &&
        (pipe_fid != 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_field_value_get(unit,
                                   key,
                                   pipe_fid,
                                   &pipe_val));
        pipe = pipe_val;
    } else {
        pipe_val = 0;
        pipe = -1;
    }

    pt_id = drv->action_pt_info->pt_id[pool_id];

    alloc_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, alloc_sz, "bcmcthMeterActionEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, alloc_sz);

    /*
     * Clear the HW entry by writing all 0s.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_action_hw_entry_write(unit,
                                    trans_id,
                                    pipe,
                                    sid,
                                    pt_id,
                                    action_id,
                                    entry_buf));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Meter action entries update
 *
 * Update all action entries using this PDD profile
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  pdd_id        PDD profile ID.
 * \param [in]  offset_info   Offset information of this PDD profile.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_fp_update_action_entries(int unit,
                                      int stage,
                                      uint32_t trans_id,
                                      int pipe,
                                      uint32_t pdd_id,
                                      uint8_t *pdd_state,
                                      bcmcth_meter_action_pdd_offset_t *offset_info)
{
    uint32_t    pipe_fid = 0;
    uint32_t    i = 0, j = 0, k = 0, sid = 0;
    uint32_t    key_fid = 0;
    uint32_t    min = 0, max = 0, pt_id = 0;
    uint32_t    fid = 0, ret_fid = 0, data = 0, pdd_action = 0;
    uint32_t    alloc_sz = 0, pdd_fid = 0;
    uint8_t     *entry_buf = NULL;
    uint8_t     oper_mode = 0, key_count = 0;
    bool        valid = true, match = true, found = false;
    bool        insert = false;
    bcmlrd_field_def_t  def;
    shr_famm_hdl_t      hdl = NULL;
    bcmltd_fields_t     in_flds = {0}, out_flds = {0};
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_pdd_fld_arr_hdl_get(unit, &hdl);
        bcmcth_meter_fp_ing_oper_mode_get(unit, &oper_mode);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_pdd_fld_arr_hdl_get(unit, &hdl);
        bcmcth_meter_fp_egr_oper_mode_get(unit, &oper_mode);
    }
    if (drv == NULL || drv->action_lt_info == NULL || drv->pdd_lt_info == NULL
        || drv->action_pt_info == NULL || drv->pdd_pt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pt_id = drv->action_pt_info->pt_id[0];

    /*
     * Do an IMM lookup to retrieve the action
     * entries using this PDD profile.
     */
    in_flds.count = 3;
    in_flds.field = shr_famm_alloc(hdl, 3);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = 64;
    out_flds.field = shr_famm_alloc(hdl, 64);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) {
        key_count = 2;
    }

    /* Allocate buffer to read/write to HW action table. */
    alloc_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, alloc_sz, "bcmcthMeterActionEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, alloc_sz);

    for (j = 0; j < drv->num_action_lts; j++) {
        sid = drv->action_lt_info[j].ltid;
        key_fid = drv->action_lt_info[j].key_fid;
        pipe_fid = drv->action_lt_info[j].pipe_fid;
        pdd_fid = drv->action_lt_info[j].pdd_fid;
        ret_fid = drv->action_lt_info[j].oper_fid;

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, sid, key_fid, &def));
        min = METER_FP_FIELD_MIN(def);
        max = METER_FP_FIELD_MAX(def);

        for (i = min; i <= max; i++) {
            in_flds.count = 1;
            in_flds.field[0]->id = key_fid;
            in_flds.field[0]->data = i;
            if ((key_count == 2) && (pipe_fid != 0)) {
                in_flds.field[1]->id = pipe_fid;
                in_flds.field[1]->data = pipe;
                in_flds.count = 2;
            }

            if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) == SHR_E_NOT_FOUND) {
                continue;
            }
            valid = true;
            match = true;
            found = false;
            insert = false;
            /* Iterate over action entries using this PDD profile ID. */
            for (k = 0; k < out_flds.count; k++) {
                fid = out_flds.field[k]->id;
                data = out_flds.field[k]->data;

                if (fid == pdd_fid) {
                    found = true;
                    /* PDD profile ID is different. */
                    if (data != pdd_id) {
                        match = false;
                        break;
                    }
                }
                if (data != 0) {
                    /* Check if this action is enabled in the PDD. */
                    pdd_action = drv->action_lt_info[j].fid_info[fid].map_fid;
                    if ((pdd_action != 0) && offset_info[pdd_action].en == false) {
                        valid = false;
                    }
                }
            }

            /*
             * If the action entry has a different PDD profile ID,
             * continue onto the next entry.
             */
            if (match == false) {
                continue;
            } else {
                /*
                 * PDD profile ID is not specified in the action entry, therefore
                 * PDD profile ID takes default value "0".
                 * If the current PDD profile being inserted/update is not "0",
                 * continue.
                 */
                if (found == false) {
                    if (pdd_id != 0) {
                        continue;
                    }
                }
            }

            if (*pdd_state != 0) {
                insert = false;
                in_flds.field[in_flds.count]->data = METER_ACTION_PDD_WIDTH_EXCEEDS;
            } else if (valid == false) {
                insert = false;
                in_flds.field[in_flds.count]->data = METER_ACTION_ENTRY_STATE_INVALID;
            } else {
                insert = true;
                in_flds.field[in_flds.count]->data = METER_ACTION_ENTRY_STATE_VALID;
            }
            in_flds.field[in_flds.count]->id = ret_fid;
            in_flds.count++;

            /* Update the status of the action entry. */
            SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_update(unit, 0, sid, &in_flds));
            sal_memset(entry_buf, 0, alloc_sz);

            pt_id = drv->action_pt_info->pt_id[j];
            if (insert == true) {
                SHR_IF_ERR_VERBOSE_EXIT(
                    meter_action_hw_entry_prepare(unit,
                                                  stage,
                                                  j,
                                                  pt_id,
                                                  &out_flds,
                                                  NULL,
                                                  offset_info,
                                                  entry_buf));
            }

            SHR_IF_ERR_VERBOSE_EXIT(
                meter_action_hw_entry_write(unit,
                                            trans_id,
                                            pipe,
                                            sid,
                                            pt_id,
                                            i,
                                            entry_buf));
        }
    }

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 3);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, 64);
    }
    SHR_FREE(entry_buf);

    SHR_FUNC_EXIT();
}
