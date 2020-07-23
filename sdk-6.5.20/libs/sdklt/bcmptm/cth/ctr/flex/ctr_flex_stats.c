/*! \file ctr_flex_stats.c
 *
 * custom tabler handler routines for the flex-ctr stats LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ctr_flex_internal.h"
#include <shr/shr_error.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_internal.h>

static bcmlrd_sid_t req_ltid[2] = {CTR_ING_FLEX_STATSt, CTR_EGR_FLEX_STATSt};
static bcmlrd_fid_t pkts_fid[2] = {CTR_ING_FLEX_STATSt_PKTSf,
                                  CTR_EGR_FLEX_STATSt_PKTSf};
static bcmlrd_fid_t bytes_fid[2] = {CTR_ING_FLEX_STATSt_BYTESf,
                                   CTR_EGR_FLEX_STATSt_BYTESf};
static bcmlrd_fid_t entry_fid[2] = {CTR_ING_FLEX_ENTRYt_CTR_ING_FLEX_ENTRY_IDf,
                                   CTR_EGR_FLEX_ENTRYt_CTR_EGR_FLEX_ENTRY_IDf};
static bcmlrd_fid_t instance_fid[2] = {CTR_ING_FLEX_STATSt_CTR_FLEX_INSTANCEf,
                                      CTR_EGR_FLEX_STATSt_CTR_FLEX_INSTANCEf};
static bcmlrd_fid_t oper_state_fid[2] = {CTR_ING_FLEX_STATSt_OPERATIONAL_STATEf,
                                         CTR_EGR_FLEX_STATSt_OPERATIONAL_STATEf};

/**********************************************************************
* Private functions
 */
/*!
 * Parse IMM input array for entry_id and instance_id.
 */
