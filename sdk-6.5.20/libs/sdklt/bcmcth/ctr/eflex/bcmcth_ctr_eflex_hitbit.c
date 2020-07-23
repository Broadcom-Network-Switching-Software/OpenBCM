/*! \file bcmcth_ctr_eflex_hitbit.c
 *
 * BCMCTH CTR_EFLEX_HITBIT_CONTROLt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*! Generate CTR_xxx_EFLEX_HITBIT_CONTROL LT field ids. */
#define CTR_ING_EGR_HITBIT_CONTROL_FIDS(x) \
    CTR_ING_EFLEX_HITBIT_CONTROLt_##x, \
    CTR_EGR_EFLEX_HITBIT_CONTROLt_##x

/*******************************************************************************
 * Local definitions
 */

/* HITBIT_CONTROL LT field info */
static const bcmlrd_sid_t hitbit_ctrl_table_sid[][2] = {
        {CTR_ING_EGR_LT_IDS(EFLEX_HITBIT_CONTROLt)},
        {0, 0},
};

static const bcmlrd_fid_t table_id_fid[][2] = {
        {CTR_ING_EGR_HITBIT_CONTROL_FIDS(TABLE_IDf)},
        {0, 0},
};

static const bcmlrd_fid_t pipe_idx_fid[][2] = {
        {CTR_ING_EGR_HITBIT_CONTROL_FIDS(PIPEf)},
        {0, 0},
};

static const bcmlrd_fid_t dst_grp_action_profile_id_fid[][2] = {
    {CTR_ING_EFLEX_HITBIT_CONTROLt_DST_CTR_ING_EFLEX_GROUP_ACTION_PROFILE_IDf,
     CTR_EGR_EFLEX_HITBIT_CONTROLt_DST_CTR_EGR_EFLEX_GROUP_ACTION_PROFILE_IDf},
    {0, 0},
};

static const bcmlrd_fid_t src_grp_action_profile_id_fid[][2] = {
    {CTR_ING_EFLEX_HITBIT_CONTROLt_SRC_CTR_ING_EFLEX_GROUP_ACTION_PROFILE_IDf,
     CTR_EGR_EFLEX_HITBIT_CONTROLt_SRC_CTR_EGR_EFLEX_GROUP_ACTION_PROFILE_IDf},
    {0, 0},
};

static const bcmlrd_fid_t dst_action_profile_id_fid[][2] = {
    {CTR_ING_EFLEX_HITBIT_CONTROLt_DST_CTR_ING_EFLEX_ACTION_PROFILE_IDf,
     CTR_EGR_EFLEX_HITBIT_CONTROLt_DST_CTR_EGR_EFLEX_ACTION_PROFILE_IDf},
    {0, 0},
};

static const bcmlrd_fid_t src_action_profile_id_fid[][2] = {
    {CTR_ING_EFLEX_HITBIT_CONTROLt_SRC_CTR_ING_EFLEX_ACTION_PROFILE_IDf,
     CTR_EGR_EFLEX_HITBIT_CONTROLt_SRC_CTR_EGR_EFLEX_ACTION_PROFILE_IDf},
    {0, 0},
};

static const bcmlrd_fid_t oper_state_fid[][2] = {
        {CTR_ING_EGR_HITBIT_CONTROL_FIDS(OPERATIONAL_STATEf)},
        {0, 0},
};

/* Shift factor lookup array for each Hash entry width */
static uint32_t shift_factor[BCMPTM_RM_HASH_ENTRY_WIDTH_COUNT] = {
    2, /* SINGLE */
    1, /* DOUBLE */
    0, /* QUAD */
    3, /* HALF */
    4  /* THIRD */
};

/**********************************************************************
* Private functions
 */
/* Function to initialize HITBIT_CONTROL entry with invalid values */
static int
bcmcth_ctr_eflex_hitbit_ctrl_entry_init(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry)
{
    SHR_FUNC_ENTER(unit);

    entry->dst_grp_action_profile_id = CTR_EFLEX_INVALID_VAL;
    entry->src_grp_action_profile_id = CTR_EFLEX_INVALID_VAL;
    entry->dst_action_profile_id = CTR_EFLEX_INVALID_VAL;
    entry->src_action_profile_id = CTR_EFLEX_INVALID_VAL;

    SHR_FUNC_EXIT();
}

