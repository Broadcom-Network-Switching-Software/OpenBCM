/*! \file bcmfp_cth_presel_entry.c
 *
 * APIs for FP pse template LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate pse config provided
 * using pse template LTs.
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
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_action_internal.h>
#include <bcmfp/bcmfp_idp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_pse_id_get(int unit,
                  bcmfp_stage_t *stage,
                  bcmltd_field_t *buffer,
                  uint32_t *pse_id)
{
    uint32_t pse_id_fid = 0;
    uint64_t pse_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(pse_id, SHR_E_PARAM);

    if (stage->tbls.pse_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    pse_id_fid = stage->tbls.pse_tbl->key_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             pse_id_fid,
                             (void *)buffer,
                             &pse_id_u64),
            SHR_E_NOT_FOUND);

    *pse_id = pse_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_pse_enable_get(int unit,
                  bcmfp_stage_t *stage,
                  bcmltd_field_t *buffer,
                  uint32_t *enable)
{
    uint32_t enable_fid = 0;
    uint64_t enable_u64 = 1;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    if (stage->tbls.pse_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    enable_fid = stage->tbls.pse_tbl->enable_fid;

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
bcmfp_pse_priority_get(int unit,
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

    if (stage->tbls.pse_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    priority_fid = stage->tbls.pse_tbl->priority_fid;

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
bcmfp_pse_qualifiers_config_get(int unit,
                              bcmltd_sid_t tbl_id,
                              bcmfp_stage_t *stage,
                              bcmltd_field_t *buffer,
                              bcmfp_qual_data_t **qc)
{
    uint32_t *data_or_mask = NULL;
    bcmfp_qualifier_t qual_id = 0;
    bcmfp_qual_data_t *qual_config = NULL;
    bcmfp_qual_data_t *temp_config = NULL;
    const bcmlrd_field_data_t *field = NULL;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    int qual_enum = 0;
    uint16_t bit_offset = 0;
    uint16_t curr_offset = 0;
    uint16_t match_id_len = 0;
    uint32_t bit_mask = 1;
    const bcmlrd_match_id_db_t *qual_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(qc, SHR_E_PARAM);

    if (stage->tbls.pse_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    qualifiers_fid_info =
        stage->tbls.pse_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *qc = NULL;
    while (buffer != NULL) {
        if (qualifiers_fid_info[buffer->id].qualifier == 0) {
            buffer = buffer->next;
            continue;
        }
        /* Continue if the data is if type action */
        if (SHR_BITGET(qualifiers_fid_info[buffer->id].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_ACTION))  {
            buffer = buffer->next;
            continue;
        }
        qual_id = qualifiers_fid_info[buffer->id].qualifier + buffer->idx;
        qual_config = *qc;
        while (qual_config != NULL) {
            if (qual_config->qual_id == qual_id) {
                break;
            }
            qual_config = qual_config->next;
        }
        if (qual_config == NULL) {
            BCMFP_ALLOC(qual_config,
                    sizeof(bcmfp_qual_data_t),
                    "bcmfpPreselEntryQualifierConfig");
            /*
             * Add the new node to the linked list of
             * qualifier configurations.
             */
            qual_config->next = *qc;
            *qc = qual_config;

            /* Update the qualifier. */
            qual_config->qual_id = qual_id;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_get(unit, tbl_id, buffer->id, &field));

        if (field->edata) {
            qual_enum = buffer->data & 0xFFFFFFFF;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_qualifier_enum_set(unit,
                                          stage->stage_id,
                                          qual_id,
                                          TRUE,
                                          qual_enum,
                                          qual_config->key,
                                          qual_config->mask));
        } else if (SHR_BITGET(qualifiers_fid_info[buffer->id].flags.w,
                              BCMFP_QUALIFIER_FID_FLAGS_MATCH_ID)) {

            /* Compute data and mask if it is a match ID qualifier */
            if (SHR_BITGET(qualifiers_fid_info[buffer->id].flags.w,
                           BCMFP_QUALIFIER_FID_FLAGS_DATA)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_table_match_id_db_get(unit,
                                              stage->tbls.pse_tbl->sid,
                                              buffer->id,
                                              &qual_info));
                bit_offset = 0;
                if (buffer->data != 0) {
                    match_id_len = qual_info->pmaxbit - qual_info->pminbit + 1;
                    for (curr_offset = 0; curr_offset < match_id_len;
                        curr_offset++) {
                        /* coverity[copy_paste_error: FALSE] */
                        if ((qual_info->match_mask &
                            (bit_mask << curr_offset)) != 0) {
                            SHR_BITCOPY_RANGE(qual_config->key,
                                              bit_offset,
                                              &(qual_info->match),
                                              curr_offset,
                                              1);
                            bit_offset++;
                        }
                    }
                }
            } else {
                /* For mask type match ID qualifier */

                /*
                * Match ID db get function return qual_info
                * for only match ID data fid, not mask fid.
                * so decrement fid by 1, to get the data field fid.
                */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_table_match_id_db_get(unit,
                                              stage->tbls.pse_tbl->sid,
                                              (buffer->id) - 1,
                                              &qual_info));
                bit_offset = 0;
                if (buffer->data != 0) {
                    match_id_len = qual_info->pmaxbit - qual_info->pminbit + 1;
                    for (curr_offset = 0; curr_offset < match_id_len;
                        curr_offset++) {
                        if ((qual_info->match_mask &
                            (bit_mask << curr_offset)) != 0) {
                            SHR_BITCOPY_RANGE(qual_config->mask,
                                              bit_offset,
                                              &(qual_info->match_mask),
                                              curr_offset,
                                              1);
                            bit_offset++;
                        }
                    }
                }
            }
        } else {
            /* Get the qualifier data/mask */
            if (SHR_BITGET(qualifiers_fid_info[buffer->id].flags.w,
                           BCMFP_QUALIFIER_FID_FLAGS_DATA)) {
                data_or_mask = qual_config->key;
            } else {
                data_or_mask = qual_config->mask;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_tbl_field_value_get(unit,
                                           buffer->data,
                                           field->width,
                                           buffer->idx,
                                           data_or_mask));
        }
        qual_config->fid = qualifiers_fid_info[buffer->id].qualifier;
        qual_config->fidx = buffer->idx;
        buffer = buffer->next;
    }

