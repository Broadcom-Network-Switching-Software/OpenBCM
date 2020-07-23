/*! \file bcmcth_mon_int_imm.c
 *
 * IMM handler for MON_FLOWTRACKER_XXX LTs.
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
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcth/bcmcth_mon_flowtracker_int.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>
#include <bcmcth/bcmcth_mon_flowtracker_util.h>
#include <bcmcth/bcmcth_mon_collector.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * IMM event handler
 */
static int
bcmcth_mon_ft_control_lt_field_get(int unit,
                         const bcmltd_field_t *gen_field,
                         bcmcth_mon_ft_control_t *entry)
{
    uint32_t fid;
    uint32_t idx;
    uint64_t fval;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);

    fid = gen_field->id;
    idx = gen_field->idx;
    fval = gen_field->data;

    switch (fid) {
        case MON_FLOWTRACKER_CONTROLt_FLOWTRACKERf:
            entry->flowtracker = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_OBSERVATION_DOMAINf:
            entry->observation_domain = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_MAX_GROUPSf:
            entry->max_groups = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_MAX_EXPORT_LENGTHf:
            entry->max_export_length = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_MAX_FLOWSf:
            entry->max_flows[idx] = fval;
            SHR_BITSET(entry->fbmp_maxflows, idx);
            break;

        case MON_FLOWTRACKER_CONTROLt_PKT_BYTE_CTR_ING_EFLEX_ACTION_PROFILE_IDf:
            entry->ctr_ing_flex_action_profile_id = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_IDf:
            entry->db_ewide_hb_ctr_ing_flex_action_profile_id = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_IDf:
            entry->qd_ewide_hb_ctr_ing_flex_action_profile_id = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTIONf:
            entry->db_ewide_hb_ctr_ing_flex_grp_action = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTIONf:
            entry->qd_ewide_hb_ctr_ing_flex_grp_action = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_ETRAPf:
            entry->etrap = fval;
            ft_info->eleph_mode = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_SCAN_INTERVAL_USECSf:
            entry->scan_interval_usecs = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_FLOW_START_TIMESTAMP_ENABLEf:
            entry->flow_start_timestamp_enable = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_HOST_MEMf:
            entry->host_mem = fval;
            break;
        case MON_FLOWTRACKER_CONTROLt_HARDWARE_LEARNf:
            entry->hw_learn = fval;
            break;

        case MON_FLOWTRACKER_CONTROLt_MAX_GROUPS_OPERf:
        case MON_FLOWTRACKER_CONTROLt_MAX_EXPORT_LENGTH_OPERf:
        case MON_FLOWTRACKER_CONTROLt_MAX_FLOWS_OPERf:
        case MON_FLOWTRACKER_CONTROLt_PKT_BYTE_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPERf:
        case MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPERf:
        case MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPERf:
        case MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION_OPERf:
        case MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION_OPERf:
        case MON_FLOWTRACKER_CONTROLt_ETRAP_OPERf:
        case MON_FLOWTRACKER_CONTROLt_FLOW_START_TIMESTAMP_ENABLE_OPERf:
        case MON_FLOWTRACKER_CONTROLt_SCAN_INTERVAL_USECS_OPERf:
        case MON_FLOWTRACKER_CONTROLt_HOST_MEM_OPERf:
        case MON_FLOWTRACKER_CONTROLt_HARDWARE_LEARN_OPERf:
        case MON_FLOWTRACKER_CONTROLt_OPERATIONAL_STATEf:
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
bcmcth_mon_ft_control_lt_fields_arr_parse(int unit,
                                bcmltd_field_t **data,
                                int count,
                                bcmcth_mon_ft_control_t *entry)
{
    const bcmltd_field_t *gen_field;
    int idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    for (idx = 0; idx < count; idx++) {
        gen_field = ((bcmltd_field_t **) data)[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_control_lt_field_get(unit, gen_field, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_control_lt_fields_ll_parse(int unit,
                              const bcmltd_field_t *data,
                              bcmcth_mon_ft_control_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_control_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_control_entry_get(int unit, bcmcth_mon_ft_control_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    uint32_t field_count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_control_out_fields_count(unit, &field_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, field_count,
                          &out));

    in.count = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, MON_FLOWTRACKER_CONTROLt, &in, &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_control_lt_fields_arr_parse(unit,
                                         out.field, out.count,
                                         entry));

exit:
    bcmcth_mon_ft_util_fields_free(unit, field_count, &out);
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_control_lt_stage_out_fields_populate(
    int unit,
    bcmcth_mon_ft_control_oper_fields_t *oper,
    bcmltd_fields_t *output_fields)
{
    int count = 0,i = 0;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    SHR_FUNC_ENTER(unit);
    ft_drv = bcmcth_mon_ft_drv_get(unit);

    SHR_NULL_CHECK(ft_drv, SHR_E_INIT);
    SHR_NULL_CHECK(oper, SHR_E_PARAM);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    /* Update the operational fields. */
    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_MAX_GROUPS_OPERf;
    output_fields->field[count]->data = oper->max_groups;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_MAX_EXPORT_LENGTH_OPERf;
    output_fields->field[count]->data = oper->max_export_length;
    output_fields->field[count]->idx = 0;
    count++;

    for (i = 0; i < ft_drv->num_pipes; i++) {
        output_fields->field[count]->data = oper->max_flows[i];
        output_fields->field[count]->idx = i;
        output_fields->field[count]->id =
            MON_FLOWTRACKER_CONTROLt_MAX_FLOWS_OPERf;
        count++;
    }

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_PKT_BYTE_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPERf;
    output_fields->field[count]->data = oper->ctr_ing_flex_action_profile_id;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPERf;
    output_fields->field[count]->data =
        oper->db_ewide_hb_ctr_ing_flex_action_profile_id;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPERf;
    output_fields->field[count]->data =
        oper->qd_ewide_hb_ctr_ing_flex_action_profile_id;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION_OPERf;
    output_fields->field[count]->data =
        oper->db_ewide_hb_ctr_ing_flex_grp_action;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION_OPERf;
    output_fields->field[count]->data =
        oper->qd_ewide_hb_ctr_ing_flex_grp_action;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_ETRAP_OPERf;
    output_fields->field[count]->data = oper->etrap;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_SCAN_INTERVAL_USECS_OPERf;
    output_fields->field[count]->data = oper->scan_interval_usecs;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_FLOW_START_TIMESTAMP_ENABLE_OPERf;
    output_fields->field[count]->data = oper->flow_start_timestamp_enable;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_HOST_MEM_OPERf;
    output_fields->field[count]->data = oper->host_mem;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_HARDWARE_LEARN_OPERf;
    output_fields->field[count]->data = oper->hw_learn;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->field[count]->id =
        MON_FLOWTRACKER_CONTROLt_OPERATIONAL_STATEf;
    output_fields->field[count]->data = oper->operational_state;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->count = count;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_control_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mon_ft_control_t cur_entry, new_entry;
    bcmcth_mon_ft_control_oper_fields_t oper;

    SHR_FUNC_ENTER(unit);

    sal_memset(&oper, 0, sizeof(oper));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_control_lt_fields_ll_parse(unit, data, &new_entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_control_insert(unit, &new_entry, &oper));
            break;

        case BCMIMM_ENTRY_UPDATE:
            /* Get the current state of the IMM entry. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_control_entry_get(unit, &cur_entry));

            /* Pass the old and new entry to the driver */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_control_update(unit, &cur_entry,
                                              &new_entry, &oper));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_control_delete(unit));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_control_lt_stage_out_fields_populate(unit,
                                                      &oper,
                                                      output_fields));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief MON_FLOWTRACKER_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_FLOWTRACKER_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t ft_control_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bcmcth_mon_ft_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};


static int
bcmcth_mon_ft_stats_lt_lookup_out_fields_populate(int unit,
                                       bcmcth_mon_ft_stats_t *entry,
                                       bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint32_t idx = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    for (idx = 0; idx < output_fields->count; idx++) {
        fid = output_fields->field[idx]->id;

        switch (fid) {
            case MON_FLOWTRACKER_STATSt_LEARN_PKT_DISCARD_PARSE_ERRORf:
                output_fields->field[idx]->data = entry->learn_pkt_discard_parse_err_cnt;
                break;

            case MON_FLOWTRACKER_STATSt_LEARN_PKT_DISCARD_INVALID_GROUPf:
                output_fields->field[idx]->data =
                    entry->learn_pkt_discard_invalid_grp_cnt;
                break;

            case MON_FLOWTRACKER_STATSt_LEARN_PKT_DISCARD_FLOW_LIMIT_EXCEEDf:
                output_fields->field[idx]->data =
                    entry->learn_pkt_discard_flow_limit_exceed_cnt;
                break;

            case MON_FLOWTRACKER_STATSt_LEARN_PKT_DISCARD_PIPE_LIMIT_EXCEEDf:
                output_fields->field[idx]->data =
                    entry->learn_pkt_discard_pipe_limit_exceed_cnt;
                break;

            case MON_FLOWTRACKER_STATSt_LEARN_PKT_DISCARD_EM_FAILf:
                output_fields->field[idx]->data = entry->learn_pkt_discard_em_fail_cnt;
                break;

            case MON_FLOWTRACKER_STATSt_LEARN_PKT_DISCARD_DUPLICATEf:
                output_fields->field[idx]->data = entry->learn_pkt_discard_duplicate_cnt;
                break;

            case MON_FLOWTRACKER_STATSt_NUM_FLOWS_LEARNTf:
                output_fields->field[idx]->data = entry->num_flows_learnt;
                break;

            case MON_FLOWTRACKER_STATSt_NUM_FLOWS_EXPORTEDf:
                output_fields->field[idx]->data = entry->num_flow_exported;
                break;

            case MON_FLOWTRACKER_STATSt_NUM_PACKETS_EXPORTEDf:
                output_fields->field[idx]->data = entry->num_packets_exported;
                break;

            case MON_FLOWTRACKER_STATSt_NUM_FLOWS_AGEDf:
                output_fields->field[idx]->data = entry->num_flows_aged;
                break;

            case MON_FLOWTRACKER_STATSt_NUM_FLOWS_ELEPHANTf:
                output_fields->field[idx]->data = entry->num_elephant_flows;
                break;

            default :
                break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_stats_lookup(int unit,
                 bcmltd_sid_t sid,
                 uint32_t trans_id,
                 void *context,
                 bcmimm_lookup_type_t lkup_type,
                 const bcmltd_fields_t *in,
                 bcmltd_fields_t *output_fields)
{
    bcmcth_mon_ft_stats_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_uc_stats_lookup(unit, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_stats_lt_lookup_out_fields_populate(unit, &entry,
                                                output_fields));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_status_lt_key_fields_parse(
    int unit,
    const bcmltd_fields_t *key,
    bcmcth_mon_ft_group_status_t *entry)
{
    uint32_t i;
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(key);

    for (i = 0; i < key->count; i++) {
        if (key->field[i] == NULL) {
             SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (key->field[i]->id == MON_FLOWTRACKER_GROUP_STATUSt_MON_FLOWTRACKER_GROUP_IDf) {
            entry->group_id = key->field[i]->data;
        } else if (key->field[i]->id == MON_FLOWTRACKER_GROUP_STATUSt_FLOW_COUNTf) {
            entry->flow_count = key->field[i]->data;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_status_lookup(int unit,
                 bcmltd_sid_t sid,
                 uint32_t trans_id,
                 void *context,
                 bcmimm_lookup_type_t lkup_type,
                 const bcmltd_fields_t *in,
                 bcmltd_fields_t *output_fields)
{
    bcmcth_mon_ft_group_status_t entry;
    uint32_t idx = 0;
    uint32_t fid;
    const bcmltd_fields_t *key_fields = NULL;
    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? output_fields : in;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_group_status_lt_key_fields_parse(unit, key_fields, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_uc_group_status_lookup(unit, &entry));

    for (idx = 0; idx < output_fields->count; idx++) {
        fid = output_fields->field[idx]->id;

        switch (fid) {
            case MON_FLOWTRACKER_GROUP_STATUSt_FLOW_COUNTf:
                output_fields->field[idx]->data = entry.flow_count;
                break;

            default :
                break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_elephant_profile_lt_field_get(int unit,
        const bcmltd_field_t *gen_field,
        bcmcth_mon_flowtracker_elephant_profile_t *entry)
{
    uint32_t fid, idx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = gen_field->id;
    fval = gen_field->data;
    idx = gen_field->idx;

    switch (fid) {
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_MON_FLOWTRACKER_ELEPHANT_PROFILE_IDf:
            entry->mon_flowtracker_elephant_profile_id = fval;
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_NUM_PROMOTION_FILTERSf:
            entry->num_promotion_filters = fval;
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_MONITOR_INTERVAL_USECSf:
            entry->promotion_filters[idx].monitor_interval_usecs = fval;
            SHR_BITSET(entry->fbmp_pro_filters, idx);
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_RATE_HIGH_THRESHOLD_KBITS_SECf:
            entry->promotion_filters[idx].rate_high_threshold_kbits_sec = fval;
            SHR_BITSET(entry->fbmp_pro_filters, idx);
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_RATE_LOW_THRESHOLD_KBITS_SECf:
            entry->promotion_filters[idx].rate_low_threshold_kbits_sec = fval;
            SHR_BITSET(entry->fbmp_pro_filters, idx);
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_SIZE_THRESHOLD_BYTESf:
            entry->promotion_filters[idx].size_threshold_bytes = fval;
            SHR_BITSET(entry->fbmp_pro_filters, idx);
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_INCR_RATEf:
            entry->promotion_filters[idx].incr_rate = fval;
            SHR_BITSET(entry->fbmp_pro_filters, idx);
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_MONITOR_INTERVAL_USECSf:
            entry->demotion_filter.monitor_interval_usecs = fval;
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_RATE_HIGH_THRESHOLD_KBITS_SECf:
            entry->demotion_filter.rate_high_threshold_kbits_sec = fval;
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_RATE_LOW_THRESHOLD_KBITS_SECf:
            entry->demotion_filter.rate_low_threshold_kbits_sec = fval;
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_SIZE_THRESHOLD_BYTESf:
            entry->demotion_filter.size_threshold_bytes = fval;
            break;
        case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_INCR_RATEf:
            entry->demotion_filter.incr_rate = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_BITSET(entry->fbmp, fid);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_elephant_profile_lt_fields_ll_parse(int unit,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        bcmcth_mon_flowtracker_elephant_profile_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_elephant_profile_lt_field_get(unit, gen_field,
                                                         entry));
        gen_field = gen_field->next;
    }

    gen_field = key;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_elephant_profile_lt_field_get(unit, gen_field,
                                                               entry));
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_elephant_profile_stage
(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mon_flowtracker_elephant_profile_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_elephant_profile_lt_fields_ll_parse(unit, key, data,
                                                           &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_elephant_profile_insert(unit, &entry));
            break;

        case BCMIMM_ENTRY_UPDATE:
            
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_elephant_profile_delete(unit, &entry));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }


exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_flow_group_lt_field_get (int unit,
        const bcmltd_field_t *gen_field,
        bcmcth_mon_ft_group_t *entry)
{
    uint32_t fid;
    uint32_t idx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = gen_field->id;
    idx = gen_field->idx;
    fval = gen_field->data;

    switch (fid) {
        case MON_FLOWTRACKER_GROUPt_MON_FLOWTRACKER_GROUP_IDf:
            entry->mon_flowtracker_group_id = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_LEARNf:
            entry->learn = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_DT_EM_GRP_TEMPLATE_IDf:
            entry->dt_em_grp_template_id = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_NUM_TRACKING_PARAMETERSf:
            entry->num_tp = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_TRACKING_PARAMETERSu_TYPEf:
            entry->tp[idx].type = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_TRACKING_PARAMETERSu_UDF_POLICY_IDf:
            entry->tp[idx].udf_policy_id = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_FLOW_LIMITf:
            entry->flow_limit = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_AGING_INTERVAL_MSf:
            entry->aging_interval_ms = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_NUM_EXPORT_TRIGGERSf:
            entry->num_export_triggers = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_EXPORT_TRIGGERSf:
            entry->export_triggers[idx] = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_NUM_ACTIONSf:
            entry->num_actions = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_TYPEf:
            entry->actions[idx].type = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_OPAQUE_OBJ0_VALf:
            entry->actions[idx].em_ft_opaque_obj0_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_IOAM_GBP_ACTION_VALf:
            entry->actions[idx].em_ft_ioam_gbp_action_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_COPY_TO_CPU_VALf:
            entry->actions[idx].em_ft_copy_to_cpu_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_DROP_ACTION_VALf:
            entry->actions[idx].em_ft_drop_action_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_DESTINATION_VALf:
            entry->actions[idx].destination_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_DESTINATION_TYPE_VALf:
            entry->actions[idx].destination_type_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_FLEX_STATE_ACTION_VALf:
            entry->actions[idx].em_ft_flex_state_action_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_FLEX_CTR_ACTION_VALf:
            entry->actions[idx].flex_ctr_action_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_PKT_FLOW_ELIGIBILITY_VALf:
            entry->actions[idx].pkt_flow_eligibility_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_ACTIONSu_L2_IIF_SVP_MIRROR_INDEX_0_VALf:
            entry->actions[idx].l2_iif_svp_mirror_index_0_val = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_MON_FLOWTRACKER_ELEPHANT_PROFILE_IDf:
            entry->mon_flowtracker_elephant_profile_id = fval;
            break;
        case MON_FLOWTRACKER_GROUPt_OPERATIONAL_STATEf:
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
bcmcth_mon_ft_group_lt_fields_arr_parse(int unit,
        bcmltd_field_t **data,
        int count,
        bcmcth_mon_ft_group_t *entry)
{
    const bcmltd_field_t *gen_field;
    int idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    for (idx = 0; idx < count; idx++) {
        gen_field = ((bcmltd_field_t **) data)[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_flow_group_lt_field_get(unit, gen_field, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_group_entry_get(int unit, uint32_t id,
                              bcmcth_mon_ft_group_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    uint32_t field_count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_group_out_fields_count(unit, &field_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit,
                                         field_count,
                                         &out));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit,
                                         1,
                                         &in));

    in.count = 1;
    in.field[0]->id = MON_FLOWTRACKER_GROUPt_MON_FLOWTRACKER_GROUP_IDf;
    in.field[0]->data = id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, MON_FLOWTRACKER_GROUPt, &in, &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_group_lt_fields_arr_parse(unit,
                                                 out.field, out.count,
                                                 entry));

exit:
    bcmcth_mon_ft_util_fields_free(unit, field_count,
                                   &out);
    bcmcth_mon_ft_util_fields_free(unit, 1, &in);
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_lt_fields_ll_parse(int unit,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        bcmcth_mon_ft_group_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_flow_group_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }

    gen_field = key;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_flow_group_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_lt_stage_out_fields_populate(int unit,
    bcmltd_common_mon_flowtracker_group_state_t_t *oper,
    bcmltd_fields_t *output_fields)
{
    int count = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oper, SHR_E_PARAM);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    /* Update the operational fields. */
    output_fields->field[count]->id =
        MON_FLOWTRACKER_GROUPt_OPERATIONAL_STATEf;
    output_fields->field[count]->data = *oper;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->count = count;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_stage(
        int unit,
        bcmltd_sid_t sid,
        uint32_t trans_id,
        bcmimm_entry_event_t event,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    bcmcth_mon_ft_group_t cur_entry, entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_group_lt_fields_ll_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_group_insert(unit, &entry));
            break;

        case BCMIMM_ENTRY_UPDATE:
            /* Get the current state of the IMM entry. */
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_group_entry_get(unit,
                        entry.mon_flowtracker_group_id,
                        &cur_entry));

            /* Pass the old and new entry to the driver */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_group_update(unit, &cur_entry, &entry));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_group_delete(unit,
                                            entry.mon_flowtracker_group_id));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_group_lt_stage_out_fields_populate(unit,
            &entry.oper, output_fields));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_export_template_lt_field_get(int unit,
        const bcmltd_field_t *gen_field,
        bcmcth_mon_ft_export_template_t *entry)
{
    uint32_t fid;
    uint32_t idx;
    uint64_t fval;
    int rv;

    SHR_FUNC_ENTER(unit);

    fid = gen_field->id;
    idx = gen_field->idx;
    fval = gen_field->data;

    switch (fid) {
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_FLOWTRACKER_EXPORT_TEMPLATE_IDf:
            entry->mon_ft_export_template_id = fval;
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_SET_IDf:
            entry->set_id = fval;
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_NUM_EXPORT_ELEMENTSf:
            entry->num_export_elements = fval;
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_TYPEf:
            entry->export_elements[idx].type = fval;
            SHR_BITSET(entry->fbmp_export_ele, idx);
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_DATA_SIZEf:
            entry->export_elements[idx].data_size = fval;
            SHR_BITSET(entry->fbmp_export_ele, idx);
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_ENTERPRISEf:
            entry->export_elements[idx].enterprise = fval;
            SHR_BITSET(entry->fbmp_export_ele, idx);
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_ENTERPRISE_IDf:
            entry->export_elements[idx].id = fval;
            SHR_BITSET(entry->fbmp_export_ele, idx);
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_COLLECTOR_TYPEf:
            entry->collector_type = fval;
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_COLLECTOR_IPV4_IDf:
            entry->collector_ipv4_id = fval;
            rv = bcmcth_mon_collector_ipv4_entry_get(unit,
                    entry->collector_ipv4_id,
                    &(entry->collector_ipv4));
            if (rv == SHR_E_NONE) {
                entry->collector_ipv4_found = true;
            } else {
                entry->oper_status =
                    BCMLTD_COMMON_MON_FLOWTRACKER_EXPORT_TEMPLATE_STATE_T_T_COLLECTOR_NOT_EXISTS;
            }
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_COLLECTOR_IPV6_IDf:
            entry->collector_ipv6_id = fval;
            rv = bcmcth_mon_collector_ipv6_entry_get(unit,
                    entry->collector_ipv6_id,
                    &(entry->collector_ipv6));
            if (rv == SHR_E_NONE) {
                entry->collector_ipv6_found = true;
            } else {
                entry->oper_status =
                    BCMLTD_COMMON_MON_FLOWTRACKER_EXPORT_TEMPLATE_STATE_T_T_COLLECTOR_NOT_EXISTS;
            }
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_TRANSMIT_INTERVALf:
            entry->transmit_interval = fval;
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_INITIAL_BURSTf:
            entry->initial_burst = fval;
            break;
        case MON_FLOWTRACKER_EXPORT_TEMPLATEt_OPERATIONAL_STATUSf:
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
bcmcth_mon_ft_export_template_fields_ll_parse(int unit,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        bcmcth_mon_ft_export_template_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = key;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_export_template_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_export_template_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_export_template_lt_fields_arr_parse(int unit,
        bcmltd_field_t **data,
        int count,
        bcmcth_mon_ft_export_template_t *entry)
{
    const bcmltd_field_t *gen_field;
    int idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    for (idx = 0; idx < count; idx++) {
        gen_field = ((bcmltd_field_t **) data)[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_export_template_lt_field_get(unit, gen_field, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_export_template_entry_get(int unit, uint32_t id,
                            bcmcth_mon_ft_export_template_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    uint32_t field_count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_export_template_out_fields_count
            (unit, &field_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit,
                          field_count,
                          &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, 1, &in));

    in.field[0]->id = MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_FLOWTRACKER_EXPORT_TEMPLATE_IDf;
    in.field[0]->data = id;
    in.count = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, MON_FLOWTRACKER_EXPORT_TEMPLATEt, &in, &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_export_template_lt_fields_arr_parse(unit,
                                                           out.field, out.count,
                                                           entry));

exit:
    bcmcth_mon_ft_util_fields_free(unit, 1, &in);
    bcmcth_mon_ft_util_fields_free(unit, field_count, &out);
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_export_template_lt_stage_out_fields_populate(int unit,
    bcmltd_common_mon_flowtracker_export_template_state_t_t *oper,
    bcmltd_fields_t *output_fields)
{
    int count = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oper, SHR_E_PARAM);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    /* Update the operational fields. */
    output_fields->field[count]->id =
        MON_FLOWTRACKER_EXPORT_TEMPLATEt_OPERATIONAL_STATUSf;
    output_fields->field[count]->data = *oper;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->count = count;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_export_template_stage(
        int unit,
        bcmltd_sid_t sid,
        uint32_t trans_id,
        bcmimm_entry_event_t event,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    bcmcth_mon_ft_export_template_t entry, cur_entry;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_export_template_fields_ll_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            if (entry.oper_status == BCMLTD_COMMON_MON_FLOWTRACKER_EXPORT_TEMPLATE_STATE_T_T_SUCCESS) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_mon_ft_export_template_insert(unit, &entry));
            }
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_export_template_entry_get(unit,
                                                         entry.mon_ft_export_template_id,
                                                         &cur_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_export_template_update(unit,
                                                      &cur_entry,
                                                      &entry));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_export_template_delete(unit,
                                            entry.mon_ft_export_template_id));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_export_template_lt_stage_out_fields_populate(unit,
                &entry.oper_status,  output_fields));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_col_map_lt_field_get(int unit,
        const bcmltd_field_t *gen_field,
        bcmcth_mon_ft_group_col_map_t *entry)
{
    uint32_t fid;
    uint64_t fval;
    int rv;
    bcmcth_mon_ft_group_t grp_entry;

    SHR_FUNC_ENTER(unit);

    fid = gen_field->id;
    fval = gen_field->data;

    switch (fid) {
        case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_IDf:
            entry->mon_ft_group_col_map_id  = fval;
            break;
        case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_FLOWTRACKER_GROUP_IDf:
            entry->mon_ft_group_id = fval;
            rv = bcmcth_mon_ft_group_entry_get(unit, entry->mon_ft_group_id,
                    &grp_entry);
            if (rv != SHR_E_NONE) {
                entry->oper_status =
                    BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_STATE_T_T_FLOWTRACKER_GROUP_NOT_EXISTS;
                SHR_EXIT();
            }
            entry->mon_ft_group_id = fval;
            break;
        case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_COLLECTOR_TYPEf:
            entry->collector_type = fval;
            break;
        case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_COLLECTOR_IPV4_IDf:
            entry->collector_ipv4_id = fval;
            rv = bcmcth_mon_collector_ipv4_entry_get(unit,
                    entry->collector_ipv4_id,
                    &(entry->collector_ipv4));
            if (rv == SHR_E_NONE) {
                entry->collector_ipv4_found = true;
            } else {
                entry->oper_status =
                    BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_STATE_T_T_COLLECTOR_NOT_EXISTS;
                SHR_EXIT();
            }
            break;
        case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_COLLECTOR_IPV6_IDf:
            entry->collector_ipv6_id = fval;
            rv = bcmcth_mon_collector_ipv6_entry_get(unit,
                    entry->collector_ipv6_id,
                    &(entry->collector_ipv6));
            if (rv == SHR_E_NONE) {
                entry->collector_ipv6_found = true;
            } else {
                entry->oper_status =
                    BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_STATE_T_T_COLLECTOR_NOT_EXISTS;
                SHR_EXIT();
            }
            break;
        case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_EXPORT_PROFILE_IDf:
            entry->export_profile_id = fval;
            rv = bcmcth_mon_export_profile_entry_get(unit,
                    entry->export_profile_id,
                    &(entry->export_profile));
            if (rv == SHR_E_NONE) {
                entry->export_profile_found = true;
            } else {
                entry->oper_status =
                    BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_STATE_T_T_EXPORT_PROFILE_NOT_EXISTS;
                SHR_EXIT();
            }
            break;
        case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_FLOWTRACKER_EXPORT_TEMPLATE_IDf:
            entry->export_template_id = fval;
            rv = bcmcth_mon_ft_export_template_entry_get(unit,
                    entry->export_template_id,
                    &(entry->export_template));
            if (rv == SHR_E_NONE) {
                entry->export_template_found = true;
            } else {
                entry->oper_status =
                    BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_STATE_T_T_EXPORT_TEMPLATE_NOT_EXISTS;
                SHR_EXIT();
            }
            break;
        case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_OPERATIONAL_STATUSf:
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
bcmcth_mon_ft_group_col_map_fields_ll_parse(int unit,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        bcmcth_mon_ft_group_col_map_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = key;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_group_col_map_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_group_col_map_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_col_map_lt_stage_out_fields_populate(int unit,
    bcmltd_common_mon_flowtracker_group_collector_map_state_t_t *oper,
    bcmltd_fields_t *output_fields)
{
    int count = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oper, SHR_E_PARAM);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    /* Update the operational fields. */
    output_fields->field[count]->id =
        MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_OPERATIONAL_STATUSf;
    output_fields->field[count]->data = *oper;
    output_fields->field[count]->idx = 0;
    count++;

    output_fields->count = count;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_collector_map_stage(
        int unit,
        bcmltd_sid_t sid,
        uint32_t trans_id,
        bcmimm_entry_event_t event,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    bcmcth_mon_ft_group_col_map_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_group_col_map_fields_ll_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            if (entry.oper_status == 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_mon_ft_group_col_map_insert(unit, &entry));
            }
            break;

        case BCMIMM_ENTRY_UPDATE:
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_group_col_map_delete(unit, &entry));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_group_col_map_lt_stage_out_fields_populate(unit,
            &entry.oper_status, output_fields));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_actions_compare(
                    int unit,
                    flowtracker_action_info_t *actions1,
                    flowtracker_action_info_t *actions2,
                    uint32_t num_actions)
{
    bool actn_found = false;
    uint32_t i,j;
    SHR_FUNC_ENTER(unit);
    for (i = 0; i < num_actions; i++) {
        actn_found = false;
        for (j = 0; j < num_actions; j++) {
            if (actions1[i].type == actions2[j].type) {
                actn_found = true;
                break;
            }
        }
        if (!actn_found) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}


static int
bcmcth_mon_ft_hw_lrn_flow_actn_ctrl_lt_field_get(int unit,
        const bcmltd_field_t *gen_field,
        bcmcth_mon_ft_hw_learn_flow_action_t *entry)
{
    uint32_t fid;
    uint32_t idx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = gen_field->id;
    idx = gen_field->idx;
    fval = gen_field->data;

    switch (fid) {
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_PIPEf:
            entry->pipe  = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_EXACT_MATCH_INDEXf:
            entry->exact_match_idx = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_MODEf:
            entry->mode = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_MON_FLOWTRACKER_GROUP_IDf:
            entry->mon_flowtracker_group_id = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_TYPEf:
            entry->actions[idx].type = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_EM_FT_OPAQUE_OBJ0_VALf:
            entry->actions[idx].em_ft_opaque_obj0_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_EM_FT_IOAM_GBP_ACTION_VALf:
            entry->actions[idx].em_ft_ioam_gbp_action_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_EM_FT_COPY_TO_CPU_VALf:
            entry->actions[idx].em_ft_copy_to_cpu_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_EM_FT_DROP_ACTION_VALf:
            entry->actions[idx].em_ft_drop_action_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_DESTINATION_VALf:
            entry->actions[idx].destination_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_DESTINATION_TYPE_VALf:
            entry->actions[idx].destination_type_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_EM_FT_FLEX_STATE_ACTION_VALf:
            entry->actions[idx].em_ft_flex_state_action_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_FLEX_CTR_ACTION_VALf:
            entry->actions[idx].flex_ctr_action_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_PKT_FLOW_ELIGIBILITY_VALf:
            entry->actions[idx].pkt_flow_eligibility_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_ACTIONSu_L2_IIF_SVP_MIRROR_INDEX_0_VALf:
            entry->actions[idx].l2_iif_svp_mirror_index_0_val = fval;
            break;
        case MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_CMDf:
            entry->cmd = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_BITSET(entry->fbmp, fid);
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_hw_lrn_flow_actn_ctrl_fields_ll_parse(int unit,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        bcmcth_mon_ft_hw_learn_flow_action_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = key;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_hw_lrn_flow_actn_ctrl_lt_field_get(unit, gen_field,
                                                              entry));
        gen_field = gen_field->next;
    }

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_hw_lrn_flow_actn_ctrl_lt_field_get(unit, gen_field,
                                                              entry));
        gen_field = gen_field->next;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_insert_hw_lrn_flow_actn_ctrl_dummy_entry(int unit)
{
    uint32_t field_cnt = 1; /* Just insert with one field */
    bcmltd_fields_t lt_flds = {0};
    uint8_t pipe = 0;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, field_cnt, &lt_flds));
    lt_flds.count = field_cnt;
    lt_flds.field[0]->id = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_PIPEf;
    lt_flds.field[0]->data = pipe;


    SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_insert(unit,
                MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt, &lt_flds));
exit:
    bcmcth_mon_ft_util_fields_free(unit, field_cnt, &lt_flds);
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_insert_hw_lrn_flow_actn_state_dummy_entries(int unit)
{
    uint32_t field_cnt = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt_FIELD_COUNT;
    bcmltd_fields_t status_lt_flds = {0};
    uint8_t pipe;
    bcmltd_common_flowtracker_hw_learn_flow_action_state_t_t status =
        BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_SUCCESS;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    SHR_FUNC_ENTER(unit);
    ft_drv = bcmcth_mon_ft_drv_get(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, field_cnt, &status_lt_flds));
    /* pipe is the key */
    for (pipe = 0; pipe < ft_drv->num_pipes; pipe++) {

        status_lt_flds.count = field_cnt;
        /* Key = Pipe */
        status_lt_flds.field[0]->id = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt_PIPEf;
        status_lt_flds.field[0]->data = pipe;

        /* Data = status */
        status_lt_flds.field[1]->id = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt_STATEf;
        status_lt_flds.field[1]->data = status;


        SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_insert(unit,
                    MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt, &status_lt_flds));
    }
exit:
    bcmcth_mon_ft_util_fields_free(unit, field_cnt, &status_lt_flds);
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_hw_lrn_flow_actn_state_update(
         int unit,
         uint8_t pipe,
         bcmltd_common_flowtracker_hw_learn_flow_action_state_t_t status)
{
    uint32_t field_cnt = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt_FIELD_COUNT;
    bcmltd_fields_t status_lt_flds = {0};
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, field_cnt, &status_lt_flds));

    status_lt_flds.count = field_cnt;
    /* Key = Pipe */
    status_lt_flds.field[0]->id = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt_PIPEf;
    status_lt_flds.field[0]->data = pipe;

    /* Data = status */
    status_lt_flds.field[1]->id = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt_STATEf;
    status_lt_flds.field[1]->data = status;


    SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_update(unit, true,
                MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt, &status_lt_flds));
exit:
    bcmcth_mon_ft_util_fields_free(unit, field_cnt, &status_lt_flds);
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_hw_lrn_flow_actn_ctrl_update(
        int unit,
        bcmcth_mon_ft_hw_learn_flow_action_t *entry,
        bcmltd_common_flowtracker_hw_learn_flow_action_state_t_t *status)
{
    bcmcth_mon_ft_info_t *info = NULL;
    int rv = SHR_E_NONE;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    bcmcth_mon_ft_group_t grp_entry;
    SHR_FUNC_ENTER(unit);
    ft_drv = bcmcth_mon_ft_drv_get(unit);

    *status = BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_SUCCESS;
    info = bcmcth_mon_ft_info_get(unit);
    if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
        *status = BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_NOT_SUPPORTED;
        SHR_EXIT();
    }

    /* Get the group info and validate */
    rv = bcmcth_mon_ft_group_entry_get(unit, entry->mon_flowtracker_group_id,
            &grp_entry);
    if (SHR_FAILURE(rv) ||
       (grp_entry.oper != BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS)) {
        *status = BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_GROUP_NOT_PRESENT;
        SHR_EXIT();
    }

    if (entry->cmd == BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_CMD_T_T_MODIFY) {
        /* Validate the actions by comparing with group actions */
        rv = bcmcth_mon_ft_group_actions_compare(
                unit,
                entry->actions,
                grp_entry.actions,
                grp_entry.num_actions);
        if (rv < 0) {
            *status = BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_ACTION_MISMATCH;
            SHR_EXIT();
        }
    }

    /* Call the HW set function */
    if (ft_drv && ft_drv->ft_command_set) {
        rv = ft_drv->ft_command_set(unit, entry, &grp_entry);
        if (rv < 0) {
            *status = BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_FAILURE;
        } else {
            *status = BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_SUCCESS;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_hw_learn_flow_action_control_stage(
        int unit,
        bcmltd_sid_t sid,
        uint32_t trans_id,
        bcmimm_entry_event_t event,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    bcmcth_mon_ft_hw_learn_flow_action_t entry;
    bcmltd_common_flowtracker_hw_learn_flow_action_state_t_t status;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_hw_lrn_flow_actn_ctrl_fields_ll_parse(unit, key, data, &entry));
    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            /* Do nothing */
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_hw_lrn_flow_actn_ctrl_update(unit,
                                                            &entry,
                                                            &status));
            /* Update the results from the action to STATE table. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_ft_hw_lrn_flow_actn_state_update(unit,
                                                              entry.pipe,
                                                              status));
            break;

        case BCMIMM_ENTRY_DELETE:
            /* Do nothing */
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief MON_FLOWTRACKER_GROUP_STATUS In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_FLOWTRACKER_GROUP_STATUS logical table entry commit stages.
 */
static bcmimm_lt_cb_t ft_group_status_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = NULL,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = bcmcth_mon_ft_group_status_lookup
};

/*!
 * \brief MON_FLOWTRACKER_ELEPHANT_PROFILE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_FLOWTRACKER_ELEPHANT_PROFILE logical table entry commit stages.
 */
static bcmimm_lt_cb_t ft_elephant_profile_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bcmcth_mon_ft_elephant_profile_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

/*!
 * \brief MON_FLOWTRACKER_STATS In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_FLOWTRACKER_STATS logical table entry commit stages.
 */
static bcmimm_lt_cb_t ft_stats_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = NULL,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = bcmcth_mon_ft_stats_lookup
};

/*!
 * \brief MON_FLOWTRACKER_GROUP In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_FLOWTRACKER_GROUP logical table entry commit stages.
 */
static bcmimm_lt_cb_t ft_group_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bcmcth_mon_ft_group_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

/*!
 * \brief MON_FLOWTRACKER_EXPORT_TEMPLATE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_FLOWTRACKER_EXPORT_TEMPLATE logical table entry commit stages.
 */
static bcmimm_lt_cb_t ft_export_template_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bcmcth_mon_ft_export_template_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

/*!
 * \brief MON_FLOWTRACKER_GROUP_COLLECTOR_MAP In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_FLOWTRACKER_GROUP_COLLECTOR_MAP logical table entry commit stages.
 */
static bcmimm_lt_cb_t ft_group_collector_map_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bcmcth_mon_ft_group_collector_map_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

/*!
 * \brief MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t ft_hw_learn_flow_action_control_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bcmcth_mon_ft_hw_learn_flow_action_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

/*!
 * \brief MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE logical table entry commit stages.
 */
static bcmimm_lt_cb_t ft_hw_learn_flow_action_state_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = NULL,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};


int
bcmcth_mon_ft_imm_register(int unit, bool warm)
{

    SHR_FUNC_ENTER(unit);

    /*
     * Callback registration for FT LTs
     */
    if (bcmcth_mon_ft_imm_mapped(unit, MON_FLOWTRACKER_CONTROLt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_FLOWTRACKER_CONTROLt,
                                 &ft_control_hdl, NULL));
    }
    if (bcmcth_mon_ft_imm_mapped(unit, MON_FLOWTRACKER_STATSt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_FLOWTRACKER_STATSt,
                                 &ft_stats_hdl, NULL));
    }
    if (bcmcth_mon_ft_imm_mapped(unit, MON_FLOWTRACKER_GROUP_STATUSt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_FLOWTRACKER_GROUP_STATUSt,
                                 &ft_group_status_hdl, NULL));
    }
    if (bcmcth_mon_ft_imm_mapped(unit, MON_FLOWTRACKER_ELEPHANT_PROFILEt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_FLOWTRACKER_ELEPHANT_PROFILEt,
                                 &ft_elephant_profile_hdl, NULL));
    }
    if (bcmcth_mon_ft_imm_mapped(unit, MON_FLOWTRACKER_EXPORT_TEMPLATEt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_FLOWTRACKER_EXPORT_TEMPLATEt,
                                 &ft_export_template_hdl, NULL));
    }
    if (bcmcth_mon_ft_imm_mapped(unit, MON_FLOWTRACKER_GROUPt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_FLOWTRACKER_GROUPt,
                                 &ft_group_hdl, NULL));
    }
    if (bcmcth_mon_ft_imm_mapped(unit, MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt,
                                 &ft_group_collector_map_hdl, NULL));
    }
    if (bcmcth_mon_ft_imm_mapped(unit, MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt,
                                 &ft_hw_learn_flow_action_control_hdl, NULL));
        if (!warm) {
            /*
             * Dummy insertion of MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROL
             * entry
             */
            SHR_IF_ERR_VERBOSE_EXIT(
                    bcmcth_mon_ft_insert_hw_lrn_flow_actn_ctrl_dummy_entry(unit));
        }
    }
    if (bcmcth_mon_ft_imm_mapped(unit, MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEt,
                                 &ft_hw_learn_flow_action_state_hdl, NULL));
        if (!warm) {
            /*
             * Dummy insertion of MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE
             * entries
             */
            SHR_IF_ERR_VERBOSE_EXIT(
                    bcmcth_mon_ft_insert_hw_lrn_flow_actn_state_dummy_entries(unit));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