/* Function to get default values for HITBIT_CONTROL entry */
static int
bcmcth_ctr_eflex_hitbit_ctrl_default_get(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info = NULL;
    size_t num_fid = 0, i;
    int select;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select  = entry->ingress ? 0 : 1;
    comp = entry->comp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                hitbit_ctrl_table_sid[comp][select],
                                &num_fid));

    lt_info = bcmlrd_table_get(hitbit_ctrl_table_sid[comp][select]);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmcthCtrEflexHitbitAllocFields");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fid,
                                     f_info,
                                     &num_fid));

    for (i = 0; i < num_fid; i++) {
        if (f_info[i].id == dst_grp_action_profile_id_fid[comp][select]) {
            entry->dst_grp_action_profile_id = f_info[i].def.u16;
        } else if (f_info[i].id ==
                   src_grp_action_profile_id_fid[comp][select]) {
            entry->src_grp_action_profile_id = f_info[i].def.u16;
        } else if (f_info[i].id == dst_action_profile_id_fid[comp][select]) {
            entry->dst_action_profile_id = f_info[i].def.u16;
        } else if (f_info[i].id == src_action_profile_id_fid[comp][select]) {
            entry->src_action_profile_id = f_info[i].def.u16;
        }
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

/*
 * Function to replace HITBIT_CONTROL entry with values from another
 * entry if is invalid
 */
static int
bcmcth_ctr_eflex_hitbit_ctrl_fill(int unit,
                              ctr_eflex_hitbit_ctrl_data_t *entry,
                              ctr_eflex_hitbit_ctrl_data_t *cur_entry)
{
    SHR_FUNC_ENTER(unit);

    if (entry->dst_grp_action_profile_id == CTR_EFLEX_INVALID_VAL) {
        entry->dst_grp_action_profile_id = cur_entry->dst_grp_action_profile_id;
    }

    if (entry->src_grp_action_profile_id == CTR_EFLEX_INVALID_VAL) {
        entry->src_grp_action_profile_id = cur_entry->src_grp_action_profile_id;
    }

    if (entry->dst_action_profile_id == CTR_EFLEX_INVALID_VAL) {
        entry->dst_action_profile_id = cur_entry->dst_action_profile_id;
    }

    if (entry->src_action_profile_id == CTR_EFLEX_INVALID_VAL) {
        entry->src_action_profile_id = cur_entry->src_action_profile_id;
    }

    SHR_FUNC_EXIT();
}

/* Function to parse data fields in given HITBIT_CONTROL entry */
static int
bcmcth_ctr_eflex_hitbit_ctrl_lt_fields_check(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    const bcmltd_field_t *data_fields)
{
    int select;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    if (data_fields->id == table_id_fid[comp][select]) {
        entry->lt_id = data_fields->data;
    } else if (entry->pipe_unique &&
               data_fields->id == pipe_idx_fid[comp][select]) {
        entry->pipe_idx = data_fields->data;
    } else if (data_fields->id == dst_grp_action_profile_id_fid[comp][select]) {
        entry->dst_grp_action_profile_id = data_fields->data;
    } else if (data_fields->id == src_grp_action_profile_id_fid[comp][select]) {
        entry->src_grp_action_profile_id = data_fields->data;
    } else if (data_fields->id == dst_action_profile_id_fid[comp][select]) {
        entry->dst_action_profile_id = data_fields->data;
    } else if (data_fields->id == src_action_profile_id_fid[comp][select]) {
        entry->src_action_profile_id = data_fields->data;
    } else if (data_fields->id == oper_state_fid[comp][select]) {
        entry->oper_state = data_fields->data;
    }

    SHR_FUNC_EXIT();
}

/* Function to parse given HITIBIT_CONTROL entry */
static int
bcmcth_ctr_eflex_hitbit_ctrl_lt_fields_parse(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_hitbit_ctrl_data_t *entry)
{
    SHR_FUNC_ENTER(unit);

    /* Parse data fields */
    while (data_fields) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_hitbit_ctrl_lt_fields_check(unit,
                                                          entry,
                                                          data_fields));

        data_fields = data_fields->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse HITIBIT_CONTROL IMM entry */
static int
bcmcth_ctr_eflex_hitbit_ctrl_entry_parse(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    bcmltd_fields_t *flist)
{
    uint32_t i;
    bcmltd_field_t *data_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* extract data fields */
    for (i = 0; i < flist->count; i++) {
        data_fields = flist->field[i];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_hitbit_ctrl_lt_fields_check(unit,
                                                          entry,
                                                          data_fields));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to remove hitbit control info from internal software state */
static int
bcmcth_ctr_eflex_hitbit_ctrl_info_remove(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry)
{
    int idx;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    hitbit_lt_info = entry->ingress ? ctrl->ing_hitbit_lt_info :
                                      ctrl->egr_hitbit_lt_info;

    /* Find LT Id entry */
    for (idx = 0; idx < HITBIT_LT_NUM; idx++) {
        if (hitbit_lt_info[idx].lt_id == entry->lt_id &&
            hitbit_lt_info[idx].pipe_idx == entry->pipe_idx) {
            /* Clear hitbit lt info */
            sal_memset(&hitbit_lt_info[idx], 0,
                       sizeof(hitbit_lt_info[idx]));

            /* Set lt id to invalid value */
            hitbit_lt_info[idx].lt_id =
                ctrl->ctr_eflex_enum.invalid_lt_id;
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "Clear Hitbit LT info idx %d\n"), idx));
            break;
        }
    }

    if (idx == HITBIT_LT_NUM) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Hitbit entry LT id %d not found.\n"),
                              entry->lt_id));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to update OPERATIONAL_STATE in HITBIT_CONTROL IMM LT */
static int
bcmcth_ctr_eflex_hitbit_ctrl_oper_state_update(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0, num_key_fields, num_readonly_fields;
    int select;
    uint32_t t;
    ctr_eflex_control_t *ctrl = NULL;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);
    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
                hitbit_ctrl_table_sid[comp][select]) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "HITBIT_CONTROL LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;
    num_readonly_fields = ctrl->ctr_eflex_field_info[t].num_read_only_fields;

    /* Allocate key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &key_fields,
                                       (num_key_fields + num_readonly_fields)));

    key_fields.count = num_key_fields;
    key_fields.field[0]->id = table_id_fid[comp][select];
    key_fields.field[0]->data = entry->lt_id;
    if (entry->pipe_unique) {
        key_fields.field[1]->id = pipe_idx_fid[comp][select];
        key_fields.field[1]->data = entry->pipe_idx;
    }

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                hitbit_ctrl_table_sid[comp][select],
                                &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             hitbit_ctrl_table_sid[comp][select],
                             &key_fields,
                             &imm_fields));

    /* Set the fields.count to include OPERATIONAL_STATE */
    key_fields.count = num_key_fields + num_readonly_fields;
    key_fields.field[1]->id = oper_state_fid[comp][select];
    key_fields.field[1]->data = entry->oper_state;

    /* Update IMM entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_update(unit,
                             0,
                             hitbit_ctrl_table_sid[comp][select],
                             &key_fields));

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/*
 * Function to lookup HITIBIT_CONTROL table.
 * It populates the LT entry with all fields except the key field
 * which is provided within the entry as input.
 */
static int
bcmcth_ctr_eflex_hitbit_ctrl_lookup(int unit,
                                    ctr_eflex_hitbit_ctrl_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0, num_key_fields;
    int select;
    uint32_t t;
    ctr_eflex_control_t *ctrl = NULL;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Assign default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_default_get(unit, entry));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            hitbit_ctrl_table_sid[comp][select]) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "HITBIT_CONTROL LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;

    /* Allocate key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &key_fields,
                                       num_key_fields));

    key_fields.count = num_key_fields;
    key_fields.field[0]->id = table_id_fid[comp][select];
    key_fields.field[0]->data = entry->lt_id;
    if (entry->pipe_unique) {
        key_fields.field[1]->id = pipe_idx_fid[comp][select];
        key_fields.field[1]->data = entry->pipe_idx;
    }

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                hitbit_ctrl_table_sid[comp][select],
                                &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             hitbit_ctrl_table_sid[comp][select],
                             &key_fields,
                             &imm_fields));

    /* Parse IMM entry fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_entry_parse(unit, entry, &imm_fields));

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to help insert entry for HITIBIT_CONTROL LT */
static int
bcmcth_ctr_eflex_hitbit_ctrl_insert_helper(
    int unit,
    bool notify,
    uint32_t skip_index,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_group_action_data_t *grp_action_entry)
{
    int num_lookups = 2;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info = NULL;
    bcmptm_rm_hash_entry_width_t entry_width = 0;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    /* Add hitbit control info to internal software state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_info_add(unit,
                                               notify,
                                               skip_index,
                                               ctrl,
                                               entry,
                                               grp_action_entry,
                                               &hitbit_lt_info));

    /*
     * If hitbit is not configured (i.e. counter move),
     * then number of lookups is 1.
     */
    if (!hitbit_lt_info->is_hitbit[DST_LKUP_IDX] &&
        !hitbit_lt_info->is_hitbit[SRC_LKUP_IDX]) {
        num_lookups = 1;
    }

    /* Call BCMPTM RM API to report entry move for LT id */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_hash_hit_index_callback_register(
            unit,
            hitbit_lt_info->lt_id,
            bcmcth_ctr_eflex_hash_entry_index_cb,
            true,
            num_lookups,
            hitbit_lt_info->enable,
            (void *)hitbit_lt_info,
            &entry_width));

    /* Save shift factor based on entry width lookup */
    if (entry_width >= BCMPTM_RM_HASH_ENTRY_WIDTH_COUNT) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "For lt id %d, invalid entry width %d provided \n"),
            hitbit_lt_info->lt_id, entry_width));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    hitbit_lt_info->shift_factor = shift_factor[entry_width];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "For lt id %d,lkups %d enable dst %d src %d RM hash reporting\n"),
            hitbit_lt_info->lt_id,
            num_lookups,
            hitbit_lt_info->enable[DST_LKUP_IDX],
            hitbit_lt_info->enable[SRC_LKUP_IDX]));

