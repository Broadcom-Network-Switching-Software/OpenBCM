/*! \file bcmcth_ts_bs_interface.c
 *
 * BCMCTH DEVICE TS BROADSYNC APIs for interface LTs.
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
#include <bcmlrd/bcmlrd_map.h>
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
 * IMM event handlers.
 */
static int
bs_interface_field_get(int unit,
                      const bcmltd_field_t *field,
                      bcmcth_ts_bs_interface_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case DEVICE_TS_BROADSYNC_INTERFACEt_MODEf:
            entry->mode = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_BITCLOCK_HZf:
            entry->bitclk_hz = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_HEARTBEAT_HZf:
            entry->hb_hz = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SIGNf:
            entry->time.sign = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SECf:
            entry->time.sec = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_NSECf:
            entry->time.nsec = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_SIGNf:
            entry->phase_adj.sign = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_SECf:
            entry->phase_adj.sec = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_NSECf:
            entry->phase_adj.nsec = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_SIGNf:
            entry->freq_adj.sign = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_SECf:
            entry->freq_adj.sec = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_NSECf:
            entry->freq_adj.nsec = fval;
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_OPERATIONAL_STATEf:
            entry->state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_BITSET(entry->fbmp, fid);

exit:
    SHR_FUNC_EXIT();
}

/* Get a BS interface entry with default values for all the fields. */
static int
bcmcth_ts_bs_interface_defaults_get(int unit,
                                    bcmcth_ts_bs_interface_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    int sid;
    size_t num_fld;
    size_t actual_num_fld;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    sid = DEVICE_TS_BROADSYNC_INTERFACEt;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fld));

    lt_info = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * num_fld,
              "bcmcthTsBsInterfaceFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fld,
                                     fields_info,
                                     &actual_num_fld));

    for (idx = 0; idx < actual_num_fld; idx++) {

        switch (fields_info[idx].id) {
            case DEVICE_TS_BROADSYNC_INTERFACEt_MODEf:
                entry->mode = fields_info[idx].def.u8;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_BITCLOCK_HZf:
                entry->bitclk_hz = fields_info[idx].def.u32;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_HEARTBEAT_HZf:
                entry->hb_hz = fields_info[idx].def.u32;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SIGNf:
                entry->time.sign = fields_info[idx].def.is_true;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SECf:
                entry->time.sec = fields_info[idx].def.u64;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_NSECf:
                entry->time.nsec = fields_info[idx].def.u32;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_SIGNf:
                entry->phase_adj.sign = fields_info[idx].def.is_true;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_SECf:
                entry->phase_adj.sec = fields_info[idx].def.u64;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_NSECf:
                entry->phase_adj.nsec = fields_info[idx].def.u32;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_SIGNf:
                entry->freq_adj.sign = fields_info[idx].def.is_true;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_SECf:
                entry->freq_adj.sec = fields_info[idx].def.u64;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_NSECf:
                entry->freq_adj.nsec = fields_info[idx].def.u32;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_OPERATIONAL_STATEf:
                /* Don't get the defaults for the read-only fields. */
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_DEVICE_TS_BROADSYNC_INTERFACE_IDf:
                /* Invalid or 0 default value. */
                break;

            default:
               LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "BS(unit %d) Interface defaults_get().!!!(%ld:%u)\n"),
                    unit, (unsigned long)idx, fields_info[idx].id));
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

/* Get the TS_BROADSYNC_INTERFACE entry currently in IMM. */
static int
bcmcth_ts_bs_interface_entry_get(int unit, uint32_t intf_id,
                                 bcmcth_ts_bs_interface_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    const bcmltd_field_t *field;
    uint32_t sid;
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sid = DEVICE_TS_BROADSYNC_INTERFACEt;

    /* Allocate memory for key. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_fields_alloc(unit, 1, &in));
    in.field[0]->id =
        DEVICE_TS_BROADSYNC_INTERFACEt_DEVICE_TS_BROADSYNC_INTERFACE_IDf;
    in.field[0]->data = intf_id;

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
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_interface_defaults_get(unit, entry));

    for (idx = 0; idx < out.count; idx++) {
        field = out.field[idx];
        SHR_IF_ERR_VERBOSE_EXIT(bs_interface_field_get(unit, field, entry));
    }

    entry->intf_id = intf_id;

exit:
    bcmcth_ts_bs_fields_free(unit, 1, &in);
    bcmcth_ts_bs_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}


/*
 * Convert the user passed fields in the stage callback handler to a structure.
 */
static int
bs_interface_stage_fields_parse(int unit,
                                bcmimm_entry_event_t event,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data,
                                bcmcth_ts_bs_interface_t *entry)
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
            (bcmcth_ts_bs_interface_defaults_get(unit, entry));
    } else {
        /* Get the current view of IMM if it is an update operation. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_interface_entry_get(unit, key->data, entry));
    }

    /*
     * Clear off the fbmp.
     * It will be set with the user programmed fields.
     */
    sal_memset(&(entry->fbmp), 0, sizeof(entry->fbmp));

    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(bs_interface_field_get(unit, field, entry));
        field = field->next;
    }

    entry->intf_id = key->data;

exit:
    SHR_FUNC_EXIT();
}

static void
bs_interface_stage_out_fields_populate(int unit,
              bcmltd_common_device_ts_broadsync_operational_state_t_t state,
              bcmltd_fields_t *output_fields)
{
    int count = 0;

    output_fields->field[count]->id =
                    DEVICE_TS_BROADSYNC_INTERFACEt_OPERATIONAL_STATEf;
    output_fields->field[count]->data = state;
    count++;

    output_fields->count = count;
}