static int
ctr_stats_parse_inputs(int unit,
                       bool ingress,
                       const bcmltd_fields_t *in,  /* array */
                       uint32_t *entry_id,
                       uint32_t *instance_id)
{
    int select;
    uint32_t i, t;
    bcmltd_field_t *data_field;
    size_t num=0;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    select  = ingress ? 0 : 1;

    if (in == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get all the key fields */
    for (i = 0; i < in->count; i++) {
        data_field = in->field[i];
        if (data_field->id == entry_fid[select]) {
            *entry_id = data_field->data;
            num++;
        }
        else if (data_field->id == instance_fid[select]) {
            *instance_id = data_field->data;
            num++;
        }
    }

    for (t=0; t<LT_INFO_NUM_TABLES; t++) {
        if (!sal_strcmp("CTR_ING_FLEX_STATS", ctrl->ctr_flex_field_info[t].name)) break;
        if (!sal_strcmp("CTR_EGR_FLEX_STATS", ctrl->ctr_flex_field_info[t].name)) break;
    }
    if (t == LT_INFO_NUM_TABLES) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "table not found in LT INFO control list.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (num < ctrl->ctr_flex_field_info[t].num_key_fields) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "incomplete key fields from imm.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Read an entry_id from the LT ENTRY in-memory table.
 */
static int
flex_ctr_stats_get_entry_data(int unit,
                              bool ingress,
                              uint32_t entry_id,
                              ctr_flex_entry_data_t *entry_data)
{
    SHR_FUNC_ENTER(unit);

    /* Validate ENTRY_ID and retrieve pool_id and base_index */
    SHR_IF_ERR_VERBOSE_EXIT
        (lookup_flex_entry_table (unit,
                                  ingress,
                                  entry_id,
                                  entry_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Retrieve packet and byte counts given entry_id and instance_id.
 * Packet and Byte counts are accumulated from all pipes in global mode.
 * In pipe-unique mode, the counts are read from the specified pipe_num.
 */
static int
flex_ctr_stats_read(int unit,
                    bool ingress,
                    ctr_flex_entry_data_t *entry_data,
                    uint32_t instance,
                    uint64_t *packet_count,
                    uint64_t *byte_count)
{
    int rv;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t drd_sid;
    bcmltd_sid_t rsp_ltid;
    uint32_t rsp_flags;
    uint32_t rsp_entry_wsize;
    uint32_t entry_id;
    uint32_t pkt_val;
    uint64_t byte_val;
    uint32_t base_index=0;
    uint32_t pool_id=0;
    uint32_t offset;
    uint32_t entry_sz;
    uint32_t *pt_entry_buff = NULL;
    uint32_t pipe_map;
    uint8_t pipe=0;
    bool pipe_unique;
    int select;
    uint64_t packet_counter=0;
    uint64_t byte_counter=0;

    SHR_FUNC_ENTER(unit);

    select = ingress ? 0 : 1;
    pool_id = entry_data->pool_id;
    base_index = entry_data->base_index;
    pipe_unique = entry_data->pipe_unique;
    entry_id = entry_data->entry_id;

    rv = get_counter_sid(unit,
                         ingress,
                         pool_id,
                         &drd_sid);

    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    entry_sz = bcmdrd_pt_entry_wsize(unit, drd_sid);

    /* Set size to 5 for now - to be removed after BCMLRD API ready*/
    entry_sz = 5;
    entry_sz *= 4;
    SHR_ALLOC(pt_entry_buff, entry_sz, CTR_FLEX_STATS);
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);
    sal_memset(pt_entry_buff, 0, entry_sz);

    SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));

    /* Instance value is offset value */
    offset = instance;

    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, drd_sid);

    /* To be removed after bcmlrd API ready */
    /* CCI NORMALIZED CTR return to be 5 words */
    rsp_entry_wsize = 5;

    /* Initialize physical table index */
    pt_dyn_info.index = base_index + offset;

    /* read specified pipe only */
    if (pipe_unique) {
       pipe_map &= 1 << entry_data->pipe_num;
    }

    while (pipe_map) {
        if (pipe_map & 1) {
            pt_dyn_info.tbl_inst = pipe;
            /* Read counter table. */
             SHR_IF_ERR_EXIT
                (bcmptm_ltm_ireq_read(unit,
                                      0,
                                      drd_sid,
                                      &pt_dyn_info,
                                      NULL,
                                      rsp_entry_wsize,
                                      req_ltid[select],
                                      pt_entry_buff,
                                      &rsp_ltid,
                                      &rsp_flags));

             pkt_val = (uint64_t)*pt_entry_buff | (uint64_t)*(pt_entry_buff + 1) << 32;
             byte_val = (uint64_t)*(pt_entry_buff+2) | (uint64_t)*(pt_entry_buff + 3) << 32;

            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "entry_id %d instance %d pipe %d "
                    "pkt_val = %d byte_val = %lu\n"),
                    (uint32_t) entry_id, (uint32_t) instance, pipe,
                    pkt_val, (unsigned long int) byte_val));

             packet_counter += pkt_val;
             byte_counter += byte_val;
        }
        pipe++;
        pipe_map >>= 1;
    }

    *packet_count = packet_counter;
    *byte_count = byte_counter;

exit:
    if (pt_entry_buff) {
        SHR_FREE(pt_entry_buff);
    }
    SHR_FUNC_EXIT();
}

/*!
 * Write/Update packet and/or byte counts for a given entry_id, instace_id.
 * This routine is used for both inserts and updates.
 * Packet and Byte counts are set on all pipes.
 */
