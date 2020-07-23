/*! \file bcmfp_cth_policy.c
 *
 * APIs for FP policy template LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate policy config provided
 * using policy template LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <shr/shr_util.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_action_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_policy_id_get(int unit,
                 bcmfp_stage_t *stage,
                 bcmltd_field_t *buffer,
                 uint32_t *policy_id)
{
    uint32_t policy_id_fid = 0;
    uint64_t policy_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(policy_id, SHR_E_PARAM);

    if (stage->tbls.policy_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    policy_id_fid = stage->tbls.policy_tbl->key_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             policy_id_fid,
                             (void *)buffer,
                             &policy_id_u64),
            SHR_E_NOT_FOUND);

    *policy_id = policy_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_policy_flex_ctr_params_get(int unit,
                                 bcmfp_stage_t *stage,
                                 bcmltd_field_t *buffer,
                                 uint16_t *flex_r_count,
                                 uint16_t *flex_y_count,
                                 uint16_t *flex_g_count,
                                 uint32_t *flex_ctr_object,
                                 uint16_t *flex_ctr_container_id)
{
    uint32_t flex_count_fid = 0;
    uint32_t flex_ctr_object_fid = 0;
    uint32_t flex_ctr_container_id_fid = 0;
    uint64_t flex_count_u64 = 0;
    uint64_t flex_counter_object_u64 = 0;
    uint64_t flex_counter_container_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    if (stage->tbls.policy_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the flex count fields for red packets. */
    flex_count_fid = stage->tbls.policy_tbl->flex_ctr_r_count_fid;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             flex_count_fid,
                             (void *)buffer,
                             &flex_count_u64),
            SHR_E_NOT_FOUND);
    *flex_r_count = flex_count_u64 & 0x3;

    /* Get the flex count fields for yellow packets. */
    flex_count_fid = stage->tbls.policy_tbl->flex_ctr_y_count_fid;
    flex_count_u64 = 0;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             flex_count_fid,
                             (void *)buffer,
                             &flex_count_u64),
            SHR_E_NOT_FOUND);
    *flex_y_count = flex_count_u64 & 0x3;

    /* Get the flex count fields for green packets. */
    flex_count_fid = stage->tbls.policy_tbl->flex_ctr_g_count_fid;
    flex_count_u64 = 0;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             flex_count_fid,
                             (void *)buffer,
                             &flex_count_u64),
            SHR_E_NOT_FOUND);
    *flex_g_count = flex_count_u64 & 0x3;

    /* Get the flex counter object ID value. */
    flex_ctr_object_fid = stage->tbls.policy_tbl->flex_ctr_object_fid;
    flex_counter_object_u64 = 0;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             flex_ctr_object_fid,
                             (void *)buffer,
                             &flex_counter_object_u64),
            SHR_E_NOT_FOUND);
    *flex_ctr_object = flex_counter_object_u64 & 0xFFFFFFFF;

    /* Get the flex counter container ID value. */
    flex_ctr_container_id_fid = stage->tbls.policy_tbl->flex_ctr_container_id_fid;
    flex_counter_container_id_u64 = 0;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             flex_ctr_container_id_fid,
                             (void *)buffer,
                             &flex_counter_container_id_u64),
            SHR_E_NOT_FOUND);
    *flex_ctr_container_id = flex_counter_container_id_u64 & 0xF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_policy_action_data_get(int unit,
                 bcmfp_stage_t *stage,
                 bcmltd_field_t *buffer,
                 bcmfp_action_data_t **ad)
{
    bcmfp_action_data_t *action_data = NULL;
    bcmfp_tbl_action_fid_info_t *actions_fid_info = NULL;
    uint32_t idx = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ad, SHR_E_PARAM);

    if (stage->tbls.policy_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    actions_fid_info = stage->tbls.policy_tbl->actions_fid_info;
    if (actions_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    while (buffer != NULL) {
        idx = buffer->idx;
        if (actions_fid_info[buffer->id].action == 0) {
            buffer = buffer->next;
            continue;
        }
        BCMFP_ALLOC(action_data,
                    sizeof(bcmfp_action_data_t),
                    "bcmfpPolicyActionData");
        /*
         * Add the new node to the linked list of
         * action data.
         */
        action_data->next = *ad;
        *ad = action_data;

        /* Update the action. */
        action_data->action = actions_fid_info[buffer->id].action + idx;

        /* Update the action parameter */
        action_data->param = buffer->data & 0xFFFFFFFF;
        action_data = NULL;
        buffer = buffer->next;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_policy_config_free(int unit,
                      bcmfp_policy_config_t *policy_config)
{
    bcmfp_action_data_t *action_data = NULL;
    bcmfp_action_data_t *action_data_part[BCMFP_ACTION_MAX_PARTS];
    uint8_t part_id = 0;

    SHR_FUNC_ENTER(unit);

    if (policy_config == NULL) {
        SHR_EXIT();
    }

    action_data_part[0] = policy_config->action_data;
    action_data_part[1] = policy_config->action_data1;
    action_data_part[2] = policy_config->action_data2;

    for (part_id = 0; part_id < BCMFP_ACTION_MAX_PARTS; part_id++) {
            action_data = action_data_part[part_id];
            while (action_data != NULL) {
                policy_config->action_data = action_data->next;
                SHR_FREE(action_data);
                action_data = policy_config->action_data;
            }
    }

    SHR_FREE(policy_config);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_policy_config_compare(int unit,
                            bcmfp_policy_config_t *p1,
                            bcmfp_policy_config_t *p2,
                            bool *matched)
{
    bcmfp_action_data_t *p1_action_data = NULL;
    bcmfp_action_data_t *p2_action_data = NULL;
    uint16_t p1_actions_count = 0;
    uint16_t p2_actions_count = 0;
    bool match_found = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(p1, SHR_E_PARAM);
    SHR_NULL_CHECK(p2, SHR_E_PARAM);
    SHR_NULL_CHECK(matched, SHR_E_PARAM);

    *matched = FALSE;
    /* Count the number of actions in first policy. */
    p1_action_data = p1->action_data;
    while (p1_action_data != NULL) {
        p1_actions_count++;
        p1_action_data = p1_action_data->next;
    }
    /* Count the number of actions in second policy. */
    p2_action_data = p2->action_data;
    while (p2_action_data != NULL) {
        p2_actions_count++;
        p2_action_data = p2_action_data->next;
    }

    /*
     * Number of actions in first and second policy must
     * be same.
     */
    if (p1_actions_count != p2_actions_count) {
        SHR_EXIT();
    }

    /*
     * Entries must share the same counters if counters
     * are attached.
     */
    if ((p1->flex_ctr_r_count != p2->flex_ctr_r_count) ||
        (p1->flex_ctr_y_count != p2->flex_ctr_y_count) ||
        (p1->flex_ctr_g_count != p2->flex_ctr_g_count) ||
        (p1->flex_ctr_object != p2->flex_ctr_object) ||
        (p1->flex_ctr_container_id != p2->flex_ctr_container_id)) {
        SHR_EXIT();
    }

    /* Actions data in both the policies must be same. */
    p1_action_data = p1->action_data;
    while (p1_action_data != NULL) {
        p2_action_data = p2->action_data;
        while (p2_action_data != NULL) {
            if ((p2_action_data->action ==
                 p1_action_data->action) &&
                (p2_action_data->param ==
                 p1_action_data->param) &&
                (p2_action_data->flags ==
                 p1_action_data->flags)) {
                 match_found = TRUE;
                 break;
            }
            p2_action_data = p2_action_data->next;
        }
        if (match_found == FALSE) {
            SHR_EXIT();
        }
        match_found = FALSE;
        p1_action_data = p1_action_data->next;
    }

    *matched = TRUE;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_policy_viewtype_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_action_data_t *action_data,
                          bcmfp_action_viewtype_t *viewtype)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_action_cfg_t *action_cfg = NULL;
    bool initial_viewtype_set = FALSE;
    bcmfp_action_viewtype_t temp_view = BCMFP_ACTION_VIEWTYPE_DEFAULT;
    uint8_t viewtype_bmp = 0x0;
    uint8_t curr_viewtype_bmp = 0x0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /*
     * iterate through all the viewtypes supported
     * for all actions in action_data and finalize on
     * one viewtype
     */
    while (action_data != NULL) {
        action_cfg = NULL;
        /*
         * curr_viewtype_bmp maintains the
         * viewtypes supported by the current action
         */
        curr_viewtype_bmp = 0x0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_action_cfg_get(unit,
                                  stage_id,
                                  action_data->action,
                                  &action_cfg));
        while (action_cfg != NULL) {
            curr_viewtype_bmp = curr_viewtype_bmp |
                (1 << action_cfg->viewtype);
            action_cfg = action_cfg->next;
        }
        /*
         * viewtype_bmp maintains the common viewtypes
         * for all the actions parsed so far
         */
        if (initial_viewtype_set == FALSE) {
            viewtype_bmp = curr_viewtype_bmp;
        } else {
            viewtype_bmp = viewtype_bmp & curr_viewtype_bmp;
        }
        initial_viewtype_set = TRUE;
        action_data = action_data->next;
    }
    /*
     * incompatible actions are part of policy
     * no common viewtype is applicable for
     * all actions
     */
    if (viewtype_bmp == 0x0) {
        /* Not a valid view type */
        *viewtype = BCMFP_ACTION_VIEWTYPE_COUNT;
        SHR_ERR_EXIT(SHR_E_NONE);
    }

    /* set the viewtype */
    for (temp_view = BCMFP_ACTION_VIEWTYPE_DEFAULT;
         temp_view < BCMFP_ACTION_VIEWTYPE_COUNT;
         temp_view++) {
        if (viewtype_bmp & (1 << temp_view)) {
            *viewtype = temp_view;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}


int
bcmfp_policy_config_get(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     bcmltd_sid_t tbl_id,
                     bcmfp_stage_id_t stage_id,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     bcmfp_policy_config_t **config)
{
    uint32_t policy_id = 0;
    uint16_t r_count = 0;
    uint16_t y_count = 0;
    uint16_t g_count = 0;
    uint32_t flex_ctr_object = 0;
    uint16_t flex_ctr_container_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    bcmfp_policy_config_t *policy_config = NULL;
    bcmfp_action_data_t *action_data = NULL;
    bcmfp_action_viewtype_t viewtype = BCMFP_ACTION_VIEWTYPE_DEFAULT;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.policy_tbl == NULL ||
        stage->tbls.policy_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(policy_config,
                sizeof(bcmfp_policy_config_t),
                "bcmfpPolicyConfig");
    *config = policy_config;

    /* Get the policy ID */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_id_get(unit, stage, buffer, &policy_id));
    policy_config->policy_id = policy_id;

    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the flex counter red/green/yellow count fields. */
    if (stage->tbls.policy_tbl->flex_ctr_r_count_fid != 0 ||
        stage->tbls.policy_tbl->flex_ctr_y_count_fid != 0 ||
        stage->tbls.policy_tbl->flex_ctr_g_count_fid != 0 ||
        stage->tbls.policy_tbl->flex_ctr_object_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_flex_ctr_params_get(unit,
                                              stage,
                                              buffer,
                                              &r_count,
                                              &y_count,
                                              &g_count,
                                              &flex_ctr_object,
                                              &flex_ctr_container_id));
        policy_config->flex_ctr_r_count = r_count;
        policy_config->flex_ctr_y_count = y_count;
        policy_config->flex_ctr_g_count = g_count;
        policy_config->flex_ctr_object = flex_ctr_object;
        policy_config->flex_ctr_container_id = flex_ctr_container_id;
    }

    /* Get the policy action data. */
    buffer = (bcmltd_field_t *)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_action_data_get(unit, stage, buffer, &action_data));
    policy_config->action_data = action_data;
    /* Set the viewtype of the policy data if supported */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_POLICY_MULTI_VIEW)) {
        if (action_data)  {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_policy_viewtype_get(unit,
                                           stage_id,
                                           action_data,
                                           &viewtype));
            policy_config->viewtype = viewtype;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_policy_remote_config_get(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_policy_id_t policy_id,
                               bcmfp_policy_config_t **config)
{
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_policy_config_t *policy_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.policy_tbl->key_fid;
    tbl_id = stage->tbls.policy_tbl->sid;
    num_fid = stage->tbls.policy_tbl->lrd_info.num_fid;

    /* Get the FP policy LT entry saved in IMM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &policy_id,
                                1,
                                num_fid,
                                &in,
                                &out,
                                &(stage->imm_buffers)));

    /*
     * If not found found in IMM return with no error but
     * config is set to NULL.
     */
    if (SHR_FUNC_ERR() && SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        SHR_EXIT();
    } else if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    /* Assign key and data from in and out. */
    key = in.field[0];
    if (out.count != 0) {
        data = out.field[0];
    }

    /* Get the FP policy LT entry config of ACL. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage->stage_id,
                                key,
                                data,
                                &policy_config));
    *config = policy_config;
    policy_config = NULL;
exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_policy_config_free(unit,policy_config);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_policy_action_fid_get(int unit,
                         bcmfp_stage_t *stage,
                         bcmfp_action_t action,
                         bcmlrd_fid_t *fid)
{
    uint16_t idx = 0;
    bcmfp_tbl_action_fid_info_t *actions_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fid, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    actions_fid_info =
        stage->tbls.policy_tbl->actions_fid_info;
    if (actions_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < stage->tbls.policy_tbl->fid_count; idx++) {
        if (actions_fid_info[idx].action == action) {
            *fid = idx;
            break;
        }
    }

    if (idx == stage->tbls.policy_tbl->fid_count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_policy_count_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_entry_id_t entry_id,
                                 bcmfp_policy_id_t rule_id,
                                 void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    idp_info->entry_or_group_count++;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_policy_config_dump(int unit,
                     bcmltd_sid_t tbl_id,
                     bcmfp_stage_t *stage,
                     bcmfp_policy_config_t *policy_config)
{
    bcmfp_action_data_t *action_data = NULL;
    bcmlrd_field_def_t field_def;
    bcmlrd_fid_t fid = 0;
    bcmfp_action_t action;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(policy_config, SHR_E_PARAM);

    if (stage->tbls.policy_tbl->key_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Policy ID = %d\n"),
            policy_config->policy_id));
    }

    action_data = policy_config->action_data;
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "Action parameters:\n")));
    while (action_data != NULL) {
        sal_memset(&field_def, 0, sizeof(bcmlrd_field_def_t));
        action = action_data->action;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_action_fid_get(unit,
                                      stage,
                                      action,
                                      &fid));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        fid,
                                        &field_def));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "    %s:0x%x(%d)\n"),
                   field_def.name,
                   action_data->param,
                   action_data->param));

        action_data = action_data->next;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
               POLICY CONFIG VALIDATE FUNCTIONS
 */
int
bcmfp_policy_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_policy_config_t *policy_config = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_policy_id_t policy_id = 0;
    bool not_mapped = FALSE;
    bool def_not_mapped = TRUE;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmltd_field_t *in_key = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_id_get(unit, stage, in_key, &policy_id));

    /* Process entries associated to this policy. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_entry_map_check(unit,
                                    stage_id,
                                    policy_id,
                                    &not_mapped));
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                               BCMFP_STAGE_DEFAULT_POLICY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dpolicy_group_map_check(unit,
                                           stage_id,
                                           policy_id,
                                           &def_not_mapped));
    }

    /* Return if no entries are associated to the policy */
    if ((not_mapped == TRUE) && (def_not_mapped == TRUE)) {
        SHR_EXIT();
    }

    /*
     * Build list of fields with non default values from
     * the incoming data.
     */
    in_data = (bcmltd_field_t *)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_non_default_data_get(unit,
                                        tbl_id,
                                        in_data,
                                        &non_default_data,
                                        &(stage->imm_buffers)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              non_default_data,
                              &policy_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_config_dump(unit,
                                      tbl_id,
                                      stage,
                                      policy_config));
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_POLICY_TEMPLATE;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_INSERT;

    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_policy_count_get;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_traverse(unit,
                                       stage_id,
                                       policy_id,
                                       cb_func,
                                       user_data));
    if (not_mapped == false) {
        cb_func = bcmfp_entry_idp_policy_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_entry_map_traverse(unit,
                                             stage_id,
                                             policy_id,
                                             cb_func,
                                             user_data));
    }
    if (def_not_mapped == false) {
        cb_func = bcmfp_group_idp_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dpolicy_group_map_traverse(unit,
                                              stage_id,
                                              policy_id,
                                              cb_func,
                                              user_data));
    }

