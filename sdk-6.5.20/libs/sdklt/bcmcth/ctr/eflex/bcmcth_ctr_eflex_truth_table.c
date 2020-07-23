/*! \file bcmcth_ctr_eflex_truth_table.c
 *
 * BCMCTH FLEX_STATE_ING/EGR_TRUTH_TABLEt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX


/*! Generate FLEX_STATE_xxx_TRUTH_TABLE LT field ids. */
#define STATE_ING_EGR_TRUTH_TABLE_FIDS(x) \
    FLEX_STATE_ING_TRUTH_TABLEt_##x, \
    FLEX_STATE_EGR_TRUTH_TABLEt_##x

/*******************************************************************************
 * Local definitions
 */

static const bcmlrd_sid_t truth_table_sid[][2] = {
        {0,0},
        {STATE_ING_EGR_LT_IDS(TRUTH_TABLEt)},
};

/* TRUTH_TABLE LT field info */
static const bcmlrd_fid_t pipe_idx_fid[][2] = {
        {0,0},
        {STATE_ING_EGR_TRUTH_TABLE_FIDS(PIPEf)},
};

static const bcmlrd_fid_t pool_idx_fid[][2] = {
        {0,0},
        {STATE_ING_EGR_TRUTH_TABLE_FIDS(POOLf),},
};


static const bcmlrd_fid_t truth_table_instance_fid[][2] = {
        {0,0},
        {STATE_ING_EGR_TRUTH_TABLE_FIDS(TRUTH_TABLE_INSTANCEf)},
};

static const bcmlrd_fid_t index_fid[][2] = {
        {0,0},
        {STATE_ING_EGR_TRUTH_TABLE_FIDS(INDEXf)},
};

static const bcmlrd_fid_t output_fid[][2] = {
        {0,0},
        {STATE_ING_EGR_TRUTH_TABLE_FIDS(OUTPUTf)},
};

static const bcmlrd_fid_t instance_fid[][2] = {
        {0, 0},
        {FLEX_STATE_ING_TRUTH_TABLEt_INSTANCEf, 0},
};

static const bcmlrd_fid_t egr_instance_fid[][2] = {
        {0, 0},
        {0,FLEX_STATE_EGR_TRUTH_TABLEt_INSTANCEf},
};


/**********************************************************************
* Private functions
 */

/* Function to initialize TRUTH_TABLE entry with invalid values */
static int
bcmcth_ctr_eflex_truth_table_entry_init(int unit,
                                      ctr_eflex_truth_table_data_t *entry)
{
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));


    entry->pipe_idx = CTR_EFLEX_INVALID_VAL;
    entry->pool = CTR_EFLEX_INVALID_VAL;
    entry->inst = CTR_EFLEX_INVALID_VAL;
    entry->egr_inst = CTR_EFLEX_INVALID_VAL;
    entry->output = CTR_EFLEX_INVALID_VAL;
    entry->truth_table_instance = CTR_EFLEX_INVALID_VAL;
    entry->index = CTR_EFLEX_INVALID_VAL;

exit:
    SHR_FUNC_EXIT();
}

