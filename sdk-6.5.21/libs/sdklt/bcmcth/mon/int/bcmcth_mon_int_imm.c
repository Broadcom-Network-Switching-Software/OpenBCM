/*! \file bcmcth_mon_int_imm.c
 *
 * IMM handler for MON_INBAND_TELEMETRY_XXX LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcth/bcmcth_mon_int_drv.h>
#include <bcmcth/bcmcth_mon_util.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*******************************************************************************
 * Local definitions
 */
static int int_ipfix_export_replay(int unit);

static int
int_fields_free(int unit, int num_fields, bcmltd_fields_t *fields)
{
    int idx;

    SHR_FUNC_ENTER(unit);

    if (num_fields == 0) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    if (fields->field != NULL) {
        for (idx = 0; idx < num_fields; idx++) {
            if (fields->field[idx] != NULL) {
                shr_fmm_free(fields->field[idx]);
            }
        }
        SHR_FREE(fields->field);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
int_fields_alloc(int unit, size_t num_fields, bcmltd_fields_t *fields)
{
    size_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    /* Allocate fields buffers */
    SHR_ALLOC(fields->field,
              sizeof(bcmltd_field_t *) * num_fields,
              "bcmcthMonIntFields");

    SHR_NULL_CHECK(fields->field, SHR_E_MEMORY);

    sal_memset(fields->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    fields->count = num_fields;

    for (idx = 0; idx < num_fields; idx++) {
        fields->field[idx] = shr_fmm_alloc();
        SHR_NULL_CHECK(fields->field[idx], SHR_E_MEMORY);
        sal_memset(fields->field[idx], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        int_fields_free(unit, num_fields, fields);
    }
    SHR_FUNC_EXIT();
}

static int
int_num_fields_get(int unit, const char *table_name, size_t *num_fields)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    int sid;
    size_t idx, num_fids;

    SHR_FUNC_ENTER(unit);

    sid = bcmlrd_table_name_to_idx(unit, table_name);
    if (sid < 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Get the number of unique fields. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_field_count_get(unit, sid, &num_fids));


    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * num_fids,
              "bcmcthMonIntFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlrd_table_fields_def_get(unit, table_name,
                                    num_fids, fields_info, &num_fids));

    for (idx = 0, *num_fields = 0; idx < num_fids; idx++) {
        (*num_fields) += fields_info[idx].elements;
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handler
 */
static int
int_control_lt_field_get(int unit,
                         const bcmltd_field_t *gen_field,
                         bcmcth_mon_int_control_entry_t *entry)
{
    uint32_t fid;
    uint64_t fval;
    int rv;

    SHR_FUNC_ENTER(unit);

    fid = gen_field->id;
    fval = gen_field->data;

    switch (fid) {
        case MON_INBAND_TELEMETRY_CONTROLt_INBAND_TELEMETRYf:
            entry->inband_telemetry = fval;
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_MAX_EXPORT_PKT_LENGTHf:
            entry->max_export_pkt_length = fval;
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_MAX_EXPORT_PKT_LENGTH_OPERf:
            entry->oper.max_export_pkt_length = fval;
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_MAX_RX_PKT_LENGTHf:
            entry->max_rx_pkt_length = fval;
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_MAX_RX_PKT_LENGTH_OPERf:
            entry->oper.max_rx_pkt_length = fval;
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_DEVICE_IDENTIFIERf:
            entry->device_id = fval;
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_EXPORTf:
            entry->export = fval;
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_RX_PKT_EXPORT_MAX_LENGTHf:
            entry->rx_pkt_export_max_length = fval;
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_COLLECTOR_TYPEf:
            entry->collector_type = fval;
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_MON_COLLECTOR_IPV4_IDf:
            entry->collector_ipv4_id = fval;
            rv = bcmcth_mon_collector_ipv4_entry_get(unit,
                                                 entry->collector_ipv4_id,
                                                 &(entry->collector_ipv4));
            if (rv == SHR_E_NONE) {
                entry->collector_ipv4_found = true;
            }
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_MON_COLLECTOR_IPV6_IDf:
            entry->collector_ipv6_id = fval;
            rv = bcmcth_mon_collector_ipv6_entry_get(unit,
                                                     entry->collector_ipv6_id,
                                                     &(entry->collector_ipv6));
            if (rv == SHR_E_NONE) {
                entry->collector_ipv6_found = true;
            }
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_MON_EXPORT_PROFILE_IDf:
            entry->export_profile_id = fval;
            rv = bcmcth_mon_export_profile_entry_get(unit,
                                                     entry->export_profile_id,
                                                     &(entry->export_profile));
            if (rv == SHR_E_NONE) {
                entry->export_profile_found = true;
            }
            break;

        case MON_INBAND_TELEMETRY_CONTROLt_OPERATIONAL_STATEf:
            /* Ignore dynamically calculated fields. */
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_BITSET(entry->fbmp, fid);

 exit:
    SHR_FUNC_EXIT();
}

static int
int_control_lt_fields_ll_parse(int unit,
                               const bcmltd_field_t *data,
                               bcmcth_mon_int_control_entry_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (int_control_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
int_control_lt_fields_arr_parse(int unit,
                                bcmltd_field_t **data,
                                int count,
                                bcmcth_mon_int_control_entry_t *entry)
{
    const bcmltd_field_t *gen_field;
    int idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    for (idx = 0; idx < count; idx++) {
        gen_field = ((bcmltd_field_t **) data)[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (int_control_lt_field_get(unit, gen_field, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
int_control_entry_get(int unit, bcmcth_mon_int_control_entry_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    size_t num_fields;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (int_num_fields_get(unit, "MON_INBAND_TELEMETRY_CONTROL", &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT(int_fields_alloc(unit, num_fields, &out));

    in.count = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, MON_INBAND_TELEMETRY_CONTROLt, &in, &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (int_control_lt_fields_arr_parse(unit,
                                         out.field, out.count,
                                         entry));

exit:
    int_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

static int
int_control_lt_stage_out_fields_populate(int unit,
                                         bcmcth_mon_int_control_oper_fields_t *oper,
                                         bcmltd_fields_t *output_fields)
{
    int count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oper, SHR_E_PARAM);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    /* Update the operational fields. */
    output_fields->field[count]->id =
        MON_INBAND_TELEMETRY_CONTROLt_MAX_EXPORT_PKT_LENGTH_OPERf;
    output_fields->field[count]->data = oper->max_export_pkt_length;
    count++;

    output_fields->field[count]->id =
        MON_INBAND_TELEMETRY_CONTROLt_MAX_RX_PKT_LENGTH_OPERf;
    output_fields->field[count]->data = oper->max_rx_pkt_length;
    count++;

    output_fields->count = count;

exit:
    SHR_FUNC_EXIT();
}

static int
int_control_stage(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    bcmcth_mon_int_control_entry_t cur_entry, new_entry;
    bcmcth_mon_int_control_oper_fields_t oper;
    bool replay = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (int_control_lt_fields_ll_parse(unit, data, &new_entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_control_insert(unit, trans_id,
                                               &new_entry,
                                               &oper, &replay));
            break;

        case BCMIMM_ENTRY_UPDATE:
            /* Get the current state of the IMM entry. */
            SHR_IF_ERR_VERBOSE_EXIT(int_control_entry_get(unit, &cur_entry));


            /* Pass the old and new entry to the driver */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_control_update(unit, trans_id,
                                               &cur_entry, &new_entry,
                                               &oper, &replay));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_control_delete(unit));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        SHR_IF_ERR_VERBOSE_EXIT
            (int_control_lt_stage_out_fields_populate(unit,
                                                      &oper,
                                                      output_fields));
    }

    if (replay == true) {
        /* Replay all the configurations. */
        SHR_IF_ERR_VERBOSE_EXIT(int_ipfix_export_replay(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
int_control_lookup(int unit,
                   bcmltd_sid_t sid,
                   uint32_t trans_id,
                   void *context,
                   bcmimm_lookup_type_t lkup_type,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *output_fields)
{
    bcmltd_common_mon_inband_telemetry_control_state_t_t oper_state;
    uint32_t fid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    if (lkup_type == BCMIMM_LOOKUP) {
        /* Get the oper state and overwrite the IMM entry. */
        oper_state = bcmcth_mon_int_control_oper_state_get(unit);

        fid = MON_INBAND_TELEMETRY_CONTROLt_OPERATIONAL_STATEf;
        output_fields->field[fid]->data = oper_state;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief MON_INBAND_TELEMETRY_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_INBAND_TELEMETRY_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t int_control_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = int_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = int_control_lookup
};

static int
int_ipfix_export_lt_field_get(int unit,
                              const bcmltd_field_t *gen_field,
                              bcmcth_mon_int_ipfix_export_entry_t *entry)
{
    uint32_t fid;
    uint32_t idx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = gen_field->id;
    idx = gen_field->idx;
    fval = gen_field->data;

    switch (fid) {
        case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_SET_IDf:
            entry->set_id = fval;
            SHR_BITSET(entry->fbmp, fid);
            break;

        case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_NUM_EXPORT_ELEMENTSf:
            entry->num_elements = fval;
            SHR_BITSET(entry->fbmp, fid);
            break;

        case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_EXPORT_ELEMENTf:
            entry->elements[idx].element = fval;
            SHR_BITSET(entry->elements[idx].fbmp, fid);
            break;

        case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_ENTERPRISEf:
            entry->elements[idx].enterprise = fval;
            SHR_BITSET(entry->elements[idx].fbmp, fid);
            break;

        case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_INFORMATION_ELEMENT_IDENTIFIERf:
            entry->elements[idx].info_element_id = fval;
            SHR_BITSET(entry->elements[idx].fbmp, fid);
            break;

        case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_TX_INTERVALf:
            entry->tx_interval = fval;
            SHR_BITSET(entry->fbmp, fid);
            break;

        case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_INITIAL_BURSTf:
            entry->initial_burst = fval;
            SHR_BITSET(entry->fbmp, fid);
            break;

        case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_OPERATIONAL_STATEf:
            /* Ignore dynamic fields. */
            break;


        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }


exit:
    SHR_FUNC_EXIT();
}

static int
int_ipfix_export_lt_fields_ll_parse(int unit,
                                    const bcmltd_field_t *data,
                                    bcmcth_mon_int_ipfix_export_entry_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (int_ipfix_export_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
int_ipfix_export_lt_fields_arr_parse(int unit,
                                     bcmltd_field_t **data,
                                     int count,
                                     bcmcth_mon_int_ipfix_export_entry_t *entry)
{
    const bcmltd_field_t *gen_field;
    int idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    for (idx = 0; idx < count; idx++) {
        gen_field = ((bcmltd_field_t **) data)[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (int_ipfix_export_lt_field_get(unit, gen_field, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
int_ipfix_export_entry_get(int unit, bcmcth_mon_int_ipfix_export_entry_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    size_t num_fields = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (int_num_fields_get(unit, "MON_INBAND_TELEMETRY_IPFIX_EXPORT",
                            &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT(int_fields_alloc(unit, 1, &in));
    SHR_IF_ERR_VERBOSE_EXIT(int_fields_alloc(unit, num_fields, &out));

    in.count = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTt, &in, &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ipfix_export_lt_fields_arr_parse(unit, out.field, out.count,
                                              entry));

exit:
    int_fields_free(unit, 1, &in);
    int_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

static int
int_ipfix_export_stage(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       bcmimm_entry_event_t event,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context,
                       bcmltd_fields_t *output_fields)
{
    bcmcth_mon_int_ipfix_export_entry_t cur_entry, new_entry;
    bcmltd_common_mon_inband_telemetry_ipfix_export_state_t_t oper_state;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (int_ipfix_export_lt_fields_ll_parse(unit, data, &new_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_ipfix_export_insert(unit, &new_entry,
                                                    &oper_state));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (int_ipfix_export_lt_fields_ll_parse(unit, data, &new_entry));

            /* Get the current state of the IMM entry. */
            SHR_IF_ERR_VERBOSE_EXIT
                (int_ipfix_export_entry_get(unit, &cur_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_ipfix_export_update(unit, &cur_entry,
                                                    &new_entry, &oper_state));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_int_ipfix_export_delete(unit));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        output_fields->count = 0;

        output_fields->field[output_fields->count]->id =
            MON_INBAND_TELEMETRY_IPFIX_EXPORTt_OPERATIONAL_STATEf;
        output_fields->field[output_fields->count]->data = oper_state;
        output_fields->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
int_ipfix_export_replay(int unit)
{
    bcmcth_mon_int_ipfix_export_entry_t entry;
    bcmltd_common_mon_inband_telemetry_ipfix_export_state_t_t oper_state;
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    size_t num_fields = 0;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (int_num_fields_get(unit, "MON_INBAND_TELEMETRY_IPFIX_EXPORT",
                            &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT(int_fields_alloc(unit, 1, &in));
    SHR_IF_ERR_VERBOSE_EXIT(int_fields_alloc(unit, num_fields, &out));

    in.count = 0;
    rv = bcmimm_entry_lookup(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTt, &in, &out);
    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }


    SHR_IF_ERR_VERBOSE_EXIT
        (int_ipfix_export_lt_fields_arr_parse(unit,
                                              out.field,
                                              out.count, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_int_ipfix_export_replay(unit, &entry,
                                            &oper_state));

    /* Update the oper state. */
    in.field[0]->id = MON_INBAND_TELEMETRY_IPFIX_EXPORTt_OPERATIONAL_STATEf;
    in.field[0]->data = oper_state;
    in.count = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_update(unit, false,
                             MON_INBAND_TELEMETRY_IPFIX_EXPORTt, &in));


exit:
    int_fields_free(unit, 1, &in);
    int_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief MON_INBAND_TELEMETRY_IPFIX_EXPORT In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_INBAND_TELEMETRY_IPFIX_EXPORT logical table entry commit stages.
 */
static bcmimm_lt_cb_t int_ipfix_export_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = int_ipfix_export_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

static int
int_stats_lt_fields_parse(int unit,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmcth_mon_int_stats_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_CNTf:
                entry->rx_pkt_cnt = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_TX_PKT_CNTf:
                entry->tx_pkt_cnt = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_TX_RECORD_CNTf:
                entry->tx_record_cnt = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_EXPORT_DISABLED_DISCARDf:
                entry->rx_pkt_export_disabled_discard = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_NO_EXPORT_CONFIG_DISCARDf:
                entry->rx_pkt_no_export_config_discard = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_NO_IPFIX_CONFIG_DISCARDf:
                entry->rx_pkt_no_ipfix_config_discard = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_CURRENT_LENGTH_EXCEED_DISCARDf:
                entry->rx_pkt_current_length_exceed_discard = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_LENGTH_EXCEED_MAX_DISCARDf:
                entry->rx_pkt_length_exceed_max_discard = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_PARSE_ERROR_DISCARDf:
                entry->rx_pkt_parse_error_discard = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_UNKNOWN_NAMESPACE_DISCARDf:
                entry->rx_pkt_unknown_namespace_discard = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_EXCESS_RATE_DISCARDf:
                entry->rx_pkt_excess_rate_discard = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_RX_PKT_INCOMPLETE_METADATA_DISCARDf:
                entry->rx_pkt_incomplete_metadata_discard = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_INBAND_TELEMETRY_STATSt_TX_PKT_FAILURE_CNTf:
                entry->tx_pkt_failure_cnt = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
int_stats_lt_lookup_out_fields_populate(int unit,
                                        bcmcth_mon_int_stats_t *entry,
                                        bcmltd_fields_t *output_fields)
{
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_CNTf;
    output_fields->field[fid]->data = entry->rx_pkt_cnt;

    fid = MON_INBAND_TELEMETRY_STATSt_TX_PKT_CNTf;
    output_fields->field[fid]->data = entry->tx_pkt_cnt;

    fid = MON_INBAND_TELEMETRY_STATSt_TX_RECORD_CNTf;
    output_fields->field[fid]->data = entry->tx_record_cnt;

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_EXPORT_DISABLED_DISCARDf;
    output_fields->field[fid]->data = entry->rx_pkt_export_disabled_discard;

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_NO_EXPORT_CONFIG_DISCARDf;
    output_fields->field[fid]->data = entry->rx_pkt_no_export_config_discard;

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_NO_IPFIX_CONFIG_DISCARDf;
    output_fields->field[fid]->data = entry->rx_pkt_no_ipfix_config_discard;

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_CURRENT_LENGTH_EXCEED_DISCARDf;
    output_fields->field[fid]->data = entry->rx_pkt_current_length_exceed_discard;

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_LENGTH_EXCEED_MAX_DISCARDf;
    output_fields->field[fid]->data = entry->rx_pkt_length_exceed_max_discard;

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_PARSE_ERROR_DISCARDf;
    output_fields->field[fid]->data = entry->rx_pkt_parse_error_discard;

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_UNKNOWN_NAMESPACE_DISCARDf;
    output_fields->field[fid]->data = entry->rx_pkt_unknown_namespace_discard;

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_EXCESS_RATE_DISCARDf;
    output_fields->field[fid]->data = entry->rx_pkt_excess_rate_discard;

    fid = MON_INBAND_TELEMETRY_STATSt_RX_PKT_INCOMPLETE_METADATA_DISCARDf;
    output_fields->field[fid]->data = entry->rx_pkt_incomplete_metadata_discard;

    fid = MON_INBAND_TELEMETRY_STATSt_TX_PKT_FAILURE_CNTf;
    output_fields->field[fid]->data = entry->tx_pkt_failure_cnt;

exit:
    SHR_FUNC_EXIT();
}

static int
int_stats_stage(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event,
                const bcmltd_field_t *key,
                const bcmltd_field_t *data,
                void *context,
                bcmltd_fields_t *output_fields)
{
    bcmcth_mon_int_stats_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (int_stats_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_int_stats_update(unit, &entry));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
int_stats_lookup(int unit,
                 bcmltd_sid_t sid,
                 uint32_t trans_id,
                 void *context,
                 bcmimm_lookup_type_t lkup_type,
                 const bcmltd_fields_t *in,
                 bcmltd_fields_t *output_fields)
{
    bcmcth_mon_int_stats_t entry;

    SHR_FUNC_ENTER(unit);

    if (lkup_type == BCMIMM_LOOKUP) {
        sal_memset(&entry, 0, sizeof(entry));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_int_stats_lookup(unit, &entry));

        SHR_IF_ERR_VERBOSE_EXIT
            (int_stats_lt_lookup_out_fields_populate(unit, &entry,
                                                     output_fields));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief MON_INBAND_TELEMETRY_STATS In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_INBAND_TELEMETRY_STATS logical table entry commit stages.
 */
static bcmimm_lt_cb_t int_stats_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = int_stats_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = int_stats_lookup
};

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_mon_int_imm_collector_ipv4_handler(int unit,
                                          bcmimm_entry_event_t event,
                                          bcmcth_mon_collector_ipv4_t *collector)
{
    bcmcth_mon_int_control_entry_t int_ctrl;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Get the INT control IMM entry. */
    rv = int_control_entry_get(unit, &int_ctrl);
    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_collector_ipv4_insert(unit,
                                                      &int_ctrl, collector));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_collector_ipv4_update(unit,
                                                      &int_ctrl, collector));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_collector_ipv4_delete(unit,
                                                      &int_ctrl,
                                                      collector->collector_ipv4_id));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_imm_collector_ipv6_handler(int unit,
                                          bcmimm_entry_event_t event,
                                          bcmcth_mon_collector_ipv6_t *collector)
{
    bcmcth_mon_int_control_entry_t int_ctrl;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Get the INT control IMM entry. */
    rv = int_control_entry_get(unit, &int_ctrl);
    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_collector_ipv6_insert(unit,
                                                      &int_ctrl, collector));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_collector_ipv6_update(unit,
                                                      &int_ctrl, collector));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_collector_ipv6_delete(unit,
                                                      &int_ctrl,
                                                      collector->collector_ipv6_id));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_imm_export_profile_handler(int unit,
                                          bcmimm_entry_event_t event,
                                          bcmcth_mon_export_profile_t *export_profile)
{
    bcmcth_mon_int_control_entry_t int_ctrl;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Get the INT control IMM entry. */
    rv = int_control_entry_get(unit, &int_ctrl);
    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_export_profile_insert(unit,
                                                      &int_ctrl, export_profile));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_export_profile_update(unit,
                                                      &int_ctrl, export_profile));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_int_export_profile_delete(unit,
                                                      &int_ctrl,
                                                      export_profile->export_profile_id));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_imm_register(int unit, bool warm)
{
    bcmltd_fields_t stats_fields = {0};
    size_t num_fields = 0;

    SHR_FUNC_ENTER(unit);


    /*
     * Callback registration for INT LTs
     */
    if (bcmcth_mon_imm_mapped(unit, MON_INBAND_TELEMETRY_CONTROLt)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_lt_event_reg(unit, MON_INBAND_TELEMETRY_CONTROLt,
                                 &int_control_hdl, NULL));
    }
    if (bcmcth_mon_imm_mapped(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTt)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_lt_event_reg(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTt,
                                 &int_ipfix_export_hdl, NULL));
    }
    if (bcmcth_mon_imm_mapped(unit, MON_INBAND_TELEMETRY_STATSt)) {
        size_t idx;

        if (warm == false) {
            SHR_IF_ERR_VERBOSE_EXIT
                (int_num_fields_get(unit, "MON_INBAND_TELEMETRY_STATS",
                                    &num_fields));

            /* Insert a dummy entry to make the lookups and updates work. */
            SHR_IF_ERR_VERBOSE_EXIT
                (int_fields_alloc(unit, num_fields, &stats_fields));

            /* Fill up zeros for all the stats */
            for (idx = 0; idx < num_fields; idx++) {
                stats_fields.field[idx]->id = idx;
                stats_fields.field[idx]->data = 0;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_entry_insert(unit, MON_INBAND_TELEMETRY_STATSt,
                                     &stats_fields));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_lt_event_reg(unit, MON_INBAND_TELEMETRY_STATSt,
                                 &int_stats_hdl, NULL));
    }

 exit:
    int_fields_free(unit, num_fields, &stats_fields);
    SHR_FUNC_EXIT();
}
