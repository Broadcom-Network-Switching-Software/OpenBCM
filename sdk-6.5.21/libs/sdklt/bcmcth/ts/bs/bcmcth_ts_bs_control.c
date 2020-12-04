/*! \file bcmcth_ts_bs_control.c
 *
 * BCMCTH DEVICE TS BROADSYNC APIs for global LTs.
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
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_ts_drv.h>
#include <bcmcth/bcmcth_ts_bs.h>
#include <bcmcth/bcmcth_ts_bs_uc.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

/*******************************************************************************
 * Private Functions
 */


/*******************************************************************************
 * BROADSYNC Control handlers
 */
static int
bs_control_field_get(int unit,
                      const bcmltd_field_t *field,
                      bcmcth_ts_bs_control_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case DEVICE_TS_BROADSYNC_CONTROLt_BROADSYNCf:
            entry->broadsync = fval;
            break;

        case DEVICE_TS_BROADSYNC_CONTROLt_OPERATIONAL_STATEf:
            entry->state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Get a BS control entry with default values for all the fields. */
static int
bcmcth_ts_bs_control_defaults_get(int unit,
                                    bcmcth_ts_bs_control_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    int sid;
    size_t num_fld;
    size_t actual_num_fld;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    sid = DEVICE_TS_BROADSYNC_CONTROLt;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fld));

    lt_info = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * num_fld,
              "bcmcthTsBsControlFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fld,
                                     fields_info,
                                     &actual_num_fld));

    for (idx = 0; idx < actual_num_fld; idx++) {
        switch (fields_info[idx].id) {
            case DEVICE_TS_BROADSYNC_CONTROLt_BROADSYNCf:
                entry->broadsync = fields_info[idx].def.is_true;
                break;

            case DEVICE_TS_BROADSYNC_CONTROLt_OPERATIONAL_STATEf:
                /* Don't get the defaults for the read-only fields. */
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

/* Get the TIME_BROADSYNC_CONTROL entry currently in IMM. */
static int
bcmcth_ts_bs_control_entry_get(int unit, bcmcth_ts_bs_control_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    const bcmltd_field_t *field;
    uint32_t sid;
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sid = DEVICE_TS_BROADSYNC_CONTROLt;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ts_bs_fields_alloc(unit, num_fields, &out));

    in.count = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, sid, &in, &out));

    sal_memset(entry, 0, sizeof(*entry));

    /* Get the defaults for all the fields. Some of these may get overwritten
     * with the values from IMM.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_control_defaults_get(unit, entry));

    for (idx = 0; idx < out.count; idx++) {
        field = out.field[idx];
        SHR_IF_ERR_VERBOSE_EXIT(bs_control_field_get(unit, field, entry));
    }

exit:
    bcmcth_ts_bs_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

/*
 * Convert the user passed fields in the stage callback handler to a structure.
 */
