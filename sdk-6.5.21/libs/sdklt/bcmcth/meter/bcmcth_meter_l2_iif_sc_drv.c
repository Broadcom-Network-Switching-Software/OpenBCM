/*! \file bcmcth_meter_l2_iif_sc_drv.c
 *
 * This file defines HW interface handler functions for
 * storm control meters.
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
#include <bcmcth/bcmcth_meter_l2_iif_sc.h>

#include <bcmcth/bcmcth_meter_l2_iif_sc_drv.h>

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
 * Initialize sc entry structure with default values.
 */
static int
meter_l2_iif_sc_init_params (int unit,
                             bcmcth_meter_l2_iif_sc_entry_t *entry)
{
    bcmlrd_fid_t *fid_list = NULL;
    bcmlrd_field_def_t field_def;
    bcmlrd_sid_t    sid;
    size_t num_fid = 0, count = 0;
    uint32_t i = 0, j = 0, arr_len = 0;
    uint32_t table_sz = 0;

    SHR_FUNC_ENTER(unit);

    sid = METER_L2_IIF_STORM_CONTROLt;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthMeterFldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit, sid, num_fid, fid_list, &count));

    for (i = 0; i < count; i++) {
        sal_memset(&field_def, 0, sizeof(field_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid_list[i], &field_def));
        arr_len = field_def.depth;
        for (j = 0; j < arr_len; j++) {
            switch (fid_list[i]) {
            case METER_L2_IIF_STORM_CONTROLt_METER_RATE_KBPSf:
                entry->rate_kbps[j] = METER_L2_IIF_FIELD_DEF(field_def);
                break;
            case METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_KBITSf:
                entry->burst_kbits[j] = METER_L2_IIF_FIELD_DEF(field_def);
                break;
            case METER_L2_IIF_STORM_CONTROLt_METER_RATE_PPSf:
                entry->rate_pps[j] = METER_L2_IIF_FIELD_DEF(field_def);
                break;
            case METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_PKTSf:
                entry->burst_pkts[j] = METER_L2_IIF_FIELD_DEF(field_def);
                break;
            default:
                break;
            }
        }
    }
exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*
 * Copy input data to SC entry structure.
 */
static void
meter_l2_iif_sc_fid_to_entry_data(uint32_t fid,
                                  uint32_t idx,
                                  uint64_t data,
                                  bcmcth_meter_l2_iif_sc_entry_t *entry)
{
    switch (fid) {
    case METER_L2_IIF_STORM_CONTROLt_BYTE_MODEf:
        entry->byte_mode = data;
        break;
    case METER_L2_IIF_STORM_CONTROLt_METER_OFFSETf:
        entry->meter_en[idx] = data;
        break;
    case METER_L2_IIF_STORM_CONTROLt_METER_RATE_KBPSf:
        entry->rate_kbps[idx] = data;
        break;
    case METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_KBITSf:
        entry->burst_kbits[idx] = data;
        break;
    case METER_L2_IIF_STORM_CONTROLt_METER_RATE_PPSf:
        entry->rate_pps[idx] = data;
        break;
    case METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_PKTSf:
        entry->burst_pkts[idx] = data;
        break;
    case METER_L2_IIF_STORM_CONTROLt_METER_RATE_KBPS_OPERf:
        entry->rate_kbps_oper[idx] = data;
        break;
    case METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_KBITS_OPERf:
        entry->burst_kbits_oper[idx] = data;
        break;
    case METER_L2_IIF_STORM_CONTROLt_METER_RATE_PPS_OPERf:
        entry->rate_pps_oper[idx] = data;
        break;
    case METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_PKTS_OPERf:
        entry->burst_pkts_oper[idx] = data;
        break;
    default:
        break;
    }
}

/*
 * Parse IMM fields list and update sc entry structure.
 */
static int
meter_l2_iif_imm_fields_parse(int unit,
                              bcmltd_fields_t *out,
                              bcmcth_meter_l2_iif_sc_entry_t *entry)
{
    uint32_t idx = 0, count = 0, arr_idx = 0, fid = 0;
    uint64_t data = 0;

    SHR_FUNC_ENTER(unit);

    count = out->count;
    for (idx = 0; idx < count; idx++) {
        fid = out->field[idx]->id;
        arr_idx = out->field[idx]->idx;
        data = out->field[idx]->data;

        meter_l2_iif_sc_fid_to_entry_data(fid,
                                          arr_idx,
                                          data,
                                          entry);
    }

    SHR_FUNC_EXIT();
}

