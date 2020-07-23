/*! \file bcmfp_cth_entry.c
 *
 * APIs for FP entry template LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate entry config provided
 * using entry template LTs.
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
#include <bcmfp/bcmfp_cth_filter.h>
#include <bcmfp/bcmfp_trie_mgmt.h>
#include <bcmfp/bcmfp_compression_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_entry_id_get(int unit,
                   bcmfp_stage_t *stage,
                   bcmltd_field_t *buffer,
                   uint32_t *entry_id)
{
    uint32_t entry_id_fid = 0;
    uint64_t entry_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_id, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    entry_id_fid = stage->tbls.entry_tbl->key_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             entry_id_fid,
                             (void *)buffer,
                             &entry_id_u64),
            SHR_E_NOT_FOUND);

    *entry_id = entry_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_group_id_get(int unit,
                   bcmfp_stage_t *stage,
                   bcmltd_field_t *buffer,
                   uint32_t *group_id)
{
    uint32_t group_id_fid = 0;
    uint64_t group_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(group_id, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    group_id_fid = stage->tbls.entry_tbl->group_id_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             group_id_fid,
                             (void *)buffer,
                             &group_id_u64),
         SHR_E_NOT_FOUND);

    *group_id = group_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_rule_id_get(int unit,
                   bcmfp_stage_t *stage,
                   bcmltd_field_t *buffer,
                   uint32_t *rule_id)
{
    uint32_t rule_id_fid = 0;
    uint64_t rule_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(rule_id, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    rule_id_fid = stage->tbls.entry_tbl->rule_id_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             rule_id_fid,
                             (void *)buffer,
                             &rule_id_u64),
            SHR_E_NOT_FOUND);

    *rule_id = rule_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_policy_id_get(int unit,
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

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    policy_id_fid = stage->tbls.entry_tbl->policy_id_fid;

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
bcmfp_entry_pdd_id_get(int unit,
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

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    pdd_id_fid = stage->tbls.entry_tbl->pdd_id_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             pdd_id_fid,
                             (void *)buffer,
                             &pdd_id_u64),
            SHR_E_NOT_FOUND);

    *pdd_id = pdd_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_sbr_id_get(int unit,
                   bcmfp_stage_t *stage,
                   bcmltd_field_t *buffer,
                   uint32_t *sbr_id)
{
    uint32_t sbr_id_fid = 0;
    uint64_t sbr_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(sbr_id, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sbr_id_fid = stage->tbls.entry_tbl->sbr_id_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             sbr_id_fid,
                             (void *)buffer,
                             &sbr_id_u64),
            SHR_E_NOT_FOUND);

    *sbr_id = sbr_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_meter_id_get(int unit,
                  bcmfp_stage_t *stage,
                  bcmltd_field_t *buffer,
                  uint32_t *meter_id)
{
    uint32_t meter_id_fid = 0;
    uint64_t meter_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(meter_id, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    meter_id_fid = stage->tbls.entry_tbl->meter_id_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             meter_id_fid,
                             (void *)buffer,
                             &meter_id_u64),
            SHR_E_NOT_FOUND);

    *meter_id = meter_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_ctr_entry_id_get(int unit,
                      bcmfp_stage_t *stage,
                      bcmltd_field_t *buffer,
                      uint32_t *ctr_entry_id)
{
    uint32_t ctr_id_fid = 0;
    uint64_t ctr_entry_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_entry_id, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ctr_id_fid = stage->tbls.entry_tbl->ctr_id_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             ctr_id_fid,
                             (void *)buffer,
                             &ctr_entry_id_u64),
            SHR_E_NOT_FOUND);

    *ctr_entry_id = ctr_entry_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the enhanced flex counter action ID and object ID
 * from FP entry LT entry.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage              BCMFP stage.
 * \param [in] buffer             Field buffer
 * \param [out] flex_ctr_action   Flex ctr action ID
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
static int
bcmfp_entry_eflex_ctr_params_get(int unit,
                      bcmfp_stage_t *stage,
                      bcmltd_field_t *buffer,
                      uint32_t *flex_ctr_action)
{
    uint32_t flex_ctr_action_fid = 0;
    uint64_t flex_ctr_action_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(flex_ctr_action, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    flex_ctr_action_fid = stage->tbls.entry_tbl->flex_ctr_action_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             flex_ctr_action_fid,
                             (void *)buffer,
                             &flex_ctr_action_u64),
            SHR_E_NOT_FOUND);

    *flex_ctr_action = flex_ctr_action_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_priority_get(int unit,
                     bcmfp_stage_t *stage,
                     bcmltd_field_t *buffer,
                     uint32_t *priority)
{
    uint32_t priority_fid = 0;
    uint64_t priority_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(priority, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    priority_fid = stage->tbls.entry_tbl->priority_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             priority_fid,
                             (void *)buffer,
                             &priority_u64),
            SHR_E_NOT_FOUND);

    *priority = priority_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_state_get(int unit,
                      bcmfp_stage_t *stage,
                      bcmltd_field_t *buffer,
                      bcmfp_entry_state_t *state)
{
    uint32_t state_fid = 0;
    uint64_t state_u64 = BCMFP_ENTRY_SUCCESS;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(state, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    state_fid = stage->tbls.entry_tbl->oprtnl_state_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             state_fid,
                             (void *)buffer,
                             &state_u64),
            SHR_E_NOT_FOUND);

    *state = state_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_enable_get(int unit,
                      bcmfp_stage_t *stage,
                      bcmltd_field_t *buffer,
                      bool *enable)
{
    uint32_t enable_fid = 0;
    uint64_t enable_u64 = 1;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    enable_fid = stage->tbls.entry_tbl->enable_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             enable_fid,
                             (void *)buffer,
                             &enable_u64),
            SHR_E_NOT_FOUND);

    *enable = enable_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_flex_ctr_params_get(int unit,
                     bcmfp_stage_t *stage,
                     bcmltd_field_t *buffer,
                     uint32_t *base_idx,
                     uint32_t *offset_mode,
                     uint32_t *pool_id)
{
    uint32_t base_idx_fid = 0;
    uint64_t base_idx_u64 = 0;
    uint32_t offset_mode_fid = 0;
    uint64_t offset_mode_u64 = 0;
    uint32_t pool_id_fid = 0;
    uint64_t pool_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(base_idx, SHR_E_PARAM);
    SHR_NULL_CHECK(offset_mode, SHR_E_PARAM);
    SHR_NULL_CHECK(pool_id, SHR_E_PARAM);

    if (stage->tbls.entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    base_idx_fid = stage->tbls.entry_tbl->flex_ctr_base_idx_fid;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             base_idx_fid,
                             (void *)buffer,
                             &base_idx_u64),
            SHR_E_NOT_FOUND);
    *base_idx = base_idx_u64 & 0xFFFFFFFF;

    offset_mode_fid = stage->tbls.entry_tbl->flex_ctr_offset_mode_fid;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             offset_mode_fid,
                             (void *)buffer,
                             &offset_mode_u64),
            SHR_E_NOT_FOUND);
    *offset_mode = offset_mode_u64 & 0xFFFFFFFF;

    pool_id_fid = stage->tbls.entry_tbl->flex_ctr_pool_id_fid;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             pool_id_fid,
                             (void *)buffer,
                             &pool_id_u64),
            SHR_E_NOT_FOUND);
    *pool_id = pool_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_config_dump(int unit,
                   bcmltd_sid_t tbl_id,
                   bcmfp_stage_id_t stage_id,
                   bcmfp_entry_config_t *entry_config)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.entry_tbl == NULL ||
        stage->tbls.entry_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Dump the entry ID */
    if (stage->tbls.entry_tbl->key_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entry ID = %d\n"),
            entry_config->entry_id));
    }

    /* Dump the entries group ID */
    if (stage->tbls.entry_tbl->group_id_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entries group ID = %d\n"),
            entry_config->group_id));
    }

    /* Dump the entries rule ID */
    if (stage->tbls.entry_tbl->rule_id_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entries rule ID = %d\n"),
            entry_config->rule_id));
    }

    /* Dump the entries policy ID */
    if (stage->tbls.entry_tbl->policy_id_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entries policy ID = %d\n"),
            entry_config->policy_id));
    }

    /* Dump the entries meter ID */
    if (stage->tbls.entry_tbl->meter_id_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entries meter ID = %d\n"),
            entry_config->meter_id));
    }

    /* Dump the entries counter ID */
    if (stage->tbls.entry_tbl->ctr_id_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entries counter ID = %d\n"),
            entry_config->ctr_entry_id));
    }

    /* Dump the entries flex counter parameters */
    if (stage->tbls.entry_tbl->flex_ctr_base_idx_fid != 0 &&
        stage->tbls.entry_tbl->flex_ctr_offset_mode_fid != 0 &&
        stage->tbls.entry_tbl->flex_ctr_pool_id_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entries flex counter base index = %d\n"),
            entry_config->flex_ctr_base_idx));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entries flex counter offset mode = %d\n"),
            entry_config->flex_ctr_offset_mode));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entries flex counter pool id = %d\n"),
            entry_config->flex_ctr_pool_id));
    }

    /* Dump the entry priority */
    if (stage->tbls.entry_tbl->priority_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entry priority = %d\n"),
            entry_config->priority));
    }

    /* Dump the entry enable */
    if (stage->tbls.entry_tbl->enable_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Entry enable = %d\n"),
            entry_config->enable));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmfp_entry_config_t **config)
{
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    uint32_t entry_id = 0;
    uint32_t group_id = 0;
    uint32_t rule_id = 0;
    uint32_t policy_id = 0;
    uint32_t pdd_id = 0;
    uint32_t sbr_id = 0;
    uint32_t meter_id = 0;
    uint32_t ctr_entry_id = 0;
    uint32_t base_idx = 0;
    uint32_t pool_id = 0;
    uint32_t offset_mode = 0;
    uint32_t priority = 0;
    uint32_t flex_ctr_action = 0;
    bool enable = 0;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.entry_tbl == NULL ||
        stage->tbls.entry_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(entry_config,
                sizeof(bcmfp_entry_config_t),
                "bcmfpEntryConfig");
    *config = entry_config;

    /* Get the entry ID */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_id_get(unit, stage, buffer, &entry_id));
    entry_config->entry_id = entry_id;

    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the entries group ID */
    if (stage->tbls.entry_tbl->group_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_group_id_get(unit, stage, buffer, &group_id));
        entry_config->group_id = group_id;
    }

    /* Get the entries rule ID */
    if (stage->tbls.entry_tbl->rule_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_rule_id_get(unit, stage, buffer, &rule_id));
        entry_config->rule_id = rule_id;
    }

    /* Get the entries policy ID */
    if (stage->tbls.entry_tbl->policy_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_policy_id_get(unit, stage, buffer, &policy_id));
        entry_config->policy_id = policy_id;
    }

    /* Get the entries PDD ID */
    if (stage->tbls.entry_tbl->pdd_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_pdd_id_get(unit, stage, buffer, &pdd_id));
        entry_config->pdd_id = pdd_id;
    }

    /* Get the entries SBR ID */
    if (stage->tbls.entry_tbl->sbr_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_sbr_id_get(unit, stage, buffer, &sbr_id));
        entry_config->sbr_id = sbr_id;
    }

    /* Get the entries meter ID */
    if (stage->tbls.entry_tbl->meter_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_meter_id_get(unit, stage, buffer, &meter_id));
        entry_config->meter_id = meter_id;
    }

    /* Get the entries counter ID */
    if (stage->tbls.entry_tbl->ctr_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_ctr_entry_id_get(unit, stage, buffer, &ctr_entry_id));
        entry_config->ctr_entry_id = ctr_entry_id;
    }

    /* Get the entries flex counter parameters */
    if (stage->tbls.entry_tbl->flex_ctr_base_idx_fid != 0 &&
        stage->tbls.entry_tbl->flex_ctr_offset_mode_fid != 0 &&
        stage->tbls.entry_tbl->flex_ctr_pool_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_flex_ctr_params_get(unit,
                                             stage,
                                             buffer,
                                             &base_idx,
                                             &offset_mode,
                                             &pool_id));
        entry_config->flex_ctr_base_idx = base_idx;
        entry_config->flex_ctr_offset_mode = offset_mode;
        entry_config->flex_ctr_pool_id = pool_id;
    }

    /* Get the entry flex ctr action and object */
    if (stage->tbls.entry_tbl->flex_ctr_action_fid != 0)  {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_eflex_ctr_params_get(unit,
                                              stage,
                                              buffer,
                                              &flex_ctr_action));
        entry_config->flex_ctr_action = flex_ctr_action;
    }

    /* Get the entry priority */
    if (stage->tbls.entry_tbl->priority_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_priority_get(unit, stage, buffer, &priority));
        entry_config->priority = priority;
    }

    /* Get the entry operational state. */
    if (stage->tbls.entry_tbl->oprtnl_state_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_state_get(unit, stage, buffer, &state));
        entry_config->entry_state = state;
    }

    /* Get the entry enable bit. */
    if (stage->tbls.entry_tbl->enable_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_enable_get(unit, stage, buffer, &enable));
        entry_config->enable = enable;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_config_copy(int unit,
                        bcmfp_entry_config_t *src_econfig,
                        bcmfp_entry_config_t *dst_econfig)
{
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_econfig, SHR_E_PARAM);
    SHR_NULL_CHECK(dst_econfig, SHR_E_PARAM);

    size = sizeof(bcmfp_entry_config_t);
    sal_memcpy(dst_econfig, src_econfig, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_remote_config_get(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_entry_config_t **config)
{
    int rv;
    bcmltd_fid_t key_fid;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmltd_sid_t tbl_id;
    bcmfp_entry_config_t *entry_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.entry_tbl->key_fid;
    tbl_id = stage->tbls.entry_tbl->sid;
    num_fid = stage->tbls.entry_tbl->lrd_info.num_fid;

    /* Get the FP entry LT entry saved in IMM. */
    rv = bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
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

    /* Get the FP entry LT entry config of ACL. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage->stage_id,
                                key,
                                data,
                                &entry_config));

    *config = entry_config;
    entry_config = NULL;
exit:
    SHR_FREE(entry_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_config_free(int unit,
                        bcmfp_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_FREE(config);

    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_list_compress_insert(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t child_eid,
                              bcmfp_entry_id_t parent_eid,
                              bcmfp_idp_info_t *idp_info)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    /* Add entry Id to list compressed entry ID mapping. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_child_entry_map_add(unit,
                                               stage_id,
                                               parent_eid,
                                               child_eid));

    /* Add parent entry Id to list compressed entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_map_add(unit,
                                               stage_id,
                                               child_eid,
                                               parent_eid));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_list_compress_delete(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t child_eid,
                              bcmfp_entry_id_t parent_eid,
                              bcmfp_idp_info_t *idp_info)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    /*
     * Get the parent entry ID for the given list compressed
     * entry ID.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_get(unit,
                                                stage_id,
                                                child_eid,
                                                &parent_eid));

    /* Delete entry Id from list compressed entry ID mapping. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_child_entry_map_delete(unit,
                                               stage_id,
                                               parent_eid,
                                               child_eid));

    /* Delete parent entry Id from list compressed entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_map_delete(unit,
                                               stage_id,
                                               child_eid,
                                               parent_eid));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_list_compress_update(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t child_eid,
                              bcmfp_entry_id_t parent_eid,
                              bcmfp_idp_info_t *idp_info)
{
    bcmfp_filter_t filter;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_entry_config_t *entry_config = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    /*
     * Delete all the list compressed child entries from
     * parent entry id and add them(except first child entry)
     * to the first child entry as child entries.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_child_entry_map_move(unit,
                                               stage_id,
                                               parent_eid,
                                               &child_eid));

    /* Copy the parent entry filter to child entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       child_eid,
                                       &entry_config));
    sal_memset(&filter, 0, sizeof(bcmfp_filter_t));
    filter.entry_config = entry_config;
    group_id = filter.entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter.group_oper_info = opinfo;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_copy_cid_update(unit,
                                      idp_info->op_arg,
                                      stage_id,
                                      idp_info->tbl_id,
                                      idp_info->compress_type,
                                      idp_info->cid_next,
                                      idp_info->cid_mask_next,
                                      parent_eid,
                                      &filter));

exit:
    bcmfp_entry_config_free(unit,entry_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_idp_list_compress_process(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t child_eid,
                              bcmfp_entry_id_t parent_eid,
                              void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (idp_info->event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_list_compress_insert(unit,
                                                  stage_id,
                                                  child_eid,
                                                  parent_eid,
                                                  idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_list_compress_delete(unit,
                                                  stage_id,
                                                  child_eid,
                                                  parent_eid,
                                                  idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_list_compress_update(unit,
                                                  stage_id,
                                                  child_eid,
                                                  parent_eid,
                                                  idp_info));

    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_ckey_insert(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            uint32_t ckey,
                            void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ckey_entry_map_add(unit,
                                  stage_id,
                                  idp_info->compress_type,
                                  idp_info->ckey,
                                  idp_info->prefix,
                                  entry_id));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_ckey_delete(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            uint32_t ckey,
                            void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ckey_entry_map_delete(unit,
                                     stage_id,
                                     idp_info->compress_type,
                                     idp_info->ckey,
                                     idp_info->prefix,
                                     entry_id));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_ckey_update(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            uint32_t ckey,
                            void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;
    bool list_compressed = FALSE, map_not_found = FALSE;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t filter;
    bcmfp_entry_id_t parent_eid = 0;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_cid_update_cb_info_t *cb_info = NULL;
    uint8_t compress_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    cb_info = idp_info->generic_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_status_get(unit,
                                              stage_id,
                                              entry_id,
                                              &list_compressed));
    if (idp_info->cid_update_flags == BCMFP_TRIE_NODE_LIST_COMPRESSED &&
        list_compressed == TRUE) {
        SHR_EXIT();
    }

    if (list_compressed == TRUE &&
        idp_info->cid_update_flags == BCMFP_TRIE_NODE_WIDTH_COMPRESSED) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_list_compress_parent_entry_get(unit,
                                                        stage_id,
                                                        entry_id,
                                                        &parent_eid));
        if (cb_info->event == BCMIMM_ENTRY_INSERT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_remote_config_get(unit,
                                               idp_info->op_arg,
                                               stage_id,
                                               entry_id,
                                               &entry_config));
            sal_memset(&filter, 0, sizeof(bcmfp_filter_t));
            filter.entry_config = entry_config;
            group_id  = filter.entry_config->group_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_oper_info_get(unit, stage_id, group_id, &opinfo));
            filter.group_oper_info = opinfo;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_filter_copy_cid_update(unit,
                                              idp_info->op_arg,
                                              stage_id,
                                              idp_info->tbl_id,
                                              idp_info->compress_type,
                                              idp_info->cid_next,
                                              idp_info->cid_mask_next,
                                              parent_eid,
                                              &filter));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_list_compress_child_entry_map_delete(unit,
                                                           stage_id,
                                                           parent_eid,
                                                           entry_id));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_list_compress_parent_entry_map_delete(unit,
                                                            stage_id,
                                                            entry_id,
                                                            parent_eid));
         } else if (cb_info->event == BCMIMM_ENTRY_DELETE) {
            /*
             * Check if the parent entry has
             * list compressed child nodes attached to it.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_list_compress_child_entry_map_check(unit,
                                                           stage_id,
                                                           parent_eid,
                                                           &map_not_found));
            /*
             * If map is found, move child nodes to current entry,
             * as the parent entry is going to be deleted.
             */
            if (map_not_found == FALSE) {
                idp_info->event_reason = BCMIMM_ENTRY_UPDATE;
                idp_info->list_compress_child_entry_id = entry_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_entry_idp_list_compress_process(unit,
                                                           stage_id,
                                                           entry_id,
                                                           parent_eid,
                                                           (void *)idp_info));
            }
        }
    } else if (list_compressed == FALSE) {
        /*
         * Update the CID and CID_MASK in FP entry to
         * new values.
         */
        compress_type = idp_info->compress_type;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_remote_config_get(unit,
                                           idp_info->op_arg,
                                           stage_id,
                                           entry_id,
                                           &entry_config));
        sal_memset(&filter, 0, sizeof(bcmfp_filter_t));
        filter.entry_config = entry_config;
        group_id  = filter.entry_config->group_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_oper_info_get(unit,
                                       stage_id,
                                       group_id,
                                       &opinfo));
        filter.group_oper_info = opinfo;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_cid_update(unit,
                                     idp_info->op_arg,
                                     stage_id,
                                     idp_info->tbl_id,
                                     compress_type,
                                     idp_info->cid_next,
                                     idp_info->cid_mask_next,
                                     &filter));
    }