static int
flex_ctr_stats_write (int unit,
                      bool ingress,
                      uint64_t entry_id,
                      uint32_t instance,
                      ctr_flex_entry_data_t *entry_data,
                      uint32_t *new_packet_counter,
                      uint32_t *new_byte_counter,
                      bool update_pkt_cnt,
                      bool update_byte_cnt)
{
    int rv;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t *pt_entry_buff = NULL;
    bcmdrd_sid_t drd_sid;
    bcmltd_sid_t rsp_ltid;
    uint32_t pipe_map;
    uint32_t rsp_flags;
    uint64_t packet_count=0;
    uint64_t byte_count=0;
    uint32_t base_index;
    uint32_t offset;
    uint32_t entry_sz;
    uint32_t pool_id;
    uint8_t pipe = 0;
    bool pipe_unique;
    int select;

    SHR_FUNC_ENTER(unit);

    select  = ingress ? 0 : 1;

    pool_id = entry_data->pool_id;
    base_index = entry_data->base_index;
    pipe_unique = entry_data->pipe_unique;

    rv = get_counter_sid(unit,
                         ingress,
                         pool_id,
                         &drd_sid);

    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    entry_sz = bcmdrd_pt_entry_wsize(unit, drd_sid);

    entry_sz = 5;
    entry_sz *= 4;

    SHR_ALLOC(pt_entry_buff, entry_sz, CTR_FLEX_STATS);
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);
    sal_memset(pt_entry_buff, 0, entry_sz);

    SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));

    /* Instance value is offset value */
    offset = instance;

    /* Initialize physical table index */
    pt_dyn_info.index = base_index + offset;

    /* read specified pipe only */
    if (pipe_unique) {
       pipe_map &= 1 << entry_data->pipe_num;
    }
    else {
        /* in global mode; read/modify/write each pipe one at at time */
        entry_data->pipe_unique = 1;
    }

    while (pipe_map) {
        if (pipe_map & 1) {
            pt_dyn_info.tbl_inst = pipe;

            /* Read un-modified pkt/byte values for single field updates */
            if (update_pkt_cnt ^ update_byte_cnt) {
                SHR_IF_ERR_EXIT
                    (flex_ctr_stats_read(unit,
                                         ingress,
                                         entry_data,
                                         instance,
                                         &packet_count,
                                         &byte_count));

                 if (!update_pkt_cnt) {
                     CTR_FLEX_UINT64_TO_UINT32(new_packet_counter,
                                               packet_count);
                 }

                 if (!update_byte_cnt) {
                     CTR_FLEX_UINT64_TO_UINT32(new_byte_counter,
                                               byte_count);
                 }
            }

            pt_entry_buff[0] = new_packet_counter[0];
            pt_entry_buff[1] = new_packet_counter[1];
            pt_entry_buff[2] = new_byte_counter[0];
            pt_entry_buff[3] = new_byte_counter[1];

            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "entry_id %d, instance %d, pipe %d "
                    "new_packet_counter = 0x%x_%08x, "
                    "new_byte_counter = 0x%x_%08x\n"),
                    (uint32_t) entry_id, (uint32_t) instance, pipe,
                    new_packet_counter[1], new_packet_counter[0],
                    new_byte_counter[1], new_byte_counter[0]));

            /* Write back counter table. */
            SHR_IF_ERR_EXIT
                (bcmptm_ltm_ireq_write(unit,
                                       0,
                                       drd_sid,
                                       &pt_dyn_info,
                                       NULL,
                                       pt_entry_buff,
                                       req_ltid[select],
                                       &rsp_ltid,
                                       &rsp_flags));
        }
        pipe++;
        pipe_map >>= 1;
    }

exit:
    if (pt_entry_buff) {
        SHR_FREE(pt_entry_buff);
    }
    SHR_FUNC_EXIT();
}

/*!
 * Insert, update, delete helper routine.
 */