/* Function to get default values for TRUTH_TABLE entry */
static int
bcmcth_ctr_eflex_truth_table_default_get(int unit,
                                       ctr_eflex_truth_table_data_t *entry)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info = NULL;
    size_t num_fid = 0, i;
    uint8_t pipe_unique;
    eflex_comp_t comp;
    ctr_eflex_control_t *ctrl = NULL;
    uint8_t select;

    SHR_FUNC_ENTER(unit);

    select  = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));


    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                truth_table_sid[comp][select],
                                &num_fid));

    lt_info = bcmlrd_table_get(truth_table_sid[comp][select]);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmcthCtrEflextruthTableAllocFields");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fid,
                                     f_info,
                                     &num_fid));

    for (i = 0; i < num_fid; i++) {
        if (pipe_unique &&
            f_info[i].id == pipe_idx_fid[comp][select]) {
            entry->pipe_idx = f_info[i].def.u8;
        } else if (entry->comp == STATE_EFLEX &&
                   entry->ingress == true &&
                   f_info[i].id == instance_fid[comp][select]) {
            entry->inst = (state_eflex_inst_t)f_info[i].def.u32;
        } else if (entry->comp == STATE_EFLEX &&
                   entry->ingress == EGRESS &&
                   f_info[i].id == egr_instance_fid[comp][select]) {
            entry->egr_inst = (state_eflex_egr_inst_t)f_info[i].def.u32;
        } else if (f_info[i].id == pool_idx_fid[comp][select]) {
            entry->pool = f_info[i].def.u8;
        } else if (f_info[i].id == index_fid[comp][select]) {
            entry->index = f_info[i].def.u8;
        } else if (f_info[i].id == truth_table_instance_fid[comp][select]) {
            entry->truth_table_instance = f_info[i].def.u8;
        } else if (f_info[i].id == output_fid[comp][select]) {
            entry->output = f_info[i].def.u8;
        }
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

/*
 * Function to replace TRUTH_TABLE entry with values from another
 * entry if is invalid
 * entry -- LT entry with updated values provided by application
 * cur_entry -- current LT entry present in the table.
 */
static int
bcmcth_ctr_eflex_truth_table_fill(int unit,
                                ctr_eflex_truth_table_data_t *entry,
                                ctr_eflex_truth_table_data_t *cur_entry)
{
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));


    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    if (pipe_unique && entry->pipe_idx == CTR_EFLEX_INVALID_VAL) {
        entry->pipe_idx = cur_entry->pipe_idx;
    }

    if (entry->ingress == true && cur_entry->ingress == true &&
        entry->comp == STATE_EFLEX &&
        entry->inst == CTR_EFLEX_INVALID_VAL) {
        entry->inst = cur_entry->inst;
    }

    if (entry->ingress == false && cur_entry->ingress == false &&
        entry->comp == STATE_EFLEX &&
        entry->egr_inst == CTR_EFLEX_INVALID_VAL) {
        entry->egr_inst = cur_entry->egr_inst;
    }

    if (entry->pool == CTR_EFLEX_INVALID_VAL) {
        entry->pool = cur_entry->pool;
    }

    if (entry->index == CTR_EFLEX_INVALID_VAL) {
        entry->index = cur_entry->index;
    }

    if (entry->truth_table_instance == CTR_EFLEX_INVALID_VAL) {
        entry->truth_table_instance = cur_entry->truth_table_instance;
    }

    if (entry->output == CTR_EFLEX_INVALID_VAL) {
        entry->output = cur_entry->output;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse data fields in given TRUTH_TABLE entry */
static int
bcmcth_ctr_eflex_truth_table_lt_fields_check(
    int unit,
    ctr_eflex_truth_table_data_t *entry,
    const bcmltd_field_t *data_fields)
{
    int select;
    uint8_t pipe_unique;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    if (pipe_unique &&
        data_fields->id == pipe_idx_fid[comp][select]) {
        entry->pipe_idx = data_fields->data;
    } else if (entry->comp == STATE_EFLEX &&
               entry->ingress == true &&
               data_fields->id == instance_fid[comp][select]) {
        entry->inst = data_fields->data;
    } else if (entry->comp == STATE_EFLEX &&
               entry->ingress == false &&
               data_fields->id == egr_instance_fid[comp][select]) {
        entry->egr_inst = data_fields->data;
    } else if (data_fields->id == pool_idx_fid[comp][select]) {
        entry->pool = data_fields->data;
    } else if (data_fields->id == truth_table_instance_fid[comp][select]) {
        entry->truth_table_instance = data_fields->data;
    } else if (data_fields->id == index_fid[comp][select]) {
        entry->index = data_fields->data;
    } else if (data_fields->id == output_fid[comp][select]) {
        entry->output = data_fields->data;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse given TRUTH_TABLE entry */
static int
bcmcth_ctr_eflex_truth_table_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    ctr_eflex_truth_table_data_t *entry)
{
    int select;
    uint8_t pipe_unique;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Parse key fields */
    while (key_fields) {
        if (pipe_unique &&
            key_fields->id == pipe_idx_fid[comp][select]) {
            entry->pipe_idx = key_fields->data;
        } else if (entry->comp == STATE_EFLEX &&
                   entry->ingress == true &&
                   key_fields->id == instance_fid[comp][select]) {
            entry->inst = key_fields->data;
        } else if (entry->comp == STATE_EFLEX &&
                   entry->ingress == false &&
                   key_fields->id == egr_instance_fid[comp][select]) {
            entry->egr_inst = key_fields->data;
        } else if (key_fields->id == pool_idx_fid[comp][select]) {
            entry->pool = key_fields->data;
        } else if (key_fields->id == truth_table_instance_fid[comp][select]) {
            entry->truth_table_instance = key_fields->data;
        } else if (key_fields->id == index_fid[comp][select]) {
            entry->index = key_fields->data;
        }

        key_fields = key_fields->next;
    }

    /* Parse data fields */
    while (data_fields) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_truth_table_lt_fields_check(unit,
                                                        entry,
                                                        data_fields));

        data_fields = data_fields->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse TRUTH_TABLE IMM entry */
static int
bcmcth_ctr_eflex_truth_table_entry_parse(
    int unit,
    ctr_eflex_truth_table_data_t *entry,
    bcmltd_fields_t *flist)
{
    uint32_t i;
    bcmltd_field_t *data_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* extract key and data fields */
    for (i = 0; i < flist->count; i++) {
        data_fields = flist->field[i];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_truth_table_lt_fields_check(unit,
                                                        entry,
                                                        data_fields));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to lookup TRUTH_TABLE table
 * with given pipe index and flex state ingress instance,
 * pool id , truth table_instance ,index as key
 * It populates the LT entry with all fields except the key field
 * which is provided within the entry as input.
 */
static int
bcmcth_ctr_eflex_truth_table_lookup(int unit,
                                  ctr_eflex_truth_table_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0, num_key_fields;
    int select;
    uint32_t t;
    uint8_t pipe_unique, count = 0;
    ctr_eflex_control_t *ctrl = NULL;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));


    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            truth_table_sid[comp][select]) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "TRUTH TABLE LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;

    if (num_key_fields) {
        /* Allocate key field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_fields_alloc(unit,
                                           &key_fields,
                                           num_key_fields));

        key_fields.count = num_key_fields;
        if (pipe_unique) {
            key_fields.field[count]->id = pipe_idx_fid[comp][select];
            key_fields.field[count++]->data = entry->pipe_idx;
        }

        if (comp == STATE_EFLEX && entry->ingress == true) {
            key_fields.field[count]->id = instance_fid[comp][select];
            key_fields.field[count++]->data = entry->inst;
        }

        if (comp == STATE_EFLEX && entry->ingress == false) {
            key_fields.field[count]->id = egr_instance_fid[comp][select];
            key_fields.field[count++]->data = entry->egr_inst;
        }

        if (comp == STATE_EFLEX) {
            key_fields.field[count]->id = pool_idx_fid[comp][select];
            key_fields.field[count++]->data = entry->pool;
        }

        if (comp == STATE_EFLEX) {
            key_fields.field[count]->id =
                                    truth_table_instance_fid[comp][select];
            key_fields.field[count++]->data = entry->truth_table_instance;
        }

        if (comp == STATE_EFLEX) {
            key_fields.field[count]->id = index_fid[comp][select];
            key_fields.field[count++]->data = entry->index;
        }
    }

    assert(count == num_key_fields);

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                truth_table_sid[comp][select],
                                &num_fields));

    /*
     * Adjust num_fields for array size
     * since LRD API counts field array size as 1
     */
    num_fields -= num_key_fields;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             truth_table_sid[comp][select],
                             &key_fields,
                             &imm_fields));

    /* Parse IMM entry fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_entry_parse(unit, entry, &imm_fields));

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to insert entry into TRUTH_TABLE IMM LT */
static int
bcmcth_ctr_eflex_truth_table_lt_entry_insert(
    int unit,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    ctr_eflex_truth_table_data_t *entry)
{
    uint8_t pipe_unique;

    SHR_FUNC_ENTER(unit);

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Get default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_default_get(unit, entry));

    /*
     * Parse user values
     * default values will be over-written by user values, if present
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_lt_fields_parse(unit,
                                                    key_fields,
                                                    data_fields,
                                                    entry));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Insert %s Entry:\n"
            "pipe_unique %d, pipe_idx %d, inst %d\n"
            "pool %d, truth_table_instance %d n"
            "Egress inst %d, index %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            pipe_unique, entry->pipe_idx, entry->inst,
             entry->pool, entry->truth_table_instance,
            entry->egr_inst, entry->index));

    /* Write to obj quantization PT */
    if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_truth_table_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to update entry into TRUTH_TABLE IMM LT */
