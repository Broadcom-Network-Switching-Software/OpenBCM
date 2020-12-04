/*! \file bcmcth_ts_synce_imm.c
 *
 * BCMCTH TimeSync SYNCE IMM handler.
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
#include <bcmcth/bcmcth_ts_synce_drv.h>

/*******************************************************************************
 * Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

/*******************************************************************************
 * Private Functions
 */
static int
synce_imm_lt_fields_free(int unit,
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
synce_imm_lt_fields_allocate(int unit,
                             bcmltd_fields_t *in,
                             size_t num_fields)
{
    uint32_t i;
    size_t alloc_sz;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    alloc_sz = sizeof(bcmltd_field_t *) * num_fields;
    SHR_ALLOC(in->field, alloc_sz, "bcmcthTsSynceImmEntryFields");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, alloc_sz);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t),
                  "bcmcthTsSynceImmEntryFields");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        synce_imm_lt_fields_free(unit, in, num_fields);
    }

    SHR_FUNC_EXIT();
}

static int
synce_ctrl_lt_fields_current_get(int unit,
                                 ts_synce_ctrl_entry_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields, i;
    uint32_t fid, sid, idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (synce_imm_lt_fields_allocate(unit,
                                      &key_fields,
                                      1));

    sid = DEVICE_TS_SYNCE_CLK_CONTROLt;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fields));

    SHR_IF_ERR_EXIT
        (synce_imm_lt_fields_allocate(unit, &imm_fields, entry->out_counts));
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
        idx = data_field->idx;

        if (SHR_BITGET(entry->fbmp,
                       fid + (idx * num_fields))) {
            continue;
        }

        switch (fid) {
            case DEVICE_TS_SYNCE_CLK_CONTROLt_CLK_RECOVERYf:
                entry->clk_recovery[idx] = data_field->data;
                break;
            case DEVICE_TS_SYNCE_CLK_CONTROLt_PORT_IDf:
                entry->port[idx] = data_field->data;
                break;
            case DEVICE_TS_SYNCE_CLK_CONTROLt_CLK_DIVISORf:
                entry->clk_divisor[idx] = data_field->data;
                break;
            case DEVICE_TS_SYNCE_CLK_CONTROLt_OVERRIDEf:
                entry->override = data_field->data;
                break;
            case DEVICE_TS_SYNCE_CLK_CONTROLt_OVERRIDE_CLK_VALIDf:
                entry->ovr_valid[idx] = data_field->data;
                break;
            default:
                break;
        }
    }

exit:
    synce_imm_lt_fields_free(unit, &key_fields, 1);
    synce_imm_lt_fields_free(unit, &imm_fields, entry->out_counts);

    SHR_FUNC_EXIT();
}

static int
synce_ctrl_lt_fields_default_get(int unit,
                                 ts_synce_ctrl_entry_t *entry)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    int sid, fid, out_counts;
    size_t num_fid, i;
    uint32_t elements, j;

    SHR_FUNC_ENTER(unit);

    sid = DEVICE_TS_SYNCE_CLK_CONTROLt;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));

    lt_info = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmcthTsSynceDefaultFields");
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
        elements = f_info[i].elements;

        for (j = 0; j < elements; j++) {
            switch (fid) {
                case DEVICE_TS_SYNCE_CLK_CONTROLt_CLK_RECOVERYf:
                    entry->clk_recovery[j] = f_info[i].def.is_true;
                break;
                case DEVICE_TS_SYNCE_CLK_CONTROLt_PORT_IDf:
                    entry->port[j] = f_info[i].def.u16;
                break;
                case DEVICE_TS_SYNCE_CLK_CONTROLt_CLK_DIVISORf:
                    entry->clk_divisor[j] = f_info[i].def.u32;
                break;
                case DEVICE_TS_SYNCE_CLK_CONTROLt_OVERRIDEf:
                    entry->override = f_info[i].def.is_true;
                break;
                case DEVICE_TS_SYNCE_CLK_CONTROLt_OVERRIDE_CLK_VALIDf:
                    entry->ovr_valid[j] = f_info[i].def.is_true;
                break;
            }
            out_counts++;
        }
    }
    entry->out_counts = out_counts;

exit:
    if (f_info) {
        sal_free(f_info);
    }
    SHR_FUNC_EXIT();
}