static int
bcmptm_flex_ctr_stats_lt_stage_cb(int unit,
                                  bool ingress,
                                  bcmimm_entry_event_t event_reason,
                                  const bcmltd_field_t *key_fields,
                                  const bcmltd_field_t *data_fields,
                                  bcmltd_fields_t *output_fields)
{
    uint32_t entry_id = 0;
    uint32_t instance_id = 0;
    ctr_flex_entry_data_t entry_data = {0};
    uint32_t new_packet_counter[2] = {0, 0};
    uint32_t new_byte_counter[2] = {0, 0};
    bool update_pkt_cnt = false;
    bool update_byte_cnt = false;
    uint32_t oper_status_valid = 0;
    uint32_t oper_status = 0;
    int select;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    select  = ingress ? 0 : 1;

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Parse the key fields */
    while (key_fields) {
        if (key_fields->id == entry_fid[select]) {
            entry_id = key_fields->data;
        }
        else if (key_fields->id == instance_fid[select]) {
            instance_id = key_fields->data;
        }
        key_fields = key_fields->next;
    }

    /* Parse the data fields */
    while (data_fields) {
        if (data_fields->id == pkts_fid[select]) {
            CTR_FLEX_UINT64_TO_UINT32(new_packet_counter, data_fields->data);
            update_pkt_cnt = true;
        }
        else if (data_fields->id == bytes_fid[select]) {
            CTR_FLEX_UINT64_TO_UINT32(new_byte_counter, data_fields->data);
            update_byte_cnt = true;
        }
        data_fields = data_fields->next;
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit, req_ltid[select],
                                      oper_state_fid[select],
                                      "ACTIVE",
                                      &oper_status_valid));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit, req_ltid[select],
                                      oper_state_fid[select],
                                      "INACTIVE",
                                      &oper_status));

    /* lookup the IMM with this entry_id */
    SHR_IF_ERR_VERBOSE_EXIT
        (flex_ctr_stats_get_entry_data(unit,
                                       ingress,
                                       entry_id,
                                       &entry_data));

    if (event_reason == BCMIMM_ENTRY_INSERT ||
        event_reason == BCMIMM_ENTRY_UPDATE) {

        if (entry_data.op_state ==
            ctrl->ctr_flex_enum.awaiting_counter_resource) {
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "entry is waitlisted\n")));
            SHR_EXIT();
        }

        if (event_reason == BCMIMM_ENTRY_INSERT) {
            update_pkt_cnt = true;
            update_byte_cnt = true;
        }

        SHR_IF_ERR_EXIT
            (flex_ctr_stats_write (unit,
                                   ingress,
                                   entry_id,
                                   instance_id,
                                   &entry_data,
                                   new_packet_counter,
                                   new_byte_counter,
                                   update_pkt_cnt,
                                   update_byte_cnt));
    }

    oper_status = oper_status_valid;

exit:
    if (output_fields) {
        output_fields->count = 1;
        output_fields->field[0]->id = oper_state_fid[select];
        output_fields->field[0]->data = oper_status;
    }
    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        return SHR_E_NONE;
    }
    SHR_FUNC_EXIT();
}

/*!
 * Lookup helper routine.
 */
static int
bcmptm_flex_ctr_stats_lookup_cb (int unit,
                                 bool ingress,
                                 bcmimm_lookup_type_t lkup_type,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out)
{
    int select;
    uint32_t instance=0, i;
    ctr_flex_entry_data_t entry_data = {0};
    const bcmltd_fields_t *key_fields;
    uint32_t oper_status_valid = 0;
    uint32_t oper_status = 0;
    uint64_t byte_count=0;
    uint64_t packet_count=0;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    select  = ingress ? 0 : 1;

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit, req_ltid[select],
                                      oper_state_fid[select],
                                      "ACTIVE", &oper_status_valid));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit, req_ltid[select],
                                      oper_state_fid[select],
                                      "INACTIVE",
                                      &oper_status));

    SHR_IF_ERR_EXIT
        (ctr_stats_parse_inputs(unit,
                                ingress,
                                key_fields,
                                &entry_data.entry_id,
                                &instance));

    /* lookup the IMM with this entry_id */
    SHR_IF_ERR_VERBOSE_EXIT
        (flex_ctr_stats_get_entry_data(unit,
                                       ingress,
                                       entry_data.entry_id,
                                       &entry_data));

    if (entry_data.op_state ==
        ctrl->ctr_flex_enum.awaiting_counter_resource) {
        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "entry is waitlisted\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (flex_ctr_stats_read(unit,
                             ingress,
                             &entry_data,
                             instance,
                             &packet_count,
                             &byte_count));

    oper_status = oper_status_valid;

exit:
    if (out) {
        for (i=0; i < out->count; i++) {
            /* Set PACKET_COUNT field value. */
            if (out->field[i]->id == pkts_fid[select]) {
                out->field[i]->data = packet_count;
            }
            /* Set BYTE_COUNT field value. */
            else if (out->field[i]->id == bytes_fid[select]) {
                out->field[i]->data = byte_count;
            }
            /* Set OPERATIONAL_STATUS. */
            else if (out->field[i]->id == oper_state_fid[select]) {
                out->field[i]->data = oper_status;
            }
        }
    }
    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        return SHR_E_NONE;
    }

    SHR_FUNC_EXIT();
}