static int
bcmcth_ctr_eflex_truth_table_lt_entry_update(
    int unit,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    ctr_eflex_truth_table_data_t *entry,
    ctr_eflex_truth_table_data_t *cur_entry)
{
    uint8_t pipe_unique;

    SHR_FUNC_ENTER(unit);

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Parse user values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_lt_fields_parse(unit,
                                                    key_fields,
                                                    data_fields,
                                                    entry));

    cur_entry->inst = entry->inst;
    cur_entry->egr_inst = entry->egr_inst;
    cur_entry->pipe_idx = entry->pipe_idx;
    cur_entry->pool = entry->pool;
    cur_entry->truth_table_instance = entry->truth_table_instance;
    cur_entry->index = entry->index;

    /* Lookup entry with given key fields */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_lookup(unit, cur_entry));

    /* Fill missing fields in new entry with current entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_fill(unit, entry, cur_entry));

    /* Determine first and last pipe number for new entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    /* Determine first and last pipe number for current entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        cur_entry->ingress,
                                        cur_entry->comp,
                                        cur_entry->pipe_idx,
                                        &cur_entry->start_pipe_idx,
                                        &cur_entry->end_pipe_idx));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Update %s Entry:\n"
            "pipe_unique %d, pipe_idx %d, inst %d\n"
            "pool %d, truth_table_instance %d\n"
            "egr inst %d, index %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            pipe_unique, entry->pipe_idx, entry->inst,
             entry->pool, entry->truth_table_instance,
            entry->egr_inst, entry->index));

    /* Write to obj quantization PT */
    if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_truth_table_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to delete entry from TRUTH_TABLE IMM LT */