exit:
    SHR_FUNC_EXIT();
}

/* Function to recover internal state from HITBIT_CONTROL LT during warmboot */
int
bcmcth_ctr_eflex_hitbit_ctrl_state_recover(int unit,
                                          bool ingress,
                                          eflex_comp_t comp)
{
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t key_fields = {0};
    size_t num_fields = 0, num_key_fields;
    int rv, select;
    uint32_t t;
    uint8_t pipe_unique;
    ctr_eflex_hitbit_ctrl_data_t entry = {0};
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit, ingress, comp, &pipe_unique));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            hitbit_ctrl_table_sid[comp][select]) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "HITBIT_CONTROL LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;

    /* Allocate key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &key_fields,
                                       num_key_fields));

    key_fields.count = num_key_fields;
    key_fields.field[0]->id = table_id_fid[comp][select];
    if (pipe_unique) {
        key_fields.field[1]->id = pipe_idx_fid[comp][select];
    }

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                hitbit_ctrl_table_sid[comp][select],
                                &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* Get first entry from HITBIT_CONTROL IMM LT */
    rv = bcmimm_entry_get_first(unit,
                                hitbit_ctrl_table_sid[comp][select],
                                &imm_fields);

    while (rv == SHR_E_NONE) {
        sal_memset(&entry, 0, sizeof(ctr_eflex_hitbit_ctrl_data_t));
        entry.ingress = ingress;
        entry.comp = comp;
        entry.pipe_unique = pipe_unique;

        /* Assign default values */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_hitbit_ctrl_default_get(unit, &entry));

        /* Parse IMM entry fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_hitbit_ctrl_entry_parse(unit,
                                                      &entry,
                                                      &imm_fields));

        /*
         * If entry is ACTIVE then,
         * add new hitbit control info to internal software state
         */
        if (entry.oper_state == CTR_EFLEX_OPER_STATE_ACTIVE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_hitbit_ctrl_insert_helper(
                    unit,
                    false,
                    CTR_EFLEX_INVALID_VAL,
                    &entry,
                    NULL));
        }

        /* Setup key for next entry */
        key_fields.field[0]->data = entry.lt_id;
        if (pipe_unique) {
            key_fields.field[1]->data = entry.pipe_idx;
        }

        /* imm_fields.count may be modified. */
        imm_fields.count = num_fields;

        /* Get next entry from HITBIT_CONTROL IMM LT */
        rv = bcmimm_entry_get_next(unit,
                                   hitbit_ctrl_table_sid[comp][select],
                                   &key_fields,
                                   &imm_fields);
    } /* end WHILE */

    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified. */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to insert entry into HITIBIT_CONTROL IMM LT */
static int
bcmcth_ctr_eflex_hitbit_ctrl_lt_entry_insert(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_hitbit_ctrl_data_t *entry)
{
    SHR_FUNC_ENTER(unit);

    /* Get default values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_default_get(unit, entry));

    /*
     * Parse user values
     * default values will be over-written by user values, if present
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lt_fields_parse(unit,
                                                      data_fields,
                                                      entry));

    /* Add new hitbit control info to internal software state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_insert_helper(unit,
                                                    false,
                                                    CTR_EFLEX_INVALID_VAL,
                                                    entry,
                                                    NULL));

exit:
    SHR_FUNC_EXIT();
}

/* Function to delete entry from HITIBIT_CONTROL IMM LT */
static int
bcmcth_ctr_eflex_hitbit_ctrl_lt_entry_delete(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry)
{
    bool enable[2] = {0};

    SHR_FUNC_ENTER(unit);

    /* Lookup HITIBIT_CONTROL entry to get group action profile id */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lookup(unit, entry));

    /* If entry is INACTIVE, no further action is needed */
    if (entry->oper_state == CTR_EFLEX_OPER_STATE_INACTIVE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "HITIBIT_CONTROL entry is INACTIVE.\n")));

        SHR_EXIT();
    }

    /* Call BCMPTM RM API to cancel reporting of entry move for LT id */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_hash_hit_index_callback_register(
            unit,
            entry->lt_id,
            NULL,
            false,
            2,
            enable,
            NULL,
            NULL));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "For lt id %d, disable dst & src RM hash reporting\n"),
            entry->lt_id));

    /* Remove hitbit control info from internal software state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_info_remove(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

/* Function to update entry into HITIBIT_CONTROL IMM LT */
static int
bcmcth_ctr_eflex_hitbit_ctrl_lt_entry_update(
    int unit,
    const bcmltd_field_t *data_fields,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_hitbit_ctrl_data_t *cur_entry)
{
    uint32_t skip_index = CTR_EFLEX_INVALID_VAL;
    SHR_FUNC_ENTER(unit);

    /* Parse user values */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lt_fields_parse(unit,
                                                      data_fields,
                                                      entry));

    cur_entry->lt_id = entry->lt_id;
    if (entry->pipe_unique) {
        cur_entry->pipe_idx = entry->pipe_idx;
    }

    /* Lookup entry with given key fields to get action profile id */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lookup(unit, cur_entry));

    /* Fill missing fields in new entry with current entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_fill(unit, entry, cur_entry));

    /* Get dst and/or src profile id to be skipped */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_skip_idx_get(unit,
                                                   entry,
                                                   cur_entry,
                                                   &skip_index));

    /* Add new hitbit control info to internal software state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_insert_helper(unit,
                                                    false,
                                                    skip_index,
                                                    entry,
                                                    NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to check if HITBIT_CONTROL entry
 * should be updated based on GROUP_ACTION_PROFILE LT entry OR
 * ACTION_PROFILE LT entry insert/delete operation.
 *
 * HITBIT_CONTROL entry includes dst and src group/non-group action profile ids.
 *
 * This function may be called after GROUP_ACTION_PROFILE / ACTION_PROFILE entry
 * insert/delete operation for either one.
 *
 * This function iterates over each HITBIT_CONTROL entry searching for a match
 * with group action profile id in input GROUP_ACTION_PROFILE/ACTION_PROFILE
 * entry.
 *
 * For INSERT notification, the current OPERATIONAL_STATE must be INACTIVE.
 * If group action profile id matches either dst or src group action profile id,
 * then
 *      - Call insert_helper() routine which will
 *          -Check if the other id is valid & present in GROUP_ACTION_PROFILE LT
 *          -If found, then
 *                  - save hitbit control info
 *                  - Call BCMPTM RM API to enable reporting for LT entry move.
 *                  - Update OPERATIONAL_STATE to ACTIVE
 *                  - return SUCCESS.
 *          -If not found, then
 *                  - Do NOT Update OPERATIONAL_STATE
 *                  - return SUCCESS
 *
 * For DELETE notification, the current OPERATIONAL_STATE must be ACTIVE.
 * If group action profile id matches either dst or src group action profile id,
 * then
 *      - Call entry_delete() routine which will
 *          - remove hitbit control info
 *          - Call BCMPTM RM API to disable reporting for LT entry move.
 *          - return SUCCESS.
 *      - Update OPERATIONAL_STATE to INACTIVE
 */