static int
bs_interface_stage(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    bcmcth_ts_bs_interface_t old_entry, new_entry;
    bcmltd_common_device_ts_broadsync_operational_state_t_t oper;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bs_interface_stage_fields_parse(unit, BCMIMM_ENTRY_INSERT,
                                                 key, data, &new_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_interface_insert(unit, &new_entry, &oper));

            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_interface_entry_get(unit, key->data, &old_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bs_interface_stage_fields_parse(unit, BCMIMM_ENTRY_UPDATE,
                                                 key, data, &new_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_bs_interface_update(unit, &new_entry,
                                               &old_entry, &oper));

            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_interface_delete(unit));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        bs_interface_stage_out_fields_populate(unit, oper, output_fields);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bs_interface_lookup_out_fields_populate(int unit,
              bcmcth_ts_bs_interface_t *entry,
              bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case DEVICE_TS_BROADSYNC_INTERFACEt_DEVICE_TS_BROADSYNC_INTERFACE_IDf:
                *data = entry->intf_id;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_MODEf:
                *data = entry->mode;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_BITCLOCK_HZf:
                *data = entry->bitclk_hz;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_HEARTBEAT_HZf:
                *data = entry->hb_hz;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SIGNf:
                *data = entry->time.sign;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SECf:
                *data = entry->time.sec;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_NSECf:
                *data = entry->time.nsec;
                break;

            case DEVICE_TS_BROADSYNC_INTERFACEt_OPERATIONAL_STATEf:
                *data = entry->state;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

static int
bs_interface_lookup(int unit,
                           bcmltd_sid_t sid,
                           uint32_t trans_id,
                           void *context,
                           bcmimm_lookup_type_t lkup_type,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out)
{
    bcmcth_ts_bs_interface_t entry;
    const bcmltd_fields_t *key_fields = NULL;
    uint32_t intf_id = 0;
    size_t i;

    SHR_FUNC_ENTER(unit);

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* Parse the key fields to get the intf id */
    for (i = 0; i < key_fields->count; i++) {
        if (key_fields->field[i]->id ==
            DEVICE_TS_BROADSYNC_INTERFACEt_DEVICE_TS_BROADSYNC_INTERFACE_IDf) {
            intf_id = key_fields->field[i]->data;
            break;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ts_bs_interface_entry_get(unit, intf_id, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ts_bs_interface_lookup(unit, intf_id, &entry));

    bs_interface_lookup_out_fields_populate(unit, &entry, out);

exit:
    SHR_FUNC_EXIT();
}


/* DEVICE_TS_BROADSYNC_INTERFACE IMM event callback structure. */
static bcmimm_lt_cb_t bs_interface_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bs_interface_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = bs_interface_lookup
};

static int
bs_status_prepopulate(int unit,
            bcmltd_sid_t sid)
{
    bcmltd_fields_t fields = {0};
    uint32_t max_intf;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_table_attr_get(unit, sid,
                                   BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                                   &max_intf));

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_fields_alloc(unit, 1, &fields));

    for (idx = 0; idx < max_intf; idx++) {
        fields.field[0]->id = DEVICE_TS_BROADSYNC_STATUSt_DEVICE_TS_BROADSYNC_INTERFACE_IDf;
        fields.field[0]->data = idx;
        SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_insert(unit, sid, &fields));
    }

exit:
    bcmcth_ts_bs_fields_free(unit, 1, &fields);
    SHR_FUNC_EXIT();

}

static void
bs_status_lookup_out_fields_populate(int unit,
              bcmcth_ts_bs_interface_status_t *entry,
              bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case DEVICE_TS_BROADSYNC_STATUSt_DEVICE_TS_BROADSYNC_INTERFACE_IDf:
                *data = entry->intf_id;
                break;

            case DEVICE_TS_BROADSYNC_STATUSt_STATEf:
                *data = entry->status;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

static int
bs_status_lookup(int unit,
                           bcmltd_sid_t sid,
                           uint32_t trans_id,
                           void *context,
                           bcmimm_lookup_type_t lkup_type,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out)
{
    bcmcth_ts_bs_interface_t intf_entry;
    bcmcth_ts_bs_interface_status_t entry;
    const bcmltd_fields_t *key_fields = NULL;
    uint32_t intf_id = 0;
    size_t i;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* Parse the key fields to get the intf id */
    for (i = 0; i < key_fields->count; i++) {
        if (key_fields->field[i]->id ==
            DEVICE_TS_BROADSYNC_STATUSt_DEVICE_TS_BROADSYNC_INTERFACE_IDf) {
            intf_id = key_fields->field[i]->data;
            break;
        }
    }

    bcmcth_ts_bs_interface_entry_get(unit, intf_id, &intf_entry);

    entry.intf_id = intf_id;

    if (intf_entry.state == OPERATIONAL_STATE(SUCCESS)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ts_bs_status_lookup(unit, intf_id, &entry));
    }

    bs_status_lookup_out_fields_populate(unit, &entry, out);

exit:
    SHR_FUNC_EXIT();
}

/* DEVICE_TS_BROADSYNC_INTERFACE IMM event callback structure. */
static bcmimm_lt_cb_t bs_status_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = NULL,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = bs_status_lookup
};


/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ts_bs_interface_imm_register(int unit, bool warm)
{
    bcmcth_ts_bs_imm_reg_t imm_reg[] = {
        {
            DEVICE_TS_BROADSYNC_INTERFACEt,
            NULL,
            &bs_interface_hdl
        },
        {
            DEVICE_TS_BROADSYNC_STATUSt,
            bs_status_prepopulate,
            &bs_status_hdl
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
