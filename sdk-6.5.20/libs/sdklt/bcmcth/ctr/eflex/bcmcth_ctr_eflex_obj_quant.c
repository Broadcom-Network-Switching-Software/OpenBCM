/*! \file bcmcth_ctr_eflex_obj_quant.c
 *
 * BCMCTH CTR_EFLEX_OBJ_QUANTIZATIONt IMM handler.
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

/**********************************************************************
* Private functions
 */

/* Function to initialize OBJ_QUANTIZATION entry with invalid values */
static int
bcmcth_ctr_eflex_obj_quant_entry_init(int unit,
                                      ctr_eflex_obj_quant_data_t *entry)
{
    int i, num_objs,num_global_objs;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;
    num_global_objs= device_info->num_global_objs;

    for (i = 0; i < num_objs; i++) {
        entry->quantize[i] = CTR_EFLEX_INVALID_VAL;
    }
    for (i = 0; i < (num_objs + num_global_objs); i++) {
        entry->obj[i] = CTR_EFLEX_INVALID_VAL;
    }

    entry->pipe_idx = CTR_EFLEX_INVALID_VAL;
    entry->inst = CTR_EFLEX_INVALID_VAL;
    entry->egr_inst = CTR_EFLEX_INVALID_VAL;
    entry->obj_0_1_mode = CTR_EFLEX_INVALID_VAL;
    entry->obj_2_3_mode = CTR_EFLEX_INVALID_VAL;

exit:
    SHR_FUNC_EXIT();
}