int
bcmcth_ctr_eflex_hitbit_ctrl_notify(
    int unit,
    bool insert,
    bool grp_valid,
    void *context)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    ctr_eflex_hitbit_ctrl_data_t entry = {0};
    size_t num_fields = 0, num_key_fields;
    uint32_t skip_index, t;
    uint8_t pipe_unique;
    int select, rv, insert_rv;
    bool profile_id_match, cur_oper_state, ingress;
    ctr_eflex_control_t *ctrl = NULL;
    eflex_comp_t comp;
    const bcmlrd_map_t *map = NULL;
    ctr_eflex_group_action_data_t *grp_action_entry = NULL;
    ctr_eflex_action_profile_data_t *action_entry = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(context, SHR_E_PARAM);

    /* Check if context contains GROUP_ACTION_PROFILE or ACTION_PROFILE entry */
    if (grp_valid) {
        grp_action_entry = (ctr_eflex_group_action_data_t *)context;
        select = grp_action_entry->ingress ? 0 : 1;
        comp = grp_action_entry->comp;
        ingress = grp_action_entry->ingress;
    } else {
        action_entry = (ctr_eflex_action_profile_data_t *)context;
        select = action_entry->ingress ? 0 : 1;
        comp = action_entry->comp;
        ingress = action_entry->ingress;
    }

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit, ingress, comp, &pipe_unique));

    /* Check if LT is supported for given device */
    rv = bcmlrd_map_get(unit,
                        hitbit_ctrl_table_sid[comp][select],
                        &map);

    if (SHR_FAILURE(rv) && !map) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"HITBIT_CONTROL LT not supported.\n")));
        SHR_EXIT();
    }

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /*
     * If insert, update HITBIT_CONTROL entry if it is INACTIVE
     * If delete, update HITBIT_CONTROL entry if it is ACTIVE
     */
    cur_oper_state = insert ? CTR_EFLEX_OPER_STATE_INACTIVE :
                              CTR_EFLEX_OPER_STATE_ACTIVE;

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            hitbit_ctrl_table_sid[comp][select]) {
            break;
        }
    }

    if (t == ctrl->lt_info_num) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "HITBIT_CONTROL LT info not found.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_key_fields = ctrl->ctr_eflex_field_info[t].num_key_fields;

    /* Allocate key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &key_fields,
                                       num_key_fields));

    key_fields.count = num_key_fields;
    key_fields.field[0]->id = table_id_fid[comp][select];
    if (pipe_unique) {
        key_fields.field[1]->id = pipe_idx_fid[comp][select];
    }

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                hitbit_ctrl_table_sid[comp][select],
                                &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* Get first entry from HITBIT_CONTROL IMM LT */
    rv = bcmimm_entry_get_first(unit,
                                hitbit_ctrl_table_sid[comp][select],
                                &imm_fields);

    /* Iterate HITBIT_CONTROL LT searching for input action profile id */
    while (rv == SHR_E_NONE) {
        sal_memset(&entry, 0, sizeof(ctr_eflex_hitbit_ctrl_data_t));
        profile_id_match = false;
        skip_index = CTR_EFLEX_INVALID_VAL;
        entry.ingress = ingress;
        entry.comp = comp;
        entry.pipe_unique = pipe_unique;
        if (!entry.pipe_unique) {
            entry.pipe_idx = ALL_PIPES;
        }
        insert_rv = SHR_E_NONE;

        /* Parse IMM entry fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_hitbit_ctrl_entry_parse(unit,
                                                      &entry,
                                                      &imm_fields));

        /* HITBIT_CONTROL LT entry OPERATIONAL_STATE is expected value */
        if (entry.oper_state == cur_oper_state) {
            if (grp_valid) {
                /*
                 * Check if incoming GROUP_ACTION_PROFILE id
                 * matches dst or src HITBIT_CONTROL group action profile id
                 */
                if (entry.dst_grp_action_profile_id ==
                    grp_action_entry->group_profile_id) {
                    profile_id_match = true;
                    skip_index = DST_LKUP_IDX;
                } else if (entry.src_grp_action_profile_id ==
                           grp_action_entry->group_profile_id) {
                    profile_id_match = true;
                    skip_index = SRC_LKUP_IDX;
                }
            } else {
                /*
                 * Check if incoming ACTION_PROFILE id
                 * matches dst or src HITBIT_CONTROL action profile id
                 */
                if (entry.dst_action_profile_id ==
                    action_entry->action_profile_id) {
                    profile_id_match = true;
                    skip_index = DST_LKUP_IDX;
                } else if (entry.src_action_profile_id ==
                           action_entry->action_profile_id) {
                    profile_id_match = true;
                    skip_index = SRC_LKUP_IDX;
                }
            }

            /* Profile id match found */
            if (profile_id_match) {
                if (insert) {
                    /* Insert notification */
                    insert_rv = bcmcth_ctr_eflex_hitbit_ctrl_insert_helper(
                                    unit,
                                    true,
                                    skip_index,
                                    &entry,
                                    grp_action_entry);

                    /*
                     * If insert was not successful, then it indicates that
                     * the other action profile id also does not exist.
                     * Hence do NOT change OPERATIONAL_STATE as ACTIVE.
                     */
                    if (insert_rv != SHR_E_NONE) {
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                "Insert failed due to other group action "
                                "profile id not present\n")));
                        SHR_EXIT();
                    }

                    /*
                     * Update HITBIT_CONTROL LT entry
                     * OPERATIONAL_STATE to ACTIVE
                     */
                    entry.oper_state = CTR_EFLEX_OPER_STATE_ACTIVE;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmcth_ctr_eflex_hitbit_ctrl_oper_state_update(
                            unit, &entry));

                } else {
                    /*
                     * Delete notification
                     * Incoming action profile id matches either dst or src
                     * action profile id.
                     * In either case, set OPERATIONAL_STATE to INACTIVE.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmcth_ctr_eflex_hitbit_ctrl_lt_entry_delete(unit,
                                                                      &entry));

                    /*
                     * Update HITBIT_CONTROL LT entry
                     * OPERATIONAL_STATE to INACTIVE
                     */
                    entry.oper_state = CTR_EFLEX_OPER_STATE_INACTIVE;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmcth_ctr_eflex_hitbit_ctrl_oper_state_update(
                            unit, &entry));
                }
            }
        }

        /* Setup key for next entry */
        key_fields.field[0]->data = entry.lt_id;
        if (pipe_unique) {
            key_fields.field[1]->data = entry.pipe_idx;
        }

        /* imm_fields.count may be modified. */
        imm_fields.count = num_fields;

        /* Get next entry from HITBIT_CONTROL IMM LT */
        rv = bcmimm_entry_get_next(unit,
                                   hitbit_ctrl_table_sid[comp][select],
                                   &key_fields,
                                   &imm_fields);
    } /* end WHILE */

    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