exit:
    if (SHR_FUNC_ERR()) {
        while (qual_config != NULL) {
            temp_config = qual_config;
            qual_config = qual_config->next;
            SHR_FREE(temp_config);
        }
        *qc = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_pse_actions_config_get(int unit,
                             bcmfp_stage_t *stage,
                             bcmltd_field_t *buffer,
                             bcmfp_action_data_t **ad)
{
    bcmfp_action_data_t *action_data = NULL;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ad, SHR_E_PARAM);

    if (stage->tbls.policy_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    qualifiers_fid_info =
        stage->tbls.pse_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    while (buffer != NULL) {
        if (qualifiers_fid_info[buffer->id].qualifier == 0) {
            buffer = buffer->next;
            continue;
        }
        if (SHR_BITGET(qualifiers_fid_info[buffer->id].flags.w,
                    BCMFP_QUALIFIER_FID_FLAGS_ACTION))  {
            BCMFP_ALLOC(action_data,
                    sizeof(bcmfp_action_data_t),
                    "bcmfpPreselEntryActionData");
            /*
             * Add the new node to the linked list of
             * action data.
             */
            action_data->next = *ad;
            *ad = action_data;

            /* Update the action. */
            action_data->action =
                qualifiers_fid_info[buffer->id].qualifier;

            /* Update flags. */
            if (SHR_BITGET(qualifiers_fid_info[buffer->id].flags.w,
                           BCMFP_QUALIFIER_FID_FLAGS_ALL_PARTS)) {
                action_data->flags = BCMFP_ACTION_FLAGS_ALL_PARTS;
            }

            /* Update the action parameter */
            action_data->param = buffer->data & 0xFFFFFFFF;
            action_data = NULL;
        }
        buffer = buffer->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_pse_qualifier_fid_get(int unit,
                              bcmfp_stage_t *stage,
                              bcmfp_qualifier_t qualifier,
                              bcmlrd_fid_t *key_fid,
                              bcmlrd_fid_t *mask_fid)
{
    uint16_t idx = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(mask_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    qualifiers_fid_info =
        stage->tbls.group_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *key_fid = 0;
    *mask_fid = 0;
    for (idx = 0; idx < stage->tbls.group_tbl->fid_count; idx++) {
        if (qualifiers_fid_info[idx].qualifier == qualifier &&
            SHR_BITGET(qualifiers_fid_info[idx].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_DATA)) {
            *key_fid = idx;
        }
        if (qualifiers_fid_info[idx].qualifier == qualifier &&
            SHR_BITGET(qualifiers_fid_info[idx].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_MASK)) {
            *mask_fid = idx;
        }
        if (*key_fid != 0 && *mask_fid != 0) {
            break;
        }
    }

    if (idx == stage->tbls.group_tbl->fid_count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_pse_config_dump(int unit,
                  bcmltd_sid_t tbl_id,
                  bcmfp_stage_t *stage,
                  bcmfp_pse_config_t *pse_config)
{
    bcmfp_qual_data_t *qual_data = NULL;
    bcmlrd_field_def_t field_def;
    bcmlrd_fid_t key_fid = 0;
    bcmlrd_fid_t mask_fid = 0;
    bcmfp_qualifier_t qualifier;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(pse_config, SHR_E_PARAM);

    if (stage->tbls.pse_tbl->key_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Presel entry ID = %d\n"),
            pse_config->pse_id));
    }

    qual_data = pse_config->qual_data;
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "Qualifiers Data(KEY, MASK):\n")));
    while (qual_data != NULL) {

        qualifier = qual_data->qual_id;

        /* Get the key and mask field corresponding to the qualifier. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pse_qualifier_fid_get(unit,
                                      stage,
                                      qualifier,
                                      &key_fid,
                                      &mask_fid));

        /* Dump the key portion of the qualifier. */
        sal_memset(&field_def, 0, sizeof(bcmlrd_field_def_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        key_fid,
                                        &field_def));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "    %s="),
                   field_def.name));
        for (idx = (field_def.width / 32); idx >= 0; idx--) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "0x%x_"),
                       qual_data->key[idx]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

        /* Dump the mask portion of the qualifier. */
        sal_memset(&field_def, 0, sizeof(bcmlrd_field_def_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        mask_fid,
                                        &field_def));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "    %s="),
                   field_def.name));
        for (idx = (field_def.width / 32); idx >= 0; idx--) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "0x%x_"),
                       qual_data->mask[idx]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

        qual_data = qual_data->next;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_config_get(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     bcmltd_sid_t tbl_id,
                     bcmfp_stage_id_t stage_id,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     bcmfp_pse_config_t **config)
{
    uint32_t pse_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    bcmfp_pse_config_t *pse_config = NULL;
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_action_data_t *action_data = NULL;
    uint32_t enable = 1;
    uint32_t priority = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.pse_tbl == NULL ||
        stage->tbls.pse_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(pse_config,
                sizeof(bcmfp_pse_config_t),
                "bcmfpPreselEntryConfig");
    *config = pse_config;

    /* Get the presel entry id */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_id_get(unit,
                           stage,
                           buffer,
                           &pse_id));
    pse_config->pse_id = pse_id;

    /* Get the presel entry enable */
    buffer = (bcmltd_field_t *)data;
    if (buffer != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pse_enable_get(unit,
                                  stage,
                                  buffer,
                                  &enable));
    }
    pse_config->enable = enable;

    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the presel entry priority */
    buffer = (bcmltd_field_t *)data;
    if (stage->tbls.pse_tbl->priority_fid != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pse_priority_get(unit,
                                    stage,
                                    buffer,
                                    &priority));
    }
    pse_config->priority = priority;


    /* Get the qualifiers config */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_actions_config_get(unit,
                                      stage,
                                      buffer,
                                      &action_data));
    pse_config->action_data = action_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_qualifiers_config_get(unit,
                                         tbl_id,
                                         stage,
                                         buffer,
                                         &qual_data));
    pse_config->qual_data = qual_data;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_remote_config_get(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pse_id_t pse_id,
                          bcmfp_pse_config_t **config)
{
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_pse_config_t *pse_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.pse_tbl->key_fid;
    tbl_id = stage->tbls.pse_tbl->sid;
    num_fid = stage->tbls.pse_tbl->lrd_info.num_fid;

    /* Get the FP pse LT entry saved in IMM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &pse_id,
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

    /* Get the FP pse LT entry config of ACL. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_config_get(unit,
                           op_arg,
                           tbl_id,
                           stage->stage_id,
                           key,
                           data,
                           &pse_config));
    *config = pse_config;
    pse_config = NULL;

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_pse_config_free(unit, pse_config);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_config_free(int unit,
             bcmfp_pse_config_t *pse_config)
{
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_action_data_t *action_data = NULL;

    SHR_FUNC_ENTER(unit);

    if (pse_config == NULL) {
        SHR_EXIT();
    }

    qual_data = pse_config->qual_data;
    while (qual_data != NULL) {
        pse_config->qual_data = qual_data->next;
        SHR_FREE(qual_data);
        qual_data = pse_config->qual_data;
    }

    action_data = pse_config->action_data;
    while (action_data != NULL) {
        pse_config->action_data = action_data->next;
        SHR_FREE(action_data);
        action_data = pse_config->action_data;
    }

    SHR_FREE(pse_config);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_pse_config_t *pse_config = NULL;
    bcmfp_idp_info_t group_idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_pse_id_t pse_id = 0;
    bool not_mapped = FALSE;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;

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

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              non_default_data,
                              &pse_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pse_config_dump(unit,
                                   tbl_id,
                                   stage,
                                   pse_config));
    }

    /* Process group associated to this pse. */
    pse_id = pse_config->pse_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_check(unit,
                                   stage_id,
                                   pse_id,
                                   &not_mapped));

    /* Return if no groups are associated to the pse */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
    group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    group_idp_info.sid_type = BCMFP_SID_TYPE_PSE_TEMPLATE;
    group_idp_info.key = key;
    group_idp_info.data = data;
    group_idp_info.output_fields = output_fields;
    group_idp_info.event_reason = BCMIMM_ENTRY_INSERT;
    group_idp_info.entry_state = state;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_traverse(unit,
                                      stage_id,
                                      pse_id,
                                      bcmfp_group_idp_process,
                                      (void *)(&group_idp_info)));
