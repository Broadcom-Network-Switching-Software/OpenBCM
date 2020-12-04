/*! \file bcmcth_meter_pdd_internal.c
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
#include <shr/shr_error.h>
#include <shr/shr_bitop.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
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

#include <bcmcth/bcmcth_meter_pdd_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Private functions
 */
/*
 * Read an entry from the H/W table.
 */
static int
meter_pdd_hw_entry_read(int unit,
                        int stage,
                        uint32_t trans_id,
                        int pipe,
                        uint32_t pt_id,
                        uint32_t hw_idx,
                        uint32_t *cont_bmp,
                        uint8_t *entry_buf)
{
    uint32_t            rsp_flags, pt_fid = 0;
    uint32_t            lt_id = 0;
    bcmltd_sid_t        rsp_ltid;
    bcmdrd_sym_info_t   sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = hw_idx;
    pt_dyn_info.tbl_inst = pipe;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_info_get(unit, pt_id, &sinfo));

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
    }
    if (drv == NULL || drv->pdd_lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    lt_id = drv->pdd_lt_info->ltid;
    pt_fid = drv->pdd_pt_info->pt_fid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_id,
                                    &pt_dyn_info,
                                    NULL, NULL,
                                    BCMPTM_OP_READ,
                                    NULL,
                                    sinfo.entry_wsize,
                                    lt_id,
                                    trans_id,
                                    NULL, NULL,
                                    (uint32_t *)entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    bcmdrd_pt_field_get(unit,
                        pt_id,
                        (uint32_t *)entry_buf,
                        pt_fid,
                        cont_bmp);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Write an entry into the H/W table.
 */
static int
meter_pdd_hw_entry_write(int unit,
                         int stage,
                         uint32_t trans_id,
                         int pipe,
                         uint32_t pt_id,
                         uint32_t hw_idx,
                         uint8_t *entry_buf)
{
    uint32_t            rsp_flags, lt_id = 0;
    bcmltd_sid_t        rsp_ltid;
    bcmdrd_sym_info_t   sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = hw_idx;
    pt_dyn_info.tbl_inst = pipe;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_info_get(unit, pt_id, &sinfo));

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
    }
    if (drv == NULL || drv->pdd_lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    lt_id = drv->pdd_lt_info->ltid;

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
 * Convert a PDD LT entry into corresponding HW entry.
 */
static int
meter_pdd_hw_entry_prepare(int unit,
                           int stage,
                           uint32_t pt_id,
                           bcmltd_fields_t *old_data,
                           const bcmltd_field_t *in_data,
                           uint32_t *cont_bmp,
                           uint8_t *buf)
{
    uint32_t    fid = 0, data = 0, i = 0, j = 0;
    uint32_t    cont_id = 0, num_cont = 0, map_fid = 0;
    const bcmltd_field_t *field;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
    }

    if (old_data && old_data->count != 0) {
        for (j = 0; j < old_data->count; j++) {
            fid = old_data->field[j]->id;
            data = old_data->field[j]->data;

            map_fid = drv->pdd_lt_info->fid_info[fid].map_fid;
            num_cont = drv->pdd_lt_info->cont_info[map_fid].num_conts;

            for (i = 0; i < num_cont; i++) {
                cont_id = drv->pdd_lt_info->cont_info[map_fid].cont_ids[i];
                if (data != 0) {
                    SHR_BITSET(cont_bmp, cont_id);
                } else {
                    SHR_BITCLR(cont_bmp, cont_id);
                }
                bcmdrd_field_set((uint32_t *)buf,
                                 cont_id,
                                 cont_id,
                                 &data);
            }
        }
    }

    field = in_data;
    while (field) {
        fid = field->id;
        data = field->data;

        map_fid = drv->pdd_lt_info->fid_info[fid].map_fid;
        num_cont = drv->pdd_lt_info->cont_info[map_fid].num_conts;

        for (i = 0; i < num_cont; i++) {
            cont_id = drv->pdd_lt_info->cont_info[map_fid].cont_ids[i];
            if (data != 0) {
                SHR_BITSET(cont_bmp, cont_id);
            } else {
                SHR_BITCLR(cont_bmp, cont_id);
            }
            bcmdrd_field_set((uint32_t *)buf,
                             cont_id,
                             cont_id,
                             &data);
        }

        field = field->next;
    }

    SHR_FUNC_EXIT();
}

