/*! \file bcmcth_ts_tod_imm.c
 *
 * BCMCTH DEVISCE_TS_TOD IMM handler.
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
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_ts_tod_drv.h>

/*******************************************************************************
 * Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

#define OP_STATE_SUCCESS    BCMLTD_COMMON_TOD_HW_UPDATE_STATE_T_T_SUCCESS
#define OP_STATE_FAIL   BCMLTD_COMMON_TOD_HW_UPDATE_STATE_T_T_FAIL
/*******************************************************************************
 * Private Functions
 */
static int
tod_imm_lt_fields_free(int unit,
                       bcmltd_fields_t *in,
                       size_t size)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (in->field) {
        for (i = 0; i < size; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

    SHR_FUNC_EXIT();
}

static int
tod_imm_lt_fields_allocate(int unit,
                           bcmltd_fields_t *in,
                           size_t num_fields)
{
    uint32_t i;
    size_t alloc_sz;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    alloc_sz = sizeof(bcmltd_field_t *) * num_fields;
    SHR_ALLOC(in->field, alloc_sz, "bcmcthTsTodImmEntryFields");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, alloc_sz);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t),
                  "bcmcthTsTodImmEntryFields");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        tod_imm_lt_fields_free(unit, in, num_fields);
    }

    SHR_FUNC_EXIT();
}

static int
tod_lt_fields_default_get(int unit,
                          ts_tod_entry_t *entry)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    int sid, fid, out_counts;
    size_t num_fid, i;

    SHR_FUNC_ENTER(unit);

    sid = DEVICE_TS_TODt;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));

    lt_info = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmcthTsTodDefaultFields");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fid,
                                     f_info,
                                     &num_fid));
    out_counts = 0;
    for (i = 0; i < num_fid; i++) {
        fid = f_info[i].id;
        switch (fid) {
            case DEVICE_TS_TODt_HW_UPDATEf:
                entry->hw_update = f_info[i].def.is_true;
            break;
            case DEVICE_TS_TODt_TOD_SECf:
                entry->sec = f_info[i].def.u64;
            break;
            case DEVICE_TS_TODt_TOD_NSECf:
                entry->nsec = f_info[i].def.u32;
            break;
            case DEVICE_TS_TODt_ADJUSTf:
                entry->adjust = f_info[i].def.u64;
            break;
        }
        out_counts++;
    }

exit:
    if (f_info) {
        sal_free(f_info);
    }
    SHR_FUNC_EXIT();
}

static int
tod_lt_fields_current_get(int unit,
                          ts_tod_entry_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields, i;
    uint32_t fid, sid;

    SHR_FUNC_ENTER(unit);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (tod_imm_lt_fields_allocate(unit,
                                    &key_fields,
                                    1));

    sid = DEVICE_TS_TODt;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fields));

    SHR_IF_ERR_EXIT
        (tod_imm_lt_fields_allocate(unit, &imm_fields, num_fields));
    /* Lookup IMM table; error if entry not found */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             sid,
                             &key_fields,
                             &imm_fields));
    flist = &imm_fields;

    /* Parse IMM table data fields */
    for (i = 0; i < flist->count; i++) {
        data_field = flist->field[i];
        fid = data_field->id;

        if (SHR_BITGET(entry->fbmp, fid)) {
            continue;
        }

        switch (fid) {
            case DEVICE_TS_TODt_HW_UPDATEf:
                entry->hw_update = data_field->data;
                break;
            case DEVICE_TS_TODt_TOD_SECf:
                entry->sec = data_field->data;
                break;
            case DEVICE_TS_TODt_TOD_NSECf:
                entry->nsec = data_field->data;
                break;
            case DEVICE_TS_TODt_ADJUSTf:
                entry->adjust = data_field->data;
                break;
            default:
                break;
        }
    }

exit:
    tod_imm_lt_fields_free(unit, &key_fields, 1);
    tod_imm_lt_fields_free(unit, &imm_fields, num_fields);

    SHR_FUNC_EXIT();
}

static int
tod_lt_field_get(int unit,
                 const bcmltd_field_t *gen_field,
                 ts_tod_entry_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);
    fid = gen_field->id;
    fval = gen_field->data;

    switch (fid) {
        case DEVICE_TS_TODt_HW_UPDATEf:
            entry->hw_update = fval;
        break;
        case DEVICE_TS_TODt_TOD_SECf:
            entry->sec = fval;
        break;
        case DEVICE_TS_TODt_TOD_NSECf:
            entry->nsec = fval;
        break;
        case DEVICE_TS_TODt_ADJUSTf:
            entry->adjust = fval;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_BITSET(entry->fbmp, fid);
exit:
    SHR_FUNC_EXIT();
}