exit:
    bcmfp_policy_config_free(unit, policy_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_policy_config_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_policy_config_t *policy_config = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_policy_id_t policy_id = 0;
    bool not_mapped = FALSE;
    bool def_not_mapped = TRUE;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    uint16_t num_fid = 0;
    bcmltd_field_t *in_key = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_fields_t updated_fields;
    const bcmltd_field_t *updated_data = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&updated_fields, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_id_get(unit, stage, in_key, &policy_id));

    /* Process entries associated to this policy. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_entry_map_check(unit,
                                    stage_id,
                                    policy_id,
                                    &not_mapped));

    if (BCMFP_STAGE_FLAGS_TEST(stage,
                               BCMFP_STAGE_DEFAULT_POLICY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dpolicy_group_map_check(unit,
                                           stage_id,
                                           policy_id,
                                           &def_not_mapped));
    }

    /* Return if no entries are associated to the policy */
    if ((not_mapped == TRUE) && (def_not_mapped == TRUE)) {
        SHR_EXIT();
    }

    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.policy_tbl->lrd_info.num_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_updated_data_get(unit,
                                    tbl_id,
                                    num_fid,
                                    in_key,
                                    in_data,
                                    &updated_fields));

    updated_data =
        (const bcmltd_field_t *)(updated_fields.field[0]);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              updated_data,
                              &policy_config));

    sal_memset(&idp_info,
               0,
               sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_POLICY_TEMPLATE;
    idp_info.key = key;
    idp_info.data = updated_data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_UPDATE;

    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_policy_count_get;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_traverse(unit,
                                       stage_id,
                                       policy_id,
                                       cb_func,
                                       user_data));
    if (not_mapped == false) {
        cb_func = bcmfp_entry_idp_policy_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_entry_map_traverse(unit,
                                             stage_id,
                                             policy_id,
                                             cb_func,
                                             user_data));
    }

    if (def_not_mapped == false) {
        cb_func = bcmfp_group_idp_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dpolicy_group_map_traverse(unit,
                                              stage_id,
                                              policy_id,
                                              cb_func,
                                              user_data));
    }