exit:
    bcmcth_ctr_eflex_fields_free(unit, &key_fields);
    /* imm_fields.count may be modified. */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);
    SHR_FUNC_EXIT();
}

/*! Function to insert/update/delete HITBIT_CONTROL IMM LT entry. */
static int
bcmcth_ctr_eflex_hitbit_ctrl_lt_stage(int unit,
                                      bool ingress,
                                      eflex_comp_t comp,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key_fields,
                                      const bcmltd_field_t *data_fields,
                                      bcmltd_fields_t *out)
{
    int select;
    ctr_eflex_hitbit_ctrl_data_t entry = {0};
    ctr_eflex_hitbit_ctrl_data_t cur_entry = {0};
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    entry.ingress = ingress;
    entry.comp = comp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_entry_init(unit, &entry));

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry.ingress,
                                            entry.comp,
                                            &entry.pipe_unique));

    /* Parse the key fields to get LT id and pipe, if applicable */
    while (key_fields) {
        if (key_fields->id == table_id_fid[comp][select]) {
            entry.lt_id = key_fields->data;
        } else if (entry.pipe_unique &&
              key_fields->id == pipe_idx_fid[comp][select]) {
            entry.pipe_idx = key_fields->data;
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,"Unmatched key id %d\n"), key_fields->id));
        }
        key_fields = key_fields->next;
    }

    if (!entry.pipe_unique) {
        entry.pipe_idx = cur_entry.pipe_idx = ALL_PIPES;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_hitbit_ctrl_lt_entry_insert(unit,
                                                              data_fields,
                                                              &entry));
            break;

        case BCMIMM_ENTRY_UPDATE:
            cur_entry.ingress = ingress;
            cur_entry.comp = comp;
            cur_entry.pipe_unique = entry.pipe_unique;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_hitbit_ctrl_entry_init(unit, &cur_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_hitbit_ctrl_lt_entry_update(unit,
                                                              data_fields,
                                                              &entry,
                                                              &cur_entry));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_hitbit_ctrl_lt_entry_delete(unit, &entry));
            break;

        default:
            break;
    }

exit:
    if (out) {
        out->count = 0;
        out->field[out->count]->id = oper_state_fid[comp][select];
        out->field[out->count]->flags = 0;
        out->field[out->count]->next = 0;
        out->field[out->count]->idx = 0;
        if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
            /* Set OPERATIONAL_STATE to INACTIVE . */
            out->field[out->count++]->data =
                                    ctrl->ctr_eflex_enum.oper_state_inactive;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Dst and/or src action profile Id entry not found in "
                    "GROUP ACTION_PROFILE LT. "
                    "Setting operational state to INACTIVE.\n")));
        } else {
            /* Set OPERATIONAL_STATE to ACTIVE . */
            out->field[out->count++]->data =
                                    ctrl->ctr_eflex_enum.oper_state_active;
        }
    }

    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        return SHR_E_NONE;
    }

    SHR_FUNC_EXIT();
}

/*! Function to lookup HITBIT_CONTROL IMM LT entry. */
static int
bcmcth_ctr_eflex_hitbit_ctrl_lt_lookup(int unit,
                                       bool ingress,
                                       eflex_comp_t comp,
                                       bcmimm_lookup_type_t lkup_type,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out)
{
    int select;
    size_t i;
    const bcmltd_fields_t *key_fields = NULL;
    ctr_eflex_hitbit_ctrl_data_t entry = {0};
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;
    entry.ingress = ingress;
    entry.comp = comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry.ingress,
                                            entry.comp,
                                            &entry.pipe_unique));

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* Parse the key fields to get pipe index if pipe unique is true */
    for (i = 0; i < key_fields->count; i++) {
        if (entry.pipe_unique &&
            key_fields->field[i]->id == pipe_idx_fid[comp][select]) {
            entry.pipe_idx = key_fields->field[i]->data;
        }
    }

    /* Parse IMM fields to get dst and src action profile ids */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_entry_parse(unit, &entry, out));

    /*
     * Check if corresponding GROUP_ACTION_PROFILE/ACTION_PROFILE IDs
     * are configured
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lookup_help(unit, &entry));

exit:
    if (out) {
        if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
            /* Set OPERATIONAL_STATE to INACTIVE . */
            (void)bcmcth_ctr_eflex_oper_state_set(
                    unit,
                    oper_state_fid[comp][select],
                    ctrl->ctr_eflex_enum.oper_state_inactive,
                    out);

            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Dst and/or src action profile Id entry not found in "
                    "ACTION_PROFILE LT.\n"
                    "Setting operational state to INACTIVE.\n")));
        } else {
            /* Set OPERATIONAL_STATE to ACTIVE . */
            (void)bcmcth_ctr_eflex_oper_state_set(
                    unit,
                    oper_state_fid[comp][select],
                    ctrl->ctr_eflex_enum.oper_state_active,
                    out);
        }
    }

    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        return SHR_E_NONE;
    }

    SHR_FUNC_EXIT();
}