exit:
    bcmfp_entry_config_free(unit, entry_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_idp_ckey_process(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             uint32_t ckey,
                             void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (idp_info->event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ckey_insert(unit,
                                         stage_id,
                                         entry_id,
                                         ckey,
                                         idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ckey_delete(unit,
                                         stage_id,
                                         entry_id,
                                         ckey,
                                         idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ckey_update(unit,
                                         stage_id,
                                         entry_id,
                                         ckey,
                                         idp_info));

    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_ctr_entry_insert(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_entry_id_t entry_id,
                                 bcmfp_ctr_entry_id_t ctr_entry_id,
                                 bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_ctr_entry_config_t *ctr_entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.ctr_entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_config_get(unit,
                                    idp_info->op_arg,
                                    tbl_id,
                                    stage_id,
                                    idp_info->key,
                                    idp_info->data,
                                    &ctr_entry_config));
    filter->ctr_entry_config = ctr_entry_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    /* Create the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    filter->group_oper_info = opinfo;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_ctr_entry_update(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_entry_id_t entry_id,
                                 bcmfp_ctr_entry_id_t ctr_entry_id,
                                 bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state_curr;
    bcmfp_entry_state_t entry_state_prev;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_ctr_entry_config_t *ctr_entry_config = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.ctr_entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_config_get(unit,
                                    idp_info->op_arg,
                                    tbl_id,
                                    stage_id,
                                    idp_info->key,
                                    idp_info->data,
                                    &ctr_entry_config));
    filter->ctr_entry_config = ctr_entry_config;

    /*
     * Keep the group id  and entry state for incrementing
     * or decrementing its entry  reference count at the
     * end of this function.
     */
    group_id = filter->entry_config->group_id;
    entry_state_prev = filter->entry_config->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /* Update the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_update(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             NULL,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS &&
        entry_state_prev != BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    if (filter->entry_state != BCMFP_ENTRY_SUCCESS &&
        entry_state_prev == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count--;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state_curr = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state_curr,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_ctr_entry_delete(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_entry_id_t entry_id,
                                 bcmfp_ctr_entry_id_t ctr_entry_id,
                                 bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    filter->group_oper_info = opinfo;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        /* Create the filter corresponding to entry ID. */
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_CTR_ENTRY;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;

        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_COUNTER_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_idp_ctr_entry_process(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_entry_id_t entry_id,
                                  bcmfp_ctr_entry_id_t ctr_entry_id,
                                  void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    /*
     * Enable atomic transaction because there might be
     * multiple PTM opcode requests for single LT opcode
     * request. Because single counter can be associated
     * to multiple entries.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_atomic_state_set(unit, idp_info->op_arg->trans_id, TRUE));

    if (idp_info->event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ctr_entry_insert(unit,
                                              stage_id,
                                              entry_id,
                                              ctr_entry_id,
                                              idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ctr_entry_delete(unit,
                                              stage_id,
                                              entry_id,
                                              ctr_entry_id,
                                              idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ctr_entry_update(unit,
                                              stage_id,
                                              entry_id,
                                              ctr_entry_id,
                                              idp_info));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_meter_insert(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_meter_id_t meter_id,
                             bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    /* Create the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    filter->group_oper_info = opinfo;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_meter_update(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_meter_id_t meter_id,
                             bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state_curr;
    bcmfp_entry_state_t entry_state_prev;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_group_id_t group_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Keep the group id  and entry state for incrementing
     * or decrementing its entry  reference count at the
     * end of this function.
     */
    group_id = filter->entry_config->group_id;
    entry_state_prev = filter->entry_config->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /* Update the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_update(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             NULL,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS &&
        entry_state_prev != BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    if (filter->entry_state != BCMFP_ENTRY_SUCCESS &&
        entry_state_prev == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count--;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state_curr = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state_curr,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_meter_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_meter_id_t meter_id,
                             bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    filter->group_oper_info = opinfo;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        /* Create the filter corresponding to entry ID. */
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_METER_TEMPLATE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;

        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_METER_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_idp_meter_process(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_meter_id_t meter_id,
                              void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (idp_info->event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_meter_insert(unit,
                                          stage_id,
                                          entry_id,
                                          meter_id,
                                          idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_meter_delete(unit,
                                          stage_id,
                                          entry_id,
                                          meter_id,
                                          idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_meter_update(unit,
                                          stage_id,
                                          entry_id,
                                          meter_id,
                                          idp_info));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_group_insert(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_group_id_t group_id,
                             bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.group_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_config_get(unit,
                                idp_info->op_arg,
                                tbl_id,
                                stage_id,
                                idp_info->key,
                                idp_info->data,
                                &group_config));
    filter->group_config = group_config;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /* Create the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_group_update(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_group_id_t group_id,
                             bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state_curr;
    bcmfp_entry_state_t entry_state_prev;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_group_config_t *group_config = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.group_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_config_get(unit,
                                idp_info->op_arg,
                                tbl_id,
                                stage_id,
                                idp_info->key,
                                idp_info->data,
                                &group_config));
    filter->group_config = group_config;

    /*
     * Keep the entry state for incrementing or
     * decrementing its entry reference count at the
     * end of this function.
     */
    entry_state_prev = filter->entry_config->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /* Update the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_update(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             NULL,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS &&
        entry_state_prev != BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    if (filter->entry_state != BCMFP_ENTRY_SUCCESS &&
        entry_state_prev == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count--;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state_curr = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state_curr,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_group_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_group_id_t group_id,
                             bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_GRP_TEMPLATE;
        /* Create the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;

        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_GROUP_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_idp_group_process(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_group_id_t group_id,
                              void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    /*
     * Enable atomic transaction because there might be
     * multiple PTM opcode requests for single LT opcode
     * request. Because single group can be associated
     * to multiple entries.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_atomic_state_set(unit,
                                      idp_info->op_arg->trans_id,
                                      TRUE));

    if (idp_info->event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_group_insert(unit,
                                          stage_id,
                                          entry_id,
                                          group_id,
                                          idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_group_delete(unit,
                                          stage_id,
                                          entry_id,
                                          group_id,
                                          idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_group_update(unit,
                                          stage_id,
                                          entry_id,
                                          group_id,
                                          idp_info));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_rule_insert(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            bcmfp_rule_id_t rule_id,
                            bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.rule_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_config_get(unit,
                               idp_info->op_arg,
                               tbl_id,
                               stage_id,
                               idp_info->key,
                               idp_info->data,
                               &rule_config));
    filter->rule_config = rule_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    if (!(idp_info->entry_or_group_count == 1 &&
        opinfo != NULL && opinfo->ref_count != 0)) {
       /*
        * Enable atomic transaction because there might be
        * multiple PTM opcode requests for single LT opcode
        * request. Because single rule can be associated
        * to multiple entries.
        */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_trans_atomic_state_set(unit,
                                         idp_info->op_arg->trans_id,
                                         TRUE));
    }

    /* Create the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_rule_update(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            bcmfp_rule_id_t rule_id,
                            bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state_curr;
    bcmfp_entry_state_t entry_state_prev;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_rule_config_t *rule_config = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.rule_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_config_get(unit,
                               idp_info->op_arg,
                               tbl_id,
                               stage_id,
                               idp_info->key,
                               idp_info->data,
                               &rule_config));
    filter->rule_config = rule_config;

    /*
     * Keep the group id  and entry state for incrementing
     * or decrementing its entry  reference count at the
     * end of this function.
     */
    group_id = filter->entry_config->group_id;
    entry_state_prev = filter->entry_config->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /*
     * Enable atomic transaction because there might be
     * multiple PTM opcode requests for single LT opcode
     * request. Because single rule can be associated
     * to multiple entries.
     */
    if ((!(idp_info->entry_or_group_count == 1 &&
           entry_state_prev == BCMFP_ENTRY_SUCCESS)) ||
         BCMFP_STAGE_FLAGS_TEST(stage,
                     BCMFP_STAGE_ENTRY_TYPE_HASH)) {
       /* Enable atomic transaction if hash tables are included
        * in the stage updates as hash entry updates are deleted
        * and inserted again resulting in multiple PT requests.
        */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_trans_atomic_state_set(unit,
                                         idp_info->op_arg->trans_id,
                                         TRUE));
    }

    if (opinfo != NULL) {
        if (idp_info->entry_or_group_count == 1 &&
            opinfo->ref_count == 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_trans_atomic_state_set(unit,
                                    idp_info->op_arg->trans_id,
                                    TRUE));
        }
    }

    /* Update the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_update(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             NULL,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS &&
        entry_state_prev != BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    if (filter->entry_state != BCMFP_ENTRY_SUCCESS &&
        entry_state_prev == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count--;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state_curr = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state_curr,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_rule_delete(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            bcmfp_rule_id_t rule_id,
                            bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    if (!(idp_info->entry_or_group_count == 1 &&
        opinfo != NULL && opinfo->ref_count != 1)) {
       /*
        * Enable atomic transaction because there might be
        * multiple PTM opcode requests for single LT opcode
        * request. Because single rule can be associated
        * to multiple entries.
        */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_trans_atomic_state_set(unit,
                                         idp_info->op_arg->trans_id,
                                         TRUE));
    }
    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_RULE_TEMPLATE;
        /* Create the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;

        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_RULE_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_idp_rule_process(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_rule_id_t rule_id,
                             void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (idp_info->event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_rule_insert(unit,
                                         stage_id,
                                         entry_id,
                                         rule_id,
                                         idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_rule_delete(unit,
                                         stage_id,
                                         entry_id,
                                         rule_id,
                                         idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_rule_update(unit,
                                         stage_id,
                                         entry_id,
                                         rule_id,
                                         idp_info));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_policy_insert(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_policy_id_t policy_id,
                              bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_policy_config_t *policy_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.policy_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_config_get(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage_id,
                                 idp_info->key,
                                 idp_info->data,
                                 &policy_config));
    filter->policy_config = policy_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    if (!(idp_info->entry_or_group_count == 1 &&
        opinfo != NULL && opinfo->ref_count != 0)) {
       /*
        * Enable atomic transaction because there might be
        * multiple PTM opcode requests for single LT opcode
        * request. Because single rule can be associated
        * to multiple entries.
        */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_trans_atomic_state_set(unit,
                                         idp_info->op_arg->trans_id,
                                         TRUE));
    }

    /* Create the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_policy_update(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_policy_id_t policy_id,
                              bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state_curr;
    bcmfp_entry_state_t entry_state_prev;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_policy_config_t *policy_config = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.policy_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_config_get(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage_id,
                                 idp_info->key,
                                 idp_info->data,
                                 &policy_config));
    filter->policy_config = policy_config;

    /*
     * Keep the group id  and entry state for incrementing
     * or decrementing its entry  reference count at the
     * end of this function.
     */
    group_id = filter->entry_config->group_id;
    entry_state_prev = filter->entry_config->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /*
     * Enable atomic transaction because there might be
     * multiple PTM opcode requests for single LT opcode
     * request. Because single rule can be associated
     * to multiple entries.
     */
    if ((!(idp_info->entry_or_group_count == 1 &&
           entry_state_prev == BCMFP_ENTRY_SUCCESS)) ||
         BCMFP_STAGE_FLAGS_TEST(stage,
                     BCMFP_STAGE_ENTRY_TYPE_HASH)) {
       /* Enable atomic transaction if hash tables are included
        * in the stage updates as hash entry updates are deleted
        * and inserted again resulting in multiple PT requests.
        */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_trans_atomic_state_set(unit,
                                         idp_info->op_arg->trans_id,
                                         TRUE));
    }

    if (opinfo != NULL) {
        if (idp_info->entry_or_group_count == 1 &&
            opinfo->ref_count == 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_trans_atomic_state_set(unit,
                                    idp_info->op_arg->trans_id,
                                    TRUE));
        }
    }

    /* Update the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_update(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             NULL,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS &&
        entry_state_prev != BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    if (filter->entry_state != BCMFP_ENTRY_SUCCESS &&
        entry_state_prev == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count--;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state_curr = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state_curr,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_policy_delete(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_policy_id_t policy_id,
                              bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    if (!(idp_info->entry_or_group_count == 1 &&
        opinfo != NULL && opinfo->ref_count != 1)) {
       /*
        * Enable atomic transaction because there might be
        * multiple PTM opcode requests for single LT opcode
        * request. Because single rule can be associated
        * to multiple entries.
        */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_trans_atomic_state_set(unit,
                                         idp_info->op_arg->trans_id,
                                         TRUE));
    }

    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_POLICY_TEMPLATE;
        /* Create the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;

        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_POLICY_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_idp_policy_process(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_entry_id_t entry_id,
                               bcmfp_policy_id_t policy_id,
                               void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (idp_info->event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_policy_insert(unit,
                                           stage_id,
                                           entry_id,
                                           policy_id,
                                           idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_policy_delete(unit,
                                           stage_id,
                                           entry_id,
                                           policy_id,
                                           idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_policy_update(unit,
                                           stage_id,
                                           entry_id,
                                           policy_id,
                                           idp_info));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_sbr_insert(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_entry_id_t entry_id,
                           bcmfp_policy_id_t policy_id,
                           bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_sbr_config_t *sbr_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.sbr_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_config_get(unit,
                              idp_info->op_arg,
                              tbl_id,
                              stage_id,
                              idp_info->key,
                              idp_info->data,
                              &sbr_config));
    filter->sbr_config = sbr_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    if (!(idp_info->entry_or_group_count == 1 &&
        opinfo != NULL && opinfo->ref_count != 0)) {
       /*
        * Enable atomic transaction because there might be
        * multiple PTM opcode requests for single LT opcode
        * request. Because single rule can be associated
        * to multiple entries.
        */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_trans_atomic_state_set(unit,
                                         idp_info->op_arg->trans_id,
                                         TRUE));
    }

    /* Create the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_sbr_update(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_entry_id_t entry_id,
                           bcmfp_sbr_id_t sbr_id,
                           bcmfp_idp_info_t *idp_info)
{
    bcmfp_filter_t *filter = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_sid_t tbl_id;
    bcmfp_group_id_t group_id = 0;
    bcmfp_pdd_config_t *pdd_config = NULL;
    bcmfp_sbr_config_t *sbr_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    tbl_id = stage->tbls.sbr_tbl->sid;
    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpEntryIdpSbrUpdate");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_config_get(unit,
                              idp_info->op_arg,
                              tbl_id,
                              stage_id,
                              idp_info->key,
                              idp_info->data,
                              &sbr_config));
    filter->sbr_config = sbr_config;

    /* Get the entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * If entry was not operational its SBR might not be installed
     * in hardware until unless the same SBR is associated with
     * some other entry which is operational. And SBR will be updated
     * when this function is called for that entry.
     */
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        SHR_EXIT();
    }

    group_id = filter->entry_config->group_id;

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &opinfo));
    filter->group_oper_info = opinfo;
    /* Get the group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));
    filter->group_config = group_config;

    /* Get the PDD configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
    (bcmfp_pdd_remote_config_get(unit,
                                 idp_info->op_arg->trans_id,
                                 stage_id,
                                 filter->group_config->pdd_id,
                                 &pdd_config));
    filter->pdd_config = pdd_config;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_update_sbr(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage_id,
                                 filter));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_sbr_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_entry_id_t entry_id,
                           bcmfp_policy_id_t sbr_id,
                           bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    if (!(idp_info->entry_or_group_count == 1 &&
        opinfo != NULL && opinfo->ref_count != 1)) {
       /*
        * Enable atomic transaction because there might be
        * multiple PTM opcode requests for single LT opcode
        * request. Because single rule can be associated
        * to multiple entries.
        */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_trans_atomic_state_set(unit,
                                         idp_info->op_arg->trans_id,
                                         TRUE));
    }

    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_SBR_TEMPLATE;
        /* Create the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;

        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_SBR_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_idp_sbr_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            bcmfp_sbr_id_t sbr_id,
                            void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    if (idp_info->event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_sbr_insert(unit,
                                        stage_id,
                                        entry_id,
                                        sbr_id,
                                        idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_sbr_delete(unit,
                                        stage_id,
                                        entry_id,
                                        sbr_id,
                                        idp_info));
    } else if (idp_info->event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_sbr_update(unit,
                                        stage_id,
                                        entry_id,
                                        sbr_id,
                                        idp_info));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_maps_add(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_entry_config_t *ec)
{
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_id_t entry_id = 0;
    bcmfp_rule_id_t rule_id = 0;
    bcmfp_policy_id_t policy_id = 0;
    bcmfp_meter_id_t meter_id = 0;
    bcmfp_ctr_entry_id_t ctr_entry_id = 0;
    bcmfp_sbr_id_t sbr_id = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ec, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    group_id = ec->group_id;
    entry_id = ec->entry_id;
    rule_id = ec->rule_id;
    policy_id = ec->policy_id;
    meter_id = ec->meter_id;
    ctr_entry_id = ec->ctr_entry_id;
    sbr_id = ec->sbr_id;

    /* Map group associated to the entry. */
    if (group_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_entry_map_add(unit,
                                       stage_id,
                                       group_id,
                                       entry_id));
    }

    /* Map rule associated to the entry. */
    if (rule_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_rule_entry_map_add(unit,
                                      stage_id,
                                      rule_id,
                                      entry_id));
    }

    /* Map policy associated to the entry. */
    if (policy_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_entry_map_add(unit,
                                        stage_id,
                                        policy_id,
                                        entry_id));
    }

    /*
     * Map meter associated to the entry and inform
     * meter module.
     */
    if (meter_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_meter_entry_map_add(unit,
                                       stage_id,
                                       meter_id,
                                       entry_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_entry_attach(unit,
                                       stage_id,
                                       meter_id));
    }

    /* Map counter (not flex) associated to the entry. */
    if (ctr_entry_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ctr_entry_map_add(unit,
                                     stage_id,
                                     ctr_entry_id,
                                     entry_id));
    }

    /* Map SBR id associated to the entry. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        (sbr_id != 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_entry_map_add(unit,
                                     stage_id,
                                     sbr_id,
                                     entry_id));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_maps_delete(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_entry_config_t *ec)
{
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_id_t entry_id = 0;
    bcmfp_rule_id_t rule_id = 0;
    bcmfp_policy_id_t policy_id = 0;
    bcmfp_meter_id_t meter_id = 0;
    bcmfp_ctr_entry_id_t ctr_entry_id = 0;
    bcmfp_sbr_id_t sbr_id = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ec, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    group_id = ec->group_id;
    entry_id = ec->entry_id;
    rule_id = ec->rule_id;
    policy_id = ec->policy_id;
    meter_id = ec->meter_id;
    ctr_entry_id = ec->ctr_entry_id;
    sbr_id = ec->sbr_id;

    /* Unmap group associated to the entry. */
    if (group_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_entry_map_delete(unit,
                                          stage_id,
                                          group_id,
                                          entry_id));
    }

    /* Unmap rule associated to the entry. */
    if (rule_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_rule_entry_map_delete(unit,
                                         stage_id,
                                         rule_id,
                                         entry_id));
    }

    /* Unmap policy associated to the entry. */
    if (policy_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_entry_map_delete(unit,
                                           stage_id,
                                           policy_id,
                                           entry_id));
    }

    /*
     * Unmap meter associated to the entry and also inform
     * the meter module.
     */
    if (meter_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_meter_entry_map_delete(unit,
                                          stage_id,
                                          meter_id,
                                          entry_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_entry_detach(unit,
                                       stage_id,
                                       meter_id));
    }

    /* Unmap counter (not flex) associated to the entry. */
    if (ctr_entry_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ctr_entry_map_delete(unit,
                                        stage_id,
                                        ctr_entry_id,
                                        entry_id));
    }

    /* Unmap SBR id associated to the entry. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        (sbr_id != 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_entry_map_delete(unit,
                                        stage_id,
                                        sbr_id,
                                        entry_id));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_ro_fields_add(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_filter_t *filter,
                          bcmltd_fields_t *ro_fields)
{
    uint32_t entry_state_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ro_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    entry_state_fid =
        stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_field_add(unit,
                             entry_state_fid,
                             filter->entry_state,
                             ro_fields));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_config_insert(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t filter;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&filter, 0, sizeof(bcmfp_filter_t));
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));

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

    /* Get the entry configuration from key and data */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage_id,
                                key,
                                non_default_data,
                                &entry_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_config_dump(unit,
                                     tbl_id,
                                     stage_id,
                                     entry_config));
    }

    /*
     * Keep the group id for incrementing its entry
     * reference count at the end of this function.
     */
    group_id = entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    /*
     * Enable atomic transaction because there might be
     * multiple PTM opcode requests for single LT opcode
     * request. Multiple PTM requests will be triggered
     * when the first entry in the group is inserted.
     */
    if (opinfo != NULL && opinfo->ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_trans_atomic_state_set(unit,
                                          op_arg->trans_id,
                                          TRUE));
    }

    /* Insert the entry in HW. */
    sal_memset(&filter, 0, sizeof(bcmfp_filter_t));
    filter.entry_config = entry_config;
    entry_config = NULL;
    filter.group_oper_info = opinfo;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_create(unit,
                             op_arg,
                             tbl_id,
                             stage->stage_id,
                             &filter));

    /*
     * Map the resources(group, rule, policy, meter
     * and counters).
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_maps_add(unit,
                              stage_id,
                              filter.entry_config));

    /* Add read only LT fields to output_fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_ro_fields_add(unit,
                                   stage_id,
                                   &filter,
                                   output_fields));

    /* Increment the groups entry reference count. */
    if (filter.entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (entry_config != NULL) {
            SHR_FREE(entry_config);
        }
    }
    bcmfp_filter_configs_free(unit, &filter);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_config_update(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *ec_curr = NULL;
    bcmfp_entry_config_t *ec_prev = NULL;
    bcmfp_entry_id_t entry_id = 0;
    bcmfp_filter_t *filter_curr = NULL;
    bcmfp_filter_t *filter_prev = NULL;
    bcmfp_group_oper_info_t *curr_opinfo = NULL;
    bcmfp_group_oper_info_t *prev_opinfo = NULL;
    bcmfp_group_id_t group_id_prev = 0;
    bcmfp_group_id_t group_id_curr = 0;
    size_t size = 0;
    bcmltd_field_t *in_key = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_fields_t updated_fields;
    uint16_t num_fid = 0;
    const bcmltd_field_t *updated_data = NULL;
    bcmfp_entry_state_t entry_state_prev = BCMFP_ENTRY_SUCCESS;
    bcmfp_entry_state_t entry_state_curr = BCMFP_ENTRY_SUCCESS;

    SHR_FUNC_ENTER(unit);

    sal_memset(&updated_fields, 0, sizeof(bcmltd_fields_t));
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.entry_tbl->lrd_info.num_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_updated_data_get(unit,
                                    tbl_id,
                                    num_fid,
                                    in_key,
                                    in_data,
                                    &updated_fields));

    updated_data =
        (const bcmltd_field_t *)(updated_fields.field[0]);

    /* Get the entry configuration from key and data */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage_id,
                                key,
                                updated_data,
                                &ec_curr));
    /*
     * Keep the current group id for increment or decremnt
     * its entry reference count at the end of this function.
     */
    group_id_curr = ec_curr->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id_curr,
                                   &curr_opinfo));

    /* Get the entry configuration from data in IMM */
    entry_id = ec_curr->entry_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       op_arg,
                                       stage_id,
                                       entry_id,
                                       &ec_prev));
    /*
     * Keep the previous group id for increment or decremnt
     * its entry reference count at the end of this function.
     */
    group_id_prev = ec_prev->group_id;
    entry_state_prev = ec_prev->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id_prev,
                                   &prev_opinfo));

    /*
     * If group id is changed, then delete the entry from
     * old group and insert the entry in new group.
     */
    if ((ec_prev->group_id != 0) &&
        (ec_prev->group_id != ec_curr->group_id)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_config_delete(unit,
                                       op_arg,
                                       tbl_id,
                                       stage_id,
                                       key,
                                       NULL,
                                       output_fields));
    } else {
       /*
        * Unmap the old resources(group, rule, policy, meter
        * and counters).
        */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_entry_maps_delete(unit,
                                    stage_id,
                                    ec_prev));
    }
    /*
     * Map the new resources(group, rule, policy, meter
     * and counters).
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_maps_add(unit,
                              stage_id,
                              ec_curr));

    /*
     * Enable atomic transaction because there might be
     * multiple PTM opcode requests for single LT opcode
     * request. Multiple PTM requests will be triggered
     * when the first entry in the group is inserted.
     */
    if (curr_opinfo != NULL && curr_opinfo->ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_trans_atomic_state_set(unit, op_arg->trans_id, TRUE));
    }

    size = sizeof(bcmfp_filter_t);
    BCMFP_ALLOC(filter_curr, size, "bcmfpEntryUpdateCurrentFilter");
    BCMFP_ALLOC(filter_prev, size, "bcmfpEntryUpdatePreviousFilter");

    filter_curr->entry_config = ec_curr;
    filter_prev->entry_config = ec_prev;
    ec_curr = NULL;
    ec_prev = NULL;
    filter_curr->group_oper_info = curr_opinfo;
    filter_prev->group_oper_info = prev_opinfo;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_update(unit,
                             op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter_prev,
                             filter_curr));
    entry_state_curr = filter_curr->entry_state;

    /* Add read only LT fields to output_fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_ro_fields_add(unit,
                                   stage_id,
                                   filter_curr,
                                   output_fields));

    if (group_id_curr == group_id_prev) {
        /*
         * Decrement the groups entry reference count if
         * 1. No change in group id.
         * 2. Entries previous state is SUCCESS.
         * 3. Entries current state is not SUCCESS.
         */
        if (entry_state_curr != BCMFP_ENTRY_SUCCESS &&
            entry_state_prev == BCMFP_ENTRY_SUCCESS) {
            if (curr_opinfo != NULL) {
                curr_opinfo->ref_count--;
            }
        }
        /*
         * Increment the groups entry reference count if
         * 1. No change in group id.
         * 2. Entries previous state is not SUCCESS.
         * 3. Entries current state is SUCCESS.
         */
        if (entry_state_curr == BCMFP_ENTRY_SUCCESS &&
            entry_state_prev != BCMFP_ENTRY_SUCCESS) {
            if (curr_opinfo != NULL) {
                curr_opinfo->ref_count++;
            }
        }
    }

    /*
     * Update the groups entry reference count if
     * 1. group id is changed.
     * 2. Entries current state or previous state is SUCCESS.
     */
    if (group_id_curr != group_id_prev) {
        if (entry_state_curr == BCMFP_ENTRY_SUCCESS)  {
            if (curr_opinfo != NULL) {
                curr_opinfo->ref_count++;
            }
        }
        /*
         * Decrementing the ref count of the prev group is done
         * during the entry delete.
         */
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (ec_curr != NULL) {
            SHR_FREE(ec_curr);
        }
        if (ec_prev != NULL) {
            SHR_FREE(ec_prev);
        }
    }
    bcmfp_filter_configs_free(unit, filter_curr);
    bcmfp_filter_configs_free(unit, filter_prev);
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FREE(filter_curr);
    SHR_FREE(filter_prev);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t filter;
    bcmfp_entry_id_t entry_id = 0;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_entry_state_t entry_state;

    SHR_FUNC_ENTER(unit);

    sal_memset(&filter, 0, sizeof(bcmfp_filter_t));

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /*
     * Get the entry configuration. Only entry_id will be
     * populated in entry_config as data is NULL in case
     * of delete.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage_id,
                                key,
                                data,
                                &entry_config));

    /* Get the entry configuration from data in IMM */
    entry_id = entry_config->entry_id;
    bcmfp_entry_config_free(unit, entry_config);
    entry_config = NULL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    /*
     * Unmap the resources(group, rule, policy, meter
     * and counters).
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_maps_delete(unit,
                                 stage_id,
                                 entry_config));
    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    group_id = entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter.group_oper_info = opinfo;

    /*
     * Enable atomic transaction because there might be
     * multiple PTM opcode requests for single LT opcode
     * request. Multiple PTM requests will be triggered
     * when the last entry in the group is deleted.
     */
    if (opinfo != NULL && opinfo->ref_count == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_trans_atomic_state_set(unit, op_arg->trans_id, TRUE));
    }

    /* Delete the entry confiuration from the HW. */
    filter.entry_config = entry_config;
    entry_state = entry_config->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_delete(unit,
                             op_arg,
                             tbl_id,
                             stage->stage_id,
                             &filter));

    if ((entry_state == BCMFP_ENTRY_SUCCESS) &&
        (opinfo != NULL)) {
        opinfo->ref_count--;
    }