exit:
    bcmfp_pse_config_free(unit, pse_config);
    SHR_FUNC_EXIT();
}

static int
bcmfp_pse_config_lookup_from_entry_map(int unit,
                                       bcmfp_stage_id_t stage_id,
                                       bcmfp_entry_id_t entry_id,
                                       bcmfp_group_id_t group_id,
                                       void *user_data,
                                       bool *cond_met)
{
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_idp_info_t *idp_info = user_data;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_filter_t filter;
    bcmfp_pse_config_t *pse_config = NULL;
    bcmfp_pse_id_t pse_id = 0;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&filter, 0, sizeof(bcmfp_filter_t));

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    sal_memset(&filter, 0, sizeof(bcmfp_filter_t));

    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));

    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        bcmfp_entry_config_free(unit, entry_config);
        entry_config = NULL;
        SHR_EXIT();
    }

    /* if the entry is succesfully installed lookup the entry in HW */
    filter.entry_config = entry_config;
    size = sizeof(bcmfp_pse_config_t *);
    BCMFP_ALLOC(filter.pse_config,
                size,
                "bcmfpFilterPreselEntryConfigArray");

    group_id = entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter.group_oper_info = opinfo;

    pse_id = idp_info->template_id;
    SHR_IF_ERR_EXIT
        (bcmfp_pse_remote_config_get(unit,
                                     idp_info->op_arg,
                                     stage_id,
                                     pse_id,
                                     &pse_config));
    filter.pse_config[0] = pse_config;
    filter.num_pse_configs = 1;
    pse_config = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_pse_lookup(unit,
                                 idp_info->op_arg,
                                 idp_info->tbl_id,
                                 stage_id,
                                 &filter,
                                 (bcmltd_fields_t *)idp_info->data));
    *cond_met = TRUE;