/*
 * Parse input field list and update sc entry structure.
 */
static int
meter_l2_iif_input_fields_parse(int unit,
                                const bcmltd_field_t *list,
                                bcmcth_meter_l2_iif_sc_entry_t *entry)
{
    uint32_t idx = 0;
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    field = list;
    while (field) {
        idx = field->idx;
        meter_l2_iif_sc_fid_to_entry_data(field->id,
                                          idx,
                                          field->data,
                                          entry);
        field = field->next;
    }

    SHR_FUNC_EXIT();
}

/*
 * Read from a PT.
 */
static int
meter_l2_iif_sc_hw_read(int unit,
                        uint32_t trans_id,
                        bcmltd_sid_t lt_id,
                        bcmdrd_sid_t pt_id,
                        uint32_t index,
                        int pipe,
                        void *data)
{
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    bcmbd_pt_dyn_info_t pt_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    pt_info.tbl_inst = pipe;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
#if 0
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_READ,
                                   NULL, rsp_entry_wsize, lt_id,
                                   trans_id, NULL, NULL,
                                   data, &rsp_ltid, &rsp_flags));
#endif
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_ireq_read(unit, 0, pt_id, &pt_info, NULL,
                             rsp_entry_wsize, lt_id,
                             data, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();

}

/*
 * Write to a PT.
 */
static int
meter_l2_iif_sc_hw_write(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t lt_id,
                         bcmdrd_sid_t pt_id,
                         uint32_t index,
                         int pipe,
                         void *data)
{
    uint32_t        rsp_flags = 0;
    bcmltd_sid_t    rsp_ltid = 0;
    bcmbd_pt_dyn_info_t pt_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    pt_info.tbl_inst = pipe;
#if 0
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_WRITE,
                                   data, 0, lt_id,
                                   trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));
#endif
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_ireq_write(unit, 0, pt_id, &pt_info, NULL,
                              data, lt_id,
                              &rsp_ltid, &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Read SC meter config data.
 */
static int
meter_l2_iif_sc_get_hw_config(int unit,
                              uint32_t sid,
                              uint32_t trans_id,
                              uint32_t meter_id,
                              int pipe,
                              uint32_t *byte_mode,
                              uint32_t *pkt_quantum)
{
    uint32_t pt_id = 0, alloc_sz = 0, fid = 0;
    uint8_t  *buf = NULL;
    bcmcth_meter_l2_iif_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_l2_iif_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pt_id = drv->config_ptid;
    alloc_sz = bcmdrd_pt_entry_wsize(unit, pt_id);
    alloc_sz *= 4;

    SHR_ALLOC(buf, alloc_sz, "bcmcthMeterScHwWrite");
    SHR_NULL_CHECK(buf, SHR_E_MEMORY);
    sal_memset(buf, 0, alloc_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_sc_hw_read(unit,
                                 trans_id,
                                 sid,
                                 pt_id,
                                 meter_id,
                                 pipe,
                                 (uint32_t *)buf));

    fid =  drv->byte_fid;
    bcmdrd_pt_field_get(unit, pt_id, (uint32_t *)buf, fid, byte_mode);
    fid =  drv->quantum_fid;
    bcmdrd_pt_field_get(unit, pt_id, (uint32_t *)buf, fid, pkt_quantum);

exit:
    SHR_FREE(buf);
    SHR_FUNC_EXIT();
}

/*
 * Write the H/W tables.
 * Program SC meter config table and SC meter table.
 */
static int
meter_l2_iif_sc_entry_install(int unit,
                              uint32_t sid,
                              uint32_t trans_id,
                              bcmcth_meter_l2_iif_sc_entry_t *entry)
{
    uint32_t pt_id = 0, alloc_sz = 0, fid = 0;
    uint32_t meter_index = 0;
    uint8_t  *config_buf = NULL;
    uint8_t  *meter_buf = NULL;
    uint8_t  i = 0, num_offset = 0;
    bcmcth_meter_l2_iif_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_l2_iif_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    num_offset = drv->num_offset;
    pt_id = drv->config_ptid;
    alloc_sz = bcmdrd_pt_entry_wsize(unit, pt_id);
    alloc_sz *= 4;

    SHR_ALLOC(config_buf, alloc_sz, "bcmcthMeterScHwCfgWrite");
    SHR_NULL_CHECK(config_buf, SHR_E_MEMORY);
    sal_memset(config_buf, 0, alloc_sz);

    fid = drv->byte_fid;
    bcmdrd_pt_field_set(unit, pt_id, (uint32_t *)config_buf, fid, &entry->byte_mode);
    fid = drv->quantum_fid;
    bcmdrd_pt_field_set(unit, pt_id, (uint32_t *)config_buf, fid, &entry->quantum);

    /* Program SC config table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_sc_hw_write(unit,
                                  trans_id,
                                  sid,
                                  pt_id,
                                  entry->meter_id,
                                  entry->pipe,
                                  (uint32_t *)config_buf));

    pt_id = drv->meter_ptid;
    alloc_sz = bcmdrd_pt_entry_wsize(unit, pt_id);
    alloc_sz *= 4;

    SHR_ALLOC(meter_buf, alloc_sz, "bcmcthMeterScHwWrite");
    SHR_NULL_CHECK(meter_buf, SHR_E_MEMORY);

    for (i = 0; i < num_offset; i++) {
        /* Program each offset of the SC meter table. */
        sal_memset(meter_buf, 0, alloc_sz);

        fid = drv->rc_fid;
        bcmdrd_pt_field_set(unit, pt_id, (uint32_t *)meter_buf, fid, &entry->refresh_count[i]);
        fid = drv->bs_fid;
        bcmdrd_pt_field_set(unit, pt_id, (uint32_t *)meter_buf, fid, &entry->bucket_size[i]);
        fid = drv->bc_fid;
        bcmdrd_pt_field_set(unit, pt_id, (uint32_t *)meter_buf, fid, &entry->bucket_count[i]);

        meter_index = ((entry->meter_id * num_offset) + i);
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_l2_iif_sc_hw_write(unit,
                                      trans_id,
                                      sid,
                                      pt_id,
                                      meter_index,
                                      entry->pipe,
                                      (uint32_t *)meter_buf));
    }

