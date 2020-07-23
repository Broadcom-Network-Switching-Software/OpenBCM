/*! \file bcmcth_ctr_eflex_action_profile_info.c
 *
 * BCMCTH CTR_EFLEX_ACTION_PROFILE_INFOt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*! Generate CTR_xxx_EFLEX_ACTION_PROFILE_INFO LT field ids. */
#define CTR_ING_EGR_ACTION_PROFILE_INFO_FIDS(x) \
    CTR_ING_EFLEX_ACTION_PROFILE_INFOt_##x, \
    CTR_EGR_EFLEX_ACTION_PROFILE_INFOt_##x

/*! Generate STATE_xxx_EFLEX_ACTION_PROFILE_INFO LT field ids. */
#define STATE_ING_EGR_ACTION_PROFILE_INFO_FIDS(x) \
    FLEX_STATE_ING_ACTION_PROFILE_INFOt_##x, \
    FLEX_STATE_EGR_ACTION_PROFILE_INFOt_##x

/*! Generate CTR_xxx_FLEX_POOL_INFO LT field ids. */
#define CTR_ING_EGR_POOL_INFO_FIDS(x) \
    CTR_ING_FLEX_POOL_INFOt_##x, \
    CTR_EGR_FLEX_POOL_INFOt_##x

/*! Generate STATE_xxx_EFLEX_POOL_INFO LT field ids. */
#define STATE_ING_EGR_POOL_INFO_FIDS(x) \
    FLEX_STATE_ING_POOL_INFOt_##x, \
    FLEX_STATE_EGR_POOL_INFOt_##x

/*******************************************************************************
 * Local definitions
 */
/* ACTION_PROFILE_INFO LT field info */
static const bcmlrd_sid_t action_profile_info_table_sid[][2] = {
        {CTR_ING_EGR_LT_IDS(EFLEX_ACTION_PROFILE_INFOt)},
        {STATE_ING_EGR_LT_IDS(ACTION_PROFILE_INFOt)},
};

static const bcmlrd_fid_t action_profile_info_id_fid[][2] = {
    {CTR_ING_EFLEX_ACTION_PROFILE_INFOt_CTR_ING_EFLEX_ACTION_PROFILE_IDf,
     CTR_EGR_EFLEX_ACTION_PROFILE_INFOt_CTR_EGR_EFLEX_ACTION_PROFILE_IDf},
    {FLEX_STATE_ING_ACTION_PROFILE_INFOt_FLEX_STATE_ING_ACTION_PROFILE_IDf,
     FLEX_STATE_EGR_ACTION_PROFILE_INFOt_FLEX_STATE_EGR_ACTION_PROFILE_IDf},
};

static const bcmlrd_fid_t pool_fid[][2] = {
        {CTR_ING_EGR_ACTION_PROFILE_INFO_FIDS(POOL_IDf)},
        {STATE_ING_EGR_ACTION_PROFILE_INFO_FIDS(POOL_IDf)},
};

static const bcmlrd_fid_t base_idx_fid[][2] = {
        {CTR_ING_EGR_ACTION_PROFILE_INFO_FIDS(BASE_INDEXf)},
        {STATE_ING_EGR_ACTION_PROFILE_INFO_FIDS(BASE_INDEXf)},
};

static const bcmlrd_fid_t top_idx_fid[][2] = {
        {CTR_ING_EGR_ACTION_PROFILE_INFO_FIDS(TOP_INDEXf)},
        {STATE_ING_EGR_ACTION_PROFILE_INFO_FIDS(TOP_INDEXf)},
};

static const bcmlrd_fid_t num_pools_fid[][2] = {
        {CTR_ING_EGR_ACTION_PROFILE_INFO_FIDS(NUM_POOLSf)},
        {STATE_ING_EGR_ACTION_PROFILE_INFO_FIDS(NUM_POOLSf)},
};

static const bcmlrd_fid_t shd_pool_fid[][2] = {
        {CTR_ING_EGR_ACTION_PROFILE_INFO_FIDS(SHADOW_POOL_IDf)},
        {STATE_ING_EGR_ACTION_PROFILE_INFO_FIDS(SHADOW_POOL_IDf)},
};

static const bcmlrd_fid_t shd_valid_fid[][2] = {
        {CTR_ING_EGR_ACTION_PROFILE_INFO_FIDS(SHADOW_VALIDf)},
        {STATE_ING_EGR_ACTION_PROFILE_INFO_FIDS(SHADOW_VALIDf)},
};

/* POOL_INFO LT field info */
static const bcmlrd_sid_t pool_info_table_sid[][2] = {
        {CTR_ING_EGR_LT_IDS(FLEX_POOL_INFOt)},
        {STATE_ING_EGR_LT_IDS(POOL_INFOt)}
};