static int
tod_lt_fields_parse(int unit,
                    const bcmltd_field_t *data,
                    ts_tod_entry_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (tod_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tod_lt_stage_out_fields_populate(int unit,
                                 ts_tod_entry_t *entry,
                                 ts_tod_entry_oper_t *oper,
                                 bcmltd_fields_t *out)
{
    int count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oper, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->field[count]->id = DEVICE_TS_TODt_TOD_SECf;
    out->field[count]->data = entry->sec;
    count++;
    out->field[count]->id = DEVICE_TS_TODt_TOD_NSECf;
    out->field[count]->data = entry->nsec;
    count++;
    out->field[count]->id = DEVICE_TS_TODt_ADJUSTf;
    out->field[count]->data = entry->adjust;
    count++;
    out->field[count]->id = DEVICE_TS_TODt_HW_UPDATEf;
    out->field[count]->data = entry->hw_update;
    count++;
    out->field[count]->id = DEVICE_TS_TODt_TOD_SEC_OPERf;
    out->field[count]->data = oper->sec;
    count++;
    out->field[count]->id =
            DEVICE_TS_TODt_TOD_NSEC_OPERf;
    out->field[count]->data = oper->nsec;
    count++;
    out->field[count]->id =
            DEVICE_TS_TODt_ADJUST_OPERf;
    out->field[count]->data = oper->adjust;
    count++;
    out->field[count]->id =
            DEVICE_TS_TODt_OPERATIONAL_STATEf;
    if (oper->update_fail) {
        out->field[count]->data = OP_STATE_FAIL;
    } else {
        out->field[count]->data = OP_STATE_SUCCESS;
    }
    count++;

    out->count = count;
exit:
    SHR_FUNC_EXIT();
}

static int
tod_lt_stage(int unit,
             bcmltd_sid_t sid,
             const bcmltd_op_arg_t *op_arg,
             bcmimm_entry_event_t event,
             const bcmltd_field_t *key,
             const bcmltd_field_t *data,
             void *context,
             bcmltd_fields_t *output_fields)
{
    ts_tod_entry_t entry;
    ts_tod_entry_oper_t oper;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(entry));
    sal_memset(&oper, 0, sizeof(oper));

    /* Get default value */
    SHR_IF_ERR_EXIT
        (tod_lt_fields_default_get(unit, &entry));
    /* Parse LT input */
    SHR_IF_ERR_EXIT
        (tod_lt_fields_parse(unit, data, &entry));
    if (output_fields) {
        output_fields->count = 0;
    }
    entry.op_arg = op_arg;
    entry.ltid = sid;

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_tod_update(unit, &entry));
        break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (tod_lt_fields_current_get(unit, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_tod_update(unit, &entry));
        break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_tod_update(unit, &entry));
        break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tod_lt_lookup(int unit,
              bcmltd_sid_t sid,
              const bcmltd_op_arg_t *op_arg,
              void *context,
              bcmimm_lookup_type_t lkup_type,
              const bcmltd_fields_t *in,
              bcmltd_fields_t *out)
{
    ts_tod_entry_t entry;
    ts_tod_entry_oper_t oper;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    sal_memset(&entry, 0, sizeof(entry));
    sal_memset(&oper, 0, sizeof(oper));
    SHR_IF_ERR_EXIT
        (bcmcth_ts_tod_lookup(unit, sid, op_arg, &oper));
    SHR_IF_ERR_EXIT
        (tod_lt_fields_current_get(unit, &entry));
    SHR_IF_ERR_EXIT
        (tod_lt_stage_out_fields_populate(unit, &entry, &oper, out));
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief DEVICE_TS_TOD In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to DEVICE_TS_TOD logical table entry commit stages.
 */
static bcmimm_lt_cb_t tod_lt_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = tod_lt_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .op_lookup = tod_lt_lookup,
};

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ts_tod_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    bcmlrd_table_attrib_t t_attrib;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_table_attributes_get(unit,
                                     "DEVICE_TS_TOD",
                                     &t_attrib);
    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, DEVICE_TS_TODt, &map);
        if (SHR_SUCCESS(rv) &&
            map &&
            map->group &&
            map->group[0].dest.kind == BCMLRD_MAP_CUSTOM) {
            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit,
                                     DEVICE_TS_TODt,
                                     &tod_lt_hdl,
                                     NULL));
        }
    } else {
        /* If DEVICE_TS_TOD is direct-mapped LT. */
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