static int
bcmcth_ctr_eflex_truth_table_lt_entry_delete(
    int unit,
    ctr_eflex_truth_table_data_t *entry)
{
    state_eflex_inst_t inst;
    state_eflex_egr_inst_t egr_inst;

    SHR_FUNC_ENTER(unit);

    /* Get flex state instance for current entry */
    inst = entry->inst;
    egr_inst = entry->egr_inst;

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry->ingress,
                                        entry->comp,
                                        entry->pipe_idx,
                                        &entry->start_pipe_idx,
                                        &entry->end_pipe_idx));

    /* Get default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_default_get(unit, entry));

    /* Restore flex state instance for entry */
    entry->inst = inst;
    entry->egr_inst = egr_inst;

    /* Write to range check profile table */
     if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_truth_table_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to insert/update/delete TRUTH TABLE IMM LT entry. */
static int
bcmcth_ctr_eflex_truth_table_lt_stage(int unit,
                                    bool ingress,
                                    eflex_comp_t comp,
                                    uint64_t req_flags,
                                    bcmimm_entry_event_t event_reason,
                                    const bcmltd_field_t *key_fields,
                                    const bcmltd_field_t *data_fields,
                                    bcmltd_fields_t *output_fields)
{
    ctr_eflex_truth_table_data_t entry = {0};
    ctr_eflex_truth_table_data_t cur_entry = {0};

    SHR_FUNC_ENTER(unit);

    entry.ingress = ingress;
    entry.comp = comp;
    entry.req_flags = req_flags;

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_entry_init(unit, &entry));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_truth_table_lt_entry_insert(unit,
                                                            key_fields,
                                                            data_fields,
                                                            &entry));
            break;

        case BCMIMM_ENTRY_UPDATE:
            cur_entry.ingress = ingress;
            cur_entry.comp = comp;
            cur_entry.req_flags = req_flags;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_truth_table_entry_init(unit, &cur_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_truth_table_lt_entry_update(unit,
                                                            key_fields,
                                                            data_fields,
                                                            &entry,
                                                            &cur_entry));
            break;

        case BCMIMM_ENTRY_DELETE:
            /* Parse user values */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_truth_table_lt_fields_parse(unit,
                                                            key_fields,
                                                            data_fields,
                                                            &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_truth_table_lt_entry_delete(unit,
                                                            &entry));
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();

}

/*! Function to get TRUTH TABLE IMM LT entry limit value. */
static int
bcmcth_ctr_eflex_truth_table_lt_ent_limit_get(int unit,
                                            bool ingress,
                                            eflex_comp_t comp,
                                            uint32_t *count)
{
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    const bcmlrd_map_t *map = NULL;
    uint8_t pipe_unique;
    uint32_t num_pipes, k;
    size_t ent_limit;
    int rv, select;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            ingress,
                                            comp,
                                            &pipe_unique));

    num_pipes = pipe_unique ? 1: device_info->num_pipes;

    /* Check if LT is supported for given device */
    rv = bcmlrd_map_get(unit, truth_table_sid[comp][select], &map);

    if (SHR_SUCCESS(rv) && map) {
        for (k = 0; k < map->table_attr->attributes; k++) {
            if (map->table_attr->attr[k].key ==
                    BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT) {
                ent_limit = map->table_attr->attr[k].value;
                *count = ent_limit / num_pipes;
                break;
            }
        }
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"TRUTH TABLE LT not supported."
                             "Set limit to 0\n")));
        *count = 0;

    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,"comp %d ingress %d entry_limit 0x%x\n"),
        comp, ingress, *count));