exit:
    SHR_FREE(config_buf);
    SHR_FREE(meter_buf);
    SHR_FUNC_EXIT();
}

/*
 * Delete the meter entry from H/w
 * Clear the config table as well as meter table.
 */
static int
meter_l2_iif_sc_entry_destroy(int unit,
                              uint32_t sid,
                              uint32_t trans_id,
                              bcmcth_meter_l2_iif_sc_entry_t *entry)
{
    uint32_t pt_id = 0, alloc_sz = 0;
    uint32_t meter_index = 0;
    uint8_t  *config_buf = NULL;
    uint8_t  *meter_buf = NULL;
    uint8_t  i = 0, num_offset = 0;
    bcmcth_meter_l2_iif_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_l2_iif_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    num_offset = drv->num_offset;
    pt_id = drv->config_ptid;
    alloc_sz = bcmdrd_pt_entry_wsize(unit, pt_id);
    alloc_sz *= 4;

    SHR_ALLOC(config_buf, alloc_sz, "bcmcthMeterScHwCfgWrite");
    SHR_NULL_CHECK(config_buf, SHR_E_MEMORY);
    sal_memset(config_buf, 0, alloc_sz);

    /* Clear Storm control meter config table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_sc_hw_write(unit,
                                  trans_id,
                                  sid,
                                  pt_id,
                                  entry->meter_id,
                                  entry->pipe,
                                  (uint32_t *)config_buf));

    pt_id = drv->meter_ptid;
    alloc_sz = bcmdrd_pt_entry_wsize(unit, pt_id);
    alloc_sz *= 4;

    SHR_ALLOC(meter_buf, alloc_sz, "bcmcthMeterScHwWrite");
    SHR_NULL_CHECK(meter_buf, SHR_E_MEMORY);

    sal_memset(meter_buf, 0, alloc_sz);
    for (i = 0; i < num_offset; i++) {
        /* Clear each offset of the storm control meter table. */
        meter_index = ((entry->meter_id * num_offset) + i);
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_l2_iif_sc_hw_write(unit,
                                      trans_id,
                                      sid,
                                      pt_id,
                                      meter_index,
                                      entry->pipe,
                                      (uint32_t *)meter_buf));
    }

