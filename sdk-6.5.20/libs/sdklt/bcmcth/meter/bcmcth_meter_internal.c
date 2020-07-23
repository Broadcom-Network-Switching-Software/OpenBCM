/*! \file bcmcth_meter_internal.c
 *
 * This file defines HW interface handler functions for
 * FP Meters.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_meter_fp_drv.h>
#include <bcmcth/bcmcth_meter_fp_ing.h>
#include <bcmcth/bcmcth_meter_fp_egr.h>
#include <bcmcth/bcmcth_meter_fp_ing_imm.h>
#include <bcmcth/bcmcth_meter_fp_egr_imm.h>

#include <bcmcth/bcmcth_meter_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Private functions
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

static void
bcmcth_meter_lt_fid_to_entry_data(uint32_t fid,
                                  uint64_t data,
                                  bcmcth_meter_fp_fid_info_t *fid_info,
                                  bcmcth_meter_sw_params_t *entry)
{
    if (fid_info == NULL) {
        return;
    }

    if (fid == fid_info->pipe_fid) {
        entry->pipe = data;
    } else if (fid == fid_info->pool_fid) {
        entry->meter_pool_id = data;
    } else if (fid == fid_info->pkt_mode_fid) {
        entry->byte_mode = data;
    } else if (fid == fid_info->meter_mode_fid) {
        entry->meter_mode = data;
    } else if (fid == fid_info->color_mode_fid) {
        entry->color_mode = data;
    } else if (fid == fid_info->min_rate_kbps_fid) {
        entry->min_rate_kbps = data;
    } else if (fid == fid_info->min_burst_kbits_fid) {
        entry->min_burst_kbits = data;
    } else if (fid == fid_info->max_rate_kbps_fid) {
        entry->max_rate_kbps = data;
    } else if (fid == fid_info->max_burst_kbits_fid) {
        entry->max_burst_kbits = data;
    } else if (fid == fid_info->min_rate_pps_fid) {
        entry->min_rate_pps = data;
    } else if (fid == fid_info->min_burst_pkts_fid) {
        entry->min_burst_pkts = data;
    } else if (fid == fid_info->max_rate_pps_fid) {
        entry->max_rate_pps = data;
    } else if (fid == fid_info->max_burst_pkts_fid) {
        entry->max_burst_pkts = data;
    } else {
        return;
    }
}

static void
bcmcth_meter_fp_oper_field_update(int unit,
                                  uint32_t idx,
                                  bcmcth_meter_fp_fid_info_t *fid_info,
                                  bcmcth_meter_sw_params_t *lt_entry,
                                  bcmltd_fields_t *out)
{
    uint32_t fid = 0;

    fid = out->field[idx]->id;

    if (fid == fid_info->meter_action_set_fid) {
        out->field[idx]->data = lt_entry->meter_format;
    } else {
        if (lt_entry->byte_mode == METER_FP_MODE_BYTES) {
            if ((fid == fid_info->min_rate_pps_oper_fid) ||
                (fid == fid_info->max_rate_pps_oper_fid) ||
                (fid == fid_info->min_burst_pkts_oper_fid) ||
                (fid == fid_info->max_burst_pkts_oper_fid)) {
                out->field[idx]->data = 0;
            } else if (fid == fid_info->min_rate_kbps_oper_fid) {
                out->field[idx]->data = lt_entry->min_rate_oper;
            } else if (fid == fid_info->max_rate_kbps_oper_fid) {
                out->field[idx]->data = lt_entry->max_rate_oper;
            } else if (fid == fid_info->min_burst_kbits_oper_fid) {
                out->field[idx]->data = lt_entry->min_burst_oper;
            } else if (fid == fid_info->max_burst_kbits_oper_fid) {
                out->field[idx]->data = lt_entry->max_burst_oper;
            } else {
                return;
            }
        } else {
            if ((fid == fid_info->min_rate_kbps_oper_fid) ||
                (fid == fid_info->max_rate_kbps_oper_fid) ||
                (fid == fid_info->min_burst_kbits_oper_fid) ||
                (fid == fid_info->max_burst_kbits_oper_fid)) {
                out->field[idx]->data = 0;
            } else if (fid == fid_info->min_rate_pps_oper_fid) {
                out->field[idx]->data = lt_entry->min_rate_oper;
            } else if (fid == fid_info->max_rate_pps_oper_fid) {
                out->field[idx]->data = lt_entry->max_rate_oper;
            } else if (fid == fid_info->min_burst_pkts_oper_fid) {
                out->field[idx]->data = lt_entry->min_burst_oper;
            } else if (fid == fid_info->max_burst_pkts_oper_fid) {
                out->field[idx]->data = lt_entry->max_burst_oper;
            } else {
                return;
            }
        }
    }
}

static int
meter_entry_init_params(int unit,
                        int stage,
                        bcmcth_meter_sw_params_t *entry)
{
    bcmlrd_fid_t *fid_list = NULL;
    bcmlrd_field_def_t field_def;
    bcmltd_sid_t sid;
    size_t num_fid = 0, count = 0;
    uint32_t i = 0;
    uint32_t table_sz = 0;
    uint64_t data = 0;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_fid_info_t *fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    sid = attr->ltid;
    fid_info = attr->fid_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthMeterFldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit, sid, num_fid, fid_list, &count));

    for (i = 0; i < count; i++) {
        sal_memset(&field_def, 0, sizeof(field_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid_list[i], &field_def));

        meter_field_def_val_get(&field_def, &data);
        bcmcth_meter_lt_fid_to_entry_data(fid_list[i],
                                          data,
                                          fid_info,
                                          entry);
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

static int
meter_entry_parse_params(int unit,
                         int stage,
                         const bcmltd_field_t *data,
                         bcmcth_meter_fp_fid_info_t *fid_info,
                         bcmcth_meter_sw_params_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    field = data;
    while (field) {
        bcmcth_meter_lt_fid_to_entry_data(field->id,
                                          field->data,
                                          fid_info,
                                          entry);
        field = field->next;
    }

    SHR_FUNC_EXIT();
}

/*
 * Parse IMM fields list and update sc entry structure.
 */