static int
synce_ctrl_lt_field_get(int unit,
                        const bcmltd_field_t *gen_field,
                        ts_synce_ctrl_entry_t *entry)
{
    uint32_t fid;
    uint32_t idx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);
    fid = gen_field->id;
    idx = gen_field->idx;
    fval = gen_field->data;

    switch (fid) {
        case DEVICE_TS_SYNCE_CLK_CONTROLt_CLK_RECOVERYf:
            entry->clk_recovery[idx] = fval;
        break;
        case DEVICE_TS_SYNCE_CLK_CONTROLt_PORT_IDf:
            entry->port[idx] = fval;
        break;
        case DEVICE_TS_SYNCE_CLK_CONTROLt_CLK_DIVISORf:
            entry->clk_divisor[idx] = fval;
        break;
        case DEVICE_TS_SYNCE_CLK_CONTROLt_OVERRIDEf:
            entry->override = fval;
        break;
        case DEVICE_TS_SYNCE_CLK_CONTROLt_OVERRIDE_CLK_VALIDf:
            entry->ovr_valid[idx] = fval;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_BITSET(entry->fbmp,
               fid + (idx * DEVICE_TS_SYNCE_CLK_CONTROLt_FIELD_COUNT));
exit:
    SHR_FUNC_EXIT();
}

static int
synce_ctrl_lt_fields_parse(int unit,
                           const bcmltd_field_t *data,
                           ts_synce_ctrl_entry_t *entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    gen_field = data;
    while (gen_field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (synce_ctrl_lt_field_get(unit, gen_field, entry));
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
synce_ctrl_lt_stage_out_fields_populate(int unit,
                                        ts_synce_ctrl_entry_oper_t *oper,
                                        bcmltd_fields_t *out)
{
    int count = 0, i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oper, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    for (i = 0; i < SYNCE_MAX_CLK_TYPE; i++) {
        out->field[count]->id = DEVICE_TS_SYNCE_CLK_CONTROLt_PORT_ID_OPERf;
        out->field[count]->data = oper->port[i];
        out->field[count]->idx = i;
        count++;
        out->field[count]->id =
                DEVICE_TS_SYNCE_CLK_CONTROLt_CLK_DIVISOR_OPERf;
        out->field[count]->data = oper->clk_divisor[i];
        out->field[count]->idx = i;
        count++;
        out->field[count]->id =
                DEVICE_TS_SYNCE_CLK_CONTROLt_OVERRIDE_CLK_VALID_OPERf;
        out->field[count]->data = oper->ovr_valid[i];
        out->field[count]->idx = i;
        count++;
        out->field[count]->id =
                DEVICE_TS_SYNCE_CLK_CONTROLt_OPERATIONAL_STATEf;
        out->field[count]->data = oper->oper_stat[i];
        out->field[count]->idx = i;
        count++;
    }
    out->field[count]->id = DEVICE_TS_SYNCE_CLK_CONTROLt_OVERRIDE_OPERf;
    out->field[count]->data = oper->override;
    out->field[count]->idx = 0;
    count++;

    out->count = count;
exit:
    SHR_FUNC_EXIT();
}

static int
synce_control_stage(int unit,
                    bcmltd_sid_t sid,
                    const bcmltd_op_arg_t *op_arg,
                    bcmimm_entry_event_t event,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *output_fields)
{
    ts_synce_ctrl_entry_t entry;
    ts_synce_ctrl_entry_oper_t oper;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(entry));
    sal_memset(&oper, 0, sizeof(oper));

    /* Get default value */
    SHR_IF_ERR_VERBOSE_EXIT
        (synce_ctrl_lt_fields_default_get(unit, &entry));
    /* Parse LT input */
    SHR_IF_ERR_VERBOSE_EXIT
        (synce_ctrl_lt_fields_parse(unit, data, &entry));
    if (output_fields) {
        output_fields->count = 0;
    }
    entry.op_arg = op_arg;
    entry.ltid = sid;
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_synce_control_update(unit, &entry, &oper));
        break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (synce_ctrl_lt_fields_current_get(unit, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_synce_control_update(unit, &entry, &oper));
        break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ts_synce_control_update(unit, &entry, &oper));
        break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        SHR_IF_ERR_VERBOSE_EXIT
            (synce_ctrl_lt_stage_out_fields_populate(unit,
                                                     &oper,
                                                     output_fields));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_TS_SYNCE_CLK_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to DEVICE_TS_SYNCE_CLK_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t synce_control_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = synce_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};
/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ts_synce_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    bcmlrd_table_attrib_t t_attrib;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_table_attributes_get(unit,
                                     "DEVICE_TS_SYNCE_CLK_CONTROL",
                                     &t_attrib);
    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, DEVICE_TS_SYNCE_CLK_CONTROLt, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit,
                                     DEVICE_TS_SYNCE_CLK_CONTROLt,
                                     &synce_control_hdl,
                                     NULL));
        }
    } else {
        /* If DEVICE_TS_SYNCE_CLK_CONTROL LT doesn't exist, exit */
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}