/* Function to handle hash entry hitbit event from BCMPTM RM. */
static int
bcmcth_ctr_eflex_hash_hitbit_event_helper(
    int unit,
    uint32_t lkup_idx,
    uint32_t ctr_bit_idx_offset,
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info)
{
    bcmdrd_sid_t pool_sid = {0};
    uint32_t pool_idx, start_pipe_idx, end_pipe_idx, pipe_idx;
    uint32_t tmp_ctr_bit_idx, ctr_idx, bit_idx;
    uint32_t num_hitbit_ctr, max_ctr_num;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    /* Number of hitbits per counter */
    num_hitbit_ctr = device_info->num_hitbit_ctr;

    pool_idx = hitbit_lt_info->pool_idx[lkup_idx];

    /* Get max number of counters per pool */
    max_ctr_num = hitbit_lt_info->ingress ? ctrl->ing_pool_ctr_num[pool_idx] :
                                            ctrl->egr_pool_ctr_num[pool_idx];

    /* Calculate pool idx, ctr index and bit index */
    pool_idx = ctr_bit_idx_offset / (num_hitbit_ctr * max_ctr_num);
    tmp_ctr_bit_idx = ctr_bit_idx_offset -
                      (pool_idx * max_ctr_num * num_hitbit_ctr);
    ctr_idx = tmp_ctr_bit_idx / num_hitbit_ctr;
    bit_idx = tmp_ctr_bit_idx % num_hitbit_ctr;

    /* Get counter pool sid */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_pool_sid_get(unit,
                                   hitbit_lt_info->ingress,
                                   CTR_EFLEX,
                                   0,
                                   0,
                                   pool_idx,
                                   &pool_sid));

    /* Determine first and last pipe number */
    if (hitbit_lt_info->pipe_idx == ALL_PIPES) {
        start_pipe_idx = 0;
        end_pipe_idx = device_info->num_pipes;
    } else {
        start_pipe_idx = hitbit_lt_info->pipe_idx;
        end_pipe_idx = hitbit_lt_info->pipe_idx + 1;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "%s Entry:\n"
            "lt_id %d, lkup_idx %d, pool_idx %d, ctr_idx %d, bit_idx %d\n"
            "start_pipe_idx %d, end_pipe_idx %d\n"),
            hitbit_lt_info->ingress ? "Ingress" : "Egress",
            hitbit_lt_info->lt_id, lkup_idx,
            pool_idx, ctr_idx, bit_idx, start_pipe_idx, end_pipe_idx));

    /* Set hitbit for all applicable pipes */
    for (pipe_idx = start_pipe_idx; pipe_idx < end_pipe_idx; pipe_idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_cci_hit_bit_set(unit,
                                    pool_sid,
                                    pipe_idx,
                                    ctr_idx,
                                    bit_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Function to populate CCI counter info struct for counter move.
 *
 * \param [in] unit Unit number.
 * \param [in] lkup_idx Dst/Src lookup index.
 * \param [in] ctr_idx_offset Counter index offset.
 * \param [in] hitbit_lt_info Internal hitbit lt info.
 * \param [out] pt_dyn_info Dynamic PT info.
 * \param [out] ctr_info Dynamic counter info.
 *
 * The counter index offset provided by RM Hash in the event data is converted
 * into pool index, counter index and slim counter index (if required).
 * The dynamic PT info and CCI counter info structures are then populated
 * accordingly in the function.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmcth_ctr_eflex_move_event_info_get(
    int unit,
    uint32_t lkup_idx,
    uint32_t ctr_idx_offset,
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info,
    bcmbd_pt_dyn_info_t *pt_dyn_info,
    bcmptm_cci_ctr_info_t *ctr_info)
{
    bcmdrd_sid_t pool_sid = {0};
    uint32_t pool_idx, abs_ctr_idx;
    uint32_t tmp_ctr_idx, ctr_idx, bit_idx = 0;
    uint32_t num_slim_ctrs = 1, max_ctr_num;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    pool_idx = hitbit_lt_info->pool_idx[lkup_idx];

    /* Get max number of counters per pool */
    max_ctr_num = hitbit_lt_info->ingress ? ctrl->ing_pool_ctr_num[pool_idx] :
                                            ctrl->egr_pool_ctr_num[pool_idx];

    /* SLIM counter calculation */
    if (hitbit_lt_info->is_slim[lkup_idx]) {
        /* Get number of slim counters per pool entry */
        num_slim_ctrs = device_info->num_slim_ctrs;

        /* Calculate absolute counter index offset */
        abs_ctr_idx = (pool_idx * max_ctr_num * num_slim_ctrs) +
                      hitbit_lt_info->abs_base_idx[lkup_idx] +
                      ctr_idx_offset;

        /* Calculate pool idx, ctr index and slim ctr field (bit_index) */
        pool_idx = abs_ctr_idx / (max_ctr_num * num_slim_ctrs);
        tmp_ctr_idx =  abs_ctr_idx -
                       (pool_idx * max_ctr_num * num_slim_ctrs);
        bit_idx = tmp_ctr_idx / max_ctr_num;
        ctr_idx = tmp_ctr_idx % max_ctr_num;
    } else {
        /* Calculate absolute counter index offset */
        abs_ctr_idx = (pool_idx * max_ctr_num * num_slim_ctrs) +
                      hitbit_lt_info->abs_base_idx[lkup_idx] +
                      ctr_idx_offset;

        /* Calculate pool idx and ctr index */
        pool_idx = abs_ctr_idx / max_ctr_num;
        ctr_idx = abs_ctr_idx % max_ctr_num;
    }

    /* Get pool sid */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_pool_sid_get(unit,
                                   hitbit_lt_info->ingress,
                                   CTR_EFLEX,
                                   0,
                                   0,
                                   pool_idx,
                                   &pool_sid));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "%s Entry:\n"
            "lt_id %d, lkup_idx %s, is_slim %d "
            "pool_idx %d, ctr_idx %d, bit_idx %d\n"),
            hitbit_lt_info->ingress ? "Ingress" : "Egress",
            hitbit_lt_info->lt_id,
            (lkup_idx == DST_LKUP_IDX) ? "DST" : "SRC",
            hitbit_lt_info->is_slim[lkup_idx],
            pool_idx, ctr_idx, bit_idx));

    /* Populate info structures */
    pt_dyn_info->index = ctr_idx;
    ctr_info->sid = pool_sid;
    ctr_info->txfm_fld = bit_idx;

exit:
    SHR_FUNC_EXIT();
}