/*
 * Clear offsets of each of each action in this PDD profile.
 */
static int
meter_fp_action_offset_clear(int unit,
                             bcmcth_meter_action_pdd_lt_info_t *pdd_lt_info,
                             bcmcth_meter_action_pdd_offset_t *offset_info)
{
    uint32_t i = 0, j = 0, pdd_count = 0, num_cont = 0;

    SHR_FUNC_ENTER(unit);

    pdd_count = pdd_lt_info->action_count;
    /* Clear all offset information. */
    for (i = 0; i < pdd_count; i++) {
        num_cont = pdd_lt_info->cont_info[i].num_conts;
        j = 0;
        while (j < num_cont) {
            offset_info[i].in_use[j] = false;
            offset_info[i].offset[j] = 0;
            j++;
        }
        offset_info[i].en = false;
    }

    SHR_FUNC_EXIT();
}

/*
 * Derive offsets for each action in this PDD profile.
 */
static int
meter_fp_action_offset_construct(int unit,
                                 uint32_t *cont_bmp,
                                 uint32_t pdd_bmp_size,
                                 uint32_t pt_width,
                                 bcmcth_meter_action_pdd_lt_info_t *pdd_lt_info,
                                 bcmcth_meter_action_pdd_offset_t *offset_info)
{
    bool        found = false;
    uint32_t    i = 0, j = 0, cont_id = 0, pdd_count = 0;
    uint32_t    width = 0, num_cont = 0, w_bit = 0;

    SHR_FUNC_ENTER(unit);

    pdd_count = pdd_lt_info->action_count;

    /* Parse all PDD actions and find the ones that are enabled. */
    SHR_BIT_ITER(cont_bmp, pdd_bmp_size, cont_id) {
        /* Find an action that uses this container. */
        for (i = 0; i < pdd_count; i++) {
            num_cont = pdd_lt_info->cont_info[i].num_conts;
            j = 0;
            while (j < num_cont) {
                if (pdd_lt_info->cont_info[i].cont_ids[j] == cont_id) {
                    width = pdd_lt_info->cont_info[i].cont_width[j];

                    offset_info[i].offset[j] = w_bit;
                    w_bit = (w_bit + width);
                    if (w_bit > pt_width) {
                        SHR_ERR_EXIT(SHR_E_RESOURCE);
                    }
                    offset_info[i].in_use[j] = true;
                    found = true;
                    break;
                }
                j++;
            }
            if (found == true) {
                offset_info[i].en = true;
                found = false;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Retrieve value corresponding to a field from
 * the input field list.
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

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief FP meter PDD insert
 *
 * Insert an FP Meter PDD in the hardware PDD table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  reason        IMM event reason.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_pdd_entry_insert(int unit,
                              bcmltd_sid_t sid,
                              int stage,
                              uint32_t trans_id,
                              bcmimm_entry_event_t reason,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data)
{
    int         rv = 0;
    int         pipe = -1;
    uint32_t    num_profiles = 0;
    uint32_t    alloc_sz = 0, arr_idx = 0;
    uint32_t    key_fid = 0, pdd_id = 0, pdd_bmp_size = 0;
    uint32_t    pipe_val = 0, pipe_fid = 0;
    uint32_t    pt_id = 0, pt_width = 0;
    uint8_t     *entry_buf = NULL;
    uint8_t     *pdd_state = NULL;
    uint8_t     oper_mode = 0;
    SHR_BITDCL  *cont_bmp = NULL;
    bcmltd_fields_t  in_flds = {0}, out_flds = {0};
    shr_famm_hdl_t   hdl = NULL;
    bcmcth_meter_action_drv_t *drv = NULL;
    bcmcth_meter_action_pdd_lt_info_t *pdd_lt_info = NULL;
    bcmcth_meter_action_pdd_pt_info_t *pdd_pt_info = NULL;
    bcmcth_meter_action_pdd_offset_t *offset_info = NULL;
    bcmcth_meter_action_pt_info_t *action_pt_info = NULL;
    bcmcth_meter_action_pdd_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_oper_mode_get(unit, &oper_mode);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_oper_mode_get(unit, &oper_mode);
    }
    if (drv == NULL || drv->pdd_lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pdd_lt_info = drv->pdd_lt_info;
    pdd_pt_info = drv->pdd_pt_info;
    action_pt_info = drv->action_pt_info;

    key_fid = pdd_lt_info->key_fid;
    pipe_fid = pdd_lt_info->pipe_fid;
    pt_width = action_pt_info->pt_width;
    num_profiles = pdd_pt_info->num_profiles;

    /* Parse the input to retrieve key. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &pdd_id));

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

    pt_id = pdd_pt_info->pt_id;
    pdd_bmp_size = pdd_pt_info->pdd_bmp_size;
    arr_idx = ((pipe_val * num_profiles * pdd_bmp_size) +
               (pdd_id * pdd_bmp_size));

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_pdd_state_get(unit, &state);
        bcmcth_meter_fp_ing_pdd_fld_arr_hdl_get(unit, &hdl);
    } else {
        bcmcth_meter_fp_egr_pdd_state_get(unit, &state);
        bcmcth_meter_fp_egr_pdd_fld_arr_hdl_get(unit, &hdl);
    }
    offset_info = &state->offset_info[arr_idx];
    pdd_state = &state->state[(num_profiles * pipe_val) + pdd_id];

    alloc_sz = SHR_BITALLOCSIZE(pdd_bmp_size);
    SHR_ALLOC(cont_bmp, alloc_sz, "bcmcthMeterActionPddBmp");
    SHR_NULL_CHECK(cont_bmp, SHR_E_MEMORY);
    sal_memset(cont_bmp, 0, alloc_sz);

    /* Allocate buffer to read/write to HW action table. */
    alloc_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, alloc_sz, "bcmcthMeterActionPddEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, alloc_sz);

    if (reason == BCMIMM_ENTRY_UPDATE) {
        /*
         * Do an IMM lookup to retrieve the
         * previous fields in this LT entry.
         */
        in_flds.count = 1;
        in_flds.field = shr_famm_alloc(hdl, 2);

        if (in_flds.field == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        out_flds.count = 32;
        out_flds.field = shr_famm_alloc(hdl, 32);

        if (out_flds.field == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        in_flds.field[0]->id = key_fid;
        in_flds.field[0]->data = pdd_id;

        if ((oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) &&
            (pipe_fid != 0)) {
            in_flds.count = 2;
            in_flds.field[1]->id = pipe_fid;
            in_flds.field[1]->data = pipe_val;
        }

        if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) == SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    /* Read PDD profile entry and retrieve container bitmap. */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_pdd_hw_entry_read(unit,
                                stage,
                                trans_id,
                                pipe,
                                pt_id,
                                pdd_id,
                                cont_bmp,
                                entry_buf));

    /*
     * Prepare HW entry corresponding to the LT entry configuration.
     * Also update the container bitmap as per the actions enabled.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_pdd_hw_entry_prepare(unit,
                                   stage,
                                   pt_id,
                                   &out_flds,
                                   data,
                                   cont_bmp,
                                   entry_buf));

    /*
     * Clear offset information for this PDD profile.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_fp_action_offset_clear(unit,
                                     pdd_lt_info,
                                     offset_info));

    /*
     * Derive offset for each action in this PDD profile.
     */
    rv = meter_fp_action_offset_construct(unit,
                                          cont_bmp,
                                          pdd_bmp_size,
                                          pt_width,
                                          pdd_lt_info,
                                          offset_info);
    if (rv == SHR_E_RESOURCE) {
        *pdd_state = METER_ACTION_PDD_WIDTH_EXCEEDS;
        sal_memset(entry_buf, 0, alloc_sz);
    } else {
        *pdd_state = 0;
    }

    /* Update the Meter action entries that reference this PDD ID. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_fp_update_action_entries(unit,
                                              stage,
                                              trans_id,
                                              pipe,
                                              pdd_id,
                                              pdd_state,
                                              offset_info));
    /*
     * Write back the entry into HW.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_pdd_hw_entry_write(unit,
                                 stage,
                                 trans_id,
                                 pipe,
                                 pt_id,
                                 pdd_id,
                                 entry_buf));

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 2);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, 32);
    }
    SHR_FREE(cont_bmp);
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP meter PDD delete
 *
 * Delete an FP Meter PDD from the hardware PDD table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_pdd_entry_delete(int unit,
                              int stage,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data)
{
    uint32_t    num_profiles = 0;
    uint32_t    alloc_sz = 0, pdd_bmp_size = 0;
    uint32_t    key_fid = 0, pdd_id = 0;
    uint32_t    pt_id = 0, arr_idx = 0;
    uint32_t    pipe_val = 0, pipe_fid = 0;
    int         pipe = -1;
    uint8_t     *entry_buf = NULL;
    uint8_t     *pdd_state = NULL;
    uint8_t     oper_mode = 0;
    bcmcth_meter_action_drv_t *drv = NULL;
    bcmcth_meter_action_pdd_lt_info_t   *pdd_lt_info = NULL;
    bcmcth_meter_action_pdd_pt_info_t   *pdd_pt_info = NULL;
    bcmcth_meter_action_pdd_offset_t *offset_info = NULL;
    bcmcth_meter_action_pdd_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_oper_mode_get(unit, &oper_mode);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_oper_mode_get(unit, &oper_mode);
    }
    if (drv == NULL || drv->pdd_lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pdd_lt_info = drv->pdd_lt_info;
    pdd_pt_info = drv->pdd_pt_info;

    num_profiles = pdd_pt_info->num_profiles;
    key_fid = pdd_lt_info->key_fid;
    pipe_fid = pdd_lt_info->pipe_fid;

    /* Parse the input to retrieve key values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &pdd_id));

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

    pt_id = pdd_pt_info->pt_id;
    pdd_bmp_size = pdd_pt_info->pdd_bmp_size;
    arr_idx = ((pipe_val * num_profiles * pdd_bmp_size) +
               (pdd_id * pdd_bmp_size));

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_pdd_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_egr_pdd_state_get(unit, &state);
    }
    offset_info = &state->offset_info[arr_idx];
    pdd_state = &state->state[(num_profiles * pipe_val) + pdd_id];

    /* Allocate buffer to read/write to HW action table. */
    alloc_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, alloc_sz, "bcmcthMeterActionPddEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, alloc_sz);

    /* Delete PDD entry by writing 0s. */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_pdd_hw_entry_write(unit,
                                 stage,
                                 trans_id,
                                 pipe,
                                 pt_id,
                                 pdd_id,
                                 entry_buf));

    /* Clear offset info corresponding to this PDD profile. */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_fp_action_offset_clear(unit,
                                     pdd_lt_info,
                                     offset_info));

    *pdd_state = 0;

    /* Update the Meter action entries that reference this PDD ID. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_fp_update_action_entries(unit,
                                              stage,
                                              trans_id,
                                              pipe,
                                              pdd_id,
                                              pdd_state,
                                              offset_info));
exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}