static const bcmlrd_fid_t pool_info_id_fid[][2] = {
       {CTR_ING_FLEX_POOL_INFOt_CTR_ING_FLEX_POOL_INFO_IDf,
        CTR_EGR_FLEX_POOL_INFOt_CTR_EGR_FLEX_POOL_INFO_IDf},
       {FLEX_STATE_ING_POOL_INFOt_FLEX_STATE_ING_POOL_INFO_IDf,
        FLEX_STATE_EGR_POOL_INFOt_FLEX_STATE_EGR_POOL_INFO_IDf}
};

static const bcmlrd_fid_t pool_size_fid[][2] = {
        {CTR_ING_EGR_POOL_INFO_FIDS(POOL_SIZEf)},
        {STATE_ING_EGR_POOL_INFO_FIDS(POOL_SIZEf)},
};

static const bcmlrd_fid_t pool_inst_fid[][2] = {
        {0},
        {0,FLEX_STATE_EGR_POOL_INFOt_INSTANCEf},
};

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Entry limit get callback function of CTR_ING_EFLEX_ACTION_PROFILE_INFO
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_ing_action_profile_info_lt_ent_limit_get_cb(int unit,
                                                             uint32_t trans_id,
                                                             bcmltd_sid_t sid,
                                                             uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_ent_limit_get(unit,
                                                          INGRESS,
                                                          CTR_EFLEX,
                                                          count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of CTR_EGR_EFLEX_ACTION_PROFILE_INFO
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_egr_action_profile_info_lt_ent_limit_get_cb(int unit,
                                                             uint32_t trans_id,
                                                             bcmltd_sid_t sid,
                                                             uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_ent_limit_get(unit,
                                                          EGRESS,
                                                          CTR_EFLEX,
                                                          count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of
 *        FLEX_STATE_ING_ACTION_PROFILE_INFO LT
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_ing_action_profile_info_lt_ent_limit_get_cb(int unit,
                                                               uint32_t trans_id,
                                                               bcmltd_sid_t sid,
                                                               uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_ent_limit_get(unit,
                                                          INGRESS,
                                                          STATE_EFLEX,
                                                          count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Entry limit get callback function of
 *        FLEX_STATE_EGR_ACTION_PROFILE_INFO LT
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Logical table ID.
 * \param [in] count Entry limit value.
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_state_eflex_egr_action_profile_info_lt_ent_limit_get_cb(int unit,
                                                               uint32_t trans_id,
                                                               bcmltd_sid_t sid,
                                                               uint32_t *count)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lt_ent_limit_get(unit,
                                                          EGRESS,
                                                          STATE_EFLEX,
                                                          count));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
/* Function to populate POOL_INFO entry */
int
bcmcth_ctr_eflex_pool_info_populate(int unit, bool ingress, eflex_comp_t comp)
{
    int select, num_pools, pool_idx , egr_num_instance;
    bcmltd_fields_t pool_info_lt_flds = {0};
    size_t num_fields = 0, f_cnt, count;
    bcmlrd_fid_t   *fid_list = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    select = ingress ? 0 : 1;
    num_pools = ingress ? device_info->num_ingress_pools :
                          device_info->num_egress_pools;
    egr_num_instance = device_info->state_ext.num_egress_instance;
    /* Get number of fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                pool_info_table_sid[comp][select],
                                &num_fields));

    SHR_ALLOC(fid_list, sizeof(bcmlrd_fid_t) * num_fields,
        "bcmcthCtrEFlexPoolInfoFields");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, sizeof(bcmlrd_fid_t) * num_fields);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               pool_info_table_sid[comp][select],
                               num_fields,
                               fid_list,
                               &count));

    pool_info_lt_flds.field = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &pool_info_lt_flds,
                                       num_fields));

    for (pool_idx = 0; pool_idx < num_pools; pool_idx++) {
        for (f_cnt = 0; f_cnt < count; f_cnt++) {
            pool_info_lt_flds.field[f_cnt]->id = fid_list[f_cnt];

            if (fid_list[f_cnt] == pool_info_id_fid[comp][select]) {
                pool_info_lt_flds.field[f_cnt]->data = pool_idx;
            }  else  if (fid_list[f_cnt] == pool_inst_fid[comp][select]) {
                pool_info_lt_flds.field[f_cnt]->data = 0;
            } else if (fid_list[f_cnt] == pool_size_fid[comp][select]) {
                if (ingress) {
                    pool_info_lt_flds.field[f_cnt]->data =
                                            ctrl->ing_pool_ctr_num[pool_idx];
                } else {
                    pool_info_lt_flds.field[f_cnt]->data =
                                            ctrl->egr_pool_ctr_num[pool_idx];

                }
            } else {
                pool_info_lt_flds.field[f_cnt]->data = 0;
            }
        }

        pool_info_lt_flds.count = count;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_insert(unit,
                                 pool_info_table_sid[comp][select],
                                 &pool_info_lt_flds));
    }

    if (!ingress) {
        switch (egr_num_instance)  {
               case 2 :

                 for (pool_idx = 0; pool_idx < num_pools; pool_idx++) {
                   for (f_cnt = 0; f_cnt < count; f_cnt++) {
                         pool_info_lt_flds.field[f_cnt]->id = fid_list[f_cnt];
                         if (fid_list[f_cnt] ==
                             pool_info_id_fid[comp][select]) {
                             pool_info_lt_flds.field[f_cnt]->data = pool_idx;
                         } else  if (fid_list[f_cnt] ==
                                     pool_inst_fid[comp][select]) {
                             pool_info_lt_flds.field[f_cnt]->data = 1;
                         } else if (fid_list[f_cnt] ==
                                    pool_size_fid[comp][select]) {
                             pool_info_lt_flds.field[f_cnt]->data =
                                ctrl->state_ext.egr_pool_ctr_num_b[pool_idx];
                         } else {
                             pool_info_lt_flds.field[f_cnt]->data = 0;
                         }
                   }
                   pool_info_lt_flds.count = count;
                   SHR_IF_ERR_VERBOSE_EXIT
                        (bcmimm_entry_insert(unit,
                        pool_info_table_sid[comp][select],
                          &pool_info_lt_flds));
                 }
                   break;
               default : break;
       }

   }

exit:
    /* Restore pool_info_lt_flds.count */
    pool_info_lt_flds.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &pool_info_lt_flds);
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*
 * Function to lookup ACTION_PROFILE_INFO LT.
 * It populates the ACTION_PROFILE LT entry with first and last
 * primary and shadow pool ids.
 */
int
bcmcth_ctr_eflex_action_profile_info_lookup(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_field_t fld;
    bcmltd_field_t *fld_ptr_arr[1];
    bcmltd_field_t *data_fields = NULL;
    bcmltd_fields_t imm_fields = {0};
    uint32_t num_pools = 0, i;
    size_t num_fields = 0;
    int select;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Configure key field */
    key_fields.count = 1;
    key_fields.field = fld_ptr_arr;
    fld_ptr_arr[0] = &fld;
    fld.id = action_profile_info_id_fid[comp][select];
    fld.data = entry->action_profile_id;

    /* Allocate imm fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                action_profile_info_table_sid[comp][select],
                                &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &imm_fields, num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             action_profile_info_table_sid[comp][select],
                             &key_fields,
                             &imm_fields));

    /* Parse IMM entry fields */
    for (i = 0; i < imm_fields.count; i++) {
        data_fields = imm_fields.field[i];
        if (data_fields->id == shd_pool_fid[comp][select]) {
            entry->start_shdw_pool_idx = data_fields->data;
        } else if (data_fields->id == num_pools_fid[comp][select]) {
            num_pools = data_fields->data;
        } else if (data_fields->id == pool_fid[comp][select]) {
            entry->start_pool_idx = data_fields->data;
        }
    }

    entry->end_shdw_pool_idx = entry->start_shdw_pool_idx + num_pools - 1;
    entry->end_pool_idx = entry->start_pool_idx + num_pools - 1;

exit:
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/* Function to insert/delete ACTION_PROFILE_INFO entry */
int
bcmcth_ctr_eflex_action_profile_info_program(
    int unit,
    bool insert,
    ctr_eflex_action_profile_data_t *entry)
{
    int select;
    bcmltd_fields_t action_profile_info_lt_flds = {0};
    size_t num_fields, f_cnt, count;
    bcmlrd_fid_t   *fid_list = NULL;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get number of fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                action_profile_info_table_sid[comp][select],
                                &num_fields));

    SHR_ALLOC(fid_list, sizeof(bcmlrd_fid_t) * num_fields,
        "bcmcthCtrEFlexInfoFields");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, sizeof(bcmlrd_fid_t) * num_fields);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               action_profile_info_table_sid[comp][select],
                               num_fields,
                               fid_list,
                               &count));

    action_profile_info_lt_flds.field = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit,
                                       &action_profile_info_lt_flds,
                                       num_fields));

    for (f_cnt = 0; f_cnt < count; f_cnt++) {
        action_profile_info_lt_flds.field[f_cnt]->id = fid_list[f_cnt];

        if (fid_list[f_cnt] == action_profile_info_id_fid[comp][select]) {
            action_profile_info_lt_flds.field[f_cnt]->data =
                                        entry->action_profile_id;
        } else if (fid_list[f_cnt] == top_idx_fid[comp][select]) {
            action_profile_info_lt_flds.field[f_cnt]->data =
                                        entry->end_base_idx - 1;
        } else if (fid_list[f_cnt] == num_pools_fid[comp][select]) {
            action_profile_info_lt_flds.field[f_cnt]->data =
                        entry->end_pool_idx - entry->start_pool_idx + 1;
        } else if (fid_list[f_cnt] == pool_fid[comp][select]) {
            action_profile_info_lt_flds.field[f_cnt]->data =
                        entry->start_pool_idx;
        } else if (fid_list[f_cnt] == base_idx_fid[comp][select]) {
            action_profile_info_lt_flds.field[f_cnt]->data =
                        entry->start_base_idx;
        } else if (fid_list[f_cnt] == shd_pool_fid[comp][select]) {
            action_profile_info_lt_flds.field[f_cnt]->data =
                        entry->start_shdw_pool_idx;
        } else if (fid_list[f_cnt] == shd_valid_fid[comp][select]) {
            action_profile_info_lt_flds.field[f_cnt]->data =
                        entry->shadow;
        } else {
            action_profile_info_lt_flds.field[f_cnt]->data = 0;
        }
    }

    action_profile_info_lt_flds.count = count;

    if (insert) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_insert(unit,
                                 action_profile_info_table_sid[comp][select],
                                 &action_profile_info_lt_flds));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_delete(unit,
                                 action_profile_info_table_sid[comp][select],
                                 &action_profile_info_lt_flds));
    }