exit:
    bcmfp_filter_configs_free(unit, &filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_pse_config_lookup_from_group_map(int unit,
                                       bcmfp_stage_id_t stage_id,
                                       bcmfp_group_id_t group_id,
                                       uint32_t template_id,
                                       void *user_data,
                                       bool *cond_met)
{
    bcmfp_idp_info_t *idp_info = NULL;
    bool not_mapped = FALSE;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_entry_id_cond_traverse_cb cb_func;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    idp_info = (bcmfp_idp_info_t *)user_data;

    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_check(unit,
                                     stage_id,
                                     group_id,
                                     &not_mapped));

    /* Return if no entries are associated to the group */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    /* return if the group mapped to the pse_id is not operational */
    if (opinfo->state != BCMFP_GROUP_STATE_SUCCESS) {
        SHR_EXIT();
    }

    idp_info->sid_type = BCMFP_SID_TYPE_GRP_TEMPLATE;
    idp_info->event_reason = BCMIMM_ENTRY_LOOKUP;

    user_data = (void *)idp_info;
    cb_func = bcmfp_pse_config_lookup_from_entry_map;
    /* extract the entry ids mapped to this group */
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_cond_traverse(unit,
                                             stage_id,
                                             group_id,
                                             cb_func,
                                             user_data,
                                             cond_met));