exit:
    bcmfp_policy_config_free(unit, policy_config);
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FUNC_EXIT();
}

int
bcmfp_policy_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_policy_config_t *policy_config = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_policy_id_t policy_id = 0;
    bool not_mapped = FALSE;
    bool def_not_mapped = TRUE;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    bcmltd_field_t *in_key = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_id_get(unit, stage, in_key, &policy_id));

    /* Process entries associated to this policy. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_entry_map_check(unit,
                                    stage_id,
                                    policy_id,
                                    &not_mapped));
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                               BCMFP_STAGE_DEFAULT_POLICY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dpolicy_group_map_check(unit,
                                           stage_id,
                                           policy_id,
                                           &def_not_mapped));
    }

    /* Return if no entries are associated to the policy */
    if ((not_mapped == TRUE) && (def_not_mapped == TRUE)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              data,
                              &policy_config));

    sal_memset(&idp_info,
               0,
               sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_POLICY_TEMPLATE;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_DELETE;

    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_policy_count_get;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_traverse(unit,
                                       stage_id,
                                       policy_id,
                                       cb_func,
                                       user_data));
    if (not_mapped == false) {
        cb_func = bcmfp_entry_idp_policy_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_entry_map_traverse(unit,
                                             stage_id,
                                             policy_id,
                                             cb_func,
                                             user_data));
    }

    if (def_not_mapped == false) {
        cb_func = bcmfp_group_idp_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dpolicy_group_map_traverse(unit,
                                              stage_id,
                                              policy_id,
                                              cb_func,
                                              user_data));
    }

exit:
    bcmfp_policy_config_free(unit, policy_config);
    SHR_FUNC_EXIT();
}