static int
bs_control_stage_fields_parse(int unit,
                               bcmimm_entry_event_t event,
                               const bcmltd_field_t *data,
                               bcmcth_ts_bs_control_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    if ((event != BCMIMM_ENTRY_INSERT) && (event != BCMIMM_ENTRY_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (event == BCMIMM_ENTRY_INSERT) {
        /*
         * Get the defaults for all the fields. Some of these may get
         * overwritten with the values from user.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ts_bs_control_defaults_get(unit, entry));
    } else {
        /* Get the current view of IMM if it is an update operation. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_control_entry_get(unit, entry));
    }

    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(bs_control_field_get(unit, field, entry));
        field = field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bs_control_stage_out_fields_populate(int unit,
              bcmltd_common_device_ts_broadsync_operational_state_t_t state,
              bcmltd_fields_t *output_fields)
{
    int count = 0;

    output_fields->field[count]->id = DEVICE_TS_BROADSYNC_CONTROLt_OPERATIONAL_STATEf;
    output_fields->field[count]->data = state;
    count++;

    output_fields->count = count;
}

static int
bs_control_stage(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    bcmcth_ts_bs_control_t old_entry, new_entry;
    bcmltd_common_device_ts_broadsync_operational_state_t_t oper;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bs_control_stage_fields_parse(unit, BCMIMM_ENTRY_INSERT,
                                                data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_control_insert(unit, &new_entry, &oper));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_control_entry_get(unit, &old_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bs_control_stage_fields_parse(unit, BCMIMM_ENTRY_UPDATE,
                                                data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_control_update(unit, &new_entry,
                                               &old_entry, &oper));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_control_delete(unit));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        bs_control_stage_out_fields_populate(unit, oper, output_fields);
    }

exit:
    SHR_FUNC_EXIT();
}

/* DEVICE_TS_BROADSYNC_CONTROL IMM event callback structure. */
static bcmimm_lt_cb_t bs_control_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bs_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

/*******************************************************************************
 * BROADSYNC Signal handlers
 */
static int
bs_signal_field_get(int unit,
                      const bcmltd_field_t *field,
                      bcmcth_ts_bs_signal_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_FREQUENCYf:
            entry->frequency = fval;
            break;

        case DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_OPERATIONAL_STATEf:
            entry->state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/* Get a BS signal entry with default values for all the fields. */
static int
bcmcth_ts_bs_signal_defaults_get(int unit,
                                    bcmcth_ts_bs_signal_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    int sid;
    size_t num_fld;
    size_t actual_num_fld;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    sid = DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fld));

    lt_info = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * num_fld,
              "bcmcthTsBsSignalFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fld,
                                     fields_info,
                                     &actual_num_fld));

    for (idx = 0; idx < actual_num_fld; idx++) {
        switch (fields_info[idx].id) {
            case DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_FREQUENCYf:
                entry->frequency = fields_info[idx].def.u32;
                break;

            case DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_OPERATIONAL_STATEf:
                /* Don't get the defaults for the read-only fields. */
                break;

            case DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_DEVICE_TS_BROADSYNC_SIGNAL_OUTPUT_IDf:
                /* Invalid or 0 default value. */
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

/* Get the TIME_BROADSYNC_SIGNAL_OUTPUT entry currently in IMM. */
static int
bcmcth_ts_bs_signal_entry_get(int unit, uint32_t signal_id,
                                 bcmcth_ts_bs_signal_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    const bcmltd_field_t *field;
    uint32_t sid;
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    sid = DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt;

    /* Allocate memory for key. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_fields_alloc(unit, 1, &in));
    in.field[0]->id =
    DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_DEVICE_TS_BROADSYNC_SIGNAL_OUTPUT_IDf;
    in.field[0]->data = signal_id;

    /* Allocate memory for data. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fields));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ts_bs_fields_alloc(unit, num_fields, &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, sid, &in, &out));

    sal_memset(entry, 0, sizeof(*entry));

    /* Get the defaults for all the fields. Some of these may get overwritten
     * with the values from IMM.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_signal_defaults_get(unit, entry));

    for (idx = 0; idx < out.count; idx++) {
        field = out.field[idx];
        SHR_IF_ERR_VERBOSE_EXIT(bs_signal_field_get(unit, field, entry));
    }

    entry->signal_id = signal_id;

exit:
    bcmcth_ts_bs_fields_free(unit, 1, &in);
    bcmcth_ts_bs_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

/*
 * Convert the user passed fields in the stage callback handler to a structure.
 */
static int
bs_signal_stage_fields_parse(int unit,
                               bcmimm_entry_event_t event,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               bcmcth_ts_bs_signal_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    if ((event != BCMIMM_ENTRY_INSERT) && (event != BCMIMM_ENTRY_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (event == BCMIMM_ENTRY_INSERT) {
        /*
         * Get the defaults for all the fields. Some of these may get
         * overwritten with the values from user.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ts_bs_signal_defaults_get(unit, entry));

    } else {
        /* Get the current view of IMM if it is an update operation. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_signal_entry_get(unit, key->data, entry));
    }

    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(bs_signal_field_get(unit, field, entry));
        field = field->next;
    }

    entry->signal_id = key->data;

exit:
    SHR_FUNC_EXIT();
}

static void
bs_signal_stage_out_fields_populate(int unit,
              bcmltd_common_device_ts_broadsync_operational_state_t_t state,
              bcmltd_fields_t *output_fields)
{
    int count = 0;

    output_fields->field[count]->id = DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_OPERATIONAL_STATEf;
    output_fields->field[count]->data = state;
    count++;

    output_fields->count = count;
}

static int
bs_signal_stage(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    bcmcth_ts_bs_signal_t old_entry, new_entry;
    bcmltd_common_device_ts_broadsync_operational_state_t_t oper;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bs_signal_stage_fields_parse(unit, BCMIMM_ENTRY_INSERT,
                                              key, data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_signal_insert(unit, &new_entry, &oper));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_signal_entry_get(unit, key->data, &old_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bs_signal_stage_fields_parse(unit, BCMIMM_ENTRY_UPDATE,
                                              key, data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_signal_update(unit, &new_entry,
                                               &old_entry, &oper));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_signal_delete(unit, key->data));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        bs_signal_stage_out_fields_populate(unit, oper, output_fields);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bs_signal_lookup_out_fields_populate(int unit,
              bcmcth_ts_bs_signal_t *entry,
              bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_DEVICE_TS_BROADSYNC_SIGNAL_OUTPUT_IDf:
                *data = entry->signal_id;
                break;

            case DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_FREQUENCYf:
                *data = entry->frequency;
                break;

            case DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_OPERATIONAL_STATEf:
                *data = entry->state;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

static int
bs_signal_lookup(int unit,
                 bcmltd_sid_t sid,
                 uint32_t trans_id,
                 void *context,
                 bcmimm_lookup_type_t lkup_type,
                 const bcmltd_fields_t *in,
                 bcmltd_fields_t *out)
{

    bcmcth_ts_bs_signal_t entry;
    const bcmltd_fields_t *key_fields = NULL;
    uint32_t signal_id = 0;
    size_t i;

    SHR_FUNC_ENTER(unit);

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* Parse the key fields to get the signal id */
    for (i = 0; i < key_fields->count; i++) {
        if (key_fields->field[i]->id ==
            DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt_DEVICE_TS_BROADSYNC_SIGNAL_OUTPUT_IDf) {
            signal_id = key_fields->field[i]->data;
            break;
        }
    }


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ts_bs_signal_entry_get(unit, signal_id, &entry));

    bs_signal_lookup_out_fields_populate(unit, &entry, out);

exit:
    SHR_FUNC_EXIT();
}

/* DEVICE_TS_BROADSYNC_SIGNAL_OUTPUT IMM event callback structure. */
static bcmimm_lt_cb_t bs_signal_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bs_signal_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = bs_signal_lookup
};

/*******************************************************************************
 * BROADSYNC log control handlers
 */
static int
bs_logctrl_field_get(int unit,
                      const bcmltd_field_t *field,
                      bcmcth_ts_bs_logctrl_t *entry)
{
    uint64_t fval;
    uint32_t fval32[2];
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_DEBUG_MASKf:
            entry->debug_mask = fval;
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_UDP_LOG_MASKf:
            entry->udp_log_mask = fval;
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_SRC_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->src_mac, fval32);
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_DST_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->dst_mac, fval32);
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_TPIDf:
            entry->tpid = fval;
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_VLAN_IDf:
            entry->vlan_id = fval;
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_TTLf:
            entry->ttl = fval;
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_SRC_IPV4f:
            entry->src_ip = fval;
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_DST_IPV4f:
            entry->dst_ip = fval;
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_UDP_SRC_PORTf:
            entry->src_port = fval;
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_UDP_DST_PORTf:
            entry->dst_port = fval;
            break;

        case DEVICE_TS_BROADSYNC_LOG_CONTROLt_OPERATIONAL_STATEf:
            entry->state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Get a BS log control entry with default values for all the fields. */
static int
bcmcth_ts_bs_logctrl_defaults_get(int unit,
                                  bcmcth_ts_bs_logctrl_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    int sid;
    size_t num_fld;
    size_t actual_num_fld;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    sid = DEVICE_TS_BROADSYNC_LOG_CONTROLt;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fld));

    lt_info = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * num_fld,
              "bcmcthTsBsLogCtrlFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fld,
                                     fields_info,
                                     &actual_num_fld));

    for (idx = 0; idx < actual_num_fld; idx++) {
        switch (fields_info[idx].id) {
            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_DEBUG_MASKf:
                entry->debug_mask = fields_info[idx].def.u32;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_UDP_LOG_MASKf:
                entry->udp_log_mask = fields_info[idx].def.u64;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_TPIDf:
                entry->tpid = fields_info[idx].def.u16;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_VLAN_IDf:
                entry->vlan_id = fields_info[idx].def.u16;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_TTLf:
                entry->ttl = fields_info[idx].def.u8;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_SRC_IPV4f:
                entry->src_ip = fields_info[idx].def.u32;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_DST_IPV4f:
                entry->dst_ip = fields_info[idx].def.u32;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_UDP_SRC_PORTf:
                entry->src_port = fields_info[idx].def.u16;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_UDP_DST_PORTf:
                entry->dst_port = fields_info[idx].def.u16;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_SRC_MACf:
            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_DST_MACf:
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_OPERATIONAL_STATEf:
                /* Don't get the defaults for the read-only fields. */
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

/* Get the TIME_BROADSYNC_LOG_CONTROL entry currently in IMM. */
static int
bcmcth_ts_bs_logctrl_entry_get(int unit, bcmcth_ts_bs_logctrl_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    const bcmltd_field_t *field;
    uint32_t sid;
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sid = DEVICE_TS_BROADSYNC_LOG_CONTROLt;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ts_bs_fields_alloc(unit, num_fields, &out));

    in.count = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, sid, &in, &out));

    sal_memset(entry, 0, sizeof(*entry));

    /* Get the defaults for all the fields. Some of these may get overwritten
     * with the values from IMM.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_logctrl_defaults_get(unit, entry));

    for (idx = 0; idx < out.count; idx++) {
        field = out.field[idx];
        SHR_IF_ERR_VERBOSE_EXIT(bs_logctrl_field_get(unit, field, entry));
    }

exit:
    bcmcth_ts_bs_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

/*
 * Convert the user passed fields in the stage callback handler to a structure.
 */
static int
bs_logctrl_stage_fields_parse(int unit,
                               bcmimm_entry_event_t event,
                               const bcmltd_field_t *data,
                               bcmcth_ts_bs_logctrl_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    if ((event != BCMIMM_ENTRY_INSERT) && (event != BCMIMM_ENTRY_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (event == BCMIMM_ENTRY_INSERT) {
        /*
         * Get the defaults for all the fields. Some of these may get
         * overwritten with the values from user.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ts_bs_logctrl_defaults_get(unit, entry));
    } else {
        /* Get the current view of IMM if it is an update operation. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_logctrl_entry_get(unit, entry));
    }

    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(bs_logctrl_field_get(unit, field, entry));
        field = field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bs_logctrl_stage_out_fields_populate(int unit,
              bcmltd_common_device_ts_broadsync_operational_state_t_t state,
              bcmltd_fields_t *output_fields)
{
    int count = 0;

    output_fields->field[count]->id = DEVICE_TS_BROADSYNC_LOG_CONTROLt_OPERATIONAL_STATEf;
    output_fields->field[count]->data = state;
    count++;

    output_fields->count = count;
}

static int
bs_logctrl_stage(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    bcmcth_ts_bs_logctrl_t old_entry, new_entry;
    bcmltd_common_device_ts_broadsync_operational_state_t_t oper;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bs_logctrl_stage_fields_parse(unit, BCMIMM_ENTRY_INSERT,
                                                data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_logctrl_insert(unit, &new_entry, &oper));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_logctrl_entry_get(unit, &old_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bs_logctrl_stage_fields_parse(unit, BCMIMM_ENTRY_UPDATE,
                                                data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_logctrl_update(unit, &new_entry,
                                               &old_entry, &oper));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_logctrl_delete(unit));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        bs_logctrl_stage_out_fields_populate(unit, oper, output_fields);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bs_logctrl_lookup_out_fields_populate(int unit,
              bcmcth_ts_bs_logctrl_t *entry,
              bcmltd_fields_t *output_fields)
{
    uint64_t *data;
    uint32_t fval32[2];
    uint32_t fid;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_DEBUG_MASKf:
                *data = entry->debug_mask;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_UDP_LOG_MASKf:
                *data = entry->udp_log_mask;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_SRC_MACf:
                BCMCTH_MAC_ADDR_TO_UINT32(entry->src_mac, fval32);
                *data = (((uint64_t)fval32[1] << 32) | fval32[0]);
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_DST_MACf:
                BCMCTH_MAC_ADDR_TO_UINT32(entry->dst_mac, fval32);
                *data = (((uint64_t)fval32[1] << 32) | fval32[0]);
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_TPIDf:
                *data = entry->tpid;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_VLAN_IDf:
                *data = entry->vlan_id;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_TTLf:
                *data = entry->ttl;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_SRC_IPV4f:
                *data = entry->src_ip;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_DST_IPV4f:
                *data = entry->dst_ip;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_UDP_SRC_PORTf:
                *data = entry->src_port;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_UDP_DST_PORTf:
                *data = entry->dst_port;
                break;

            case DEVICE_TS_BROADSYNC_LOG_CONTROLt_OPERATIONAL_STATEf:
                *data = entry->state;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }

}

static int
bs_logctrl_lookup(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  void *context,
                  bcmimm_lookup_type_t lkup_type,
                  const bcmltd_fields_t *in,
                  bcmltd_fields_t *out)
{
    bcmcth_ts_bs_logctrl_t entry;
    bcmltd_common_device_ts_broadsync_operational_state_t_t oper;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ts_bs_logctrl_entry_get(unit, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ts_bs_logctrl_lookup(unit, &oper));
    entry.state = oper;

    bs_logctrl_lookup_out_fields_populate(unit, &entry, out);

exit:
    SHR_FUNC_EXIT();
}

/* DEVICE_TS_BROADSYNC_LOG_CONTROL IMM event callback structure. */
static bcmimm_lt_cb_t bs_logctrl_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bs_logctrl_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = bs_logctrl_lookup
};

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ts_bs_control_imm_register(int unit, bool warm)
{
    bcmcth_ts_bs_imm_reg_t imm_reg[] = {
        {
            DEVICE_TS_BROADSYNC_CONTROLt,
            NULL,
            &bs_control_hdl
        },
        {
            DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTt,
            NULL,
            &bs_signal_hdl
        },
        {
            DEVICE_TS_BROADSYNC_LOG_CONTROLt,
            NULL,
            &bs_logctrl_hdl
        },
    };
    int num_sids;

    SHR_FUNC_ENTER(unit);

    num_sids = sizeof(imm_reg) / sizeof(bcmcth_ts_bs_imm_reg_t);

    /* Register the IMM callbacks. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ts_bs_imm_cb_register(unit, warm, num_sids, imm_reg));

 exit:
    SHR_FUNC_EXIT();
}