/****************************************************************
* Public functions
 */
/*!
 * In-memory ING_STATS lookup callback helper function.
 */
int
bcmptm_flex_ing_ctr_stats_lt_lookup_cb (int unit,
                                        bcmltd_sid_t sid,
                                        uint32_t trans_id,
                                        void *context,
                                        bcmimm_lookup_type_t lkup_type,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out)
{
    bool ingress = true;
    int t;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    COMPILER_REFERENCE(trans_id);

    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "in ing lookup cth\n")));

    for (t=0; t<LT_INFO_NUM_TABLES; t++) {
        if (!sal_strcmp("CTR_ING_FLEX_STATS",
                        ctrl->ctr_flex_field_info[t].name))
            break;
    }

    if (out) {
        if (t == LT_INFO_NUM_TABLES) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        else if (out->count < ctrl->ctr_flex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "out supplied to lookup is too"
                                  "small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }
    else {
        LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "out not supplied to lookup.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT(
        bcmptm_flex_ctr_stats_lookup_cb (unit,
                                         ingress,
                                         lkup_type,
                                         in,
                                         out));

exit:
    if (SHR_FAILURE(_func_rv)) {
        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "lookup failed")));
    }
    SHR_FUNC_EXIT();
}

/*!
 * In-memory EGR_STATS lookup callback helper function.
 */
int
bcmptm_flex_egr_ctr_stats_lt_lookup_cb (int unit,
                                     bcmltd_sid_t sid,
                                     uint32_t trans_id,
                                     void *context,
                                     bcmimm_lookup_type_t lkup_type,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out)
{
    bool ingress = false;
    int t;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    COMPILER_REFERENCE(trans_id);

    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "in egr lookup cth\n")));

    for (t=0; t<LT_INFO_NUM_TABLES; t++) {
        if (!sal_strcmp("CTR_EGR_FLEX_STATS",
                        ctrl->ctr_flex_field_info[t].name))
            break;
    }

    if (out) {
        if (t == LT_INFO_NUM_TABLES) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        else if (out->count < ctrl->ctr_flex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "out supplied to lookup is too"
                                  "small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }
    else {
        LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "out not supplied to lookup.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT(
        bcmptm_flex_ctr_stats_lookup_cb (unit,
                                         ingress,
                                         lkup_type,
                                         in,
                                         out));

exit:
    if (SHR_FAILURE(_func_rv)) {
        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "lookup failed")));
    }
    SHR_FUNC_EXIT();
}
/*!
 * In-memory ING_STATS insert/update callback helper function.
 */
int
bcmptm_flex_ing_ctr_stats_lt_stage_cb(int unit,
                                      bcmltd_sid_t sid,
                                      uint32_t trans_id,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key_fields,
                                      const bcmltd_field_t *data_fields,
                                      void *context,
                                      bcmltd_fields_t *output_fields)
{
    bool ingress = true;
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    SHR_IF_ERR_EXIT(
        bcmptm_flex_ctr_stats_lt_stage_cb(unit,
                                          ingress,
                                          event_reason,
                                          key_fields,
                                          data_fields,
                                          output_fields));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory EGR_STATS insert/update callback helper function.
 */
int
bcmptm_flex_egr_ctr_stats_lt_stage_cb(int unit,
                                      bcmltd_sid_t sid,
                                      uint32_t trans_id,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key_fields,
                                      const bcmltd_field_t *data_fields,
                                      void *context,
                                      bcmltd_fields_t *output_fields)
{
    bool ingress = false;
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    SHR_IF_ERR_EXIT(
        bcmptm_flex_ctr_stats_lt_stage_cb(unit,
                                          ingress,
                                          event_reason,
                                          key_fields,
                                          data_fields,
                                          output_fields));
exit:
    SHR_FUNC_EXIT();
}