static int
meter_entry_imm_fields_parse(int unit,
                             int stage,
                             bcmltd_fields_t *out,
                             bcmcth_meter_fp_fid_info_t *fid_info,
                             bcmcth_meter_sw_params_t *entry)
{
    uint32_t idx = 0, count = 0, fid = 0;
    uint64_t data = 0;

    SHR_FUNC_ENTER(unit);

    count = out->count;
    for (idx = 0; idx < count; idx++) {
        fid = out->field[idx]->id;
        data = out->field[idx]->data;

        bcmcth_meter_lt_fid_to_entry_data(fid,
                                          data,
                                          fid_info,
                                          entry);
    }

    SHR_FUNC_EXIT();
}

static int
meter_imm_entry_lookup(int unit,
                       int stage,
                       uint32_t meter_id,
                       bcmcth_meter_sw_params_t *lt_entry)
{
    uint32_t key_fid = 0, sid = 0;
    bcmltd_fields_t  in_flds = {0}, out_flds = {0};
    shr_famm_hdl_t   hdl = NULL;
    bcmcth_meter_fp_fid_info_t *fid_info = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
        bcmcth_meter_fp_egr_template_fld_arr_hdl_get(unit, &hdl);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
        bcmcth_meter_fp_ing_template_fld_arr_hdl_get(unit, &hdl);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    sid = attr->ltid;
    key_fid = attr->key_fid;
    fid_info = attr->fid_info;

    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(hdl, 1);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = 32;
    out_flds.field = shr_famm_alloc(hdl, 32);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    in_flds.field[0]->id = key_fid;
    in_flds.field[0]->data = meter_id;

    if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) ==
        SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_imm_fields_parse(unit,
                                      stage,
                                      &out_flds,
                                      fid_info,
                                      lt_entry));
exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 1);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, 32);
    }
    SHR_FUNC_EXIT();
}

static int
meter_entry_hw_insert(int unit,
                      uint32_t trans_id,
                      int stage,
                      int pool,
                      bcmcth_meter_sw_params_t *lt_entry)
{
    uint32_t meter_id = 0, actual_hw_idx = 0;
    uint32_t hw_index = 0;
    uint32_t meter_mode = 0;
    uint8_t oper_mode = 0;
    int pipe_id = 0;
    bool meter_pairing = false;
    bcmcth_meter_fp_drv_t *drv = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;
    bool *hw_idx_state = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;
    bcmcth_meter_fp_event_info_t meter_info;

    SHR_FUNC_ENTER(unit);

    meter_id = lt_entry->meter_id;
    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    meter_entry = &state->list[meter_id];
    hw_idx_state = state->hw_idx;

    sal_memset(&meter_info, 0, sizeof(meter_info));
    meter_info.trans_id = trans_id;
    meter_info.meter_id = meter_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_oper_mode_get(unit, stage, &oper_mode));

    /* Find a free index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_hw_idx_alloc(unit, drv,
                                      pool, oper_mode,
                                      lt_entry, &hw_index));

    meter_entry->hw_fields.meter_hw_index = hw_index;
    meter_entry->hw_fields.meter_pair_index = (hw_index / 2);
    meter_entry->pool = pool;

    /*
     * Convert to HW parameters
     * Find granularity, calculate refresh count and bucket size
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_hw_param_compute(unit, drv,
                                          lt_entry));

    meter_mode = lt_entry->meter_mode;
    METER_FP_PAIRING_CHECK_ASSIGN(meter_mode, meter_pairing);

    if ((attr->meter_pool_global == false) ||
        (meter_entry->ref_count != 0) ||
        (meter_pairing == true)) {
        SHR_IF_ERR_EXIT(bcmptm_mreq_atomic_trans_enable(unit));
    }

    /* Actual write to h/w */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_hw_write(unit, drv, oper_mode,
                                  trans_id, lt_entry));

    pipe_id = lt_entry->pipe;

    actual_hw_idx = (attr->num_meters_per_pipe * pipe_id) + hw_index;
    hw_idx_state[actual_hw_idx] = true;
    if (meter_pairing == true) {
        hw_idx_state[actual_hw_idx + 1] = true;
    }

    meter_entry->meter_installed = true;

    /* Notify if ref count is not zero. */
    if (meter_entry->ref_count != 0) {
        if (stage == METER_FP_STAGE_ID_EGRESS) {
            bcmevm_publish_event_notify(unit,
                                        "METER_EGR_INSERT",
                                        (uintptr_t)&meter_info);
        } else {
            bcmevm_publish_event_notify(unit,
                                        "METER_ING_INSERT",
                                        (uintptr_t)&meter_info);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a meter entry from h/w.
 *
 * Delete the meter entry from hardware and free the hardware index.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
static int
meter_entry_hw_delete(int unit,
                      uint32_t trans_id,
                      int stage,
                      uint32_t meter_id)
{
    uint32_t hw_meter_idx = 0;
    uint8_t oper_mode = 0;
    uint32_t meter_mode = 0;
    bool meter_pairing = false;
    bcmcth_meter_fp_drv_t *drv = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;
    bcmcth_meter_sw_params_t lt_entry;
    bcmcth_meter_fp_event_info_t meter_info;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    meter_entry = &state->list[meter_id];

    /* If meter entry not installed in hw, return. */
    if (meter_entry->meter_installed == false) {
        SHR_EXIT();
    }

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.meter_id = meter_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_init_params(unit,
                                 stage,
                                 &lt_entry));


    SHR_IF_ERR_VERBOSE_EXIT
        (meter_imm_entry_lookup(unit,
                                stage,
                                meter_id,
                                &lt_entry));

    sal_memset(&meter_info, 0, sizeof(meter_info));
    meter_info.trans_id = trans_id;
    meter_info.meter_id = meter_id;

    hw_meter_idx = meter_entry->hw_fields.meter_hw_index;

    /* Clear meter entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_oper_mode_get(unit, stage, &oper_mode));

    meter_mode = lt_entry.meter_mode;
    METER_FP_PAIRING_CHECK_ASSIGN(meter_mode, meter_pairing);

    if ((attr->meter_pool_global == false) ||
        (meter_entry->ref_count != 0) ||
        (meter_pairing == true)) {
        SHR_IF_ERR_EXIT(bcmptm_mreq_atomic_trans_enable(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_hw_clear(unit, drv, oper_mode,
                                  trans_id, meter_entry->pool,
                                  hw_meter_idx, &lt_entry));

    /* MArk hw index as free. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_hw_idx_free(unit, drv,
                                     hw_meter_idx, &lt_entry));

    meter_entry->hw_fields.meter_hw_index = 0;
    meter_entry->hw_fields.meter_pair_index = 0;
    meter_entry->meter_installed = false;

    if (meter_entry->ref_count != 0) {
        if (stage == METER_FP_STAGE_ID_EGRESS) {
            bcmevm_publish_event_notify(unit,
                                        "METER_EGR_DELETE",
                                        (uintptr_t)&meter_info);
        } else {
            bcmevm_publish_event_notify(unit,
                                        "METER_ING_DELETE",
                                        (uintptr_t)&meter_info);
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Update Meter entry
 *
 * Update meter entry in H/W table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
static int
meter_entry_hw_update(int unit,
                      uint32_t trans_id,
                      int stage,
                      int pool,
                      bcmcth_meter_sw_params_t *old_entry,
                      bcmcth_meter_sw_params_t *new_entry)
{
    int pipe_id = 0;
    bool move = false;
    bool old_pair_mode = false, new_pair_mode = false;
    uint32_t meter_id = 0;
    uint32_t cur_index = 0, hw_index = 0, actual_hw_idx = 0;
    uint8_t oper_mode = 0;
    bcmcth_meter_fp_drv_t *drv = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;
    bool *hw_idx_state = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;
    bcmcth_meter_fp_event_info_t meter_info;

    SHR_FUNC_ENTER(unit);

    meter_id = old_entry->meter_id;
    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    meter_entry = &state->list[meter_id];
    hw_idx_state = state->hw_idx;

    sal_memset(&meter_info, 0, sizeof(meter_info));
    meter_info.trans_id = trans_id;
    meter_info.meter_id = meter_id;

    /* If PIPE field has changed, entry has to be moved. */
    if (old_entry->pipe != new_entry->pipe) {
        move = true;
    }

    /* If Pool instance has changed, entry has to be moved. */
    if (old_entry->meter_pool_id != new_entry->meter_pool_id) {
        move = true;
    }

    METER_FP_PAIRING_CHECK_ASSIGN(old_entry->meter_mode, old_pair_mode);
    METER_FP_PAIRING_CHECK_ASSIGN(new_entry->meter_mode, new_pair_mode);

    /* If pairing mode has changed, entry has to be moved. */
    if (old_pair_mode != new_pair_mode) {
        move = true;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_oper_mode_get(unit, stage, &oper_mode));

    if ((attr->meter_pool_global == false) ||
        (meter_entry->ref_count != 0) ||
        (old_pair_mode == true) ||
        (new_pair_mode == true) ||
        (move == true)) {
        SHR_IF_ERR_EXIT(bcmptm_mreq_atomic_trans_enable(unit));
    }

    cur_index = hw_index = meter_entry->hw_fields.meter_hw_index;
    if (move == true) {
        /* Mark old index as free. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_fp_hw_idx_free(unit, drv,
                                         cur_index, old_entry));

        /* Find another free index. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_fp_hw_idx_alloc(unit, drv,
                                          pool, oper_mode,
                                          new_entry, &hw_index));

        meter_entry->hw_fields.meter_hw_index = hw_index;
        meter_entry->hw_fields.meter_pair_index = (hw_index / 2);
        meter_entry->pool = pool;
    }

    /* Get the HW values for the new meter params and write to H/W */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_hw_param_compute(unit, drv,
                                          new_entry));

    /* Write the meter entry to h/w. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_hw_write(unit, drv, oper_mode,
                                  trans_id, new_entry));

    pipe_id = new_entry->pipe;
    actual_hw_idx = ((attr->num_meters_per_pipe * pipe_id) + hw_index);
    hw_idx_state[actual_hw_idx] = true;
    if (new_pair_mode == true) {
        hw_idx_state[actual_hw_idx + 1] = true;
    }

    meter_entry->meter_installed = true;

    if (meter_entry->ref_count != 0) {
        if (stage == METER_FP_STAGE_ID_EGRESS) {
            bcmevm_publish_event_notify(unit,
                                        "METER_EGR_UPDATE",
                                        (uintptr_t)&meter_info);
        } else {
            bcmevm_publish_event_notify(unit,
                                        "METER_ING_UPDATE",
                                        (uintptr_t)&meter_info);
        }
    }

    if (cur_index != hw_index) {
        /* Clear previous hw index. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_fp_oper_mode_get(unit, stage, &oper_mode));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_fp_hw_clear(unit, drv, oper_mode,
                                      trans_id, old_entry->meter_pool_id,
                                      cur_index, old_entry));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
meter_entry_get_first(int unit,
                      int stage,
                      uint32_t *meter_id)
{
    uint32_t            idx = 0, sid = 0;
    uint64_t            min = 0, max = 0;
    bcmltd_sid_t        key_fid = 0;
    bcmlrd_field_def_t  field_def;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    sid = attr->ltid;
    key_fid = attr->key_fid;

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(
                unit,
                sid,
                key_fid,
                &field_def));

    meter_field_min_val_get(&field_def, &min);
    meter_field_max_val_get(&field_def, &max);
    for (idx = min; idx <= max; idx++) {
        if (stage == METER_FP_STAGE_ID_EGRESS) {
            bcmcth_meter_fp_egr_sw_state_get(unit, &state);
        } else {
            bcmcth_meter_fp_ing_sw_state_get(unit, &state);
        }
        meter_entry = &state->list[idx];
        if (meter_entry->meter_in_use == true) {
            *meter_id = idx;
            break;
        }
    }

    if (idx > max) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}


static int
meter_lt_fields_update(int unit,
                       bcmcth_meter_fp_fid_info_t *fid_info,
                       bcmcth_meter_sw_params_t *lt_entry,
                       bcmltd_fields_t *out)
{
    uint32_t idx = 0;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < out->count; idx++) {
        bcmcth_meter_fp_oper_field_update(unit,
                                          idx,
                                          fid_info,
                                          lt_entry,
                                          out);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Insert meter into hardware table.
 *
 * Find a free index and write this meter into hardware table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] pool Meter pool id.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 * \retval SHR_E_FULL Hardware meter table full.
 * \retval SHR_E_PARAM Invalid meter param.
 */
int
bcmcth_meter_entry_insert(int unit,
                          uint32_t trans_id,
                          int stage,
                          int pool,
                          uint32_t meter_id)
{
    bcmcth_meter_sw_params_t lt_entry;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    meter_entry = &state->list[meter_id];

    /* Meter has already been programmed in hardware, Return */
    if (meter_entry->meter_installed) {
        if ((attr->meter_pool_global == false) &&
            (pool != meter_entry->pool)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_EXIT();
    }

    /* Meter entry not installed yet. Save pool information and return. */
    if (meter_entry->meter_in_use == false) {
        meter_entry->pool = pool;
        SHR_EXIT();
    }

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.meter_id = meter_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_init_params(unit,
                                 stage,
                                 &lt_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_imm_entry_lookup(unit,
                                stage,
                                meter_id,
                                &lt_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_hw_insert(unit,
                               trans_id,
                               stage,
                               pool,
                               &lt_entry));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach an FP entry to the meter.
 *
 * Increment the ref count of this meter entry to keep track of
 * number of FP entries pointing to this meter.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_meter_entry_attach(int unit,
                          int stage,
                          uint32_t meter_id)
{
    bcmcth_meter_fp_sw_state_t *state = NULL;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }

    meter_entry = &state->list[meter_id];

    /*
     * Called from FP Entry insert/update.
     * Increment ref count and return
     */
    meter_entry->ref_count++;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup an FP entry.
 *
 * Lookup the meter entry from Software meter table
 * and retrieve meter parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 * \param [out] meter_entry Pointer to meter entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmcth_meter_entry_lookup(int unit,
                          int stage,
                          uint32_t meter_id,
                          bcmcth_meter_fp_policy_fields_t *meter_entry)
{
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;
    bcmcth_meter_fp_entry_t *entry = NULL;

    SHR_FUNC_ENTER(unit);

    if (meter_entry == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    entry = &state->list[meter_id];

    /* Copy from HA structure */
    meter_entry->meter_pair_index = entry->hw_fields.meter_pair_index;
    meter_entry->meter_pair_mode = entry->hw_fields.meter_pair_mode;
    meter_entry->mode_modifier = entry->hw_fields.mode_modifier;
    meter_entry->meter_update_odd = entry->hw_fields.meter_update_odd;
    meter_entry->meter_test_odd = entry->hw_fields.meter_test_odd;
    meter_entry->meter_update_even = entry->hw_fields.meter_update_even;
    meter_entry->meter_test_even = entry->hw_fields.meter_test_even;
    meter_entry->pipe = entry->pipe;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a meter entry.
 *
 * Delete the meter entry from hardware if ref count is zero and free the
 * hardware index.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmcth_meter_entry_delete(int unit,
                          uint32_t trans_id,
                          int stage,
                          uint32_t meter_id)
{
    bcmcth_meter_fp_entry_t *meter_entry = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }

    meter_entry = &state->list[meter_id];

    /*
     * If ref count is not zero, return
     */
    if (meter_entry->ref_count != 0) {
        SHR_EXIT();
    }

    /*
     * Clear meter entry.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_hw_delete(unit, trans_id, stage, meter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach an FP entry from the meter.
 *
 * Decrement the ref count of this meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_meter_entry_detach(int unit,
                          int stage,
                          uint32_t meter_id)
{
    bcmcth_meter_fp_entry_t *meter_entry = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }

    meter_entry = &state->list[meter_id];

    /*
     * Called from FP Entry delete/update.
     * Decrement ref count and return
     */
    if (meter_entry->ref_count != 0) {
        meter_entry->ref_count--;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate meter id.
 *
 * Check if the meter entry is present in s/w table.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software Meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Meter entry not present in s/w table.
 */
int
bcmcth_meter_entry_valid(int unit,
                         int stage,
                         uint32_t meter_id)
{
    bcmcth_meter_fp_entry_t *meter_entry = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    meter_entry = &state->list[meter_id];

    if (meter_entry->meter_in_use == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Return global status of meter pool.
 *
 * Check the chip specific attributes and return whether the meter pool
 * is global or not.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [out] global Global pool status of meters in this FP stage.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_meter_pool_global(int unit,
                         int stage,
                         bool *global)
{
    bcmcth_meter_fp_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    *global = attr->meter_pool_global;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure FP meter related device info.
 *
 * Initialize FP meter related device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_meter_fp_dev_config(int unit,
                           int stage,
                           uint32_t refresh_en)
{
    bcmcth_meter_fp_drv_t *drv = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    } else {
        bcmcth_meter_fp_ing_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_hw_config(unit, drv,
                                   refresh_en));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get FP Pipe mode configured.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [out] mode Operational mode.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_meter_fp_oper_mode_get(int unit,
                              int stage,
                              uint8_t *mode)
{
    int rv = SHR_E_NONE;
    uint64_t val = 0;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    *mode = METER_FP_OPER_MODE_GLOBAL;

    rv = bcmlrd_map_get(unit, METER_FP_CONFIGt, &map);
    if (SHR_SUCCESS(rv) && map) {
        if (stage == METER_FP_STAGE_ID_EGRESS) {
            rv = bcmcfg_field_get(unit,
                                  METER_FP_CONFIGt,
                                  METER_FP_CONFIGt_METER_EGR_OPERMODE_PIPEUNIQUEf,
                                  &val);
        } else {
            rv = bcmcfg_field_get(unit,
                                  METER_FP_CONFIGt,
                                  METER_FP_CONFIGt_METER_ING_OPERMODE_PIPEUNIQUEf,
                                  &val);
        }
    }

    if (rv == SHR_E_NONE) {
        *mode = val;
    }

    SHR_FUNC_EXIT();
}

void
bcmcth_meter_fp_event_cb(int unit,
                         const char *event,
                         uint64_t ev_data)
{
    bcmcth_meter_fp_attrib_t *attr = NULL;
    uint32_t trans_id = (uint32_t)ev_data;

    bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    if (attr == NULL) {
        return;
    }

    if (sal_strcmp(event, "FP_ING_COMMIT") == 0) {
        bcmcth_meter_ing_template_commit(unit, attr->ltid, trans_id, NULL);
    } else if (sal_strcmp(event, "FP_ING_ABORT") == 0) {
        bcmcth_meter_ing_template_abort(unit, attr->ltid, trans_id, NULL);
    } else if (sal_strcmp(event, "FP_EGR_COMMIT") == 0) {
        bcmcth_meter_egr_template_commit(unit, attr->ltid, trans_id, NULL);
    } else if (sal_strcmp(event, "FP_EGR_ABORT") == 0) {
        bcmcth_meter_egr_template_abort(unit, attr->ltid, trans_id, NULL);
    }
}

int
bcmcth_meter_template_insert(int unit,
                             int stage,
                             uint32_t trans_id,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data)
{
    uint32_t meter_id = 0, meter_pool_id = 0;
    bcmltd_sid_t key_fid = 0;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_sw_params_t lt_entry;
    bcmcth_meter_fp_entry_t *meter_entry= NULL;
    bcmcth_meter_fp_fid_info_t *fid_info = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    key_fid = attr->key_fid;
    fid_info = attr->fid_info;

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &meter_id));
    lt_entry.meter_id = meter_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_init_params(unit,
                                 stage,
                                 &lt_entry));

    /*
     * Parse the input fields and copy them to meter_entry->fields
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_parse_params(unit,
                                  stage,
                                  data,
                                  fid_info,
                                  &lt_entry));

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    meter_entry = &state->list[meter_id];


    meter_entry->meter_in_use = true;
    meter_entry->pipe = lt_entry.pipe;

    if (attr->meter_pool_global || meter_entry->ref_count) {
        /*
         * Program H/W meter table if Pool ID is known at this time.
         */
        if (attr->meter_pool_global) {
            meter_pool_id = lt_entry.meter_pool_id;
        } else {
            meter_pool_id = meter_entry->pool;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_entry_hw_insert(unit,
                                   trans_id,
                                   stage,
                                   meter_pool_id,
                                   &lt_entry));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_template_delete(int unit,
                             int stage,
                             uint32_t trans_id,
                             const bcmltd_field_t *key)
{
    uint32_t meter_id = 0, key_fid = 0;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_entry_t *meter_entry= NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    key_fid = attr->key_fid;

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &meter_id));

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    meter_entry = &state->list[meter_id];

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_hw_delete(unit,
                               trans_id,
                               stage,
                               meter_id));

    meter_entry->meter_in_use = false;
    meter_entry->pipe = 0;
    sal_memset(&meter_entry->hw_fields, 0, sizeof(bcmcth_meter_hw_params_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_template_update(int unit,
                             int stage,
                             uint32_t trans_id,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data)
{
    uint32_t key_fid = 0, meter_id = 0, meter_pool_id = 0;
    bcmcth_meter_sw_params_t old_entry;
    bcmcth_meter_sw_params_t new_entry;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_entry_t *meter_entry= NULL;
    bcmcth_meter_fp_fid_info_t *fid_info = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    key_fid = attr->key_fid;
    fid_info = attr->fid_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &meter_id));

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    meter_entry = &state->list[meter_id];

    sal_memset(&old_entry, 0, sizeof(old_entry));
    sal_memset(&new_entry, 0, sizeof(new_entry));
    old_entry.meter_id = meter_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_init_params(unit,
                                 stage,
                                 &old_entry));


    SHR_IF_ERR_VERBOSE_EXIT
        (meter_imm_entry_lookup(unit,
                                stage,
                                meter_id,
                                &old_entry));
    sal_memcpy(&new_entry, &old_entry, sizeof(new_entry));
    /*
     * Parse the input fields and copy them to meter_entry->fields
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_parse_params(unit,
                                  stage,
                                  data,
                                  fid_info,
                                  &new_entry));

    if (attr->meter_pool_global) {
        meter_pool_id = new_entry.meter_pool_id;
    } else {
        meter_pool_id = meter_entry->pool;
    }

    meter_entry->pipe = new_entry.pipe;
    if (attr->meter_pool_global || meter_entry->ref_count) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_entry_hw_update(unit,
                                   trans_id,
                                   stage,
                                   meter_pool_id,
                                   &old_entry,
                                   &new_entry));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_template_lookup(int unit,
                             int stage,
                             uint32_t trans_id,
                             void *context,
                             bcmimm_lookup_type_t lkup_type,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out)
{
    uint32_t meter_id = 0;
    bcmltd_sid_t key_fid = 0;
    bcmcth_meter_fp_drv_t *drv = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_sw_params_t lt_entry;
    bcmcth_meter_fp_fid_info_t *fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    } else {
        bcmcth_meter_fp_ing_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    key_fid = attr->key_fid;
    fid_info = attr->fid_info;

    if (in != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_entry_field_value_get(unit, in, key_fid, &meter_id));
    }

    if (lkup_type == BCMIMM_TRAVERSE && in == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_entry_get_first(unit, stage, &meter_id));
    }

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.meter_id = meter_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_entry_init_params(unit,
                                 stage,
                                 &lt_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_imm_entry_lookup(unit,
                                stage,
                                meter_id,
                                &lt_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_hw_oper_get(unit, drv,
                                     &lt_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_meter_format_get(unit, drv,
                                          &lt_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_lt_fields_update(unit,
                                fid_info,
                                &lt_entry,
                                out));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_util_used_entries_calc(int unit,
                                    int stage,
                                    uint32_t *num_entries)
{
    int i = 0;
    int max = 0, min = 0;
    uint8_t oper_mode = 0;
    uint32_t in_use_count = 0;
    bool *hw_idx_state = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_sw_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_EGRESS) {
        bcmcth_meter_fp_egr_attrib_get(unit, &attr);
        bcmcth_meter_fp_egr_sw_state_get(unit, &state);
    } else {
        bcmcth_meter_fp_ing_attrib_get(unit, &attr);
        bcmcth_meter_fp_ing_sw_state_get(unit, &state);
    }
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    hw_idx_state = state->hw_idx;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_oper_mode_get(unit, stage, &oper_mode));

    if (oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) {
        min = 0;
        max = (attr->num_meters_per_pipe * attr->num_pipes);
    } else {
        min = 0;
        max = attr->num_meters_per_pipe;
    }

    /*
     * Iterate over the pool of the particular pipe and find a
     * free index
     */
    for (i = min; i < max; i++) {
        if (hw_idx_state[i] == true) {
            in_use_count++;
        }
    }

    *num_entries = in_use_count;
exit:
    SHR_FUNC_EXIT();
}