/* Function to handle hash entry counter move event from BCMPTM RM. */
static int
bcmcth_ctr_eflex_hash_move_event_helper(
    int unit,
    uint32_t dst_ctr_bit_idx_offset,
    uint32_t src_ctr_bit_idx_offset,
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info)
{
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    uint32_t start_pipe_idx, end_pipe_idx, pipe_idx;
    bcmbd_pt_dyn_info_t dst_pt_dyn_info = {0};
    bcmbd_pt_dyn_info_t src_pt_dyn_info = {0};
    bcmptm_cci_ctr_info_t dst_ctr_info = {0};
    bcmptm_cci_ctr_info_t src_ctr_info = {0};

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    /* Get DST counter info for move */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_move_event_info_get(unit,
                                              DST_LKUP_IDX,
                                              dst_ctr_bit_idx_offset,
                                              hitbit_lt_info,
                                              &dst_pt_dyn_info,
                                              &dst_ctr_info));

    /* Get SRC counter info for move */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_move_event_info_get(unit,
                                              SRC_LKUP_IDX,
                                              src_ctr_bit_idx_offset,
                                              hitbit_lt_info,
                                              &src_pt_dyn_info,
                                              &src_ctr_info));

    /* Determine first and last pipe number */
    if (hitbit_lt_info->pipe_idx == ALL_PIPES) {
        start_pipe_idx = 0;
        end_pipe_idx = device_info->num_pipes;
    } else {
        start_pipe_idx = hitbit_lt_info->pipe_idx;
        end_pipe_idx = hitbit_lt_info->pipe_idx + 1;
    }

    /* Move counter entry for all applicable pipes */
    for (pipe_idx = start_pipe_idx; pipe_idx < end_pipe_idx; pipe_idx++) {
        dst_pt_dyn_info.tbl_inst = src_pt_dyn_info.tbl_inst = pipe_idx;
        dst_ctr_info.in_pt_dyn_info = &dst_pt_dyn_info;
        src_ctr_info.in_pt_dyn_info = &src_pt_dyn_info;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_cci_ctr_flex_entry_move(unit,
                                            &src_ctr_info,
                                            &dst_ctr_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to get HITBIT_CONTROL IMM LT entry limit value. */
static int
bcmcth_ctr_eflex_hitbit_ctrl_lt_ent_limit_get(int unit,
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

    num_pipes = pipe_unique ? 1 : device_info->num_pipes;

    /* Check if LT is supported for given device */
    rv = bcmlrd_map_get(unit, hitbit_ctrl_table_sid[comp][select], &map);

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
            (BSL_META_U(unit,"HITBIT_CONTROL LT not supported.\n")));
        *count = 0;

    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,"comp %d ingress %d entry_limit %d\n"),
        comp, ingress, *count));

exit:
    SHR_FUNC_EXIT();
}

/***********************
 * IMM Functions
 */

/*!
 * \brief Stage callback function of CTR_ING_EFLEX_HITBIT_CONTROL to
 *        insert, update, delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
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
bcmcth_ctr_eflex_ing_hitbit_ctrl_lt_stage_cb(int unit,
                                             bcmltd_sid_t sid,
                                             uint32_t trans_id,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key_fields,
                                             const bcmltd_field_t *data_fields,
                                             void *context,
                                             bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lt_stage(unit,
                                               INGRESS,
                                               CTR_EFLEX,
                                               event_reason,
                                               key_fields,
                                               data_fields,
                                               output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of CTR_ING_EFLEX_HITBIT_CONTROL
 *  IMM event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Structure which contains the key fields but may include
 *                also other fields. The component should only focus on the
 *                key fields.
 * \param [in,out] out Structure contains all the fields of the table, so that
 *                     the component should not delete or add any field to this
 *                     structure. The data values of the field were set by the
 *                     IMM so the component may only overwrite the data section
 *                     of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_hitbit_ctrl_lt_lookup_cb(int unit,
                                              bcmltd_sid_t sid,
                                              uint32_t trans_id,
                                              void *context,
                                              bcmimm_lookup_type_t lkup_type,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out)
{
    uint32_t t;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            CTR_ING_EFLEX_HITBIT_CONTROLt) {
            break;
        }
    }

    if (out) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (out->count <
                   ctrl->ctr_eflex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "out fields supplied to lookup is too "
                                  "small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "out not supplied to lookup.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lt_lookup(unit,
                                                INGRESS,
                                                CTR_EFLEX,
                                                lkup_type,
                                                in,
                                                out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of CTR_ING_EFLEX_HITBIT_CONTROL
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_hitbit_ctrl_lt_ent_limit_get_cb(int unit,
                                                     uint32_t trans_id,
                                                     bcmltd_sid_t sid,
                                                     uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lt_ent_limit_get(unit,
                                                       INGRESS,
                                                       CTR_EFLEX,
                                                       count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of CTR_EGR_EFLEX_HITBIT_CONTROL to
 *        insert, update, delete IMM LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
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
bcmcth_ctr_eflex_egr_hitbit_ctrl_lt_stage_cb(int unit,
                                             bcmltd_sid_t sid,
                                             uint32_t trans_id,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key_fields,
                                             const bcmltd_field_t *data_fields,
                                             void *context,
                                             bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lt_stage(unit,
                                               EGRESS,
                                               CTR_EFLEX,
                                               event_reason,
                                               key_fields,
                                               data_fields,
                                               output_fields));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of CTR_EGR_EFLEX_HITBIT_CONTROL
 *  IMM event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Structure which contains the key fields but may include
 *                also other fields. The component should only focus on the
 *                key fields.
 * \param [in,out] out Structure contains all the fields of the table, so that
 *                     the component should not delete or add any field to this
 *                     structure. The data values of the field were set by the
 *                     IMM so the component may only overwrite the data section
 *                     of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_hitbit_ctrl_lt_lookup_cb(int unit,
                                              bcmltd_sid_t sid,
                                              uint32_t trans_id,
                                              void *context,
                                              bcmimm_lookup_type_t lkup_type,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out)
{
    uint32_t t;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(in, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid ==
            CTR_EGR_EFLEX_HITBIT_CONTROLt) {
            break;
        }
    }

    if (out) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (out->count <
                   ctrl->ctr_eflex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "out fields supplied to lookup is too "
                                  "small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "out not supplied to lookup.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lt_lookup(unit,
                                                EGRESS,
                                                CTR_EFLEX,
                                                lkup_type,
                                                in,
                                                out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of CTR_EGR_EFLEX_HITBIT_CONTROL
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_hitbit_ctrl_lt_ent_limit_get_cb(int unit,
                                                     uint32_t trans_id,
                                                     bcmltd_sid_t sid,
                                                     uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hitbit_ctrl_lt_ent_limit_get(unit,
                                                       EGRESS,
                                                       CTR_EFLEX,
                                                       count));

exit:
    SHR_FUNC_EXIT();
}

/**********************************************************************
* Public functions
 */