exit:
    SHR_FUNC_EXIT();

}

int
bcmfp_pse_config_lookup(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        bcmimm_lookup_type_t lkup_type,
                        const bcmltd_fields_t *key,
                        bcmltd_fields_t *data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_filter_t filter;
    bcmfp_pse_id_t pse_id = 0;
    bcmfp_idp_info_t group_idp_info;
    bool not_mapped = FALSE;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bool cond_met = FALSE;
    bcmfp_entry_id_cond_traverse_cb cb_func;

    SHR_FUNC_ENTER(unit);

    sal_memset(&filter,0,sizeof(bcmfp_filter_t));

    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_NULL_CHECK(key, SHR_E_PARAM);
    }
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (lkup_type == BCMIMM_LOOKUP) {
        pse_id = key->field[0]->data & 0xFFFFFFFF;
    } else if (lkup_type == BCMIMM_TRAVERSE) {
        pse_id = data->field[0]->data & 0xFFFFFFFF;
    }

    /*
     * check if any entry mapped to pse_id is successfully installed
     * else presel entry will not be installed in HW
     */
    /* Process group associated to this pse. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_check(unit,
                                   stage_id,
                                   pse_id,
                                   &not_mapped));

    /* Return if no groups are associated to the pse */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }
    sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
    group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    group_idp_info.sid_type = BCMFP_SID_TYPE_PSE_TEMPLATE;
    group_idp_info.key = (bcmltd_field_t *)key;
    group_idp_info.data = (bcmltd_field_t *)data;
    group_idp_info.event_reason = BCMIMM_ENTRY_LOOKUP;
    group_idp_info.entry_state = state;
    group_idp_info.tbl_id = tbl_id;
    /*
     * the pse_id stored in template_id will be used by bcmfp_pse_config_lookup_from_entry_map
     * for extracting the pse_id to lookup
     */
    group_idp_info.template_id = pse_id;
    cb_func = bcmfp_pse_config_lookup_from_group_map;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_cond_traverse(unit,
                                           stage_id,
                                           pse_id,
                                           cb_func,
                                           (void *)(&group_idp_info),
                                           &cond_met));
exit:
    SHR_FUNC_EXIT();

}


int
bcmfp_pse_config_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_pse_config_t *pse_config = NULL;
    bcmfp_idp_info_t group_idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_pse_id_t pse_id = 0;
    bool not_mapped = FALSE;
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

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.pse_tbl->lrd_info.num_fid;
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
        (bcmfp_pse_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              updated_data,
                              &pse_config));

    /* Process group associated to this pse. */
    pse_id = pse_config->pse_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_check(unit,
                                   stage_id,
                                   pse_id,
                                   &not_mapped));

    /* Return if no groups are associated to the pse */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
    group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    group_idp_info.sid_type = BCMFP_SID_TYPE_PSE_TEMPLATE;
    group_idp_info.key = key;
    group_idp_info.data = updated_data;
    group_idp_info.output_fields = output_fields;
    group_idp_info.event_reason = BCMIMM_ENTRY_UPDATE;
    group_idp_info.entry_state = state;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_traverse(unit,
                                      stage_id,
                                      pse_id,
                                      bcmfp_group_idp_process,
                                      (void *)(&group_idp_info)));

exit:
    bcmfp_pse_config_free(unit, pse_config);
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_pse_config_t *pse_config = NULL;
    bcmfp_idp_info_t group_idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_pse_id_t pse_id = 0;
    bool not_mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              data,
                              &pse_config));

    /* Process group associated to this pse. */
    pse_id = pse_config->pse_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_check(unit,
                                   stage_id,
                                   pse_id,
                                   &not_mapped));

    /* Return if no groups are associated to the pse */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
    group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    group_idp_info.sid_type = BCMFP_SID_TYPE_PSE_TEMPLATE;
    group_idp_info.key = key;
    group_idp_info.data = data;
    group_idp_info.output_fields = output_fields;
    group_idp_info.event_reason = BCMIMM_ENTRY_DELETE;
    group_idp_info.entry_state = state;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_traverse(unit,
                                      stage_id,
                                      pse_id,
                                      bcmfp_group_idp_process,
                                      (void *)(&group_idp_info)));
exit:
    bcmfp_pse_config_free(unit, pse_config);
    SHR_FUNC_EXIT();
}
