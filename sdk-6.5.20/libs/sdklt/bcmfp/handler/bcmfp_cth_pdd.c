/*! \file bcmfp_cth_pdd.c
 *
 * APIs for FP pdd template LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate pdd config provided
 * using pdd template LTs.
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
#include <bcmfp/generated/bcmfp_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*******************************************************************************
               ASET/PDD PROCESS FUNCTIONS
 */
int
bcmfp_pdd_config_process(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t tbl_id,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_pdd_config_t *pdd_config,
                         uint32_t pdd_id,
                         bcmfp_pdd_attrs_t *pdd_attrs,
                         bcmfp_group_mode_t min_mode)
{
    int rv;
    bcmfp_pdd_cfg_t *pdd_cfg_info = NULL;
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    uint8_t valid_pdd_data = FALSE;
    uint8_t pdd_parts = 0;
    bcmfp_group_mode_t group_mode = BCMFP_GROUP_MODE_COUNT;
    bcmfp_group_mode_t max_mode = BCMFP_GROUP_MODE_COUNT;
    uint32_t *mode_bmp = NULL;
    uint32_t policy_width = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_pdd_oper_type_t pdd_type = BCMFP_PDD_OPER_TYPE_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pdd_attrs, SHR_E_PARAM);

    if (pdd_id == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    max_mode = BCMFP_GROUP_MODE_COUNT;
    mode_bmp = stage->group_mode_bmp.w;
    for (group_mode = min_mode;
         group_mode < max_mode;
         group_mode++) {

        if (group_mode == BCMFP_GROUP_MODE_AUTO) {
            continue;
        }

        if (!SHR_BITGET(mode_bmp, group_mode)) {
            continue;
        }

        rv = bcmfp_pdd_config_info_set(unit,
                                       op_arg,
                                       stage_id,
                                       pdd_id,
                                       group_mode,
                                       FALSE,
                                       pdd_attrs->pdd_per_group,
                                       pdd_attrs->sbr_per_group,
                                       pdd_config,
                                       &valid_pdd_data,
                                       &pdd_cfg_info);
        if (valid_pdd_data == FALSE) {
            pdd_parts = 1;
            BCMFP_ALLOC(pdd_oper_info,
                        sizeof(bcmfp_pdd_oper_t),
                        "bcmfpPddConfigProcessPddOper");
            break;
        }
        if (rv == SHR_E_NOT_FOUND) {
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(rv);

        rv = bcmfp_pdd_cfg_process(unit,
                                   pdd_cfg_info,
                                   &pdd_oper_info,
                                   &pdd_parts);
        if (SHR_SUCCESS(rv)) {
            policy_width = pdd_cfg_info->pdd_hw_info->raw_policy_width;
            break;
        } else {
            bcmfp_pdd_config_info_free(unit, pdd_cfg_info);
            pdd_cfg_info = NULL;
        }
    }

    /*
     * Update the PDD info i.e, container bitmap,
     * section offsets and action offsets in pdd
     * oper info.
     */
    if (pdd_oper_info != NULL && pdd_parts != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_oper_type_get(unit, pdd_attrs, &pdd_type));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_oper_info_set(unit,
                                     stage_id,
                                     pdd_id,
                                     pdd_type,
                                     pdd_parts,
                                     group_mode,
                                     policy_width,
                                     pdd_oper_info));
    }