exit:
    bcmfp_filter_configs_free(unit, &filter);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_config_lookup(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          bcmimm_lookup_type_t lkup_type,
                          const bcmltd_fields_t *key,
                          bcmltd_fields_t *data)

{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_filter_t filter;
    bcmfp_entry_id_t entry_id = 0;
    bcmfp_group_id_t group_id = 0;
    bool lc_status = FALSE;
    bcmfp_entry_id_t parent_eid = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&filter,0,sizeof(bcmfp_filter_t));

    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(key, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (lkup_type == BCMIMM_LOOKUP) {
        entry_id = key->field[0]->data & 0xFFFFFFFF;
    } else if (lkup_type == BCMIMM_TRAVERSE) {
        entry_id = data->field[0]->data & 0xFFFFFFFF;
    }

    /*
     * If the entry is list compressed, entry will not be
     * installed in the hardware.
     */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_LIST_COMPRESSION)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_list_compress_status_get(unit,
                                              stage_id,
                                              entry_id,
                                              &lc_status));
        if (lc_status == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_list_compress_parent_entry_get(unit,
                                                        stage_id,
                                                        entry_id,
                                                        &parent_eid));
            entry_id = parent_eid;
        }
    }

    /* Get the entry configuration from data in IMM */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));

    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        bcmfp_entry_config_free(unit, entry_config);
        entry_config = NULL;
        SHR_EXIT();
    }

    /* Delete the entry confiuration from the HW. */
    sal_memset(&filter, 0, sizeof(bcmfp_filter_t));
    filter.entry_config = entry_config;

    group_id = entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter.group_oper_info = opinfo;
    filter.lkup_type = lkup_type;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_lookup(unit,
                             op_arg,
                             tbl_id,
                             stage->stage_id,
                             &filter,
                             data));