exit:
    SHR_FUNC_EXIT();
}

/***********************
 * INGRESS IMM Functions
 */


/*!
 * \brief Stage callback function of FLEX_STATE_ING_TRUTH_TABLE
 *        to insert, update, delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
 * \param [in] event_reason Reason for the entry event.
 * \param [in] key_fields Linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_fields Linked list of the data fields in the
 *                         modified entry.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields Linked list of the output fields in the
 *                            modified entry.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_truth_table_lt_stage_cb(int unit,
                                             bcmltd_sid_t sid,
                                             const bcmltd_op_arg_t *op_arg,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key_fields,
                                             const bcmltd_field_t *data_fields,
                                             void *context,
                                             bcmltd_fields_t *output_fields)
{
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_lt_stage(unit,
                                             true,
                                             STATE_EFLEX,
                                             req_flags,
                                             event_reason,
                                             key_fields,
                                             data_fields,
                                             output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of FLEX_STATE_ING_TRUTH_TABLE
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_truth_table_lt_ent_limit_get_cb(int unit,
                                                     uint32_t trans_id,
                                                     bcmltd_sid_t sid,
                                                     uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_lt_ent_limit_get(unit,
                                                     true,
                                                     STATE_EFLEX,
                                                     count));

exit:
    SHR_FUNC_EXIT();
}

/***********************
 * EGRESS IMM Functions
 */


/*!
 * \brief Stage callback function of FLEX_STATE_EGR_TRUTH_TABLE
 *        to insert, update, delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] op_arg The operation arguments.
 * \param [in] event_reason Reason for the entry event.
 * \param [in] key_fields Linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_fields Linked list of the data fields in the
 *                         modified entry.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields Linked list of the output fields in the
 *                            modified entry.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_egr_truth_table_lt_stage_cb(int unit,
                                             bcmltd_sid_t sid,
                                             const bcmltd_op_arg_t *op_arg,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key_fields,
                                             const bcmltd_field_t *data_fields,
                                             void *context,
                                             bcmltd_fields_t *output_fields)
{
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_lt_stage(unit,
                                             false,
                                             STATE_EFLEX,
                                             req_flags,
                                             event_reason,
                                             key_fields,
                                             data_fields,
                                             output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of FLEX_STATE_EGR_OBJ_QUANTIZATION
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_egr_truth_table_lt_ent_limit_get_cb(int unit,
                                                     uint32_t trans_id,
                                                     bcmltd_sid_t sid,
                                                     uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_truth_table_lt_ent_limit_get(unit,
                                                     false,
                                                     STATE_EFLEX,
                                                     count));

exit:
    SHR_FUNC_EXIT();
}

/**********************************************************************
* Public functions
 */

/*
 * Function to register callback handlers for
 * FLEX_STATE_xxx_TRUTH_TABLE LTs
 */
int
bcmcth_state_eflex_imm_truth_table_register(int unit)
{
    bcmimm_lt_cb_t state_eflex_lt_cb;
    const bcmlrd_map_t *ctr_eflex_map = NULL;
    int rv = 0;
    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, FLEX_STATE_ING_TRUTH_TABLEt, &ctr_eflex_map);
    if (SHR_SUCCESS(rv) && ctr_eflex_map) {
        /* Register FLEX_STATE_ING_TRUTH_TABLE handler */
        sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        state_eflex_lt_cb.op_stage  = bcmcth_state_eflex_ing_truth_table_lt_stage_cb;
        state_eflex_lt_cb.ent_limit_get =
            bcmcth_state_eflex_ing_truth_table_lt_ent_limit_get_cb;

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_lt_event_reg(unit,
                                FLEX_STATE_ING_TRUTH_TABLEt,
                                &state_eflex_lt_cb,
                                NULL));

        /* Register CTR_EGR_EFLEX_TRUTH_TABLE handler */
        sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        state_eflex_lt_cb.op_stage  = bcmcth_state_eflex_egr_truth_table_lt_stage_cb;
        state_eflex_lt_cb.ent_limit_get =
            bcmcth_state_eflex_egr_truth_table_lt_ent_limit_get_cb;

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_lt_event_reg(unit,
                            FLEX_STATE_EGR_TRUTH_TABLEt,
                            &state_eflex_lt_cb,
                            NULL));
    }
exit:
    SHR_FUNC_EXIT();
}