/* Function to calculate hit index for hash LTs */
int
bcmcth_ctr_eflex_hash_entry_index_cb(int unit,
                                     bcmltd_sid_t ltid,
                                     bcmdrd_sid_t sid,
                                     bcmptm_rm_hash_hit_context_t *hit_context,
                                     int tbl_inst,
                                     uint32_t bkt_num,
                                     uint32_t bkt_entry,
                                     void *context,
                                     uint32_t *entry_idx)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry_idx, SHR_E_PARAM);
    SHR_NULL_CHECK(context, SHR_E_PARAM);

    /* Calculate hash entry index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_hash_entry_index_calc(unit,
                                                ltid,
                                                sid,
                                                hit_context,
                                                tbl_inst,
                                                bkt_num,
                                                bkt_entry,
                                                context,
                                                entry_idx));

exit:
    SHR_FUNC_EXIT();
}

/* Function to handle hash entry move event from BCMPTM RM. */
void
bcmcth_ctr_eflex_hash_move_event_cb(int unit,
                                    const char *event,
                                    uint64_t ev_data)
{
    uint32_t lt_id, pipe_bmap, dst_ctr_bit_idx_offset, src_ctr_bit_idx_offset;
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info = NULL;

    bcmevm_extend_ev_data_t *event_d =
                                (bcmevm_extend_ev_data_t *)((uintptr_t)ev_data);

    if (event_d) {
        /* LT Id is stored as first event data */
        lt_id = (uint32_t)event_d->data[0];

        /* Pipe bitmap is stored as second event data */
        pipe_bmap = (uint32_t)event_d->data[1];

        /* Hitbit lt info is stored as third event data */
        hitbit_lt_info =
            (ctr_eflex_hitbit_lt_info_t *)((uintptr_t)event_d->data[2]);

        /* Dst counter offset is stored as fourth event data */
        dst_ctr_bit_idx_offset = (uint32_t)event_d->data[3];

        /* Src counter offset is stored as fifth event data */
        src_ctr_bit_idx_offset = (uint32_t)event_d->data[4];

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
            "LT %d entry move evt recvd with pipe %d dst idx %d, src idx %d\n"),
            lt_id, pipe_bmap, dst_ctr_bit_idx_offset, src_ctr_bit_idx_offset));

        if (lt_id != hitbit_lt_info->lt_id) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                "LT ID mismatch between evt %d and ctx %d\n"),
                lt_id, hitbit_lt_info->lt_id));
            return;
        }

        if (!hitbit_lt_info->is_hitbit[DST_LKUP_IDX] &&
            !hitbit_lt_info->is_hitbit[SRC_LKUP_IDX]) {
            if (src_ctr_bit_idx_offset == CTR_EFLEX_INVALID_VAL ||
                dst_ctr_bit_idx_offset == CTR_EFLEX_INVALID_VAL) {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "NOOP for LT %d counter move event recvd with dst "
                    "and/or src idx as -1 indicating insert event.\n"), lt_id));
                return;
            }

            /* Call CCI counter entry move API */
            (void)bcmcth_ctr_eflex_hash_move_event_helper(
                unit,
                dst_ctr_bit_idx_offset,
                src_ctr_bit_idx_offset,
                hitbit_lt_info);
        } else {
            /* Call CCI setbit API for destination hitbit entry move */
            if (dst_ctr_bit_idx_offset != CTR_EFLEX_INVALID_VAL &&
                hitbit_lt_info->enable[DST_LKUP_IDX] &&
                hitbit_lt_info->is_hitbit[DST_LKUP_IDX]) {
                (void)bcmcth_ctr_eflex_hash_hitbit_event_helper(
                    unit,
                    DST_LKUP_IDX,
                    dst_ctr_bit_idx_offset,
                    hitbit_lt_info);
            }

            /* Call CCI setbit API for source hitbit entry move */
            if (src_ctr_bit_idx_offset != CTR_EFLEX_INVALID_VAL &&
                hitbit_lt_info->enable[SRC_LKUP_IDX] &&
                hitbit_lt_info->is_hitbit[SRC_LKUP_IDX]) {
                (void)bcmcth_ctr_eflex_hash_hitbit_event_helper(
                    unit,
                    SRC_LKUP_IDX,
                    src_ctr_bit_idx_offset,
                    hitbit_lt_info);
            }
        }
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
            "No event data received\n")));
    }

    return;
}

/* Function to register callback handlers for CTR_xxx_EFLEX_HITBIT_CONTROL LT */
int
bcmcth_ctr_eflex_imm_hitbit_ctrl_register(int unit)
{
    int rv;
    const bcmlrd_map_t *ctr_eflex_map = NULL;
    bcmimm_lt_cb_t ctr_eflex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Check if LT is supported for given device */
    rv = bcmlrd_map_get(unit, CTR_ING_EFLEX_HITBIT_CONTROLt, &ctr_eflex_map);

    if (SHR_SUCCESS(rv) && ctr_eflex_map) {
        /* Register CTR_ING_EFLEX_HITBIT_CONTROL handler */
        sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        ctr_eflex_lt_cb.stage  = bcmcth_ctr_eflex_ing_hitbit_ctrl_lt_stage_cb;
        ctr_eflex_lt_cb.lookup = bcmcth_ctr_eflex_ing_hitbit_ctrl_lt_lookup_cb;
        ctr_eflex_lt_cb.ent_limit_get =
            bcmcth_ctr_eflex_ing_hitbit_ctrl_lt_ent_limit_get_cb;

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_lt_event_reg(unit,
                                CTR_ING_EFLEX_HITBIT_CONTROLt,
                                &ctr_eflex_lt_cb,
                                NULL));

        /* Register CTR_EGR_EFLEX_HITBIT_CONTROL handler */
        sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        ctr_eflex_lt_cb.stage  = bcmcth_ctr_eflex_egr_hitbit_ctrl_lt_stage_cb;
        ctr_eflex_lt_cb.lookup = bcmcth_ctr_eflex_egr_hitbit_ctrl_lt_lookup_cb;
        ctr_eflex_lt_cb.ent_limit_get =
            bcmcth_ctr_eflex_egr_hitbit_ctrl_lt_ent_limit_get_cb;

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_lt_event_reg(unit,
                                CTR_EGR_EFLEX_HITBIT_CONTROLt,
                                &ctr_eflex_lt_cb,
                                NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