exit:
    /* Restore action_profile_info_lt_flds.count */
    action_profile_info_lt_flds.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &action_profile_info_lt_flds);
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_imm_action_profile_info_register(int unit)
{
    bcmimm_lt_cb_t ctr_eflex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Register CTR_ING_EFLEX_ACTION_PROFILE_INFO handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.validate =
        bcmcth_ctr_eflex_read_only_lt_validate_cb;
    ctr_eflex_lt_cb.ent_limit_get  =
        bcmcth_ctr_eflex_ing_action_profile_info_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             CTR_ING_EFLEX_ACTION_PROFILE_INFOt,
                             &ctr_eflex_lt_cb,
                             NULL));

    /* Register CTR_EGR_EFLEX_ACTION_PROFILE_INFO handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.validate =
        bcmcth_ctr_eflex_read_only_lt_validate_cb;
    ctr_eflex_lt_cb.ent_limit_get  =
        bcmcth_ctr_eflex_egr_action_profile_info_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             CTR_EGR_EFLEX_ACTION_PROFILE_INFOt,
                             &ctr_eflex_lt_cb,
                             NULL));

    /* Register CTR_ING_FLEX_POOL_INFO handler */
    sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_eflex_lt_cb.validate =
        bcmcth_ctr_eflex_read_only_lt_validate_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             CTR_ING_FLEX_POOL_INFOt,
                             &ctr_eflex_lt_cb,
                             NULL));

    /* Register CTR_EGR_FLEX_POOL_INFO handler */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             CTR_EGR_FLEX_POOL_INFOt,
                             &ctr_eflex_lt_cb,
                             NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_state_eflex_imm_action_profile_info_register(int unit)
{
    bcmimm_lt_cb_t state_eflex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Register callback handlers for the in-memory logical tables */

    /* Register FLEX_STATE_ING_ACTION_PROFILE_INFO handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.validate =
        bcmcth_ctr_eflex_read_only_lt_validate_cb;
    state_eflex_lt_cb.ent_limit_get  =
        bcmcth_state_eflex_ing_action_profile_info_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             FLEX_STATE_ING_ACTION_PROFILE_INFOt,
                             &state_eflex_lt_cb,
                             NULL));

    /* Register FLEX_STATE_EGR_ACTION_PROFILE_INFO handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.validate =
        bcmcth_ctr_eflex_read_only_lt_validate_cb;
    state_eflex_lt_cb.ent_limit_get  =
        bcmcth_state_eflex_egr_action_profile_info_lt_ent_limit_get_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             FLEX_STATE_EGR_ACTION_PROFILE_INFOt,
                             &state_eflex_lt_cb,
                             NULL));

    /* Register FLEX_STATE_ING_POOL_INFO handler */
    sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    state_eflex_lt_cb.validate =
        bcmcth_ctr_eflex_read_only_lt_validate_cb;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             FLEX_STATE_ING_POOL_INFOt,
                             &state_eflex_lt_cb,
                             NULL));

    /* Register FLEX_STATE_EGR_POOL_INFO handler */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_lt_event_reg(unit,
                             FLEX_STATE_EGR_POOL_INFOt,
                             &state_eflex_lt_cb,
                             NULL));

exit:
    SHR_FUNC_EXIT();
}