/* Function to get default values for OBJ_QUANTIZATION entry */
static int
bcmcth_ctr_eflex_obj_quant_default_get(int unit,
                                       ctr_eflex_obj_quant_data_t *entry)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *t_info = NULL;
    size_t num_fid = 0, i;
    int select, num_objs,num_global_objs, idx;
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    ctr_eflex_obj_quant_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    select  = entry->ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;
    num_global_objs= device_info->num_global_objs;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Get LT sid and fid info */
    lt_info = entry->ingress ? &ctrl->ing_obj_quant_lt_info :
                               &ctrl->egr_obj_quant_lt_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                lt_info->lt_sid,
                                &num_fid));

    t_info = bcmlrd_table_get(lt_info->lt_sid);
    SHR_NULL_CHECK(t_info, SHR_E_BADID);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmcthCtrEflexObjQuantAllocFields");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     t_info->name,
                                     num_fid,
                                     f_info,
                                     &num_fid));

    for (i = 0; i < num_fid; i++) {
        if (pipe_unique &&
            f_info[i].id == lt_info->pipe_idx_fid) {
            entry->pipe_idx = f_info[i].def.u8;
        } else if (entry->comp == STATE_EFLEX &&
                   entry->ingress == true &&
                   f_info[i].id == lt_info->instance_fid) {
            entry->inst = (state_eflex_inst_t)f_info[i].def.u32;
        } else if (entry->comp == STATE_EFLEX &&
                   entry->ingress == false &&
                   f_info[i].id == lt_info->instance_fid) {
            entry->egr_inst = (state_eflex_egr_inst_t)f_info[i].def.u32;
        } else if (f_info[i].id == lt_info->obj_0_1_mode_fid) {
            entry->obj_0_1_mode = f_info[i].def.u8;
        } else if (f_info[i].id == lt_info->obj_2_3_mode_fid) {
            entry->obj_2_3_mode = f_info[i].def.u8;
        } else if (f_info[i].id == lt_info->obj_fid) {
            for (idx = 0; idx < (num_objs + num_global_objs); idx++) {
                entry->obj[idx] = bcmcth_ctr_eflex_obj_id_get(
                                    unit,
                                    select,
                                    f_info[i].def.u32);
            }
        } else if (f_info[i].id == lt_info->quantize_fid) {
            for (idx = 0; idx < num_objs; idx++) {
                entry->quantize[idx] = f_info[i].def.u8;
            }
        }
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

/*
 * Function to replace OBJ_QUANTIZATION entry with values from another
 * entry if is invalid
 * entry -- LT entry with updated values provided by application
 * cur_entry -- current LT entry present in the table.
 */
static int
bcmcth_ctr_eflex_obj_quant_fill(int unit,
                                ctr_eflex_obj_quant_data_t *entry,
                                ctr_eflex_obj_quant_data_t *cur_entry)
{
    int i, num_objs,num_global_objs;
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;
    num_global_objs= device_info->num_global_objs;

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

    if (entry->obj_0_1_mode == CTR_EFLEX_INVALID_VAL) {
        entry->obj_0_1_mode = cur_entry->obj_0_1_mode;
    }

    if (entry->obj_2_3_mode == CTR_EFLEX_INVALID_VAL) {
        entry->obj_2_3_mode = cur_entry->obj_2_3_mode;
    }

    for (i = 0; i < (num_objs + num_global_objs); i++) {
        if (entry->obj[i] == CTR_EFLEX_INVALID_VAL) {
            entry->obj[i] = cur_entry->obj[i];
        }

    }
    for (i = 0; i < num_objs; i++) {
        if (entry->quantize[i] == CTR_EFLEX_INVALID_VAL) {
            entry->quantize[i] = cur_entry->quantize[i];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse data fields in given OBJ_QUANTIZATION entry */
static int
bcmcth_ctr_eflex_obj_quant_lt_fields_check(
    int unit,
    ctr_eflex_obj_quant_data_t *entry,
    const bcmltd_field_t *data_fields)
{
    int select;
    uint8_t pipe_unique;
    eflex_comp_t comp;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_obj_quant_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Get LT fid info */
    lt_info = entry->ingress ? &ctrl->ing_obj_quant_lt_info :
                               &ctrl->egr_obj_quant_lt_info;

    if (pipe_unique &&
        data_fields->id == lt_info->pipe_idx_fid) {
        entry->pipe_idx = data_fields->data;
    } else if (entry->comp == STATE_EFLEX &&
               entry->ingress == true &&
               data_fields->id == lt_info->instance_fid) {
        entry->inst = data_fields->data;
    } else if (entry->comp == STATE_EFLEX &&
               entry->ingress == false &&
               data_fields->id == lt_info->instance_fid) {
               entry->inst = data_fields->data;
    } else if (data_fields->id == lt_info->obj_0_1_mode_fid) {
        entry->obj_0_1_mode = data_fields->data;
    } else if (data_fields->id == lt_info->obj_2_3_mode_fid) {
        entry->obj_2_3_mode = data_fields->data;
    } else if (data_fields->id == lt_info->obj_fid) {
        entry->obj[data_fields->idx] = bcmcth_ctr_eflex_obj_id_get(
                                            unit,
                                            select,
                                            data_fields->data);
    } else if (data_fields->id == lt_info->quantize_fid) {
        entry->quantize[data_fields->idx] = data_fields->data;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse given RANGE_CHK_PROFILE entry */
static int
bcmcth_ctr_eflex_obj_quant_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    ctr_eflex_obj_quant_data_t *entry)
{
    uint8_t pipe_unique;
    eflex_comp_t comp;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_obj_quant_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get LT sid and fid info */
    lt_info = entry->ingress ? &ctrl->ing_obj_quant_lt_info :
                               &ctrl->egr_obj_quant_lt_info;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Parse key fields */
    while (key_fields) {
        if (pipe_unique &&
            key_fields->id == lt_info->pipe_idx_fid) {
            entry->pipe_idx = key_fields->data;
        } else if (entry->comp == STATE_EFLEX &&
                   entry->ingress == true &&
                   key_fields->id == lt_info->instance_fid) {
                   entry->inst = key_fields->data;
        } else if (entry->comp == STATE_EFLEX &&
                   entry->ingress == false &&
                   key_fields->id == lt_info->instance_fid) {
                   entry->egr_inst = key_fields->data;
        }
        key_fields = key_fields->next;
    }

    /* Parse data fields */
    while (data_fields) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_obj_quant_lt_fields_check(unit,
                                                        entry,
                                                        data_fields));

        data_fields = data_fields->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse OBJ_QUANTIZATION IMM entry */
static int
bcmcth_ctr_eflex_obj_quant_entry_parse(
    int unit,
    ctr_eflex_obj_quant_data_t *entry,
    bcmltd_fields_t *flist)
{
    uint32_t i;
    bcmltd_field_t *data_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* extract key and data fields */
    for (i = 0; i < flist->count; i++) {
        data_fields = flist->field[i];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_obj_quant_lt_fields_check(unit,
                                                        entry,
                                                        data_fields));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to lookup OBJ_QUANTIZATION table
 * with given pipe index and flex state ingress instance as key fields .
 * It populates the LT entry with all fields except the key field
 * which is provided within the entry as input.
 */
static int
bcmcth_ctr_eflex_obj_quant_lookup(int unit,
                                  ctr_eflex_obj_quant_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0, num_key_fields;
    int num_objs,num_global_objs;
    uint32_t t, pipe_idx;
    uint8_t pipe_unique, count = 0;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    eflex_comp_t comp;
    state_eflex_inst_t inst;
    state_eflex_egr_inst_t egr_inst;
    uint32_t egr_num_instances;
    ctr_eflex_obj_quant_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    comp = entry->comp;

    /* Get pipe index & flex state instance for current entry */
    inst = entry->inst;
    egr_inst = entry->egr_inst;
    pipe_idx = entry->pipe_idx;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_objs = device_info->num_objs;
    num_global_objs = device_info->num_global_objs;
    egr_num_instances = device_info->state_ext.num_egress_instance;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Restore pipe index & flex state instance for entry */
    egr_inst = entry->egr_inst;
    inst = entry->inst;

    /* Get LT sid and fid info */
    lt_info = entry->ingress ? &ctrl->ing_obj_quant_lt_info :
                               &ctrl->egr_obj_quant_lt_info;

    /* Assign default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_default_get(unit, entry));
    entry->inst = inst;
    entry->egr_inst = egr_inst;
    entry->pipe_idx = pipe_idx;

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid == lt_info->lt_sid) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "OBJ_QUANTIZATION LT info not found.\n")));
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
            key_fields.field[count]->id = lt_info->pipe_idx_fid;
            key_fields.field[count++]->data = entry->pipe_idx;
        }

        if (comp == STATE_EFLEX && entry->ingress == true) {
            key_fields.field[count]->id = lt_info->instance_fid;
            key_fields.field[count++]->data = entry->inst;
        }

        if (comp == STATE_EFLEX && entry->ingress == false &&
                                              (egr_num_instances > 1)) {
            key_fields.field[count]->id = lt_info->instance_fid;
            key_fields.field[count++]->data = entry->egr_inst;
        }
    }

    assert(count == num_key_fields);

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, lt_info->lt_sid, &num_fields));

    /*
     * Adjust num_fields for array size
     * since LRD API counts field array size as 1
     *
     * num_fields contains key fields count as well.
     * There are 2 array fields QUANTIZE and OBJ each of size num_objs.
     */
    num_fields -= num_key_fields;
    num_fields += ((num_objs - 1) * 2);

   num_fields += num_global_objs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             lt_info->lt_sid,
                             &key_fields,
                             &imm_fields));

    /* Parse IMM entry fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_entry_parse(unit, entry, &imm_fields));

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to insert entry into OBJ_QUANTIZATION IMM LT */
static int
bcmcth_ctr_eflex_obj_quant_lt_entry_insert(
    int unit,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    ctr_eflex_obj_quant_data_t *entry)
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
        (bcmcth_ctr_eflex_obj_quant_default_get(unit, entry));

    /*
     * Parse user values
     * default values will be over-written by user values, if present
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_lt_fields_parse(unit,
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
            "obj[0] %d, obj[1] %d, obj[2] %d, obj[3] %d\n"
            "quantize[0] %d, quantize[1] %d, quantize[2] %d, quantize[3] %d\n"
            "obj_0_1_mode %d, obj_2_3_mode %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            pipe_unique, entry->pipe_idx, entry->inst,
            entry->obj[0], entry->obj[1], entry->obj[2], entry->obj[3],
            entry->quantize[0], entry->quantize[1],
            entry->quantize[2], entry->quantize[3],
            entry->obj_0_1_mode, entry->obj_2_3_mode));

    /* Write to obj quantization PT */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_obj_quant_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_obj_quant_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to update entry into OBJ_QUANTIZATION IMM LT */
static int
bcmcth_ctr_eflex_obj_quant_lt_entry_update(
    int unit,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    ctr_eflex_obj_quant_data_t *entry,
    ctr_eflex_obj_quant_data_t *cur_entry)
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
        (bcmcth_ctr_eflex_obj_quant_lt_fields_parse(unit,
                                                    key_fields,
                                                    data_fields,
                                                    entry));
    cur_entry->inst = entry->inst;
    cur_entry->egr_inst = entry->egr_inst;
    cur_entry->pipe_idx = entry->pipe_idx;

    /* Lookup entry with given key fields */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_lookup(unit, cur_entry));

    /* Fill missing fields in new entry with current entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_fill(unit, entry, cur_entry));

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
            "obj[0] %d, obj[1] %d, obj[2] %d, obj[3] %d\n"
            "quantize[0] %d, quantize[1] %d, quantize[2] %d, quantize[3] %d\n"
            "obj_0_1_mode %d, obj_2_3_mode %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            pipe_unique, entry->pipe_idx, entry->inst,
            entry->obj[0], entry->obj[1], entry->obj[2], entry->obj[3],
            entry->quantize[0], entry->quantize[1],
            entry->quantize[2], entry->quantize[3],
            entry->obj_0_1_mode, entry->obj_2_3_mode));

    /* Write to obj quantization PT */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_obj_quant_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_obj_quant_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to delete entry from OBJ_QUANTIZATION IMM LT */
static int
bcmcth_ctr_eflex_obj_quant_lt_entry_delete(
    int unit,
    ctr_eflex_obj_quant_data_t *entry)
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
        (bcmcth_ctr_eflex_obj_quant_default_get(unit, entry));

    /* Restore flex state instance for entry */
    entry->inst = inst;
    entry->egr_inst = egr_inst;

    /* Write to range check profile table */
    if (entry->comp == CTR_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_obj_quant_write(unit, entry));
    } else if (entry->comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_state_eflex_obj_quant_write(unit, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to insert/update/delete OBJ_QUANTIZATION IMM LT entry. */
static int
bcmcth_ctr_eflex_obj_quant_lt_stage(int unit,
                                    bool ingress,
                                    eflex_comp_t comp,
                                    uint64_t req_flags,
                                    bcmimm_entry_event_t event_reason,
                                    const bcmltd_field_t *key_fields,
                                    const bcmltd_field_t *data_fields,
                                    bcmltd_fields_t *output_fields)
{
    ctr_eflex_obj_quant_data_t entry = {0};
    ctr_eflex_obj_quant_data_t cur_entry = {0};

    SHR_FUNC_ENTER(unit);

    entry.ingress = ingress;
    entry.comp = comp;
    entry.req_flags = req_flags;

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_entry_init(unit, &entry));

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_obj_quant_lt_entry_insert(unit,
                                                            key_fields,
                                                            data_fields,
                                                            &entry));
            break;

        case BCMIMM_ENTRY_UPDATE:
            cur_entry.ingress = ingress;
            cur_entry.comp = comp;
            cur_entry.req_flags = req_flags;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_obj_quant_entry_init(unit, &cur_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_obj_quant_lt_entry_update(unit,
                                                            key_fields,
                                                            data_fields,
                                                            &entry,
                                                            &cur_entry));
            break;

        case BCMIMM_ENTRY_DELETE:
            /* Parse user values */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_obj_quant_lt_fields_parse(unit,
                                                            key_fields,
                                                            data_fields,
                                                            &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_obj_quant_lt_entry_delete(unit,
                                                            &entry));
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();

}

/*! Function to get OBJ_QUANTIZATION IMM LT entry limit value. */
static int
bcmcth_ctr_eflex_obj_quant_lt_ent_limit_get(int unit,
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
    int rv;
    ctr_eflex_obj_quant_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

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

    /* Get LT sid and fid info */
    lt_info = ingress ? &ctrl->ing_obj_quant_lt_info :
                        &ctrl->egr_obj_quant_lt_info;



    /* Check if LT is supported for given device */
    rv = bcmlrd_map_get(unit, lt_info->lt_sid, &map);

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
            (BSL_META_U(unit,"OBJ_QUANTIZATION LT not supported."
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
 * \brief Stage callback function of CTR_ING_EFLEX_OBJ_QUANTIZATION
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
bcmcth_ctr_eflex_ing_obj_quant_lt_stage_cb(int unit,
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
        (bcmcth_ctr_eflex_obj_quant_lt_stage(unit,
                                             true,
                                             CTR_EFLEX,
                                             req_flags,
                                             event_reason,
                                             key_fields,
                                             data_fields,
                                             output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of CTR_ING_EFLEX_OBJ_QUANTIZATION
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_obj_quant_lt_ent_limit_get_cb(int unit,
                                                   uint32_t trans_id,
                                                   bcmltd_sid_t sid,
                                                   uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_lt_ent_limit_get(unit,
                                                     true,
                                                     CTR_EFLEX,
                                                     count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of FLEX_STATE_ING_OBJ_QUANTIZATION
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
bcmcth_state_eflex_ing_obj_quant_lt_stage_cb(int unit,
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
        (bcmcth_ctr_eflex_obj_quant_lt_stage(unit,
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
 * \brief Entry limit get callback function of FLEX_STATE_ING_OBJ_QUANTIZATION
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_obj_quant_lt_ent_limit_get_cb(int unit,
                                                     uint32_t trans_id,
                                                     bcmltd_sid_t sid,
                                                     uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_lt_ent_limit_get(unit,
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
 * \brief Stage callback function of CTR_EGR_EFLEX_OBJ_QUANTIZATION
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
bcmcth_ctr_eflex_egr_obj_quant_lt_stage_cb(int unit,
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
        (bcmcth_ctr_eflex_obj_quant_lt_stage(unit,
                                             false,
                                             CTR_EFLEX,
                                             req_flags,
                                             event_reason,
                                             key_fields,
                                             data_fields,
                                             output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of CTR_EGR_EFLEX_OBJ_QUANTIZATION
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_obj_quant_lt_ent_limit_get_cb(int unit,
                                                   uint32_t trans_id,
                                                   bcmltd_sid_t sid,
                                                   uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_lt_ent_limit_get(unit,
                                                     false,
                                                     CTR_EFLEX,
                                                     count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of FLEX_STATE_EGR_OBJ_QUANTIZATION
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
bcmcth_state_eflex_egr_obj_quant_lt_stage_cb(int unit,
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
        (bcmcth_ctr_eflex_obj_quant_lt_stage(unit,
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
bcmcth_state_eflex_egr_obj_quant_lt_ent_limit_get_cb(int unit,
                                                     uint32_t trans_id,
                                                     bcmltd_sid_t sid,
                                                     uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_lt_ent_limit_get(unit,
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
 * CTR_xxx_EFLEX_OBJ_QUANTIZATION LTs
 */
int
bcmcth_ctr_eflex_imm_obj_quant_register(int unit)
{
    bcmimm_lt_cb_t ctr_eflex_lt_cb;
    bool imm_reg = false;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Check if OBJ_QUANTIZATION handlers should be
     * registered with IMM. For some devices, OBJ_QUANTIZATION LTs
     * are implemented as direct mapped and hence do not require IMM support.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_obj_quant_imm_reg(unit, &imm_reg));

    if (imm_reg) {
        /* Get flex control struct */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

        /* Register IMM handlers only if OBJ_QUANTIZATION LT is supported*/
        if (ctrl->ing_obj_quant_lt_info.supported) {
            /* Register CTR_ING_EFLEX_OBJ_QUANTIZATION handler */
            sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
            ctr_eflex_lt_cb.op_stage =
                bcmcth_ctr_eflex_ing_obj_quant_lt_stage_cb;
            ctr_eflex_lt_cb.ent_limit_get =
                bcmcth_ctr_eflex_ing_obj_quant_lt_ent_limit_get_cb;

            SHR_IF_ERR_VERBOSE_EXIT(
                bcmimm_lt_event_reg(unit,
                                    ctrl->ing_obj_quant_lt_info.lt_sid,
                                    &ctr_eflex_lt_cb,
                                    NULL));
        }

        if (ctrl->egr_obj_quant_lt_info.supported) {
            /* Register CTR_EGR_EFLEX_OBJ_QUANTIZATION handler */
            sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
            ctr_eflex_lt_cb.op_stage =
                bcmcth_ctr_eflex_egr_obj_quant_lt_stage_cb;
            ctr_eflex_lt_cb.ent_limit_get =
                bcmcth_ctr_eflex_egr_obj_quant_lt_ent_limit_get_cb;

            SHR_IF_ERR_VERBOSE_EXIT(
                bcmimm_lt_event_reg(unit,
                                    ctrl->egr_obj_quant_lt_info.lt_sid,
                                    &ctr_eflex_lt_cb,
                                    NULL));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to register callback handlers for
 * FLEX_STATE_xxx_OBJ_QUANTIZATION LTs
 */
int
bcmcth_state_eflex_imm_obj_quant_register(int unit)
{
    bcmimm_lt_cb_t state_eflex_lt_cb;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    /* Register IMM handlers only if OBJ_QUANTIZATION LT is supported*/
    if (ctrl->ing_obj_quant_lt_info.supported) {
        /* Register FLEX_STATE_ING_OBJ_QUANTIZATION handler */
        sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        state_eflex_lt_cb.op_stage =
            bcmcth_state_eflex_ing_obj_quant_lt_stage_cb;
        state_eflex_lt_cb.ent_limit_get =
            bcmcth_state_eflex_ing_obj_quant_lt_ent_limit_get_cb;

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_lt_event_reg(unit,
                                ctrl->ing_obj_quant_lt_info.lt_sid,
                                &state_eflex_lt_cb,
                                NULL));
    }

    /* Register IMM handlers only if OBJ_QUANTIZATION LT is supported*/
    if (ctrl->egr_obj_quant_lt_info.supported) {
        /* Register CTR_EGR_EFLEX_OBJ_QUANTIZATION handler */
        sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        state_eflex_lt_cb.op_stage =
            bcmcth_state_eflex_egr_obj_quant_lt_stage_cb;
        state_eflex_lt_cb.ent_limit_get =
            bcmcth_state_eflex_egr_obj_quant_lt_ent_limit_get_cb;

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_lt_event_reg(unit,
                                ctrl->egr_obj_quant_lt_info.lt_sid,
                                &state_eflex_lt_cb,
                                NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