exit:
    bcmfp_pdd_config_info_free(unit, pdd_cfg_info);
    bcmfp_pdd_oper_info_free(unit, pdd_parts, pdd_oper_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_oper_type_get(int unit,
                        bcmfp_pdd_attrs_t *pdd_attrs,
                        bcmfp_pdd_oper_type_t *pdd_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pdd_type, SHR_E_PARAM);
    SHR_NULL_CHECK(pdd_attrs, SHR_E_PARAM);

    if (pdd_attrs->sbr_not_supported == TRUE &&
        pdd_attrs->pdd_per_group == TRUE) {
        *pdd_type = BCMFP_PDD_OPER_TYPE_PDD_GROUP;
        SHR_EXIT();
    }

    if (pdd_attrs->default_policy_pdd == TRUE) {
        *pdd_type = BCMFP_PDD_OPER_TYPE_DEFAULT_POLICY;
        SHR_EXIT();
    }

    if (pdd_attrs->pdd_per_group == TRUE &&
        pdd_attrs->sbr_per_group == TRUE) {
        *pdd_type = BCMFP_PDD_OPER_TYPE_PDD_SBR_GROUP;
    } else if (pdd_attrs->pdd_per_group == FALSE &&
               pdd_attrs->sbr_per_group == FALSE) {
        *pdd_type = BCMFP_PDD_OPER_TYPE_PDD_SBR_ENTRY;
    } else if (pdd_attrs->pdd_per_group == TRUE &&
               pdd_attrs->sbr_per_group == FALSE) {
        *pdd_type = BCMFP_PDD_OPER_TYPE_PDD_GROUP_SBR_ENTRY;
    } else if (pdd_attrs->pdd_per_group == FALSE &&
               pdd_attrs->sbr_per_group == TRUE) {
        *pdd_type = BCMFP_PDD_OPER_TYPE_PDD_ENTRY_SBR_GROUP;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_cfg_get(int unit,
                  uint32_t trans_id,
                  bcmfp_group_mode_t mode,
                  bool is_default_policy,
                  bool is_pdd_per_group,
                  bool is_sbr_per_group,
                  bcmfp_stage_id_t stage_id,
                  bcmfp_pdd_config_t *pdd_config,
                  uint8_t *valid_pdd_data,
                  bcmfp_pdd_cfg_t **pdd_cfg)
{
    uint16_t num_actions = 0;
    bcmfp_action_t pdd_actions[BCMFP_ACTIONS_COUNT];
    bcmfp_pdd_cfg_t *pdd_cfg_info = NULL;
    bcmfp_stage_t *stage = NULL;
    uint16_t per_group_offset = 0;
    uint16_t per_entry_offset = 0;
    bcmfp_action_data_t *action_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pdd_config, SHR_E_PARAM);
    SHR_NULL_CHECK(valid_pdd_data, SHR_E_PARAM);
    SHR_NULL_CHECK(pdd_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(pdd_cfg_info,
                sizeof(bcmfp_pdd_cfg_t),
                "bcmfpPddConfigInfoCfgGet");

    *valid_pdd_data = 0;
    pdd_cfg_info->action_cfg_db = stage->action_cfg_db;
    pdd_cfg_info->pdd_hw_info = stage->pdd_hw_info;
    per_group_offset =
        stage->misc_info->per_group_pdd_sbr_raw_data_offset;
    per_entry_offset =
        stage->misc_info->per_entry_pdd_sbr_raw_data_offset;
    if (BCMFP_STAGE_FLAGS_TEST(stage,
            BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        pdd_cfg_info->raw_data_offset = 0;
        if (TRUE == is_default_policy) {
            pdd_cfg_info->pdd_hw_info->raw_policy_width =
                stage->misc_info->default_policy_data_width;
        } else {
            pdd_cfg_info->pdd_hw_info->raw_policy_width =
                stage->key_data_size_info->data_size[mode];
        }
    } else {
        if ((TRUE == is_pdd_per_group) &&
            (TRUE == is_sbr_per_group)) {
            pdd_cfg_info->raw_data_offset = per_group_offset;
        } else if ((TRUE == is_pdd_per_group) &&
                   (FALSE == is_sbr_per_group)) {
            pdd_cfg_info->raw_data_offset = per_entry_offset;
        } else if ((FALSE == is_pdd_per_group) &&
                   (TRUE == is_sbr_per_group)) {
            pdd_cfg_info->raw_data_offset = per_entry_offset;
        } else {
            pdd_cfg_info->raw_data_offset = per_entry_offset;
        }
    }

    action_data = pdd_config->action_data;
    while (action_data != NULL) {
        *valid_pdd_data = 1;
        pdd_actions[num_actions] = action_data->action;
        num_actions++;
        action_data = action_data->next;
    }

    if (*valid_pdd_data == 1) {
        size_t size = 0;
        size = sizeof(bcmfp_action_t) * num_actions;
        BCMFP_ALLOC(pdd_cfg_info->pdd_action,
                    size,
                    "bcmfpPddConfigInfoActions");
        sal_memcpy(pdd_cfg_info->pdd_action, &pdd_actions, size);
        pdd_cfg_info->num_actions = num_actions;
        *pdd_cfg = pdd_cfg_info;
    }
    else {
        SHR_FREE(pdd_cfg_info);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_cfg_free(int unit,
                   bcmfp_pdd_cfg_t *pdd_cfg)
{
    SHR_FUNC_ENTER(unit);

    if (NULL != pdd_cfg) {
        if (NULL != pdd_cfg->pdd_action) {
            SHR_FREE(pdd_cfg->pdd_action);
        }
        SHR_FREE(pdd_cfg);
    }

    SHR_FUNC_EXIT()
}

static int
bcmfp_pdd_id_get(int unit,
                 bcmfp_stage_t *stage,
                 bcmltd_field_t *buffer,
                 uint32_t *pdd_id)
{
    uint32_t pdd_id_fid = 0;
    uint64_t pdd_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(pdd_id, SHR_E_PARAM);

    if (stage->tbls.pdd_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    pdd_id_fid = stage->tbls.pdd_tbl->key_fid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_value_get(unit,
                             pdd_id_fid,
                             (void *)buffer,
                             &pdd_id_u64));

    *pdd_id = pdd_id_u64 & 0xFFFFFFFF;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_pdd_action_data_get(int unit,
                 bcmfp_stage_t *stage,
                 bcmltd_field_t *buffer,
                 bcmfp_action_data_t **ad)
{
    bcmfp_action_data_t *action_data = NULL;
    bcmfp_tbl_action_fid_info_t *actions_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ad, SHR_E_PARAM);

    if (stage->tbls.pdd_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    actions_fid_info =
        stage->tbls.pdd_tbl->actions_fid_info;
    if (actions_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    while (buffer != NULL) {
        if (actions_fid_info[buffer->id].action == 0) {
            buffer = buffer->next;
            continue;
        }
        BCMFP_ALLOC(action_data,
                    sizeof(bcmfp_action_data_t),
                    "bcmfpPddActionData");
        /*
         * Add the new node to the linked list of
         * action data.
         */
        action_data->next = *ad;
        *ad = action_data;

        /* Update the action. */
        action_data->action =
            actions_fid_info[buffer->id].action + buffer->idx;

        /* Update whether the action is enabled or not? */
        action_data->param = buffer->data;

        action_data = NULL;
        buffer = buffer->next;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_config_free(int unit,
                      bcmfp_pdd_config_t *pdd_config)
{
    bcmfp_action_data_t *action_data = NULL;

    SHR_FUNC_ENTER(unit);

    if (pdd_config == NULL) {
        SHR_EXIT();
    }

    action_data = pdd_config->action_data;
    while (action_data != NULL) {
        pdd_config->action_data = action_data->next;
        SHR_FREE(action_data);
        action_data = pdd_config->action_data;
    }

    SHR_FREE(pdd_config);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_config_get(int unit,
                     uint32_t trans_id,
                     bcmltd_sid_t tbl_id,
                     bcmfp_stage_id_t stage_id,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     bcmfp_pdd_config_t **config)
{
    uint32_t pdd_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    bcmfp_pdd_config_t *pdd_config = NULL;
    bcmfp_action_data_t *action_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.pdd_tbl == NULL ||
        stage->tbls.pdd_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(pdd_config,
                sizeof(bcmfp_pdd_config_t),
                "bcmfpPDDConfig");
    *config = pdd_config;

    /* Get the PDD ID */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_id_get(unit, stage, buffer, &pdd_id));
    pdd_config->pdd_id = pdd_id;

    /* If data is NULL no need to extract remaining config. */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the PDD action data. */
    buffer = (bcmltd_field_t *)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_action_data_get(unit, stage, buffer, &action_data));
    pdd_config->action_data = action_data;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_remote_config_get(int unit,
                            uint32_t trans_id,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_pdd_id_t pdd_id,
                            bcmfp_pdd_config_t **config)
{
    int rv;
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_pdd_config_t *pdd_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.pdd_tbl->key_fid;
    tbl_id = stage->tbls.pdd_tbl->sid;
    num_fid = stage->tbls.pdd_tbl->lrd_info.num_fid;

    /* Get the FP pdd LT entry saved in IMM. */
    rv = bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &pdd_id,
                                1,
                                num_fid,
                                &in,
                                &out,
                                &(stage->imm_buffers));

    /*
     * If not found found in IMM return with no error but
     * config is set to NULL.
     */
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    /* Assign key and data from in and out. */
    key = in.field[0];
    if (out.count != 0) {
        data = out.field[0];
    }

    /* Get the FP pdd LT entry config of ACL. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_config_get(unit,
                              trans_id,
                              tbl_id,
                              stage->stage_id,
                              key,
                              data,
                              &pdd_config));
    *config = pdd_config;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_pdd_action_fid_get(int unit,
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
        stage->tbls.pdd_tbl->actions_fid_info;
    if (actions_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < stage->tbls.pdd_tbl->fid_count; idx++) {
        if (actions_fid_info[idx].action == action) {
            *fid = idx;
            break;
        }
    }

    if (idx == stage->tbls.pdd_tbl->fid_count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_pdd_config_dump(int unit,
                      bcmltd_sid_t tbl_id,
                      bcmfp_stage_t *stage,
                      bcmfp_pdd_config_t *pdd_config)
{
    bcmfp_action_data_t *action_data = NULL;
    bcmlrd_field_def_t field_def;
    bcmlrd_fid_t fid = 0;
    bcmfp_action_t action;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(pdd_config, SHR_E_PARAM);

    if (stage->tbls.pdd_tbl->key_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "PDD ID = %d\n"),
            pdd_config->pdd_id));
    }

    action_data = pdd_config->action_data;
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "Actions enabled:\n")));
    while (action_data != NULL) {
        sal_memset(&field_def, 0, sizeof(bcmlrd_field_def_t));
        action = action_data->action;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_action_fid_get(unit,
                                      stage,
                                      action,
                                      &fid));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        fid,
                                        &field_def));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "    %s\n"),
                   field_def.name));

        action_data = action_data->next;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_pdd_config_t *pdd_config = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmfp_idp_info_t group_idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_pdd_id_t pdd_id = 0;
    bool not_mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
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

    /* Get the PDD configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_config_get(unit,
                              op_arg->trans_id,
                              tbl_id,
                              stage_id,
                              key,
                              non_default_data,
                              &pdd_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_config_dump(unit,
                                   tbl_id,
                                   stage,
                                   pdd_config));
    }

    pdd_id = pdd_config->pdd_id;

    /* Reset the existing PDD operational information if present. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_oper_info_reset(unit, stage->stage_id, pdd_id));

    /* Process group associated to this pdd. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_group_map_check(unit,
                                   stage_id,
                                   pdd_id,
                                   &not_mapped));

    /* Return if no groups are associated to the pdd */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
    group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    group_idp_info.sid_type = BCMFP_SID_TYPE_PDD_TEMPLATE;
    group_idp_info.key = key;
    group_idp_info.data = non_default_data;
    group_idp_info.output_fields = output_fields;
    group_idp_info.event_reason = BCMIMM_ENTRY_INSERT;
    group_idp_info.entry_state = state;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_group_map_traverse(unit,
                                      stage_id,
                                      pdd_id,
                                      bcmfp_group_idp_process,
                                      (void *)(&group_idp_info)));
exit:
    bcmfp_pdd_config_free(unit, pdd_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_config_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_pdd_config_t *pdd_config = NULL;
    uint16_t num_fid = 0;
    bcmltd_field_t *in_key = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_fields_t updated_fields;
    const bcmltd_field_t *updated_data = NULL;
    bcmfp_idp_info_t group_idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_pdd_id_t pdd_id = 0;
    bool not_mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&updated_fields, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.pdd_tbl->lrd_info.num_fid;
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
        (bcmfp_pdd_config_get(unit,
                              op_arg->trans_id,
                              tbl_id,
                              stage_id,
                              key,
                              updated_data,
                              &pdd_config));

   if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_config_dump(unit,
                                   tbl_id,
                                   stage,
                                   pdd_config));
    }

    /* Process group associated to this pdd. */
    pdd_id = pdd_config->pdd_id;

    /* Reset the existing PDD operational information if present. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_oper_info_reset(unit, stage->stage_id, pdd_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_group_map_check(unit,
                                   stage_id,
                                   pdd_id,
                                   &not_mapped));

    /* Return if no groups are associated to the pdd */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
    group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    group_idp_info.sid_type = BCMFP_SID_TYPE_PDD_TEMPLATE;
    group_idp_info.key = key;
    group_idp_info.data = updated_data;
    group_idp_info.output_fields = output_fields;
    group_idp_info.event_reason = BCMIMM_ENTRY_UPDATE;
    group_idp_info.entry_state = state;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_group_map_traverse(unit,
                                      stage_id,
                                      pdd_id,
                                      bcmfp_group_idp_process,
                                      (void *)(&group_idp_info)));

exit:
    bcmfp_pdd_config_free(unit, pdd_config);
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_pdd_config_t *pdd_config = NULL;
    uint16_t num_fid = 0;
    bcmltd_field_t *in_key = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_fields_t updated_fields;
    const bcmltd_field_t *updated_data = NULL;
    bcmfp_idp_info_t group_idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_pdd_id_t pdd_id = 0;
    bool not_mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&updated_fields, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.pdd_tbl->lrd_info.num_fid;
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
        (bcmfp_pdd_config_get(unit,
                              op_arg->trans_id,
                              tbl_id,
                              stage_id,
                              key,
                              updated_data,
                              &pdd_config));

   if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_config_dump(unit,
                                   tbl_id,
                                   stage,
                                   pdd_config));
    }

    /* Process group associated to this pdd. */
    pdd_id = pdd_config->pdd_id;

    /* Reset the existing PDD operational information if present. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_oper_info_reset(unit, stage->stage_id, pdd_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_group_map_check(unit,
                                   stage_id,
                                   pdd_id,
                                   &not_mapped));

    /* Return if no groups are associated to the pdd */
    if (not_mapped == FALSE) {
        sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
        group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
        group_idp_info.sid_type = BCMFP_SID_TYPE_PDD_TEMPLATE;
        group_idp_info.key = key;
        group_idp_info.data = updated_data;
        group_idp_info.output_fields = output_fields;
        group_idp_info.event_reason = BCMIMM_ENTRY_DELETE;
        group_idp_info.entry_state = state;


        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_group_map_traverse(unit,
                                          stage_id,
                                          pdd_id,
                                          bcmfp_group_idp_process,
                                          (void *)(&group_idp_info)));
   }

exit:
    bcmfp_pdd_config_free(unit, pdd_config);
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FUNC_EXIT();
}