exit:
    bcmfp_filter_configs_free(unit, &filter);
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_idp_pse_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            bcmfp_group_id_t group_id,
                            void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    /*
     * Enable atomic transaction because there might be
     * multiple PTM opcode requests for single LT opcode
     * request. Because single presel entry can be associated
     * to multiple entries.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_atomic_state_set(unit, idp_info->op_arg->trans_id, TRUE));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");
    tbl_id = stage->tbls.entry_tbl->sid;
    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    if (idp_info->entry_state != BCMFP_ENTRY_SUCCESS) {
        if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
            /* Create the filter corresponding to entry ID. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_filter_create(unit,
                                     idp_info->op_arg,
                                     tbl_id,
                                     stage->stage_id,
                                     filter));
            entry_state = filter->entry_state;
        } else {
            /* Delete the filter corresponding to entry ID. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_filter_delete(unit,
                                     idp_info->op_arg,
                                     tbl_id,
                                     stage->stage_id,
                                     filter));
            /* Decrement the group reference count. */
            opinfo->ref_count--;

            /* Update the entry state. */
            entry_state = idp_info->entry_state;
            key_fid = stage->tbls.entry_tbl->key_fid;
            data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_imm_entry_update(unit,
                                        tbl_id,
                                        &key_fid,
                                        &entry_id,
                                        1,
                                        &data_fid,
                                        &entry_state,
                                        1));
            SHR_EXIT();
        }
    } else {
        if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
            /* Create the filter corresponding to entry ID. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_filter_create(unit,
                                     idp_info->op_arg,
                                     tbl_id,
                                     stage->stage_id,
                                     filter));
            entry_state = filter->entry_state;
        }
    }

exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();

}