exit:
    SHR_FREE(config_buf);
    SHR_FREE(meter_buf);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief FP meter action insert
 *
 * Insert an FP Meter action in the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 * \param [in]  out           Output data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_l2_iif_sc_insert(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out)
{
    bool     pipe_unmapped = false;
    uint32_t pipe = 0, ret_cnt = 0, idx = 0;
    uint32_t num_offset = 0;
    bcmcth_meter_l2_iif_sc_entry_t sc_entry;
    bcmcth_meter_l2_iif_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_l2_iif_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(&sc_entry, 0, sizeof(sc_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               METER_L2_IIF_STORM_CONTROLt_METER_L2_IIF_STORM_CONTROL_IDf,
                               &sc_entry.meter_id));

    /* Check if PIPE field is valid. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_is_unmapped(unit,
                                  sid,
                                  METER_L2_IIF_STORM_CONTROLt_PIPEf,
                                  &pipe_unmapped));

    if (pipe_unmapped == TRUE) {
        sc_entry.pipe = -1;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_field_value_get(unit,
                                   key,
                                   METER_L2_IIF_STORM_CONTROLt_PIPEf,
                                   &pipe));
        sc_entry.pipe = pipe;
    }

    /* Initialize SC meter structure with default vlaues. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_sc_init_params(unit, &sc_entry));

    /* Update the structure based on current user input. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_input_fields_parse(unit,
                                         data,
                                         &sc_entry));

    /* Convert LT entry into corresponding HW fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (drv->compute_hw_param(unit, &sc_entry));

    /* Program the H/W tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_sc_entry_install(unit,
                                       sid,
                                       trans_id,
                                       &sc_entry));

    /* Fill and return OPER fields. */
    ret_cnt = 0;
    if (out) {
        num_offset = drv->num_offset;
        for (idx = 0; idx < num_offset; idx++) {
            if (sc_entry.byte_mode == 1) {
                out->field[ret_cnt]->id = METER_L2_IIF_STORM_CONTROLt_METER_RATE_KBPS_OPERf;
                out->field[ret_cnt]->idx = idx;
                out->field[ret_cnt]->data = sc_entry.rate_kbps_oper[idx];
                ret_cnt++;
                out->field[ret_cnt]->id = METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_KBITS_OPERf;
                out->field[ret_cnt]->idx = idx;
                out->field[ret_cnt]->data = sc_entry.burst_kbits_oper[idx];
                ret_cnt++;
            } else {
                out->field[ret_cnt]->id = METER_L2_IIF_STORM_CONTROLt_METER_RATE_PPS_OPERf;
                out->field[ret_cnt]->idx = idx;
                out->field[ret_cnt]->data = sc_entry.rate_pps_oper[idx];
                ret_cnt++;
                out->field[ret_cnt]->id = METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_PKTS_OPERf;
                out->field[ret_cnt]->idx = idx;
                out->field[ret_cnt]->data = sc_entry.burst_pkts_oper[idx];
                ret_cnt++;
            }
        }
        out->count = ret_cnt;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Storm control meter update.
 *
 * Update a storm control meter in the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 * \param [in]  out           Output data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_l2_iif_sc_update(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out)
{
    bool     pipe_unmapped = false;
    uint32_t hw_byte_mode = 0;
    uint32_t pkt_quantum = 0, idx = 0;
    uint32_t pipe = 0, ret_cnt = 0, num_keys = 0;
    uint32_t num_offset = 0;
    bcmcth_meter_l2_iif_sc_entry_t sc_entry;
    bcmltd_fields_t  in_flds = {0}, out_flds = {0};
    shr_famm_hdl_t   hdl = NULL;
    bcmcth_meter_l2_iif_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    /* If not data field is being updated, return. */
    if (data == NULL) {
        SHR_EXIT();
    }

    bcmcth_meter_l2_iif_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(&sc_entry, 0, sizeof(sc_entry));

    num_keys = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               METER_L2_IIF_STORM_CONTROLt_METER_L2_IIF_STORM_CONTROL_IDf,
                               &sc_entry.meter_id));

    /* Check if PIPE field is valid. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_is_unmapped(unit,
                                  sid,
                                  METER_L2_IIF_STORM_CONTROLt_PIPEf,
                                  &pipe_unmapped));

    if (pipe_unmapped == TRUE) {
        sc_entry.pipe = -1;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_field_value_get(unit,
                                   key,
                                   METER_L2_IIF_STORM_CONTROLt_PIPEf,
                                   &pipe));
        sc_entry.pipe = pipe;
        num_keys++;
    }

    /*
     * Do an IMM lookup to retrieve the
     * previous fields in this LT entry.
     */
    bcmcth_meter_l2_iif_sc_arr_hdl_get(unit, &hdl);
    in_flds.count = num_keys;
    in_flds.field = shr_famm_alloc(hdl, num_keys);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = 64;
    out_flds.field = shr_famm_alloc(hdl, 64);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    in_flds.field[0]->id = METER_L2_IIF_STORM_CONTROLt_METER_L2_IIF_STORM_CONTROL_IDf;
    in_flds.field[0]->data = sc_entry.meter_id;
    if (pipe_unmapped == FALSE) {
        in_flds.field[1]->id = METER_L2_IIF_STORM_CONTROLt_PIPEf;
        in_flds.field[1]->data = sc_entry.pipe;
    }

    if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) == SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Initialize the SC meter structure with default value. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_sc_init_params(unit, &sc_entry));

    /* Update the fields of SC meter structure based on the fields from IMM lookup. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_imm_fields_parse(unit,
                                       &out_flds,
                                       &sc_entry));

    /* Update the fields again based on current user input. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_input_fields_parse(unit,
                                         data,
                                         &sc_entry));

    /* Get the current SC meter configuration value. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_sc_get_hw_config(unit,
                                       sid,
                                       trans_id,
                                       sc_entry.meter_id,
                                       sc_entry.pipe,
                                       &hw_byte_mode,
                                       &pkt_quantum));
    sc_entry.quantum = pkt_quantum;

    /* Convert the LT entry fields into HW fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (drv->compute_hw_param(unit, &sc_entry));

    /* Program the HW tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_sc_entry_install(unit,
                                       sid,
                                       trans_id,
                                       &sc_entry));

    /* Fill and return OPER fields. */
    ret_cnt = 0;
    if (out) {
        num_offset = drv->num_offset;
        for (idx = 0; idx < num_offset; idx++) {
            out->field[ret_cnt]->id = METER_L2_IIF_STORM_CONTROLt_METER_RATE_KBPS_OPERf;
            out->field[ret_cnt]->idx = idx;
            if (sc_entry.byte_mode == 1) {
                out->field[ret_cnt]->data = sc_entry.rate_kbps_oper[idx];
            } else {
                out->field[ret_cnt]->data = 0;
            }
            ret_cnt++;
            out->field[ret_cnt]->id = METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_KBITS_OPERf;
            out->field[ret_cnt]->idx = idx;
            if (sc_entry.byte_mode == 1) {
                out->field[ret_cnt]->data = sc_entry.burst_kbits_oper[idx];
            } else {
                out->field[ret_cnt]->data = 0;
            }
            ret_cnt++;

            out->field[ret_cnt]->id = METER_L2_IIF_STORM_CONTROLt_METER_RATE_PPS_OPERf;
            out->field[ret_cnt]->idx = idx;
            if (sc_entry.byte_mode == 1) {
                out->field[ret_cnt]->data = 0;
            } else {
                out->field[ret_cnt]->data = sc_entry.rate_pps_oper[idx];
            }
            ret_cnt++;
            out->field[ret_cnt]->id = METER_L2_IIF_STORM_CONTROLt_BURST_SIZE_PKTS_OPERf;
            out->field[ret_cnt]->idx = idx;
            if (sc_entry.byte_mode == 1) {
                out->field[ret_cnt]->data = 0;
            } else {
                out->field[ret_cnt]->data = sc_entry.burst_pkts_oper[idx];
            }
            ret_cnt++;
        }
        out->count = ret_cnt;
    }

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, num_keys);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, 64);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP meter action delete
 *
 * Delete an FP Meter action from the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_l2_iif_sc_delete(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              const bcmltd_field_t *key)
{
    bool pipe_unmapped = false;
    uint32_t pipe = 0;
    bcmcth_meter_l2_iif_sc_entry_t sc_entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&sc_entry, 0, sizeof(sc_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               METER_L2_IIF_STORM_CONTROLt_METER_L2_IIF_STORM_CONTROL_IDf,
                               &sc_entry.meter_id));

    /* Check if PIPE field is valid. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_is_unmapped(unit,
                                  sid,
                                  METER_L2_IIF_STORM_CONTROLt_PIPEf,
                                  &pipe_unmapped));

    if (pipe_unmapped == TRUE) {
        sc_entry.pipe = -1;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_field_value_get(unit,
                                   key,
                                   METER_L2_IIF_STORM_CONTROLt_PIPEf,
                                   &pipe));
        sc_entry.pipe = pipe;
    }

    /*
     * Delete the entry fom hardware.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_l2_iif_sc_entry_destroy(unit,
                                       sid,
                                       trans_id,
                                       &sc_entry));
exit:
    SHR_FUNC_EXIT();
}
